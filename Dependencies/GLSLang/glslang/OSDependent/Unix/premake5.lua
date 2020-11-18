project "OSDependent"
    kind "StaticLib"
    language "C++"
    systemversion "latest"
    staticruntime "on"
    cppdialect "C++17"
    warnings "off"

    targetdir ("../../../../../bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("../../../../../bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    filter "system:linux"
      files
      {
        "ossource.cpp",
        "../osinclude.h"
      }

      links
      {
        "pthread"
      }

      defines
      {
        "AMD_EXTENSIONS=true",
        "NV_EXTENSIONS=true",
        "GLSLANG_OSINCLUDE_UNIX=true",
        "ENABLE_OPT=false",
        "ENABLE_HLSL=true"
      }  

    filter "system:macosx"
      files
      {
        "ossource.cpp",
        "../osinclude.h"
      }

      links
      {
        "pthread"
      }

      defines
      {
        "AMD_EXTENSIONS=true",
        "NV_EXTENSIONS=true",
        "GLSLANG_OSINCLUDE_UNIX=true",
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