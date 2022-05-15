#include "TRAPPCH.h"
#include "Log.h"

#include "FS/FS.h"

TRAP::Log TRAP::TRAPLog{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log::Log()
	: m_path("trap.log")
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

	const std::filesystem::path logFile = FS::GetFolderPath(m_path) / (FS::GetFileName(m_path) + "-" +
		                                                               GetDateTimeStamp() +
		                                                               FS::GetFileEnding(m_path));

	TP_INFO(LoggerPrefix, "Saving ", m_path.generic_u8string());
	std::string output;

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
	std::string timeStamp = TRAP::Utils::String::GetTimeStamp(std::chrono::system_clock::now());
	return "[" + timeStamp + ']';
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Log::GetDateTimeStamp()
{
	std::string dateTimeStamp = TRAP::Utils::String::GetDateTimeStamp(std::chrono::system_clock::now());
	//Make the date-time-stamp usable as file-name
	std::replace(dateTimeStamp.begin(), dateTimeStamp.end(), ':', '-');
	return dateTimeStamp;
}