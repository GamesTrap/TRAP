project "SPIRV-Cross"
    kind "StaticLib"
    language "C++"
    warnings "off"

    files
    {
        "SPIRV-Cross/GLSL.std.450.h",
        "SPIRV-Cross/spirv_common.hpp",

        "SPIRV-Cross/spirv_cross.hpp",
        "SPIRV-Cross/spirv_parser.hpp",
        "SPIRV-Cross/spirv_parsed_ir.hpp",
        "SPIRV-Cross/spirv_cfg.hpp",

        "SPIRV-Cross/spirv_cross.cpp",
        "SPIRV-Cross/spirv_parser.cpp",
        "SPIRV-Cross/spirv_cross_parsed_ir.cpp",
        "SPIRV-Cross/spirv_cfg.cpp",

        "SPIRV-Cross/spirv_reflect.cpp",
        "SPIRV-Cross/spirv_reflect.hpp",

        "SPIRV-Cross/spirv_glsl.cpp",
        "SPIRV-Cross/spirv_glsl.hpp",

        "SPIRV-Cross/spirv_hlsl.cpp",
        "SPIRV-Cross/spirv_hlsl.hpp",

        "SPIRV-Cross/spirv.hpp"
    }

    includedirs "%{IncludeDir.SPIRV}"
