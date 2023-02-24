project "ModernDialogs"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    systemversion "latest"
    warnings "off"
	architecture "x86_64"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    files
    {
        "ModernDialogs/ModernDialogs/**.h",
        "ModernDialogs/ModernDialogs/**.cpp"
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
        editandcontinue "Off"
        runtime "Release"
        optimize "Full"
        symbols "On"

    filter "configurations:ASAN"
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