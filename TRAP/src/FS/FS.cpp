#include "TRAPPCH.h"
#include "FS.h"

//-------------------------------------------------------------------------------------------------------------------//

std::string GetHomeFolderPathLinux();
std::string GetDocumentsFolderPathLinux();

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FS::Init()
{
    TP_PROFILE_FUNCTION();

	TP_DEBUG(Log::FileSystemPrefix, "Initializing File System");

    std::error_code ec;
    std::filesystem::path p = GetTempFolderPath() / "TRAP" / TRAP::Application::GetGameName();
    bool res = std::filesystem::exists(p, ec);
    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if temp directory exists: \"", p, "\" (", ec.message(), ")");
        return;
    }
    if(!res)
    {
        res = std::filesystem::create_directories(p, ec);
        if(ec || !res)
        {
            TP_ERROR(Log::FileSystemPrefix, "Couldn't create temp directory: \"", p, "\"",
                     (res ? "" : (" (" + ec.message() + ")")));
            return;
        }
    }

    p = GetDocumentsFolderPath() / "TRAP" / TRAP::Application::GetGameName();
    res = std::filesystem::exists(p, ec);
    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if games document directory exists: \"", p, "\" (", ec.message(), ")");
        return;
    }
    if(!res)
    {
        res = std::filesystem::create_directories(p, ec);
        if(ec || !res)
        {
            TP_ERROR(Log::FileSystemPrefix, "Couldn't create games document directory: \"", p, "\"",
                     (res ? "" : (" (" + ec.message() + ")")));
            return;
        }
    }

    p = GetDocumentsFolderPath() / "TRAP" / TRAP::Application::GetGameName() / "logs";
    res = std::filesystem::exists(p, ec);
    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if games log directory exists: \"", p, "\" (", ec.message(), ")");
        return;
    }
    if(!res)
    {
        res = std::filesystem::create_directories(p);
        if(ec || !res)
        {
            TP_ERROR(Log::FileSystemPrefix, "Couldn't create games log directory: \"", p, "\"",
                     (res ? "" : (" (" + ec.message() + ")")));
            return;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FS::Shutdown()
{
    TP_PROFILE_FUNCTION();

	TP_DEBUG(Log::FileSystemPrefix, "Shutting down File System");
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::FS::ReadFile(const std::filesystem::path& path)
{
    TP_PROFILE_FUNCTION();

    if(!FileOrFolderExists(path))
        return {};

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if(!file.is_open() || !file.good())
    {
		TP_ERROR(Log::FileSystemPrefix, "Couldn't open file: \"", path.generic_u8string(), "\"");
        return {};
    }

    const int64_t length = file.tellg();
    file.seekg(0);
    std::vector<uint8_t> buffer(length);
    file.read(reinterpret_cast<char*>(buffer.data()), length);
    file.close();

    return buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::FS::ReadTextFile(const std::filesystem::path& path)
{
    TP_PROFILE_FUNCTION();

    if(!FileOrFolderExists(path))
        return "";

    std::ifstream file(path);
    if(!file.is_open() || !file.good())
    {
		TP_ERROR(Log::FileSystemPrefix, "Couldn't open file: \"", path.generic_u8string(), "\"");
        return "";
    }

    std::string line;
    std::string result;
    while (file.good() && std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        result += line + '\n';
    }
    file.close();

    return result;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FS::WriteFile(const std::filesystem::path& path, std::vector<uint8_t>& buffer, const WriteMode mode)
{
    TP_PROFILE_FUNCTION();

    if(path.empty() || buffer.empty())
        return false;

    const std::ios_base::openmode modeFlags = (mode == WriteMode::Overwrite) ? (std::ios::binary | std::ios::trunc) :
	                                                                           (std::ios::binary | std::ios::ate);
    std::ofstream file(path, modeFlags);
    if(!file.is_open() || !file.good())
    {
	    TP_ERROR(Log::FileSystemPrefix, "Couldn't write file: \"", path.generic_u8string(), "\"");
        return false;
    }

    file.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
    file.close();

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FS::WriteTextFile(const std::filesystem::path& path, const std::string_view text, const WriteMode mode)
{
    TP_PROFILE_FUNCTION();

    if(path.empty() || text.empty())
        return false;

    std::ofstream file;
    if(mode == WriteMode::Append)
        file.open(path, std::ios::ate);
    else
        file.open(path);
    if(!file.is_open() || !file.good())
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't write file: \"", path.generic_u8string(), "\"");
        return false;
    }

    file << text;
    file.close();

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FS::CreateFolder(const std::filesystem::path& path)
{
    TP_PROFILE_FUNCTION();

    if(path.empty())
        return false;

    std::error_code ec;
    bool res = std::filesystem::create_directories(path, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't create folder: \"", path.generic_u8string(), "\" (", ec.message(), ")");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FS::DeleteFileOrFolder(const std::filesystem::path& path)
{
    TP_PROFILE_FUNCTION();

    if(path.empty())
        return false;

    if(!FileOrFolderExists(path))
        return false;

    std::error_code ec;
    if(std::filesystem::is_directory(path, ec)) //Directory
    {
        if(ec)
        {
            TP_ERROR(Log::FileSystemPrefix, "Couldn't delete file or folder: \"", path.generic_u8string(),
                     "\" (", ec.message(), ")");
            return false;
        }

        bool res = std::filesystem::remove_all(path, ec) > 1;

        if(ec)
        {
            TP_ERROR(Log::FileSystemPrefix, "Couldn't delete file or folder: \"", path.generic_u8string(),
                     "\" (", ec.message(), ")");
            return false;
        }

        return res;
    }

    //File
    bool res = std::filesystem::remove(path, ec);
    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't delete file or folder: \"", path.generic_u8string(),
                 "\" (", ec.message(), ")");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FS::MoveFolder(const std::filesystem::path& oldPath, const std::filesystem::path& newPath)
{
    if(FileOrFolderExists(newPath))
        return false;

    std::error_code ec;
    std::filesystem::rename(oldPath, newPath, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't move from: \"", oldPath.generic_u8string(),
                 "\" to \"", newPath.generic_u8string(), "\"(", ec.message(), ")");
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FS::MoveFile(const std::filesystem::path& filePath, const std::filesystem::path& destFolder)
{
    return MoveFolder(filePath, destFolder / filePath.filename());
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FS::RenameFolder(const std::filesystem::path& oldPath, const std::filesystem::path& newPath)
{
    return MoveFolder(oldPath, newPath);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FS::RenameFile(const std::filesystem::path& oldPath, const std::string& newName)
{
    std::filesystem::path newPath = oldPath.parent_path() / newName / oldPath.extension();
    return RenameFolder(oldPath, newPath);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FS::FileOrFolderExists(const std::filesystem::path& path)
{
    TP_PROFILE_FUNCTION();

    if(path.empty())
        return false;

    std::error_code ec;
    bool res = std::filesystem::exists(path, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if file or folder exists: \"", path.generic_u8string(),
                 "\" (", ec.message(), ")");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

uintmax_t TRAP::FS::GetFileOrFolderSize(const std::filesystem::path& path, const bool recursive)
{
    TP_PROFILE_FUNCTION();

    if(!FileOrFolderExists(path))
        return 0;

    uintmax_t size = 0;
    std::error_code ec;
    bool res = std::filesystem::is_directory(path, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if path is a directory: \"", path.generic_u8string(),
                 "\" (", ec.message(), ")");
        return 0;
    }
    if(!res)
        return 0;

    const std::filesystem::recursive_directory_iterator rDIt(path, ec);
    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't create recursive directory iterator: \"", path.generic_u8string(),
                 "\" (", ec.message(), ")");
        return 0;
    }
    const std::filesystem::directory_iterator dIt(path, ec);
    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't create directory iterator: \"", path.generic_u8string(),
                 "\" (", ec.message(), ")");
        return 0;
    }

    if(recursive)
    {
        for(const auto& entry : rDIt)
        {
            res = entry.is_regular_file(ec);
            if(ec)
            {
                TP_ERROR(Log::FileSystemPrefix, "Couldn't check if path is a regular file: \"", entry.path().generic_u8string(),
                         "\" (", ec.message(), ")");
                return 0;
            }
            if(res)
            {
                std::uintmax_t fileSize = entry.file_size(ec);
                if(ec)
                {
                    TP_ERROR(Log::FileSystemPrefix, "Couldn't get file size: \"", entry.path().generic_u8string(),
                            "\" (", ec.message(), ")");
                    return 0;
                }
                size += fileSize;
            }
        }
    }
    else
    {
        for(const auto& entry : dIt)
        {
            res = entry.is_regular_file(ec);
            if(ec)
            {
                TP_ERROR(Log::FileSystemPrefix, "Couldn't check if path is a regular file: \"", entry.path().generic_u8string(),
                         "\" (", ec.message(), ")");
                return 0;
            }
            if(res)
            {
                std::uintmax_t fileSize = entry.file_size(ec);
                if(ec)
                {
                    TP_ERROR(Log::FileSystemPrefix, "Couldn't get file size: \"", entry.path().generic_u8string(),
                            "\" (", ec.message(), ")");
                    return 0;
                }
                size += fileSize;
            }
        }
    }

    return size;
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::file_time_type TRAP::FS::GetLastWriteTime(const std::filesystem::path& path)
{
    TP_PROFILE_FUNCTION();

    if(!FileOrFolderExists(path))
        return std::filesystem::file_time_type::min();

    std::error_code ec;
    auto res = std::filesystem::last_write_time(path, ec);
    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get last write time: \"", path.generic_u8string(),
                 "\" (", ec.message(), ")");
        return std::filesystem::file_time_type::min();
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::FS::GetFileNameWithEnding(const std::filesystem::path& path)
{
    TP_PROFILE_FUNCTION();

    if(path.empty() || !path.has_filename())
        return "";

    return path.filename().generic_u8string();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::FS::GetFileName(const std::filesystem::path& path)
{
    TP_PROFILE_FUNCTION();

    if(path.empty() || !path.has_stem())
        return "";

    return path.stem().generic_u8string();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::FS::GetFileEnding(const std::filesystem::path& path)
{
    TP_PROFILE_FUNCTION();

    if(path.empty() || !path.has_extension())
        return "";

    return path.extension().generic_u8string();
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::path TRAP::FS::GetFolderPath(const std::filesystem::path& filePath)
{
    TP_PROFILE_FUNCTION();

    if (filePath.empty())
        return {};

    return filePath.parent_path().generic_u8string();
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::path TRAP::FS::GetTempFolderPath()
{
    std::error_code ec;
    std::filesystem::path path = std::filesystem::temp_directory_path(ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get temp directory path: \"", path, "\" (", ec.message(), ")");
        return {};
    }

    return path.generic_u8string();
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::path TRAP::FS::GetGameTempFolderPath()
{
    return GetTempFolderPath() / "TRAP" / TRAP::Application::GetGameName();
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::path TRAP::FS::GetCurrentFolderPath()
{
    std::error_code ec;
    std::filesystem::path path = std::filesystem::current_path(ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't get current directory path: \"", path, "\" (", ec.message(), ")");
        return {};
    }

    return path.generic_u8string();
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::path TRAP::FS::GetDocumentsFolderPath()
{
    std::string folderPath{};

#ifdef TRAP_PLATFORM_WINDOWS
    PWSTR path = nullptr;
    if(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &path) != S_OK)
        return "";

    const int32_t count = WideCharToMultiByte(CP_UTF8, 0, path, -1, nullptr, 0, nullptr, nullptr);
	if (!count)
		return {};

	folderPath.resize(count);

	if (!WideCharToMultiByte(CP_UTF8, 0, path, -1, folderPath.data(), static_cast<int32_t>(folderPath.size()), nullptr, nullptr))
		return {};

	CoTaskMemFree(path);
    folderPath.pop_back(); //Remove the extra null byte
#elif defined(TRAP_PLATFORM_LINUX)
    folderPath = GetDocumentsFolderPathLinux();
#endif

    return std::filesystem::path(folderPath).generic_u8string();
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::path TRAP::FS::GetGameDocumentsFolderPath()
{
	const std::filesystem::path path = GetDocumentsFolderPath();

    if(path.empty())
        return {};

    return path / "TRAP" / TRAP::Application::GetGameName();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FS::IsPathEquivalent(const std::filesystem::path& p1, const std::filesystem::path& p2)
{
    std::error_code ec;
    bool res = std::filesystem::equivalent(p1, p2, ec);

    if(ec)
        TP_ERROR(Log::FileSystemPrefix, "Error while checking if path is equivalent: ", ec.message());

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FS::IsPathAbsolute(const std::filesystem::path& p)
{
    try
    {
        return p.is_absolute();

    }
    catch(const std::exception& e)
    {

        TP_ERROR(Log::FileSystemPrefix, "Error while checking if path is absolute: ", e.what());
        return false;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FS::IsPathRelative(const std::filesystem::path& p)
{
    try
    {
        return p.is_relative();

    }
    catch(const std::exception& e)
    {

        TP_ERROR(Log::FileSystemPrefix, "Error while checking if path is relative: ", e.what());
        return false;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FS::IsFolder(const std::filesystem::path& p)
{
    std::error_code ec;
    bool res = std::filesystem::is_directory(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if path leads to a folder: \"", p, "\" (", ec.message(), ")");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FS::IsFile(const std::filesystem::path& p)
{
    std::error_code ec;
    bool res = std::filesystem::is_regular_file(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Couldn't check if path leads to a regular file: \"", p, "\" (", ec.message(), ")");
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::path TRAP::FS::ToAbsolutePath(const std::filesystem::path& p)
{
    if(p.is_absolute())
        return p;

    std::error_code ec;
    std::filesystem::path res = std::filesystem::absolute(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Error while converting path to absolute: ", ec.message());
        return {};
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::path TRAP::FS::ToRelativePath(const std::filesystem::path& p)
{
    if(p.is_relative())
        return p;

    std::error_code ec;
    std::filesystem::path res = std::filesystem::proximate(p, ec);

    if(ec)
    {
        TP_ERROR(Log::FileSystemPrefix, "Error while converting path to relative: ", ec.message());
        return {};
    }

    return res;
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
/// The home directory.
/// HOME environment is respected for non-root users if it exists.
/// </returns>
std::string GetHomeFolderPathLinux()
{
    static std::string homeDir{};

    if(!homeDir.empty())
        return homeDir;

    int uid = getuid();
    const char* homeEnv = std::getenv("HOME");
    if(uid != 0 && homeEnv)
    {
        //We only acknowledge HOME if not root.
        homeDir = homeEnv;
        return homeDir;
    }

    passwd* pw = nullptr;
    passwd pwd;
    int32_t bufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if(bufSize < 0)
        bufSize = 16384;
    std::vector<char> buffer{};
    buffer.resize(bufSize);
    int32_t errorCode = getpwuid_r(uid, &pwd, buffer.data(), buffer.size(), &pw);
    if(errorCode)
    {
        TP_ERROR(TRAP::Log::FileSystemPrefix, "Failed to get home folder path (", errorCode, ")");
        return {};
    }
    const char* tempRes = pw->pw_dir;
    if(!tempRes)
    {
        TP_ERROR(TRAP::Log::FileSystemPrefix, "Failed to get home folder path (", errorCode, ")");
        return {};
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
/// The document directory.
/// HOME environment is respected for non-root users if it exists.
/// </returns>
std::string GetDocumentsFolderPathLinux()
{
    static std::string documentsDir{};

    if(!documentsDir.empty())
        return documentsDir;

    documentsDir = "$HOME/Documents";

    //Get config folder
    const char* tempRes = std::getenv("XDG_CONFIG_HOME");
    std::string configPath{};
    if(tempRes)
        configPath = tempRes;
    else
        configPath = GetHomeFolderPathLinux() + "/.config";
    configPath += "/user-dirs.dirs";

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
            std::size_t splitPos = line.find('=');
            if(splitPos == std::string::npos)
                continue;
            std::string key = line.substr(0, splitPos);
            if(key != "XDG_DOCUMENTS_DIR") //Only interested in documents folder
                continue;
            std::size_t valueStart = line.find('"', splitPos);
            if(valueStart == std::string::npos)
                continue;
            std::size_t valueEnd = line.find('"', valueStart + 1);
            if(valueEnd == std::string::npos)
                continue;
            documentsDir = line.substr(valueStart + 1, valueEnd - valueStart - 1);
            break;
        }
    }
    else
    {
        TP_ERROR(TRAP::Log::FileSystemPrefix, "Failed to get documents folder path");
        return {};
    }
    file.close();

    if(documentsDir.compare(0, 5, "$HOME") == 0)
        documentsDir = GetHomeFolderPathLinux() + documentsDir.substr(5, std::string::npos);
    TP_TRACE("DocumentsDir: ", documentsDir);

    //Finally
    return documentsDir;
}
#endif