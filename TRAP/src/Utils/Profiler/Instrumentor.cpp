#include "TRAPPCH.h"
#include "Instrumentor.h"

TRAP::Utils::Debug::Instrumentor::~Instrumentor()
{
	EndSession();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Debug::Instrumentor& TRAP::Utils::Debug::Instrumentor::Get()
{
	static Instrumentor instance;

	return instance;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Debug::Instrumentor::BeginSession(const std::string& name, const std::string& filePath)
{
	if (m_activeSession)
		EndSession();

	m_activeSession = true;

	m_outputStream.open(filePath);

	if (m_outputStream.is_open())
	{
		m_sessionName = name;
		WriteHeader();
	}
	else
		TP_ERROR("[Instrumentor] Could not open results file: ", filePath);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Debug::Instrumentor::EndSession()
{
	if (!m_activeSession)
		return;

	m_activeSession = false;
	WriteFooter();
	m_outputStream.close();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Debug::Instrumentor::WriteProfile(const ProfileResult& result)
{
	std::lock_guard<std::mutex> lock(m_lock);
	
	std::stringstream json;

	std::string name = result.Name;
	std::replace(name.begin(), name.end(), '"', '\'');

	json << std::setprecision(3) << std::fixed;
	json << ",{";
	json << "\"cat\":\"function\",";
	json << "\"dur\":" << (result.ElapsedTime.count()) << ',';
	json << "\"name\":\"" << name << "\",";
	json << "\"ph\":\"X\",";
	json << "\"pid\":0,";
	json << "\"tid\":" << result.ThreadID << ",";
	json << "\"ts\":" << result.Start.count();
	json << "}";
	
	if(m_outputStream.is_open())
		m_outputStream << json.str();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Debug::Instrumentor::WriteHeader()
{
	m_outputStream << "{\"otherData\": {},\"traceEvents\":[{}";
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Debug::Instrumentor::WriteFooter()
{
	m_outputStream << "]}";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Debug::Instrumentor::Instrumentor()
	: m_sessionName("None"), m_activeSession(false)
{	
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
	const auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimePoint).time_since_epoch();

	Instrumentor::Get().WriteProfile({ m_name, highResStart, elapsedTime, std::this_thread::get_id() });

	m_stopped = true;
}