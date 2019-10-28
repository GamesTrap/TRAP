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
IncludeDir["SPIRVCROSS"] = "Dependencies/SPIRV-Cross"
IncludeDir["ZLIB"] = "Dependencies/zlib-ng"

group "Dependencies"
	include "Dependencies/GLFW"
	include "Dependencies/GLAD"
	include "Dependencies/ImGui"
	include "Dependencies/zlib-ng"
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
		"%{prj.name}/src/Graphics/API/D3D12/**",
		"%{prj.name}/src/Platform/**",
		"%{prj.name}/src/Utils/MsgBox/MsgBoxWindows.cpp",
		"%{prj.name}/src/Utils/MsgBox/MsgBoxLinuxX11.cpp",
		"%{prj.name}/src/Utils/MsgBox/MsgBoxOSX.mm"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.VULKAN}/Include/",
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}",
		"%{IncludeDir.STANDALONE}",
		"%{IncludeDir.SPIRVCROSS}",
		"%{IncludeDir.ZLIB}"
	}

	filter "system:windows"
		files
		{
			"%{prj.name}/src/Utils/Win.h",
			"%{prj.name}/src/Log/WindowsLog.cpp",
			"%{prj.name}/src/Platform/Windows/**.h",
			"%{prj.name}/src/Platform/Windows/**.cpp",
			"%{prj.name}/src/Graphics/API/D3D12/**",
			"%{prj.name}/src/Utils/MsgBox/MsgBoxWindows.cpp"
		}

		defines
		{
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
			"%{IncludeDir.VULKAN}/Lib/vulkan-1",
			"GLSLang",
			"SPIRV",
			"StandAlone",
			"SPIRV-Cross-Core",
			"SPIRV-Cross-GLSL",
			"SPIRV-Cross-HLSL",
			"ZLib-NG"
		}

	filter "system:linux"
	if os.getenv("WAYLAND_DISPLAY") and os.getenv("XDG_SESSION_TYPE") then
		if os.getenv("XDG_SESSION_TYPE") == "wayland" then			
			files
			{
				"%{prj.name}/src/Utils/MsgBox/MsgBoxLinuxWayland.cpp"
			}
		end
	else		
		files
		{
			"%{prj.name}/src/Utils/MsgBox/MsgBoxLinuxX11.cpp"
		}
	end

		-- Add Linux-specific files
        files
        {
			"%{prj.name}/src/Log/ANSILog.cpp",
            "%{prj.name}/src/Platform/Linux/**.h",
			"%{prj.name}/src/Platform/Linux/**.cpp"
		}

		defines
		{
			"GLFW_INCLUDE_NONE"
		}

		links
		{
			"GLAD",
			"GLFW",
			"ImGui",
			"GLSLang",
			"SPIRV",
			"StandAlone",
			"vulkan",
			"SPIRV-Cross-Core",
			"SPIRV-Cross-GLSL",
			"SPIRV-Cross-HLSL",
			"ZLib-NG"
		}

	filter "system:macosx"
		files
		{
			"%{prj.name}/src/Log/ANSILog.cpp",
			"%{prj.name}/src/Platform/Mac/**.h",
			"%{prj.name}/src/Platform/Mac/**.cpp",
			"%{prj.name}/src/Utils/MsgBox/MsgBoxOSX.mm"
		}

		defines
		{
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
			"Cocoa.framework",
			"Foundation.framework",
			"SPIRV-Cross-Core",
			"SPIRV-Cross-GLSL",
			"SPIRV-Cross-HLSL",
			"ZLib-NG",

			"c++fs"
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
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}",
		"%{IncludeDir.STANDALONE}",
		"%{IncludeDir.VULKAN}/Include/",
		"%{IncludeDir.SPIRVCROSS}",
		"%{IncludeDir.ZLIB}"
	}

	links
	{
		"TRAP"
	}

	filter "system:linux"
		links
		{
			"GLAD",
			"GLFW",
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
			"SPIRV-Cross-HLSL",
			"ZLib-NG"
		}

	filter "system:macosx"
		links
		{
			"GLFW",
			"GLAD",
			"ImGui",
			"%{IncludeDir.VULKAN}/lib/libvulkan",
			"GLSLang",
			"SPIRV",
			"StandAlone",
			"Cocoa.framework",
			"Foundation.framework",
			"SPIRV-Cross-Core",
			"SPIRV-Cross-GLSL",
			"SPIRV-Cross-HLSL",
			"ZLib-NG",

			"c++fs"
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

project "ConvertToSPIRV"
	location "ConvertToSPIRV"
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
	
	filter "system:macosx"
		links
		{
			"GLSLang",
			"SPIRV",
			"StandAlone",
	
			"c++fs"
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