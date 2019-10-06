#include "TRAPPCH.h"
#include "Log.h"

TRAP::Log::Log()
{
	m_buffer.reserve(256);
	s_Instance = this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log::~Log()
{
	Save();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::Save()
{
	TP_INFO("[Logger] Saving Log.txt");

	std::ofstream file("Log.txt");
	if (file.is_open())
	{
		for (const auto& [level, message] : m_buffer)
			file << message << '\n';

		file.close();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log& TRAP::Log::Get()
{
	return *s_Instance;
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