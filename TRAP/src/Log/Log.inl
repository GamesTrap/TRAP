#ifndef _TRAP_LOG_INL_
#define _TRAP_LOG_INL_

//-------------------------------------------------------------------------------------------------------------------//

template <typename ... Args>
void TRAP::Log::Trace(const Args& ... args)
{
	std::ostringstream stream;
	stream << GetTimeStamp() << "[Trace]";
	using List = int[];
	(void)List {
		0, ((void)(stream << args), 0)...
	};

	//Standard White
	std::cout << stream.str() << '\n';
	m_buffer.emplace_back(Level::Trace, stream.str());
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_WINDOWS
template <typename ... Args>
void TRAP::Log::Debug(const Args& ... args)
{
	std::ostringstream stream;
	stream << GetTimeStamp() << "[Debug]";
	using List = int[];
	(void)List {
		0, ((void)(stream << args), 0)...
	};

	//Standard Cyan
	SetColor(FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << stream.str() << '\n';
	ResetColor();
	m_buffer.emplace_back(Level::Debug, stream.str());
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename ... Args>
void TRAP::Log::Info(const Args& ... args)
{
	std::ostringstream stream;
	stream << GetTimeStamp() << "[Info]";
	using List = int[];
	(void)List {
		0, ((void)(stream << args), 0)...
	};

	//Standard Green
	SetColor(FOREGROUND_GREEN);
	std::cout << stream.str() << '\n';
	ResetColor();
	m_buffer.emplace_back(Level::Info, stream.str());
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename ... Args>
void TRAP::Log::Warn(const Args& ... args)
{
	std::ostringstream stream;
	stream << GetTimeStamp() << "[Warn]";
	using List = int[];
	(void)List {
		0, ((void)(stream << args), 0)...
	};

	//Bold Yellow
	SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	std::cout << stream.str() << '\n';
	ResetColor();
	m_buffer.emplace_back(Level::Warn, stream.str());
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename ... Args>
void TRAP::Log::Error(const Args& ... args)
{
	std::ostringstream stream;
	stream << GetTimeStamp() << "[Error]";
	using List = int[];
	(void)List {
		0, ((void)(stream << args), 0)...
	};

	//Bold Red
	SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
	std::cout << stream.str() << '\n';
	ResetColor();
	m_buffer.emplace_back(Level::Error, stream.str());
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename ... Args>
void TRAP::Log::Critical(const Args& ... args)
{
	std::ostringstream stream;
	stream << GetTimeStamp() << "[Critical]";
	using List = int[];
	(void)List {
		0, ((void)(stream << args), 0)...
	};

	//Bold White on Red
	SetColor(BACKGROUND_RED | FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << stream.str() << '\n';
	ResetColor();
	m_buffer.emplace_back(Level::Critical, stream.str());
}

//-------------------------------------------------------------------------------------------------------------------//

#else
template<typename... Args>
void TRAP::Log::Debug(const Args& ... args)
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

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
void TRAP::Log::Info(const Args& ... args)
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

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
void TRAP::Log::Warn(const Args& ... args)
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

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
void TRAP::Log::Error(const Args& ... args)
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

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Args>
void TRAP::Log::Critical(const Args& ... args)
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

#endif /*_TRAP_LOG_INL_*/