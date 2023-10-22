project "ConvertToSPIRV"
	location "."
	kind "ConsoleApp"
	language "C++"

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	externalincludedirs
	{
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}",
		"%{IncludeDir.SPIRVTOOLS}",
		"%{IncludeDir.FMT}"
	}

	links
	{
		"SPIRV-Tools",
		"GLSLang",
		"fmt"
	}

	filter "system:linux"
		links
		{
			"dl",
			"pthread"
		}

	filter { "action:gmake*", "toolset:gcc" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}
