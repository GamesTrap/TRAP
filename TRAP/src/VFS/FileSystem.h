#pragma once

namespace TRAP
{
	class FileSystem
	{
	public:
		static bool FileOrFolderExists(const std::filesystem::path& path);
		static bool SilentFileOrFolderExists(const std::filesystem::path& path);
		static uintmax_t GetFileSize(const std::filesystem::path& path);
		static std::filesystem::file_time_type GetLastWriteTime(const std::filesystem::path& path);

		static std::vector<std::byte> ReadFile(const std::filesystem::path& path);
		static std::string ReadTextFile(const std::filesystem::path& path);
		static std::string SilentReadTextFile(const std::filesystem::path& path);

		static bool WriteFile(const std::filesystem::path& path, std::vector<std::byte>& buffer);
		static bool WriteTextFile(const std::filesystem::path& path, std::string_view text);
	};
}