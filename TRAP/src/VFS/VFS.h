#ifndef _TRAP_VFS_H_
#define _TRAP_VFS_H_

#include "Application.h"
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
		/// WriteMode to be used by writing operations
		/// </summary>
		enum class WriteMode
		{
			Overwrite,
			Append
		};

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
		/// <param name="outPhysicalPath">
		/// Output physical path.<br>
		/// Unchanged if path couldn't be resolved.</param>
		/// <param name="silent">If set to false no error messages will be logged</param>
		/// <returns>
		/// True if file or folder was found.<br>
		/// False if it wasn't found doesn't exist.<br>
		/// </returns>
		static bool ResolveReadPhysicalPath(std::string_view path, std::filesystem::path& outPhysicalPath, bool silent = false);
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
		static bool ResolveWritePhysicalPath(std::string_view path, std::filesystem::path& outPhysicalPath);
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
		/// <param name="silent">If set to false no error messages will be logged</param>
		/// <returns>
		/// Vector with file content on success.<br>
		/// Empty vector if an error has occurred.
		/// </returns>
		static std::vector<uint8_t> ReadFile(const std::string& path, bool silent = false);
		/// <summary>
		/// Read the given text file.<br>
		/// <br>
		/// Prints an error if path is empty
		/// </summary>
		/// <param name="path">Virtual or physical file path</param>
		/// <param name="silent">If set to false no error messages will be logged</param>
		/// <returns>
		/// String with file content on success.<br>
		/// Empty string if an error has occurred.
		/// </returns>
		static std::string ReadTextFile(std::string_view path, bool silent = false);

		/// <summary>
		/// Write the given data to the given file path.<br>
		/// Can be a virtual or a physical file path
		/// </summary>
		/// <param name="path">Virtual or physical file path</param>
		/// <param name="buffer">Data to be written</param>
		/// <param name="mode">WriteMode to use</param>
		/// <returns>If path could be resolved and data has been written true, false otherwise</returns>
		static bool WriteFile(const std::string& path, std::vector<uint8_t>& buffer, WriteMode mode = WriteMode::Overwrite);
		/// <summary>
		/// Write the given text to the given file path.<br>
		/// Can be a virtual or a physical file path
		/// </summary>
		/// <param name="path">Virtual or physical file path</param>
		/// <param name="text">Text to be written</param>
		/// <param name="mode">WriteMode to use</param>
		/// <returns>If path could be resolved and text has been written true, false otherwise</returns>
		static bool WriteTextFile(std::string_view path, const std::string& text, WriteMode mode = WriteMode::Overwrite);

		/// <summary>
		/// Check if a file or folder exists.<br>
		/// Can be a virtual or a physical file or folder path.<br>
		/// <br>
		/// Note: If a virtual path is provided every mounted physical path to it gets checked!
		/// <br>
		/// Prints a warning if the file or folder doesn't exist.
		/// </summary>
		/// <param name="path">Virtual or physical path to a folder or a file</param>
		/// <param name="silent">If set to false no error messages will be logged</param>
		/// <returns>
		/// True if file or folder exists.<br>
		/// False if file or folder doesn't exist.<br>
		/// False if an error has occurred.
		/// </returns>
		static bool FileOrFolderExists(const std::filesystem::path& path, bool silent = false);
		/// <summary>
		/// Get the size of an physical file or folder in bytes.<br>
		/// Can be a virtual or a physical file or folder path.<br>
		/// <br>
		/// Note: If a virtual folder path is provided only the size of the first mounted folder gets returned!<br>
		/// Note: If a virtual file path is provided every mounted physical folder path to it gets checked!
		/// </summary>
		/// <param name="path">Virtual or physical path to a file or folder</param>
		/// <returns>
		/// File or folder size in bytes.<br>
		/// 0 if an error has occurred.
		/// </returns>
		static uintmax_t GetFileOrFolderSize(const std::filesystem::path& path);
		/// <summary>
		/// Get the last write time of a file or folder.<br>
		/// Can be a virtual or a physical file or folder path.<br>
		/// <br>
		/// Note: If a virtual folder path is provided only the last write time of the first mounted folder gets returned!<br>
		/// Note: If a virtual file path is provided every mounted physical folder path to it gets checked!
		/// </summary>
		/// <param name="path">Virtual or physical path to a file or folder</param>
		/// <returns>
		/// Last write time of the file or folder.<br>
		/// std::filesystem::file_time_type::min() if an error has occurred.
		/// </returns>
		static std::filesystem::file_time_type GetLastWriteTime(const std::filesystem::path& path);

		/// <summary>
		/// Get status of hot shader reloading.
		/// </summary>
		/// <returns>True if hot shader reloading is enabled, False otherwise.</returns>
		static bool GetHotShaderReloading();
		/// <summary>
		/// Set status of hot shader reloading.
		/// </summary>
		/// <param name="enabled">Whether to enable or disable hot shader reloading.</param>
		static void SetHotShaderReloading(bool enabled);

		/// <summary>
		/// Get status of hot texture reloading.
		/// </summary>
		/// <returns>True if hot texture reloading is enabled, False otherwise.</returns>
		static bool GetHotTextureReloading();
		/// <summary>
		/// Set status of hot texture reloading.
		/// </summary>
		/// <param name="enabled">Whether to enable or disable hot texture reloading.</param>
		static void SetHotTextureReloading(bool enabled);

		/// <summary>
		/// Makes user written virtual paths compatible with the Virtual File System<br>
		/// <br>
		/// NOTE: Physical path won't be affected by this and just return the same as virtualPath.
		/// </summary>
		/// <param name="virtualPath">Path to make compatible</param>
		/// <returns>String with a compatible path for the Virtual File System</returns>
		static std::string MakeVirtualPathCompatible(std::string_view virtualPath);
		/// <summary>
		/// Get only the filename without its file ending from a virtual or physical file path.
		/// </summary>
		/// <param name="virtualOrPhysicalPath">Virtual or Physical file path.</param>
		/// <returns>String only containing the filename without its file ending.</returns>
		static std::string GetFileName(const std::string& virtualOrPhysicalPath);

	private:
		/// <summary>
		/// Initializes the Virtual File System
		/// </summary>
		static void Init();
		/// <summary>
		/// Shuts down the Virtual File System
		/// </summary>
		static void Shutdown();
		friend TRAP::Application::Application();
		friend TRAP::Application::~Application();

		/// <summary>
		/// Get a pointer to the FileWatcher instance used for hot shader reloading.
		/// </summary>
		/// <returns>Pointer to the hot shader reloading FileWatcher.</returns>
		static FileWatcher* GetShaderFileWatcher();
		/// <summary>
		/// Get a pointer to the FileWatcher instance used for hot texture reloading.
		/// </summary>
		/// <returns>Pointer to the hot texture reloading FileWatcher.</returns>
		static FileWatcher* GetTextureFileWatcher();
		friend static void TRAP::Application::ProcessHotReloading(std::vector<std::string>& shaders, std::vector<std::string>& textures, const bool& running);
		
		/// <summary>
		/// Read the given physical file.<br>
		/// <br>
		/// Prints an error when file couldn't be opened.<br>
		/// Prints an error when physicalFilePath doesn't exist.
		/// </summary>
		/// <param name="physicalFilePath">Physical file path</param>
		/// <param name="silent">If set to false no error messages will be logged</param>
		/// <returns>
		/// Vector filled with the file content.<br>
		/// Empty vector if an error has occurred.
		/// </returns>
		static std::vector<uint8_t> ReadPhysicalFile(const std::filesystem::path& physicalFilePath, bool silent = false);
		
		/// <summary>
		/// Read the given physical file as text.<br>
		/// <br>
		/// Prints an error if physicalFilePath doesn't exist.<br>
		/// Prints an error if physicalFilePath couldn't be opened.
		/// </summary>
		/// <param name="physicalFilePath">Physical file path</param>
		/// <param name="silent">If set to false no error messages will be logged</param>
		/// <returns>
		/// String filled with the file content.<br>
		/// Empty string if an error has occurred.
		/// </returns>
		static std::string ReadPhysicalTextFile(const std::filesystem::path& physicalFilePath, bool silent = false);

		/// <summary>
		/// Write the given data to the given physical file path.<br>
		/// <br>
		/// Prints an error if physicalFilePath and/or buffer is empty.<br>
		/// Prints an error if file couldn't be written.
		/// </summary>
		/// <param name="physicalFilePath">Physical file path to be written to</param>
		/// <param name="buffer">Data to be written</param>
		/// <param name="mode">WriteMode to be used (Overwrite or Append)</param>
		/// <returns>
		/// True if file was successfully written.<br>
		/// False if an error has occurred.
		/// </returns>
		static bool WritePhysicalFile(const std::filesystem::path& physicalFilePath, std::vector<uint8_t>& buffer, WriteMode mode = WriteMode::Overwrite);
		/// <summary>
		/// Write the given text to the given physical file path.<br>
		/// <br>
		/// Prints an error if physicalFilePath and/or text is empty.<br>
		/// Prints an error if file couldn't be written.
		/// </summary>
		/// <param name="physicalFilePath">Physical file path to be written to</param>
		/// <param name="text">Text to be written</param>
		/// <param name="mode">WriteMode to be used (Overwrite or Append)</param>
		/// <returns>
		/// True if file was successfully written.<br>
		/// False if an error has occurred.
		/// </returns>
		static bool WritePhysicalTextFile(const std::filesystem::path& physicalFilePath, std::string_view text, WriteMode mode = WriteMode::Overwrite);
		
		static Scope<VFS> s_Instance;
		
		std::unordered_map<std::string, std::vector<std::string>> m_mountPoints;

		bool m_hotShaderReloading = false;
		Scope<FileWatcher> m_shaderFileWatcher;

		bool m_hotTextureReloading = false;
		Scope<FileWatcher> m_textureFileWatcher;
	};
}

#endif /*_TRAP_VFS_H_*/