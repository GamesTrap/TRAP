#include "TRAPPCH.h"
#include "FS.h"

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

bool TRAP::FS::GetHotShaderReloading()
{
    return false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FS::SetHotShaderReloading(const bool)
{
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FS::GetHotTextureReloading()
{
    return false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::FS::SetHotTextureReloading(const bool)
{
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

TRAP::FileWatcher* TRAP::FS::GetShaderFileWatcher()
{
    return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FileWatcher* TRAP::FS::GetTextureFileWatcher()
{
    return nullptr;
}