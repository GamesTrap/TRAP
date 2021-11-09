#ifndef TRAP_INSTRUMENTOR_H
#define TRAP_INSTRUMENTOR_H

namespace TRAP::Utils::Debug
{
	/// <summary>
	/// Duration type for microseconds.
	/// </summary>
	using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

	/// <summary>
	/// Profiling results.
	/// </summary>
	struct ProfileResult
	{
		std::string Name;

		FloatingPointMicroseconds Start;
		std::chrono::microseconds ElapsedTime;
		std::thread::id ThreadID;
	};

	/// <summary>
	/// Profiling session.
	/// </summary>
	struct InstrumentationSession
	{
		std::string Name;
	};

	/// <summary>
	/// Basic profiler.
	/// </summary>
	class Instrumentor
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		Instrumentor();
		/// <summary>
		/// Destructor.
		/// </summary>
		~Instrumentor();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		Instrumentor(const Instrumentor&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Instrumentor& operator=(const Instrumentor&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Instrumentor(Instrumentor&&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Instrumentor& operator=(Instrumentor&&) = delete;

		/// <summary>
		/// Retrieve reference to the instrumentor instance.
		/// </summary>
		/// <returns>Instrumentor instance.</returns>
		static Instrumentor& Get();

		/// <summary>
		/// Begin a new profiling session.
		/// </summary>
		/// <param name="name">Name for the new session.</param>
		/// <param name="filePath">Non-VFS file path for the new session.</param>
		void BeginSession(const std::string& name, const std::string& filePath = "results.json");
		/// <summary>
		/// End current profiling session.
		/// </summary>
		void EndSession();

		/// <summary>
		/// Write profiling profile to file.
		/// </summary>
		/// <param name="result">Profiling result.</param>
		void WriteProfile(const ProfileResult& result);

	private:
		/// <summary>
		/// Write header of profiler.
		/// </summary>
		void WriteHeader();
		/// <summary>
		/// Write footer of profiler.
		/// </summary>
		void WriteFooter();
		/// <summary>
		/// End session and close file.
		/// </summary>
		void InternalEndSession();

		std::mutex m_mutex;
		TRAP::Scope<InstrumentationSession> m_currentSession;
		std::ofstream m_outputStream;
	};

	/// <summary>
	/// Profiling timer to profile how long something took.
	/// </summary>
	class InstrumentationTimer
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name of the function to profile.</param>
		explicit InstrumentationTimer(const char* name);

		/// <summary>
		/// Destructor.
		/// </summary>
		~InstrumentationTimer();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		InstrumentationTimer(const InstrumentationTimer&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		InstrumentationTimer& operator=(const InstrumentationTimer&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		InstrumentationTimer(InstrumentationTimer&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		InstrumentationTimer& operator=(InstrumentationTimer&&) = delete;

		/// <summary>
		/// Stop the time and write to profiler.
		/// </summary>
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
				while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 &&
				       expr[srcIndex + matchIndex] == remove[matchIndex])
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
	//Note that this only is resolved when the (pre)compiler starts, so the syntax highlighting
	//could mark the wrong one in your editor!
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

	/// <summary>
	/// Begin a new profiler session.
	/// Note: Only one session at a time is allowed!
	/// </summary>
	/// <param name="name">Name for the session.</param>
	/// <param name="filePath">Physical file path where to store session.</param>
	inline void TP_PROFILE_BEGIN_SESSION(const std::string& name, const std::string& filePath)
	{
		::TRAP::Utils::Debug::Instrumentor::Get().BeginSession(name, filePath);
	}

	/// <summary>
	/// End the current active profiler session.
	/// </summary>
	inline void TP_PROFILE_END_SESSION()
	{
		::TRAP::Utils::Debug::Instrumentor::Get().EndSession();
	}
	#define TP_PROFILE_SCOPE(name) constexpr auto fixedName = \
	    ::TRAP::Utils::Debug::InstrumentorUtils::CleanupOutputString(name, "__cdecl ");\
		::TRAP::Utils::Debug::InstrumentationTimer timer##__LINE__(fixedName.Data);
	#define TP_PROFILE_FUNCTION() TP_PROFILE_SCOPE(TP_FUNC_SIG)
#else
	/// <summary>
	/// Begin a new profiler session.
	/// Note: Only one session at a time is allowed!
	/// </summary>
	/// <param name="name">Name for the session.</param>
	/// <param name="filePath">Physical file path where to store session.</param>
	inline constexpr void TP_PROFILE_BEGIN_SESSION(const std::string&, const std::string&) {}
	/// <summary>
	/// End the current active profiler session.
	/// </summary>
	inline constexpr void TP_PROFILE_END_SESSION() {}
	inline constexpr void TP_PROFILE_SCOPE(const std::string&) {}
	inline constexpr void TP_PROFILE_FUNCTION() {}
#endif

#endif /*TRAP_INSTRUMENTOR_H*/