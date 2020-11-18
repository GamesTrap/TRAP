project "SPIRV-Cross-Core"
    kind "StaticLib"
    language "C++"
    systemversion "latest"
    staticruntime "on"
	cppdialect "C++17"
	warnings "off"

    targetdir ("../../bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("../../bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    files
    {
        "GLSL.std.450.h",
		"spirv_common.hpp",
		"spirv.hpp",
		"spirv_cross.hpp",
		"spirv_cross.cpp",
		"spirv_parser.hpp",
		"spirv_parser.cpp",
		"spirv_cross_parsed_ir.hpp",
		"spirv_cross_parsed_ir.cpp",
		"spirv_cfg.hpp",
		"spirv_cfg.cpp"
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