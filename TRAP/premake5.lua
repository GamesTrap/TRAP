local firstparty = require "includeandlinkfirstparty"

function SharedOSIncludes()
	filter {}
	removefiles
	{
		"src/Utils/Win.h",
		"src/Utils/Linux.h",
		"src/Input/WindowsInput.cpp",
		"src/Input/LinuxInput.cpp",
		"src/Window/WindowingAPIWin32.cpp",
		"src/Window/WindowingAPILinux.cpp",
		"src/Window/WindowingAPILinuxX11.cpp",
		"src/Window/WindowingAPILinuxWayland.cpp",
		"src/Network/Sockets/Platform/SocketImplLinux.h",
		"src/Network/Sockets/Platform/SocketImplLinux.cpp",
		"src/Network/Sockets/Platform/SocketImplWinAPI.h",
		"src/Network/Sockets/Platform/SocketImplWinAPI.cpp"
	}

	filter "system:windows"
		files
		{
			"src/Utils/Win.h",
			"src/Input/WindowsInput.cpp",
			"src/Window/WindowingAPIWin32.cpp",
			"src/Network/Sockets/Platform/SocketImplWinAPI.h",
			"src/Network/Sockets/Platform/SocketImplWinAPI.cpp"
		}

	filter "system:linux"
        files
        {
			"src/Utils/Linux.h",
			"src/Input/LinuxInput.cpp",
			"src/Window/WindowingAPILinux.cpp",
			"src/Window/WindowingAPILinuxX11.cpp",
			"src/Window/WindowingAPILinuxWayland.cpp",
			"src/Network/Sockets/Platform/SocketImplLinux.h",
			"src/Network/Sockets/Platform/SocketImplLinux.cpp",

			"%{IncludeDir.WAYLAND}/**.h"
		}

	filter{}
end

project "TRAP"
	location "."
	kind "StaticLib"
	language "C++"

	pchheader "TRAPPCH.h"
	pchsource "src/TRAPPCH.cpp"

	--Add all source and header files
	files
	{
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp",
		"src/**.inl",
	}

	SharedOSIncludes()

	firstparty.IncludeTRAP()

	filter { "action:gmake*", "toolset:gcc" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}

project "TRAP-UnitTests"
	location "."
	kind "StaticLib"
	language "C++"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	pchheader "TRAPPCH.h"
	pchsource "src/TRAPPCH.cpp"

	--Add all source and header files
	files
	{
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp",
		"src/**.inl",
	}

	SharedOSIncludes()

	removefiles
	{
		"src/EntryPoint.cpp"
	}

	firstparty.IncludeTRAPUnitTests()

	filter { "system:linux", "configurations:Debug", "action:gmake*", "toolset:gcc" }
		buildoptions
		{
			"--coverage"
		}
		links
		{
			"gcov"
		}


	filter { "action:gmake*", "toolset:gcc" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}

	filter { "system:linux", "configurations:Debug", "action:gmake*", "toolset:gcc" }
		buildoptions
		{
			"-fdeclone-ctor-dtor"
		}

project "TRAP-Headless"
	location "."
	kind "StaticLib"
	language "C++"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	pchheader "TRAPPCH.h"
	pchsource "src/TRAPPCH.cpp"

	--Add all source and header files
	files
	{
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp",
		"src/**.inl"
	}

	SharedOSIncludes()

	firstparty.IncludeTRAPHeadless()

	filter { "action:gmake*", "toolset:gcc" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}
