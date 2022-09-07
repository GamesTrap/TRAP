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

	externalincludedirs
	{
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.IMGUIZMO}",
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
			"ImGuizmo",
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

		runpathdirs
		{
			".",
			"%{cfg.targetdir}",
			"$ORIGIN"
		}

		-- Discord Game SDK stuff
		if (os.isfile("../../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.so") or
		    os.isfile("../../Dependencies/DiscordGameSDK/lib/x86_64/libdiscord_game_sdk.so")) and
		   os.isdir("../../Dependencies/DiscordGameSDK/cpp") and
		   os.isfile("../../Dependencies/DiscordGameSDK/cpp/discord.h") then

			links "discord_game_sdk"
			libdirs "%{IncludeDir.DISCORDGAMESDK}/../lib/x86_64"

			postbuildcommands
			{
				"{COPYFILE} %{IncludeDir.DISCORDGAMESDK}/../lib/x86_64/libdiscord_game_sdk.so %{cfg.targetdir}"
			}

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
			externalincludedirs
			{
				"%{IncludeDir.NSIGHTAFTERMATH}"
			}

			postbuildcommands
			{
				"{COPYFILE} %{IncludeDir.NSIGHTAFTERMATH}/../lib/x64/libGFSDK_Aftermath_Lib.x64.so %{cfg.targetdir}"
			}

			defines "NSIGHT_AFTERMATH_AVAILABLE"
		end

		-- Steamworks SDK stuff
		if os.isfile("../../Dependencies/SteamworksSDK/sdk/redistributable_bin/linux64/libsteam_api.so") and
		   os.isdir("../../Dependencies/SteamworksSDK/sdk/public/steam") then

			links "steam_api"
			libdirs "%{IncludeDir.STEAMWORKSSDK}/../../redistributable_bin/linux64"

			postbuildcommands
			{
				"{COPYFILE} %{IncludeDir.STEAMWORKSSDK}/../../redistributable_bin/linux64/libsteam_api.so %{cfg.targetdir}"
			}

			files
			{
				"%{IncludeDir.STEAMWORKSSDK}/**.h"
			}

			defines "USE_STEAMWORKS_SDK"
		end

	filter "system:windows"
		links
		{
			-- Needed for Networking
			"ws2_32",
			"wsock32"
		}

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
				"{COPYDIR} %{IncludeDir.DISCORDGAMESDK}/../lib/x86_64/discord_game_sdk.dll %{cfg.targetdir}"
			}

			defines "USE_DISCORD_GAME_SDK"
		end

		-- Nsight Aftermath stuff
		if os.isfile("../../Dependencies/Nsight-Aftermath/lib/x64/GFSDK_Aftermath_Lib.x64.dll") and
		   os.isfile("../../Dependencies/Nsight-Aftermath/lib/x64/GFSDK_Aftermath_Lib.x64.lib") and
		   os.isfile("../../Dependencies/Nsight-Aftermath/lib/x64/llvm_7_0_1.dll") and
		   os.isdir("../../Dependencies/Nsight-Aftermath/include") and
		   os.isfile("../../Dependencies/Nsight-Aftermath/include/GFSDK_Aftermath.h") then
			externalincludedirs
			{
				"%{IncludeDir.NSIGHTAFTERMATH}"
			}

			postbuildcommands
			{
				"{COPYDIR} %{IncludeDir.NSIGHTAFTERMATH}/../lib/x64/GFSDK_Aftermath_Lib.x64.dll %{cfg.targetdir}"
			}

			defines "NSIGHT_AFTERMATH_AVAILABLE"
		end

		-- Steamworks SDK stuff
		if os.isfile("../../Dependencies/SteamworksSDK/sdk/redistributable_bin/win64/steam_api64.dll") and
		   os.isfile("../../Dependencies/SteamworksSDK/sdk/redistributable_bin/win64/steam_api64.lib") and
		   os.isdir("../../Dependencies/SteamworksSDK/sdk/public/steam") then

			links "%{IncludeDir.STEAMWORKSSDK}/../../redistributable_bin/win64/steam_api64.lib"

			postbuildcommands
			{
				"{COPYDIR} %{IncludeDir.STEAMWORKSSDK}/../../redistributable_bin/win64/steam_api64.dll %{cfg.targetdir}"
			}

			files
			{
				"%{IncludeDir.STEAMWORKSSDK}/**.h"
			}

			defines "USE_STEAMWORKS_SDK"
		end

		-- NVIDIA Reflex SDK stuff
		if os.isfile("../../Dependencies/NVIDIA-Reflex/Nvidia_Reflex_SDK_1.6/1.6/Reflex_Vulkan/Reflex_Vulkan/inc/NvLowLatencyVk.h") and
		   os.isfile("../../Dependencies/NVIDIA-Reflex/Nvidia_Reflex_SDK_1.6/1.6/Reflex_Vulkan/Reflex_Vulkan/lib/NvLowLatencyVk.lib") and
		   os.isfile("../../Dependencies/NVIDIA-Reflex/Nvidia_Reflex_SDK_1.6/1.6/Reflex_Vulkan/Reflex_Vulkan/lib/NvLowLatencyVk.dll") then
			links "%{IncludeDir.NVIDIAREFLEX}/../lib/NvLowLatencyVk.lib"

			postbuildcommands "{COPYDIR} %{IncludeDir.NVIDIAREFLEX}/../lib/NvLowLatencyVk.dll %{cfg.targetdir}"

			externalincludedirs "%{IncludeDir.NVIDIAREFLEX}"

			defines "NVIDIA_REFLEX_AVAILABLE"
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
