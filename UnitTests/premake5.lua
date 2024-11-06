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

	filter { "options:gencodecoverage" }
		defines "CATCH_CONFIG_RUNTIME_STATIC_REQUIRE"

	filter { "toolset:gcc" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}

	filter { "options:gencodecoverage", "system:linux", "toolset:gcc" }
		buildoptions
		{
			"-fdeclone-ctor-dtor"
		}

	filter { "options:gencodecoverage", "system:linux", "toolset:clang" }
		buildoptions
		{
			"-fprofile-instr-generate",
			"-fcoverage-mapping"
		}

	filter { "options:gencodecoverage", "system:linux", "toolset:gcc or clang" }
		buildoptions
		{
			"--coverage"
		}
		linkoptions
		{
			"--coverage"
		}
		links
		{
			"gcov"
		}

	filter {"options:gencodecoverage", "system:linux"}
		buildoptions
		{
			"-fprofile-update=atomic"
		}
