#ifndef TRAP_LOG_INL
#define TRAP_LOG_INL

#include "Maths/Types.h"

#ifdef TRAP_PLATFORM_WINDOWS

template <typename ... Args>
void TRAP::Log::Trace(Args&& ... args)
{
	ZoneScoped;

	std::ostringstream stream;
	stream << GetTimeStamp() << "[Trace]";
	(void)std::initializer_list<int32_t>{((void)(stream << args), 0)...}; //In place variadic unfolding "hack"

	{
		std::lock_guard<std::mutex> lock(m_mtx);
		if (static_cast<uint32_t>(m_importance) & static_cast<uint32_t>(Level::Trace))
		{
#if !defined(TRAP_RELEASE)
			SetColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE);
			//Standard Magenta
			std::cout << stream.str() << '\n';
			ResetColor();
#endif
		}

#ifdef TRACY_ENABLE
		TracyMessageC(stream.str().c_str(), stream.str().size(), tracy::Color::Magenta);
#endif

		m_buffer.emplace_back(Level::Trace, stream.str());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename ... Args>
void TRAP::Log::Debug(Args&& ... args)
{
	ZoneScoped;

	std::ostringstream stream;
	stream << GetTimeStamp() << "[Debug]";
	(void)std::initializer_list<int32_t>{((void)(stream << args), 0)...}; //In place variadic unfolding "hack"

	{
		std::lock_guard<std::mutex> lock(m_mtx);
		if (static_cast<uint32_t>(m_importance) & static_cast<uint32_t>(Level::Debug))
		{
#if !defined(TRAP_RELEASE)
			//Standard Cyan
			SetColor(FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << stream.str() << '\n';
			ResetColor();
#endif
		}

#ifdef TRACY_ENABLE
		TracyMessageC(stream.str().c_str(), stream.str().size(), tracy::Color::Cyan);
#endif

		m_buffer.emplace_back(Level::Debug, stream.str());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename ... Args>
void TRAP::Log::Info(Args&& ... args)
{
	ZoneScoped;

	std::ostringstream stream;
	stream << GetTimeStamp() << "[Info]";
	(void)std::initializer_list<int32_t>{((void)(stream << args), 0)...}; //In place variadic unfolding "hack"

	{
		std::lock_guard<std::mutex> lock(m_mtx);
		if (static_cast<uint32_t>(m_importance) & static_cast<uint32_t>(Level::Info))
		{
#if !defined(TRAP_RELEASE)
			//Standard Green
			SetColor(FOREGROUND_GREEN);
			std::cout << stream.str() << '\n';
			ResetColor();
#endif
		}

#ifdef TRACY_ENABLE
		TracyMessageC(stream.str().c_str(), stream.str().size(), tracy::Color::Green);
#endif

		m_buffer.emplace_back(Level::Info, stream.str());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename ... Args>
void TRAP::Log::Warn(Args&& ... args)
{
	ZoneScoped;

	std::ostringstream stream;
	stream << GetTimeStamp() << "[Warn]";
	(void)std::initializer_list<int32_t>{((void)(stream << args), 0)...}; //In place variadic unfolding "hack"

	{
		std::lock_guard<std::mutex> lock(m_mtx);
		if (static_cast<uint32_t>(m_importance) & static_cast<uint32_t>(Level::Warn))
		{
#if !defined(TRAP_RELEASE)
			//Bold Yellow
			SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			std::cout << stream.str() << '\n';
			ResetColor();
#endif
		}

#ifdef TRACY_ENABLE
		TracyMessageC(stream.str().c_str(), stream.str().size(), tracy::Color::Yellow);
#endif

		m_buffer.emplace_back(Level::Warn, stream.str());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename ... Args>
void TRAP::Log::Error(Args&& ... args)
{
	ZoneScoped;

	std::ostringstream stream;
	stream << GetTimeStamp() << "[Error]";
	(void)std::initializer_list<int32_t>{((void)(stream << args), 0)...}; //In place variadic unfolding "hack"

	{
		std::lock_guard<std::mutex> lock(m_mtx);
		if (static_cast<uint32_t>(m_importance) & static_cast<uint32_t>(Level::Error))
		{
#if !defined(TRAP_RELEASE)
			//Bold Red
			SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
			std::cout << stream.str() << '\n';
			ResetColor();
#endif
		}

#ifdef TRACY_ENABLE
		TracyMessageC(stream.str().c_str(), stream.str().size(), tracy::Color::Red);
#endif

		m_buffer.emplace_back(Level::Error, stream.str());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename ... Args>
void TRAP::Log::Critical(Args&& ... args)
{
	ZoneScoped;

	std::ostringstream stream;
	stream << GetTimeStamp() << "[Critical]";
	(void)std::initializer_list<int32_t>{((void)(stream << args), 0)...}; //In place variadic unfolding "hack"

	{
		std::lock_guard<std::mutex> lock(m_mtx);
		if (static_cast<uint32_t>(m_importance) & static_cast<uint32_t>(Level::Critical))
		{
#if !defined(TRAP_RELEASE)
			//Bold White on Red
			SetColor(BACKGROUND_RED | FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << stream.str() << '\n';
			ResetColor();
#endif
		}

#ifdef TRACY_ENABLE
		TracyMessageC(stream.str().c_str(), stream.str().size(), tracy::Color::DarkRed);
#endif

		m_buffer.emplace_back(Level::Critical, stream.str());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#else

template <typename ... Args>
void TRAP::Log::Trace(Args&& ... args)
{
	ZoneScoped;

	std::ostringstream stream;
	stream << GetTimeStamp() << "[Trace]";
	(void)std::initializer_list<int32_t>{((void)(stream << args), 0)...}; //In place variadic unfolding "hack"

	{
		std::lock_guard<std::mutex> lock(m_mtx);
		if (static_cast<uint32_t>(m_importance) & static_cast<uint32_t>(Level::Trace))
		{
#if !defined(TRAP_RELEASE)
			//Standard Magenta
			if(IsColorTerminal())
				std::cout << "\033[35m" << stream.str() << "\033[m" << '\n';
			else
				std::cout << stream.str() << '\n';
#endif
		}

#ifdef TRACY_ENABLE
		TracyMessageC(stream.str().c_str(), stream.str().size(), tracy::Color::Magenta);
#endif

		m_buffer.emplace_back(Level::Trace, stream.str());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
void TRAP::Log::Debug(Args&& ... args)
{
	ZoneScoped;

	std::ostringstream stream;
	stream << GetTimeStamp() << "[Debug]";
	(void)std::initializer_list<int32_t>{((void)(stream << args), 0)...}; //In place variadic unfolding "hack"

	{
		std::lock_guard<std::mutex> lock(m_mtx);
		if (static_cast<uint32_t>(m_importance) & static_cast<uint32_t>(Level::Debug))
		{
#if !defined(TRAP_RELEASE)
			//Standard Cyan
			if(IsColorTerminal())
				std::cout << "\033[36m" << stream.str() << "\033[m" << '\n';
			else
				std::cout << stream.str() << '\n';
#endif
		}

#ifdef TRACY_ENABLE
		TracyMessageC(stream.str().c_str(), stream.str().size(), tracy::Color::Cyan);
#endif

		m_buffer.emplace_back(Level::Debug, stream.str());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
void TRAP::Log::Info(Args&& ... args)
{
	ZoneScoped;

	std::ostringstream stream;
	stream << GetTimeStamp() << "[Info]";
	(void)std::initializer_list<int32_t>{((void)(stream << args), 0)...}; //In place variadic unfolding "hack"

	{
		std::lock_guard<std::mutex> lock(m_mtx);
		if (static_cast<uint32_t>(m_importance) & static_cast<uint32_t>(Level::Info))
		{
#if !defined(TRAP_RELEASE)
			//Standard Green
			if(IsColorTerminal())
				std::cout << "\033[32m" << stream.str() << "\033[m" << '\n';
			else
				std::cout << stream.str() << '\n';
#endif
		}

#ifdef TRACY_ENABLE
		TracyMessageC(stream.str().c_str(), stream.str().size(), tracy::Color::Green);
#endif

		m_buffer.emplace_back(Level::Info, stream.str());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
void TRAP::Log::Warn(Args&& ... args)
{
	ZoneScoped;

	std::ostringstream stream;
	stream << GetTimeStamp() << "[Warn]";
	(void)std::initializer_list<int32_t>{((void)(stream << args), 0)...}; //In place variadic unfolding "hack"

	{
		std::lock_guard<std::mutex> lock(m_mtx);
		if (static_cast<uint32_t>(m_importance) & static_cast<uint32_t>(Level::Warn))
		{
#if !defined(TRAP_RELEASE)
			//Bold Yellow
			if(IsColorTerminal())
				std::cout << "\033[33m" << "\033[1m" << stream.str() << "\033[m" << '\n';
			else
				std::cout << stream.str() << '\n';
#endif
		}

#ifdef TRACY_ENABLE
		TracyMessageC(stream.str().c_str(), stream.str().size(), tracy::Color::Yellow);
#endif

		m_buffer.emplace_back(Level::Warn, stream.str());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
void TRAP::Log::Error(Args&& ... args)
{
	ZoneScoped;

	std::ostringstream stream;
	stream << GetTimeStamp() << "[Error]";
	(void)std::initializer_list<int32_t>{((void)(stream << args), 0)...}; //In place variadic unfolding "hack"

	{
		std::lock_guard<std::mutex> lock(m_mtx);
		if (static_cast<uint32_t>(m_importance) & static_cast<uint32_t>(Level::Error))
		{
#if !defined(TRAP_RELEASE)
			//Bold Red
			if(IsColorTerminal())
				std::cout << "\033[31m" << "\033[1m" << stream.str() << "\033[m" << '\n';
			else
				std::cout << stream.str() << '\n';
#endif
		}

#ifdef TRACY_ENABLE
		TracyMessageC(stream.str().c_str(), stream.str().size(), tracy::Color::Red);
#endif

		m_buffer.emplace_back(Level::Error, stream.str());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
void TRAP::Log::Critical(Args&& ... args)
{
	ZoneScoped;

	std::ostringstream stream;
	stream << GetTimeStamp() << "[Critical]";
	(void)std::initializer_list<int32_t>{((void)(stream << args), 0)...}; //In place variadic unfolding "hack"

	{
		std::lock_guard<std::mutex> lock(m_mtx);
		if (static_cast<uint32_t>(m_importance) & static_cast<uint32_t>(Level::Critical))
		{
#if !defined(TRAP_RELEASE)
			//Bold White on Red
			if(IsColorTerminal())
				std::cout << "\033[41m" << "\033[1m" << stream.str() << "\033[m" << '\n';
			else
				std::cout << stream.str() << '\n';
#endif
		}

#ifdef TRACY_ENABLE
		TracyMessageC(stream.str().c_str(), stream.str().size(), tracy::Color::DarkRed);
#endif

		m_buffer.emplace_back(Level::Critical, stream.str());
	}
}
#endif

//-------------------------------------------------------------------------------------------------------------------//
//Math ToString for streams

//Vectors
template<typename OStream, uint32_t L, typename T>
inline OStream& operator<<(OStream& os, const TRAP::Math::Vec<L, T>& vec)
{
	ZoneScoped;

	return os << vec.ToString();
}

//Matrices
template<typename OStream, uint32_t L, typename T>
inline OStream& operator<<(OStream& os, const TRAP::Math::Mat<L, L, T>& mat)
{
	ZoneScoped;

	return os << mat.ToString();
}

//Quaternion
template<typename OStream, typename T>
inline OStream& operator<<(OStream& os, const TRAP::Math::tQuat<T>& quat)
{
	ZoneScoped;

	return os << quat.ToString();
}

#endif /*TRAP_LOG_INL*/