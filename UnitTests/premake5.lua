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

	postbuildcommands ("{COPYDIR} " .. path.join(_MAIN_SCRIPT_DIR, "UnitTests/Testfiles") .. " %{cfg.targetdir}/Testfiles")

	filter { "toolset:gcc" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}

	filter { "system:linux", "configurations:Debug", "toolset:gcc" }
		buildoptions
		{
			"--coverage",
			"-fdeclone-ctor-dtor"
		}
		links
		{
			"gcov"
		}

	filter {"system:linux", "configurations:Debug"}
		buildoptions
		{
			"-fprofile-update=atomic"
		}
