#ifndef _TRAP_WIN_H_
#define _TRAP_WIN_H_

//Target Windows 7 or later
#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0601
#endif
#include <sdkddkver.h>
//The following #defines disable a bunch of unused windows stuff.
//If you get weird errors when trying to do some windows stuff,
//try removing some (or all) of these defines (it will increase build time though).
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOGDICAPMASKS
	#define NOGDICAPMASKS
#endif
#ifndef NOSYSMETRICS
	#define NOSYSMETRICS
#endif
#ifndef NOMENUS
	#define NOMENUS
#endif
#ifndef NOICONS
	#define NOICONS
#endif
#ifndef NOSYSCOMMANDS
	#define NOSYSCOMMANDS
#endif
#ifndef NORASTEROPS
	#define NORASTEROPS
#endif
#ifndef OEMRESOURCE
	#define OEMRESOURCE
#endif
#ifndef NOATOM
	#define NOATOM
#endif
#ifndef NOCLIPBOARD
	#define NOCLIPBOARD
#endif
#ifndef NOCOLOR
	#define NOCOLOR
#endif
#ifndef NOCTLMGR
	#define NOCTLMGR
#endif
#ifndef NODRAWTEXT
	#define NODRAWTEXT
#endif
#ifndef NOKERNEL	
	#define NOKERNEL
#endif
#ifndef NONLS
	#define NONLS
#endif
#ifndef NOMEMMGR
	#define NOMEMMGR
#endif
#ifndef NOMETAFILE
	#define NOMETAFILE
#endif
#ifndef NOMINMAX
	#define NOMINMAX
#endif
#ifndef NOOPENFILE
	#define NOOPENFILE
#endif
#ifndef NOSCROLL
	#define NOSCROLL
#endif
#ifndef NOSERVICE
	#define NOSERVICE
#endif
#ifndef NOSOUND
	#define NOSOUND
#endif
#ifndef NOTEXTMETRIC
	#define NOTEXTMETRIC
#endif
#ifndef NOWH
	#define NOWH
#endif
#ifndef NOCOMM
	#define NOCOMM
#endif
#ifndef NOKANJI
	#define NOKANJI
#endif
#ifndef NOHELP
	#define NOHELP
#endif
#ifndef NOPROFILER
	#define NOPROFILER
#endif
#ifndef NODEFERWINDOWPOS
	#define NODEFERWINDOWPOS
#endif
#ifndef NOMCX
	#define NOMCX
#endif
#ifndef NORPC
	#define NORPC
#endif
#ifndef NOPROXYSTUB
	#define NOPROXYSTUB
#endif
#ifndef NOIMAGE
	#define NOIMAGE
#endif
#ifndef NOTAPE
	#define NOTAPE
#endif

#ifndef STRICT
	#define STRICT
#endif

#include <Windows.h>

#undef near
#undef DELETE

#endif /*_TRAP_WIN_H_*/