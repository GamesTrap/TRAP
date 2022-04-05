project "Headless-Tests"
	location "."
	kind "ConsoleApp"
	language "C++"
	staticruntime "on"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"
	warnings "Extra"

	targetdir ("../../bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("../../bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"../../TRAP/src"
	}

	sysincludedirs
	{
		"../../%{IncludeDir.IMGUI}",
		"../../%{IncludeDir.GLSLANG}",
		"../../%{IncludeDir.SPIRV}",
		"../../%{IncludeDir.VULKAN}/include/",
		"../../%{IncludeDir.SPIRVCROSS}",
		"../../%{IncludeDir.ENTT}",
		"../../%{IncludeDir.YAMLCPP}",
		"../../%{IncludeDir.MODERNDIALOGS}",
		"../../%{IncludeDir.VMA}"
	}

	links
	{
		"TRAP-Headless"
	}

	defines
	{
		"TRAP_HEADLESS_MODE"
	}

	filter "system:linux"
		links
		{
			"ImGui",
			"YAMLCpp",
			"ModernDialogs",
			"GLSLang",
			"SPIRV",
			"GLSLang-Default-Resource-Limits",

			"dl",
			"pthread",
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
		kind "WindowedApp"

	filter "configurations:RelWithDebInfo"
		defines "TRAP_RELWITHDEBINFO"
		runtime "Release"
		optimize "On"
		symbols "On"