#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>
#include <vector>

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

    // Event: when a .txt is selected, we may parse a FITS pair to load.
    bool HasNewFitsPair() const { return m_HasNewFitsPair; }
    void ClearNewFitsPairFlag() { m_HasNewFitsPair = false; }
    const std::string& GetNewAlignedFitsPath() const { return m_NewAlignedFitsPath; }
    const std::string& GetNewTemplateFitsPath() const { return m_NewTemplateFitsPath; }
    const std::string& GetNewFitsSourceTxtPath() const { return m_NewFitsSourceTxtPath; }
    const std::string& GetLastParseMessage() const { return m_LastParseMessage; }

private:
    void ResolveRootPath();
    void RenderDirectoryTree(const std::filesystem::path& dir);
    void TryParseFitsPairFromTxtSelection(const std::filesystem::path& txtPath);

    struct CachedEntry {
        std::filesystem::path path;
        std::string name;
        bool isDirectory;
        std::uintmax_t size;
    };

    const std::vector<CachedEntry>& GetDirectoryEntriesCached(const std::filesystem::path& dir);

    bool m_IsOpen;
    std::string m_RootPath;          // configured root (usually relative)
    std::string m_ResolvedRootPath;  // absolute path used for browsing
    std::string m_SelectedPath;      // absolute selected file path
    std::string m_PreviewText;       // cached preview for selected file
    std::uintmax_t m_SelectedSize;
    bool m_SelectedIsFile;

    bool m_HasNewFitsPair;
    std::string m_NewAlignedFitsPath;
    std::string m_NewTemplateFitsPath;
    std::string m_NewFitsSourceTxtPath;
    std::string m_LastParseMessage;

    // Cache directory listings so the UI doesn't re-scan the filesystem every frame.
    std::unordered_map<std::string, std::vector<CachedEntry>> m_DirectoryCache;
};

