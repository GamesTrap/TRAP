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
IncludeDir["IMGUI"] = "Dependencies/ImGui"
IncludeDir["VULKAN"] = os.getenv("VULKAN_SDK")
IncludeDir["GLSLANG"] = "Dependencies/GLSLang"
IncludeDir["SPIRV"] = "Dependencies/GLSLang/SPIRV"
IncludeDir["SPIRVCROSS"] = "Dependencies/SPIRV-Cross"
IncludeDir["ENTT"] = "Dependencies/EnTT/src/entt"
IncludeDir["YAMLCPP"] = "Dependencies/YAMLCpp/include"
IncludeDir["MODERNDIALOGS"] = "Dependencies/ModernDialogs/ModernDialogs"
IncludeDir["DISCORDGAMESDK"] = "Dependencies/DiscordGameSDK/cpp"
IncludeDir["VMA"] = "Dependencies/VulkanMemoryAllocator/include"

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