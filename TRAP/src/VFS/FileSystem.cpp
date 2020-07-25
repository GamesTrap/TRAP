#include "TRAPPCH.h"
#include "FileSystem.h"

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::PhysicalFileOrFolderExists(const std::filesystem::path& physicalPath)
{
	try
	{
		if (!exists(physicalPath))
		{
			TP_WARN(Log::FileSystemPrefix, "File/Folder: ", physicalPath, " doesn't exist!");
			return false;
		}

		return true;
	}
	catch(std::exception&)
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::SilentPhysicalFileOrFolderExists(const std::filesystem::path& physicalPath)
{
	try
	{
		return std::filesystem::exists(physicalPath);
	}
	catch(std::exception&)
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uintmax_t TRAP::FileSystem::GetPhysicalFileOrFolderSize(const std::filesystem::path& physicalPath)
{
	try
	{
		return file_size(physicalPath);
	}
	catch(std::exception&)
	{
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::file_time_type TRAP::FileSystem::GetPhysicalLastWriteTime(const std::filesystem::path& physicalPath)
{
	try
	{
		return last_write_time(physicalPath);
	}
	catch(std::exception&)
	{
		return std::filesystem::file_time_type::min();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::FileSystem::ReadPhysicalFile(const std::filesystem::path& physicalFilePath)
{
	if (PhysicalFileOrFolderExists(physicalFilePath))
	{
		std::ifstream file(physicalFilePath, std::ios::binary | std::ios::ate);
		if (file.is_open())
		{
			const uint32_t length = static_cast<uint32_t>(file.tellg());
			file.seekg(0);
			std::vector<uint8_t> buffer(length);
			file.read(reinterpret_cast<char*>(buffer.data()), length);

			file.close();
			return buffer;
		}

		TP_ERROR(Log::FileSystemPrefix, "Could not open File: ", physicalFilePath);
	}

	return std::vector<uint8_t>();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::FileSystem::SilentReadPhysicalFile(const std::filesystem::path& physicalFilePath)
{
	if (SilentPhysicalFileOrFolderExists(physicalFilePath))
	{
		std::ifstream file(physicalFilePath, std::ios::binary | std::ios::ate);
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

std::string TRAP::FileSystem::ReadPhysicalTextFile(const std::filesystem::path& physicalFilePath)
{
	if (PhysicalFileOrFolderExists(physicalFilePath))
	{
		std::ifstream file(physicalFilePath);
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

		TP_ERROR(Log::FileSystemPrefix, "Could not open File: ", physicalFilePath);
	}

	return "";
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::FileSystem::SilentReadPhysicalTextFile(const std::filesystem::path& physicalFilePath)
{
	if (SilentPhysicalFileOrFolderExists(physicalFilePath))
	{
		std::ifstream file(physicalFilePath);
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

	TP_ERROR(Log::FileSystemPrefix, "Could not write File: ", filePath);

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

	TP_ERROR(Log::FileSystemPrefix, "Could not write File: ", filePath);

	return false;
}