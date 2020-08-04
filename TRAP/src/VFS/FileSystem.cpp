#include "TRAPPCH.h"
#include "FileSystem.h"

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::PhysicalFileOrFolderExists(const std::filesystem::path& physicalPath)
{
	try
	{
		if (!std::filesystem::exists(physicalPath))
		{
			TP_WARN(Log::FileSystemPrefix, "File/Folder: \"", physicalPath, "\" doesn't exist!");
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
	if (PhysicalFileOrFolderExists(physicalPath))
	{
		try
		{
			return std::filesystem::file_size(physicalPath);
		}
		catch (std::exception&)
		{
			return 0;
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

std::filesystem::file_time_type TRAP::FileSystem::GetPhysicalLastWriteTime(const std::filesystem::path& physicalPath)
{
	if (PhysicalFileOrFolderExists(physicalPath))
	{
		try
		{
			return std::filesystem::last_write_time(physicalPath);
		}
		catch (std::exception&)
		{
			return std::filesystem::file_time_type::min();
		}
	}
	
	return std::filesystem::file_time_type::min();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::FileSystem::ReadPhysicalFile(const std::filesystem::path& physicalFilePath)
{
	if (PhysicalFileOrFolderExists(physicalFilePath))
	{
		std::ifstream file(physicalFilePath, std::ios::binary | std::ios::ate);
		if (file.is_open())
		{
			const uintmax_t length = file.tellg();
			file.seekg(0);
			std::vector<uint8_t> buffer(length);
			file.read(reinterpret_cast<char*>(buffer.data()), length);

			file.close();
			return buffer;
		}

		TP_ERROR(Log::FileSystemPrefix, "Could not open File: \"", physicalFilePath, "\"");
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
			const uintmax_t length = file.tellg();
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

			while (file.good() && std::getline(file, line))
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

		TP_ERROR(Log::FileSystemPrefix, "Could not open File: \"", physicalFilePath, "\"");
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

			while (file.good() && std::getline(file, line))
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

bool TRAP::FileSystem::WritePhysicalFile(const std::filesystem::path& physicalFilePath, std::vector<uint8_t>& buffer)
{
	if (!physicalFilePath.empty() && !buffer.empty())
	{
		std::ofstream file(physicalFilePath, std::ios::binary);
		if (file.is_open() && file.good())
		{
			file.write(reinterpret_cast<char*>(buffer.data()), buffer.size());

			file.close();
			return true;
		}

		TP_ERROR(Log::FileSystemPrefix, "Could not write File: \"", physicalFilePath, "\"");
	}
	else
		TP_ERROR(Log::FileSystemPrefix, "Could not write File because physical file path and/or data was empty!");

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::FileSystem::WritePhysicalTextFile(const std::filesystem::path& physicalFilePath, const std::string_view text)
{
	if (!physicalFilePath.empty() && !text.empty())
	{
		std::ofstream file(physicalFilePath);
		if (file.is_open() && file.good())
		{
			file << text;

			file.close();
			return true;
		}

		TP_ERROR(Log::FileSystemPrefix, "Could not write File: ", physicalFilePath);
	}
	else
		TP_ERROR(Log::FileSystemPrefix, "Could not write File because physical file path and/or data was empty!");

	return false;
}