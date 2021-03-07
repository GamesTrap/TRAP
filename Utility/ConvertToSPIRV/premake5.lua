project "ConvertToSPIRV"
	location "."
	kind "ConsoleApp"
	language "C++"
	staticruntime "on"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"

	targetdir ("../../bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("../../bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"../../%{IncludeDir.GLSLANG}",
		"../../%{IncludeDir.SPIRV}"
	}

	filter "system:windows"
		links
		{
			"GLSLang",
			"SPIRV"
		}

	filter "system:linux"
		links
		{
			"GLSLang",
			"SPIRV",

			"dl",
			"pthread",
			"HLSL",
			"OGLCompiler",
			"OSDependent"
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
