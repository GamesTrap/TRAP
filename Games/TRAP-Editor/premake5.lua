local firstparty = require "includeandlinkfirstparty"

project "TRAP-Editor"
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
