project "YAMLCpp"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    systemversion "latest"
    warnings "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    files
	{
		"YAMLCpp/src/**.h",
		"YAMLCpp/src/**.cpp",

		"YAMLCpp/include/**.h"
	}

	includedirs "%{IncludeDir.YAMLCPP}"

	defines "YAML_CPP_STATIC_DEFINE"

    filter "configurations:Debug"
	    runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
        optimize "Full"

	filter "configurations:RelWithDebInfo"
		runtime "Release"
        optimize "Debug"
		symbols "On"

	filter "configurations:Profiling"
		editandcontinue "Off"
		runtime "Release"
		optimize "Full"
		symbols "On"