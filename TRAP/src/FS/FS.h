// The MIT License (MIT)

// Copyright (c) 2015 Poul Sander

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//Note: The above copyright notice only applies for the following functions:
//GetHomeFolderPathLinux & GetDocumentsFolderPathLinux

//Modified by Jan "GamesTrap" Schuerkamp

#ifndef TRAP_FS_H
#define TRAP_FS_H

#include "Application.h"
#include "FileWatcher.h"

namespace TRAP
{
    class FS final //TODO Make this a namespace
    {
    private:
        /// <summary>
		/// Constructor.
		/// </summary>
		FS() = default;

	public:
		/// <summary>
		///	Destructor.
		/// </summary>
		~FS() = default;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		FS(const FS&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		FS& operator=(const FS&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		FS(FS&&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		FS& operator=(FS&&) = delete;

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
		/// Read the given file.
		///
		/// Prints an error if path is empty, wasn't found, doesn't exist or couldn't be opened.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <returns>
		/// Vector with file content on success.
		/// Empty vector if an error has occurred.
		/// </returns>
		static std::vector<uint8_t> ReadFile(const std::filesystem::path& path);
        /// <summary>
		/// Read the given text file.
		///
		/// Prints an error if path is empty.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <returns>
		/// String with file content on success.
		/// Empty string if an error has occurred.
		/// </returns>
		static std::string ReadTextFile(const std::filesystem::path& path);

        /// <summary>
		/// Write the given data to the given file path.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <param name="buffer">Data to be written.</param>
		/// <param name="mode">Write mode to use.</param>
		/// <returns>True if path could be resolved and data has been written, false otherwise.</returns>
		static bool WriteFile(const std::filesystem::path& path, std::vector<uint8_t>& buffer,
		                      WriteMode mode = WriteMode::Overwrite);
		/// <summary>
		/// Write the given text to the given file path.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <param name="text">Text to be written.</param>
		/// <param name="mode">Write mode to use.</param>
		/// <returns>True if path could be resolved and text has been written, false otherwise.</returns>
		static bool WriteTextFile(const std::filesystem::path& path, const std::string_view text,
		                          WriteMode mode = WriteMode::Overwrite);

        /// <summary>
		/// Check if a file or folder exists.
		///
		/// Prints a warning if the file or folder doesn't exist.
		/// </summary>
		/// <param name="path">Path to a folder or a file.</param>
		/// <returns>
		/// True if file or folder exists.
		/// False if file or folder doesn't exist or an error has occurred.
		/// </returns>
		static bool FileOrFolderExists(const std::filesystem::path& path);
		/// <summary>
		/// Get the size of an file or folder in bytes.
		/// </summary>
		/// <param name="path">Path to a file or folder.</param>
		/// <returns>
		/// File or folder size in bytes.
		/// 0 if an error has occurred.
		/// </returns>
		static uintmax_t GetFileOrFolderSize(const std::filesystem::path& path, bool recursive = true);
        /// <summary>
		/// Get the last write time of a file or folder.
		/// </summary>
		/// <param name="path">Path to a file or folder.</param>
		/// <returns>
		/// Last write time of the file or folder.
		/// std::filesystem::file_time_type::min() if an error has occurred.
		/// </returns>
		static std::filesystem::file_time_type GetLastWriteTime(const std::filesystem::path& path);

        /// <summary>
		/// Get status of hot shader reloading.
		/// </summary>
		/// <returns>True if hot shader reloading is enabled, false otherwise.</returns>
        [[deprecated("Will soon be replaced by TRAP::FS::GetHotReloading()")]]
		static bool GetHotShaderReloading();
		/// <summary>
		/// Set status of hot shader reloading.
		/// </summary>
		/// <param name="enabled">Whether to enable or disable hot shader reloading.</param>
        [[deprecated("Will soon be replaced by TRAP::FS::SetHotReloading()")]]
		static void SetHotShaderReloading(bool enabled);

        /// <summary>
		/// Get status of hot texture reloading.
		/// </summary>
		/// <returns>True if hot texture reloading is enabled, false otherwise.</returns>
        [[deprecated("Will soon be replaced by TRAP::FS::GetHotReloading()")]]
		static bool GetHotTextureReloading();
		/// <summary>
		/// Set status of hot texture reloading.
		/// </summary>
		/// <param name="enabled">Whether to enable or disable hot texture reloading.</param>
        [[deprecated("Will soon be replaced by TRAP::FS::SetHotReloading()")]]
		static void SetHotTextureReloading(bool enabled);

        /// <summary>
		/// Get only the filename without its folders from a file path.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <returns>String only containing the filename without its folders.</returns>
		static std::string GetFileNameWithEnding(const std::filesystem::path& path);
        /// <summary>
		/// Get only the filename without its folders and ending/suffix from a file path.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <returns>String only containing the filename without its folders and file ending.</returns>
		static std::string GetFileName(const std::filesystem::path& path);
        /// <summary>
		/// Get only the file ending without its name from a file path.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <returns>String only containing the file ending without its file name.</returns>
		static std::string GetFileEnding(const std::filesystem::path& path);

		/// <summary>
		/// Get only the folders without the filename and its ending.
		/// </summary>
		/// <param name="filePath">File path.</param>
		/// <returns>Folder path from file path.</returns>
		static std::filesystem::path GetFolderPath(const std::filesystem::path& filePath);

        /// <summary>
		/// Get the path to the temp folder.
		/// </summary>
		/// <returns>Path to the temp folder.</returns>
		static std::filesystem::path GetTempFolderPath();
        /// <summary>
		/// Get the path to the temp folder of the game.
		/// </summary>
		/// <returns>Path to the temp folder.</returns>
		static std::filesystem::path GetGameTempFolderPath();
		/// <summary>
		/// Get the path to the current working folder.
		/// </summary>
		/// <returns>Path to the current working folder.</returns>
		static std::filesystem::path GetCurrentFolderPath();
		/// <summary>
		/// Get the path to the users documents folder.
		/// </summary>
		/// <returns>Path to the users documents folder.</returns>
		static std::filesystem::path GetDocumentsFolderPath();
		/// <summary>
		/// Get the path to the users documents folder for the game.
		/// </summary>
		/// <returns>Path to the users documents folder for the game.</returns>
		static std::filesystem::path GetGameDocumentsFolderPath();

    private:
		/// <summary>
		/// Initializes the File System.
		/// </summary>
		static void Init();
		/// <summary>
		/// Shuts down the File System.
		/// </summary>
		static void Shutdown();
		friend TRAP::Application::Application(std::string);
		friend TRAP::Application::~Application();

#ifdef TRAP_PLATFORM_LINUX
		/// <summary>
		/// Retrieves the effective user's home dir.
		/// If the user is running as root we ignore the HOME environment.
		/// It works badly with sudo.
		///
		/// Note: Writing to $HOME as root implies security concerns that a multiplatform program
		///       cannot be assumed to handle.
		/// </summary>
		/// <returns>
		/// The home directory.
		/// HOME environment is respected for non-root users if it exists.
		/// </returns>
		static std::string GetHomeFolderPathLinux();
		static std::string GetDocumentsFolderPathLinux();
#endif

        /// <summary>
		/// Get a pointer to the file watcher instance used for hot shader reloading.
		/// </summary>
		/// <returns>Pointer to the hot shader reloading file watcher.</returns>
        [[deprecated("Will soon be removed")]]
		static FileWatcher* GetShaderFileWatcher();
		/// <summary>
		/// Get a pointer to the file watcher instance used for hot texture reloading.
		/// </summary>
		/// <returns>Pointer to the hot texture reloading file watcher.</returns>
		static FileWatcher* GetTextureFileWatcher();
        [[deprecated("Will soon be removed")]]
        friend void TRAP::Application::ProcessHotReloading(std::vector<std::string>& shaders,
		                                                   std::vector<std::string>& textures, const bool& run);
    };
}

#endif /*TRAP_FS_H*/