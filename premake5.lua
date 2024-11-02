require "ecc"
require "vscode"
require "ninja"

workspace "TRAP"
	startproject "Sandbox"
	architecture "x86_64"
	cppdialect "C++20"
	cdialect "C17"
	staticruntime "off"
	systemversion "latest"
	vectorextensions "AVX2"
	warnings "Extra"

	flags
	{
		"MultiProcessorCompile",
		"FatalCompileWarnings"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	runpathdirs
	{
		".",
		"%{cfg.targetdir}",
		"$ORIGIN"
	}

	defines
	{
		"YAML_CPP_STATIC_DEFINE",
		"IMGUI_DISABLE_OBSOLETE_FUNCTIONS",
		"IMGUI_DISABLE_OBSOLETE_KEYIO",
        "FMT_OS",
        "ENABLE_OPT=0",
        "ENABLE_HLSL=0",
	}

	configurations
	{
		"Debug",
		"Release",
		"RelWithDebInfo",
		"Profiling"
	}

	filter {"language:C or C++", "toolset:gcc or clang"}
		buildoptions "-Wno-error=deprecated-declarations"

	filter "system:linux"
		configurations
		{
			"ASan_LSan_UBSan",
			"TSan"
		}

	filter {"language:C or C++", "configurations:Release or configurations:Profiling" }
		flags "LinkTimeOptimization"

	filter {"language:C or C++", "configurations:Debug" }
		runtime "Debug"
		symbols "On"
		defines "TRAP_DEBUG"

	filter {"language:C or C++", "configurations:Release"}
		runtime "Release"
		optimize "Full"
		defines "TRAP_RELEASE"
		entrypoint "mainCRTStartup"
		kind "WindowedApp"

	filter {"language:C or C++", "configurations:RelWithDebInfo"}
		runtime "Release"
		optimize "Debug"
		symbols "On"
		defines "TRAP_RELWITHDEBINFO"

	filter {"language:C or C++", "configurations:Profiling"}
		defines
		{
			"TRAP_RELEASE",
			"TRACY_ENABLE"
		}
		editandcontinue "Off"
		runtime "Release"
		optimize "Full"
		symbols "On"

	filter {"language:C or C++", "configurations:ASan_LSan_UBSan"}
		defines
		{
			"TRAP_RELWITHDEBINFO",
			"TRAP_ASAN",
			"TRAP_LSAN",
			"TRAP_UBSAN"
		}
		runtime "Release"
		optimize "Debug"
		symbols "On"
		buildoptions
		{
			"-fsanitize=address,leak,undefined",
			"-fno-omit-frame-pointer",
			"-g"
		}
		linkoptions
		{
			"-fsanitize=address,leak,undefined"
		}

	filter {"language:C or C++", "configurations:TSAN"}
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
			"-fsanitize=thread"
		}

	filter { "language:C or C++", "toolset:gcc or clang"}
		local result, errorCode = os.outputof("mold -v")
		local moldInstalled = false
		if errorCode == 0 and string.len(result) > 0 then
			moldInstalled = true
		end

		if moldInstalled then
			linkoptions "-fuse-ld=mold"
		end

	filter { "toolset:gcc"}
		buildoptions "-fdiagnostics-color=always -Winvalid-pch"
	filter { "toolset:gcc", "language:C++"}
		buildoptions "-Wno-interference-size"
	filter { "toolset:clang"}
		buildoptions "-fcolor-diagnostics"

	filter { "language:C or C++", "toolset:msc-v143"}
		buildoptions "/utf-8"

--Include directories relative to root folder(solution folder)
IncludeDir = {}
IncludeDir["IMGUI"] = "%{wks.location}/Dependencies/ImGui"
IncludeDir["IMGUIZMO"] = "%{wks.location}/Dependencies/ImGuizmo"
IncludeDir["VULKAN"] = "%{wks.location}/Dependencies/Vulkan-Headers"
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
IncludeDir["BOX2D"] = "%{wks.location}/Dependencies/Box2D/include"
IncludeDir["TRACY"] = "%{wks.location}/Dependencies/Tracy/public"
IncludeDir["FMT"] = "%{wks.location}/Dependencies/fmt/include"
IncludeDir["CATCH2"] = "%{wks.location}/Dependencies/Catch2/src"
IncludeDir["GCEM"] = "%{wks.location}/Dependencies/GCEM/include"
IncludeDir["ZLIB"] = "%{wks.location}/Dependencies/zlib"
IncludeDir["LIBPNG"] = "%{wks.location}/Dependencies/libpng"
IncludeDir["QUITEOKIMAGE"] = "%{wks.location}/Dependencies/QuiteOKImage"

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
	include "Dependencies/SPIRV-Cross.lua"
	include "Dependencies/GLSLang.lua"
	include "Dependencies/Catch2.lua"
if os.isfile(path.join(_MAIN_SCRIPT_DIR, "Dependencies/DiscordGameSDK/cpp/discord.h")) then
	include "Dependencies/DiscordGameSDK.lua"
end
	include "Dependencies/zlib.lua"
	include "Dependencies/libpng.lua"
	include "Dependencies/QuiteOKImage.lua"

group "Games"
	include "Games/Sandbox"
	include "Games/Tests"
	include "Games/Headless-Tests"
	include "Games/Tests3D"
	include "Games/TestsNetwork"
	include "Games/TRAP-Editor"

group "Utility"
	include "Utility/ConvertToSPIRV"
