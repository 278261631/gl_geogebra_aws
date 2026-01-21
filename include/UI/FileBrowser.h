#pragma once

#include <string>
#include <vector>
#include <filesystem>

class FileBrowser {
public:
    FileBrowser();
    ~FileBrowser();

    void Render();
    void SetDefaultPath(const std::string& path);
    void SetDefaultFile(const std::string& filename);

    bool IsOpen() const { return m_IsOpen; }
    void Open() { m_IsOpen = true; }
    void Close() { m_IsOpen = false; }

    const std::string& GetSelectedFile() const { return m_SelectedFile; }
    bool HasSelection() const { return !m_SelectedFile.empty(); }
    void ClearSelection() { m_SelectedFile.clear(); }

    bool HasNewSelection() const { return m_HasNewSelection; }
    void ClearNewSelectionFlag() { m_HasNewSelection = false; }

    bool HasNewCheck() const { return m_HasNewCheck; }
    void ClearNewCheckFlag() { m_HasNewCheck = false; }
    const std::string& GetNewCheckedFile() const { return m_NewCheckedFile; }

    bool HasNewUncheck() const { return m_HasNewUncheck; }
    void ClearNewUncheckFlag() { m_HasNewUncheck = false; }
    const std::string& GetNewUncheckedFile() const { return m_NewUncheckedFile; }

private:
    void RefreshDirectory();
    void RenderDirectoryTree();
    void RenderFileList();

    bool m_IsOpen;
    std::string m_CurrentPath;
    std::string m_DefaultPath;
    std::string m_DefaultFile;
    std::string m_SelectedFile;
    bool m_HasNewSelection;
    bool m_FirstRender;

    struct FileEntry {
        std::string name;
        std::string path;
        bool isDirectory;
        size_t size;
        bool isChecked;
    };

    std::vector<FileEntry> m_Files;
    int m_SelectedIndex;

    std::string m_NewCheckedFile;
    std::string m_NewUncheckedFile;
    bool m_HasNewCheck;
    bool m_HasNewUncheck;
};

