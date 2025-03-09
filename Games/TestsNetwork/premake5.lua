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

	filter { "toolset:gcc or clang" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}
