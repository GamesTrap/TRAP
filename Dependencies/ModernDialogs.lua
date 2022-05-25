project "ModernDialogs"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    systemversion "latest"
    warnings "off"

    targetdir ("../bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    files
    {
        "ModernDialogs/ModernDialogs/**.h",
        "ModernDialogs/ModernDialogs/**.cpp"
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
        symbols "On"