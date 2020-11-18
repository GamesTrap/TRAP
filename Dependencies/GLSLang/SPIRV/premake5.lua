project "SPIRV"
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
        "GlslangToSpv.cpp",
        "InReadableOrder.cpp",
        "Logger.cpp",
        "SpvBuilder.cpp",
        "SpvPostProcess.cpp",
        "doc.cpp",
        "SpvTools.cpp",
        "disassemble.cpp",
        "bitutils.h",
        "spirv.hpp",
        "GLSL.std.450.h",
        "GLSL.ext.EXT.h",
        "GLSL.ext.KHR.h",
        "GlslangToSpv.h",
        "hex_float.h",
        "Logger.h",
        "SpvBuilder.h",
        "spvIR.h",
        "doc.h",
        "SpvTools.h",
        "disassemble.h"
    }

    defines
    {
        "AMD_EXTENSIONS=true",
        "NV_EXTENSIONS=true",
        "GLSLANG_OSINCLUDE_WIN32=true",
        "ENABLE_OPT=false",
        "ENABLE_HLSL=true"
    }

    includedirs
    {
        "%{IncludeDir.GLSLANG}",
        "../"
    }

    links
    {
        "HLSL",
        "OGLCOMPILER",
        "OSDEPENDENT",
        "GLSLANG"
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

project "SPVRemapper"
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
        "GlslangToSpv.cpp",
        "SPVRemapper.cpp",
        "doc.cpp",
        "SPVRemapper.h",
        "doc.h"
    }

    includedirs
    {
        "%{IncludeDir.GLSLANG}",
        "../"
    }
    
    defines
    {
        "AMD_EXTENSIONS=true",
        "NV_EXTENSIONS=true",
        "GLSLANG_OSINCLUDE_WIN32=true",
        "ENABLE_OPT=false",
        "ENABLE_HLSL=true"
    }
    
    links
    {
        "HLSL",
        "OGLCOMPILER",
        "OSDEPENDENT",
        "GLSLANG"
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