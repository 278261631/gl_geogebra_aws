#include "UI/LabelDataBrowser.h"

#include <imgui.h>

#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

namespace fs = std::filesystem;

static std::string ReadTextFilePreview(const fs::path& filePath, size_t maxLines = 80, size_t maxBytes = 64 * 1024) {
    std::ifstream in(filePath, std::ios::in);
    if (!in.is_open()) {
        return "(无法打开文件)";
    }

    std::ostringstream oss;
    std::string line;
    size_t lines = 0;
    size_t bytes = 0;

    while (std::getline(in, line)) {
        oss << line << "\n";
        lines++;
        bytes += line.size() + 1;
        if (lines >= maxLines || bytes >= maxBytes) {
            oss << "...\n";
            break;
        }
    }

    return oss.str();
}

LabelDataBrowser::LabelDataBrowser()
    : m_IsOpen(true)
    , m_RootPath("test-label-data")
    , m_ResolvedRootPath("")
    , m_SelectedPath("")
    , m_PreviewText("")
    , m_SelectedSize(0)
    , m_SelectedIsFile(false) {
    ResolveRootPath();
}

LabelDataBrowser::~LabelDataBrowser() = default;

void LabelDataBrowser::SetRootPath(const std::string& path) {
    m_RootPath = path;
    ResolveRootPath();
}

void LabelDataBrowser::ResolveRootPath() {
    try {
        fs::path p(m_RootPath);
        if (fs::exists(p)) {
            m_ResolvedRootPath = fs::absolute(p).string();
        } else {
            // Fallback to current working directory; UI 会提示路径不存在
            m_ResolvedRootPath = fs::absolute(fs::current_path() / p).string();
        }
    } catch (...) {
        m_ResolvedRootPath = m_RootPath;
    }
}

void LabelDataBrowser::Render() {
    if (!m_IsOpen) return;

    ImGui::SetNextWindowSize(ImVec2(720, 520), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Label Data Browser", &m_IsOpen)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Root: %s", m_RootPath.c_str());
    ImGui::Text("Resolved: %s", m_ResolvedRootPath.c_str());

    const bool rootExists = fs::exists(m_ResolvedRootPath) && fs::is_directory(m_ResolvedRootPath);
    if (!rootExists) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        ImGui::Text("目录不存在：请确认已复制到运行目录（build/bin/<Config>/test-label-data）");
        ImGui::PopStyleColor();
    }

    ImGui::Separator();

    if (ImGui::Button("Refresh")) {
        // 目前是即时遍历，无缓存；Refresh 用于重新解析路径
        ResolveRootPath();
    }
    ImGui::SameLine();
    if (ImGui::Button("Default Root")) {
        SetRootPath("test-label-data");
    }

    ImGui::Separator();

    // 左：目录树 右：选中项信息
    ImGui::Columns(2, "LabelDataCols");

    ImGui::BeginChild("LabelDataTree", ImVec2(0, 0), true);
    if (rootExists) {
        ImGui::TextUnformatted("Directory Tree");
        ImGui::Separator();
        RenderDirectoryTree(fs::path(m_ResolvedRootPath));
    }
    ImGui::EndChild();

    ImGui::NextColumn();

    ImGui::BeginChild("LabelDataDetails", ImVec2(0, 0), true);
    ImGui::TextUnformatted("Selection");
    ImGui::Separator();

    if (m_SelectedPath.empty()) {
        ImGui::TextUnformatted("(未选择文件)");
    } else {
        ImGui::Text("Path: %s", m_SelectedPath.c_str());
        if (m_SelectedIsFile) {
            ImGui::Text("Size: %llu bytes", static_cast<unsigned long long>(m_SelectedSize));
            ImGui::Separator();
            ImGui::TextUnformatted("Preview");
            ImGui::Separator();
            ImGui::BeginChild("LabelDataPreview", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::TextUnformatted(m_PreviewText.c_str());
            ImGui::EndChild();
        } else {
            ImGui::TextUnformatted("(目录)");
        }
    }

    ImGui::EndChild();

    ImGui::Columns(1);

    ImGui::End();
}

void LabelDataBrowser::RenderDirectoryTree(const fs::path& dir) {
    std::vector<fs::directory_entry> entries;
    try {
        for (const auto& entry : fs::directory_iterator(dir)) {
            entries.push_back(entry);
        }
    } catch (...) {
        ImGui::TextUnformatted("(无法读取目录)");
        return;
    }

    std::sort(entries.begin(), entries.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
        const bool ad = a.is_directory();
        const bool bd = b.is_directory();
        if (ad != bd) return ad > bd;
        return a.path().filename().string() < b.path().filename().string();
    });

    for (const auto& entry : entries) {
        const fs::path p = entry.path();
        const std::string name = p.filename().string();
        const bool isDir = entry.is_directory();

        ImGui::PushID(p.string().c_str());

        if (isDir) {
            const bool isSelected = (!m_SelectedPath.empty() && fs::path(m_SelectedPath) == p);
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
            if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;

            bool open = ImGui::TreeNodeEx((name + "/").c_str(), flags);
            if (ImGui::IsItemClicked()) {
                m_SelectedPath = p.string();
                m_SelectedIsFile = false;
                m_SelectedSize = 0;
                m_PreviewText.clear();
            }
            if (open) {
                RenderDirectoryTree(p);
                ImGui::TreePop();
            }
        } else {
            const bool isSelected = (!m_SelectedPath.empty() && fs::path(m_SelectedPath) == p);
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen |
                                       ImGuiTreeNodeFlags_SpanFullWidth;
            if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;

            ImGui::TreeNodeEx(name.c_str(), flags);
            if (ImGui::IsItemClicked()) {
                m_SelectedPath = p.string();
                m_SelectedIsFile = true;
                try {
                    m_SelectedSize = fs::file_size(p);
                } catch (...) {
                    m_SelectedSize = 0;
                }
                m_PreviewText = ReadTextFilePreview(p);
            }
        }

        ImGui::PopID();
    }
}

