project "HLSL"
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
        "../glslang/HLSL/hlslAttributes.cpp",
        "../glslang/HLSL/hlslParseHelper.cpp",
        "../glslang/HLSL/hlslScanContext.cpp",
        "../glslang/HLSL/hlslOpMap.cpp",
        "../glslang/HLSL/hlslTokenStream.cpp",
        "../glslang/HLSL/hlslGrammar.cpp",
        "../glslang/HLSL/hlslParseables.cpp",
        "../glslang/HLSL/hlslAttributes.h",
        "../glslang/HLSL/hlslParseHelper.h",
        "../glslang/HLSL/hlslTokens.h",
        "../glslang/HLSL/hlslScanContext.h",
        "../glslang/HLSL/hlslOpMap.h",
        "../glslang/HLSL/hlslTokenStream.h",
        "../glslang/HLSL/hlslGrammar.h",
        "../glslang/HLSL/hlslParseables.h"
    }

    defines
    {
        "AMD_EXTENSIONS=true",
        "NV_EXTENSIONS=true",
        "GLSLANG_OSINCLUDE_WIN32=true",
        "ENABLE_OPT=false",
        "ENABLE_HLSL=true"
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