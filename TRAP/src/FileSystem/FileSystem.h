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

#ifndef TRAP_FILESYSTEM_H
#define TRAP_FILESYSTEM_H

#include <cstdint>
#include <optional>
#include <vector>
#include <filesystem>

#include "Log/Log.h"

namespace TRAP
{
	namespace FileSystem
    {
        /// <summary>
		/// Write mode to be used by writing operations.
		/// </summary>
		enum class WriteMode
		{
			Overwrite,
			Append
		};

		/// <summary>
		/// Initializes the File System.
		///
		/// This functions creates the following folders if they don't exist:
		/// - Temp/TRAP/<GameName>
		/// - Documents/TRAP/<GameName>
		/// - Documents/TRAP/<GameName>/logs (only when not using Headless mode)
		/// </summary>
		void Init();
		/// <summary>
		/// Shuts down the File System.
		/// </summary>
		constexpr void Shutdown();

        /// <summary>
		/// Read the given binary file.
		///
		/// Note: This will read the whole file into memory.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <returns>File content as std::vector<uint8_t> on success, empty std::optional otherwise.</returns>
		std::optional<std::vector<uint8_t>> ReadFile(const std::filesystem::path& path);
        /// <summary>
		/// Read the given text file.
		///
		/// Note: CRLF line endings are automatically converted to LF (i.e. '\n').
		/// </summary>
		/// <param name="path">File path.</param>
		/// <returns>File content as std::string on success, empty std::optional otherwise.</returns>
		std::optional<std::string> ReadTextFile(const std::filesystem::path& path);

        /// <summary>
		/// Write the given data as binary to the given file path.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <param name="buffer">Data to be written.</param>
		/// <param name="mode">Write mode to use. Default: WriteMode::Overwrite.</param>
		/// <returns>True if file has been written successfully, false otherwise.</returns>
		bool WriteFile(const std::filesystem::path& path, const std::vector<uint8_t>& buffer,
		               WriteMode mode = WriteMode::Overwrite);
		/// <summary>
		/// Write the given text to the given file path.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <param name="text">Text to be written.</param>
		/// <param name="mode">Write mode to use. Default: WriteMode::Overwrite.</param>
		/// <returns>True if file has been written successfully, false otherwise.</returns>
		bool WriteTextFile(const std::filesystem::path& path, const std::string_view text,
		                   WriteMode mode = WriteMode::Overwrite);

		/// <summary>
		/// Create a folder at the given path.
		///
		/// Note: This will also create all parent folders if they don't exist already.
		/// </summary>
		/// <param name="path">Path to folder.</param>
		/// <returns>True if folder has been created successfully or already exists, false otherwise.</returns>
		bool CreateFolder(const std::filesystem::path& path);

		/// <summary>
		/// Delete the given file or folder.
		///
		/// Note: Folder deletion is recursive.
		/// </summary>
		/// <param name="path">File or folder to delete.</param>
		/// <returns>True on successfull deletion, false otherwise.</returns>
		bool Delete(const std::filesystem::path& path);

		/// <summary>
		/// Move file or folder from oldPath to newPath.
		///
		/// Note: Only moves if newPath doesnt already exist.
		/// </summary>
		/// <param name="oldPath">Path to move from.</param>
		/// <param name="newPath">Path to move to.</param>
		/// <returns>True on successfull move or if oldPath and newPath are the same, false otherwise.</returns>
		bool Move(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);

		/// <summary>
		/// Copy file or folder from source to destination.
		///
		/// Note: If destination is an already existing folder, then contents will be merged.
		/// </summary>
		/// <param name="source">Source to copy from.</param>
		/// <param name="destination">Target to copy into.</param>
		/// <param name="overwriteExisting">Whether to overwrite already existing files or not.</param>
		/// <returns>True on success, false otherwise.</returns>
		bool Copy(const std::filesystem::path& source, const std::filesystem::path& destination, bool overwriteExisting = false);

		/// <summary>
		/// Rename file or folder.
		///
		/// Note: Only renames if the resulting folder doesn't already exist.
		/// </summary>
		/// <param name="oldPath">File or folder to rename.</param>
		/// <param name="newName">New name for file or folder.</param>
		/// <returns>True on success, false otherwise.</returns>
		bool Rename(const std::filesystem::path& oldPath, const std::string_view newName);

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
		bool Exists(const std::filesystem::path& path);
		/// <summary>
		/// Get the size of an file or folder in bytes.
		/// </summary>
		/// <param name="path">Path to a file or folder.</param>
		/// <param name="recursive">Recursively count file sizes. This only has an effect if path leads to a folder.</param>
		/// <returns>
		/// File or folder size in bytes.
		/// Empty optional if an error has occurred.
		/// </returns>
		std::optional<uintmax_t> GetSize(const std::filesystem::path& path, bool recursive = true);
        /// <summary>
		/// Get the last write time of a file or folder.
		/// </summary>
		/// <param name="path">Path to a file or folder.</param>
		/// <returns>
		/// Last write time of the file or folder.
		/// Empty optional if an error has occurred.
		/// </returns>
		std::optional<std::filesystem::file_time_type> GetLastWriteTime(const std::filesystem::path& path);

        /// <summary>
		/// Get only the filename without its folders from a file path.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <returns>String only containing the filename without its folders on success, empty optional otherwise.</returns>
		std::optional<std::string> GetFileNameWithEnding(const std::filesystem::path& path);
        /// <summary>
		/// Get only the filename without its folders and ending/suffix from a file path.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <returns>String only containing the filename without its folders and file ending on success, empty optional otherwise.</returns>
		std::optional<std::string> GetFileNameWithoutEnding(const std::filesystem::path& path);
        /// <summary>
		/// Get only the file ending without its name from a file path.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <returns>String only containing the file ending without its file name on success, empty optional otherwise.</returns>
		std::optional<std::string> GetFileEnding(const std::filesystem::path& path);

		/// <summary>
		/// Get only the folder without the filename and its ending.
		/// </summary>
		/// <param name="filePath">File path.</param>
		/// <returns>Folder path from file path on success, empty optional otherwise.</returns>
		std::optional<std::filesystem::path> GetFolderPath(const std::filesystem::path& filePath);

        /// <summary>
		/// Get the path to the temp folder of the engine.
		/// </summary>
		/// <returns>Path to the temp folder on success, empty optional otherwise.</returns>
		std::optional<std::filesystem::path> GetTempFolderPath();
        /// <summary>
		/// Get the path to the temp folder of the game.
		/// </summary>
		/// <returns>Path to the temp folder on success, empty optional otherwise.</returns>
		std::optional<std::filesystem::path> GetGameTempFolderPath();
		/// <summary>
		/// Get the path to the current working folder.
		/// </summary>
		/// <returns>Path to the current working folder on success, empty optional otherwise.</returns>
		std::optional<std::filesystem::path> GetCurrentFolderPath();
		/// <summary>
		/// Get the path to the users documents folder.
		/// </summary>
		/// <returns>Path to the users documents folder on success, empty optional otherwise.</returns>
		std::optional<std::filesystem::path> GetDocumentsFolderPath();
		/// <summary>
		/// Get the path to the users documents folder for the game.
		/// </summary>
		/// <returns>Path to the users documents folder for the game on success, empty optional otherwise.</returns>
		std::optional<std::filesystem::path> GetGameDocumentsFolderPath();
		/// <summary>
		/// Get the path to the log folder for the game.
		/// </summary>
		/// <returns>Path to the log folder for the game on success, empty optional otherwise.</returns>
		std::optional<std::filesystem::path> GetGameLogFolderPath();

		/// <summary>
		/// Checks whether the paths p1 and p2 resolve to the same file system file/folder.
		/// </summary>
		/// <param name="p1">File/folder path.</param>
		/// <param name="p2">File/folder path</param>
		/// <returns>True if p1 and p2 refer to the same file or folder, false otherwise.</returns>
		bool IsEquivalent(const std::filesystem::path& p1, const std::filesystem::path& p2);
		/// <summary>
		/// Get whether the path p leads to a folder or not.
		/// </summary>
		/// <param name="p">Path to check.</param>
		/// <returns>True if path leads to folder, false otherwise.</returns>
		bool IsFolder(const std::filesystem::path& p);
		/// <summary>
		/// Get whether the path p leads to a regular file or not.
		///
		/// Note: False is returned if path leads to a symlink or any other non-regular file type.
		/// </summary>
		/// <param name="p">Path to check.</param>
		/// <returns>True if path leads to regular file, false otherwise.</returns>
		bool IsFile(const std::filesystem::path& p);
		/// <summary>
		/// Get whether the path p leads to an empty file or folder.
		/// </summary>
		/// <param name="p">Path to check.</param>
		/// <returns>True if path leads to an empty file or folder, false otherwise.</returns>
		bool IsEmpty(const std::filesystem::path& p);
		/// <summary>
		/// Get whether the path is absolute or relative.
		/// </summary>
		/// <param name="p">Path to check.</param>
		/// <returns>True if path is absolute, false otherwise.</returns>
		bool IsAbsolute(const std::filesystem::path& p);
		/// <summary>
		/// Get whether the path is relative or absolute.
		/// </summary>
		/// <param name="p">Path to check.</param>
		/// <returns>True if path is relative, false otherwise.</returns>
		bool IsRelative(const std::filesystem::path& p);

		/// <summary>
		/// Converts a path to an absolute path.
		/// </summary>
		/// <param name="p">Path to convert.</param>
		/// <returns>Absolute path on success, empty optional otherwise.</returns>
		std::optional<std::filesystem::path> ToAbsolutePath(const std::filesystem::path& p);
		/// <summary>
		/// Converts a path to a relative path.
		/// </summary>
		/// <param name="p">Path to convert.</param>
		/// <returns>Relative path on success, empty optional otherwise.</returns>
		std::optional<std::filesystem::path> ToRelativePath(const std::filesystem::path& p);

		/// <summary>
		/// Change the current folder to the given path.
		/// </summary>
		/// <param name="p">Folder path to change to.</param>
		/// <returns>True on success, false otherwise.</returns>
		bool SetCurrentFolderPath(const std::filesystem::path& p);

		/// <summary>
		/// Opens the file browser at the given path.
		/// If p leads to a file, the file browser will select/hightlight the file.
		///
		/// Note: On Linux this function doesn't select/highlight the file.
		/// Note: Linux uses xdg-open.
		/// </summary>
		/// <param name="p">Path to a file or folder to open.</param>
		/// <returns>True on success, false otherwise.</returns>
		bool OpenInFileBrowser(const std::filesystem::path& p);
		/// <summary>
		/// Opens a file or folder with the default application.
		///
		/// Note: Linux uses xdg-open.
		/// </summary>
		/// <param name="p">File or folder to open.</param>
		/// <returns>True on success, false otherwise.</returns>
		bool OpenExternally(const std::filesystem::path& p);
    };
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::FileSystem::Shutdown()
{
	TP_DEBUG(Log::FileSystemPrefix, "Shutting down File System");
}

#endif /*TRAP_FILESYSTEM_H*/