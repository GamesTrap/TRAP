#ifndef _TRAP_FILESYSTEM_H_
#define _TRAP_FILESYSTEM_H_

namespace TRAP
{
	class FileSystem final
	{
	public:
		/// <summary>
		/// Check if a physical file or folder exists.
		/// <br>
		/// Prints a warning if the physical file or folder doesn't exist.
		/// </summary>
		/// <param name="physicalPath">Physical path to a folder or a file</param>
		/// <returns>
		/// True if physical file or folder exists.<br>
		/// False if physical file or folder doesn't exist.<br>
		/// False if an error has occurred.
		/// </returns>
		static bool PhysicalFileOrFolderExists(const std::filesystem::path& physicalPath);
		/// <summary>
		/// Check if a physical file or folder exists silently.
		/// </summary>
		/// <param name="physicalPath">Physical path to a folder or a file</param>
		/// <returns>
		/// True if physical file or folder exists.
		/// <para>False if physical file or folder doesn't exist.</para>
		/// </returns>
		static bool SilentPhysicalFileOrFolderExists(const std::filesystem::path& physicalPath);
		/// <summary>
		/// Get the size of an physical file or folder in bytes.
		/// </summary>
		/// <param name="physicalPath">Physical path to a file or folder</param>
		/// <returns>
		/// Physical file or folder size in bytes.
		/// <para>0 if an error has occurred.</para>
		/// </returns>
		static uintmax_t GetPhysicalFileOrFolderSize(const std::filesystem::path& physicalPath);
		/// <summary>
		/// Get the last write time of a physical file or folder.
		/// </summary>
		/// <param name="physicalPath">Physical path to a file or folder</param>
		/// <returns>
		/// Last write time of the physical file or folder.
		/// <para>std::filesystem::file_time_type::min() if an error has occurred.</para>
		/// </returns>
		static std::filesystem::file_time_type GetPhysicalLastWriteTime(const std::filesystem::path& physicalPath);

		/// <summary>
		/// Read the given physical file.
		/// <para></para>
		/// <para>Prints an error when file couldn't be opened.</para>
		/// <para>Prints an error when physicalFilePath doesn't exist.</para>
		/// </summary>
		/// <param name="physicalFilePath">Physical file path</param>
		/// <returns>
		/// Vector filled with the file content.
		/// <para>Empty vector if an error has occurred.</para>
		/// </returns>
		static std::vector<uint8_t> ReadPhysicalFile(const std::filesystem::path& physicalFilePath);
		/// <summary>
		/// Read the given physical file silently.
		/// </summary>
		/// <param name="physicalFilePath">Physical file path</param>
		/// <returns>
		/// Vector filled with the file content.
		/// <para>Empty vector if an error has occurred.</para>
		/// </returns>
		static std::vector<uint8_t> SilentReadPhysicalFile(const std::filesystem::path& physicalFilePath);
		/// <summary>
		/// Read the given physical file as text.
		/// <para></para>
		/// <para>Prints an error if physicalFilePath doesn't exist.</para>
		/// <para>Prints an error if physicalFilePath couldn't be opened.</para>
		/// </summary>
		/// <param name="physicalFilePath">Physical file path</param>
		/// <returns>
		/// String filled with the file content.
		/// <para>Empty string if an error has occurred.</para>
		/// </returns>
		static std::string ReadPhysicalTextFile(const std::filesystem::path& physicalFilePath);
		/// <summary>
		/// Read the given physical file as text silently.
		/// </summary>
		/// <param name="physicalFilePath">Physical file path</param>
		/// <returns>
		/// String filled with the file content.
		/// <para>Empty string if an error has occurred.</para>
		/// </returns>
		static std::string SilentReadPhysicalTextFile(const std::filesystem::path& physicalFilePath);

		/// <summary>
		/// Write the given data to the given file path.<br>
		/// Only for non virtual paths
		/// </summary>
		/// <param name="filePath">File path to be written to</param>
		/// <param name="buffer">Data to be written</param>
		/// <returns>True on success or false and an error if it couldn't be written</returns>
		static bool WriteFile(const std::filesystem::path& filePath, std::vector<uint8_t>& buffer);
		/// <summary>
		/// Write the given text to the given file path.
		/// Only for non virtual paths
		/// </summary>
		/// <param name="filePath">File path to be written to</param>
		/// <param name="text">Text to be written</param>
		/// <returns>True on success or false and an error if it couldn't be written</returns>
		static bool WriteTextFile(const std::filesystem::path& filePath, std::string_view text);
	};
}

#endif /*_TRAP_FILESYSTEM_H_*/