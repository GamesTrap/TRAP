project "ConvertToSPIRV"
	location "."
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"
	warnings "Extra"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	externalincludedirs
	{
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}"
	}

	links
	{
		"GLSLang",
		"SPIRV",
		"GLSLang-Default-Resource-Limits",
	}

	filter "system:linux"
		links
		{
			"dl",
			"pthread"
		}

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
		runtime "Release"
		optimize "Full"
		symbols "On"
