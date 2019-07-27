#include "TRAPPCH.h"
#include "Log.h"

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Log::Save()
{
	TP_INFO("[Logger] Saving Log.txt");

	std::ofstream file("Log.txt");
	if (file.is_open())
	{
		for (const auto& item : m_buffer)
			file << item.second << '\n';

		file.close();
	}
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