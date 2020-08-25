#ifndef _TRAP_FILESYSTEM_H_
#define _TRAP_FILESYSTEM_H_

namespace TRAP
{
	class FileSystem final
	{
	public:
		enum class WriteMode
		{
			Overwrite,
			Append
		};
		
		/// <summary>
		/// Check if a physical file or folder exists.<br>
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
		/// True if physical file or folder exists.<br>
		/// False if physical file or folder doesn't exist.
		/// </returns>
		static bool SilentPhysicalFileOrFolderExists(const std::filesystem::path& physicalPath);
		/// <summary>
		/// Get the size of an physical file or folder in bytes.
		/// </summary>
		/// <param name="physicalPath">Physical path to a file or folder</param>
		/// <returns>
		/// Physical file or folder size in bytes.<br>
		/// 0 if an error has occurred.
		/// </returns>
		static uintmax_t GetPhysicalFileOrFolderSize(const std::filesystem::path& physicalPath);
		/// <summary>
		/// Get the last write time of a physical file or folder.
		/// </summary>
		/// <param name="physicalPath">Physical path to a file or folder</param>
		/// <returns>
		/// Last write time of the physical file or folder.<br>
		/// std::filesystem::file_time_type::min() if an error has occurred.
		/// </returns>
		static std::filesystem::file_time_type GetPhysicalLastWriteTime(const std::filesystem::path& physicalPath);

		/// <summary>
		/// Read the given physical file.<br>
		/// <br>
		/// Prints an error when file couldn't be opened.<br>
		/// Prints an error when physicalFilePath doesn't exist.
		/// </summary>
		/// <param name="physicalFilePath">Physical file path</param>
		/// <returns>
		/// Vector filled with the file content.<br>
		/// Empty vector if an error has occurred.
		/// </returns>
		static std::vector<uint8_t> ReadPhysicalFile(const std::filesystem::path& physicalFilePath);
		/// <summary>
		/// Read the given physical file silently.
		/// </summary>
		/// <param name="physicalFilePath">Physical file path</param>
		/// <returns>
		/// Vector filled with the file content.<br>
		/// Empty vector if an error has occurred.
		/// </returns>
		static std::vector<uint8_t> SilentReadPhysicalFile(const std::filesystem::path& physicalFilePath);
		/// <summary>
		/// Read the given physical file as text.<br>
		/// <br>
		/// Prints an error if physicalFilePath doesn't exist.<br>
		/// Prints an error if physicalFilePath couldn't be opened.
		/// </summary>
		/// <param name="physicalFilePath">Physical file path</param>
		/// <returns>
		/// String filled with the file content.<br>
		/// Empty string if an error has occurred.
		/// </returns>
		static std::string ReadPhysicalTextFile(const std::filesystem::path& physicalFilePath);
		/// <summary>
		/// Read the given physical file as text silently.
		/// </summary>
		/// <param name="physicalFilePath">Physical file path</param>
		/// <returns>
		/// String filled with the file content.<br>
		/// Empty string if an error has occurred.
		/// </returns>
		static std::string SilentReadPhysicalTextFile(const std::filesystem::path& physicalFilePath);

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
	};
}

#endif /*_TRAP_FILESYSTEM_H_*/