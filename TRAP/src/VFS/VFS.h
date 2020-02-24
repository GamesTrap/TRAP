#ifndef _TRAP_VFS_H_
#define _TRAP_VFS_H_

#include "FileWatcher.h"

namespace TRAP
{
	class VFS final
	{
	public:
		VFS() = default;
		~VFS() = default;		

		VFS(const VFS&) = delete;
		VFS& operator=(const VFS&) = delete;
		VFS(VFS&&) = delete;
		VFS& operator=(VFS&&) = delete;
		
		static void Mount(const std::string& virtualPath, const std::string& physicalPath);
		static void MountShaders(const std::string& physicalPath);
		static void MountTextures(const std::string& physicalPath);
		static void Unmount(const std::string& path);
		static bool ResolveReadPhysicalPath(const std::string& path, std::filesystem::path& outPhysicalPath);
		static bool SilentResolveReadPhysicalPath(const std::string& path, std::filesystem::path& outPhysicalPath);
		static bool ResolveWritePhysicalPath(const std::string& path, std::filesystem::path& outPhysicalPath);
		static std::vector<std::filesystem::path> ResolveToPhysicalPaths(const std::string& virtualPath);

		static std::vector<std::byte> ReadFile(const std::string& path);
		static std::vector<std::byte> SilentReadFile(const std::string& path);
		static std::string ReadTextFile(const std::string& path);
		static std::string SilentReadTextFile(const std::string& path);

		static bool WriteFile(const std::string& path, std::vector<std::byte>& buffer);
		static bool WriteTextFile(const std::string& path, const std::string& text);

		static void Init();
		static void Shutdown();

		static bool GetHotShaderReloading();
		static void SetHotShaderReloading(bool enabled);
		static FileWatcher* GetShaderFileWatcher();

		static bool GetHotTextureReloading();
		static void SetHotTextureReloading(bool enabled);
		static FileWatcher* GetTextureFileWatcher();

		static std::string MakeVirtualPathCompatible(const std::string& virtualPath);
		static std::string GetFileName(const std::string& virtualPath);

	private:
		static Scope<VFS> s_Instance;
		
		std::unordered_map<std::string, std::vector<std::string>> m_mountPoints;

		bool m_hotShaderReloading = false;
		Scope<FileWatcher> m_shaderFileWatcher;

		bool m_hotTextureReloading = false;
		Scope<FileWatcher> m_textureFileWatcher;
	};
}

#endif /*_TRAP_VFS_H_*/