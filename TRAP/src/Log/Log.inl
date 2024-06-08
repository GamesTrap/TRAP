#ifndef TRAP_LOG_INL
#define TRAP_LOG_INL

#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/std.h>

template <typename ... Args>
void TRAP::Log::Trace(Args&& ... args)
{
	ZoneScoped;

	const std::string result = GetTimeStamp() + "[Trace]" + (fmt::format("{}", args) + ...);

	{
		std::lock_guard<std::mutex> lock(m_mtx);
#if !defined(TRAP_RELEASE)
		if ((m_importance & Level::Trace) != Level::None)
			fmt::print(fg(fmt::color::magenta), "{}\n", result);
#endif

#ifdef TRACY_ENABLE
		TracyMessageC(result.c_str(), result.size(), tracy::Color::Magenta);
#endif

		m_buffer.emplace_back(Level::Trace, result);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
void TRAP::Log::Debug(Args&& ... args)
{
	ZoneScoped;

	const std::string result = GetTimeStamp() + "[Debug]" + (fmt::format("{}", args) + ...);

	{
		std::lock_guard<std::mutex> lock(m_mtx);
#if !defined(TRAP_RELEASE)
		if ((m_importance & Level::Debug) != Level::None)
			fmt::print(fg(fmt::color::cyan), "{}\n", result);
#endif

#ifdef TRACY_ENABLE
		TracyMessageC(result.c_str(), result.size(), tracy::Color::Cyan);
#endif

		m_buffer.emplace_back(Level::Debug, result);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
void TRAP::Log::Info(Args&& ... args)
{
	ZoneScoped;

	const std::string result = GetTimeStamp() + "[Info]" + (fmt::format("{}", args) + ...);

	{
		std::lock_guard<std::mutex> lock(m_mtx);
#if !defined(TRAP_RELEASE)
		if ((m_importance & Level::Info) != Level::None)
			fmt::print(fg(fmt::color::green), "{}\n", result);
#endif

#ifdef TRACY_ENABLE
		TracyMessageC(result.c_str(), result.size(), tracy::Color::Green);
#endif

		m_buffer.emplace_back(Level::Info, result);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
void TRAP::Log::Warn(Args&& ... args)
{
	ZoneScoped;

	const std::string result = GetTimeStamp() + "[Warn]" + (fmt::format("{}", args) + ...);

	{
		std::lock_guard<std::mutex> lock(m_mtx);
#if !defined(TRAP_RELEASE)
		if ((m_importance & Level::Warn) != Level::None)
			fmt::print(std::cerr, "{}\n", fmt::styled(result, fmt::fg(fmt::color::yellow)));
#endif

#ifdef TRACY_ENABLE
		TracyMessageC(result.c_str(), result.size(), tracy::Color::Yellow);
#endif

		m_buffer.emplace_back(Level::Warn, result);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
void TRAP::Log::Error(Args&& ... args)
{
	ZoneScoped;

	const std::string result = GetTimeStamp() + "[Error]" + (fmt::format("{}", args) + ...);

	{
		std::lock_guard<std::mutex> lock(m_mtx);
#if !defined(TRAP_RELEASE)
		if ((m_importance & Level::Error) != Level::None)
			fmt::print(std::cerr, "{}\n", fmt::styled(result, fmt::fg(fmt::color::red)));
#endif

#ifdef TRACY_ENABLE
		TracyMessageC(result.c_str(), result.size(), tracy::Color::Red);
#endif

		m_buffer.emplace_back(Level::Error, result);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
void TRAP::Log::Critical(Args&& ... args)
{
	ZoneScoped;

	const std::string result = GetTimeStamp() + "[Critical]" + (fmt::format("{}", args) + ...);

	{
		std::lock_guard<std::mutex> lock(m_mtx);
#if !defined(TRAP_RELEASE)
		if ((m_importance & Level::Critical) != Level::None)
			fmt::print(std::cerr, "{}\n", fmt::styled(result, fmt::fg(fmt::color::dark_red)));
#endif

#ifdef TRACY_ENABLE
		TracyMessageC(result.c_str(), result.size(), tracy::Color::DarkRed);
#endif

		m_buffer.emplace_back(Level::Critical, result);
	}
}

#endif /*TRAP_LOG_INL*/
