#include "TRAPPCH.h"
#include "FileSystem.h"

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::FileOrFolderExists(const std::filesystem::path& path)
{
	if (!exists(path))
	{
		TP_WARN("[FileSystem] File/Folder: ", path, " doesn't exist!");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::SilentFileOrFolderExists(const std::filesystem::path& path)
{
	return !exists(path);
}

//-------------------------------------------------------------------------------------------------------------------//

uintmax_t TRAP::FileSystem::GetFileOrFolderSize(const std::filesystem::path& path)
{
	return file_size(path);
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::file_time_type TRAP::FileSystem::GetLastWriteTime(const std::filesystem::path& path)
{
	return last_write_time(path);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::byte> TRAP::FileSystem::ReadFile(const std::filesystem::path& filePath)
{
	if (FileOrFolderExists(filePath))
	{
		std::ifstream file(filePath, std::ios::binary | std::ios::ate);
		if (file.is_open())
		{
			const unsigned int length = static_cast<const unsigned int>(file.tellg());
			file.seekg(0);
			std::vector<std::byte> buffer{ length };
			file.read(reinterpret_cast<char*>(buffer.data()), length);

			file.close();
			return buffer;
		}

		TP_ERROR("[FileSystem] Could not open File: ", filePath);
	}

	return std::vector<std::byte>();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::FileSystem::ReadTextFile(const std::filesystem::path& filePath)
{
	if (FileOrFolderExists(filePath))
	{
		std::ifstream file(filePath);
		if (file.is_open())
		{
			std::stringstream buffer;
			buffer << file.rdbuf();

			file.close();
			return buffer.str();
		}

		TP_ERROR("[FileSystem] Could not open File: ", filePath);
	}

	return "";
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::FileSystem::SilentReadTextFile(const std::filesystem::path& filePath)
{
	if (!SilentFileOrFolderExists(filePath))
	{
		std::ifstream file(filePath);
		if (file.is_open())
		{
			std::stringstream buffer;
			buffer << file.rdbuf();

			file.close();
			return buffer.str();
		}

		TP_ERROR("[FileSystem] Could not open File: ", filePath);
	}

	return "";
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::WriteFile(const std::filesystem::path& filePath, std::vector<std::byte>& buffer)
{
	std::ofstream file(filePath, std::ios::binary);
	if (file.is_open())
	{
		file.write(reinterpret_cast<char*>(buffer.data()), buffer.size());

		file.close();
		return true;
	}

	TP_ERROR("[FileSystem] Could not write File: ", filePath);

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::WriteTextFile(const std::filesystem::path& filePath, const std::string_view text)
{
	std::ofstream file(filePath);
	if (file.is_open())
	{
		file << text;

		file.close();
		return true;
	}

	TP_ERROR("[FileSystem] Could not write File: ", filePath);

	return false;
}