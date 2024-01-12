#include "TRAPPCH.h"
#include "FileSystem.h"

#include "Utils/String/String.h"
#include "Application.h"

namespace
{
    [[nodiscard]] TRAP::Optional<uintmax_t> GetFolderSizeInternal(const std::filesystem::path& path, bool recursive);
    [[nodiscard]] TRAP::Optional<uintmax_t> GetFileSizeInternal(const std::filesystem::path& path);
    [[nodiscard]] bool OpenFolderInFileBrowserInternal(const std::filesystem::path& p);
    [[nodiscard]] bool OpenFileInFileBrowserInternal(const std::filesystem::path& p);

#ifdef TRAP_PLATFORM_LINUX
    [[nodiscard]] TRAP::Optional<std::filesystem::path> GetDocumentsFolderPathLinux();
    [[nodiscard]] bool OpenExternallyLinux(const std::filesystem::path& path);
    [[nodiscard]] bool OpenFolderInFileBrowserLinux(const std::filesystem::path& p);
    [[nodiscard]] bool OpenFileInFileBrowserLinux(const std::filesystem::path& p);
    [[nodiscard]] TRAP::Optional<std::filesystem::path> GetHomeFolderPathLinux();
#elif defined(TRAP_PLATFORM_WINDOWS)
    [[nodiscard]] TRAP::Optional<std::filesystem::path> GetDocumentsFolderPathWindows();
    [[nodiscard]] bool OpenExternallyWindows(const std::filesystem::path& path);
    [[nodiscard]] bool OpenFolderInFileBrowserWindows(const std::filesystem::path& p);
    [[nodiscard]] bool OpenFileInFileBrowserWindows(const std::filesystem::path& p);
#endif /*TRAP_PLATFORM_LINUX*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::Init()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

	TP_DEBUG(Log::FileSystemPrefix, "Initializing File System");

    //Create game temp folder
    if(const auto gameTempFolder = GetGameTempFolderPath(); gameTempFolder && !CreateFolder(*gameTempFolder))
        TP_ERROR(Log::FileSystemPrefix, "Failed to create game temp folder: ", *gameTempFolder, "!");

    //Create game document folder
#ifndef TRAP_HEADLESS_MODE
    if(const auto gameDocsFolder = GetGameDocumentsFolderPath(); gameDocsFolder && !CreateFolder(*gameDocsFolder))
        TP_ERROR(Log::FileSystemPrefix, "Failed to create game documents folder: ", *gameDocsFolder, "!");
#endif /*TRAP_HEADLESS_MODE*/

    //Create game log folder
    if(const auto gameLogFolder = GetGameLogFolderPath(); gameLogFolder && !CreateFolder(*gameLogFolder))
        TP_ERROR(Log::FileSystemPrefix, "Failed to create game log folder: ", *gameLogFolder, "!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::Shutdown()
{
	TP_DEBUG(Log::FileSystemPrefix, "Shutting down File System");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::vector<u8>> TRAP::FileSystem::ReadFile(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!path.empty(), "FileSystem::ReadFile(): Path is empty!");

    if(!IsFile(path))
        return TRAP::NullOpt;

    const auto fileSize = GetFileSizeInternal(path);
    if(!fileSize)
        return TRAP::NullOpt;

    if(*fileSize == 0)
        return std::vector<u8>{};

    std::ifstream file(path, std::ios::binary);
    if(!file.is_open() || !file.good())
    {
		TP_ERROR(Log::FileSystemPrefix, "Couldn't read file: ", path, " (failed to open file)!");
        return TRAP::NullOpt;
    }

    std::vector<u8> buffer(*fileSize);

    file.read(reinterpret_cast<char*>(buffer.data()), NumericCast<std::streamsize>(buffer.size()));
    if(file.fail())
    {
		TP_ERROR(Log::FileSystemPrefix, "Couldn't read file: ", path, " (failed to read data)!");
        return TRAP::NullOpt;
    }

    return buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::string> TRAP::FileSystem::ReadTextFile(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!path.empty(), "FileSystem::ReadTextFile(): Path is empty!");

    if(!IsFile(path))
        return TRAP::NullOpt;

    const auto fileSize = GetFileSizeInternal(path);
    if(!fileSize)
        return TRAP::NullOpt;
    if(*fileSize == 0)
        return "";

    std::ifstream file(path, std::ios::binary);
    if(!file.is_open() || !file.good())
    {
		TP_ERROR(Log::FileSystemPrefix, "Couldn't read file: ", path, " (failed to open file)!");
        return TRAP::NullOpt;
    }

    std::string result(*fileSize, '\0');

    file.read(result.data(), NumericCast<std::streamsize>(*fileSize));

    if(file.fail())
    {
		TP_ERROR(Log::FileSystemPrefix, "Couldn't read file: ", path, " (failed to read data)!");
        return TRAP::NullOpt;
    }

    std::erase(result, '\r');

    return result;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::WriteFile(const std::filesystem::path& path, const std::span<const u8> buffer, const WriteMode mode)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!path.empty(), "FileSystem::WriteFile(): Path is empty!");
    TRAP_ASSERT(!buffer.empty(), "FileSystem::WriteFile(): Buffer is empty!");

    const std::ios_base::openmode modeFlags = (mode == WriteMode::Overwrite) ? (std::ios::binary | std::ios::trunc) :
	                                                                           (std::ios::binary | std::ios::app);
    std::ofstream file(path, modeFlags);
    if(!file.is_open() || !file.good())
    {
	    TP_ERROR(Log::FileSystemPrefix, "Couldn't write file: ", path, " (failed to open file)!");
        return false;
    }

    file.write(reinterpret_cast<const char*>(buffer.data()), NumericCast<std::streamsize>(buffer.size()));
    if(file.fail())
    {
	    TP_ERROR(Log::FileSystemPrefix, "Couldn't write file: ", path, " (failed to write data)!");
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::WriteTextFile(const std::filesystem::path& path, const std::string_view text, const WriteMode mode)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!path.empty(), "FileSystem::WriteTextFile(): Path is empty!");
    TRAP_ASSERT(!text.empty(), "FileSystem::WriteTextFile(): Text is empty!");

    const std::ios_base::openmode modeFlags = (mode == WriteMode::Overwrite) ? (std::ios::trunc) : (std::ios::app);
    std::ofstream file(path, modeFlags);
    if(!file.is_open() || !file.good())
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't write file: ", path, " (failed to open file)!");
        return false;
    }

    file.write(text.data(), NumericCast<std::streamsize>(text.size()));
    if(file.fail())
    {
	    TP_ERROR(Log::FileSystemPrefix, "Couldn't write file: ", path, " (failed to write data)!");
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::CreateFolder(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!path.empty(), "FileSystem::CreateFolder(): Path is empty!");

    std::error_code ec{};
    std::filesystem::create_directories(path, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't create folder: ", path, " (", ec.message(), ")!");
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::Delete(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!path.empty(), "FileSystem::Delete(): Path is empty!");

    std::error_code ec{};
    const std::uintmax_t res = std::filesystem::remove_all(path, ec);
    if(ec || res == std::numeric_limits<std::uintmax_t>::max())
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't delete file or folder: ", path, " (", ec.message(), ")!");
        return false;
    }

    return res > 0;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::Move(const std::filesystem::path& oldPath, const std::filesystem::path& newPath)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!oldPath.empty(), "FileSystem::Move(): OldPath is empty!");
    TRAP_ASSERT(!newPath.empty(), "FileSystem::Move(): NewPath is empty!");

    std::error_code ec{};
    std::filesystem::rename(oldPath, newPath, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't move from: ", oldPath,
                 " to ", newPath, " (", ec.message(), ")!");
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::Copy(const std::filesystem::path& source, const std::filesystem::path& destination,
                            const bool overwriteExisting)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!source.empty(), "FileSystem::Copy(): Source is empty!");
    TRAP_ASSERT(!destination.empty(), "FileSystem::Copy(): Destination is empty!");

    std::error_code ec{};
    std::filesystem::copy_options options = std::filesystem::copy_options::recursive;
    if(overwriteExisting)
        options |= std::filesystem::copy_options::overwrite_existing;
    else
        options |= std::filesystem::copy_options::skip_existing;
    std::filesystem::copy(source, destination, options, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't copy folder: ", source, " to ", destination,
                 " (", ec.message(), ")!");
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::Rename(const std::filesystem::path& oldPath, const std::string_view newName)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!oldPath.empty(), "FileSystem::Rename(): OldPath is empty!");
    TRAP_ASSERT(!newName.empty(), "FileSystem::Rename(): NewName is empty!");

    std::filesystem::path newPath = oldPath;
    try
    {
        newPath.replace_filename(newName);
    }
    catch(const std::exception& e)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't rename file or folder: ", oldPath, " to \"", newName,
                 "\" (", e.what(), ")!");
        return false;
    }

    return Move(oldPath, newPath);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::FileSystem::Exists(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!path.empty(), "FileSystem::Exists(): path is empty!");

    std::error_code ec{};
    const bool res = std::filesystem::exists(path, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if file or folder exists: ", path,
                 " (", ec.message(), ")!");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<uintmax_t> TRAP::FileSystem::GetSize(const std::filesystem::path& path, const bool recursive)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!path.empty(), "FileSystem::GetSize(): Path is empty!");

    if(IsFile(path))
        return GetFileSizeInternal(path);

    if(IsFolder(path))
        return GetFolderSizeInternal(path, recursive);

    return TRAP::NullOpt;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::filesystem::file_time_type> TRAP::FileSystem::GetLastWriteTime(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!path.empty(), "FileSystem::GetLastWriteTime(): Path is empty!");

    std::error_code ec{};
    const auto res = std::filesystem::last_write_time(path, ec);
    if(ec || res == std::filesystem::file_time_type::min())
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get last write time: ", path, " (", ec.message(), ")!");
        return TRAP::NullOpt;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::string> TRAP::FileSystem::GetFileNameWithEnding(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!path.empty(), "FileSystem::GetFileNameWithEnding(): Path is empty!");

    std::string res{};
    try //filename() may throw implementation defined exceptions
    {
        res = path.filename().string();
    }
    catch(const std::exception& e)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get file name with ending: ", path, " (", e.what(), ")!");
        return TRAP::NullOpt;
    }

    if(res.empty())
        return TRAP::NullOpt;

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::string> TRAP::FileSystem::GetFileNameWithoutEnding(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!path.empty(), "FileSystem::GetFileNameWithoutEnding(): Path is empty!");

    std::string res{};
    try //stem() may throw implementation defined exceptions
    {
        res = path.stem().string();
    }
    catch(const std::exception& e)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get file name: ", path, " (", e.what(), ")!");
        return TRAP::NullOpt;
    }

    if(res.empty())
        return TRAP::NullOpt;

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::string> TRAP::FileSystem::GetFileEnding(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!path.empty(), "FileSystem::GetFileEnding(): Path is empty!");

    std::string res{};
    try //extension() may throw implementation defined exceptions
    {
        res = path.extension().string();
    }
    catch(const std::exception& e)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get file ending: ", path, " (", e.what(), ")!");
        return TRAP::NullOpt;
    }

    if(res.empty())
        return TRAP::NullOpt;

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::filesystem::path> TRAP::FileSystem::GetFolderPath(const std::filesystem::path& filePath)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!filePath.empty(), "FileSystem::GetFolderPath(): Path is empty!");

    std::filesystem::path res{};
    try //parent_path() may throw implementation defined exceptions
    {
        res = filePath.parent_path();
    }
    catch(const std::exception& e)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get folder path: ", filePath, " (", e.what(), ")!");
        return TRAP::NullOpt;
    }

    if(res.empty())
        return TRAP::NullOpt;

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::filesystem::path> TRAP::FileSystem::GetEngineTempFolderPath()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    std::error_code ec{};
    const std::filesystem::path path = std::filesystem::temp_directory_path(ec);

    if(ec || path.empty())
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get temp directory path (", ec.message(), ")!");
        return TRAP::NullOpt;
    }

    return (path / "TRAP");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::filesystem::path> TRAP::FileSystem::GetGameTempFolderPath()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    const auto tempFolder = GetEngineTempFolderPath();
    if(!tempFolder)
        return TRAP::NullOpt;

    return (*tempFolder / TRAP::Application::GetGameName());
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::filesystem::path> TRAP::FileSystem::GetDocumentsFolderPath()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

#ifdef TRAP_PLATFORM_WINDOWS
    return GetDocumentsFolderPathWindows();
#elif defined(TRAP_PLATFORM_LINUX)
    return GetDocumentsFolderPathLinux();
#else
    assert(false, "GetDocumentsFolderPath() not implemented!");
    return TRAP::NullOpt;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::filesystem::path> TRAP::FileSystem::GetGameDocumentsFolderPath()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

	const auto docsFolder = GetDocumentsFolderPath();
    if(!docsFolder)
        return TRAP::NullOpt;

    return (*docsFolder / "TRAP" / TRAP::Application::GetGameName());
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::filesystem::path> TRAP::FileSystem::GetGameLogFolderPath()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

#ifndef TRAP_HEADLESS_MODE
    const auto docsFolder = GetGameDocumentsFolderPath();
    if(!docsFolder)
        return TRAP::NullOpt;

    return (*docsFolder / "logs");
#else
    return "logs";
#endif /*TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::FileSystem::IsEquivalent(const std::filesystem::path& p1, const std::filesystem::path& p2)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!p1.empty(), "FileSystem::IsEquivalent(): Path1 is empty!");
    TRAP_ASSERT(!p2.empty(), "FileSystem::IsEquivalent(): Path2 is empty!");

    std::error_code ec{};
    const bool res = std::filesystem::equivalent(p1, p2, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Error while checking if paths are equivalent: ", p1, " and ",
                 p2, " (", ec.message(), ")!");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::FileSystem::IsFolder(const std::filesystem::path& p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!p.empty(), "FileSystem::IsFolder(): Path is empty!");

    std::error_code ec{};
    const bool res = std::filesystem::is_directory(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if path leads to a folder: ", p, " (", ec.message(), ")!");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::FileSystem::IsFile(const std::filesystem::path& p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!p.empty(), "FileSystem::IsFile(): Path is empty!");

    std::error_code ec{};
    const bool res = std::filesystem::is_regular_file(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if path leads to a regular file: ", p, " (", ec.message(), ")!");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::FileSystem::IsEmpty(const std::filesystem::path& p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!p.empty(), "FileSystem::IsEmpty(): Path is empty!");

    std::error_code ec{};
    const bool res = std::filesystem::is_empty(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if path leads to an empty file or folder: ", p, " (", ec.message(), ")!");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::FileSystem::IsAbsolute(const std::filesystem::path& p)
{
    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!p.empty(), "FileSystem::IsAbsolute(): Path is empty!");

    bool res = false;

    try
    {
        res = p.is_absolute();
    }
    catch(const std::exception& e)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if path is absolute: ", p, " (", e.what(), ")!");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::FileSystem::IsRelative(const std::filesystem::path& p)
{
    ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!p.empty(), "FileSystem::IsRelative(): Path is empty!");

    bool res = false;

    try
    {
        res = p.is_relative();
    }
    catch(const std::exception& e)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if path is relative: ", p, " (", e.what(), ")!");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::filesystem::path> TRAP::FileSystem::ToAbsolutePath(const std::filesystem::path& p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!p.empty(), "FileSystem::ToAbsolutePath(): Path is empty!");

    if(p.empty())
        return TRAP::NullOpt;

    std::error_code ec{};
    std::filesystem::path res = std::filesystem::absolute(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Error while converting path to absolute: ", p, " (", ec.message(), ")!");
        return TRAP::NullOpt;
    }

    if(res.empty())
        return TRAP::NullOpt;

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::filesystem::path> TRAP::FileSystem::ToCanonicalAbsolutePath(const std::filesystem::path& p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!p.empty(), "FileSystem::ToCanonicalAbsolutePath(): Path is empty!");

    if(p.empty())
        return TRAP::NullOpt;

    std::error_code ec{};
    std::filesystem::path res = std::filesystem::canonical(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Error while converting path to canonical absolute: ", p, " (", ec.message(), ")!");
        return TRAP::NullOpt;
    }

    if(res.empty())
        return TRAP::NullOpt;

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::filesystem::path> TRAP::FileSystem::ToRelativePath(const std::filesystem::path& p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!p.empty(), "FileSystem::ToRelativePath(): Path is empty!");

    if(p.empty())
        return TRAP::NullOpt;

    std::error_code ec{};
    std::filesystem::path res = std::filesystem::proximate(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Error while converting path to relative: ", p, " (", ec.message(), ")!");
        return TRAP::NullOpt;
    }

    if(res.empty())
        return TRAP::NullOpt;

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::filesystem::path> TRAP::FileSystem::ToRelativePath(const std::filesystem::path& p,
                                                                                     const std::filesystem::path& base)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!p.empty(), "FileSystem::ToRelativePath(): Path is empty!");
    TRAP_ASSERT(!base.empty(), "FileSystem::ToRelativePath(): Base is empty!");

    if(p.empty() || base.empty())
        return TRAP::NullOpt;

    std::error_code ec{};
    std::filesystem::path res = std::filesystem::proximate(p, base, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Error while converting path to relative: ", p, " (", ec.message(), ")!");
        return TRAP::NullOpt;
    }

    if(res.empty())
        return TRAP::NullOpt;

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::SetCurrentWorkingFolderPath(const std::filesystem::path &p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!p.empty(), "FileSystem::SetCurrentWorkingFolderPath(): Path is empty!");

    std::error_code ec{};
    std::filesystem::current_path(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Error while setting current path: ", p, " (", ec.message(), ")!");
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<std::filesystem::path> TRAP::FileSystem::GetCurrentWorkingFolderPath()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    std::error_code ec{};
    auto currPath = std::filesystem::current_path(ec);

    if(ec || currPath.empty())
    {
        TP_ERROR(Log::FileSystemPrefix, "Error while retrieving current path (", ec.message(), ")!");
        return TRAP::NullOpt;
    }

    return currPath;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::OpenInFileBrowser(const std::filesystem::path& p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!p.empty(), "FileSystem::OpenInFileBrowser(): Path is empty!");

    const auto absPath = TRAP::FileSystem::ToAbsolutePath(p);
    if(!absPath)
    {
        TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open folder in file browser: ", p, " (failed to convert path to absolute format)!");
        return false;
    }

    if(IsFile(*absPath))
        return OpenFileInFileBrowserInternal(*absPath);
    if(IsFolder(*absPath))
        return OpenFolderInFileBrowserInternal(*absPath);

    TP_ERROR(Log::FileSystemPrefix, "Couldn't open file/folder in file browser: ", p, " (path doesn't lead to a file/folder)!");
    return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::OpenExternally(const std::filesystem::path& p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

    TRAP_ASSERT(!p.empty(), "FileSystem::OpenExternally(): Path is empty!");

    const auto absPath = ToAbsolutePath(p);
    if(!absPath)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't open externally: ", p, " (failed to convert path to absolute format)!");
        return false;
    }

#ifdef TRAP_PLATFORM_WINDOWS
    return OpenExternallyWindows(*absPath);
#elif defined(TRAP_PLATFORM_LINUX)
    return OpenExternallyLinux(*absPath);
#else
    assert(false, "OpenExternally() not implemented!");
    return false;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//
//INTERNAL-----------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

namespace
{
    [[nodiscard]] TRAP::Optional<uintmax_t> GetFileSizeFallbackInternal(const std::filesystem::path& path)
    {
    	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        TRAP_ASSERT(!path.empty(), "FileSystem::GetFileSizeFallbackInternal(): Path is empty!");

        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if(!file.is_open() || !file.good())
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't get file size of: ", path, "!");
            return TRAP::NullOpt;
        }

        const auto fileSize = file.tellg();

        if(fileSize == std::numeric_limits<std::ifstream::pos_type>::max())
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't get file size of: ", path, "!");
            return TRAP::NullOpt;
        }

        return fileSize;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] TRAP::Optional<uintmax_t> GetFileSizeInternal(const std::filesystem::path& path)
    {
    	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        TRAP_ASSERT(!path.empty(), "FileSystem::GetFileSizeInternal(): Path is empty!");

        std::error_code ec{};
        const uintmax_t size = std::filesystem::file_size(path, ec);

        if(ec || size == std::numeric_limits<uintmax_t>::max())
            return GetFileSizeFallbackInternal(path);

        return size;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    template<typename T>
    requires std::same_as<T, std::filesystem::recursive_directory_iterator> ||
             std::same_as<T, std::filesystem::directory_iterator>
    [[nodiscard]] TRAP::Optional<uintmax_t> GetFolderSizeInternal(const T& dirIt)
    {
    	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        std::error_code ec{};
        uintmax_t size = 0;

        for(const auto& entry : dirIt)
        {
            if(const bool regFile = entry.is_regular_file(ec); ec || !regFile)
                continue;

            uintmax_t fileSize = entry.file_size(ec);

            if(ec || fileSize == std::numeric_limits<std::uintmax_t>::max())
            {
                const auto fileSizeFallback = GetFileSizeFallbackInternal(entry.path());
                if (!fileSizeFallback)
                    return TRAP::NullOpt;

                fileSize = *fileSizeFallback;
            }

            size += fileSize;
        }

        return size;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] TRAP::Optional<uintmax_t> GetFolderSizeInternal(const std::filesystem::path& path, const bool recursive)
    {
    	ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        std::error_code ec{};

        if(recursive)
        {
            const std::filesystem::recursive_directory_iterator rDIt(path, ec);
            if(ec)
            {
                TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't get folder size of: ", path, " (", ec.message(), ")!");
                return TRAP::NullOpt;
            }

            return GetFolderSizeInternal(rDIt);
        }

        const std::filesystem::directory_iterator dIt(path, ec);
        if(ec)
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't get folder size of: ", path, " (", ec.message(), ")!");
            return TRAP::NullOpt;
        }

        return GetFolderSizeInternal(dIt);
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Opens the file browser at the given path.
    /// @param p Path to folder to open in file browser.
    /// @return True on success, false otherwise.
    /// @remark @linux Linux uses xdg-open for this functionality.
    [[nodiscard]] bool OpenFolderInFileBrowserInternal(const std::filesystem::path& p)
    {
        ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        TRAP_ASSERT(!p.empty(), "FileSystem::OpenFolderInFileBrowserInternal(): Path is empty!");
        TRAP_ASSERT(TRAP::FileSystem::IsAbsolute(p), "FileSystem::OpenFolderInFileBrowserInternal(): Path is not absolute!");

#ifdef TRAP_PLATFORM_WINDOWS
        return OpenFolderInFileBrowserWindows(p);
#elif defined(TRAP_PLATFORM_LINUX)
        return OpenFolderInFileBrowserLinux(p);
#else
        assert(false, "OpenFolderInFileBrowserInternal() not implemented!");
        return false;
#endif
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Opens the file browser and selects the given file.
    /// @param p Path to file to open in file browser.
    /// @return True on success, false otherwise.
    /// @remark @linux Linux uses xdg-open for this functionality. The given file won't be selected.
    [[nodiscard]] bool OpenFileInFileBrowserInternal(const std::filesystem::path& p)
    {
        ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        TRAP_ASSERT(!p.empty(), "OpenFileInFileBrowser(): Path is empty!");
        TRAP_ASSERT(TRAP::FileSystem::IsAbsolute(p), "FileSystem::OpenFileInFileBrowserInternal(): Path is not absolute!");

    #ifdef TRAP_PLATFORM_WINDOWS
        return OpenFileInFileBrowserWindows(p);
    #elif defined(TRAP_PLATFORM_LINUX)
        return OpenFileInFileBrowserLinux(p);
    #else
        assert(false, "OpenFileInFileBrowser() not implemented!");
        return false;
    #endif
    }

    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_LINUX

    [[nodiscard]] TRAP::Optional<std::filesystem::path> GetXDGConfigFolderPath()
    {
        if(const char* const tempRes = getenv("XDG_CONFIG_HOME"); tempRes)
            return tempRes;

        const auto homeFolder = GetHomeFolderPathLinux();
        if(!homeFolder)
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Failed to get documents folder path (failed to retrieve home directory path)!");
            return TRAP::NullOpt;
        }

        return *homeFolder / ".config";
    }

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] TRAP::Optional<std::filesystem::path> GetDocumentsFolderPathFromUserDirs()
    {
        TRAP::Optional<std::filesystem::path> configFolder = GetXDGConfigFolderPath();
        if(!configFolder)
            return TRAP::NullOpt;
        *configFolder /= "user-dirs.dirs";

        //Get Documents folder
        std::ifstream file(*configFolder);
        if(!file.is_open() || !file.good())
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Failed to get documents folder path (failed to open ", *configFolder, ")!");
            return TRAP::NullOpt;
        }

        static constexpr std::string_view XDGDocumentsDir = "XDG_DOCUMENTS_DIR";

        std::string line{};
        while(std::getline(file, line))
        {
            const auto commentIndex = line.find_first_of('#');
            const auto xdgDocuments = line.find(XDGDocumentsDir.data());

            if(xdgDocuments == std::string::npos)
                continue;
            if(commentIndex != std::string::npos && commentIndex < xdgDocuments) //Ignore comment entries
                continue;

            const auto valueStartPos = line.find_first_of('\"');
            const auto valueEndPos = line.find_last_of('\"');

            if(valueStartPos == std::string::npos || valueEndPos == std::string::npos || valueStartPos == valueEndPos)
                continue;

            return line.substr(valueStartPos + 1, valueEndPos - valueStartPos - 1);
        }

        TP_ERROR(TRAP::Log::FileSystemPrefix, "Failed to get documents folder path (no XDG_DOCUMENTS_DIR entry found in ", *configFolder, ")!");
        return TRAP::NullOpt;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Retrieves the effective user's document dir.
    ///        If the user is running as root we ignore the HOME environment. It works badly with sudo.
    /// @return The document directory on success, empty optional otherwise.
    ///         HOME environment is respected for non-root users if it exists.
    /// @warning @linux Writing to $HOME as root implies security concerns that a multiplatform
    ///                 program cannot be assumed to handle.
    [[nodiscard]] TRAP::Optional<std::filesystem::path> GetDocumentsFolderPathLinux()
    {
        ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        static std::filesystem::path documentsDir{};

        if(!documentsDir.empty()) //Return cached docs dir path
            return documentsDir;

        documentsDir = "$HOME/Documents";

        //Get Documents folder
        const auto userDirsDocumentsFolderPath = GetDocumentsFolderPathFromUserDirs();
        if(!userDirsDocumentsFolderPath)
            return TRAP::NullOpt;
        documentsDir = *userDirsDocumentsFolderPath;

        static constexpr std::string_view HomeVarName = "$HOME";
        if(const auto homeVar = documentsDir.string().find(HomeVarName); homeVar != std::string::npos)
        {
            const auto homeFolder = GetHomeFolderPathLinux();
            if(!homeFolder)
            {
                TP_ERROR(TRAP::Log::FileSystemPrefix, "Failed to get documents folder path (failed to retrieve home directory path)!");
                return TRAP::NullOpt;
            }
            documentsDir = documentsDir.string().replace(homeVar, HomeVarName.size(), *homeFolder);
        }

        return documentsDir;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] bool OpenExternallyLinux(const std::filesystem::path& p)
    {
        ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        TRAP_ASSERT(!p.empty(), "FileSystem::OpenExternallyLinux(): Path is empty!");
        TRAP_ASSERT(TRAP::FileSystem::IsAbsolute(p), "FileSystem::OpenExternallyLinux(): Path is not absolute!");

        const std::string cmd = fmt::format("xdg-open {}", p.native());
        FILE* const xdg = popen(cmd.c_str(), "r");
        if(xdg == nullptr)
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open externally: ", p, "!");
            TP_ERROR(TRAP::Log::FileSystemPrefix, TRAP::Utils::String::GetStrError());
            return false;
        }
        pclose(xdg);

        return true;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Opens the file browser at the given path.
    /// @param p Path to folder to open in file browser.
    /// @return True on success, false otherwise.
    /// @remark @linux Linux uses xdg-open for this functionality.
    [[nodiscard]] bool OpenFolderInFileBrowserLinux(const std::filesystem::path& p)
    {
        ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        TRAP_ASSERT(!p.empty(), "FileSystem::OpenFolderInFileBrowserLinux(): Path is empty!");
        TRAP_ASSERT(TRAP::FileSystem::IsAbsolute(p), "FileSystem::OpenFolderInFileBrowserLinux(): Path is not absolute!");

        const std::string cmd = fmt::format("xdg-open {}", p.string());
        FILE* const xdg = popen(cmd.c_str(), "r");
        if(xdg == nullptr)
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open folder in file browser: ", p, "!");
            TP_ERROR(TRAP::Log::FileSystemPrefix, TRAP::Utils::String::GetStrError());
            return false;
        }
        pclose(xdg);

        return true;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Opens the file browser and selects the given file.
    /// @param p Path to file to open in file browser.
    /// @return True on success, false otherwise.
    /// @remark @linux Linux uses xdg-open for this functionality. The given file won't be selected.
    [[nodiscard]] bool OpenFileInFileBrowserLinux(const std::filesystem::path& p)
    {
        ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        TRAP_ASSERT(!p.empty(), "FileSystem::OpenFileInFileBrowserLinux(): Path is empty!");
        TRAP_ASSERT(TRAP::FileSystem::IsAbsolute(p), "FileSystem::OpenFileInFileBrowserLinux(): Path is not absolute!");

        const std::string cmd = fmt::format("xdg-open {}", p.parent_path().string());
        FILE* const xdg = popen(cmd.c_str(), "r");
        if(xdg == nullptr)
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open file in file browser: ", p, "!");
            TP_ERROR(TRAP::Log::FileSystemPrefix, TRAP::Utils::String::GetStrError());
            return false;
        }
        pclose(xdg);

        return true;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Retrieves the effective user's home dir.
    ///        If the user is running as root we ignore the HOME environment. It works badly with sudo.
    /// @return The home directory on success, empty optional.
    ///         HOME environment is respected for non-root users if it exists.
    /// @warning @linux Writing to $HOME as root implies security concerns that a multiplatform
    ///                 program cannot be assumed to handle.
    [[nodiscard]] TRAP::Optional<std::filesystem::path> GetHomeFolderPathLinux()
    {
        ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        static std::filesystem::path homeDir{};

        if(!homeDir.empty()) //Return cached home dir path
            return homeDir;

        //Non root user way
        const uid_t uid = getuid();
        if(const char* const homeEnv = getenv("HOME"); uid != 0 && (homeEnv != nullptr))
        {
            //We only acknowledge HOME if not root.
            homeDir = homeEnv;
        }
        else
        {
            //Root user way
            passwd* pw = nullptr;
            passwd pwd{};
            i64 bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
            if(bufSize <= 0)
                bufSize = 16384;
            std::vector<char> buffer(NumericCast<usize>(bufSize), '\0');
            const i32 errorCode = getpwuid_r(uid, &pwd, buffer.data(), buffer.size(), &pw);
            if(errorCode != 0)
            {
                TP_ERROR(TRAP::Log::FileSystemPrefix, "Failed to get home folder path (", errorCode, ")!");
                return TRAP::NullOpt;
            }
            const char* const tempRes = pw->pw_dir;
            if(tempRes == nullptr)
            {
                TP_ERROR(TRAP::Log::FileSystemPrefix, "Failed to get home folder path (", errorCode, ")!");
                return TRAP::NullOpt;
            }
            homeDir = tempRes;
        }

        return homeDir;
    }

    //-------------------------------------------------------------------------------------------------------------------//

#elif defined(TRAP_PLATFORM_WINDOWS)

    /// @brief Retrieves the user's document dir.
    /// @return The document directory on success, empty optional otherwise.
    [[nodiscard]] TRAP::Optional<std::filesystem::path> GetDocumentsFolderPathWindows()
    {
        ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        PWSTR path = nullptr;
        if(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &path) != S_OK || !path)
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't get documents folder path!");
            TP_ERROR(TRAP::Log::FileSystemPrefix, TRAP::Utils::String::GetStrError());
            return TRAP::NullOpt;
        }

        std::string folderPath = TRAP::Utils::String::CreateUTF8StringFromWideStringWin32(path);
        CoTaskMemFree(path); //Free path as required by SHGetKnownFolderPath()

        if(folderPath.empty())
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't get documents folder path (failed wide-string to utf-8 conversion)!");
            return TRAP::NullOpt;
        }

        folderPath.pop_back(); //Remove the extra null byte

        return folderPath;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    [[nodiscard]] bool OpenExternallyWindows(const std::filesystem::path& p)
    {
        ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        TRAP_ASSERT(!p.empty(), "FileSystem::OpenExternallyWindows(): Path is empty!");
        TRAP_ASSERT(TRAP::FileSystem::IsAbsolute(p), "FileSystem::OpenExternallyWindows(): Path is not absolute!");

        TRAP::Utils::Windows::COMInitializer comInitializer{};
        if(comInitializer.IsInitialized())
        {
            HINSTANCE res = ShellExecuteW(nullptr, L"open", p.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            if(std::bit_cast<INT_PTR>(res) <= 32)
            {
                TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open externally: ", p, "!");
                TP_ERROR(TRAP::Log::FileSystemPrefix, TRAP::Utils::String::GetStrError());
                return false;
            }
        }
        else
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open externally: ", p, " (COM initialization failed)!");
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Opens the file browser at the given path.
    /// @param p Path to folder to open in file browser.
    /// @return True on success, false otherwise.
    [[nodiscard]] bool OpenFolderInFileBrowserWindows(const std::filesystem::path& p)
    {
        ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        TRAP_ASSERT(!p.empty(), "FileSystem::OpenFolderInFileBrowserWindows(): Path is empty!");
        TRAP_ASSERT(TRAP::FileSystem::IsAbsolute(p), "FileSystem::OpenFolderInFileBrowserWindows(): Path is not absolute!");

        TRAP::Utils::Windows::COMInitializer comInitializer{};
        if(comInitializer.IsInitialized())
        {
            HINSTANCE res = ShellExecuteW(nullptr, L"explore", p.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            if(std::bit_cast<INT_PTR>(res) <= 32)
            {
                TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open folder in file browser: ", p, "!");
                TP_ERROR(TRAP::Log::FileSystemPrefix, TRAP::Utils::String::GetStrError());
                return false;
            }
        }
        else
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open folder in file browser: ", p, " (COM initialization failed)!");
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Opens the file browser and selects the given file.
    /// @param p Path to file to open in file browser.
    /// @return True on success, false otherwise.
    [[nodiscard]] bool OpenFileInFileBrowserWindows(const std::filesystem::path& p)
    {
        ZoneNamedC(__tracy, tracy::Color::Blue, (GetTRAPProfileSystems() & ProfileSystems::FileSystem) != ProfileSystems::None);

        TRAP_ASSERT(!p.empty(), "FileSystem::OpenFileInFileBrowserWindows(): Path is empty!");
        TRAP_ASSERT(TRAP::FileSystem::IsAbsolute(p), "FileSystem::OpenFileInFileBrowserWindows(): Path is not absolute!");

        const std::wstring openPath = p.wstring();

        TRAP::Utils::Windows::COMInitializer comInitializer{};
        if(comInitializer.IsInitialized())
        {
            LPITEMIDLIST pidl = nullptr;
            if (SHParseDisplayName(openPath.c_str(), nullptr, &pidl, 0, nullptr) != S_OK)
            {
                TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open file in file browser: ", p, "!");
                TP_ERROR(TRAP::Log::FileSystemPrefix, TRAP::Utils::String::GetStrError());
                return false;
            }

            const HRESULT res = SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
            if(res != S_OK)
            {
                TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open file in file browser: ", p, "!");
                TP_ERROR(TRAP::Log::FileSystemPrefix, TRAP::Utils::String::GetStrError());
                return false;
            }
        }
        else
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open file in file browser: ", p, " (COM initialization failed)!");
            return false;
        }

        return true;
    }

#endif
}
