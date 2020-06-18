#ifndef _TRAP_INSTRUMENTOR_H_
#define _TRAP_INSTRUMENTOR_H_

namespace TRAP::Utils::Debug
{
	using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;
	
	struct ProfileResult
	{
		std::string Name;

		FloatingPointMicroseconds Start;
		std::chrono::microseconds ElapsedTime;
		std::thread::id ThreadID;
	};

	struct InstrumentationSession
	{
		std::string Name;
	};

	class Instrumentor
	{
	public:
		Instrumentor();
		~Instrumentor();
		
		Instrumentor(const Instrumentor&) = delete;
		Instrumentor& operator=(const Instrumentor&) = delete;
		Instrumentor(Instrumentor&&) = delete;
		Instrumentor& operator=(Instrumentor&&) = delete;
		
		static Instrumentor& Get();

		void BeginSession(const std::string& name, const std::string& filePath = "results.json");
		void EndSession();

		void WriteProfile(const ProfileResult& result);
		
	private:
		void WriteHeader();
		void WriteFooter();
		void InternalEndSession();
		
		std::mutex m_mutex;
		InstrumentationSession* m_currentSession;
		std::ofstream m_outputStream;
	};

	class InstrumentationTimer
	{
	public:
		explicit InstrumentationTimer(const char* name);
		~InstrumentationTimer();

		InstrumentationTimer(const InstrumentationTimer&) = default;
		InstrumentationTimer& operator=(const InstrumentationTimer&) = delete;
		InstrumentationTimer(InstrumentationTimer&&) = default;
		InstrumentationTimer& operator=(InstrumentationTimer&&) = delete;

		void Stop();
		
	private:
		const char* m_name = nullptr;
		std::chrono::time_point<std::chrono::steady_clock> m_startTimePoint;
		bool m_stopped;
	};

	namespace InstrumentorUtils
	{
		template<std::size_t N>
		struct ChangeResult
		{
			char Data[N];
		};

		template<std::size_t N, std::size_t K>
		constexpr auto CleanupOutputString(const char(&expr)[N], const char(&remove)[K])
		{
			ChangeResult<N> result = {};

			std::size_t srcIndex = 0;
			std::size_t dstIndex = 0;
			while(srcIndex < N)
			{
				std::size_t matchIndex = 0;
				while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
					matchIndex++;
				if (matchIndex == K - 1)
					srcIndex += matchIndex;
				result.Data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
				srcIndex++;
			}

			return result;
		}
	}
}

//#define TRAP_PROFILE

#ifdef TRAP_PROFILE
	//Resolve which function signature macro will be used.
	//Note that this only is resolved when the (pre)compiler starts, so the syntax highlighting could mark the wrong one in your editor!
	#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
		#define TP_FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define TP_FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__FUNCSIG__)
		#define TP_FUNC_SIG __FUNCSIG__
	#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
		#define TP_FUNC_SIG __FUNCTION__
	#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
		#define TP_FUNC_SIG __FUNC__
	#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
		#define TP_FUNC_SIG __func__
	#elif defined(__cplusplus) && (__cplusplus >= 201103)
		#define TP_FUNC_SIG __func__
	#else
		#define TP_FUNC_SIG "TP_FUNC_SIG unknown!"
	#endif

	inline void TP_PROFILE_BEGIN_SESSION(const std::string& name, const std::string& filePath)
	{
		::TRAP::Utils::Debug::Instrumentor::Get().BeginSession(name, filePath);
	}

	inline void TP_PROFILE_END_SESSION()
	{
		::TRAP::Utils::Debug::Instrumentor::Get().EndSession();
	}
	#define TP_PROFILE_SCOPE(name) constexpr auto fixedName = ::TRAP::Utils::Debug::InstrumentorUtils::CleanupOutputString(name, "__cdecl ");\
		::TRAP::Utils::Debug::InstrumentationTimer timer##__LINE__(fixedName.Data);
	#define TP_PROFILE_FUNCTION() TP_PROFILE_SCOPE(TP_FUNC_SIG)
#else
	constexpr void TP_PROFILE_BEGIN_SESSION(const std::string& name, const std::string& filePath) {}
	constexpr void TP_PROFILE_END_SESSION() {}
	#define TP_PROFILE_SCOPE(name)
	#define TP_PROFILE_FUNCTION()
#endif

#endif /*_TRAP_INSTRUMENTOR_H_*/