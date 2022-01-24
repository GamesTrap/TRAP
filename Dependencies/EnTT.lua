project "EnTT"
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
        "EnTT/src/entt/**.h",
        "EnTT/src/entt/**.hpp",
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
