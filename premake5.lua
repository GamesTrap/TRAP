require "generatecontrollermappings"
require "generatedocs"
require "generatevulkanloader"
require "ecc"

workspace "TRAP"
	architecture "x86_64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"RelWithDebInfo"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--Include directories relative to root folder(solution folder)
IncludeDir = {}
IncludeDir["IMGUI"] = "%{wks.location}/Dependencies/ImGui"
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

include "TRAP"

group "Dependencies"
	include "Dependencies/ImGui.lua"
	include "Dependencies/YAMLCpp.lua"
	include "Dependencies/ModernDialogs.lua"
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