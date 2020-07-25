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

		/// <summary>
		/// Mount a physical folder path to a virtual folder path.
		/// NOTE: Virtual file paths always use lowercase.
		///
		/// If HotShaderReloading is enabled and the virtualPath is "/shaders" then this also instantiates the Shader TRAP::FileWatcher.
		/// If HotTextureReloading is enabled and the virtualPath is "/textures" then this also instantiates the Texture TRAP::FileWatcher.
		/// 
		/// Prints an error if either virtualPath or physicalPath is empty
		/// </summary>
		/// <param name="virtualPath">Virtual folder path to be bound to</param>
		/// <param name="physicalPath">Physical folder path</param>
		static void Mount(const std::string& virtualPath, const std::string& physicalPath);
		/// <summary>
		/// Mount a physical folder path to the virtual folder path "/shaders".
		///
		/// If HotShaderReloading is enabled and the virtualPath is "/shaders" then this also instantiates the Shader TRAP::FileWatcher
		/// </summary>
		/// <param name="physicalPath">Physical folder path</param>
		static void MountShaders(const std::string& physicalPath);
		/// <summary>
		/// Mount a physical folder path to the virtual folder path "/textures".
		///
		/// If HotTextureReloading is enabled and the virtualPath is "/textures" then this also instantiates the Texture TRAP::FileWatcher
		/// </summary>
		/// <param name="physicalPath">Physical folder path</param>
		static void MountTextures(const std::string& physicalPath);
		/// <summary>
		/// Unmount a virtual folder path.
		///
		/// If virtualPath is "/shaders" and HotShaderReloading is enabled and the Shader TRAP::FileWatcher is already instantiated then this also stops it.
		/// If virtualPath is "/textures" and HotTextureReloading is enabled and the Texture TRAP::FileWatcher is already instantiated then this also stops it.
		/// </summary>
		/// <param name="virtualPath">Virtual folder path to unmount</param>
		static void Unmount(const std::string& virtualPath);
		/// <summary>
		/// Resolve a virtual file or folder path to its physical file or folder path.
		///
		/// Prints an error if path wasn't found or doesn't exist
		/// </summary>
		/// <param name="path">Virtual or physical file or folder path to resolve</param>
		/// <param name="outPhysicalPath">Output physical path. Unchanged if path couldn't be resolved</param>
		/// <returns>
		/// True if file or folder was found.
		/// False if it wasn't found doesn't exist
		/// </returns>
		static bool ResolveReadPhysicalPath(const std::string& path, std::filesystem::path& outPhysicalPath);
		/// <summary>
		/// Resolve a virtual file or folder path to its physical file or folder path silently
		/// </summary>
		/// <param name="path">Virtual or physical file or folder path to resolve</param>
		/// <param name="outPhysicalPath">Output physical path. Unchanged if path couldn't be resolved</param>
		/// <returns>
		/// True if file or folder was found.
		/// False if it wasn't found or doesn't exist
		/// </returns>
		static bool SilentResolveReadPhysicalPath(const std::string& path, std::filesystem::path& outPhysicalPath);
		/// <summary>
		/// Resolve a virtual file or folder path to its physical file or folder path.
		///
		/// If path already is a physical path then it will just be written to outPhysicalPath without changes 
		/// </summary>
		/// <param name="path">Virtual or physical file or folder path to resolve</param>
		/// <param name="outPhysicalPath">Output physical path. Unchanged if path couldn't be resolved</param>
		/// <returns>
		/// True if file or folder was found.
		/// False if it wasn't found
		/// </returns>
		static bool ResolveWritePhysicalPath(const std::string& path, std::filesystem::path& outPhysicalPath);
		/// <summary>
		/// Resolve a virtual file or folder path to its (multiple) physical file or folder path(s)
		/// </summary>
		/// <param name="virtualPath">Virtual file or folder path to resolve</param>
		/// <returns>Vector filled with all paths mounted to the virtualPath</returns>
		static std::vector<std::filesystem::path> ResolveToPhysicalPaths(const std::string& virtualPath);

		/// <summary>
		/// Read the given file.
		/// Can be a virtual or a physical file path
		/// </summary>
		/// <param name="path">Virtual or physical file path</param>
		/// <returns>If path could be resolved a vector with the file content else an error and an empty vector</returns>
		static std::vector<uint8_t> ReadFile(const std::string& path);
		/// <summary>
		/// Read the given file silently.
		/// Can be a virtual or a physical file path
		/// </summary>
		/// <param name="path">Virtual or physical file path</param>
		/// <returns>If path could be resolved a vector with the file content else an empty vector</returns>
		static std::vector<uint8_t> SilentReadFile(const std::string& path);
		/// <summary>
		/// Read the given text file.
		/// Can be a virtual or a physical file path
		/// </summary>
		/// <param name="path">Virtual or physical file path</param>
		/// <returns>If path could be resolved a string with the file content else an error and an empty string</returns>
		static std::string ReadTextFile(const std::string& path);
		/// <summary>
		/// Read the given text file silently.
		/// Can be a virtual or a physical file path
		/// </summary>
		/// <param name="path">Virtual or physical file path</param>
		/// <returns>If path could be resolved a string with the file content else an empty string</returns>
		static std::string SilentReadTextFile(const std::string& path);

		/// <summary>
		/// Write the given data to the given file path.
		/// Can be a virtual or a physical file path
		/// </summary>
		/// <param name="path">Virtual or physical file path</param>
		/// <param name="buffer">Data to be written</param>
		/// <returns>If path could be resolved and data </returns>
		static bool WriteFile(const std::string& path, std::vector<uint8_t>& buffer);
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