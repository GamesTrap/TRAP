#ifndef _TRAP_LOG_H_
#define _TRAP_LOG_H_

#ifdef TRAP_PLATFORM_WINDOWS
	#include "Utils/Win.h"
#endif

#include "Utils/Singleton.h"

namespace TRAP
{
	class Log final : public Singleton
	{
	public:
		enum class Level
		{
			Trace,
			Debug,
			Info,
			Warn,
			Error,
			Critical
		};
		
		Log();
		~Log();
		Log(const Log&) = delete;
		Log& operator=(const Log&) = delete;
		Log(Log&&) = delete;
		Log& operator=(Log&&) = delete;

		template<typename... Args>
		static void Trace(Args&& ... args);

		template<typename... Args>
		static void Debug(Args&& ... args);

		template<typename... Args>
		static void Info(Args&& ... args);

		template<typename... Args>
		static void Warn(Args&& ... args);

		template<typename... Args>
		static void Error(Args&& ... args);

		template<typename... Args>
		static void Critical(Args&& ... args);

		static const std::vector<std::pair<Level, std::string>>& GetBuffer();

		static void Save();
		static void Clear();

#ifdef TRAP_PLATFORM_WINDOWS
	private:
		static void GetInfo() { GetConsoleScreenBufferInfo(m_handleConsole, &m_csbi); }
		static void SetColor(WORD wRGBI);
		static void ResetColor() { SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); }

		static HANDLE m_handleConsole;
		static CONSOLE_SCREEN_BUFFER_INFO m_csbi;
#else
	private:
		static bool IsColorTerminal() noexcept;
#endif
		static std::string GetTimeStamp();

		std::vector<std::pair<Level, std::string>> m_buffer{};

		inline static Log* s_Instance = nullptr;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

#include "Log.inl"

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TP_TRACE(const Args& ... args)
{
	::TRAP::Log::Trace(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
template<typename... Args>
constexpr void TP_DEBUG(const Args& ... args)
{
	::TRAP::Log::Debug(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

#else
template<typename... Args>
constexpr void TP_DEBUG(const Args& ... args)
{
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TP_INFO(const Args& ... args)
{
	::TRAP::Log::Info(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TP_WARN(const Args& ... args)
{
	::TRAP::Log::Warn(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TP_ERROR(const Args& ... args)
{
	::TRAP::Log::Error(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TP_CRITICAL(const Args& ... args)
{
	::TRAP::Log::Critical(args...);
}

#endif /*_TRAP_LOG_H_*/