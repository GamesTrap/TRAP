#ifndef TRAP_ENTRYPOINT_H
#define TRAP_ENTRYPOINT_H

#include "Application.h"
#include "Core/PlatformDetection.h"

#ifdef TRAP_PLATFORM_WINDOWS
//Only used by OpenGL
extern "C"
{
	inline _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;		  //Utilize NVIDIA GPU
	inline _declspec(dllexport) int32_t AmdPowerXpressRequestHighPerformance = 1; //Utilize AMD GPU
}
#endif

/// <summary>
/// This function will be called by TRAP engines main function to initialize the engine.
/// IT MUST BE IMPLEMENTED BY ANY CLIENT!
/// </summary>
/// <returns>A unique pointer containing a TRAP::Application</returns>
extern std::unique_ptr<TRAP::Application> TRAP::CreateApplication(int32_t argc, char** argv);

int main(int32_t argc, char** argv);

#endif /*TRAP_ENTRYPOINT_H*/