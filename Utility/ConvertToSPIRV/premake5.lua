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
		"%{IncludeDir.FMT}"
	}

	links
	{
		"GLSLang",
		"fmt"
	}

	filter "system:linux"
		links
		{
			"dl",
			"pthread"
		}

	filter { "toolset:gcc" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}
