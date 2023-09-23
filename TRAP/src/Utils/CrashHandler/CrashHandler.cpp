#include "TRAPPCH.h"
#include "CrashHandler.h"

namespace
{
	#ifdef TRAP_PLATFORM_LINUX
	void LinuxCrashHandler(const int32_t signal, siginfo_t* const info, [[maybe_unused]] void* const ucontext)
	{
		struct sigaction act{};
		act.sa_handler = SIG_DFL;
		sigaction(SIGABRT, &act, nullptr);

		std::string crashText;
		switch(signal)
		{
		case SIGILL:
			crashText += "Illegal Instruction. ";
			switch(info->si_code)
			{
			case ILL_ILLOPC:
				crashText += "Illegal opcode. ";
				break;
			case ILL_ILLOPN:
				crashText += "Illegal operand. ";
				break;
			case ILL_ILLADR:
				crashText += "Illegal addressing mode. ";
				break;
			case ILL_ILLTRP:
				crashText += "Illegal trap. ";
				break;
			case ILL_PRVOPC:
				crashText += "Privileged opcode. ";
				break;
			case ILL_PRVREG:
				crashText += "Privileged register. ";
				break;
			case ILL_COPROC:
				crashText += "Coprocessor error. ";
				break;
			case ILL_BADSTK:
				crashText += "Internal stack error. ";
				break;
			default:
				break;
			}
			break;
		case SIGFPE:
			crashText += "Floating-point exception. ";
			switch(info->si_code)
			{
			case FPE_INTDIV:
				crashText += "Integer divide by zero. ";
				break;
			case FPE_INTOVF:
				crashText += "Integer overflow. ";
				break;
			case FPE_FLTDIV:
				crashText += "Floating-point divide by zero. ";
				break;
			case FPE_FLTOVF:
				crashText += "Floating-point overflow. ";
				break;
			case FPE_FLTUND:
				crashText += "Floating-point underflow. ";
				break;
			case FPE_FLTRES:
				crashText += "Floating-point inexact result. ";
				break;
			case FPE_FLTINV:
				crashText += "Floating-point invalid operation. ";
				break;
			case FPE_FLTSUB:
				crashText += "Subscript out of range. ";
				break;
			default:
				break;
			}
			break;
		case SIGSEGV:
			crashText += "Invalid memory reference. ";
			switch(info->si_code)
			{
			case SEGV_MAPERR:
				crashText += "Address not mapped to object. ";
				break;
			case SEGV_ACCERR:
				crashText += "Invalid permissions for mapped object. ";
				break;
	#ifdef SEGV_BNDERR
			case SEGV_BNDERR:
				crashText += "Failed address bound checks. ";
				break;
	#endif
	#ifdef SEGV_PKUERR
			case SEGV_PKUERR:
				crashText += "Access was denied by memory protection keys. ";
				break;
	#endif
			default:
				break;
			}
			break;
		case SIGPIPE:
			crashText += "Broken pipe. ";
			break;
		case SIGBUS:
			crashText += "Bus error. ";
			switch(info->si_code)
			{
			case BUS_ADRALN:
				crashText += "Invalid address alignment. ";
				break;
			case BUS_ADRERR:
				crashText += "Nonexistent physical address. ";
				break;
			case BUS_OBJERR:
				crashText += "Object-specific hardware error. ";
				break;
	#ifdef BUS_MCEERR_AR
			case BUS_MCEERR_AR:
				crashText += "Hardware memory error consumed on a machine check; action required. ";
				break;
	#endif
	#ifdef BUS_MCEERR_AO
			case BUS_MCEERR_AO:
				crashText += "Hardware memory error detected in process but not consumed; action optional. ";
				break;
	#endif
			default:
				break;
			}
			break;
		case SIGABRT:
			crashText += "Abort signal from abort(). ";
			break;
		default:
			break;
		}

		if(signal != SIGPIPE)
			crashText += fmt::format("Fault address: {:#x} ", uint64_t(info->si_addr));

		if(!crashText.empty())
			TP_CRITICAL(' ', crashText);
		TRAP::TRAPLog.Save();
		std::abort();
	}
	#endif /*TRAP_PLATFORM_LINUX*/

	//-------------------------------------------------------------------------------------------------------------------//

	#ifdef TRAP_PLATFORM_WINDOWS
	LONG WINAPI WindowsCrashHandler(PEXCEPTION_POINTERS pExp)
	{
		const DWORD ec = pExp->ExceptionRecord->ExceptionCode;
		std::string crashText;
		switch(ec)
		{
		case EXCEPTION_ACCESS_VIOLATION:
			crashText += fmt::format("Exception EXCEPTION_ACCESS_VIOLATION ({:#x}). ", ec);
			switch(pExp->ExceptionRecord->ExceptionInformation[0])
			{
			case 0:
				crashText += fmt::format("Read violation at address ({:#x}). ", pExp->ExceptionRecord->ExceptionInformation[1]);
				break;
			case 1:
				crashText += fmt::format("Write violation at address ({:#x}). ", pExp->ExceptionRecord->ExceptionInformation[1]);
				break;
			case 8:
				crashText += fmt::format("DEP violation at address ({:#x}). ", pExp->ExceptionRecord->ExceptionInformation[1]);
				break;
			default:
				break;
			}
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			crashText += fmt::format("Exception EXCEPTION_ARRAY_BOUNDS_EXCEEDED ({:#x}). ", ec);
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			crashText += fmt::format("Exception EXCEPTION_DATATYPE_MISALIGNMENT ({:#x}). ", ec);
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			crashText += fmt::format("Exception EXCEPTION_FLT_DIVIDE_BY_ZERO ({:#x}). ", ec);
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			crashText += fmt::format("Exception EXCEPTION_ILLEGAL_INSTRUCTION ({:#x}). ", ec);
			break;
		case EXCEPTION_IN_PAGE_ERROR:
			crashText += fmt::format("Exception EXCEPTION_IN_PAGE_ERROR ({:#x}). ", ec);
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			crashText += fmt::format("Exception EXCEPTION_INT_DIVIDE_BY_ZERO ({:#x}). ", ec);
			break;
		case EXCEPTION_PRIV_INSTRUCTION:
			crashText += fmt::format("Exception EXCEPTION_PRIV_INSTRUCTION ({:#x}). ", ec);
			break;
		case EXCEPTION_STACK_OVERFLOW:
			crashText += fmt::format("Exception EXCEPTION_STACK_OVERFLOW ({:#x}). ", ec);
			break;
		default:
			break;
		}

		if (!crashText.empty())
			TP_CRITICAL(' ', crashText);
		TRAP::TRAPLog.Save();

		return EXCEPTION_CONTINUE_SEARCH;
	}
	#endif /*TRAP_PLATFORM_WINDOWS*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::CrashHandler::CrashHandler()
	: CrashHandler
	  (
#ifdef TRAP_PLATFORM_LINUX
		LinuxCrashHandler
#elif defined(TRAP_PLATFORM_WINDOWS)
		WindowsCrashHandler
#endif
	  )
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::CrashHandler::~CrashHandler()
{
#ifdef TRAP_PLATFORM_LINUX
	sigaction(SIGILL, &m_prevSignal.ill, nullptr);
	sigaction(SIGFPE, &m_prevSignal.fpe, nullptr);
	sigaction(SIGSEGV, &m_prevSignal.segv, nullptr);
	sigaction(SIGPIPE, &m_prevSignal.pipe, nullptr);
	sigaction(SIGBUS, &m_prevSignal.bus, nullptr);
	sigaction(SIGABRT, &m_prevSignal.abrt, nullptr);
#elif defined(TRAP_PLATFORM_WINDOWS)
	RemoveVectoredExceptionHandler(m_exceptionHandler);
#endif
}
