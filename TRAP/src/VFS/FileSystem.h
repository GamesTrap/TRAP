#ifndef _TRAP_FILESYSTEM_H_
#define _TRAP_FILESYSTEM_H_

namespace TRAP
{
	class FileSystem final
	{
	public:
		static bool FileOrFolderExists(const std::filesystem::path& path);
		static bool SilentFileOrFolderExists(const std::filesystem::path& path);
		static uintmax_t GetFileOrFolderSize(const std::filesystem::path& path);
		static std::filesystem::file_time_type GetLastWriteTime(const std::filesystem::path& path);

		static std::vector<std::byte> ReadFile(const std::filesystem::path& filePath);
		static std::vector<std::byte> SilentReadFile(const std::filesystem::path& filePath);
		static std::string ReadTextFile(const std::filesystem::path& filePath);
		static std::string SilentReadTextFile(const std::filesystem::path& filePath);

		static bool WriteFile(const std::filesystem::path& filePath, std::vector<std::byte>& buffer);
		static bool WriteTextFile(const std::filesystem::path& filePath, std::string_view text);
	};
}

#endif /*_TRAP_FILESYSTEM_H_*/