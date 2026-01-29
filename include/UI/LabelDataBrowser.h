#pragma once

#include <string>
#include <filesystem>

class LabelDataBrowser {
public:
    LabelDataBrowser();
    ~LabelDataBrowser();

    void Render();

    bool IsOpen() const { return m_IsOpen; }
    void Open() { m_IsOpen = true; }
    void Close() { m_IsOpen = false; }

    void SetRootPath(const std::string& path);
    const std::string& GetRootPath() const { return m_RootPath; }

private:
    void ResolveRootPath();
    void RenderDirectoryTree(const std::filesystem::path& dir);

    bool m_IsOpen;
    std::string m_RootPath;          // configured root (usually relative)
    std::string m_ResolvedRootPath;  // absolute path used for browsing
    std::string m_SelectedPath;      // absolute selected file path
    std::string m_PreviewText;       // cached preview for selected file
    std::uintmax_t m_SelectedSize;
    bool m_SelectedIsFile;
};

