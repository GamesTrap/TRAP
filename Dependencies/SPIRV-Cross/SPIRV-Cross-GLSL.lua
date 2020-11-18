project "SPIRV-Cross-GLSL"
    kind "StaticLib"
    language "C++"
    systemversion "latest"
    staticruntime "on"
	cppdialect "C++17"
	warnings "off"

    targetdir ("../../bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("../../bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    files
    {
		"spirv_glsl.cpp",
		"spirv_glsl.hpp"
	}
	
	links 
	{
		"SPIRV-Cross-Core"
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