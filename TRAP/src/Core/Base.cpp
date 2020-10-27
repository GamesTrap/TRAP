#include "TRAPPCH.h"
#include "Base.h"

//-------------------------------------------------------------------------------------------------------------------//

#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	#ifdef TRAP_PLATFORM_WINDOWS
		void TRAP_DEBUG_BREAK()
		{
			__debugbreak();
		}
	#elif defined(TRAP_PLATFORM_LINUX)
		#include <signal.h>		
		void TRAP_DEBUG_BREAK()
		{
			raise(SIGTRAP);
		}
	#endif
#endif