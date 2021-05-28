#include "TRAPPCH.h"
#include "Log.h"

#include "VFS/VFS.h"

TRAP::Log TRAP::TRAPLog("TRAP.Log");

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log::Log(std::string virtualOrPhysicalFilePath)
	: m_path(std::move(virtualOrPhysicalFilePath))
{
	m_buffer.reserve(256);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log::~Log()
{
	Save();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Log::GetFilePath() const
{
	return m_path;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::SetFilePath(const std::string& virtualOrPhysicalFilePath)
{
	m_path = virtualOrPhysicalFilePath;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::pair<TRAP::Log::Level, std::string>>& TRAP::Log::GetBuffer()
{
	return m_buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::Save()
{
	TP_PROFILE_FUNCTION();

	TP_INFO(LoggerPrefix, "Saving ", m_path);
	std::string output = "";

	for (const auto& [level, message] : m_buffer)
		output += message + '\n';

	if(m_path == "TRAP.Log")
	{
		std::ofstream file(m_path);
		if(file.is_open())
		{
			file << output;
			file.close();
		}
	}
	else if(!TRAP::VFS::WriteTextFile(m_path, output))
		TP_ERROR(LoggerPrefix, "Failed to save: ", m_path);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::Clear()
{
	TP_PROFILE_FUNCTION();

	m_buffer.clear();
	m_buffer.reserve(256);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Log::GetTimeStamp()
{
	std::stringstream ss;
	ss << '[';
	std::time_t time = std::time(nullptr);
	std::tm tm{};
#ifdef TRAP_PLATFORM_WINDOWS
	localtime_s(&tm, &time);
#else
	localtime_r(&time, &tm);
#endif
	ss << std::put_time(&tm, "%T") << ']';

	return ss.str();
}