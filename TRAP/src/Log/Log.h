#ifndef _TRAP_LOG_H_
#define _TRAP_LOG_H_

//#include "TRAPPCH.h" //Doesnt work anymore
#ifdef TRAP_PLATFORM_WINDOWS
#include "Utils/Win.h"
#endif

namespace TRAP
{
	enum class Level
	{
		Trace,
		Debug,
		Info,
		Warn,
		Error,
		Critical
	};

	class Log
	{
	public:
		Log();
		~Log();
		Log(const Log&) = default;
		Log& operator=(const Log&) = default;
		Log(Log&&) = default;
		Log& operator=(Log&&) = default;

		template<typename... Args>
		void Trace(const Args& ... args);

		template<typename... Args>
		void Debug(const Args& ... args);

		template<typename... Args>
		void Info(const Args& ... args);

		template<typename... Args>
		void Warn(const Args& ... args);

		template<typename... Args>
		void Error(const Args& ... args);

		template<typename... Args>
		void Critical(const Args& ... args);

		void Save();

		static Log& Get();

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
	::TRAP::Log::Get().Trace(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
template<typename... Args>
constexpr void TP_DEBUG(const Args& ... args)
{
	::TRAP::Log::Get().Debug(args...);
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
	::TRAP::Log::Get().Info(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TP_WARN(const Args& ... args)
{
	::TRAP::Log::Get().Warn(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TP_ERROR(const Args& ... args)
{
	::TRAP::Log::Get().Error(args...);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
constexpr void TP_CRITICAL(const Args& ... args)
{
	::TRAP::Log::Get().Critical(args...);
}

#endif /*_TRAP_LOG_H_*/