#include <cstdint>
#include <fmt/format.h>
#include <type_traits>

#include "Core/PlatformDetection.h"
#include "Core/Types.h"
#include "Utils/Linux.h"
#include "Utils/Win.h"

namespace TRAP::Utils
{
    /// @brief RAII wrapper class for exception handlers
    class CrashHandler
    {
    public:
        /// @brief Constructors.
        /// Sets the user provided exception handler function.
        /// @param crashHandler Exception handler function to use.
        /// @note The function signature differs between operating systems.
        template <typename F>
#ifdef TRAP_PLATFORM_LINUX
        requires std::is_invocable_r_v<void, F, i32, siginfo_t*, void*>
#elif defined(TRAP_PLATFORM_WINDOWS)
        requires std::is_invocable_r_v<LONG, F, PEXCEPTION_POINTERS>
#endif
        explicit CrashHandler(const F& crashHandler);

        /// @brief Constructor.
        /// This sets the default TRAP exception handlers.
        /// The default exception handlers add exception information to the engine log file and then save it to disk.
        CrashHandler();

        /// @brief Destructor.
        /// Removes currently set exception handlers.
        /// Sets previous handlers if there were any.
        ~CrashHandler();

        /// @brief Copy constructor.
        consteval CrashHandler(const CrashHandler&) = delete;
        /// @brief Move constructor.
        consteval CrashHandler(CrashHandler&&) noexcept = delete;
        /// @brief Copy assignment operator.
        consteval CrashHandler& operator=(const CrashHandler&) = delete;
        /// @brief Move assignment operator.
        consteval CrashHandler& operator=(CrashHandler&&) noexcept = delete;

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
