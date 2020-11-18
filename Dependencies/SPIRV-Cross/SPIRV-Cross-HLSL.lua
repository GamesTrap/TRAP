project "SPIRV-Cross-HLSL"
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
		"spirv_hlsl.cpp",
		"spirv_hlsl.hpp"
	}
	
	links 
	{
		"SPIRV-Cross-GLSL"
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