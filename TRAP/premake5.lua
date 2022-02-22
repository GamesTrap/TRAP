project "TRAP"
	location "."
	kind "StaticLib"
	language "C++"
	staticruntime "on"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"
	warnings "Extra"

	targetdir ("../bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

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
		"src/Network/Sockets/Platform/SocketImplWinAPI.cpp"
	}

	includedirs
	{
		"src"
	}

	sysincludedirs
	{
		"../%{IncludeDir.IMGUI}",
		"../%{IncludeDir.GLSLANG}",
		"../%{IncludeDir.SPIRV}",
		"../%{IncludeDir.SPIRVCROSS}",
		"../%{IncludeDir.VULKAN}/include/",
		"../%{IncludeDir.ENTT}",
		"../%{IncludeDir.YAMLCPP}",
		"../%{IncludeDir.MODERNDIALOGS}",
		"../%{IncludeDir.VMA}"
	}

	links
	{
		"ImGui",
		"YAMLCpp",
		"ModernDialogs",
		"GLSLang",
		"SPIRV",
		"SPIRV-Cross-Core",
		"SPIRV-Cross-GLSL",
		"SPIRV-Cross-HLSL"
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

		if os.isfile("../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.dll.lib") and
		   os.isfile("../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.dll") and
		   os.isdir("../Dependencies/DiscordGameSDK/cpp") and
		   os.isfile("../Dependencies/DiscordGameSDK/cpp/discord.h") then

			links
			{
				"../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.dll.lib"
			}

			sysincludedirs
			{
				"../%{IncludeDir.DISCORDGAMESDK}"
			}

			files
			{
				"../%{IncludeDir.DISCORDGAMESDK}/**.h",
				"../%{IncludeDir.DISCORDGAMESDK}/**.cpp"
			}

			dofileopt("../Dependencies/DiscordGameSDK/Compatibility")

			defines "USE_DISCORD_GAME_SDK"
		end

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

		if os.isfile("../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.so") and
		   os.isdir("../Dependencies/DiscordGameSDK/cpp") and
		   os.isfile("../Dependencies/DiscordGameSDK/cpp/discord.h") then

			linkoptions{ "-Wl,/usr/local/lib/discord_game_sdk.so"}

			sysincludedirs
			{
				"../%{IncludeDir.DISCORDGAMESDK}"
			}

			files
			{
				"../%{IncludeDir.DISCORDGAMESDK}/**.h",
				"../%{IncludeDir.DISCORDGAMESDK}/**.cpp"
			}

			prelinkcommands
			{
				-- Copy the discord_game_sdk.so file to /usr/local/lib/discord_game_sdk.so if exists and
				-- not already in /usr/local/lib
				"if [ ! -f '/usr/local/lib/discord_game_sdk.so' ]; then " ..
				"echo 'Copying discord_game_sdk.so to /usr/local/lib/discord_game_sdk.so' && " ..
				"sudo cp '../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.so' '/usr/local/lib/discord_game_sdk.so'; " ..
				"fi"
			}

			dofileopt("../Dependencies/DiscordGameSDK/Compatibility")

			defines "USE_DISCORD_GAME_SDK"
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

<<<<<<< HEAD
project "TRAP-Server"
=======
project "TRAP-Headless"
>>>>>>> ebe28fefb8e8bf89bf60c4f394eb7ef2d139c305
	location "."
	kind "StaticLib"
	language "C++"
	staticruntime "on"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"
	warnings "Extra"

	targetdir ("../bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

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
<<<<<<< HEAD
		"src/Network/Sockets/Platform/SocketImplWinAPI.cpp"

		--TODO More to come
=======
		"src/Network/Sockets/Platform/SocketImplWinAPI.cpp",

		--TODO More to come
		-- "src/Events/ControllerEvent.h",
		-- "src/Events/ControllerEvent.cpp",
		-- "src/Events/HotReloadEvent.h",
		-- "src/Events/HotReloadEvent.cpp",
		-- "src/Events/MonitorEvent.h",
		-- "src/Events/MonitorEvent.cpp",
		-- "src/Events/WindowEvent.h",
		-- "src/Events/WindowEvent.cpp",
		-- "src/Events/KeyEvent.h",
		-- "src/Events/KeyEvent.cpp",
		-- "src/Events/MouseEvent.h",
		-- "src/Events/MouseEvent.cpp",
		-- "src/Graphics/**.h",
		-- "src/Graphics/**.cpp",
		-- "src/Input/ControllerMappings.h",
		-- "src/Input/ControllerMappings.h.in",
		-- "src/Layers/ImGui/**.h",
		-- "src/Layers/ImGui/**.cpp",
		-- "src/Scene/**.h",
		-- "src/Scene/**.cpp",
		-- "src/Utils/Discord/**.h",
		-- "src/Utils/Discord/**.cpp",
		-- "src/Embed.h",
>>>>>>> ebe28fefb8e8bf89bf60c4f394eb7ef2d139c305
	}

	includedirs
	{
		"src"
	}

	sysincludedirs
	{
		"../%{IncludeDir.IMGUI}",
		"../%{IncludeDir.GLSLANG}",
		"../%{IncludeDir.SPIRV}",
		"../%{IncludeDir.SPIRVCROSS}",
		"../%{IncludeDir.VULKAN}/include/",
		"../%{IncludeDir.ENTT}",
		"../%{IncludeDir.YAMLCPP}",
		"../%{IncludeDir.MODERNDIALOGS}",
		"../%{IncludeDir.VMA}"
	}

	links
	{
		"ImGui",
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
		"TRAP_HEADLESS_MODE"
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