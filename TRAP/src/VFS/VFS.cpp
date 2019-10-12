#include "TRAPPCH.h"
#include "VFS.h"

#include "Utils/String.h"
#include "FileSystem.h"

TRAP::Scope<TRAP::VFS> TRAP::VFS::s_Instance = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::Mount(const std::string& virtualPath, const std::string& physicalPath)
{
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
	m_mountPoints[virtualPathLower].emplace_back([&]()
		{
			if (*(physicalPath.end() - 1) == '/')
				return std::string(physicalPath.begin(), physicalPath.end() - 1);

			return physicalPath;
		}());

	if (m_hotShaderReloading)
		if (virtualPathLower == "/shaders")
			//Create a ShaderFileWatcher instance that will check the mounted folders for changes every second				
			m_shaderFileWatcher = MakeScope<FileWatcher>("/shaders", 1000.0f);

	if (m_hotTextureReloading)
		if (virtualPathLower == "/textures")
			//Create a TextureFileWatcher instance that will check the mounted folder for changes every second
			m_textureFileWatcher = MakeScope<FileWatcher>("/textures", 1000.0f);
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
	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	TP_INFO("[VFS] Unmounting VirtualPath: \"", path, "\"");
	const std::string pathLower = Utils::String::ToLower(path);
	m_mountPoints[pathLower].clear();

	if (m_hotShaderReloading && m_shaderFileWatcher)
		if (pathLower == "/shaders")
			m_shaderFileWatcher.reset();

	if (m_hotTextureReloading && m_textureFileWatcher)
		if (pathLower == "/textures")
			m_textureFileWatcher.reset();
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

	if (m_mountPoints.find('/' + virtualDir) == m_mountPoints.end() || m_mountPoints['/' + virtualDir].empty())
		return false;

	const std::string remainder = path.substr(virtualDir.size() + 1, path.size() - virtualDir.size());
	for (const std::string& physicalPath : m_mountPoints['/' + virtualDir])
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

	if (m_mountPoints.find('/' + virtualDir) == m_mountPoints.end() || m_mountPoints['/' + virtualDir].empty())
		return false;

	const std::string remainder = path.substr(virtualDir.size() + 1, path.size() - virtualDir.size());
	for (const std::string& physicalPath : m_mountPoints['/' + virtualDir])
	{
		std::string newPath = physicalPath + remainder;
		if (!FileSystem::SilentFileOrFolderExists(newPath))
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

	if (m_mountPoints.find('/' + virtualDir) == m_mountPoints.end() || m_mountPoints['/' + virtualDir].empty())
		return false;

	const std::string remainder = path.substr(virtualDir.size() + 1, path.size() - virtualDir.size());
	for (const std::string& physicalPath : m_mountPoints['/' + virtualDir])
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

	if (m_mountPoints.find(virtualPathLower) == m_mountPoints.end() || m_mountPoints[virtualPathLower].empty())
		return std::vector<std::filesystem::path>();

	return std::vector<std::filesystem::path>(m_mountPoints[virtualPathLower].begin(),
		m_mountPoints[virtualPathLower].end());
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::byte> TRAP::VFS::ReadFile(const std::string& path)
{
	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return ResolveReadPhysicalPath(path, physicalPath) ? FileSystem::ReadFile(physicalPath) : std::vector<std::byte>();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::byte> TRAP::VFS::SilentReadFile(const std::string& path)
{
	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return SilentResolveReadPhysicalPath(path, physicalPath) ? FileSystem::SilentReadFile(physicalPath) : std::vector<std::byte>();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::VFS::ReadTextFile(const std::string& path)
{
	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return ResolveReadPhysicalPath(path, physicalPath) ? FileSystem::ReadTextFile(physicalPath) : "";
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::VFS::SilentReadTextFile(const std::string& path)
{
	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return SilentResolveReadPhysicalPath(path, physicalPath) ? FileSystem::SilentReadTextFile(physicalPath) : "";
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::WriteFile(const std::string& path, std::vector<std::byte>& buffer)
{
	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return ResolveWritePhysicalPath(path, physicalPath) ? FileSystem::WriteFile(physicalPath, buffer) : false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::WriteTextFile(const std::string& path, const std::string& text)
{
	TRAP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return ResolveWritePhysicalPath(path, physicalPath) ? FileSystem::WriteTextFile(physicalPath, text) : false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::Init()
{
	TP_DEBUG("[VFS] Initializing Virtual File System");
	s_Instance = MakeScope<VFS>();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::Shutdown()
{
	TP_DEBUG("[VFS] Shutting down Virtual File System");
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::GetHotShaderReloading() const
{
	return m_hotShaderReloading;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::SetHotShaderReloading(const bool enabled)
{
	m_hotShaderReloading = enabled;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FileWatcher* TRAP::VFS::GetShaderFileWatcher() const
{
	return m_shaderFileWatcher.get();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::GetHotTextureReloading() const
{
	return m_hotTextureReloading;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::SetHotTextureReloading(const bool enabled)
{
	m_hotTextureReloading = enabled;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::FileWatcher* TRAP::VFS::GetTextureFileWatcher() const
{
	return m_textureFileWatcher.get();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::VFS* TRAP::VFS::Get()
{
	return s_Instance.get();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::VFS::MakeVirtualPathCompatible(const std::string& virtualPath)
{
	std::vector<std::string> dirs = Utils::String::SplitString(virtualPath, '/');
	std::string virtualDir = dirs.front();
	virtualDir = Utils::String::ToLower(virtualDir);

	const std::string remainder = virtualPath.substr(virtualDir.size() + 1, virtualPath.size() - virtualDir.size());

	return '/' + virtualDir + remainder;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::VFS::GetFileName(const std::string& virtualPath)
{
	std::string result = Utils::String::SplitString(virtualPath, '/').back();
	result = result.substr(0, result.size() - (Utils::String::GetSuffix(result).size() + 1));

	return result;
}