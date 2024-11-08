--ODR violation fix
if os.isfile("SPIRV-Cross/spirv.hpp") then
    os.remove("SPIRV-Cross/spirv.hpp")
end

project "SPIRV-Cross"
    kind "StaticLib"
    language "C++"
    warnings "off"

if os.getenv("RUN_CICD_PIPELINE") ~= nil and not table.isempty(os.matchfiles(_MAIN_SCRIPT_DIR .. "/bin/**/*SPIRV-Cross.*")) then
    -- Using CICD Pipeline cache
else
    files
    {
        "SPIRV-Cross/GLSL.std.450.h",
        "SPIRV-Cross/spirv_common.hpp",

        -- "SPIRV-Cross/spirv.hpp", -- Replaced with GLSLangs spirv.hpp to resolve ODR violations
		"%{IncludeDir.SPIRV}/spirv.hpp",

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
        "SPIRV-Cross/spirv_hlsl.hpp"
    }
end

    includedirs "%{IncludeDir.SPIRV}"
