require "generatecontrollermappings"
require "generatedocs"
require "generatevulkanloader"
require "ecc"
require "vscode"

workspace "TRAP"
	architecture "x86_64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"RelWithDebInfo",
		"Profiling"
	}

	filter "system:linux"
		configurations
		{
			"ASan",
			"UBSan",
			"LSan",
			-- "TSan" -- Not working, crashes in thread from libnvidia-glcore.so
		}

	flags "MultiProcessorCompile"

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

include "TRAP"

group "Dependencies"
	include "Dependencies/ImGui.lua"
	include "Dependencies/ImGuizmo.lua"
	include "Dependencies/YAMLCpp.lua"
	include "Dependencies/ModernDialogs.lua"
	include "Dependencies/Box2D.lua"
	include "Dependencies/Tracy.lua"
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