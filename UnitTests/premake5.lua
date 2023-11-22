local firstparty = require "includeandlinkfirstparty"

project "UnitTests"
	location "."
	kind "ConsoleApp"
	language "C++"
	warnings "Off"

	--Add all source and header files
	files
	{
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp"
	}

	firstparty.IncludeTRAPUnitTests()
	firstparty.LinkTRAPUnitTests()

	includedirs
	{
		"src",
		"%{wks.location}",
	}

	filter { "action:gmake*", "toolset:gcc" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}

	filter { "system:linux", "configurations:Debug", "action:gmake*", "toolset:gcc" }
		buildoptions
		{
			"--coverage",
			"-fdeclone-ctor-dtor"
		}
		links
		{
			"gcov"
		}
