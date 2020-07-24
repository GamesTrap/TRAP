#include "TRAPPCH.h"
#include "Log.h"

std::mutex TRAP::Log::m_mtx;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log::Log()
{
	m_buffer.reserve(256);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log::~Log()
{	
	Save();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Log& TRAP::Log::Get()
{
	static std::unique_ptr<Log> logger(new Log());
	
	return *logger;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::pair<TRAP::Log::Level, std::string>>& TRAP::Log::GetBuffer()
{
	return Get().m_buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::Save()
{
	TP_PROFILE_FUNCTION();

	TP_INFO(LoggerPrefix, "Saving TRAP.Log");

	std::ofstream file("TRAP.Log");
	if (file.is_open())
	{
		for (const auto& [level, message] : Get().m_buffer)
			file << message << '\n';

		file.close();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::Clear()
{
	TP_PROFILE_FUNCTION();

	Save();
	Get().m_buffer.clear();
	Get().m_buffer.reserve(256);
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