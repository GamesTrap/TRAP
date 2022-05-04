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
	namespace FS
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
		/// </summary>
		void Init();
		/// <summary>
		/// Shuts down the File System.
		/// </summary>
		void Shutdown();

        /// <summary>
		/// Read the given binary file.
		///
		/// Prints an error if path is empty, wasn't found, doesn't exist or couldn't be opened.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <returns>
		/// Vector with file content on success.
		/// Empty vector if an error has occurred.
		/// </returns>
		std::vector<uint8_t> ReadFile(const std::filesystem::path& path);
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
		std::string ReadTextFile(const std::filesystem::path& path);

        /// <summary>
		/// Write the given data as binary to the given file path.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <param name="buffer">Data to be written.</param>
		/// <param name="mode">Write mode to use.</param>
		/// <returns>True if path could be resolved and data has been written, false otherwise.</returns>
		bool WriteFile(const std::filesystem::path& path, std::vector<uint8_t>& buffer,
		               WriteMode mode = WriteMode::Overwrite);
		/// <summary>
		/// Write the given text to the given file path.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <param name="text">Text to be written.</param>
		/// <param name="mode">Write mode to use.</param>
		/// <returns>True if path could be resolved and text has been written, false otherwise.</returns>
		bool WriteTextFile(const std::filesystem::path& path, const std::string_view text,
		                   WriteMode mode = WriteMode::Overwrite);

		/// <summary>
		/// Create a folder at the given path.
		/// </summary>
		/// <param name="path">Path to folder.</param>
		/// <returns>True on success, false otherwise.</returns>
		bool CreateFolder(const std::filesystem::path& path);

		/// <summary>
		/// Delete the file or folder at the given path.
		///
		/// Note: Folder deletion is recursive.
		/// </summary>
		/// <param name="path">File or folder to delete.</param>
		/// <returns>True on success, false otherwise.</returns>
		bool DeleteFileOrFolder(const std::filesystem::path& path);

		/// <summary>
		/// Move folder from oldPath to newPath.
		///
		/// Note: Only moves if newPath doesnt already exist.
		/// </summary>
		/// <param name="oldPath">Path to move from.</param>
		/// <param name="newPath">Path to move to.</param>
		/// <returns>True on success, false otherwise.</returns>
		bool MoveFolder(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);
		/// <summary>
		/// Move file to new folder.
		///
		/// Note: This doesn't move if file with the same filename already exists in destFolder.
		/// </summary>
		/// <param name="filePath">File to move.</param>
		/// <param name="destFolder">Folder to move file to.</param>
		/// <returns>True on success, false otherwise.</returns>
		bool MoveFile(const std::filesystem::path& filePath, const std::filesystem::path& destFolder);

		/// <summary>
		/// Rename folder from oldPath to newPath.
		/// </summary>
		/// <param name="oldPath">Folder path to rename.</param>
		/// <param name="newPath">Path to New folder name.</param>
		/// <returns>True on success, false otherwise.</returns>
		bool RenameFolder(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);
		/// <summary>
		/// Rename file excluding file extension.
		/// </summary>
		/// <param name="oldPath">File path to rename.</param>
		/// <param name="newName">New filename.</param>
		/// <returns>True on success, false otherwise.</returns>
		bool RenameFile(const std::filesystem::path& oldPath, const std::string_view newName);

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
		bool FileOrFolderExists(const std::filesystem::path& path);
		/// <summary>
		/// Get the size of an file or folder in bytes.
		/// </summary>
		/// <param name="path">Path to a file or folder.</param>
		/// <returns>
		/// File or folder size in bytes.
		/// 0 if an error has occurred.
		/// </returns>
		uintmax_t GetFileOrFolderSize(const std::filesystem::path& path, bool recursive = true);
        /// <summary>
		/// Get the last write time of a file or folder.
		/// </summary>
		/// <param name="path">Path to a file or folder.</param>
		/// <returns>
		/// Last write time of the file or folder.
		/// std::filesystem::file_time_type::min() if an error has occurred.
		/// </returns>
		std::filesystem::file_time_type GetLastWriteTime(const std::filesystem::path& path);

        /// <summary>
		/// Get only the filename without its folders from a file path.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <returns>String only containing the filename without its folders.</returns>
		std::string GetFileNameWithEnding(const std::filesystem::path& path);
        /// <summary>
		/// Get only the filename without its folders and ending/suffix from a file path.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <returns>String only containing the filename without its folders and file ending.</returns>
		std::string GetFileName(const std::filesystem::path& path);
        /// <summary>
		/// Get only the file ending without its name from a file path.
		/// </summary>
		/// <param name="path">File path.</param>
		/// <returns>String only containing the file ending without its file name.</returns>
		std::string GetFileEnding(const std::filesystem::path& path);

		/// <summary>
		/// Get only the folder without the filename and its ending.
		/// </summary>
		/// <param name="filePath">File path.</param>
		/// <returns>Folder path from file path.</returns>
		std::filesystem::path GetFolderPath(const std::filesystem::path& filePath);

        /// <summary>
		/// Get the path to the temp folder.
		/// </summary>
		/// <returns>Path to the temp folder.</returns>
		std::filesystem::path GetTempFolderPath();
        /// <summary>
		/// Get the path to the temp folder of the game.
		/// </summary>
		/// <returns>Path to the temp folder.</returns>
		std::filesystem::path GetGameTempFolderPath();
		/// <summary>
		/// Get the path to the current working folder.
		/// </summary>
		/// <returns>Path to the current working folder.</returns>
		std::filesystem::path GetCurrentFolderPath();
		/// <summary>
		/// Get the path to the users documents folder.
		/// </summary>
		/// <returns>Path to the users documents folder.</returns>
		std::filesystem::path GetDocumentsFolderPath();
		/// <summary>
		/// Get the path to the users documents folder for the game.
		/// </summary>
		/// <returns>Path to the users documents folder for the game.</returns>
		std::filesystem::path GetGameDocumentsFolderPath();

		/// <summary>
		/// Checks whether the paths p1 and p2 resolve to the same file system file/folder.
		/// </summary>
		/// <param name="p1">File/folder path.</param>
		/// <param name="p2">File/folder path</param>
		/// <returns>true if the p1 and p2 refer to the same file/folder, false otherwise.</returns>
		bool IsPathEquivalent(const std::filesystem::path& p1, const std::filesystem::path& p2);
		/// <summary>
		/// Get whether the path p is an absolute path or not.
		/// </summary>
		/// <param name="p">Path to check.</param>
		/// <returns>True if path is absolute, false otherwise.</returns>
		bool IsPathAbsolute(const std::filesystem::path& p);
		/// <summary>
		/// Get whether the path p is a relative path or not.
		/// </summary>
		/// <param name="p">Path to check.</param>
		/// <returns>True if path is relative, false otherwise.</returns>
		bool IsPathRelative(const std::filesystem::path& p);
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
		/// Converts a path to an absolute path.
		/// </summary>
		/// <param name="p">Path to convert.</param>
		/// <returns>Absolute path on success, empty path otherwise.</returns>
		std::filesystem::path ToAbsolutePath(const std::filesystem::path& p);
		/// <summary>
		/// Converts a path to a relative path.
		/// </summary>
		/// <param name="p">Path to convert.</param>
		/// <returns>Relative path on success, empty path otherwise.</returns>
		std::filesystem::path ToRelativePath(const std::filesystem::path& p);

		/// <summary>
		/// Opens the file browser at the given path.
		///
		/// Note: Linux uses xdg-open.
		/// </summary>
		/// <param name="p">Folder to open.</param>
		/// <returns>True on success, false otherwise.</returns>
		bool OpenFolderInFileBrowser(const std::filesystem::path& p);
		/// <summary>
		/// Opens the file browser and selects the given file.
		///
		/// Note: On Linux this function doesn't select/highlight the file.
		/// Note: Linux uses xdg-open.
		/// </summary>
		/// <param name="p">File to open.</param>
		/// <returns>True on success, false otherwise.</returns>
		bool OpenFileInFileBrowser(const std::filesystem::path& p);
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

#endif /*TRAP_FS_H*/