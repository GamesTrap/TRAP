local firstparty = require "includeandlinkfirstparty"

project "Tests"
	location "."
	kind "ConsoleApp"
	language "C++"

	files
	{
		"src/**.h",
		"src/**.cpp",

		"%{wks.location}/Games/Sanitizer.cpp"
	}

	firstparty.IncludeTRAP()
	firstparty.LinkTRAP()

	filter { "action:gmake*", "toolset:gcc" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}
