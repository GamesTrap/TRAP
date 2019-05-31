workspace "TRAP"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"RelWithDebInfo"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--Include directories relative to root folder(solution folder)
IncludeDir = {}
IncludeDir["GLFW"] = "Dependencies/GLFW/include"
IncludeDir["GLAD"] = "Dependencies/GLAD/include"
IncludeDir["IMGUI"] = "Dependencies/ImGui"
IncludeDir["VULKAN"] = os.getenv("VULKAN_SDK")
IncludeDir["GLSLANG"] = "Dependencies/GLSLang"
IncludeDir["OGLCOMPILER"] = "Dependencies/GLSLang/OGLCompilersDLL"
IncludeDir["OSDEPENDENT"] = "Dependencies/GLSLang/glslang/OSDependent"
IncludeDir["HLSL"] = "Dependencies/GLSLang/hlsl"
IncludeDir["SPIRV"] = "Dependencies/GLSLang/SPIRV"
IncludeDir["STANDALONE"] = "Dependencies/GLSLang/StandAlone"

group "Dependencies"
	include "Dependencies/GLFW"
	include "Dependencies/GLAD"
	include "Dependencies/ImGui"
	group "Dependencies/GLSLang"
		include "Dependencies/GLSLang/SPIRV"
		include "Dependencies/GLSLang/StandAlone"
		include "Dependencies/GLSLang/glslang"

group "Engine"
project "TRAP"
	location "TRAP"
	kind "StaticLib"
	language "C++"
	staticruntime "on"
	cppdialect "C++17"
	systemversion "latest"

	targetdir ("bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	pchheader "TRAPPCH.h"
	pchsource "%{prj.name}/src/TRAPPCH.cpp"

	--Add all source and header files
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.inl"
	}

	--Exclude all folders in Platform, since not all platforms need all of these
	removefiles 
	{
		"%{prj.name}/src/Utils/Win.h",
		"%{prj.name}/src/Log/ANSILog.cpp",
		"%{prj.name}/src/Log/WindowsLog.cpp",
		"%{prj.name}/src/Graphics/API/D3D12/D3D12Common.h",
		"%{prj.name}/src/Graphics/API/D3D12/D3D12Context.cpp",
		"%{prj.name}/src/Graphics/API/D3D12/D3D12Context.h",
		"%{prj.name}/src/Graphics/API/D3D12/D3D12Renderer.cpp",
		"%{prj.name}/src/Graphics/API/D3D12/D3D12Renderer.h",
		"%{prj.name}/src/Graphics/API/D3D12/D3D12Shader.h",
		"%{prj.name}/src/Graphics/API/D3D12/D3D12Shader.h",
		"%{prj.name}/src/Platform/**"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.VULKAN}/Include/vulkan",
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}",
		"%{IncludeDir.STANDALONE}"
	}

	filter "system:windows"
		files
		{
			"%{prj.name}/src/Utils/Win.h",
			"%{prj.name}/src/Log/WindowsLog.cpp",
			"%{prj.name}/src/Platform/Windows/**.h",
			"%{prj.name}/src/Platform/Windows/**.cpp",
			"%{prj.name}/src/Graphics/API/D3D12/D3D12Common.h",
			"%{prj.name}/src/Graphics/API/D3D12/D3D12Context.cpp",
			"%{prj.name}/src/Graphics/API/D3D12/D3D12Context.h",
			"%{prj.name}/src/Graphics/API/D3D12/D3D12Renderer.cpp",
			"%{prj.name}/src/Graphics/API/D3D12/D3D12Renderer.h",
			"%{prj.name}/src/Graphics/API/D3D12/D3D12Shader.h",
			"%{prj.name}/src/Graphics/API/D3D12/D3D12Shader.h"
		}

		defines
		{
			"TRAP_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

		links
		{ 
			"GLFW",
			"GLAD",
			"ImGui",
			"D3D12",
			"DXGI",
			"D3DCOMPILER",
			"%{IncludeDir.VULKAN}/lib/vulkan-1",
			"GLSLang",
			"SPIRV",
			"StandAlone"
		}

	filter "system:linux"

		-- Add Linux-specific files
        files
        {
			"%{prj.name}/src/Log/ANSILog.cpp",
            "%{prj.name}/src/Platform/Linux/**.h",
			"%{prj.name}/src/Platform/Linux/**.cpp"
        }

		defines
		{
			"TRAP_PLATFORM_LINUX",
			"GLFW_INCLUDE_NONE"
		}

		links
		{
			"GLFW",
			"GLAD",
			"ImGui",
			"%{IncludeDir.VULKAN}/lib/libvulkan",
			"GLSLang",
			"SPIRV",
			"StandAlone"
		}

	filter "system:macosx"
		files
		{
			"%{prj.name}/src/Log/ANSILog.cpp",
			"%{prj.name}/src/Platform/Mac/**.h",
			"%{prj.name}/src/Platform/Mac/**.cpp",
		}

		defines
		{
			"TRAP_PLATFORM_MACOSX",
			"GLFW_INCLUDE_NONE"
		}
	
		links
		{
			"GLFW",
			"GLAD",
			"ImGui",
			"%{IncludeDir.VULKAN}/lib/libvulkan",
			"GLSLang",
			"SPIRV",
			"StandAlone",
			"stdc++fs"
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
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.VULKAN}/Include/vulkan",
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}",
		"%{IncludeDir.STANDALONE}"
	}

	links
	{
		"TRAP"
	}

	filter "system:windows"
		defines
		{
			"TRAP_PLATFORM_WINDOWS"
		}

	filter "system:linux"
		defines
		{
			"TRAP_PLATFORM_LINUX"
		}

	filter "system:macosx"
		defines
		{
			"TRAP_PLATFORM_MACOSX"
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
		defines "TRAP_RelWithDebInfo"
		runtime "Release"
		optimize "On"