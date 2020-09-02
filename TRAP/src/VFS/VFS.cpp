#include "TRAPPCH.h"
#include "VFS.h"

#include "Utils/String/String.h"

TRAP::Scope<TRAP::VFS> TRAP::VFS::s_Instance = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::Mount(const std::string& virtualPath, const std::string& physicalPath)
{
	TP_PROFILE_FUNCTION();

	if (virtualPath.empty() || physicalPath.empty())
	{
		TP_ERROR(Log::VFSPrefix, "Virtual or Physical path is empty!");
		return;
	}
	if(virtualPath[0] != '/')
	{
		TP_ERROR(Log::VFSPrefix, "Virtual path: \"", virtualPath, R"(" doesnt start with "/"!)");
		return;
	}
	if(physicalPath[0] == '/')
	{
		TP_ERROR(Log::VFSPrefix, "Physical path: \"", physicalPath, R"(" must not start with "/"!)");
		return;
	}

	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");

	constexpr auto RemoveSlash = [](const std::string& pPath)
	{
		if (*(pPath.end() - 1) == '/')
			return std::string(pPath.begin(), pPath.end() - 1);
		
		return pPath;
	};

	const std::string virtualPathLower = Utils::String::ToLower(virtualPath);
	TP_INFO(Log::VFSPrefix, "Mounting VirtualPath: \"", virtualPath, "\" to PhysicalPath: \"", RemoveSlash(physicalPath), "\"");
	s_Instance->m_mountPoints[virtualPathLower].emplace_back(RemoveSlash(physicalPath));

	if (s_Instance->m_hotShaderReloading)
	{
		if (virtualPathLower == "/shaders")
			//Create a ShaderFileWatcher instance that will check the mounted folders for changes
			s_Instance->m_shaderFileWatcher = MakeScope<FileWatcher>("/shaders");
	}

	if (s_Instance->m_hotTextureReloading)
	{
		if (virtualPathLower == "/textures")
			//Create a TextureFileWatcher instance that will check the mounted folder for changes
			s_Instance->m_textureFileWatcher = MakeScope<FileWatcher>("/textures");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::MountShaders(const std::string& physicalPath)
{
	Mount("/shaders", physicalPath);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::MountTextures(const std::string& physicalPath)
{
	Mount("/textures", physicalPath);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::Unmount(const std::string& virtualPath)
{
	TP_PROFILE_FUNCTION();

	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");

	if(virtualPath.empty())
	{
		TP_ERROR(Log::VFSPrefix, "Can't unmount empty virtual path!");
		return;
	}
	
	TP_INFO(Log::VFSPrefix, "Unmounting VirtualPath: \"", virtualPath, "\"");
	const std::string pathLower = Utils::String::ToLower(virtualPath);
	if(s_Instance->m_mountPoints.find(pathLower) != s_Instance->m_mountPoints.end())
		s_Instance->m_mountPoints[pathLower].clear();

	if (s_Instance->m_hotShaderReloading && s_Instance->m_shaderFileWatcher)
		if (pathLower == "/shaders")
			s_Instance->m_shaderFileWatcher.reset();

	if (s_Instance->m_hotTextureReloading && s_Instance->m_textureFileWatcher)
		if (pathLower == "/textures")
			s_Instance->m_textureFileWatcher.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::ResolveReadPhysicalPath(const std::string& path, std::filesystem::path& outPhysicalPath, const bool silent)
{
	if(path.empty())
	{
		if(!silent)
			TP_ERROR("[VFS] Path couldn't be resolved because it is empty!");
		
		return false;
	}
	
	if (path[0] != '/')
	{
		outPhysicalPath = path;

		return FileOrFolderExists(path, silent);
	}

	std::vector<std::string> dirs = Utils::String::SplitString(path, '/');
	std::string virtualDir = dirs.front();
	virtualDir = Utils::String::ToLower(virtualDir);

	if (s_Instance->m_mountPoints.find('/' + virtualDir) == s_Instance->m_mountPoints.end() || s_Instance->m_mountPoints['/' + virtualDir].empty())
		return false;

	const std::string remainder = path.substr(virtualDir.size() + 1, path.size() - virtualDir.size());
	for (const std::string& physicalPath : s_Instance->m_mountPoints['/' + virtualDir])
	{
		std::string newPath = physicalPath + remainder;
		if (FileOrFolderExists(newPath, silent))
		{
			outPhysicalPath = newPath;

			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::ResolveWritePhysicalPath(const std::string& path, std::filesystem::path& outPhysicalPath)
{
	if(path.empty())
	{
		TP_ERROR("[VFS] Path couldn't be resolved because it is empty!");
		return false;
	}
	
	if (path[0] != '/')
	{
		outPhysicalPath = path;

		return true;
	}

	std::vector<std::string> dirs = Utils::String::SplitString(path, '/');
	std::string virtualDir = dirs.front();
	virtualDir = Utils::String::ToLower(virtualDir);

	if (s_Instance->m_mountPoints.find('/' + virtualDir) == s_Instance->m_mountPoints.end() || s_Instance->m_mountPoints['/' + virtualDir].empty())
		return false;

	const std::string remainder = path.substr(virtualDir.size() + 1, path.size() - virtualDir.size());
	for (const std::string& physicalPath : s_Instance->m_mountPoints['/' + virtualDir])
	{
		const std::string newPath = physicalPath + remainder;
		outPhysicalPath = newPath;

		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::filesystem::path> TRAP::VFS::ResolveToPhysicalPaths(const std::string& virtualPath)
{
	if (virtualPath.empty())
		return std::vector<std::filesystem::path>();
	
	if (virtualPath[0] != '/')
		return std::vector<std::filesystem::path>();

	const std::string virtualPathLower = Utils::String::ToLower(virtualPath);

	if (s_Instance->m_mountPoints.find(virtualPathLower) == s_Instance->m_mountPoints.end() || s_Instance->m_mountPoints[virtualPathLower].empty())
		return std::vector<std::filesystem::path>();

	return std::vector<std::filesystem::path>(s_Instance->m_mountPoints[virtualPathLower].begin(),
		s_Instance->m_mountPoints[virtualPathLower].end());
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::VFS::ReadFile(const std::string& path, bool silent)
{
	TP_PROFILE_FUNCTION();

	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return ResolveReadPhysicalPath(path, physicalPath, silent) ? ReadPhysicalFile(physicalPath, silent) : std::vector<uint8_t>();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::VFS::ReadTextFile(const std::string& path, bool silent)
{
	TP_PROFILE_FUNCTION();

	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return ResolveReadPhysicalPath(path, physicalPath, silent) ? ReadPhysicalTextFile(physicalPath, silent) : "";
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::WriteFile(const std::string& path, std::vector<uint8_t>& buffer, const WriteMode mode)
{
	TP_PROFILE_FUNCTION();

	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return ResolveWritePhysicalPath(path, physicalPath) ? WritePhysicalFile(physicalPath, buffer, mode) : false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::WriteTextFile(const std::string& path, const std::string& text, const WriteMode mode)
{
	TP_PROFILE_FUNCTION();

	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return ResolveWritePhysicalPath(path, physicalPath) ? WritePhysicalTextFile(physicalPath, text, mode) : false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::FileOrFolderExists(const std::filesystem::path& path, const bool silent)
{
	if (path.empty())
		return false;

	if(path.c_str()[0] == '/')
	{
		std::filesystem::path pPath;
		if (ResolveReadPhysicalPath(path.string(), pPath, silent))
		{
			try
			{
				if (!std::filesystem::exists(pPath))
				{
					if (!silent)
						TP_WARN(Log::FileSystemPrefix, "File/Folder: ", pPath, " doesn't exist!");

					return false;
				}

				return true;
			}
			catch (std::exception&)
			{
				return false;
			}
		}
		
		return false;
	}
	
	try
	{
		if (!std::filesystem::exists(path))
		{
			if(!silent)
				TP_WARN(Log::FileSystemPrefix, "File/Folder: ", path, " doesn't exist!");
			
			return false;
		}

		return true;
	}
	catch (std::exception&)
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uintmax_t TRAP::VFS::GetFileOrFolderSize(const std::filesystem::path& path)
{
	if (FileOrFolderExists(path))
	{
		if(path.c_str()[0] == '/')
		{
			std::filesystem::path pPath;
			if(ResolveReadPhysicalPath(path.string(), pPath))
			{
				try
				{
					if (std::filesystem::is_directory(pPath))
					{
						uintmax_t size = 0;
						for(const auto& entry : std::filesystem::recursive_directory_iterator(pPath))
						{
							if (!entry.is_directory() && entry.is_regular_file())
								size += entry.file_size();
						}						
						return size;
					}
					
					return std::filesystem::file_size(pPath);
				}
				catch (std::exception&)
				{
					return 0;
				}
			}
			
			return 0;
		}

		try
		{
			if(std::filesystem::is_directory(path))
			{
				uintmax_t size = 0;
				for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
				{
					if (!entry.is_directory() && entry.is_regular_file())
						size += entry.file_size();
				}
				return size;
			}
			
			return std::filesystem::file_size(path);
		}
		catch (std::exception&)
		{
			return 0;
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::file_time_type TRAP::VFS::GetLastWriteTime(const std::filesystem::path& path)
{
	if (FileOrFolderExists(path))
	{
		if(path.c_str()[0] == '/')
		{
			std::filesystem::path pPath;
			if(ResolveReadPhysicalPath(path.string(), pPath))
			{
				try
				{
					return std::filesystem::last_write_time(pPath);
				}
				catch (std::exception&)
				{
					return std::filesystem::file_time_type::min();
				}
			}
			
			return std::filesystem::file_time_type::min();
		}
		
		try
		{
			return std::filesystem::last_write_time(path);
		}
		catch (std::exception&)
		{
			return std::filesystem::file_time_type::min();
		}
	}

	return std::filesystem::file_time_type::min();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::VFS::ReadPhysicalFile(const std::filesystem::path& physicalFilePath, const bool silent)
{
	if (FileOrFolderExists(physicalFilePath, silent))
	{
		std::ifstream file(physicalFilePath, std::ios::binary | std::ios::ate);
		if (file.is_open())
		{
			const uintmax_t length = file.tellg();
			file.seekg(0);
			std::vector<uint8_t> buffer(length);
			file.read(reinterpret_cast<char*>(buffer.data()), length);

			file.close();
			return buffer;
		}

		if(!silent)
			TP_ERROR(Log::FileSystemPrefix, "Could not open File: \"", physicalFilePath, "\"");
	}

	return std::vector<uint8_t>();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::VFS::ReadPhysicalTextFile(const std::filesystem::path& physicalFilePath, const bool silent)
{
	if (FileOrFolderExists(physicalFilePath, silent))
	{
		std::ifstream file(physicalFilePath);
		if (file.is_open())
		{
			std::string line;
			std::string result;

			while (file.good() && std::getline(file, line))
			{
				if (!line.empty())
					if (line.back() == '\r')
						line.pop_back();

				result += line;
				result += '\n';
			}

			file.close();
			return result;
		}

		if(!silent)
			TP_ERROR(Log::FileSystemPrefix, "Could not open File: \"", physicalFilePath, "\"");
	}

	return "";
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::WritePhysicalFile(const std::filesystem::path& physicalFilePath, std::vector<uint8_t>& buffer, const WriteMode mode)
{
	if (!physicalFilePath.empty() && !buffer.empty())
	{
		std::ofstream file(physicalFilePath, mode == WriteMode::Overwrite ? std::ios::binary : (std::ios::binary | std::ios::ate));
		if (file.is_open() && file.good())
		{
			file.write(reinterpret_cast<char*>(buffer.data()), buffer.size());

			file.close();
			return true;
		}

		TP_ERROR(Log::FileSystemPrefix, "Could not write File: \"", physicalFilePath, "\"");
	}
	else
		TP_ERROR(Log::FileSystemPrefix, "Could not write File because physical file path and/or data was empty!");

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::WritePhysicalTextFile(const std::filesystem::path& physicalFilePath, const std::string_view text, WriteMode mode)
{
	if (!physicalFilePath.empty() && !text.empty())
	{
		std::ofstream file;
		if (mode == WriteMode::Append)
			file = std::ofstream(physicalFilePath, std::ios::ate);
		else
			file = std::ofstream(physicalFilePath);
		if (file.is_open() && file.good())
		{
			file << text;

			file.close();
			return true;
		}

		TP_ERROR(Log::FileSystemPrefix, "Could not write File: ", physicalFilePath);
	}
	else
		TP_ERROR(Log::FileSystemPrefix, "Could not write File because physical file path and/or data was empty!");

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::Init()
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG(Log::VFSPrefix, "Initializing Virtual File System");
	s_Instance = MakeScope<VFS>();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::Shutdown()
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG(Log::VFSPrefix, "Shutting down Virtual File System");
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::GetHotShaderReloading()
{
	return s_Instance->m_hotShaderReloading;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::SetHotShaderReloading(const bool enabled)
{
	s_Instance->m_hotShaderReloading = enabled;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FileWatcher* TRAP::VFS::GetShaderFileWatcher()
{
	return s_Instance->m_shaderFileWatcher.get();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::GetHotTextureReloading()
{
	return s_Instance->m_hotTextureReloading;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::SetHotTextureReloading(const bool enabled)
{
	s_Instance->m_hotTextureReloading = enabled;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FileWatcher* TRAP::VFS::GetTextureFileWatcher()
{
	return s_Instance->m_textureFileWatcher.get();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::VFS::MakeVirtualPathCompatible(const std::string& virtualPath)
{
	if (virtualPath.empty())
		return {};
	
	std::vector<std::string> dirs = Utils::String::SplitString(virtualPath, '/');
	std::string virtualDir = dirs.front();
	virtualDir = Utils::String::ToLower(virtualDir);

	std::string remainder;
	if(dirs.size() > 1)
		remainder = virtualPath.substr(virtualDir.size() + 1, virtualPath.size() - virtualDir.size());

	return '/' + virtualDir + remainder;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::VFS::GetFileName(const std::string& virtualPath)
{
	std::string result = Utils::String::SplitString(virtualPath, '/').back();
	result = result.substr(0, result.size() - (Utils::String::GetSuffix(result).size() + 1));

	return result;
}