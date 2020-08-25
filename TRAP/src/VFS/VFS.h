#ifndef _TRAP_VFS_H_
#define _TRAP_VFS_H_

#include "FileSystem.h"
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
		/// Mount a physical folder path to a virtual folder path.<br>
		/// Subfolders need to be mounted separately.<br>
		/// NOTE: Physical path should use forward slashes and not back slashes!
		/// NOTE: Virtual file paths always use lowercase (except for the filename and its ending).<br>
		/// <br>
		/// If HotShaderReloading is enabled and the virtualPath is "/shaders" then this also instantiates the Shader TRAP::FileWatcher.<br>
		/// If HotTextureReloading is enabled and the virtualPath is "/textures" then this also instantiates the Texture TRAP::FileWatcher.<br>
		/// <br>
		/// Prints an error if either virtualPath or physicalPath is empty or invalid.
		/// </summary>
		/// <param name="virtualPath">Virtual folder path to be bound to.</param>
		/// <param name="physicalPath">Physical folder path.</param>
		static void Mount(const std::string& virtualPath, const std::string& physicalPath);
		/// <summary>
		/// Mount a physical folder path to the virtual folder path "/shaders".<br>
		/// <br>
		/// If HotShaderReloading is enabled and the virtualPath is "/shaders" then this also instantiates the Shader TRAP::FileWatcher.<br>
		/// <br>
		/// Prints an error if physicalPath is empty.
		/// </summary>
		/// <param name="physicalPath">Physical folder path.</param>
		static void MountShaders(const std::string& physicalPath);
		/// <summary>
		/// Mount a physical folder path to the virtual folder path "/textures".<br>
		/// <br>
		/// If HotTextureReloading is enabled and the virtualPath is "/textures" then this also instantiates the Texture TRAP::FileWatcher.<br>
		/// <br>
		/// Prints an error if physicalPath is empty.
		/// </summary>
		/// <param name="physicalPath">Physical folder path.</param>
		static void MountTextures(const std::string& physicalPath);
		/// <summary>
		/// Unmount a virtual folder path.<br>
		/// <br>
		/// Prints an error if virtualPath is empty.<br>
		/// <br>
		/// If virtualPath is "/shaders" and HotShaderReloading is enabled and the Shader TRAP::FileWatcher is already instantiated then this also stops it.<br>
		/// If virtualPath is "/textures" and HotTextureReloading is enabled and the Texture TRAP::FileWatcher is already instantiated then this also stops it.<br>
		/// </summary>
		/// <param name="virtualPath">Virtual folder path to unmount.</param>
		static void Unmount(const std::string& virtualPath);
		/// <summary>
		/// Resolve a virtual file or folder path to its physical file or folder path.<br>
		/// <br>
		/// Prints an error if path is empty, wasn't found or doesn't exist.
		/// </summary>
		/// <param name="path">Virtual or physical file or folder path to resolve.</param>
		/// <param name="outPhysicalPath">Output physical path. Unchanged if path couldn't be resolved.</param>
		/// <returns>
		/// True if file or folder was found.<br>
		/// False if it wasn't found doesn't exist.<br>
		/// </returns>
		static bool ResolveReadPhysicalPath(const std::string& path, std::filesystem::path& outPhysicalPath);
		/// <summary>
		/// Resolve a virtual file or folder path to its physical file or folder path silently.
		/// </summary>
		/// <param name="path">Virtual or physical file or folder path to resolve.</param>
		/// <param name="outPhysicalPath">
		/// Output physical path.<br
		/// Unchanged if path couldn't be resolved.
		/// </param>
		/// <returns>
		/// True if file or folder was found.<br>
		/// False if it wasn't found or doesn't exist.
		/// </returns>
		static bool SilentResolveReadPhysicalPath(const std::string& path, std::filesystem::path& outPhysicalPath);
		/// <summary>
		/// Resolve a virtual file or folder path to its physical file or folder path.<br>
		/// <br>
		/// If path already is a physical path then it will just be written to outPhysicalPath without changes.<br>
		/// Prints an error if path is empty.
		/// </summary>
		/// <param name="path">Virtual or physical file or folder path to resolve.</param>
		/// <param name="outPhysicalPath">
		/// Output physical path.<br>
		/// Unchanged if path couldn't be resolved.
		/// </param>
		/// <returns>
		/// True if file or folder was found.<br>
		/// False if it wasn't found.
		/// </returns>
		static bool ResolveWritePhysicalPath(const std::string& path, std::filesystem::path& outPhysicalPath);
		/// <summary>
		/// Resolve a virtual file or folder path to its (possibly multiple) physical file or folder path(s).
		/// </summary>
		/// <param name="virtualPath">Virtual file or folder path to resolve.</param>
		/// <returns>Vector filled with all paths mounted to the virtualPath.</returns>
		static std::vector<std::filesystem::path> ResolveToPhysicalPaths(const std::string& virtualPath);

		/// <summary>
		/// Read the given file.<br>
		/// <br>
		/// Prints an error if path is empty, wasn't found, doesn't exist or couldn't be opened.
		/// </summary>
		/// <param name="path">Virtual or physical file path.</param>
		/// <returns>
		/// Vector with file content on success.<br>
		/// Empty vector if an error has occurred.
		/// </returns>
		static std::vector<uint8_t> ReadFile(const std::string& path);
		/// <summary>
		/// Read the given file silently.
		/// </summary>
		/// <param name="path">Virtual or physical file path</param>
		/// <returns>
		/// Vector with file content on success.<br>
		/// Empty vector if an error has occurred.
		/// </returns>
		static std::vector<uint8_t> SilentReadFile(const std::string& path);
		/// <summary>
		/// Read the given text file.<br>
		/// <br>
		/// Prints an error if path is empty
		/// </summary>
		/// <param name="path">Virtual or physical file path</param>
		/// <returns>
		/// String with file content on success.<br>
		/// Empty string if an error has occurred.</returns>
		static std::string ReadTextFile(const std::string& path);
		/// <summary>
		/// Read the given text file silently.
		/// </summary>
		/// <param name="path">Virtual or physical file path</param>
		/// <returns>
		/// String with file content on success.<br>
		/// Empty string if an error has occurred.</returns>
		static std::string SilentReadTextFile(const std::string& path);

		/// <summary>
		/// Write the given data to the given file path.<br>
		/// Can be a virtual or a physical file path
		/// </summary>
		/// <param name="path">Virtual or physical file path</param>
		/// <param name="buffer">Data to be written</param>
		/// <param name="mode">WriteMode to use</param>
		/// <returns>If path could be resolved and data has been written true, false otherwise</returns>
		static bool WriteFile(const std::string& path, std::vector<uint8_t>& buffer, FileSystem::WriteMode mode = FileSystem::WriteMode::Overwrite);
		/// <summary>
		/// Write the given text to the given file path.<br>
		/// Can be a virtual or a physical file path
		/// </summary>
		/// <param name="path">Virtual or physical file path</param>
		/// <param name="text">Text to be written</param>
		/// <param name="mode">WriteMode to use</param>
		/// <returns>If path could be resolved and text has been written true, false otherwise</returns>
		static bool WriteTextFile(const std::string& path, const std::string& text, FileSystem::WriteMode mode = FileSystem::WriteMode::Overwrite);

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