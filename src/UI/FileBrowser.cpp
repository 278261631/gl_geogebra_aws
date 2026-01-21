#include "UI/FileBrowser.h"
#include <imgui.h>
#include <algorithm>

namespace fs = std::filesystem;

FileBrowser::FileBrowser()
    : m_IsOpen(true)  // Default open
    , m_CurrentPath("")
    , m_DefaultPath("test-img")
    , m_DefaultFile("small_image.jpg")
    , m_SelectedFile("")
    , m_HasNewSelection(false)
    , m_FirstRender(true)
    , m_SelectedIndex(-1)
    , m_HasNewCheck(false)
    , m_HasNewUncheck(false)
{
    SetDefaultPath(m_DefaultPath);
}

FileBrowser::~FileBrowser() {
}

void FileBrowser::SetDefaultPath(const std::string& path) {
    m_DefaultPath = path;

    if (fs::exists(path) && fs::is_directory(path)) {
        m_CurrentPath = fs::absolute(path).string();
    } else {
        m_CurrentPath = fs::current_path().string();
    }

    RefreshDirectory();
}

void FileBrowser::SetDefaultFile(const std::string& filename) {
    m_DefaultFile = filename;
}

void FileBrowser::RefreshDirectory() {
    m_Files.clear();
    m_SelectedIndex = -1;

    if (!fs::exists(m_CurrentPath) || !fs::is_directory(m_CurrentPath)) {
        return;
    }

    try {
        for (const auto& entry : fs::directory_iterator(m_CurrentPath)) {
            FileEntry fileEntry;
            fileEntry.name = entry.path().filename().string();
            fileEntry.path = entry.path().string();
            fileEntry.isDirectory = entry.is_directory();
            fileEntry.size = fileEntry.isDirectory ? 0 : fs::file_size(entry.path());
            fileEntry.isChecked = false;

            m_Files.push_back(fileEntry);
        }

        std::sort(m_Files.begin(), m_Files.end(), [](const FileEntry& a, const FileEntry& b) {
            if (a.isDirectory != b.isDirectory) {
                return a.isDirectory > b.isDirectory;
            }
            return a.name < b.name;
        });

        // Auto-select and check default file if it exists
        if (!m_DefaultFile.empty() && m_FirstRender) {
            for (size_t i = 0; i < m_Files.size(); i++) {
                if (m_Files[i].name == m_DefaultFile && !m_Files[i].isDirectory) {
                    m_SelectedIndex = static_cast<int>(i);
                    m_SelectedFile = m_Files[i].path;
                    m_Files[i].isChecked = true;
                    m_HasNewCheck = true;
                    m_NewCheckedFile = m_Files[i].path;
                    m_FirstRender = false;
                    break;
                }
            }
        }

    } catch (const std::exception& e) {
    }
}

void FileBrowser::Render() {
    if (!m_IsOpen) {
        return;
    }
    
    ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("File Browser", &m_IsOpen)) {
        ImGui::Text("Current Path: %s", m_CurrentPath.c_str());
        ImGui::Separator();
        
        if (ImGui::Button("Parent Directory")) {
            fs::path parentPath = fs::path(m_CurrentPath).parent_path();
            if (fs::exists(parentPath)) {
                m_CurrentPath = parentPath.string();
                RefreshDirectory();
            }
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Refresh")) {
            RefreshDirectory();
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Default Path")) {
            SetDefaultPath(m_DefaultPath);
        }
        
        ImGui::Separator();
        
        RenderFileList();
        
        ImGui::Separator();
        
        if (!m_SelectedFile.empty()) {
            ImGui::Text("Selected: %s", m_SelectedFile.c_str());
            
            if (ImGui::Button("Open")) {
                m_IsOpen = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                m_SelectedFile.clear();
                m_IsOpen = false;
            }
        } else {
            if (ImGui::Button("Close")) {
                m_IsOpen = false;
            }
        }
    }
    ImGui::End();
}

void FileBrowser::RenderFileList() {
    ImGui::BeginChild("FileList", ImVec2(0, -40), true);

    ImGui::Columns(4, "FileColumns");
    ImGui::Separator();
    ImGui::Text("Show"); ImGui::NextColumn();
    ImGui::Text("Name"); ImGui::NextColumn();
    ImGui::Text("Type"); ImGui::NextColumn();
    ImGui::Text("Size"); ImGui::NextColumn();
    ImGui::Separator();

    for (size_t i = 0; i < m_Files.size(); i++) {
        auto& file = m_Files[i];

        bool isSelected = (m_SelectedIndex == static_cast<int>(i));

        // Checkbox column (only for files, not directories)
        if (!file.isDirectory) {
            ImGui::PushID(static_cast<int>(i));
            bool wasChecked = file.isChecked;
            if (ImGui::Checkbox("##check", &file.isChecked)) {
                if (file.isChecked && !wasChecked) {
                    // Newly checked
                    m_HasNewCheck = true;
                    m_NewCheckedFile = file.path;
                } else if (!file.isChecked && wasChecked) {
                    // Newly unchecked
                    m_HasNewUncheck = true;
                    m_NewUncheckedFile = file.path;
                }
            }
            ImGui::PopID();
        }
        ImGui::NextColumn();

        // Name column
        if (file.isDirectory) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        }

        if (ImGui::Selectable(file.name.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
            if (file.isDirectory) {
                m_CurrentPath = file.path;
                RefreshDirectory();
            } else {
                m_SelectedIndex = static_cast<int>(i);
                m_SelectedFile = file.path;
                m_HasNewSelection = true;
            }
        }

        if (file.isDirectory) {
            ImGui::PopStyleColor();
        }

        ImGui::NextColumn();

        // Type column
        ImGui::Text("%s", file.isDirectory ? "Folder" : "File");
        ImGui::NextColumn();

        // Size column
        if (!file.isDirectory) {
            if (file.size < 1024) {
                ImGui::Text("%zu B", file.size);
            } else if (file.size < 1024 * 1024) {
                ImGui::Text("%.2f KB", file.size / 1024.0);
            } else {
                ImGui::Text("%.2f MB", file.size / (1024.0 * 1024.0));
            }
        }
        ImGui::NextColumn();
    }
    
    ImGui::Columns(1);
    ImGui::EndChild();
}

