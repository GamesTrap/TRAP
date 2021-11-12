#include "TRAPPCH.h"
#include "Log.h"

#include "FS/FS.h"

TRAP::Log TRAP::TRAPLog("trap.log");

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log::Log(std::filesystem::path fileName)
	: m_path(std::move(fileName))
{
	m_buffer.reserve(256);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log::~Log()
{
	Save();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::filesystem::path& TRAP::Log::GetFileName() const
{
	return m_path;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::SetFileName(std::filesystem::path fileName)
{
	m_path = std::move(fileName);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::pair<TRAP::Log::Level, std::string>>& TRAP::Log::GetBuffer() const
{
	return m_buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::Save()
{
	TP_PROFILE_FUNCTION();

	std::filesystem::path logFile = FS::GetGameDocumentsFolderPath() / "Logs" / (FS::GetFileName(m_path) + "-" +
	                                GetDateTimeStamp() + FS::GetFileEnding(m_path));

	TP_INFO(LoggerPrefix, "Saving ", m_path.generic_u8string());
	std::string output = "";

	for (const auto& [level, message] : m_buffer)
		output += message + '\n';

	if(!TRAP::FS::WriteTextFile(logFile, output))
		TP_ERROR(LoggerPrefix, "Failed to save: ", m_path.generic_u8string());
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

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Log::GetDateTimeStamp()
{
	std::stringstream ss;
	std::time_t time = std::time(nullptr);
	std::tm tm{};
#ifdef TRAP_PLATFORM_WINDOWS
	localtime_s(&tm, &time);
#else
	localtime_r(&time, &tm);
#endif
	ss << std::put_time(&tm, "%FT%T");

	return ss.str();
}