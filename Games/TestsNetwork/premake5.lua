local firstparty = require "includeandlinkfirstparty"

project "TestsNetwork"
	location "."
	kind "ConsoleApp"
	language "C++"

	files
	{
		"src/**.h",
		"src/**.cpp",

		"%{wks.location}/Games/Sanitizer.cpp"
	}

	firstparty.IncludeTRAPHeadless()
	firstparty.LinkTRAPHeadless()

	filter { "action:gmake*", "toolset:gcc" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}
