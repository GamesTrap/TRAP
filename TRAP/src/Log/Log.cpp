#include "TRAPPCH.h"
#include "Log.h"

TRAP::Log::Log()
{
	TP_PROFILE_FUNCTION();

	m_buffer.reserve(256);
	s_Instance = this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log::~Log()
{
	TP_PROFILE_FUNCTION();
	
	Save();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::pair<TRAP::Log::Level, std::string>>& TRAP::Log::GetBuffer()
{
	return s_Instance->m_buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::Save()
{
	TP_PROFILE_FUNCTION();

	TP_INFO("[Logger] Saving Log.txt");

	std::ofstream file("Log.txt");
	if (file.is_open())
	{
		for (const auto& [level, message] : s_Instance->m_buffer)
			file << message << '\n';

		file.close();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::Clear()
{
	TP_PROFILE_FUNCTION();

	Save();
	s_Instance->m_buffer.clear();
	s_Instance->m_buffer.reserve(256);
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