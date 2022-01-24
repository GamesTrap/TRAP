project "GLSLang"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    systemversion "latest"
    warnings "off"

    targetdir ("../bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    files
    {
        "GLSLang/glslang/GenericCodeGen/*.cpp",
        "GLSLang/glslang/HLSL/*.cpp",
        "GLSLang/glslang/MachineIndependent/*.cpp",
        "GLSLang/glslang/MachineIndependent/preprocessor/*.cpp",

        "GLSLang/OGLCompilersDLL/InitializeDll.cpp",

        "GLSLang/glslang/HLSL/*.h",
        "GLSLang/glslang/Include/*.h",
        "GLSLang/glslang/MachineIndependent/*.h",
        "GLSLang/glslang/MachineIndependent/preprocessor/*.h",

        "GLSLang/OGLCompilersDLL/InitializeDll.h",
        "GLSLang/StandAlone/DirStackFileIncluder.h",
        "GLSLang/glslang/OSDependent/osinclude.h",
        "GLSLang/glslang/Public/ShaderLang.h"
    }

    removefiles
    {
        "GLSLang/glslang/HLSL/pch.h",
        "GLSLang/glslang/MachineIndependent/pch.h"
    }

    includedirs
    {
        "%{IncludeDir.HLSL}",
        "%{IncludeDir.OGLCOMPILER}",
        "GLSLang/"
    }

    -- links
    -- {
    --     "HLSL",
    --     "OGLCompiler",
    --     "OSDependent"
    -- }

    defines
    {
        -- "AMD_EXTENSIONS",
        -- "NV_EXTENSIONS",
        "ENABLE_OPT=0",
        "ENABLE_HLSL=0",
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "system:windows"
        files
        {
            "GLSLang/glslang/OSDependent/Windows/ossource.cpp"
        }

        defines
        {
            "GLSLANG_OSINCLUDE_WIN32=true",
            "GLSLANG_OSINCLUDE_UNIX=false"
        }

    filter "system:linux"
        files
        {
            "GLSLang/glslang/OSDependent/Unix/ossource.cpp"
        }

        defines
        {
            "GLSLANG_OSINCLUDE_WIN32=false",
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

project "SPIRV"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    systemversion "latest"
    warnings "off"

    targetdir ("../bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    files
    {
        "GLSLang/SPIRV/*.cpp",

        "GLSLang/SPIRV/GlslangToSpv.h",
        "GLSLang/SPIRV/Logger.h",
        "GLSLang/SPIRV/SPVRemapper.h",
        "GLSLang/SPIRV/SpvBuilder.h",
        "GLSLang/SPIRV/SpvTools.h",
        "GLSLang/SPIRV/bitutils.h",
        "GLSLang/SPIRV/disassemble.h",
        "GLSLang/SPIRV/doc.h",
        "GLSLang/SPIRV/hex_float.h",
        "GLSLang/SPIRV/spvIR.h",

        "GLSLang/SPIRV/GLSL.ext.AMD.h",
        "GLSLang/SPIRV/GLSL.ext.EXT.h",
        "GLSLang/SPIRV/GLSL.ext.KHR.h",
        "GLSLang/SPIRV/GLSL.ext.NV.h",
        "GLSLang/SPIRV/GLSL.ext.450.h",
        "GLSLang/SPIRV/NonSemanticDebugPrintf.h",
        "GLSLang/SPIRV/spirv.hpp"
    }

    removefiles
    {
        "GLSLang/SPIRV/SpvTools.cpp"
    }

    includedirs
    {
        "GLSLang/SPIRV/",
        "GLSLang/",
    }

    links
    {
        "GLSLang"
    }

    filter "system:windows"
        defines
        {
            "GLSLANG_OSINCLUDE_WIN32=true",
            "GLSLANG_OSINCLUDE_UNIX=false"
        }

    filter "system:linux"
        defines
        {
            "GLSLANG_OSINCLUDE_WIN32=false",
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

project "GLSLang-Default-Resource-Limits"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    systemversion "latest"
    warnings "off"

    targetdir ("../bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    files
    {
        "GLSLang/StandAlone/ResourceLimits.cpp",
        "GLSLang/StandAlone/ResourceLimits.h"
    }

    links
    {
        "GLSLang"
    }

    filter "system:windows"
        defines
        {
            "GLSLANG_OSINCLUDE_WIN32=true",
            "GLSLANG_OSINCLUDE_UNIX=false"
        }

    filter "system:linux"
        defines
        {
            "GLSLANG_OSINCLUDE_WIN32=false",
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