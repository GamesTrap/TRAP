#include "TRAPPCH.h"
#include "VFS.h"

#include "Utils/String.h"
#include "FileSystem.h"

std::unique_ptr<TRAP::VFS> TRAP::VFS::s_Instance = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::Init()
{
	TP_DEBUG("[VFS] Initializing Virtual File System");
	s_Instance = std::make_unique<VFS>();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::Shutdown()
{
	TP_DEBUG("[VFS] Shutting down Virtual File System");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::VFS::Mount(const std::string& virtualPath, const std::string& physicalPath)
{
	TP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	TP_INFO("[VFS] Mounting VirtualPath: \"", virtualPath, "\" to PhysicalPath: \"", physicalPath, "\"");
	const std::string virtualPathLower = Utils::String::ToLower(virtualPath);
	m_mountPoints[virtualPathLower].push_back(physicalPath);

	if (m_hotShaderReloading)
		if (virtualPathLower == "/shaders")
			//Create a ShaderFileWatcher instance that will check the mounted folders for changes every second				
			m_shaderFileWatcher = std::make_unique<FileWatcher>("/shaders", 1000.0f);

	if (m_hotTextureReloading)
		if (virtualPathLower == "/textures")
			//Create a TextureFileWatcher instance that will check the mounted folder for changes every second
			m_textureFileWatcher = std::make_unique<FileWatcher>("/textures", 1000.0f);
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
	TP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
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
	TP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return ResolveReadPhysicalPath(path, physicalPath) ? FileSystem::ReadFile(physicalPath) : std::vector<std::byte>();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::VFS::ReadTextFile(const std::string& path)
{
	TP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return ResolveReadPhysicalPath(path, physicalPath) ? FileSystem::ReadTextFile(physicalPath) : "";
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::VFS::SilentReadTextFile(const std::string& path)
{
	TP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return SilentResolveReadPhysicalPath(path, physicalPath) ? FileSystem::SilentReadTextFile(physicalPath) : "";
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::WriteFile(const std::string& path, std::vector<std::byte>& buffer)
{
	TP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return ResolveWritePhysicalPath(path, physicalPath) ? FileSystem::WriteFile(physicalPath, buffer) : false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::VFS::WriteTextFile(const std::string& path, const std::string& text)
{
	TP_ASSERT(s_Instance.get(), "s_Instance is nullptr!");
	std::filesystem::path physicalPath;

	return ResolveWritePhysicalPath(path, physicalPath) ? FileSystem::WriteTextFile(physicalPath, text) : false;
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