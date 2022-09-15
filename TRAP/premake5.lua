project "TRAP"
	location "."
	kind "StaticLib"
	language "C++"
	staticruntime "off"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"
	warnings "Extra"

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

	--Exclude all folders in Platform, since not all platforms need all of these
	removefiles
	{
		"src/Utils/Win.h",
		"src/Utils/Linux.h",
		"src/Log/ANSILog.cpp",
		"src/Log/WindowsLog.cpp",
		"src/Input/WindowsInput.cpp",
		"src/Input/LinuxInput.cpp",
		"src/Window/WindowingAPIWin32.cpp",
		"src/Window/WindowingAPILinuxX11.cpp",
		"src/Network/Sockets/Platform/SocketImplLinux.h",
		"src/Network/Sockets/Platform/SocketImplLinux.cpp",
		"src/Network/Sockets/Platform/SocketImplWinAPI.h",
		"src/Network/Sockets/Platform/SocketImplWinAPI.cpp"
	}

	includedirs	"src"

	externalincludedirs
	{
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.IMGUIZMO}",
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}",
		"%{IncludeDir.SPIRVCROSS}",
		"%{IncludeDir.VULKAN}/include/",
		"%{IncludeDir.ENTT}",
		"%{IncludeDir.YAMLCPP}",
		"%{IncludeDir.MODERNDIALOGS}",
		"%{IncludeDir.VMA}",
	}

	links
	{
		"ImGui",
		"ImGuizmo",
		"YAMLCpp",
		"ModernDialogs",
		"GLSLang",
		"SPIRV",
		"SPIRV-Cross-Core",
		"SPIRV-Cross-GLSL",
		"SPIRV-Cross-HLSL"
	}

	defines "YAML_CPP_STATIC_DEFINE"

	filter "system:windows"
		files
		{
			"src/Utils/Win.h",
			"src/Log/WindowsLog.cpp",
			"src/Input/WindowsInput.cpp",
			"src/Window/WindowingAPIWin32.cpp",
			"src/Network/Sockets/Platform/SocketImplWinAPI.h",
			"src/Network/Sockets/Platform/SocketImplWinAPI.cpp"
		}

		-- Discord Game SDK stuff
		if os.isfile("../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.dll.lib") and
		   os.isfile("../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.dll") and
		   os.isdir("../Dependencies/DiscordGameSDK/cpp") and
		   os.isfile("../Dependencies/DiscordGameSDK/cpp/discord.h") then
			externalincludedirs "%{IncludeDir.DISCORDGAMESDK}"

			files
			{
				"%{IncludeDir.DISCORDGAMESDK}/**.h",
				"%{IncludeDir.DISCORDGAMESDK}/**.cpp"
			}

			dofileopt("../Dependencies/DiscordGameSDK/Compatibility")

			defines "USE_DISCORD_GAME_SDK"
		end

		-- Nsight Aftermath stuff
		if os.isfile("../Dependencies/Nsight-Aftermath/lib/x64/GFSDK_Aftermath_Lib.x64.dll") and
		   os.isfile("../Dependencies/Nsight-Aftermath/lib/x64/GFSDK_Aftermath_Lib.x64.lib") and
		   os.isfile("../Dependencies/Nsight-Aftermath/lib/x64/llvm_7_0_1.dll") and
		   os.isdir("../Dependencies/Nsight-Aftermath/include") and
		   os.isfile("../Dependencies/Nsight-Aftermath/include/GFSDK_Aftermath.h") then
			externalincludedirs "%{IncludeDir.NSIGHTAFTERMATH}"

			defines "NSIGHT_AFTERMATH_AVAILABLE"
		end

		-- Steamworks SDK stuff
		if os.isfile("../Dependencies/SteamworksSDK/sdk/redistributable_bin/win64/steam_api64.dll") and
		   os.isfile("../Dependencies/SteamworksSDK/sdk/redistributable_bin/win64/steam_api64.lib") and
		   os.isdir("../Dependencies/SteamworksSDK/sdk/public/steam") then
			externalincludedirs "%{IncludeDir.STEAMWORKSSDK}"

			files "%{IncludeDir.DISCORDGAMESDK}/**.h"

			defines "USE_STEAMWORKS_SDK"
		end

		-- NVIDIA Reflex SDK stuff
		if os.isfile("../Dependencies/NVIDIA-Reflex/Nvidia_Reflex_SDK_1.6/1.6/Reflex_Vulkan/Reflex_Vulkan/inc/NvLowLatencyVk.h") and
		   os.isfile("../Dependencies/NVIDIA-Reflex/Nvidia_Reflex_SDK_1.6/1.6/Reflex_Vulkan/Reflex_Vulkan/lib/NvLowLatencyVk.lib") and
		   os.isfile("../Dependencies/NVIDIA-Reflex/Nvidia_Reflex_SDK_1.6/1.6/Reflex_Vulkan/Reflex_Vulkan/lib/NvLowLatencyVk.dll") and
		   os.isfile("../Dependencies/NVIDIA-Reflex/Nvidia_Reflex_SDK_1.6/1.6/Reflex_Stats/reflexstats.h") then
			externalincludedirs
			{
				"%{IncludeDir.NVIDIAREFLEX}",
				"%{IncludeDir.NVIDIAREFLEXSTATS}"
			}

			defines "NVIDIA_REFLEX_AVAILABLE"
		end

	filter "system:linux"
		-- Add Linux-specific files
        files
        {
			"src/Utils/Linux.h",
			"src/Log/ANSILog.cpp",
			"src/Input/LinuxInput.cpp",
			"src/Window/WindowingAPILinuxX11.cpp",
			"src/Network/Sockets/Platform/SocketImplLinux.h",
			"src/Network/Sockets/Platform/SocketImplLinux.cpp"
		}

		-- Discord Game SDK stuff
		if (os.isfile("../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.so") or
		    os.isfile("../Dependencies/DiscordGameSDK/lib/x86_64/libdiscord_game_sdk.so")) and
			os.isdir("../Dependencies/DiscordGameSDK/cpp") and
			os.isfile("../Dependencies/DiscordGameSDK/cpp/discord.h") then

			externalincludedirs "%{IncludeDir.DISCORDGAMESDK}"

			files
			{
				"%{IncludeDir.DISCORDGAMESDK}/**.h",
				"%{IncludeDir.DISCORDGAMESDK}/**.cpp"
			}

			dofileopt("../Dependencies/DiscordGameSDK/Compatibility")

			defines "USE_DISCORD_GAME_SDK"
		end

		-- Nsight Aftermath stuff
		if os.isfile("../Dependencies/Nsight-Aftermath/lib/x64/libGFSDK_Aftermath_Lib.x64.so") and
		   os.isdir("../Dependencies/Nsight-Aftermath/include") and
		   os.isfile("../Dependencies/Nsight-Aftermath/include/GFSDK_Aftermath.h") then

			externalincludedirs "%{IncludeDir.NSIGHTAFTERMATH}"

			defines "NSIGHT_AFTERMATH_AVAILABLE"
		end

		-- Steamworks SDK stuff
		if os.isfile("../Dependencies/SteamworksSDK/sdk/redistributable_bin/linux64/libsteam_api.so") and
		   os.isdir("../Dependencies/SteamworksSDK/sdk/public/steam") then
			externalincludedirs "%{IncludeDir.STEAMWORKSSDK}"

			files "%{IncludeDir.DISCORDGAMESDK}/**.h"

			defines "USE_STEAMWORKS_SDK"
		end

	filter "configurations:Debug"
		defines "TRAP_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "TRAP_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:RelWithDebInfo"
		defines "TRAP_RELWITHDEBINFO"
		runtime "Release"
		optimize "On"
		symbols "On"

project "TRAP-Headless"
	location "."
	kind "StaticLib"
	language "C++"
	staticruntime "off"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"
	warnings "Extra"

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

	--Exclude all folders in Platform, since not all platforms need all of these
	removefiles
	{
		"src/Utils/Win.h",
		"src/Log/ANSILog.cpp",
		"src/Log/WindowsLog.cpp",
		"src/Input/WindowsInput.cpp",
		"src/Input/LinuxInput.cpp",
		"src/Window/WindowingAPIWin32.cpp",
		"src/Window/WindowingAPILinuxX11.cpp",
		"src/Network/Sockets/Platform/SocketImplLinux.h",
		"src/Network/Sockets/Platform/SocketImplLinux.cpp",
		"src/Network/Sockets/Platform/SocketImplWinAPI.h",
		"src/Network/Sockets/Platform/SocketImplWinAPI.cpp",
	}

	includedirs "src"

	externalincludedirs
	{
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.IMGUIZMO}",
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}",
		"%{IncludeDir.SPIRVCROSS}",
		"%{IncludeDir.VULKAN}/include/",
		"%{IncludeDir.ENTT}",
		"%{IncludeDir.YAMLCPP}",
		"%{IncludeDir.MODERNDIALOGS}",
		"%{IncludeDir.VMA}"
	}

	links
	{
		"ImGui",
		"ImGuizmo",
		"YAMLCpp",
		"ModernDialogs",
		"GLSLang",
		"SPIRV",
		"SPIRV-Cross-Core",
		"SPIRV-Cross-GLSL",
		"SPIRV-Cross-HLSL"
	}

	defines
	{
		"TRAP_HEADLESS_MODE",
		"YAML_CPP_STATIC_DEFINE"
	}

	filter "system:windows"
		files
		{
			"src/Utils/Win.h",
			"src/Log/WindowsLog.cpp",
			"src/Input/WindowsInput.cpp",
			"src/Window/WindowingAPIWin32.cpp",
			"src/Network/Sockets/Platform/SocketImplWinAPI.h",
			"src/Network/Sockets/Platform/SocketImplWinAPI.cpp"
		}

		links
		{
			"Imm32",
			"ws2_32"
		}

	filter "system:linux"
		-- Add Linux-specific files
		files
		{
			"src/Log/ANSILog.cpp",
			"src/Input/LinuxInput.cpp",
			"src/Window/WindowingAPILinuxX11.cpp",
			"src/Network/Sockets/Platform/SocketImplLinux.h",
			"src/Network/Sockets/Platform/SocketImplLinux.cpp"
		}

	filter "configurations:Debug"
		defines "TRAP_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "TRAP_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:RelWithDebInfo"
		defines "TRAP_RELWITHDEBINFO"
		runtime "Release"
		optimize "On"
		symbols "On"
