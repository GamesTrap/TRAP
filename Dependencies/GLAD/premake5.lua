project "GLAD"
    kind "StaticLib"
    language "C"
    systemversion "latest"
    staticruntime "on"
    cdialect "C11"
    warnings "off"

    targetdir ("../../bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("../../bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    files
    {
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c"
    }

    includedirs
    {
        "include"
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