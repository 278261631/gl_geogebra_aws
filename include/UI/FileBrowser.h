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
    
    bool IsOpen() const { return m_IsOpen; }
    void Open() { m_IsOpen = true; }
    void Close() { m_IsOpen = false; }
    
    const std::string& GetSelectedFile() const { return m_SelectedFile; }
    bool HasSelection() const { return !m_SelectedFile.empty(); }
    void ClearSelection() { m_SelectedFile.clear(); }

private:
    void RefreshDirectory();
    void RenderDirectoryTree();
    void RenderFileList();

    bool m_IsOpen;
    std::string m_CurrentPath;
    std::string m_DefaultPath;
    std::string m_SelectedFile;
    
    struct FileEntry {
        std::string name;
        std::string path;
        bool isDirectory;
        size_t size;
    };
    
    std::vector<FileEntry> m_Files;
    int m_SelectedIndex;
};

