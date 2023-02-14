project "SPIRV-Cross-Core"
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
        "SPIRV-Cross/GLSL.std.450.h",
        "SPIRV-Cross/spirv_common.hpp",
        "SPIRV-Cross/spirv.hpp",
        "SPIRV-Cross/spirv_cross.hpp",
        "SPIRV-Cross/spirv_parser.hpp",
        "SPIRV-Cross/spirv_parsed_ir.hpp",
        "SPIRV-Cross/spirv_cfg.hpp",

        "SPIRV-Cross/spirv_cross.cpp",
        "SPIRV-Cross/spirv_parser.cpp",
        "SPIRV-Cross/spirv_cross_parsed_ir.cpp",
        "SPIRV-Cross/spirv_cfg.cpp"
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

project "SPIRV-Cross-Reflect"
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
        "SPIRV-Cross/spirv_reflect.cpp",
        "SPIRV-Cross/spirv_reflect.hpp"
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

project "SPIRV-Cross-GLSL"
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
        "SPIRV-Cross/spirv_glsl.cpp",
        "SPIRV-Cross/spirv_glsl.hpp"
    }

    links "SPIRV-Cross-Core"

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

project "SPIRV-Cross-HLSL"
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
        "SPIRV-Cross/spirv_hlsl.cpp",
        "SPIRV-Cross/spirv_hlsl.hpp"
    }

    links "SPIRV-Cross-GLSL"

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
