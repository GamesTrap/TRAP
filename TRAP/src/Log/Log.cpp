#include "TRAPPCH.h"
#include "Log.h"

#include "FileSystem/FileSystem.h"
#include "Utils/String/String.h"

namespace
{
#ifndef TRAP_UNITTESTS
	/// @brief Get a date time stamp with YYYY-MM-DDTHH-MM-SS format.
	/// @return Time stamp as a string.
	/// @threadsafe
	[[nodiscard]] std::string GetDateTimeStamp()
	{
		ZoneScoped;

		std::string dateTimeStamp = TRAP::Utils::String::GetDateTimeStamp(std::chrono::system_clock::now());
		//Make the date-time-stamp usable as file-name
		std::ranges::transform(dateTimeStamp, dateTimeStamp.begin(), [](const char c)
		{
			if(c == ':')
				return '-';
			if(c == ' ')
				return '_';
			return c;
		});
		return dateTimeStamp;
	}
#endif /*TRAP_UNITTESTS*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Log& TRAP::GetTRAPLog()
{
	static TRAP::Log TRAPLog("trap.log");
	return TRAPLog;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log::Log(std::filesystem::path filePath)
	: m_path{Utils::DefaultConstructMutexTag, std::move(filePath)}
{
	ZoneScoped;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log::~Log()
{
	ZoneScoped;

	Save();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::filesystem::path TRAP::Log::GetFilePath() const noexcept
{
	ZoneScoped;

	return *m_path.ReadLock();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::SetFilePath(std::filesystem::path filePath) noexcept
{
	ZoneScoped;

	if(!FileSystem::GetFileNameWithEnding(filePath))
	{
		TP_ERROR(LoggerPrefix, "Failed to set new log file path, expected a file name (\"", filePath, "\")");
		return;
	}

	*m_path.WriteLock() = std::move(filePath);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::SetImportance(const LogLevel level) noexcept
{
	ZoneScoped;

	m_importance = level;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::LogLevel TRAP::Log::GetImportance() const noexcept
{
	ZoneScoped;

	return m_importance;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<TRAP::LogEntry> TRAP::Log::GetBufferAndFlush()
{
	ZoneScoped;

	std::vector<TRAP::LogEntry> outputData{};

	while(auto entry = m_buffer.TryPop())
		outputData.push_back(*entry);

	std::ranges::sort(outputData, std::less{});

	return outputData;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::Save()
{
	ZoneScoped;

#ifndef TRAP_UNITTESTS
	const auto path = m_path.ReadLock();

	//Build final path and filename
	const auto folderPath = FileSystem::GetFolderPath(*path).ValueOr("");
	const auto fileName = FileSystem::GetFileNameWithoutEnding(*path);
	const auto fileEnding = FileSystem::GetFileEnding(*path).ValueOr("");
	if(!fileName)
	{
		TP_ERROR(LoggerPrefix, "Failed to save: ", *path);
		return;
	}

	const std::filesystem::path logFile = folderPath / ((*fileName) + "-" + GetDateTimeStamp() + fileEnding);

	TP_INFO(LoggerPrefix, "Saving ", logFile);
	std::string output;

	for (const auto& [level, message, time] : GetBufferAndFlush())
		output += message + '\n';

	if(!TRAP::FileSystem::WriteTextFile(logFile, output))
		TP_ERROR(LoggerPrefix, "Failed to save: ", logFile);
#endif /*TRAP_UNITTESTS*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::Clear() noexcept
{
	ZoneScoped;

	while(const auto entry = m_buffer.TryPop());
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Log::GetTimeStamp(const std::chrono::time_point<std::chrono::system_clock>& time)
{
	ZoneScoped;

	const std::string timeStamp = TRAP::Utils::String::GetTimeStamp(time);
	return "[" + timeStamp + ']';
}
