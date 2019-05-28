#include "TRAPPCH.h"
#include "FileSystem.h"

namespace TRAP
{
	bool FileSystem::FileOrFolderExists(const std::filesystem::path& path)
	{
		if (!exists(path))
		{
			TP_WARN("[FileSystem] File/Folder: ", path, " doesn't exist!");
			return false;
		}

		return true;
	}

	bool FileSystem::SilentFileOrFolderExists(const std::filesystem::path& path)
	{
		return !exists(path);
	}

	uintmax_t FileSystem::GetFileSize(const std::filesystem::path& path)
	{
		return file_size(path);
	}

	std::filesystem::file_time_type FileSystem::GetLastWriteTime(const std::filesystem::path& path)
	{
		return last_write_time(path);
	}

	std::vector<std::byte> FileSystem::ReadFile(const std::filesystem::path& path)
	{
		if (FileOrFolderExists(path))
		{
			std::ifstream file(path, std::ios::binary | std::ios::ate);
			if (file.is_open())
			{
				const unsigned int length = static_cast<const unsigned int>(file.tellg());
				file.seekg(0);
				std::vector<std::byte> buffer{ length };
				file.read(reinterpret_cast<char*>(buffer.data()), length);

				file.close();
				return buffer;
			}

			TP_ERROR("[FileSystem] Could not open File: ", path);
		}

		return std::vector<std::byte>();
	}

	std::string FileSystem::ReadTextFile(const std::filesystem::path& path)
	{
		if (FileOrFolderExists(path))
		{
			std::ifstream file(path);
			if (file.is_open())
			{
				std::stringstream buffer;
				buffer << file.rdbuf();

				file.close();
				return buffer.str();
			}

			TP_ERROR("[FileSystem] Could not open File: ", path);
		}

		return "";
	}

	std::string FileSystem::SilentReadTextFile(const std::filesystem::path& path)
	{
		if (!SilentFileOrFolderExists(path))
		{
			std::ifstream file(path);
			if (file.is_open())
			{
				std::stringstream buffer;
				buffer << file.rdbuf();

				file.close();
				return buffer.str();
			}

			TP_ERROR("[FileSystem] Could not open File: ", path);
		}

		return "";
	}

	bool FileSystem::WriteFile(const std::filesystem::path& path, std::vector<std::byte>& buffer)
	{
		std::ofstream file(path, std::ios::binary);
		if (file.is_open())
		{
			file.write(reinterpret_cast<char*>(buffer.data()), buffer.size());

			file.close();
			return true;
		}

		TP_ERROR("[FileSystem] Could not write File: ", path);

		return false;
	}

	bool FileSystem::WriteTextFile(const std::filesystem::path& path, const std::string_view text)
	{
		std::ofstream file(path);
		if (file.is_open())
		{
			file << text;

			file.close();
			return true;
		}

		TP_ERROR("[FileSystem] Could not write File: ", path);

		return false;
	}
}
