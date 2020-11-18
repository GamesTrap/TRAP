include "Dependencies/GLSLang/hlsl"
include "Dependencies/GLSLang/OGLCompilersDLL"

filter "system:windows"
    include "Dependencies/GLSLang/glslang/OSDependent/Windows"

filter "system:linux"
    include "Dependencies/GLSLang/glslang/OSDependent/Unix"

filter "system:macosx"
    include "Dependencies/GLSLang/glslang/OSDependent/Unix"

project "glslang"
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
        "MachineIndependent/glslang.y",
        "MachineIndependent/glslang_tab.cpp",
        "MachineIndependent/attribute.cpp",
        "MachineIndependent/Constant.cpp",
        "MachineIndependent/iomapper.cpp",
        "MachineIndependent/InfoSink.cpp",
        "MachineIndependent/Initialize.cpp",
        "MachineIndependent/IntermTraverse.cpp",
        "MachineIndependent/Intermediate.cpp",
        "MachineIndependent/ParseContextBase.cpp",
        "MachineIndependent/ParseHelper.cpp",
        "MachineIndependent/PoolAlloc.cpp",
        "MachineIndependent/RemoveTree.cpp",
        "MachineIndependent/Scan.cpp",
        "MachineIndependent/ShaderLang.cpp",
        "MachineIndependent/SymbolTable.cpp",
        "MachineIndependent/Versions.cpp",
        "MachineIndependent/intermOut.cpp",
        "MachineIndependent/limits.cpp",
        "MachineIndependent/linkValidate.cpp",
        "MachineIndependent/parseConst.cpp",
        "MachineIndependent/reflection.cpp",
        "MachineIndependent/preprocessor/Pp.cpp",
        "MachineIndependent/preprocessor/PpAtom.cpp",
        "MachineIndependent/preprocessor/PpContext.cpp",
        "MachineIndependent/preprocessor/PpScanner.cpp",
        "MachineIndependent/preprocessor/PpTokens.cpp",
        "MachineIndependent/propagateNoContraction.cpp",
        "GenericCodeGen/CodeGen.cpp",
        "GenericCodeGen/Link.cpp",
        "Public/ShaderLang.h",
        "Include/arrays.h",
        "Include/BaseTypes.h",
        "Include/Common.h",
        "Include/ConstantUnion.h",
        "Include/InfoSink.h",
        "Include/InitializeGlobals.h",
        "Include/intermediate.h",
        "Include/PoolAlloc.h",
        "Include/ResourceLimits.h",
        "Include/revision.h",
        "Include/ShHandle.h",
        "Include/Types.h",
        "MachineIndependent/attribute.h",
        "MachineIndependent/glslang_tab.cpp.h",
        "MachineIndependent/gl_types.h",
        "MachineIndependent/Initialize.h",
        "MachineIndependent/iomapper.h",
        "MachineIndependent/LiveTraverser.h",
        "MachineIndependent/localintermediate.h",
        "MachineIndependent/ParseHelper.h",
        "MachineIndependent/reflection.h",
        "MachineIndependent/RemoveTree.h",
        "MachineIndependent/Scan.h",
        "MachineIndependent/ScanContext.h",
        "MachineIndependent/SymbolTable.h",
        "MachineIndependent/Versions.h",
        "MachineIndependent/parseVersions.h",
        "MachineIndependent/propagateNoContraction.h",
        "MachineIndependent/preprocessor/PpContext.h",
        "MachineIndependent/preprocessor/PpTokens.h"
    }

    includedirs
    {        
        "%{IncludeDir.HLSL}",
        "%{IncludeDir.OGLCOMPILER}",
        "../"
    }


    links
    {
        "HLSL",
        "OGLCompiler"
    }

    filter "system:windows"
        links
        {
            "OSDependent"
        }

        defines
        {
            "AMD_EXTENSIONS=true",
            "NV_EXTENSIONS=true",
            "GLSLANG_OSINCLUDE_WIN32=true",
            "ENABLE_OPT=false",
            "ENABLE_HLSL=true",
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "system:linux"
        links
        {
            "OSDependent"
        }

        defines
        {
            "AMD_EXTENSIONS=true",
            "NV_EXTENSIONS=true",
            "GLSLANG_OSINCLUDE_UNIX=true",
            "ENABLE_OPT=false",
            "ENABLE_HLSL=true",
            "_CRT_SECURE_NO_WARNINGS"
        }   

    filter "system:macosx"
        defines
            {
                "AMD_EXTENSIONS=true",
                "NV_EXTENSIONS=true",
                "GLSLANG_OSINCLUDE_UNIX=true",
                "ENABLE_OPT=false",
                "ENABLE_HLSL=true",
                "_CRT_SECURE_NO_WARNINGS"
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