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
		"src/Utils/Dialogs/MsgBoxWindows.cpp",
		"src/Utils/Dialogs/MsgBoxLinux.cpp",
		"src/Utils/Dialogs/MsgBoxLinuxX11.h",
		"src/Utils/Dialogs/MsgBoxLinuxX11.cpp",
		"src/Input/WindowsInput.cpp",
		"src/Input/LinuxInput.cpp",
		"src/Window/WindowingAPIWin32.cpp",
		"src/Window/WindowingAPILinuxX11.cpp",
		"src/Network/Sockets/Platform/SocketImplLinux.h",
		"src/Network/Sockets/Platform/SocketImplLinux.cpp",
		"src/Network/Sockets/Platform/SocketImplWinAPI.h",
		"src/Network/Sockets/Platform/SocketImplWinAPI.cpp"
	}

	includedirs
	{
		"src",
		"../%{IncludeDir.IMGUI}",
		"../%{IncludeDir.GLSLANG}",
		"../%{IncludeDir.SPIRV}",
		"../%{IncludeDir.SPIRVCROSS}",
		"../%{IncludeDir.VULKAN}/include/",
		"../%{IncludeDir.ENTT}",
		"../%{IncludeDir.YAMLCPP}"
	}

	filter "system:windows"
		files
		{
			"src/Utils/Win.h",
			"src/Log/WindowsLog.cpp",
			"src/Utils/Dialogs/MsgBoxWindows.cpp",
			"src/Input/WindowsInput.cpp",
			"src/Window/WindowingAPIWin32.cpp",
			"src/Network/Sockets/Platform/SocketImplWinAPI.h",
			"src/Network/Sockets/Platform/SocketImplWinAPI.cpp"
		}

		links
		{
			"ImGui",
			"YAMLCpp",
			"GLSLang",
			"SPIRV",
			"SPIRV-Cross-Core",
			"SPIRV-Cross-GLSL",
			"SPIRV-Cross-HLSL",
			"Imm32",
			"ws2_32"
		}

	filter "system:linux"

		-- Add Linux-specific files
        files
        {
			"src/Log/ANSILog.cpp",
			"src/Input/LinuxInput.cpp",
			"src/Utils/Dialogs/MsgBoxLinux.cpp",
			"src/Utils/Dialogs/MsgBoxLinuxX11.h",
			"src/Utils/Dialogs/MsgBoxLinuxX11.cpp",
			"src/Window/WindowingAPILinuxX11.cpp",
			"src/Network/Sockets/Platform/SocketImplLinux.h",
			"src/Network/Sockets/Platform/SocketImplLinux.cpp"
		}

		links
		{
			"ImGui",
			"YAMLCpp",
			"GLSLang",
			"SPIRV",
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