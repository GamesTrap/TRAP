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

	filter { "toolset:gcc" }
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

	filter {"options:gencodecoverage", "system:linux"}
		buildoptions
		{
			"-fprofile-update=atomic"
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

	filter { "toolset:gcc" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}
