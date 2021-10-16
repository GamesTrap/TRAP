project "TestsNetwork"
	location "."
	kind "ConsoleApp"
	language "C++"
	staticruntime "on"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"
	warnings "Extra"

	targetdir ("../../bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("../../bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"../../TRAP/src"
	}

	sysincludedirs
	{
		"../../%{IncludeDir.IMGUI}",
		"../../%{IncludeDir.GLSLANG}",
		"../../%{IncludeDir.SPIRV}",
		"../../%{IncludeDir.VULKAN}/include/",
		"../../%{IncludeDir.SPIRVCROSS}",
		"../../%{IncludeDir.ENTT}",
		"../../%{IncludeDir.YAMLCPP}",
		"../../%{IncludeDir.MODERNDIALOGS}"
	}

	links
	{
		"TRAP"
	}

	filter "system:linux"
		links
		{
			"ImGui",
			"YAMLCpp",
			"ModernDialogs",
			"GLSLang",
			"SPIRV",

			"dl",
			"pthread",
			"X11",
			"HLSL",
			"OGLCompiler",
			"OSDependent",
			"SPIRV-Cross-Core",
			"SPIRV-Cross-GLSL",
			"SPIRV-Cross-HLSL"
		}

		if os.isfile("../../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.so") and
		   os.isdir("../../Dependencies/DiscordGameSDK/cpp") and
		   os.isfile("../../Dependencies/DiscordGameSDK/cpp/discord.h") then

			linkoptions{ "-Wl,/usr/local/lib/discord_game_sdk.so"}

			files
			{
				"../../%{IncludeDir.DISCORDGAMESDK}/**.h",
				"../../%{IncludeDir.DISCORDGAMESDK}/**.cpp"
			}
		end

	filter "system:windows"
		if os.isfile("../../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.dll.lib") and
		   os.isfile("../../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.dll") and
		   os.isdir("../../Dependencies/DiscordGameSDK/cpp") and
		   os.isfile("../../Dependencies/DiscordGameSDK/cpp/discord.h") then

			links
			{
				"../../%{IncludeDir.DISCORDGAMESDK}/../lib/x86_64/discord_game_sdk.dll.lib"
			}

			files
			{
				"../../%{IncludeDir.DISCORDGAMESDK}/**.h",
				"../../%{IncludeDir.DISCORDGAMESDK}/**.cpp"
			}

			postbuildcommands
			{
				"{COPY} ../../%{IncludeDir.DISCORDGAMESDK}/../lib/x86_64/discord_game_sdk.dll %{cfg.targetdir}"
			}
		end

	filter "configurations:Debug"
		defines "TRAP_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "TRAP_RELEASE"
		runtime "Release"
		optimize "On"
		entrypoint "mainCRTStartup"
		kind "WindowedApp"

	filter "configurations:RelWithDebInfo"
		defines "TRAP_RELWITHDEBINFO"
		runtime "Release"
		optimize "On"
		symbols "On"