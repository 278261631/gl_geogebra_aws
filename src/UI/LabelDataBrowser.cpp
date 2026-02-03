#include "UI/LabelDataBrowser.h"

#include <imgui.h>

#include <algorithm>
#include <fstream>
#include <sstream>
#include <utility>
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
    , m_SelectedIsFile(false)
    , m_HasNewFitsPair(false)
    , m_NewAlignedFitsPath("")
    , m_NewTemplateFitsPath("")
    , m_NewFitsSourceTxtPath("")
    , m_LastParseMessage("")
    , m_HasPixelCenter(false)
    , m_PixelX(0)
    , m_PixelY(0)
    , m_HasActivePixelCenter(false)
    , m_ActivePixelX(0)
    , m_ActivePixelY(0)
    , m_TxtTargets()
    , m_SelectedTxtTargetIndex(-1)
    , m_RoiEnabled(true)
    , m_RoiRadius(200)
    , m_HighlightSizePixels(10)
    , m_HighlightPointSizeScale(4.0f)
    , m_RequestCenterCameraOnRoi(false) {
    ResolveRootPath();
}

LabelDataBrowser::~LabelDataBrowser() = default;

void LabelDataBrowser::SetRootPath(const std::string& path) {
    m_RootPath = path;
    ResolveRootPath();
    m_DirectoryCache.clear();
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
    // Root resolution may change the absolute path; clear cached directory entries.
    m_DirectoryCache.clear();
}

const std::vector<LabelDataBrowser::CachedEntry>& LabelDataBrowser::GetDirectoryEntriesCached(const fs::path& dir) {
    const std::string key = dir.string();
    auto it = m_DirectoryCache.find(key);
    if (it != m_DirectoryCache.end()) {
        return it->second;
    }

    std::vector<CachedEntry> entries;
    try {
        for (const auto& entry : fs::directory_iterator(dir)) {
            CachedEntry e;
            e.path = entry.path();
            e.name = e.path.filename().string();
            e.isDirectory = entry.is_directory();
            if (!e.isDirectory) {
                try {
                    e.size = fs::file_size(e.path);
                } catch (...) {
                    e.size = 0;
                }
            } else {
                e.size = 0;
            }
            entries.push_back(std::move(e));
        }
    } catch (...) {
        // Cache empty list on error so we don't retry every frame.
    }

    std::sort(entries.begin(), entries.end(), [](const CachedEntry& a, const CachedEntry& b) {
        if (a.isDirectory != b.isDirectory) return a.isDirectory > b.isDirectory;
        return a.name < b.name;
    });

    auto [insIt, _] = m_DirectoryCache.emplace(key, std::move(entries));
    return insIt->second;
}

static bool ParseAllTargetsFromTxt(const fs::path& txtPath,
                                  std::vector<LabelDataBrowser::TxtTargetRecord>& outTargets,
                                  std::string& outError) {
    outTargets.clear();
    outError.clear();

    std::ifstream in(txtPath, std::ios::in);
    if (!in.is_open()) {
        outError = "无法打开 txt";
        return false;
    }

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (!line.empty() && line[0] == '#') continue;

        // Expected (whitespace-separated):
        // index file_dir aligned_filename template_aligned_filename fits_center_ra fits_center_dec time pixel_x pixel_y ra dec
        std::istringstream iss(line);
        LabelDataBrowser::TxtTargetRecord rec;
        if (!(iss >> rec.index >> rec.fileDir >> rec.alignedFilename >> rec.templateAlignedFilename)) {
            continue;
        }

        // Optional fields
        double fits_center_ra = 0.0;
        double fits_center_dec = 0.0;
        std::string timeStr;
        int px = 0, py = 0;
        double ra = 0.0, dec = 0.0;
        if ((iss >> fits_center_ra >> fits_center_dec >> timeStr >> px >> py)) {
            rec.hasPixelCenter = true;
            rec.pixelX = px;
            rec.pixelY = py;
            if ((iss >> ra >> dec)) {
                rec.hasRaDec = true;
                rec.ra = ra;
                rec.dec = dec;
            }
        }

        outTargets.push_back(std::move(rec));
    }

    if (outTargets.empty()) {
        outError = "txt 中未找到数据行";
        return false;
    }
    return true;
}

static bool TryFindFileByNameUnder(const fs::path& root, const std::string& filename, fs::path& outPath) {
    try {
        if (!fs::exists(root) || !fs::is_directory(root)) return false;
        for (const auto& entry : fs::recursive_directory_iterator(root, fs::directory_options::skip_permission_denied)) {
            if (!entry.is_regular_file()) continue;
            if (entry.path().filename().string() == filename) {
                outPath = entry.path();
                return true;
            }
        }
    } catch (...) {
        return false;
    }
    return false;
}

static fs::path ResolveMaybeMissingPath(const fs::path& candidate, const std::vector<fs::path>& searchRoots) {
    if (!candidate.empty() && fs::exists(candidate)) return candidate;

    const std::string filename = candidate.filename().string();
    if (filename.empty()) return candidate;

    for (const auto& root : searchRoots) {
        fs::path found;
        if (TryFindFileByNameUnder(root, filename, found)) return found;
    }

    return candidate; // fallback (may not exist)
}

void LabelDataBrowser::TryParseFitsPairFromTxtSelection(const fs::path& txtPath) {
    m_HasNewFitsPair = false;
    m_NewAlignedFitsPath.clear();
    m_NewTemplateFitsPath.clear();
    m_NewFitsSourceTxtPath.clear();
    m_LastParseMessage.clear();
    m_HasPixelCenter = false;
    m_PixelX = 0;
    m_PixelY = 0;
    m_HasActivePixelCenter = false;
    m_ActivePixelX = 0;
    m_ActivePixelY = 0;
    m_TxtTargets.clear();
    m_SelectedTxtTargetIndex = -1;

    std::string err;
    if (!ParseAllTargetsFromTxt(txtPath, m_TxtTargets, err)) {
        m_LastParseMessage = "解析失败: " + err;
        return;
    }

    m_NewFitsSourceTxtPath = txtPath.string();
    // Default to first target record
    SelectTxtTargetIndex(0, /*triggerReload*/ true);
}

void LabelDataBrowser::SetActivePixelCenter(int pixelX, int pixelY) {
    m_HasActivePixelCenter = true;
    m_ActivePixelX = pixelX;
    m_ActivePixelY = pixelY;

    // Changing center should re-apply ROI/highlight and recenter camera.
    m_RequestCenterCameraOnRoi = true;
    if (!m_NewAlignedFitsPath.empty() || !m_NewTemplateFitsPath.empty()) {
        m_HasNewFitsPair = true;
    }
}

void LabelDataBrowser::SelectTxtTargetIndex(int idx, bool triggerReload) {
    if (idx < 0 || idx >= static_cast<int>(m_TxtTargets.size())) return;
    m_SelectedTxtTargetIndex = idx;

    const auto& rec = m_TxtTargets[idx];

    // Resolve file paths for this record
    fs::path alignedCandidate;
    fs::path templateCandidate;

    try {
        fs::path fileDirPath(rec.fileDir);
        fs::path alignedPath(rec.alignedFilename);
        fs::path templatePath(rec.templateAlignedFilename);

        alignedCandidate = alignedPath.is_absolute() ? alignedPath : (fileDirPath / alignedPath);
        templateCandidate = templatePath.is_absolute() ? templatePath : (fileDirPath / templatePath);
    } catch (...) {
        m_LastParseMessage = "解析成功，但路径拼接失败";
        return;
    }

    // Try to resolve within common local roots (useful if txt contains remote drive paths like E:\\...)
    const fs::path cwd = fs::current_path();
    std::vector<fs::path> roots = {
        cwd,
        cwd / "test-img",
        cwd / "test-label-data",
        cwd / ".." / ".." / ".." // if running from build/bin/<Config>
    };

    alignedCandidate = ResolveMaybeMissingPath(alignedCandidate, roots);
    templateCandidate = ResolveMaybeMissingPath(templateCandidate, roots);

    m_NewAlignedFitsPath = alignedCandidate.string();
    m_NewTemplateFitsPath = templateCandidate.string();

    m_HasPixelCenter = rec.hasPixelCenter;
    m_PixelX = rec.pixelX;
    m_PixelY = rec.pixelY;

    // Default active center to record center
    m_HasActivePixelCenter = rec.hasPixelCenter;
    m_ActivePixelX = rec.pixelX;
    m_ActivePixelY = rec.pixelY;

    // Clamp UI parameters
    if (m_RoiRadius < 50) m_RoiRadius = 50;
    if (m_RoiRadius > 500) m_RoiRadius = 500;
    if (m_HighlightSizePixels < 1) m_HighlightSizePixels = 1;
    if (m_HighlightSizePixels > 300) m_HighlightSizePixels = 300;
    if (m_HighlightPointSizeScale < 1.0f) m_HighlightPointSizeScale = 1.0f;
    if (m_HighlightPointSizeScale > 20.0f) m_HighlightPointSizeScale = 20.0f;
    if (!m_HasPixelCenter) {
        m_RoiEnabled = false;
    }

    std::ostringstream oss;
    oss << "OK: targets=" << m_TxtTargets.size()
        << ", selected=" << rec.index
        << ", pixel=(" << rec.pixelX << "," << rec.pixelY << ")";
    if (rec.hasRaDec) {
        oss << ", ra/dec=(" << rec.ra << "," << rec.dec << ")";
    }
    m_LastParseMessage = oss.str();

    if (triggerReload) {
        m_HasNewFitsPair = true;
        m_RequestCenterCameraOnRoi = true;
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
        m_DirectoryCache.clear();
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
            if (!m_LastParseMessage.empty()) {
                ImGui::Separator();
                ImGui::Text("Parse: %s", m_LastParseMessage.c_str());
                if (!m_NewAlignedFitsPath.empty() || !m_NewTemplateFitsPath.empty()) {
                    ImGui::Text("Aligned FITS: %s", m_NewAlignedFitsPath.c_str());
                    ImGui::Text("Template FITS: %s", m_NewTemplateFitsPath.c_str());
                }
            }

            // If this is a txt with multiple targets, show a picker list.
            if (fs::path(m_SelectedPath).extension().string() == ".txt" && !m_TxtTargets.empty()) {
                ImGui::Separator();
                ImGui::Text("Targets in txt: %d", static_cast<int>(m_TxtTargets.size()));

                ImGui::BeginChild("TxtTargets", ImVec2(0, 160), true);
                ImGui::Columns(5, "TxtTargetCols");
                ImGui::TextUnformatted("idx"); ImGui::NextColumn();
                ImGui::TextUnformatted("pixel_x"); ImGui::NextColumn();
                ImGui::TextUnformatted("pixel_y"); ImGui::NextColumn();
                ImGui::TextUnformatted("ra"); ImGui::NextColumn();
                ImGui::TextUnformatted("dec"); ImGui::NextColumn();
                ImGui::Separator();

                for (int i = 0; i < static_cast<int>(m_TxtTargets.size()); i++) {
                    const auto& rec = m_TxtTargets[i];
                    const bool selected = (i == m_SelectedTxtTargetIndex);
                    ImGui::PushID(i);

                    if (ImGui::Selectable(rec.index.c_str(), selected, ImGuiSelectableFlags_SpanAllColumns)) {
                        SelectTxtTargetIndex(i, /*triggerReload*/ true);
                    }
                    ImGui::NextColumn();

                    ImGui::Text("%d", rec.pixelX); ImGui::NextColumn();
                    ImGui::Text("%d", rec.pixelY); ImGui::NextColumn();
                    if (rec.hasRaDec) {
                        ImGui::Text("%.6f", rec.ra);
                    } else {
                        ImGui::TextUnformatted("-");
                    }
                    ImGui::NextColumn();
                    if (rec.hasRaDec) {
                        ImGui::Text("%.6f", rec.dec);
                    } else {
                        ImGui::TextUnformatted("-");
                    }
                    ImGui::NextColumn();

                    ImGui::PopID();
                }

                ImGui::Columns(1);
                ImGui::EndChild();

                if (m_HasActivePixelCenter) {
                    ImGui::Text("Active Center: (%d, %d)", m_ActivePixelX, m_ActivePixelY);
                }
            }

            // ROI controls (only meaningful when pixel center exists)
            if (m_HasPixelCenter) {
                ImGui::Separator();
                ImGui::Text("Pixel Center: (%d, %d)", m_PixelX, m_PixelY);
                ImGui::Checkbox("Only show neighborhood points (ROI)", &m_RoiEnabled);
                ImGui::SliderInt("ROI radius (pixels)", &m_RoiRadius, 50, 500);
                ImGui::SliderInt("Highlight size (pixels)", &m_HighlightSizePixels, 1, 300);
                ImGui::SliderFloat("Highlight point size (scale)", &m_HighlightPointSizeScale, 1.0f, 20.0f, "%.1fx");
                if (ImGui::Button("Reload FITS with ROI")) {
                    // Trigger the event again with current ROI settings
                    if (!m_NewAlignedFitsPath.empty() || !m_NewTemplateFitsPath.empty()) {
                        m_HasNewFitsPair = true;
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Center view/rotate on ROI")) {
                    m_RequestCenterCameraOnRoi = true;
                }
            }
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
    const auto& entries = GetDirectoryEntriesCached(dir);
    if (entries.empty()) {
        // Could be empty directory or unreadable; keep UI lightweight.
        // Only show a hint for unreadable directories when it seems to exist but yields nothing.
        // (We avoid probing filesystem again here.)
    }

    for (const auto& entry : entries) {
        const fs::path p = entry.path;
        const std::string& name = entry.name;
        const bool isDir = entry.isDirectory;

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
                m_SelectedSize = entry.size;
                m_PreviewText = ReadTextFilePreview(p);

                // If selecting a txt, parse a FITS pair for the application to load
                if (p.extension().string() == ".txt") {
                    TryParseFitsPairFromTxtSelection(p);
                } else {
                    m_LastParseMessage.clear();
                }
            }
        }

        ImGui::PopID();
    }
}

