project "OGLCompiler"
    kind "StaticLib"
    language "C++"
    systemversion "latest"
    staticruntime "on"
    cppdialect "C++17"
    warnings "off"

    targetdir ("../../../bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("../../../bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    files
    {
        "InitializeDll.cpp",
        "InitializeDll.h"
    }

    defines
    {
        "AMD_EXTENSIONS=true",
        "NV_EXTENSIONS=true",
        "ENABLE_OPT=false",
        "ENABLE_HLSL=true"
    }

    filter "system:windows"
        defines
        {
            "GLSLANG_OSINCLUDE_WIN32=true"
        }

    filter "system:linux"
        defines
        {
            "GLSLANG_OSINCLUDE_UNIX=true"
        }

    filter "system:macosx"
        defines
        {
            "GLSLANG_OSINCLUDE_UNIX=true"
        }

    filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
        optimize "On"

	filter "configurations:RelWithDebInfo"
		runtime "Release"
        optimize "On"