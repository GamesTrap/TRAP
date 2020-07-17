#include "TRAPPCH.h"
#include "VFS.h"

#include "Utils/String/String.h"
#include "FileSystem.h"

TRAP::Scope<TRAP::VFS> TRAP::VFS::s_Instance = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::Mount(const std::string& virtualPath, const std::string& physicalPath)
{
	TP_PROFILE_FUNCTION();

	if (virtualPath.empty() || physicalPath.empty())
	{
		TP_ERROR("[VFS] Virtual or Physical path is empty!");
		return;
	}

	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");

	const std::string virtualPathLower = Utils::String::ToLower(virtualPath);
	TP_INFO("[VFS] Mounting VirtualPath: \"", virtualPath, "\" to PhysicalPath: \"", [&]()
		{
			if (*(physicalPath.end() - 1) == '/')
				return std::string(physicalPath.begin(), physicalPath.end() - 1);

			return physicalPath;
		}(), "\"");
	s_Instance->m_mountPoints[virtualPathLower].emplace_back([&]()
		{
			if (*(physicalPath.end() - 1) == '/')
				return std::string(physicalPath.begin(), physicalPath.end() - 1);

			return physicalPath;
		}());

	if (s_Instance->m_hotShaderReloading)
		if (virtualPathLower == "/shaders")
			//Create a ShaderFileWatcher instance that will check the mounted folders for changes every second				
			s_Instance->m_shaderFileWatcher = MakeScope<FileWatcher>("/shaders", 1000.0f);

	if (s_Instance->m_hotTextureReloading)
		if (virtualPathLower == "/textures")
			//Create a TextureFileWatcher instance that will check the mounted folder for changes every second
			s_Instance->m_textureFileWatcher = MakeScope<FileWatcher>("/textures", 1000.0f);
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

void TRAP::VFS::Unmount(const std::string& path)
{
	TP_PROFILE_FUNCTION();

	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	TP_INFO("[VFS] Unmounting VirtualPath: \"", path, "\"");
	const std::string pathLower = Utils::String::ToLower(path);
	s_Instance->m_mountPoints[pathLower].clear();

	if (s_Instance->m_hotShaderReloading && s_Instance->m_shaderFileWatcher)
		if (pathLower == "/shaders")
			s_Instance->m_shaderFileWatcher.reset();

	if (s_Instance->m_hotTextureReloading && s_Instance->m_textureFileWatcher)
		if (pathLower == "/textures")
			s_Instance->m_textureFileWatcher.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::ResolveReadPhysicalPath(const std::string& path, std::filesystem::path& outPhysicalPath)
{
	if (path[0] != '/')
	{
		outPhysicalPath = path;

		return FileSystem::FileOrFolderExists(path);
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
		if (FileSystem::FileOrFolderExists(newPath))
		{
			outPhysicalPath = newPath;

			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::SilentResolveReadPhysicalPath(const std::string& path, std::filesystem::path& outPhysicalPath)
{
	if (path[0] != '/')
	{
		outPhysicalPath = path;

		return FileSystem::SilentFileOrFolderExists(path);
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
		if (FileSystem::SilentFileOrFolderExists(newPath))
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
	if (virtualPath[0] != '/')
		return std::vector<std::filesystem::path>();

	const std::string virtualPathLower = Utils::String::ToLower(virtualPath);

	if (s_Instance->m_mountPoints.find(virtualPathLower) == s_Instance->m_mountPoints.end() || s_Instance->m_mountPoints[virtualPathLower].empty())
		return std::vector<std::filesystem::path>();

	return std::vector<std::filesystem::path>(s_Instance->m_mountPoints[virtualPathLower].begin(),
		s_Instance->m_mountPoints[virtualPathLower].end());
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::VFS::ReadFile(const std::string& path)
{
	TP_PROFILE_FUNCTION();

	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return ResolveReadPhysicalPath(path, physicalPath) ? FileSystem::ReadFile(physicalPath) : std::vector<uint8_t>();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::VFS::SilentReadFile(const std::string& path)
{
	TP_PROFILE_FUNCTION();

	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return SilentResolveReadPhysicalPath(path, physicalPath) ? FileSystem::SilentReadFile(physicalPath) : std::vector<uint8_t>();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::VFS::ReadTextFile(const std::string& path)
{
	TP_PROFILE_FUNCTION();

	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return ResolveReadPhysicalPath(path, physicalPath) ? FileSystem::ReadTextFile(physicalPath) : "";
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::VFS::SilentReadTextFile(const std::string& path)
{
	TP_PROFILE_FUNCTION();

	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return SilentResolveReadPhysicalPath(path, physicalPath) ? FileSystem::SilentReadTextFile(physicalPath) : "";
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::WriteFile(const std::string& path, std::vector<uint8_t>& buffer)
{
	TP_PROFILE_FUNCTION();

	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return ResolveWritePhysicalPath(path, physicalPath) ? FileSystem::WriteFile(physicalPath, buffer) : false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::WriteTextFile(const std::string& path, const std::string& text)
{
	TP_PROFILE_FUNCTION();

	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return ResolveWritePhysicalPath(path, physicalPath) ? FileSystem::WriteTextFile(physicalPath, text) : false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::Init()
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG("[VFS] Initializing Virtual File System");
	s_Instance = MakeScope<VFS>();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::Shutdown()
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG("[VFS] Shutting down Virtual File System");
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