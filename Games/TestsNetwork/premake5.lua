project "TestsNetwork"
	location "."
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"
	warnings "Extra"
	architecture "x86_64"

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
		"%{IncludeDir.TRACY}"
	}

	links "TRAP-Headless"

	defines "TRAP_HEADLESS_MODE"

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

		externalincludedirs
		{
			"%{IncludeDir.WAYLAND}"
		}

		-- Discord Game SDK stuff
		if (os.isfile("../../Dependencies/DiscordGameSDK/lib/x86_64/discord_game_sdk.so") or
		    os.isfile("../../Dependencies/DiscordGameSDK/lib/x86_64/libdiscord_game_sdk.so")) and
		   os.isdir("../../Dependencies/DiscordGameSDK/cpp") and
		   os.isfile("../../Dependencies/DiscordGameSDK/cpp/discord.h") then

			links "discord_game_sdk"
			libdirs "%{IncludeDir.DISCORDGAMESDK}/../lib/x86_64"

			postbuildcommands "{COPYFILE} %{IncludeDir.DISCORDGAMESDK}/../lib/x86_64/libdiscord_game_sdk.so %{cfg.targetdir}"

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
			externalincludedirs "%{IncludeDir.NSIGHTAFTERMATH}"

			postbuildcommands "{COPYFILE} %{IncludeDir.NSIGHTAFTERMATH}/../lib/x64/libGFSDK_Aftermath_Lib.x64.so %{cfg.targetdir}"

			defines "NSIGHT_AFTERMATH_AVAILABLE"
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

			links "%{IncludeDir.DISCORDGAMESDK}/../lib/x86_64/discord_game_sdk.dll.lib"

			files
			{
				"%{IncludeDir.DISCORDGAMESDK}/**.h",
				"%{IncludeDir.DISCORDGAMESDK}/**.cpp"
			}

			postbuildcommands "{COPYDIR} %{IncludeDir.DISCORDGAMESDK}/../lib/x86_64/discord_game_sdk.dll %{cfg.targetdir}"

			defines "USE_DISCORD_GAME_SDK"
		end

		-- Nsight Aftermath stuff
		if os.isfile("../../Dependencies/Nsight-Aftermath/lib/x64/GFSDK_Aftermath_Lib.x64.dll") and
		   os.isfile("../../Dependencies/Nsight-Aftermath/lib/x64/GFSDK_Aftermath_Lib.x64.lib") and
		   os.isfile("../../Dependencies/Nsight-Aftermath/lib/x64/llvm_7_0_1.dll") and
		   os.isdir("../../Dependencies/Nsight-Aftermath/include") and
		   os.isfile("../../Dependencies/Nsight-Aftermath/include/GFSDK_Aftermath.h") then
			externalincludedirs "%{IncludeDir.NSIGHTAFTERMATH}"

			postbuildcommands "{COPYDIR} %{IncludeDir.NSIGHTAFTERMATH}/../lib/x64/GFSDK_Aftermath_Lib.x64.dll %{cfg.targetdir}"

			defines "NSIGHT_AFTERMATH_AVAILABLE"
		end

		-- NVIDIA Reflex SDK stuff
		if os.isfile("../../Dependencies/NVIDIA-Reflex/Nvidia_Reflex_SDK_1.6/1.6/Reflex_Vulkan/Reflex_Vulkan/inc/NvLowLatencyVk.h") and
		   os.isfile("../../Dependencies/NVIDIA-Reflex/Nvidia_Reflex_SDK_1.6/1.6/Reflex_Vulkan/Reflex_Vulkan/lib/NvLowLatencyVk.lib") and
		   os.isfile("../../Dependencies/NVIDIA-Reflex/Nvidia_Reflex_SDK_1.6/1.6/Reflex_Vulkan/Reflex_Vulkan/lib/NvLowLatencyVk.dll") and
		   os.isfile("../../Dependencies/NVIDIA-Reflex/Nvidia_Reflex_SDK_1.6/1.6/Reflex_Stats/reflexstats.h") then
			links "%{IncludeDir.NVIDIAREFLEX}/../lib/NvLowLatencyVk.lib"

			postbuildcommands "{COPYDIR} %{IncludeDir.NVIDIAREFLEX}/../lib/NvLowLatencyVk.dll %{cfg.targetdir}"

			externalincludedirs
			{
				"%{IncludeDir.NVIDIAREFLEX}",
				"%{IncludeDir.NVIDIAREFLEXSTATS}"
			}

			defines "NVIDIA_REFLEX_AVAILABLE"
		end

	filter { "action:gmake*", "toolset:gcc" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}

	filter "configurations:Debug"
		defines "TRAP_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "TRAP_RELEASE"
		runtime "Release"
		optimize "Full"
		entrypoint "mainCRTStartup"
		kind "WindowedApp"

	filter "configurations:RelWithDebInfo"
		defines "TRAP_RELWITHDEBINFO"
		runtime "Release"
		optimize "Debug"
		symbols "On"

	filter "configurations:Profiling"
		editandcontinue "Off"
		defines
		{
			"TRAP_RELEASE",
			"TRACY_ENABLE"
		}
		runtime "Release"
		optimize "Full"
		symbols "On"

	filter "configurations:ASAN"
		defines
		{
			"TRAP_RELWITHDEBINFO",
			"TRAP_ASAN"
		}
		runtime "Release"
		optimize "Debug"
		symbols "On"
		buildoptions
		{
			"-fsanitize=address",
			"-fno-omit-frame-pointer",
			"-g"
		}
		linkoptions
		{
			"-fsanitize=address",
			"-static-libasan"
		}

	filter "configurations:UBSAN"
		defines
		{
			"TRAP_RELWITHDEBINFO",
			"TRAP_UBSAN"
		}
		runtime "Release"
		optimize "Debug"
		symbols "On"
		buildoptions
		{
			"-fsanitize=undefined",
			"-fno-omit-frame-pointer",
			"-g"
		}
		linkoptions
		{
			"-fsanitize=undefined",
			"-static-libubsan"
		}

	filter "configurations:LSAN"
		defines
		{
			"TRAP_RELWITHDEBINFO",
			"TRAP_LSAN"
		}
		runtime "Release"
		optimize "Debug"
		symbols "On"
		buildoptions
		{
			"-fsanitize=leak",
			"-fno-omit-frame-pointer",
			"-g"
		}
		linkoptions
		{
			"-fsanitize=leak"
		}

	filter "configurations:TSAN"
		staticruntime "off"
		defines
		{
			"TRAP_RELWITHDEBINFO",
			"TRAP_TSAN"
		}
		runtime "Release"
		optimize "Debug"
		symbols "On"
		buildoptions
		{
			"-fsanitize=thread",
			"-fno-omit-frame-pointer",
			"-g"
		}
		linkoptions
		{
			"-fsanitize=thread",
			"-static-libtsan"
		}