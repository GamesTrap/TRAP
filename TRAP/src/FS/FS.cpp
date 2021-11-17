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

        result += line;
        result += '\n';
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

bool TRAP::FS::WriteTextFile(const std::filesystem::path& path, const std::string& text, const WriteMode mode)
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

    try
    {
        if(!std::filesystem::exists(path))
        {
            TP_WARN(Log::FileSystemPrefix, "File/Folder: ", path.generic_u8string(), " doesn't exist!");
            return false;
        }
    }
    catch (std::exception&)
    {
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

uintmax_t TRAP::FS::GetFileOrFolderSize(const std::filesystem::path& path, const bool recursive)
{
    TP_PROFILE_FUNCTION();

    if(!FileOrFolderExists(path))
        return 0;

    uintmax_t size = 0;
    try
    {
        if(std::filesystem::is_directory(path))
        {
	        const std::filesystem::recursive_directory_iterator rDIt(path);
	        const std::filesystem::directory_iterator dIt(path);
            if(recursive)
            {
                for(const auto& entry : rDIt)
                {
                    if(!entry.is_directory() && entry.is_regular_file())
                        size += entry.file_size();
                }
            }
            else
            {
                for(const auto& entry : dIt)
                {
                    if(!entry.is_directory() && entry.is_regular_file())
                        size += entry.file_size();
                }
            }
        }
    }
    catch (std::exception&)
    {
        return 0;
    }

    return size;
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::file_time_type TRAP::FS::GetLastWriteTime(const std::filesystem::path& path)
{
    TP_PROFILE_FUNCTION();

    if(!FileOrFolderExists(path))
        return std::filesystem::file_time_type::min();

    try
    {
        return std::filesystem::last_write_time(path);
    }
    catch (std::exception&)
    {
        return std::filesystem::file_time_type::min();
    }
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
    std::string path;
    try
    {
        path = std::filesystem::temp_directory_path().generic_u8string();
    }
    catch (std::exception&)
    {
        return {};
    }

    return std::filesystem::path(path);
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::path TRAP::FS::GetGameTempFolderPath()
{
    return GetTempFolderPath() / "TRAP" / TRAP::Application::GetGameName();
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::path TRAP::FS::GetCurrentFolderPath()
{
    std::string path;
    try
    {
        path = std::filesystem::current_path().generic_u8string();
    }
    catch (std::exception&)
    {
        return {};
    }

    return std::filesystem::path(path);
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

void TRAP::FS::Init()
{
    TP_PROFILE_FUNCTION();

	TP_DEBUG(Log::FileSystemPrefix, "Initializing File System");

    std::filesystem::path p = GetTempFolderPath() / "TRAP" / TRAP::Application::GetGameName();
    if(!std::filesystem::exists(p))
        std::filesystem::create_directories(p);
    p = GetDocumentsFolderPath() / "TRAP" / TRAP::Application::GetGameName();
    if(!std::filesystem::exists(p))
        std::filesystem::create_directories(p);
    p = GetDocumentsFolderPath() / "TRAP" / TRAP::Application::GetGameName() / "logs";
    if(!std::filesystem::exists(p))
        std::filesystem::create_directories(p);
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