#ifndef TRAP_VFS_H
#define TRAP_VFS_H

#include "Application.h"
#include "FileWatcher.h"

namespace TRAP
{
	class VFS final
	{
	private:
		/// <summary>
		/// Constructor.
		/// </summary>
		VFS() = default;

	public:
		/// <summary>
		///	Destructor.
		/// </summary>
		~VFS() = default;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VFS(const VFS&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VFS& operator=(const VFS&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VFS(VFS&&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VFS& operator=(VFS&&) = delete;

	public:
		/// <summary>
		/// Write mode to be used by writing operations.
		/// </summary>
		enum class WriteMode
		{
			Overwrite,
			Append
		};

		/// <summary>
		/// Mount a physical folder path to a virtual folder path.
		/// Subfolders need to be mounted separately.
		/// NOTE: Physical path should use forward slashes and not back slashes!
		/// NOTE: Virtual file paths always use lowercase (except for the filename and its ending).
		///
		/// If hot shader reloading is enabled and the virtual path is "/shaders" then this also instantiates the
		/// shader TRAP::FileWatcher.
		/// If hot texture reloading is enabled and the virtual path is "/textures" then this also instantiates the
		/// texture TRAP::FileWatcher.
		///
		/// Prints an error if either virtual path or physical path is empty or invalid.
		/// </summary>
		/// <param name="virtualPath">Virtual folder path to be bound to.</param>
		/// <param name="physicalPath">Physical folder path.</param>
		static void Mount(const std::string& virtualPath, const std::string& physicalPath);
		/// <summary>
		/// Mount a physical folder path to the virtual folder path "/shaders".
		///
		/// If hot shader reloading is enabled and the virtual path is "/shaders" then this also instantiates the
		/// shader TRAP::FileWatcher.
		/// <br>
		/// Prints an error if physical path is empty.
		/// </summary>
		/// <param name="physicalPath">Physical folder path.</param>
		static void MountShaders(const std::string& physicalPath);
		/// <summary>
		/// Mount a physical folder path to the virtual folder path "/textures".
		///
		/// If hot texture reloading is enabled and the virtual path is "/textures" then this also instantiates the
		/// texture TRAP::FileWatcher.
		///
		/// Prints an error if physical path is empty.
		/// </summary>
		/// <param name="physicalPath">Physical folder path.</param>
		static void MountTextures(const std::string& physicalPath);
		/// <summary>
		/// Unmount a virtual folder path.
		///
		/// Prints an error if virtual path is empty.
		///
		/// If virtual path is "/shaders" and hot shader reloading is enabled and the shader TRAP::FileWatcher
		/// is already instantiated then this also stops it.
		/// If virtual path is "/textures" and hot texture reloading is enabled and the Texture TRAP::FileWatcher
		/// is already instantiated then this also stops it.
		/// </summary>
		/// <param name="virtualPath">Virtual folder path to unmount.</param>
		static void Unmount(const std::string& virtualPath);
		/// <summary>
		/// Resolve a virtual file or folder path to its physical file or folder path.
		///
		/// Prints an error if path is empty, wasn't found or doesn't exist.
		/// </summary>
		/// <param name="path">Virtual or physical file or folder path to resolve.</param>
		/// <param name="outPhysicalPath">
		/// Output physical path.
		/// Unchanged if path couldn't be resolved.</param>
		/// <param name="silent">If set to false no error messages will be logged.</param>
		/// <returns>
		/// True if file or folder was found.
		/// False if it wasn't found doesn't exist.
		/// </returns>
		static bool ResolveReadPhysicalPath(std::string_view path, std::filesystem::path& outPhysicalPath,
		                                    bool silent = false);
		/// <summary>
		/// Resolve a virtual file or folder path to its physical file or folder path.
		///
		/// If path already is a physical path then it will just be written to outPhysicalPath without changes.
		/// Prints an error if path is empty.
		/// </summary>
		/// <param name="path">Virtual or physical file or folder path to resolve.</param>
		/// <param name="outPhysicalPath">
		/// Output physical path.<br>
		/// Unchanged if path couldn't be resolved.
		/// </param>
		/// <returns>
		/// True if file or folder was found.
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
		/// Read the given file.
		///
		/// Prints an error if path is empty, wasn't found, doesn't exist or couldn't be opened.
		/// </summary>
		/// <param name="path">Virtual or physical file path.</param>
		/// <param name="silent">If set to false no error messages will be logged.</param>
		/// <returns>
		/// Vector with file content on success.
		/// Empty vector if an error has occurred.
		/// </returns>
		static std::vector<uint8_t> ReadFile(const std::string& path, bool silent = false);
		/// <summary>
		/// Read the given text file.
		///
		/// Prints an error if path is empty.
		/// </summary>
		/// <param name="path">Virtual or physical file path.</param>
		/// <param name="silent">If set to false no error messages will be logged.</param>
		/// <returns>
		/// String with file content on success.
		/// Empty string if an error has occurred.
		/// </returns>
		static std::string ReadTextFile(std::string_view path, bool silent = false);

		/// <summary>
		/// Write the given data to the given file path.
		/// Can be a virtual or a physical file path.
		/// </summary>
		/// <param name="path">Virtual or physical file path.</param>
		/// <param name="buffer">Data to be written.</param>
		/// <param name="mode">Write mode to use.</param>
		/// <returns>If path could be resolved and data has been written true, false otherwise.</returns>
		static bool WriteFile(const std::string& path, std::vector<uint8_t>& buffer,
		                      WriteMode mode = WriteMode::Overwrite);
		/// <summary>
		/// Write the given text to the given file path.
		/// Can be a virtual or a physical file path.
		/// </summary>
		/// <param name="path">Virtual or physical file path.</param>
		/// <param name="text">Text to be written.</param>
		/// <param name="mode">Write mode to use.</param>
		/// <returns>If path could be resolved and text has been written true, false otherwise.</returns>
		static bool WriteTextFile(std::string_view path, const std::string& text,
		                          WriteMode mode = WriteMode::Overwrite);

		/// <summary>
		/// Check if a file or folder exists.
		/// Can be a virtual or a physical file or folder path.
		///
		/// Note: If a virtual path is provided every mounted physical path to it gets checked!
		///
		/// Prints a warning if the file or folder doesn't exist.
		/// </summary>
		/// <param name="path">Virtual or physical path to a folder or a file.</param>
		/// <param name="silent">If set to false no error messages will be logged.</param>
		/// <returns>
		/// True if file or folder exists.
		/// False if file or folder doesn't exist.
		/// False if an error has occurred.
		/// </returns>
		static bool FileOrFolderExists(const std::filesystem::path& path, bool silent = false);
		/// <summary>
		/// Get the size of an physical file or folder in bytes.
		/// Can be a virtual or a physical file or folder path.
		///
		/// Note: If a virtual folder path is provided only the size of the first mounted folder gets returned!
		/// Note: If a virtual file path is provided every mounted physical folder path to it gets checked!
		/// </summary>
		/// <param name="path">Virtual or physical path to a file or folder.</param>
		/// <returns>
		/// File or folder size in bytes.
		/// 0 if an error has occurred.
		/// </returns>
		static uintmax_t GetFileOrFolderSize(const std::filesystem::path& path);
		/// <summary>
		/// Get the last write time of a file or folder.
		/// Can be a virtual or a physical file or folder path.
		///
		/// Note: If a virtual folder path is provided only the last write time of the first
		///       mounted folder gets returned!
		/// Note: If a virtual file path is provided every mounted physical folder path to it gets checked!
		/// </summary>
		/// <param name="path">Virtual or physical path to a file or folder.</param>
		/// <returns>
		/// Last write time of the file or folder.
		/// std::filesystem::file_time_type::min() if an error has occurred.
		/// </returns>
		static std::filesystem::file_time_type GetLastWriteTime(const std::filesystem::path& path);

		/// <summary>
		/// Get status of hot shader reloading.
		/// </summary>
		/// <returns>True if hot shader reloading is enabled, false otherwise.</returns>
		static bool GetHotShaderReloading();
		/// <summary>
		/// Set status of hot shader reloading.
		/// </summary>
		/// <param name="enabled">Whether to enable or disable hot shader reloading.</param>
		static void SetHotShaderReloading(bool enabled);

		/// <summary>
		/// Get status of hot texture reloading.
		/// </summary>
		/// <returns>True if hot texture reloading is enabled, false otherwise.</returns>
		static bool GetHotTextureReloading();
		/// <summary>
		/// Set status of hot texture reloading.
		/// </summary>
		/// <param name="enabled">Whether to enable or disable hot texture reloading.</param>
		static void SetHotTextureReloading(bool enabled);

		/// <summary>
		/// Makes user written virtual paths compatible with the Virtual File System.
		///
		/// NOTE: Physical path won't be affected by this and just return the same as virtual path.
		/// </summary>
		/// <param name="virtualPath">Path to make compatible.</param>
		/// <returns>String with a compatible path for the Virtual File System.</returns>
		static std::string MakeVirtualPathCompatible(std::string_view virtualPath);
		/// <summary>
		/// Get only the filename without its file ending from a virtual or physical file path.
		/// </summary>
		/// <param name="virtualOrPhysicalPath">Virtual or physical file path.</param>
		/// <returns>String only containing the filename without its file ending.</returns>
		static std::string GetFileName(const std::string& virtualOrPhysicalPath);

		/// <summary>
		/// Get the path to the temp folder.
		/// </summary>
		/// <returns>Path to the temp folder.</returns>
		static std::string GetTempFolderPath();
		/// <summary>
		/// Get the path to the current working folder.
		/// </summary>
		/// <returns>Path to the current working folder.</returns>
		static std::string GetCurrentFolderPath();
		/// <summary>
		/// Get the path to the users documents folder.
		/// </summary>
		/// <returns>Path to the users documents folder.</returns>
		static std::string GetDocumentsFolderPath();

	private:
		/// <summary>
		/// Initializes the Virtual File System.
		/// </summary>
		static void Init();
		/// <summary>
		/// Shuts down the Virtual File System.
		/// </summary>
		static void Shutdown();
		friend TRAP::Application::Application(const std::string&);
		friend TRAP::Application::~Application();

		/// <summary>
		/// Get a pointer to the file watcher instance used for hot shader reloading.
		/// </summary>
		/// <returns>Pointer to the hot shader reloading file watcher.</returns>
		static FileWatcher* GetShaderFileWatcher();
		/// <summary>
		/// Get a pointer to the file watcher instance used for hot texture reloading.
		/// </summary>
		/// <returns>Pointer to the hot texture reloading file watcher.</returns>
		static FileWatcher* GetTextureFileWatcher();
		friend void TRAP::Application::ProcessHotReloading(std::vector<std::string>& shaders,
		                                                   std::vector<std::string>& textures, const bool& run);

		/// <summary>
		/// Read the given physical file.
		///
		/// Prints an error when file couldn't be opened.
		/// Prints an error when the physical file path doesn't exist.
		/// </summary>
		/// <param name="physicalFilePath">Physical file path.</param>
		/// <param name="silent">If set to false no error messages will be logged.</param>
		/// <returns>
		/// Vector filled with the file content.
		/// Empty vector if an error has occurred.
		/// </returns>
		static std::vector<uint8_t> ReadPhysicalFile(const std::filesystem::path& physicalFilePath,
		                                             bool silent = false);

		/// <summary>
		/// Read the given physical file as text.
		///
		/// Prints an error if physical file path doesn't exist.
		/// Prints an error if physical file path couldn't be opened.
		/// </summary>
		/// <param name="physicalFilePath">Physical file path.</param>
		/// <param name="silent">If set to false no error messages will be logged.</param>
		/// <returns>
		/// String filled with the file content.
		/// Empty string if an error has occurred.
		/// </returns>
		static std::string ReadPhysicalTextFile(const std::filesystem::path& physicalFilePath, bool silent = false);

		/// <summary>
		/// Write the given data to the given physical file path.
		///
		/// Prints an error if physical file path and/or buffer is empty.
		/// Prints an error if file couldn't be written.
		/// </summary>
		/// <param name="physicalFilePath">Physical file path to be written to.</param>
		/// <param name="buffer">Data to be written.</param>
		/// <param name="mode">Write mode to be used (Overwrite or Append).</param>
		/// <returns>
		/// True if file was successfully written.
		/// False if an error has occurred.
		/// </returns>
		static bool WritePhysicalFile(const std::filesystem::path& physicalFilePath, std::vector<uint8_t>& buffer,
		                              WriteMode mode = WriteMode::Overwrite);
		/// <summary>
		/// Write the given text to the given physical file path.
		///
		/// Prints an error if physical file path and/or text is empty.
		/// Prints an error if file couldn't be written.
		/// </summary>
		/// <param name="physicalFilePath">Physical file path to be written to.</param>
		/// <param name="text">Text to be written.</param>
		/// <param name="mode">Write mode to be used (overwrite or append).</param>
		/// <returns>
		/// True if file was successfully written.
		/// False if an error has occurred.
		/// </returns>
		static bool WritePhysicalTextFile(const std::filesystem::path& physicalFilePath, std::string_view text,
		                                  WriteMode mode = WriteMode::Overwrite);

		static Scope<VFS> s_Instance;

		std::unordered_map<std::string, std::vector<std::string>> m_mountPoints;

		bool m_hotShaderReloading = false;
		Scope<FileWatcher> m_shaderFileWatcher;

		bool m_hotTextureReloading = false;
		Scope<FileWatcher> m_textureFileWatcher;
	};
}

#endif /*TRAP_VFS_H*/