project "ConvertToSPIRV"
	location "."
	kind "ConsoleApp"
	language "C++"
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

	filter { "action:gmake*", "toolset:gcc" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}

	filter "configurations:ASAN"
		defines "TRAP_ASAN"

	filter "configurations:UBSAN"
		defines "TRAP_UBSAN"

	filter "configurations:LSAN"
		defines "TRAP_LSAN"

	filter "configurations:TSAN"
		defines "TRAP_TSAN"
