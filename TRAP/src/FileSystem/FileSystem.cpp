#include "TRAPPCH.h"
#include "FileSystem.h"

#include "Utils/String/String.h"
#include "Application.h"

//-------------------------------------------------------------------------------------------------------------------//

bool OpenFolderInFileBrowser(const std::filesystem::path& p);
bool OpenFileInFileBrowser(const std::filesystem::path& p);

#ifdef TRAP_PLATFORM_LINUX
std::optional<std::filesystem::path> GetHomeFolderPathLinux();
std::optional<std::filesystem::path> GetDocumentsFolderPathLinux();
#endif

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FileSystem::Init()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

	TP_DEBUG(Log::FileSystemPrefix, "Initializing File System");

    //Create game temp folder
    const auto gameTempFolder = GetGameTempFolderPath();
    if(gameTempFolder && !Exists(*gameTempFolder))
    {
        if(!CreateFolder(*gameTempFolder))
            TP_ERROR(Log::FileSystemPrefix, "Failed to create game temp folder: \"", gameTempFolder->u8string(), "\"!");
    }

    //Create game document folder
    const auto gameDocsFolder = GetGameDocumentsFolderPath();
#ifndef TRAP_HEADLESS_MODE
    if(gameDocsFolder && !Exists(*gameDocsFolder))
    {
        if(!CreateFolder(*gameDocsFolder))
            TP_ERROR(Log::FileSystemPrefix, "Failed to create game documents folder: \"", gameDocsFolder->u8string(), "\"!");
    }
#endif

    //Create game log folder
    const auto gameLogFolder = GetGameLogFolderPath();
    if(gameLogFolder && !Exists(*gameLogFolder))
    {
        if(!CreateFolder(*gameLogFolder))
            TP_ERROR(Log::FileSystemPrefix, "Failed to create game log folder: \"", gameLogFolder->u8string(), "\"!");
    }
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::vector<uint8_t>> TRAP::FileSystem::ReadFile(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!path.empty(), "FileSystem::ReadFile(): Path is empty!");

    auto fileSize = GetSize(path);

    std::ifstream file(path, std::ios::binary);
    if(!file.is_open() || !file.good())
    {
		TP_ERROR(Log::FileSystemPrefix, "Couldn't read file: \"", path.u8string(), "\" (failed to open file)!");
        return std::nullopt;
    }

    if(!fileSize) //Fallback
    {
        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        file.seekg(0);
    }

    std::vector<uint8_t> buffer(*fileSize);

    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    file.close();

    return buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::string> TRAP::FileSystem::ReadTextFile(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!path.empty(), "FileSystem::ReadTextFile(): Path is empty!");

    auto fileSize = GetSize(path);

    std::ifstream file(path, std::ios::binary);
    if(!file.is_open() || !file.good())
    {
		TP_ERROR(Log::FileSystemPrefix, "Couldn't read file: \"", path.u8string(), "\" (failed to open file)!");
        return std::nullopt;
    }

    if(!fileSize)
    {
        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        file.seekg(0);
    }

    std::string result(*fileSize, '\0');

    file.read(result.data(), *fileSize);
    file.close();

    result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());

    return result;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::WriteFile(const std::filesystem::path& path, const std::vector<uint8_t>& buffer, const WriteMode mode)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!path.empty(), "FileSystem::WriteFile(): Path is empty!");

    const std::ios_base::openmode modeFlags = (mode == WriteMode::Overwrite) ? (std::ios::binary | std::ios::trunc) :
	                                                                           (std::ios::binary | std::ios::app);
    std::ofstream file(path, modeFlags);
    if(!file.is_open() || !file.good())
    {
	    TP_ERROR(Log::FileSystemPrefix, "Couldn't write file: \"", path.u8string(), "\" (failed to open file)!");
        return false;
    }

    file.write(reinterpret_cast<const char*>(buffer.data()), static_cast<int64_t>(buffer.size()));
    file.close();

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::WriteTextFile(const std::filesystem::path& path, const std::string_view text, const WriteMode mode)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!path.empty(), "FileSystem::WriteTextFile(): Path is empty!");

    const std::ios_base::openmode modeFlags = (mode == WriteMode::Overwrite) ? (std::ios::trunc) : (std::ios::app);
    std::ofstream file(path, modeFlags);
    if(!file.is_open() || !file.good())
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't write file: \"", path.u8string(), "\" (failed to open file)!");
        return false;
    }

    file.write(static_cast<const char*>(text.data()), static_cast<int64_t>(text.size()));
    file.close();

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::CreateFolder(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!path.empty(), "FileSystem::CreateFolder(): Path is empty!");

    if(Exists(path)) //Return true if folder already exists
        return true;

    std::error_code ec{};
    const bool res = std::filesystem::create_directories(path, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't create folder: \"", path.u8string(), "\" (", ec.message(), ")!");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::Delete(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!path.empty(), "FileSystem::Delete(): Path is empty!");

    std::error_code ec{};
    const bool res = std::filesystem::remove_all(path, ec) >= 1;
    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't delete file or folder: \"", path.u8string(),
                    "\" (", ec.message(), ")!");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::Move(const std::filesystem::path& oldPath, const std::filesystem::path& newPath)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!oldPath.empty(), "FileSystem::Move(): OldPath is empty!");
    TRAP_ASSERT(!newPath.empty(), "FileSystem::Move(): NewPath is empty!");

    std::error_code ec{};
    std::filesystem::rename(oldPath, newPath, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't move from: \"", oldPath.u8string(),
                 "\" to \"", newPath.u8string(), "\" (", ec.message(), ")!");
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::Copy(const std::filesystem::path& source, const std::filesystem::path& destination,
                            const bool overwriteExisting)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

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
        TP_ERROR(Log::FileSystemPrefix, "Couldn't copy folder: \"", source.u8string(), "\" to \"", destination.u8string(),
                 "\" (", ec.message(), ")!");
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::Rename(const std::filesystem::path& oldPath, const std::string_view newName)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!oldPath.empty(), "FileSystem::Rename(): OldFolderPath is empty!");
    TRAP_ASSERT(!newName.empty(), "FileSystem::Rename(): NewName is empty!");

    std::filesystem::path newPath = oldPath;
    try
    {
        newPath.replace_filename(newName);
    }
    catch(const std::exception& e)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't rename file or folder: \"", oldPath.u8string(), "\" to \"", newName,
                 "\" (", e.what(), ")!");
        return false;
    }

    return Move(oldPath, newPath);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::Exists(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!path.empty(), "FileSystem::Exists(): path is empty!");

    std::error_code ec{};
    const bool res = std::filesystem::exists(path, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if file or folder exists: \"", path.u8string(),
                 "\" (", ec.message(), ")!");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<uintmax_t> TRAP::FileSystem::GetSize(const std::filesystem::path& path, const bool recursive)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!path.empty(), "FileSystem::GetSize(): Path is empty!");

    //File
    if(IsFile(path))
    {
        std::error_code ec{};
        const uintmax_t size = std::filesystem::file_size(path, ec);

        if(ec)
        {
            TP_ERROR(Log::FileSystemPrefix, "Couldn't get file size of: \"", path.u8string(),
                     "\" (", ec.message(), ")!");
            return std::nullopt;
        }
        else if(size == static_cast<uintmax_t>(-1))
        {
            TP_ERROR(Log::FileSystemPrefix, "Couldn't get file size of: \"", path.u8string(),
                     "\" (size is -1)!");
            return std::nullopt;
        }

        return size;
    }

    //Folder
    if(IsFolder(path))
    {
        std::error_code ec{};
        uintmax_t size = 0;
        bool res = false;
        if(recursive)
        {
            const std::filesystem::recursive_directory_iterator rDIt(path, ec);
            if(ec)
            {
                TP_ERROR(Log::FileSystemPrefix, "Couldn't get file size of: \"", path.u8string(),
                        "\" (", ec.message(), ")!");
                return std::nullopt;
            }

            for(const auto& entry : rDIt)
            {
                res = entry.is_regular_file(ec);
                if(ec)
                {
                    TP_ERROR(Log::FileSystemPrefix, "Couldn't get file size of: \"", entry.path().u8string(),
                            "\" (", ec.message(), ")!");
                    return std::nullopt;
                }
                if(res)
                {
                    const uintmax_t fileSize = entry.file_size(ec);
                    if(ec)
                    {
                        TP_ERROR(Log::FileSystemPrefix, "Couldn't get file size of: \"", entry.path().u8string(),
                                "\" (", ec.message(), ")!");
                        return std::nullopt;
                    }
                    else if(fileSize == static_cast<uintmax_t>(-1))
                    {
                        TP_ERROR(Log::FileSystemPrefix, "Couldn't get file size of: \"", entry.path().u8string(),
                                "\" (size is -1)!");
                        return std::nullopt;
                    }
                    size += fileSize;
                }
            }
        }
        else /*if(!recursive)*/
        {
            const std::filesystem::directory_iterator dIt(path, ec);
            if(ec)
            {
                TP_ERROR(Log::FileSystemPrefix, "Couldn't get file size of: \"", path.u8string(),
                        "\" (", ec.message(), ")!");
                return std::nullopt;
            }

            for(const auto& entry : dIt)
            {
                res = entry.is_regular_file(ec);
                if(ec)
                {
                    TP_ERROR(Log::FileSystemPrefix, "Couldn't get file size of: \"", entry.path().u8string(),
                            "\" (", ec.message(), ")!");
                    return std::nullopt;
                }
                if(res)
                {
                    const uintmax_t fileSize = entry.file_size(ec);
                    if(ec)
                    {
                        TP_ERROR(Log::FileSystemPrefix, "Couldn't get file size of: \"", entry.path().u8string(),
                                "\" (", ec.message(), ")!");
                        return std::nullopt;
                    }
                    else if(fileSize == static_cast<uintmax_t>(-1))
                    {
                        TP_ERROR(Log::FileSystemPrefix, "Couldn't get file size of: \"", entry.path().u8string(),
                                "\" (size is -1)!");
                        return std::nullopt;
                    }
                    size += fileSize;
                }
            }
        }

        return size;
    }

    return std::nullopt;
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::filesystem::file_time_type> TRAP::FileSystem::GetLastWriteTime(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!path.empty(), "FileSystem::GetLastWriteTime(): Path is empty!");

    std::error_code ec{};
    const auto res = std::filesystem::last_write_time(path, ec);
    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get last write time: \"", path.u8string(),
                 "\" (", ec.message(), ")!");
        return std::nullopt;
    }
    else if(res == std::filesystem::file_time_type::min())
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get last write time: \"", path.u8string(),
                 "\" (time is min)!");
        return std::nullopt;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::string> TRAP::FileSystem::GetFileNameWithEnding(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!path.empty(), "FileSystem::GetFileNameWithEnding(): Path is empty!");

    std::string res{};
    try //filename() may throw implementation defined exceptions
    {
        res = path.filename().u8string();
    }
    catch(const std::exception& e)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get file name with ending: \"", path.u8string(),
                 "\" (", e.what(), ")!");
        return std::nullopt;
    }

    if(res.empty())
        return std::nullopt;

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::string> TRAP::FileSystem::GetFileNameWithoutEnding(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!path.empty(), "FileSystem::GetFileNameWithoutEnding(): Path is empty!");

    std::string res{};
    try //stem() may throw implementation defined exceptions
    {
        res = path.stem().u8string();
    }
    catch(const std::exception& e)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get file name: \"", path.u8string(),
                 "\" (", e.what(), ")!");
        return std::nullopt;
    }

    if(res.empty())
        return std::nullopt;

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::string> TRAP::FileSystem::GetFileEnding(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!path.empty(), "FileSystem::GetFileEnding(): Path is empty!");

    std::string res{};
    try //extension() may throw implementation defined exceptions
    {
        res = path.extension().u8string();
    }
    catch(const std::exception& e)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get file ending: \"", path.u8string(),
                 "\" (", e.what(), ")!");
        return std::nullopt;
    }

    if(res.empty())
        return std::nullopt;

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::filesystem::path> TRAP::FileSystem::GetFolderPath(const std::filesystem::path& filePath)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!filePath.empty(), "FileSystem::GetFolderPath(): Path is empty!");

    std::filesystem::path res{};
    try //parent_path() may throw implementation defined exceptions
    {
        res = filePath.parent_path();
    }
    catch(const std::exception& e)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get folder path: \"", filePath.u8string(),
                 "\" (", e.what(), ")!");
        return std::nullopt;
    }

    if(res.empty())
        return std::nullopt;

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::filesystem::path> TRAP::FileSystem::GetTempFolderPath()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    std::error_code ec{};
    const std::filesystem::path path = std::filesystem::temp_directory_path(ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get temp directory path (", ec.message(), ")!");
        return std::nullopt;
    }

    return (path / "TRAP");
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::filesystem::path> TRAP::FileSystem::GetGameTempFolderPath()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    const auto tempFolder = GetTempFolderPath();
    if(!tempFolder)
        return std::nullopt;

    return (*tempFolder / TRAP::Application::GetGameName());
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::filesystem::path> TRAP::FileSystem::GetCurrentFolderPath()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    std::error_code ec{};
    const std::filesystem::path path = std::filesystem::current_path(ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get current directory path (", ec.message(), ")!");
        return std::nullopt;
    }

    return path;
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::filesystem::path> TRAP::FileSystem::GetDocumentsFolderPath()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

#ifdef TRAP_PLATFORM_WINDOWS
    PWSTR path = nullptr;
    if(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &path) != S_OK || !path)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get documents folder path!");
        TP_ERROR(Log::FileSystemPrefix, Utils::String::GetStrError());
        return std::nullopt;
    }

    std::string folderPath = TRAP::Utils::String::CreateUTF8StringFromWideStringWin32(path);
    CoTaskMemFree(path); //Free path as required by SHGetKnownFolderPath()

    if(folderPath.empty())
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get documents folder path (failed wide-string to utf-8 conversion)!");
        return std::nullopt;
    }

    folderPath.pop_back(); //Remove the extra null byte

    return folderPath;
#elif defined(TRAP_PLATFORM_LINUX)
    return GetDocumentsFolderPathLinux();
#else
    assert(false, "GetDocumentsFolderPath() not implemented!");
    return std::nullopt;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::filesystem::path> TRAP::FileSystem::GetGameDocumentsFolderPath()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

	const auto docsFolder = GetDocumentsFolderPath();
    if(!docsFolder)
        return std::nullopt;

    return (*docsFolder / "TRAP" / TRAP::Application::GetGameName());
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::filesystem::path> TRAP::FileSystem::GetGameLogFolderPath()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

#ifndef TRAP_HEADLESS_MODE
    const auto docsFolder = GetGameDocumentsFolderPath();
    if(!docsFolder)
        return std::nullopt;

    return (*docsFolder / "logs");
#else
    return "logs";
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::IsEquivalent(const std::filesystem::path& p1, const std::filesystem::path& p2)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!p1.empty(), "FileSystem::IsEquivalent(): Path1 is empty!");
    TRAP_ASSERT(!p2.empty(), "FileSystem::IsEquivalent(): Path2 is empty!");

    std::error_code ec{};
    const bool res = std::filesystem::equivalent(p1, p2, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Error while checking if paths are equivalent: \"", p1.u8string(), "\" and \"",
                 p2.u8string(), "\" (", ec.message(), ")!");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::IsFolder(const std::filesystem::path& p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!p.empty(), "FileSystem::IsFolder(): Path is empty!");

    std::error_code ec{};
    const bool res = std::filesystem::is_directory(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if path leads to a folder: \"", p.u8string(), "\" (", ec.message(), ")!");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::IsFile(const std::filesystem::path& p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!p.empty(), "FileSystem::IsFile(): Path is empty!");

    std::error_code ec{};
    const bool res = std::filesystem::is_regular_file(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if path leads to a regular file: \"", p.u8string(), "\" (", ec.message(), ")!");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::IsEmpty(const std::filesystem::path& p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!p.empty(), "FileSystem::IsEmpty(): Path is empty!");

    std::error_code ec{};
    const bool res = std::filesystem::is_empty(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if path leads to an empty file or folder: \"", p.u8string(), "\" (", ec.message(), ")!");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::IsAbsolute(const std::filesystem::path& p)
{
    ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!p.empty(), "FileSystem::IsAbsolute(): Path is empty!");

    bool res = false;

    try
    {
        res = p.is_absolute();
    }
    catch(const std::exception& e)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if path is absolute: \"", p.u8string(), "\" (", e.what(), ")!");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::IsRelative(const std::filesystem::path& p)
{
    ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!p.empty(), "FileSystem::IsRelative(): Path is empty!");

    bool res = false;

    try
    {
        res = p.is_relative();
    }
    catch(const std::exception& e)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if path is relative: \"", p.u8string(), "\" (", e.what(), ")!");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::filesystem::path> TRAP::FileSystem::ToAbsolutePath(const std::filesystem::path& p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!p.empty(), "FileSystem::ToAbsolutePath(): Path is empty!");

    std::error_code ec{};
    const std::filesystem::path res = std::filesystem::absolute(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Error while converting path to absolute: \"", p.u8string(), "\" (", ec.message(), ")!");
        return std::nullopt;
    }

    if(res.empty())
        return std::nullopt;

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

std::optional<std::filesystem::path> TRAP::FileSystem::ToRelativePath(const std::filesystem::path& p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!p.empty(), "FileSystem::ToRelativePath(): Path is empty!");

    std::error_code ec{};
    const std::filesystem::path res = std::filesystem::proximate(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Error while converting path to relative: \"", p.u8string(), "\" (", ec.message(), ")!");
        return std::nullopt;
    }

    if(res.empty())
        return std::nullopt;

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::SetCurrentFolderPath(const std::filesystem::path &p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!p.empty(), "FileSystem::SetCurrentFolderPath(): Path is empty!");

    std::error_code ec{};
    std::filesystem::current_path(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Error while settings current path: \"", p.u8string(), "\" (", ec.message(), ")!");
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::OpenInFileBrowser(const std::filesystem::path& p)
{
    TRAP_ASSERT(!p.empty(), "FileSystem::OpenInFileBrowser(): Path is empty!");

	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    if(!Exists(p))
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't open file/folder in file browser: \"", p.u8string(), "\" (file/folder doesn't exist)!");
        return false;
    }
    if(IsFile(p))
        return OpenFileInFileBrowser(p);
    if(IsFolder(p))
        return OpenFolderInFileBrowser(p);

    TP_ERROR(Log::FileSystemPrefix, "Couldn't open file/folder in file browser: \"", p.u8string(), "\" (path doesn't lead to a file/folder)!");
    return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::OpenExternally(const std::filesystem::path& p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!p.empty(), "FileSystem::OpenExternally(): Path is empty!");

    if(!Exists(p))
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't open externally: \"", p.u8string(), "\" (file/folder doesn't exist)!");
        return false;
    }

    auto absPath = ToAbsolutePath(p);
    if(!absPath)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't open externally: \"", p.u8string(), "\" (failed to convert path to absolute format)!");
        return false;
    }

#ifdef TRAP_PLATFORM_WINDOWS
    absPath = absPath->make_preferred(); //Replaces all "/" with "\"

    HINSTANCE res{};
    {
        TRAP::Utils::Windows::COMInitializer comInitializer{};
        if(comInitializer.IsInitialized())
        {
            res = ShellExecuteW(nullptr, L"open", absPath->c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            if(reinterpret_cast<const INT_PTR>(res) <= 32)
            {
                TP_ERROR(Log::FileSystemPrefix, "Couldn't open externally: \"", p.u8string(), "\"!");
                TP_ERROR(Log::FileSystemPrefix, Utils::String::GetStrError());
            }
        }
        else
        {
            TP_ERROR(Log::FileSystemPrefix, "Couldn't open externally: \"", p.u8string(), "\" (COM initialization failed)!");
            return false;
        }
    }

    return reinterpret_cast<const INT_PTR>(res) > 32;
#elif defined(TRAP_PLATFORM_LINUX)
    std::string cmd = "xdg-open ";
    cmd += absPath->native();
    FILE* const xdg = popen(cmd.c_str(), "r");
    if(!xdg)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't open externally: \"", p.u8string(), "\"!");
        TP_ERROR(Log::FileSystemPrefix, Utils::String::GetStrError());
        return false;
    }
    pclose(xdg);

    return true;
#else
    assert(false, "OpenExternally() not implemented!");
    return false;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//
//INTERNAL-----------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Opens the file browser at the given path.
///
/// Note: Linux uses xdg-open.
/// </summary>
/// <param name="p">Path to folder to open in file browser.</param>
/// <returns>True on success, false otherwise.</returns>
bool OpenFolderInFileBrowser(const std::filesystem::path& p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!p.empty(), "OpenFolderInFileBrowser(): Path is empty!");

    auto absPath = TRAP::FileSystem::ToAbsolutePath(p);
    if(!absPath)
    {
        TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open folder in file browser: \"", p.u8string(), "\" (failed to convert path to absolute format)!");
        return false;
    }

#ifdef TRAP_PLATFORM_WINDOWS
    absPath = absPath->make_preferred(); //Replaces all "/" with "\"

    HINSTANCE res{};
    {
        TRAP::Utils::Windows::COMInitializer comInitializer{};
        if(comInitializer.IsInitialized())
        {
            res = ShellExecuteW(nullptr, L"explore", absPath->c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            if(reinterpret_cast<const INT_PTR>(res) <= 32)
            {
                TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open folder in file browser: \"", p.u8string(), "\"!");
                TP_ERROR(TRAP::Log::FileSystemPrefix, TRAP::Utils::String::GetStrError());
            }
        }
        else
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open folder in file browser: \"", p.u8string(), "\" (COM initialization failed)!");
            return false;
        }
    }

    return reinterpret_cast<const INT_PTR>(res) > 32;
#elif defined(TRAP_PLATFORM_LINUX)
    std::string cmd = "xdg-open ";
    cmd += (*absPath).u8string();
    FILE* const xdg = popen(cmd.c_str(), "r");
    if(!xdg)
    {
        TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open folder in file browser: \"", p.u8string(), "\"!");
        TP_ERROR(TRAP::Log::FileSystemPrefix, TRAP::Utils::String::GetStrError());
        return false;
    }
    pclose(xdg);

    return true;
#else
    assert(false, "OpenFolderInFileBrowser() not implemented!");
    return false;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Opens the file browser and selects the given file.
///
/// Note: On Linux this function does't select the given file.
/// Note: Linux uses xdg-open.
/// </summary>
/// <param name="p">Path to file to open in file browser.</param>
/// <returns>True on success, false otherwise.</returns>
bool OpenFileInFileBrowser(const std::filesystem::path& p)
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    TRAP_ASSERT(!p.empty(), "OpenFileInFileBrowser(): Path is empty!");

    auto absPath = TRAP::FileSystem::ToAbsolutePath(p);
    if(!absPath)
    {
        TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open file in file browser: \"", p.u8string(), "\" (failed to convert path to absolute format)!");
        return false;
    }

#ifdef TRAP_PLATFORM_WINDOWS
    const std::wstring openPath = absPath->wstring();

    HRESULT res{};
    {
        TRAP::Utils::Windows::COMInitializer comInitializer{};
        if(comInitializer.IsInitialized())
        {
            LPITEMIDLIST pidl = nullptr;
            if (SHParseDisplayName(openPath.c_str(), nullptr, &pidl, 0, nullptr) != S_OK)
            {
                TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open file in file browser: \"", p.u8string(), "\"!");
                TP_ERROR(TRAP::Log::FileSystemPrefix, TRAP::Utils::String::GetStrError());
                return false;
            }

            const HRESULT res = SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
            if(res != S_OK)
            {
                TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open file in file browser: \"", p.u8string(), "\"!");
                TP_ERROR(TRAP::Log::FileSystemPrefix, TRAP::Utils::String::GetStrError());
            }
        }
        else
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open file in file browser: \"", p.u8string(), "\" (COM initialization failed)!");
            return false;
        }
    }

    return res == S_OK;
#elif defined(TRAP_PLATFORM_LINUX)
    std::string cmd = "xdg-open ";
    cmd += absPath->parent_path().u8string();
    FILE* const xdg = popen(cmd.c_str(), "r");
    if(!xdg)
    {
        TP_ERROR(TRAP::Log::FileSystemPrefix, "Couldn't open file in file browser: \"", p.u8string(), "\"!");
        TP_ERROR(TRAP::Log::FileSystemPrefix, TRAP::Utils::String::GetStrError());
        return false;
    }
    pclose(xdg);

    return true;
#else
    assert(false, "OpenFileInFileBrowser() not implemented!");
    return false;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_LINUX

/// <summary>
/// Retrieves the effective user's home dir.
/// If the user is running as root we ignore the HOME environment.
/// It works badly with sudo.
///
/// Note: Writing to $HOME as root implies security concerns that a multiplatform program
///       cannot be assumed to handle.
/// </summary>
/// <returns>
/// The home directory on success, empty optional.
/// HOME environment is respected for non-root users if it exists.
/// </returns>
std::optional<std::filesystem::path> GetHomeFolderPathLinux()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    static std::filesystem::path homeDir{};

    if(!homeDir.empty()) //Return cached home dir path
        return homeDir;

    //Non root user way
    const uid_t uid = getuid();
    const char* const homeEnv = getenv("HOME");
    if(uid != 0 && homeEnv)
    {
        //We only acknowledge HOME if not root.
        homeDir = homeEnv;
        return homeDir;
    }

    //Root user way
    passwd* pw = nullptr;
    passwd pwd{};
    int64_t bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if(bufSize <= 0)
        bufSize = 16384;
    std::vector<char> buffer(bufSize, '\0');
    const int32_t errorCode = getpwuid_r(uid, &pwd, buffer.data(), buffer.size(), &pw);
    if(errorCode)
    {
        TP_ERROR(TRAP::Log::FileSystemPrefix, "Failed to get home folder path (", errorCode, ")!");
        homeDir = std::filesystem::path{};
        return std::nullopt;
    }
    const char* const tempRes = pw->pw_dir;
    if(!tempRes)
    {
        TP_ERROR(TRAP::Log::FileSystemPrefix, "Failed to get home folder path (", errorCode, ")!");
        homeDir = std::filesystem::path{};
        return std::nullopt;
    }
    homeDir = tempRes;

    return homeDir;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Retrieves the effective user's document dir.
/// If the user is running as root we ignore the HOME environment.
/// It works badly with sudo.
///
/// Note: Writing to $HOME as root implies security concerns that a multiplatform program
///       cannot be assumed to handle.
/// </summary>
/// <returns>
/// The document directory on success, empty optional otherwise.
/// HOME environment is respected for non-root users if it exists.
/// </returns>
std::optional<std::filesystem::path> GetDocumentsFolderPathLinux()
{
	ZoneNamedC(__tracy, tracy::Color::Blue, TRAP_PROFILE_SYSTEMS() & ProfileSystems::FileSystem);

    static std::filesystem::path documentsDir{};

    if(!documentsDir.empty()) //Return cached docs dir path
        return documentsDir;

    documentsDir = "$HOME/Documents";

    //Get config folder
    const char* const tempRes = getenv("XDG_CONFIG_HOME");
    std::filesystem::path configPath{};
    if(tempRes)
        configPath = tempRes;
    else
    {
        const auto homeFolder = GetHomeFolderPathLinux();
        if(!homeFolder)
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Failed to get documents folder path (failed to retrieve home directory path)!");
            return std::nullopt;
        }
        configPath = *homeFolder / ".config";
    }
    configPath /= "user-dirs.dirs";

    //Get Documents folder
    std::ifstream file(configPath);
    if(file.is_open() && file.good())
    {
        std::string line;
        while(std::getline(file, line))
        {
            //Skip invalid entries and comments
            if(line.empty() || line[0] == '#' || line.substr(0, 4) != "XDG_" || line.find("_DIR") == std::string::npos)
                continue;
            const std::size_t splitPos = line.find('=');
            if(splitPos == std::string::npos)
                continue;
            const std::string key = line.substr(0, splitPos);
            if(key != "XDG_DOCUMENTS_DIR") //Only interested in documents folder
                continue;
            const std::size_t valueStart = line.find('"', splitPos);
            if(valueStart == std::string::npos)
                continue;
            const std::size_t valueEnd = line.find('"', valueStart + 1);
            if(valueEnd == std::string::npos)
                continue;
            documentsDir = line.substr(valueStart + 1, valueEnd - valueStart - 1);
            break;
        }
    }
    else
    {
        TP_ERROR(TRAP::Log::FileSystemPrefix, "Failed to get documents folder path (failed to open HOME/.config/user-dirs.dirs)!");
        return std::nullopt;
    }
    file.close();

    if(documentsDir.u8string().compare(0, 5, "$HOME") == 0)
    {
        const auto homeFolder = GetHomeFolderPathLinux();
        if(!homeFolder)
        {
            TP_ERROR(TRAP::Log::FileSystemPrefix, "Failed to get documents folder path (failed to retrieve home directory path)!");
            return std::nullopt;
        }
        documentsDir = (*homeFolder).u8string() + documentsDir.u8string().substr(5, std::string::npos);
    }

    return documentsDir;
}
#endif
