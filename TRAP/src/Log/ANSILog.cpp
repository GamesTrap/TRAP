#include "TRAPPCH.h"
#include "Log.h"

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Log::IsColorTerminal() noexcept
{
	static constexpr std::array<const char*, 14> Terms = { "ansi", "color", "console", "cygwin", "gnome", "konsole", "kterm", "linux", "msys", "putty", "rxvt", "screen", "vt100", "xterm" };
	const char* envP = std::getenv("TERM");
	if (envP == nullptr)
		return false;

	return std::any_of(Terms.begin(), Terms.end(), [&](const char* term) {return std::strstr(envP, term) != nullptr; });
}