local thirdparty = require "includethirdparty"

project "UnitTests"
	location "."
	kind "ConsoleApp"
	language "C++"
	warnings "Extra"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	--Add all source and header files
	files
	{
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp",

		"%{IncludeDir.CATCH2}/catch_amalgamated.cpp",
	}

	--Exclude all folders in Platform, since not all platforms need all of these
	removefiles
	{
	}

	defines
	{
		"YAML_CPP_STATIC_DEFINE",
		"IMGUI_DISABLE_OBSOLETE_FUNCTIONS",
		"IMGUI_DISABLE_OBSOLETE_KEYIO",
		"TRAP_UNITTESTS"
	}

	includedirs
	{
		"src",

		"%{wks.location}",
	}

	externalincludedirs
	{
		"%{IncludeDir.CATCH2}",
		"%{wks.location}/TRAP/src",

		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.IMGUIZMO}",
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
		"%{IncludeDir.FMT}"
	}

	links
	{
		"TRAP-UnitTests"
	}

	-- Discord Game SDK stuff
	thirdparty.IncludeDiscordGameSDK()
	-- Nsight Aftermath stuff
	thirdparty.IncludeNsightAftermathSDK()
	-- Steamworks SDK stuff
	thirdparty.IncludeSteamworksSDK()
	-- NVIDIA Reflex SDK stuff
	thirdparty.IncludeNVIDIAReflexSDK()

	filter "system:linux"
		links
		{
			"ImGui",
			"ImGuizmo",
			"YAMLCpp",
			"ModernDialogs",
			"GLSLang",
			"SPIRV",
			"GLSLang-Default-Resource-Limits",
			"Box2D",
			"TracyClient",
			"fmt",

			"dl",
			"pthread",
			"SPIRV-Cross-Core",
			"SPIRV-Cross-GLSL",
			"SPIRV-Cross-HLSL",

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
