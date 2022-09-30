#include "TRAPPCH.h"
#include "Log.h"

#include "FileSystem/FileSystem.h"

TRAP::Log TRAP::TRAPLog{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log::Log()
	: m_path("trap.log"), m_importance(Level::Trace | Level::Debug | Level::Info | Level::Warn | Level::Error | Level::Critical)
{
	m_buffer.reserve(256);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log::Log(std::filesystem::path filePath)
	: m_path(std::move(filePath)), m_importance(Level::Trace | Level::Debug | Level::Info | Level::Warn | Level::Error | Level::Critical)
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

void TRAP::Log::SetFilePath(const std::filesystem::path& filePath)
{
	m_path = std::move(filePath);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::SetImportance(const Level level)
{
	m_importance = level;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::pair<TRAP::Log::Level, std::string>>& TRAP::Log::GetBuffer() const
{
	return m_buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::Save() const
{
	//Build final path and filename
	const auto folderPath = FileSystem::GetFolderPath(m_path);
	const auto fileName = FileSystem::GetFileName(m_path);
	const auto fileEnding = FileSystem::GetFileEnding(m_path);
	if(!folderPath || !fileName || !fileEnding)
	{
		TP_ERROR(LoggerPrefix, "Failed to save: ", m_path.generic_u8string());
		return;
	}
	const std::filesystem::path logFile = *folderPath / ((*fileName) + "-" + GetDateTimeStamp() + *fileEnding);

	TP_INFO(LoggerPrefix, "Saving ", logFile.generic_u8string());
	std::string output;

	for (const auto& [level, message] : m_buffer)
		output += message + '\n';

	if(!TRAP::FileSystem::WriteTextFile(logFile, output))
		TP_ERROR(LoggerPrefix, "Failed to save: ", logFile.generic_u8string());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::Clear()
{
	m_buffer.clear();
	m_buffer.reserve(256);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Log::GetTimeStamp()
{
	const std::string timeStamp = TRAP::Utils::String::GetTimeStamp(std::chrono::system_clock::now());
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