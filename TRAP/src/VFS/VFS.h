#ifndef _TRAP_VFS_H_
#define _TRAP_VFS_H_

#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>

#include "FileWatcher.h"

namespace TRAP
{
	class VFS
	{
	public:
		void Mount(const std::string& virtualPath, const std::string& physicalPath);
		void MountShaders(const std::string& physicalPath);
		void Unmount(const std::string& path);
		bool ResolveReadPhysicalPath(const std::string& path, std::filesystem::path& outPhysicalPath);
		bool SilentResolveReadPhysicalPath(const std::string& path, std::filesystem::path& outPhysicalPath);
		bool ResolveWritePhysicalPath(const std::string& path, std::filesystem::path& outPhysicalPath);
		std::vector<std::filesystem::path> ResolveToPhysicalPaths(const std::string& virtualPath);

		std::vector<std::byte> ReadFile(const std::string& path);
		std::string ReadTextFile(const std::string& path);
		std::string SilentReadTextFile(const std::string& path);

		bool WriteFile(const std::string& path, std::vector<std::byte>& buffer);
		bool WriteTextFile(const std::string& path, const std::string& text);

		static void Init();
		static void Shutdown();

		bool GetHotShaderReloading() const;
		void SetHotShaderReloading(bool enabled);
		FileWatcher* GetShaderFileWatcher() const;

		static VFS* Get();

		static std::string MakeVirtualPathCompatible(const std::string& virtualPath);

	private:
		static std::unique_ptr<VFS> s_Instance;

		std::unordered_map<std::string, std::vector<std::string>> m_mountPoints;

		bool m_hotShaderReloading = false;
		std::unique_ptr<FileWatcher> m_shaderFileWatcher;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool TRAP::VFS::GetHotShaderReloading() const
{
	return m_hotShaderReloading;
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::VFS::SetHotShaderReloading(const bool enabled)
{
	m_hotShaderReloading = enabled;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::FileWatcher* TRAP::VFS::GetShaderFileWatcher() const
{
	return m_shaderFileWatcher.get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::VFS* TRAP::VFS::Get()
{
	return s_Instance.get();
}

#endif /*_TRAP_VFS_H_*/