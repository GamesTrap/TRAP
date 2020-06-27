project "TRAP"
	location "."
	kind "StaticLib"
	language "C++"
	staticruntime "on"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"

	targetdir ("../bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

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
		"src/Utils/MsgBox/MsgBoxWindows.cpp",
		"src/Utils/MsgBox/MsgBoxLinux.cpp",
		"src/Utils/MsgBox/MsgBoxLinuxX11.h",
		"src/Utils/MsgBox/MsgBoxLinuxX11.cpp",
		"src/Input/WindowsInput.cpp",
		"src/Input/LinuxInput.cpp",
		"src/Window/WindowingAPIWin32.cpp",
		"src/Window/WindowingAPILinuxX11.cpp"
	}

	includedirs
	{
		"src",
		"../%{IncludeDir.IMGUI}",
		"../%{IncludeDir.VULKAN}/Include/",
		"../%{IncludeDir.GLSLANG}",
		"../%{IncludeDir.SPIRV}",
		"../%{IncludeDir.SPIRVCROSS}"
	}

	filter "system:windows"
		files
		{
			"src/Utils/Win.h",
			"src/Log/WindowsLog.cpp",
			"src/Utils/MsgBox/MsgBoxWindows.cpp",
			"src/Input/WindowsInput.cpp",
			"src/Window/WindowingAPIWin32.cpp"
		}

		links
		{
			"ImGui",
			"%{IncludeDir.VULKAN}/Lib/vulkan-1",
			"GLSLang",
			"SPIRV",
			"SPIRV-Cross-Core",
			"SPIRV-Cross-GLSL",
			"SPIRV-Cross-HLSL",
			"Imm32"
		}

	filter "system:linux"

		-- Add Linux-specific files
        files
        {
			"src/Log/ANSILog.cpp",
			"src/Input/LinuxInput.cpp",
			"src/Utils/MsgBox/MsgBoxLinux.cpp",
			"src/Utils/MsgBox/MsgBoxLinuxX11.h",
			"src/Utils/MsgBox/MsgBoxLinuxX11.cpp",
			"src/Window/WindowingAPILinuxX11.cpp"
		}

		links
		{
			"ImGui",
			"GLSLang",
			"SPIRV",
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