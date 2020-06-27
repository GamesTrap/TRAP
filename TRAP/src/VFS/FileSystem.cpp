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

std::vector<uint8_t> TRAP::FileSystem::ReadFile(const std::filesystem::path& filePath)
{
	if (FileOrFolderExists(filePath))
	{
		std::ifstream file(filePath, std::ios::binary | std::ios::ate);
		if (file.is_open())
		{
			const uint32_t length = static_cast<uint32_t>(file.tellg());
			file.seekg(0);
			std::vector<uint8_t> buffer(length);
			file.read(reinterpret_cast<char*>(buffer.data()), length);

			file.close();
			return buffer;
		}

		TP_ERROR("[FileSystem] Could not open File: ", filePath);
	}

	return std::vector<uint8_t>();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::FileSystem::SilentReadFile(const std::filesystem::path& filePath)
{
	if (!SilentFileOrFolderExists(filePath))
	{
		std::ifstream file(filePath, std::ios::binary | std::ios::ate);
		if (file.is_open())
		{
			const uint32_t length = static_cast<uint32_t>(file.tellg());
			file.seekg(0);
			std::vector<uint8_t> buffer(length);
			file.read(reinterpret_cast<char*>(buffer.data()), length);

			file.close();
			return buffer;
		}
	}

	return std::vector<uint8_t>();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::FileSystem::ReadTextFile(const std::filesystem::path& filePath)
{
	if (FileOrFolderExists(filePath))
	{
		std::ifstream file(filePath);
		if (file.is_open())
		{
			std::string line;
			std::string result;

			while (std::getline(file, line))
			{
				if(!line.empty())
					if (line.back() == '\r')
						line.pop_back();
				
				result += line;
				result += '\n';
			}
			
			file.close();
			return result;
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
			std::string line;
			std::string result;

			while (std::getline(file, line))
			{
				if(!line.empty())
					if (line.back() == '\r')
						line.pop_back();
				
				result += line;
				result += '\n';
			}

			file.close();
			return result;
		}
	}

	return "";
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::WriteFile(const std::filesystem::path& filePath, std::vector<uint8_t>& buffer)
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