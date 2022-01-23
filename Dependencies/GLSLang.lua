project "HLSL"
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
        "GLSLang/glslang/HLSL/hlslAttributes.cpp",
        "GLSLang/glslang/HLSL/hlslParseHelper.cpp",
        "GLSLang/glslang/HLSL/hlslScanContext.cpp",
        "GLSLang/glslang/HLSL/hlslOpMap.cpp",
        "GLSLang/glslang/HLSL/hlslTokenStream.cpp",
        "GLSLang/glslang/HLSL/hlslGrammar.cpp",
        "GLSLang/glslang/HLSL/hlslParseables.cpp",

        "GLSLang/glslang/HLSL/hlslAttributes.h",
        "GLSLang/glslang/HLSL/hlslParseHelper.h",
        "GLSLang/glslang/HLSL/hlslScanContext.h",
        "GLSLang/glslang/HLSL/hlslOpMap.h",
        "GLSLang/glslang/HLSL/hlslTokenStream.h",
        "GLSLang/glslang/HLSL/hlslGrammar.h",
        "GLSLang/glslang/HLSL/hlslParseables.h"
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

project "OGLCompiler"
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
        "GLSLang/OGLCompilersDLL/InitializeDll.cpp",
        "GLSLang/OGLCompilersDLL/InitializeDll.h"
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

project "OSDependent"
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
        "GLSLang/glslang/OSDependent/osinclude.h"
    }

    defines
    {
        "AMD_EXTENSIONS=true",
        "NV_EXTENSIONS=true",
        "ENABLE_OPT=false",
        "ENABLE_HLSL=true"
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

        links
        {
            "pthread"
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


project "StandAlone"
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
        "GLSLang/StandAlone/DirStackFileIncluder.h",
        "GLSLang/StandAlone/ResourceLimits.h",
        "GLSLang/StandAlone/Worklist.h"
    }

    defines
    {
        "AMD_EXTENSIONS=true",
        "NV_EXTENSIONS=true",
        "ENABLE_OPT=false",
        "ENABLE_HLSL=true",
        "_CRT_SECURE_NO_WARNINGS"
    }

    links
    {
        "SPIRV",
        "GLSLANG",
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

        links
        {
            "pthread"
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
        "GLSLang/SPIRV/GlslangToSpv.cpp",
        "GLSLang/SPIRV/InReadableOrder.cpp",
        "GLSLang/SPIRV/Logger.cpp",
        "GLSLang/SPIRV/SpvBuilder.cpp",
        "GLSLang/SPIRV/SpvPostProcess.cpp",
        "GLSLang/SPIRV/doc.cpp",
        "GLSLang/SPIRV/SpvTools.cpp",
        "GLSLang/SPIRV/disassemble.cpp",

        "GLSLang/SPIRV/bitutils.h",
        "GLSLang/SPIRV/spriv.hpp",
        "GLSLang/SPIRV/GLSL.std.450.h",
        "GLSLang/SPIRV/GLSL.ext.EXT.h",
        "GLSLang/SPIRV/GLSL.ext.KHR.h",
        "GLSLang/SPIRV/GlslangToSpv.h",
        "GLSLang/SPIRV/hex_float.h",
        "GLSLang/SPIRV/Logger.h",
        "GLSLang/SPIRV/SpvBuilder.h",
        "GLSLang/SPIRV/spvIR.h",
        "GLSLang/SPIRV/doc.h",
        "GLSLang/SPIRV/SpvTools.h",
        "GLSLang/SPIRV/disassemble.h"
    }

    defines
    {
        "AMD_EXTENSIONS=true",
        "NV_EXTENSIONS=true",
        "ENABLE_OPT=false",
        "ENABLE_HLSL=true"
    }

    includedirs
    {
        "%{IncludeDir.GLSLANG}",
        "GLSLang/"
    }

    links
    {
        "HLSL",
        "OGLCOMPILER",
        "OSDEPENDENT",
        "GLSLANG"
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

project "SPVRemapper"
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
        "GLSLang/SPIRV/GlslangToSpv.cpp",
        "GLSLang/SPIRV/SPVRemapper.cpp",
        "GLSLang/SPIRV/doc.cpp",

        "GLSLang/SPIRV/SPVRemapper.h",
        "GLSLang/SPIRV/doc.h"
    }

    includedirs
    {
        "%{IncludeDir.GLSLANG}",
        "GLSLang/"
    }

    defines
    {
        "AMD_EXTENSIONS=true",
        "NV_EXTENSIONS=true",
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

project "glslang"
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
        "GLSLang/glslang/MachineIndependent/glslang.y",
        "GLSLang/glslang/MachineIndependent/glslang_tab.cpp",
        "GLSLang/glslang/MachineIndependent/attribute.cpp",
        "GLSLang/glslang/MachineIndependent/Constant.cpp",
        "GLSLang/glslang/MachineIndependent/iomapper.cpp",
        "GLSLang/glslang/MachineIndependent/InfoSink.cpp",
        "GLSLang/glslang/MachineIndependent/Initialize.cpp",
        "GLSLang/glslang/MachineIndependent/IntermTraverse.cpp",
        "GLSLang/glslang/MachineIndependent/Intermediate.cpp",
        "GLSLang/glslang/MachineIndependent/ParseContextBase.cpp",
        "GLSLang/glslang/MachineIndependent/ParseHelper.cpp",
        "GLSLang/glslang/MachineIndependent/PoolAlloc.cpp",
        "GLSLang/glslang/MachineIndependent/RemoveTree.cpp",
        "GLSLang/glslang/MachineIndependent/Scan.cpp",
        "GLSLang/glslang/MachineIndependent/ShaderLang.cpp",
        "GLSLang/glslang/MachineIndependent/SymbolTable.cpp",
        "GLSLang/glslang/MachineIndependent/Versions.cpp",
        "GLSLang/glslang/MachineIndependent/intermOut.cpp",
        "GLSLang/glslang/MachineIndependent/limits.cpp",
        "GLSLang/glslang/MachineIndependent/linkValidate.cpp",
        "GLSLang/glslang/MachineIndependent/parseConst.cpp",
        "GLSLang/glslang/MachineIndependent/reflection.cpp",

        "GLSLang/glslang/MachineIndependent/preprocessor/Pp.cpp",
        "GLSLang/glslang/MachineIndependent/preprocessor/PpAtom.cpp",
        "GLSLang/glslang/MachineIndependent/preprocessor/PpContext.cpp",
        "GLSLang/glslang/MachineIndependent/preprocessor/PpScanner.cpp",
        "GLSLang/glslang/MachineIndependent/preprocessor/PpTokens.cpp",

        "GLSLang/glslang/MachineIndependent/propagateNoContraction.cpp",

        "GLSLang/glslang/GenericCodeGen/CodeGen.cpp",
        "GLSLang/glslang/GenericCodeGen/Link.cpp",

        "GLSLang/glslang/Public/ShaderLang.h",

        "GLSLang/glslang/Include/arrays.h",
        "GLSLang/glslang/Include/BaseTypes.h",
        "GLSLang/glslang/Include/Common.h",
        "GLSLang/glslang/Include/ConstantUnion.h",
        "GLSLang/glslang/Include/InfoSink.h",
        "GLSLang/glslang/Include/InitializeGlobals.h",
        "GLSLang/glslang/Include/intermediate.h",
        "GLSLang/glslang/Include/PoolAlloc.h",
        "GLSLang/glslang/Include/ResourceLimits.h",
        "GLSLang/glslang/Include/revision.h",
        "GLSLang/glslang/Include/ShHandle.h",
        "GLSLang/glslang/Include/Types.h",

        "GLSLang/glslang/MachineIndependent/attribute.h",
        "GLSLang/glslang/MachineIndependent/glslang_tab.cpp.h",
        "GLSLang/glslang/MachineIndependent/gl_types.h",
        "GLSLang/glslang/MachineIndependent/Initialize.h",
        "GLSLang/glslang/MachineIndependent/iomapper.h",
        "GLSLang/glslang/MachineIndependent/LiveTraverser.h",
        "GLSLang/glslang/MachineIndependent/localintermediate.h",
        "GLSLang/glslang/MachineIndependent/ParseHelper.h",
        "GLSLang/glslang/MachineIndependent/reflection.h",
        "GLSLang/glslang/MachineIndependent/RemoveTree.h",
        "GLSLang/glslang/MachineIndependent/Scan.h",
        "GLSLang/glslang/MachineIndependent/ScanContext.h",
        "GLSLang/glslang/MachineIndependent/SymbolTable.h",
        "GLSLang/glslang/MachineIndependent/Versions.h",
        "GLSLang/glslang/MachineIndependent/parseVersions.h",
        "GLSLang/glslang/MachineIndependent/propagateNoContraction.h",
        "GLSLang/glslang/MachineIndependent/preprocessor/PpContext.h",
        "GLSLang/glslang/MachineIndependent/preprocessor/PpTokens.h"
    }

    includedirs
    {
        "%{IncludeDir.HLSL}",
        "%{IncludeDir.OGLCOMPILER}",
        "GLSLang/"
    }


    links
    {
        "HLSL",
        "OGLCompiler"
    }

    defines
    {
        "AMD_EXTENSIONS=true",
        "NV_EXTENSIONS=true",
        "ENABLE_OPT=false",
        "ENABLE_HLSL=true",
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "system:windows"
        links
        {
            "OSDependent"
        }

        defines
        {
            "GLSLANG_OSINCLUDE_WIN32=true",
            "GLSLANG_OSINCLUDE_UNIX=false"
        }

    filter "system:linux"
        links
        {
            "OSDependent"
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