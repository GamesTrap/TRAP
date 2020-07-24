#include "TRAPPCH.h"
#include "Instrumentor.h"

TRAP::Utils::Debug::Instrumentor::Instrumentor()
	: m_currentSession(nullptr)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Debug::Instrumentor::~Instrumentor()
{
	EndSession();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Debug::Instrumentor::BeginSession(const std::string& name, const std::string& filePath)
{
	std::lock_guard lock(m_mutex);
	if(m_currentSession)
	{
		//If there is already a current session, then close it before beginning a new one.
		//Subsequent profiling output meant for the original session will end up in the newly opened session instead.
		//That is better than having badly formatted profiling output.
		TP_ERROR(Log::InstrumentorPrefix, "Instrumentor::BeginSession('", name, "') when session '", m_currentSession->Name, "' already open!");
		InternalEndSession();
	}
	m_outputStream.open(filePath);
	
	if(m_outputStream.is_open())
	{
		m_currentSession = new InstrumentationSession({ name });
		WriteHeader();
	}
	else
		TP_ERROR(Log::InstrumentorPrefix, "Could not open results file '", filePath, "'!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Debug::Instrumentor::EndSession()
{
	std::lock_guard lock(m_mutex);
	InternalEndSession();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Debug::Instrumentor::WriteProfile(const ProfileResult& result)
{
	std::stringstream json;

	json << std::setprecision(3) << std::fixed;
	json << ",{";
	json << "\"cat\":\"function\",";
	json << "\"dur\":" << (result.ElapsedTime.count()) << ',';
	json << "\"name\":\"" << result.Name << "\",";
	json << "\"ph\":\"X\",";
	json << "\"pid\":0,";
	json << "\"tid\":" << result.ThreadID << ",";
	json << "\"ts\":" << result.Start.count();
	json << "}";
	
	std::lock_guard lock(m_mutex);
	if(m_currentSession)
	{
		m_outputStream << json.str();
		m_outputStream.flush();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Debug::Instrumentor& TRAP::Utils::Debug::Instrumentor::Get()
{
	static Instrumentor instance;

	return instance;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Debug::Instrumentor::WriteHeader()
{
	m_outputStream << "{\"otherData\": {},\"traceEvents\":[{}";
	m_outputStream.flush();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Debug::Instrumentor::WriteFooter()
{
	m_outputStream << "]}";
	m_outputStream.flush();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Debug::Instrumentor::InternalEndSession()
{
	if(m_currentSession)
	{
		WriteFooter();
		m_outputStream.close();
		delete m_currentSession;
		m_currentSession = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Debug::InstrumentationTimer::InstrumentationTimer(const char* name)
	: m_name(name), m_stopped(false)
{
	m_startTimePoint = std::chrono::steady_clock::now();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Debug::InstrumentationTimer::~InstrumentationTimer()
{
	if (!m_stopped)
		Stop();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Debug::InstrumentationTimer::Stop()
{
	const auto endTimePoint = std::chrono::steady_clock::now();
	const auto highResStart = FloatingPointMicroseconds{ m_startTimePoint.time_since_epoch() };
	const auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch() -
			std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimePoint).time_since_epoch();

	Instrumentor::Get().WriteProfile({ m_name, highResStart, elapsedTime, std::this_thread::get_id() });

	m_stopped = true;
}