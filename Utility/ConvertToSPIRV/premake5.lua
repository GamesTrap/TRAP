project "ConvertToSPIRV"
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
		"src/**.cpp"
	}

	externalincludedirs
	{
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}"
	}

	links
	{
		"GLSLang",
		"SPIRV",
		"GLSLang-Default-Resource-Limits",
	}

	filter "system:linux"
		links
		{
			"dl",
			"pthread"
		}

	filter { "action:gmake*", "toolset:gcc" }
		buildoptions
		{
			"-Wpedantic", "-Wconversion", "-Wshadow"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "Full"

	filter "configurations:RelWithDebInfo"
		runtime "Release"
		optimize "Debug"
		symbols "On"

	filter "configurations:Profiling"
		defines "TRACY_ENABLE"
		editandcontinue "Off"
		runtime "Release"
		optimize "Full"
		symbols "On"

	filter "configurations:ASAN"
		defines
		{
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