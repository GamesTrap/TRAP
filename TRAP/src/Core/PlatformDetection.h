#ifndef _TRAP_PLATFORMDETECTION_H_
#define _TRAP_PLATFORMDETECTION_H_

#ifdef _WIN32
	//Windows x64/x86
	#ifdef _WIN64
		//Windows x64
		#define TRAP_PLATFORM_WINDOWS
	#else
		//Windows x86
		#error "x86 Builds are unsupported!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#error "macOS is unsupported!"
//We also have to check __ANDROID__ before __linux__ since android
//is based on the linux kernel it has __linux__ defined
#elif defined(__ANDROID__)
	#error "Android is unsupported!" //Maybe in the future
#elif defined(__linux__)
	//Linux x86/x64
	#ifdef __LP64__
		//Linux x64
		#define TRAP_PLATFORM_LINUX
	#else
		//Linux x86
		#error "x86 Builds are unsupported!"
	#endif
#else
	#error "Unknown platform!"
#endif

#endif /*_TRAP_PLATFORMDETECTION_H_*/