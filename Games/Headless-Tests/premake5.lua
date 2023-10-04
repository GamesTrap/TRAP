local thirdparty = require "includethirdparty"

project "Headless-Tests"
	location "."
	kind "ConsoleApp"
	language "C++"
	warnings "Extra"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",

		"%{wks.location}/Games/Sanitizer.cpp"
	}

	includedirs "%{wks.location}/TRAP/src"

	externalincludedirs
	{
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}",
		"%{IncludeDir.VULKAN}/include/",
		"%{IncludeDir.SPIRVCROSS}",
		"%{IncludeDir.ENTT}",
		"%{IncludeDir.YAMLCPP}",
		"%{IncludeDir.MODERNDIALOGS}",
		"%{IncludeDir.VMA}",
		"%{IncludeDir.BOX2D}",
		"%{IncludeDir.TRACY}",
		"%{IncludeDir.FMT}",
		"%{IncludeDir.GCEM}"
	}

	links
	{
		"TRAP-Headless",

		"YAMLCpp",
		"ModernDialogs",
		"GLSLang",
		"Box2D",
		"TracyClient",
		"fmt",
		"SPIRV-Cross"
	}

	defines "TRAP_HEADLESS_MODE"

	-- Nsight Aftermath stuff
	thirdparty.IncludeNsightAftermathSDK()
	-- Steamworks SDK stuff
	thirdparty.IncludeSteamworksSDK()

	filter "system:linux"
		links
		{
			"dl",
			"pthread",

			"wayland-client",
			"wayland-cursor",
			"xkbcommon"
		}

		runpathdirs
		{
			".",
			"%{cfg.targetdir}",
			"$ORIGIN"
		}

		externalincludedirs "%{IncludeDir.WAYLAND}"

	filter "system:windows"
		links
		{
			-- Needed for Networking
			"ws2_32",
			"wsock32"
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
		entrypoint "mainCRTStartup"
		kind "WindowedApp"

	filter "configurations:RelWithDebInfo"
		defines "TRAP_RELWITHDEBINFO"

	filter "configurations:Profiling"
		defines
		{
			"TRAP_RELEASE",
			"TRACY_ENABLE"
		}

	filter "configurations:ASAN"
		defines
		{
			"TRAP_RELWITHDEBINFO",
			"TRAP_ASAN"
		}

	filter "configurations:UBSAN"
		defines
		{
			"TRAP_RELWITHDEBINFO",
			"TRAP_UBSAN"
		}

	filter "configurations:LSAN"
		defines
		{
			"TRAP_RELWITHDEBINFO",
			"TRAP_LSAN"
		}

	filter "configurations:TSAN"
		defines
		{
			"TRAP_RELWITHDEBINFO",
			"TRAP_TSAN"
		}
