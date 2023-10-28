#include <cstdint>
#include <fmt/format.h>
#include <type_traits>

#include "Core/PlatformDetection.h"
#include "Core/Types.h"
#include "Utils/Linux.h"
#include "Utils/Win.h"

namespace TRAP::Utils
{
    /// <summary>
    /// RAII wrapper class for exception handlers
    /// </summary>
    class CrashHandler
    {
    public:
        /// <summary>
        /// Constructors.
        /// Sets the user provided exception handler function.
        /// </summary>
        /// <param name="crashHandler">Exception handler function to use. Note: The function signature differs between operating systems.</param>
        template <typename F>
#ifdef TRAP_PLATFORM_LINUX
        requires std::is_invocable_r_v<void, F, i32, siginfo_t*, void*>
#elif defined(TRAP_PLATFORM_WINDOWS)
        requires std::is_invocable_r_v<LONG, F, PEXCEPTION_POINTERS>
#endif
        explicit CrashHandler(const F& crashHandler);

        /// <summary>
        /// Constructor.
        /// This sets the default TRAP exception handlers.
        /// The default exception handlers add exception information to the engine log file and then save it to disk.
        /// </summary>
        CrashHandler();

        /// <summary>
        /// Destructor.
        /// Removes currently set exception handlers.
        /// Sets previous handlers if there were any.
        /// </summary>
        ~CrashHandler();

        /// <summary>
        /// Copy constructor.
        /// </summary>
        CrashHandler(const CrashHandler&) = delete;
        /// <summary>
        /// Move constructor.
        /// </summary>
        CrashHandler(CrashHandler&&) = delete;
        /// <summary>
        /// Copy assignment operator.
        /// </summary>
        CrashHandler& operator=(const CrashHandler&) = delete;
        /// <summary>
        /// Move assignment operator.
        /// </summary>
        CrashHandler& operator=(CrashHandler&&) = delete;

    private:
#ifdef TRAP_PLATFORM_LINUX
        struct
        {
            struct sigaction ill, fpe, segv, pipe, bus, abrt;
        } m_prevSignal{};
#elif defined(TRAP_PLATFORM_WINDOWS)
        void* m_exceptionHandler = nullptr;
#endif
    };
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename F>
#ifdef TRAP_PLATFORM_LINUX
requires std::is_invocable_r_v<void, F, i32, siginfo_t*, void*>
#elif defined(TRAP_PLATFORM_WINDOWS)
requires std::is_invocable_r_v<LONG, F, PEXCEPTION_POINTERS>
#endif
TRAP::Utils::CrashHandler::CrashHandler(const F& crashHandler)
{
#ifdef TRAP_PLATFORM_LINUX
    struct sigaction handler{};
    handler.sa_sigaction = crashHandler;
    handler.sa_flags = SA_SIGINFO;

    sigaction(SIGILL, &handler, &m_prevSignal.ill);
    sigaction(SIGFPE, &handler, &m_prevSignal.fpe);
    sigaction(SIGSEGV, &handler, &m_prevSignal.segv);
    sigaction(SIGPIPE, &handler, &m_prevSignal.pipe);
    sigaction(SIGBUS, &handler, &m_prevSignal.bus);
    sigaction(SIGABRT, &handler, &m_prevSignal.abrt);
#elif defined(TRAP_PLATFORM_WINDOWS)
    m_exceptionHandler = AddVectoredExceptionHandler(1, crashHandler);
#endif
}
