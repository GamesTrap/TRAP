#pragma once

#include "VFS/VFS.h"

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
		Log()
		{
			m_buffer.reserve(256);
			s_Instance = this;
		}
		~Log()
		{
			Save();
		}
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

		static Log& Get() { return *s_Instance; }

#ifdef TRAP_PLATFORM_WINDOWS
	private:
		static void GetInfo();
		static void SetColor(WORD wRGBI);
		static void ResetColor();

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

	template <typename ... Args>
	void Log::Trace(const Args&... args)
	{
		std::ostringstream stream;
		stream << GetTimeStamp() << "[Trace]";
		using List = int[];
		(void)List{0, ((void)(stream << args), 0)...};

		//Standard White
		std::cout << stream.str() << '\n';
		m_buffer.emplace_back(Level::Trace, stream.str());
	}

#ifdef TRAP_PLATFORM_WINDOWS
	template <typename ... Args>
	void Log::Debug(const Args&... args)
	{
		std::ostringstream stream;
		stream << GetTimeStamp() << "[Debug]";
		using List = int[];
		(void)List{0, ((void)(stream << args), 0)...};

		//Standard Cyan
		SetColor(FOREGROUND_GREEN | FOREGROUND_BLUE);
		std::cout << stream.str() << '\n';
		ResetColor();
		m_buffer.emplace_back(Level::Debug, stream.str());
	}

	template <typename ... Args>
	void Log::Info(const Args&... args)
	{
		std::ostringstream stream;
		stream << GetTimeStamp() << "[Info]";
		using List = int[];
		(void)List{0, ((void)(stream << args), 0)...};

		//Standard Green
		SetColor(FOREGROUND_GREEN);
		std::cout << stream.str() << '\n';
		ResetColor();
		m_buffer.emplace_back(Level::Info, stream.str());
	}

	template <typename ... Args>
	void Log::Warn(const Args&... args)
	{
		std::ostringstream stream;
		stream << GetTimeStamp() << "[Warn]";
		using List = int[];
		(void)List{0, ((void)(stream << args), 0)...};

		//Bold Yellow
		SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		std::cout << stream.str() << '\n';
		ResetColor();
		m_buffer.emplace_back(Level::Warn, stream.str());
	}

	template <typename ... Args>
	void Log::Error(const Args&... args)
	{
		std::ostringstream stream;
		stream << GetTimeStamp() << "[Error]";
		using List = int[];
		(void)List{0, ((void)(stream << args), 0)...};

		//Bold Red
		SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cout << stream.str() << '\n';
		ResetColor();
		m_buffer.emplace_back(Level::Error, stream.str());
	}

	template <typename ... Args>
	void Log::Critical(const Args&... args)
	{
		std::ostringstream stream;
		stream << GetTimeStamp() << "[Critical]";
		using List = int[];
		(void)List{0, ((void)(stream << args), 0)...};

		//Bold White on Red
		SetColor(BACKGROUND_RED | FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		std::cout << stream.str() << '\n';
		ResetColor();
		m_buffer.emplace_back(Level::Critical, stream.str());
	}
#else
	template<typename... Args>
	void Log::Debug(const Args& ... args)
	{
		std::ostringstream stream;
		stream << GetTimeStamp() << "[Debug]";
		using List = int[];
		(void)List {
			0, ((void)(stream << args), 0)...
		};

		//Standard Cyan
		std::cout << "\033[36m" << stream.str() << "\033[m" << '\n';
		m_buffer.emplace_back(Level::Debug, stream.str());
	}

	template<typename... Args>
	void Log::Info(const Args & ... args)
	{
		std::ostringstream stream;
		stream << GetTimeStamp() << "[Info]";
		using List = int[];
		(void)List {
			0, ((void)(stream << args), 0)...
		};

		//Standard Green
		std::cout << "\033[32m" << stream.str() << "\033[m" << '\n';
		m_buffer.emplace_back(Level::Info, stream.str());
	}

	template<typename... Args>
	void Log::Warn(const Args & ... args)
	{
		std::ostringstream stream;
		stream << GetTimeStamp() << "[Warn]";
		using List = int[];
		(void)List {
			0, ((void)(stream << args), 0)...
		};

		//Bold Yellow
		std::cout << "\033[33m" << "\033[1m" << stream.str() << "\033[m" << '\n';
		m_buffer.emplace_back(Level::Warn, stream.str());
	}

	template<typename... Args>
	void Log::Error(const Args & ... args)
	{
		std::ostringstream stream;
		stream << GetTimeStamp() << "[Error]";
		using List = int[];
		(void)List {
			0, ((void)(stream << args), 0)...
		};

		//Bold Red
		std::cout << "\033[31m" << "\033[1m" << stream.str() << "\033[m" << '\n';
		m_buffer.emplace_back(Level::Error, stream.str());
	}

	template<typename... Args>
	void Log::Critical(const Args & ... args)
	{
		std::ostringstream stream;
		stream << GetTimeStamp() << "[Critical]";
		using List = int[];
		(void)List {
			0, ((void)(stream << args), 0)...
		};

		//Bold White on Red
		std::cout << "\033[41m" << "\033[1m" << stream.str() << "\033[m" << '\n';
		m_buffer.emplace_back(Level::Critical, stream.str());
	}
#endif
	template<typename... Args>
	constexpr void TP_TRACE(const Args&... args)
	{
		Log::Get().Trace(args...);
	}

	#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
		template<typename... Args>
		constexpr void TP_DEBUG(const Args& ... args)
		{
			Log::Get().Debug(args...);
		}
	#else
		template<typename... Args>
		constexpr void TP_DEBUG(const Args&... args)
		{		
		}
	#endif
		template<typename... Args>
		constexpr void TP_INFO(const Args& ... args)
		{
			Log::Get().Info(args...);
		}
		template<typename... Args>
		constexpr void TP_WARN(const Args& ... args)
		{
			Log::Get().Warn(args...);
		}
		template<typename... Args>
		constexpr void TP_ERROR(const Args& ... args)
		{
			Log::Get().Error(args...);
		}
		template<typename... Args>
		constexpr void TP_CRITICAL(const Args& ... args)
		{
			Log::Get().Critical(args...);
		}
}
