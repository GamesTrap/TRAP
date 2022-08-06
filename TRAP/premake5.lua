function GenerateWayland()
	local files = {};
	files["https://gitlab.freedesktop.org/wayland/wayland/-/raw/main/protocol/wayland.xml"] = "../Dependencies/Wayland/wayland-client-protocol"
	files["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/stable/xdg-shell/xdg-shell.xml"] = "../Dependencies/Wayland/wayland-xdg-shell-client-protocol"
	files["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml"] = "../Dependencies/Wayland/wayland-xdg-decoration-client-protocol"
	files["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/stable/viewporter/viewporter.xml"] = "../Dependencies/Wayland/wayland-viewporter-client-protocol"
	files["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/unstable/relative-pointer/relative-pointer-unstable-v1.xml"] = "../Dependencies/Wayland/wayland-relative-pointer-unstable-v1-client-protocol"
	files["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml"] = "../Dependencies/Wayland/wayland-pointer-constraints-unstable-v1-client-protocol"
	files["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/unstable/idle-inhibit/idle-inhibit-unstable-v1.xml"] = "../Dependencies/Wayland/wayland-idle-inhibit-unstable-v1-client-protocol"

	local result, errorCode = os.outputof("wayland-scanner --version")
	if errorCode ~= 0 then
		term.setTextColor(term.errorColor)
		print("Unable to call wayland-scanner from shell!")
		term.setTextColor(nil)
		return
	end

	if not os.isdir("../Dependencies/Wayland") then
		os.mkdir("../Dependencies/Wayland")
	end

	for protocolURL,outputFile in pairs(files) do
		if not os.isfile(outputFile .. ".h") or not os.isfile(outputFile .. "-code.h") then
			local result, responseCode = http.download(protocolURL, "temp.xml");

			if result == "OK" then
				local res, errorCode = os.outputof("wayland-scanner client-header \"temp.xml\" \"" .. outputFile .. ".h\"");
				if errorCode ~= 0 then
					term.setTextColor(term.errorColor)
					print("Failed to generate " .. outputFile .. ".h")
					term.setTextColor(nil)
				else
					term.setTextColor(term.infoColor)
					print("Successfully generated " .. outputFile .. ".h")
					term.setTextColor(nil)
				end
				local res, errorCode = os.outputof("wayland-scanner private-code \"temp.xml\" \"" .. outputFile .. "-code.h\"");
				if errorCode ~= 0 then
					term.setTextColor(term.errorColor)
					print("Failed to generate " .. outputFile .. "-code.h")
					term.setTextColor(nil)
				else
					term.setTextColor(term.infoColor)
					print("Successfully generated " .. outputFile .. "-code.h")
					term.setTextColor(nil)
				end

				os.remove("temp.xml");
			else
				term.setTextColor(term.errorColor)
				print("Failed to download " .. protocolURL)
				term.setTextColor(nil)
			end
		end
	end
end

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
		"src/Window/WindowingAPILinux.cpp",
		"src/Window/WindowingAPILinuxX11.cpp",
		"src/Window/WindowingAPILinuxWayland.cpp",
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
		"%{IncludeDir.IMGUI}",
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
			sysincludedirs
			{
				"%{IncludeDir.DISCORDGAMESDK}"
			}

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
			sysincludedirs
			{
				"%{IncludeDir.NSIGHTAFTERMATH}"
			}

			defines "NSIGHT_AFTERMATH_AVAILABLE"
		end

	filter "system:linux"
		-- Add Linux-specific files
        files
        {
			"src/Utils/Linux.h",
			"src/Log/ANSILog.cpp",
			"src/Input/LinuxInput.cpp",
			"src/Window/WindowingAPILinux.cpp",
			"src/Window/WindowingAPILinuxX11.cpp",
			"src/Window/WindowingAPILinuxWayland.cpp",
			"src/Network/Sockets/Platform/SocketImplLinux.h",
			"src/Network/Sockets/Platform/SocketImplLinux.cpp"
		}

		-- Discord Game SDK stuff
		if (os.isfile("../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.so") or
		    os.isfile("../Dependencies/DiscordGameSDK/lib/x86_64/libdiscord_game_sdk.so")) and
			os.isdir("../Dependencies/DiscordGameSDK/cpp") and
			os.isfile("../Dependencies/DiscordGameSDK/cpp/discord.h") then

			sysincludedirs
			{
				"%{IncludeDir.DISCORDGAMESDK}"
			}

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

			sysincludedirs
			{
				"%{IncludeDir.NSIGHTAFTERMATH}"
			}

			defines "NSIGHT_AFTERMATH_AVAILABLE"
		end

		GenerateWayland()
		sysincludedirs
		{
			"%{IncludeDir.WAYLAND}"
		}

		files
		{
			"%{IncludeDir.WAYLAND}/**.h"
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
		"src/Window/WindowingAPILinux.cpp",
		"src/Window/WindowingAPILinuxX11.cpp",
		"src/Window/WindowingAPILinuxWayland.cpp",
		"src/Network/Sockets/Platform/SocketImplLinux.h",
		"src/Network/Sockets/Platform/SocketImplLinux.cpp",
		"src/Network/Sockets/Platform/SocketImplWinAPI.h",
		"src/Network/Sockets/Platform/SocketImplWinAPI.cpp",
	}

	includedirs
	{
		"src"
	}

	sysincludedirs
	{
		"%{IncludeDir.IMGUI}",
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
			"src/Window/WindowingAPILinux.cpp",
			"src/Window/WindowingAPILinuxX11.cpp",
			"src/Window/WindowingAPILinuxWayland.cpp",
			"src/Network/Sockets/Platform/SocketImplLinux.h",
			"src/Network/Sockets/Platform/SocketImplLinux.cpp"
		}

		GenerateWayland()
		sysincludedirs
		{
			"%{IncludeDir.WAYLAND}"
		}

		files
		{
			"%{IncludeDir.WAYLAND}/**.h"
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
