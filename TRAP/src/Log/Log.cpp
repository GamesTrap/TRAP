#include "TRAPPCH.h"
#include "Log.h"

#include "FS/FS.h"

TRAP::Log TRAP::TRAPLog("TRAP.Log");

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log::Log(std::filesystem::path filePath)
	: m_path(std::move(filePath))
{
	m_buffer.reserve(256);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log::~Log()
{
	Save();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::filesystem::path& TRAP::Log::GetFilePath() const
{
	return m_path;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::SetFilePath(std::filesystem::path filePath)
{
	m_path = std::move(filePath);
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

	TP_INFO(LoggerPrefix, "Saving ", m_path.generic_u8string());
	std::string output = "";

	for (const auto& [level, message] : m_buffer)
		output += message + '\n';

	if(!TRAP::FS::WriteTextFile(m_path, output))
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