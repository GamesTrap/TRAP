project "Sandbox"
	location "."
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"
	warnings "Extra"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/TRAP/src"
	}

	sysincludedirs
	{
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}",
		"%{IncludeDir.VULKAN}/include/",
		"%{IncludeDir.SPIRVCROSS}",
		"%{IncludeDir.ENTT}",
		"%{IncludeDir.YAMLCPP}",
		"%{IncludeDir.MODERNDIALOGS}",
		"%{IncludeDir.VMA}"
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
			"GLSLang-Default-Resource-Limits",

			"dl",
			"pthread",
			"SPIRV-Cross-Core",
			"SPIRV-Cross-GLSL",
			"SPIRV-Cross-HLSL"
		}

		-- Discord Game SDK stuff
		if os.isfile("../../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.so") and
		   os.isdir("../../Dependencies/DiscordGameSDK/cpp") and
		   os.isfile("../../Dependencies/DiscordGameSDK/cpp/discord.h") then

			linkoptions{ "-Wl,/usr/local/lib/discord_game_sdk.so"}

			files
			{
				"%{IncludeDir.DISCORDGAMESDK}/**.h",
				"%{IncludeDir.DISCORDGAMESDK}/**.cpp"
			}

			defines "USE_DISCORD_GAME_SDK"
		end

		-- Nsight Aftermath stuff
		if os.isfile("../../Dependencies/Nsight-Aftermath/lib/x64/libGFSDK_Aftermath_Lib.x64.so") and
		   os.isdir("../../Dependencies/Nsight-Aftermath/include") and
		   os.isfile("../../Dependencies/Nsight-Aftermath/include/GFSDK_Aftermath.h") then
			sysincludedirs
			{
				"%{IncludeDir.NSIGHTAFTERMATH}"
			}

			defines "NSIGHT_AFTERMATH_AVAILABLE"
		end

	filter "system:windows"
		-- Discord Game SDK stuff
		if os.isfile("../../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.dll.lib") and
		   os.isfile("../../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.dll") and
		   os.isdir("../../Dependencies/DiscordGameSDK/cpp") and
		   os.isfile("../../Dependencies/DiscordGameSDK/cpp/discord.h") then

			links
			{
				"%{IncludeDir.DISCORDGAMESDK}/../lib/x86_64/discord_game_sdk.dll.lib"
			}

			files
			{
				"%{IncludeDir.DISCORDGAMESDK}/**.h",
				"%{IncludeDir.DISCORDGAMESDK}/**.cpp"
			}

			postbuildcommands
			{
				"{COPY} %{IncludeDir.DISCORDGAMESDK}/../lib/x86_64/discord_game_sdk.dll %{cfg.targetdir}"
			}

			defines "USE_DISCORD_GAME_SDK"
		end

		-- Nsight Aftermath stuff
		if os.isfile("../../Dependencies/Nsight-Aftermath/lib/x64/GFSDK_Aftermath_Lib.x64.dll") and
		   os.isfile("../../Dependencies/Nsight-Aftermath/lib/x64/GFSDK_Aftermath_Lib.x64.lib") and
		   os.isfile("../../Dependencies/Nsight-Aftermath/lib/x64/llvm_7_0_1.dll") and
		   os.isdir("../../Dependencies/Nsight-Aftermath/include") and
		   os.isfile("../../Dependencies/Nsight-Aftermath/include/GFSDK_Aftermath.h") then
			sysincludedirs
			{
				"%{IncludeDir.NSIGHTAFTERMATH}"
			}

			postbuildcommands
			{
				"{COPY} %{IncludeDir.NSIGHTAFTERMATH}/../lib/x64/GFSDK_Aftermath_Lib.x64.dll %{cfg.targetdir}"
			}

			defines "NSIGHT_AFTERMATH_AVAILABLE"
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
