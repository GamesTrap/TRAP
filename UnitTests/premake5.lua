project "UnitTests"
	location "."
	kind "ConsoleApp"
	language "C++"
	warnings "Extra"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	--Add all source and header files
	files
	{
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp",

		"%{IncludeDir.CATCH2}/catch_amalgamated.cpp",
	}

	--Exclude all folders in Platform, since not all platforms need all of these
	removefiles
	{
	}

	includedirs
	{
		"src",

		"%{wks.location}/TRAP/src",
		"%{wks.location}",

		"%{IncludeDir.TRACY}",
		"%{IncludeDir.FMT}",
	}

	externalincludedirs
	{
		"%{IncludeDir.CATCH2}"
	}

	links
	{
	}

	filter "system:linux"
		runpathdirs
		{
			".",
			"%{cfg.targetdir}",
			"$ORIGIN"
		}

	filter { "action:gmake*", "toolset:gcc" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}

	filter "configurations:Debug"
		defines "TRAP_DEBUG"

	filter "configurations:Release"
		defines "TRAP_RELEASE"

	filter "configurations:RelWithDebInfo"
		defines "TRAP_RELWITHDEBINFO"
