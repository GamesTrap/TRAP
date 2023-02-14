project "Headless-Tests"
	location "."
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"
	cppdialect "C++17"
	systemversion "latest"
	vectorextensions "AVX2"
	warnings "Extra"
	architecture "x86_64"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",

		"%{wks.location}/Games/Sanitizer.cpp"
	}

	includedirs "%{wks.location}/TRAP/src"

	externalincludedirs
	{
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.IMGUIZMO}",
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}",
		"%{IncludeDir.VULKAN}/include/",
		"%{IncludeDir.SPIRVCROSS}",
		"%{IncludeDir.ENTT}",
		"%{IncludeDir.YAMLCPP}",
		"%{IncludeDir.MODERNDIALOGS}",
		"%{IncludeDir.VMA}",
		"%{IncludeDir.BOX2D}",
		"%{IncludeDir.TRACY}"
	}

	links "TRAP-Headless"

	defines "TRAP_HEADLESS_MODE"

	filter "system:linux"
		links
		{
			"ImGui",
			"ImGuizmo",
			"YAMLCpp",
			"ModernDialogs",
			"GLSLang",
			"SPIRV",
			"GLSLang-Default-Resource-Limits",
			"Box2D",
			"TracyClient",

			"dl",
			"pthread",
			"SPIRV-Cross-Core",
			"SPIRV-Cross-GLSL",
			"SPIRV-Cross-HLSL"
		}

	filter "system:windows"
		links
		{
			-- Needed for Networking
			"ws2_32",
			"wsock32"
		}

	filter { "action:gmake*", "toolset:gcc" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}

	filter "configurations:Debug"
		defines "TRAP_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "TRAP_RELEASE"
		runtime "Release"
		optimize "Full"
		kind "WindowedApp"

	filter "configurations:RelWithDebInfo"
		defines "TRAP_RELWITHDEBINFO"
		runtime "Release"
		optimize "Debug"
		symbols "On"

	filter "configurations:Profiling"
		editandcontinue "Off"
		defines
		{
			"TRAP_RELEASE",
			"TRACY_ENABLE"
		}
		runtime "Release"
		optimize "Full"
		symbols "On"

	filter "configurations:ASAN"
		defines
		{
			"TRAP_RELWITHDEBINFO",
			"TRAP_ASAN"
		}
		runtime "Release"
		optimize "Debug"
		symbols "On"
		buildoptions
		{
			"-fsanitize=address",
			"-fno-omit-frame-pointer",
			"-g"
		}
		linkoptions
		{
			"-fsanitize=address",
			"-static-libasan"
		}

	filter "configurations:UBSAN"
		defines
		{
			"TRAP_RELWITHDEBINFO",
			"TRAP_UBSAN"
		}
		runtime "Release"
		optimize "Debug"
		symbols "On"
		buildoptions
		{
			"-fsanitize=undefined",
			"-fno-omit-frame-pointer",
			"-g"
		}
		linkoptions
		{
			"-fsanitize=undefined",
			"-static-libubsan"
		}

	filter "configurations:LSAN"
		defines
		{
			"TRAP_RELWITHDEBINFO",
			"TRAP_LSAN"
		}
		runtime "Release"
		optimize "Debug"
		symbols "On"
		buildoptions
		{
			"-fsanitize=leak",
			"-fno-omit-frame-pointer",
			"-g"
		}
		linkoptions
		{
			"-fsanitize=leak"
		}

	filter "configurations:TSAN"
		staticruntime "off"
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
			"-fsanitize=thread",
			"-static-libtsan"
		}