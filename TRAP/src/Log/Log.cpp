#include "TRAPPCH.h"
#include "Log.h"

namespace TRAP
{
	void Log::Save()
	{
		TP_INFO("[Logger] Saving Log.txt");

		//Save to Log.txt Can't be handled by VFS see BUG#4
		std::ofstream file("Log.txt");
		if(file.is_open())
		{
			for(const auto& item : m_buffer)
				file << item.second << '\n';

			file.close();
		}
	}

	std::string Log::GetTimeStamp()
	{
		std::stringstream ss;
		ss << '[';
		std::time_t time = std::time(nullptr);
		std::tm tm{};
#ifdef TRAP_PLATFORM_WINDOWS
		localtime_s(&tm, &time);
#else
		localtime_r(&tm, &time);
#endif
		ss << std::put_time(&tm, "%T") << ']';

		return ss.str();
	}
}
