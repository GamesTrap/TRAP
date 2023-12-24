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
#include <vector>
#include <filesystem>

#include "Core/Types.h"
#include "Utils/Optional.h"

namespace TRAP::FileSystem
{
	/// @brief Write mode to be used by writing operations.
	enum class WriteMode
	{
		Overwrite,
		Append
	};

	/// @brief Initializes the File System.
	///
	/// This functions creates the following folders if they don't exist:
	/// - Temp/TRAP/<GameName>
	/// - Documents/TRAP/<GameName> (only when not using Headless mode)
	/// - Documents/TRAP/<GameName>/logs (only when not using Headless mode)
	void Init();
	/// @brief Shuts down the File System.
	void Shutdown();

	/// @brief Read the given binary file.
	/// @param path File path.
	/// @return File content as std::vector<u8> on success, empty std::optional otherwise.
	/// @note This will read the whole file into memory, only use this for small files.
	[[nodiscard]] TRAP::Optional<std::vector<u8>> ReadFile(const std::filesystem::path& path);
	/// @brief Read the given text file.
	/// @param path File path.
	/// @return File content as std::string on success, empty std::optional otherwise.
	/// @note CRLF line endings are automatically converted to LF (i.e. '\n').
	/// @note This will read the whole file into memory, only use this for small files.
	[[nodiscard]] TRAP::Optional<std::string> ReadTextFile(const std::filesystem::path& path);

	/// @brief Write the given data as binary to the given file path.
	/// @param path File path.
	/// @param buffer Data to be written.
	/// @param mode Write mode to use. Default: WriteMode::Overwrite.
	/// @return True if file has been written successfully, false otherwise.
	bool WriteFile(const std::filesystem::path& path, const std::vector<u8>& buffer,
					WriteMode mode = WriteMode::Overwrite);
	/// @brief Write the given text to the given file path.
	/// @param path File path.
	/// @param text Text to be written.
	/// @param mode Write mode to use. Default: WriteMode::Overwrite.
	/// @return True if file has been written successfully, false otherwise.
	bool WriteTextFile(const std::filesystem::path& path, std::string_view text,
						WriteMode mode = WriteMode::Overwrite);

	/// @brief Create a folder at the given path.
	/// @param path Path to folder.
	/// @return True if folder has been created successfully or already exists, false otherwise.
	/// @note This will also create all parent folders if they don't exist already.
	bool CreateFolder(const std::filesystem::path& path);

	/// @brief Delete the given file or folder.
	/// @param path File or folder to delete.
	/// @return True on successfull deletion, false otherwise.
	/// @note Folder deletion is recursive.
	bool Delete(const std::filesystem::path& path);

	/// @brief Move file or folder from oldPath to newPath.
	/// @param oldPath Path to move from.
	/// @param newPath Path to move to.
	/// @return True on successfull move or if oldPath and newPath are the same, false otherwise.
	/// @note Only moves if newPath doesnt already exist.
	bool Move(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);

	/// @brief Copy file or folder from source to destination.
	/// @param source Source to copy from.
	/// @param destination Target to copy into.
	/// @param overwriteExisting Whether to overwrite already existing files or not.
	/// @return True on success, false otherwise.
	/// @note If destination is an already existing folder, then contents will be merged.
	bool Copy(const std::filesystem::path& source, const std::filesystem::path& destination, bool overwriteExisting = false);

	/// @brief Rename file or folder.
	/// @param oldPath File or folder to rename.
	/// @param newName New name for file or folder.
	/// @return True on success, false otherwise.
	/// @note Only renames if the resulting folder doesn't already exist.
	bool Rename(const std::filesystem::path& oldPath, std::string_view newName);

	/// @brief Check if a file or folder exists.
	///        Prints a warning if the file or folder doesn't exist.
	/// @param path Path to a folder or a file.
	/// @return True if file or folder exists. False if file or folder doesn't exist or an error has occurred.
	[[nodiscard]] bool Exists(const std::filesystem::path& path);
	/// @brief Get the size of an file or folder in bytes.
	/// @param path Path to a file or folder.
	/// @param recursive Recursively count file sizes. This only has an effect if path leads to a folder.
	/// @return File or folder size in bytes. Empty optional if an error has occurred.
	[[nodiscard]] TRAP::Optional<uintmax_t> GetSize(const std::filesystem::path& path, bool recursive = true);
	/// @brief Get the last write time of a file or folder.
	/// @param path Path to a file or folder.
	/// @return Last write time of the file or folder. Empty optional if an error has occurred.
	[[nodiscard]] TRAP::Optional<std::filesystem::file_time_type> GetLastWriteTime(const std::filesystem::path& path);

	/// @brief Get only the filename without its folders from a file path.
	/// @param path File path.
	/// @return String only containing the filename without its folders on success, empty optional otherwise.
	[[nodiscard]] TRAP::Optional<std::string> GetFileNameWithEnding(const std::filesystem::path& path);
	/// @brief Get only the filename without its folders and ending/suffix from a file path.
	/// @param path File path.
	/// @return String only containing the filename without its folders and file ending on success, empty optional otherwise.
	[[nodiscard]] TRAP::Optional<std::string> GetFileNameWithoutEnding(const std::filesystem::path& path);
	/// @brief Get only the file ending without its name from a file path.
	/// @param path File path.
	/// @return String only containing the file ending without its file name on success, empty optional otherwise.
	[[nodiscard]] TRAP::Optional<std::string> GetFileEnding(const std::filesystem::path& path);

	/// @brief Get only the folder without the filename and its ending.
	/// @param filePath File path.
	/// @return Folder path from file path on success, empty optional otherwise.
	[[nodiscard]] TRAP::Optional<std::filesystem::path> GetFolderPath(const std::filesystem::path& filePath);

	/// @brief Get the path to the temp folder of the engine.
	/// @return Path to the temp folder on success, empty optional otherwise.
	[[nodiscard]] TRAP::Optional<std::filesystem::path> GetTempFolderPath();
	/// @brief Get the path to the temp folder of the game.
	/// @return Path to the temp folder on success, empty optional otherwise.
	[[nodiscard]] TRAP::Optional<std::filesystem::path> GetGameTempFolderPath();
	/// @brief Get the path to the current working folder.
	/// @return Path to the current working folder on success, empty optional otherwise.
	[[nodiscard]] TRAP::Optional<std::filesystem::path> GetCurrentFolderPath();
	/// @brief Get the path to the users documents folder.
	/// @return Path to the users documents folder on success, empty optional otherwise.
	[[nodiscard]] TRAP::Optional<std::filesystem::path> GetDocumentsFolderPath();
	/// @brief Get the path to the users documents folder for the game.
	/// @return Path to the users documents folder for the game on success, empty optional otherwise.
	[[nodiscard]] TRAP::Optional<std::filesystem::path> GetGameDocumentsFolderPath();
	/// @brief Get the path to the log folder for the game.
	/// @return Path to the log folder for the game on success, empty optional otherwise.
	[[nodiscard]] TRAP::Optional<std::filesystem::path> GetGameLogFolderPath();

	/// @brief Checks whether the paths p1 and p2 resolve to the same file system file/folder.
	/// @param p1 File/folder path.
	/// @param p2 File/folder path
	/// @return True if p1 and p2 refer to the same file or folder, false otherwise.
	[[nodiscard]] bool IsEquivalent(const std::filesystem::path& p1, const std::filesystem::path& p2);
	/// @brief Get whether the path p leads to a folder or not.
	/// @param p Path to check.
	/// @return True if path leads to folder, false otherwise.
	[[nodiscard]] bool IsFolder(const std::filesystem::path& p);
	/// @brief Get whether the path p leads to a regular file or not.
	/// @param p Path to check.
	/// @return True if path leads to regular file, false otherwise.
	/// @note False is returned if path leads to a symlink or any other non-regular file type.
	[[nodiscard]] bool IsFile(const std::filesystem::path& p);
	/// @brief Get whether the path p leads to an empty file or folder.
	/// @param p Path to check.
	/// @return True if path leads to an empty file or folder, false otherwise.
	[[nodiscard]] bool IsEmpty(const std::filesystem::path& p);
	/// @brief Get whether the path is absolute or relative.
	/// @param p Path to check.
	/// @return True if path is absolute, false otherwise.
	[[nodiscard]] bool IsAbsolute(const std::filesystem::path& p);
	/// @brief Get whether the path is relative or absolute.
	/// @param p Path to check.
	/// @return True if path is relative, false otherwise.
	[[nodiscard]] bool IsRelative(const std::filesystem::path& p);

	/// @brief Converts a path to an absolute path.
	/// @param p Path to convert.
	/// @return Absolute path on success, empty optional otherwise.
	[[nodiscard]] TRAP::Optional<std::filesystem::path> ToAbsolutePath(const std::filesystem::path& p);
	/// @brief Converts a path to a canonical absolute path, i.e. an absolute path that has no dot, dot-dot
	///        elements or symbolic links in its generic format representation.
	/// @param p Existing path to convert.
	/// @return Canonical absolute path that resolves to the same file as ToAbsolutePath(p) on success, empty optional otherwise.
	[[nodiscard]] TRAP::Optional<std::filesystem::path> ToCanonicalAbsolutePath(const std::filesystem::path& p);
	/// @brief Converts a path to a relative path.
	/// @param p Path to convert.
	/// @param base Base path, against which p will be made relative.
	/// @return Relative path on success, empty optional otherwise.
	[[nodiscard]] TRAP::Optional<std::filesystem::path> ToRelativePath(const std::filesystem::path& p,
	                                                                   const std::filesystem::path& base = std::filesystem::current_path());

	/// @brief Change the current folder to the given path.
	/// @param p Folder path to change to.
	/// @return True on success, false otherwise.
	bool SetCurrentWorkingFolderPath(const std::filesystem::path& p);
	/// @brief Change the current working folder to the given path.
	/// @param p Path to change the current working folder to.
	/// @return Current working folder on success, empty optional otherwise.
	[[nodiscard]] TRAP::Optional<std::filesystem::path> GetCurrentWorkingFolderPath();

	/// @brief Opens the file browser at the given path.
	///        If p leads to a file, the file browser will select/hightlight the file.
	/// @param p Path to a file or folder to open.
	/// @return True on success, false otherwise.
	/// @remark @linux Linux uses xdg-open for this functionality. The given file won't be selected.
	bool OpenInFileBrowser(const std::filesystem::path& p);
	/// @brief Opens a file or folder with the default application.
	/// @param p File or folder to open.
	/// @return True on success, false otherwise.
	/// @remark @linux Linux uses xdg-open for this functionality.
	bool OpenExternally(const std::filesystem::path& p);
}

#endif /*TRAP_FILESYSTEM_H*/
