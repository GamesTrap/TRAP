workspace "TRAP"
	architecture "x64"
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
IncludeDir["OGLCOMPILER"] = "Dependencies/GLSLang/OGLCompilersDLL"
IncludeDir["OSDEPENDENT"] = "Dependencies/GLSLang/glslang/OSDependent"
IncludeDir["HLSL"] = "Dependencies/GLSLang/hlsl"
IncludeDir["SPIRV"] = "Dependencies/GLSLang/SPIRV"
IncludeDir["STANDALONE"] = "Dependencies/GLSLang/StandAlone"
IncludeDir["SPIRVCROSS"] = "Dependencies/SPIRV-Cross"

group "Dependencies"
	include "Dependencies/ImGui"
	group "Dependencies/GLSLang"
		include "Dependencies/GLSLang/SPIRV"
		include "Dependencies/GLSLang/StandAlone"
		include "Dependencies/GLSLang/glslang"
	group "Dependencies/SPIRV-Cross"
		include "Dependencies/SPIRV-Cross/SPIRV-Cross-Core"
		include "Dependencies/SPIRV-Cross/SPIRV-Cross-GLSL"
		include "Dependencies/SPIRV-Cross/SPIRV-Cross-HLSL"
		include "Dependencies/SPIRV-Cross/SPIRV-Cross-Reflect"

group "Engine"
project "TRAP"
	location "TRAP"
	kind "StaticLib"
	language "C++"
	staticruntime "on"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"

	targetdir ("bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	pchheader "TRAPPCH.h"
	pchsource "%{prj.name}/src/TRAPPCH.cpp"

	--Add all source and header files
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.inl"
	}

	--Exclude all folders in Platform, since not all platforms need all of these
	removefiles 
	{
		"%{prj.name}/src/Utils/Win.h",
		"%{prj.name}/src/Log/ANSILog.cpp",
		"%{prj.name}/src/Log/WindowsLog.cpp",
		"%{prj.name}/src/Graphics/API/D3D12/**",
		"%{prj.name}/src/Utils/MsgBox/MsgBoxWindows.cpp",
		"%{prj.name}/src/Utils/MsgBox/MsgBoxLinux.cpp",
		"%{prj.name}/src/Utils/MsgBox/MsgBoxLinuxX11.h",
		"%{prj.name}/src/Utils/MsgBox/MsgBoxLinuxX11.cpp",
		"%{prj.name}/src/Input/WindowsInput.cpp",
		"%{prj.name}/src/Input/LinuxInput.cpp",
		"%{prj.name}/src/Window/WindowingAPIWin32.cpp",
		"%{prj.name}/src/Window/WindowingAPILinuxX11.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.VULKAN}/Include/",
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}",
		"%{IncludeDir.STANDALONE}",
		"%{IncludeDir.SPIRVCROSS}"
	}

	filter "system:windows"
		files
		{
			"%{prj.name}/src/Utils/Win.h",
			"%{prj.name}/src/Log/WindowsLog.cpp",
			"%{prj.name}/src/Graphics/API/D3D12/**",
			"%{prj.name}/src/Utils/MsgBox/MsgBoxWindows.cpp",
			"%{prj.name}/src/Input/WindowsInput.cpp",
			"%{prj.name}/src/Window/WindowingAPIWin32.cpp"
		}

		links
		{
			"ImGui",
			"D3D12",
			"DXGI",
			"D3DCOMPILER",
			"%{IncludeDir.VULKAN}/Lib/vulkan-1",
			"GLSLang",
			"SPIRV",
			"StandAlone",
			"SPIRV-Cross-Core",
			"SPIRV-Cross-GLSL",
			"SPIRV-Cross-HLSL",
			"DXGUID",
			"Imm32"
		}

	filter "system:linux"

		-- Add Linux-specific files
        files
        {
			"%{prj.name}/src/Log/ANSILog.cpp",
			"%{prj.name}/src/Input/LinuxInput.cpp",
			"%{prj.name}/src/Utils/MsgBox/MsgBoxLinux.cpp",
			"%{prj.name}/src/Utils/MsgBox/MsgBoxLinuxX11.h",
			"%{prj.name}/src/Utils/MsgBox/MsgBoxLinuxX11.cpp",
			"%{prj.name}/src/Window/WindowingAPILinuxX11.cpp"
		}

		links
		{
			"ImGui",
			"GLSLang",
			"SPIRV",
			"StandAlone",
			"vulkan",
			"SPIRV-Cross-Core",
			"SPIRV-Cross-GLSL",
			"SPIRV-Cross-HLSL"
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

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "on"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"

	targetdir ("bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"TRAP/src",
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}",
		"%{IncludeDir.STANDALONE}",
		"%{IncludeDir.VULKAN}/Include/",
		"%{IncludeDir.SPIRVCROSS}"
	}

	links
	{
		"TRAP"
	}

	filter "system:linux"
		links
		{
			"ImGui",
			"GLSLang",
			"SPIRV",
			"StandAlone",
			"vulkan",

			"dl",
			"pthread",
			"X11",
			"HLSL",
			"OGLCompiler",
			"OSDependent",
			"SPIRV-Cross-Core",
			"SPIRV-Cross-GLSL",
			"SPIRV-Cross-HLSL"
		}

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

project "Tests"
	location "Tests"
	kind "ConsoleApp"
	language "C++"
	staticruntime "on"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"

	targetdir ("bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"TRAP/src",
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}",
		"%{IncludeDir.STANDALONE}",
		"%{IncludeDir.VULKAN}/Include/",
		"%{IncludeDir.SPIRVCROSS}"
	}

	links
	{
		"TRAP"
	}

	filter "system:linux"
		links
		{
			"ImGui",
			"GLSLang",
			"SPIRV",
			"StandAlone",
			"vulkan",

			"dl",
			"pthread",
			"X11",
			"HLSL",
			"OGLCompiler",
			"OSDependent",
			"SPIRV-Cross-Core",
			"SPIRV-Cross-GLSL",
			"SPIRV-Cross-HLSL"
		}

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

project "ConvertToSPIRV"
	location "ConvertToSPIRV"
	kind "ConsoleApp"
	language "C++"
	staticruntime "on"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"

	targetdir ("bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}",
		"%{IncludeDir.STANDALONE}"
	}

	filter "system:windows"
		links
		{
			"GLSLang",
			"SPIRV",
			"StandAlone"
		}

	filter "system:linux"
		links
		{
			"GLSLang",
			"SPIRV",
			"StandAlone",

			"dl",
			"pthread",
			"HLSL",
			"OGLCompiler",
			"OSDependent"
		}

		filter "configurations:Debug"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			runtime "Release"
			optimize "On"

		filter "configurations:RelWithDebInfo"
			runtime "Release"
			optimize "On"
