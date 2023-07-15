require "ecc"
require "vscode"

workspace "TRAP"
	startproject "Sandbox"
	architecture "x86_64"
	cppdialect "C++20"
	staticruntime "off"
	systemversion "latest"
	vectorextensions "AVX2"

	configurations
	{
		"Debug",
		"Release",
		"RelWithDebInfo",
		"Profiling"
	}

	flags "MultiProcessorCompile"
	-- flags "LinkTimeOptimization"

	filter {"language:C or C++", "configurations:Debug" }
		runtime "Debug"
		symbols "On"

	filter {"language:C or C++", "configurations:Release"}
		runtime "Release"
		optimize "Full"

	filter {"language:C or C++", "configurations:RelWithDebInfo"}
		runtime "Release"
		optimize "Debug"
		symbols "On"

	filter {"language:C or C++", "configurations:Profiling"}
		defines "TRACY_ENABLE"
		editandcontinue "Off"
		runtime "Release"
		optimize "Full"
		symbols "On"

	filter "system:linux"
		configurations
		{
			"ASan",
			"UBSan",
			"LSan",
			"TSan" -- Works using RADV, crashes with libnvidia-glcore.so
		}

	filter {"language:C or C++", "configurations:ASAN"}
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

	filter {"language:C or C++", "configurations:UBSAN"}
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

	filter {"language:C or C++", "configurations:LSAN"}
		runtime "Release"
		optimize "Debug"
		symbols "On"
		buildoptions
		{
			"-fsanitize=leak",
			"-fno-omit-frame-pointer",
			"-g"
		}
		linkoptions "-fsanitize=leak"

	filter {"language:C or C++", "configurations:TSAN"}
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

	filter { "toolset:gcc" or "toolset:clang"}
		local result, errorCode = os.outputof("mold -v")
		local moldInstalled = false
		if errorCode == 0 and string.len(result) > 0 then
			moldInstalled = true
		end

		if moldInstalled then
			linkoptions "-fuse-ld=mold"
		end

	-- filter { "toolset:gcc" }
	-- 	if moldInstalled then
	-- 		linkoptions "-flto=auto"
	-- 		buildoptions "-flto=auto"
	-- 	end

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--Include directories relative to root folder(solution folder)
IncludeDir = {}
IncludeDir["IMGUI"] = "%{wks.location}/Dependencies/ImGui"
IncludeDir["IMGUIZMO"] = "%{wks.location}/Dependencies/ImGuizmo"
IncludeDir["VULKAN"] = os.getenv("VULKAN_SDK")
IncludeDir["GLSLANG"] = "%{wks.location}/Dependencies/GLSLang"
IncludeDir["SPIRV"] = "%{wks.location}/Dependencies/GLSLang/SPIRV"
IncludeDir["SPIRVCROSS"] = "%{wks.location}/Dependencies/SPIRV-Cross"
IncludeDir["ENTT"] = "%{wks.location}/Dependencies/EnTT/src/entt"
IncludeDir["YAMLCPP"] = "%{wks.location}/Dependencies/YAMLCpp/include"
IncludeDir["MODERNDIALOGS"] = "%{wks.location}/Dependencies/ModernDialogs/ModernDialogs"
IncludeDir["DISCORDGAMESDK"] = "%{wks.location}/Dependencies/DiscordGameSDK/cpp"
IncludeDir["NSIGHTAFTERMATH"] = "%{wks.location}/Dependencies/Nsight-Aftermath/include"
IncludeDir["VMA"] = "%{wks.location}/Dependencies/VulkanMemoryAllocator/include"
IncludeDir["WAYLAND"] = "%{wks.location}/Dependencies/Wayland"
IncludeDir["STEAMWORKSSDK"] = "%{wks.location}/Dependencies/SteamworksSDK/sdk/public/steam"
local NVReflexFiles = os.matchfiles(os.getcwd() .. "/Dependencies/NVIDIA-Reflex/**NvLowLatencyVk.h")
if next(NVReflexFiles) ~= nil then
	IncludeDir["NVIDIAREFLEX"] = path.getdirectory(NVReflexFiles[1])
end
local NVReflexStatsFiles = os.matchfiles(os.getcwd() .. "/Dependencies/NVIDIA-Reflex/**pclstats.h")
if next(NVReflexStatsFiles) ~= nil then
	IncludeDir["NVIDIAREFLEXSTATS"] = path.getdirectory(NVReflexStatsFiles[1])
end
IncludeDir["BOX2D"] = "%{wks.location}/Dependencies/Box2D/include"
IncludeDir["TRACY"] = "%{wks.location}/Dependencies/Tracy/public"
IncludeDir["FMT"] = "%{wks.location}/Dependencies/fmt/include"
IncludeDir["GOOGLETEST"] = "%{wks.location}/Dependencies/GoogleTest/googletest/include"

include "TRAP"
include "UnitTests"

group "Dependencies"
	include "Dependencies/ImGui.lua"
	include "Dependencies/ImGuizmo.lua"
	include "Dependencies/YAMLCpp.lua"
	include "Dependencies/ModernDialogs.lua"
	include "Dependencies/Box2D.lua"
	include "Dependencies/Tracy.lua"
	include "Dependencies/fmt.lua"
	group "Dependencies/GLSLang"
		include "Dependencies/GLSLang.lua"
	group "Dependencies/SPIRV-Cross"
		include "Dependencies/SPIRV-Cross.lua"

group "Games"
	include "Games/Sandbox"
	include "Games/Tests"
	include "Games/Headless-Tests"
	include "Games/Tests3D"
	include "Games/TestsNetwork"
	include "Games/TRAP-Editor"

group "Utility"
	include "Utility/ConvertToSPIRV"
