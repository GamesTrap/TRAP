project "ConvertToSPIRV"
	location "."
	kind "ConsoleApp"
	language "C++"
	staticruntime "on"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"
	warnings "Extra"

	targetdir ("../../bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("../../bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	sysincludedirs
	{
		"../../%{IncludeDir.GLSLANG}",
		"../../%{IncludeDir.SPIRV}"
	}

	filter "system:windows"
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
			optimize "On"

		filter "configurations:RelWithDebInfo"
			runtime "Release"
			optimize "On"
			symbols "On"
