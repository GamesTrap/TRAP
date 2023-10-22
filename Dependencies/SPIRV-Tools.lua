local LanguageHeaderScript = "SPIRV-Tools/utils/generate_language_headers.py"
local GrammarProcessingScript = "SPIRV-Tools/utils/generate_grammar_tables.py"
local XMLRegistryProcessingScript = "SPIRV-Tools/utils/generate_registry_tables.py"
local UpdateBuildVersionScript = "SPIRV-Tools/utils/update_build_version.py"
local ChangesFile = "SPIRV-Tools/CHANGES"
local ConfigVersion = "unified1"
local ConfigVersionPath = path.join("SPIRV-Headers/include/spirv", ConfigVersion)
local CoreGrammarJSONFile = path.join(ConfigVersionPath, "spirv.core.grammar.json")
local OpenCLGrammarJSONFile = path.join(ConfigVersionPath, "extinst.opencl.std.100.grammar.json")
local GLSLGrammarJSONFile = path.join(ConfigVersionPath, "extinst.glsl.std.450.grammar.json")
local DebugInfoGrammarJSONFile = path.join(ConfigVersionPath, "extinst.debuginfo.grammar.json")
local OpenCLDebugInfoGrammarJSONFile = path.join(ConfigVersionPath, "extinst.opencl.debuginfo.100.grammar.json")
local VulkanDebugInfoGrammarJSONFile = path.join(ConfigVersionPath, "extinst.nonsemantic.shader.debuginfo.100.grammar.json")
local SPIRVXMLRegistryFile = "SPIRV-Headers/include/spirv/spir-v.xml"

function GenerateLangHeader(grammarFilePath, outputName)
    local outputPath = "SPIRV-Tools/source/" .. outputName .. ".h"

    if os.isfile(outputPath) then
        return
    end

    -- TODO Remove dependency to Python ...
    term.setTextColor(term.infoColor)
    print("Generating language specific header for " .. outputName .. ".")
    term.setTextColor(nil)
    os.execute("python " .. LanguageHeaderScript .. " --extinst-grammar=" .. grammarFilePath .. " --extinst-output-path=" .. outputPath)
end

function GenerateEnumStringMapping()
    local grammarExtensionEnumIncFile = "SPIRV-Tools/source/extension_enum.inc"
    local grammarEnumStringMappingIncFile = "SPIRV-Tools/source/enum_string_mapping.inc"

    if os.isfile(grammarExtensionEnumIncFile) and os.isfile(grammarEnumStringMappingIncFile) then
        return
    end

    -- TODO Remove dependency to Python ...
    term.setTextColor(term.infoColor)
    print("Generating enum-string mapping for SPIR-V v" .. ConfigVersion .. ".")
    term.setTextColor(nil)
    os.execute("python " .. GrammarProcessingScript ..
               " --spirv-core-grammar=" .. CoreGrammarJSONFile ..
               " --extinst-debuginfo-grammar=" .. DebugInfoGrammarJSONFile ..
               " --extinst-cldebuginfo100-grammar=" .. OpenCLDebugInfoGrammarJSONFile ..
               " --extension-enum-output=" .. grammarExtensionEnumIncFile ..
               " --enum-string-mapping-output=" .. grammarEnumStringMappingIncFile ..
               " --output-language=c++")
end

function GenerateVendorTable(vendorTable, shortName, operandKindPrefix)
    local instsFile = "SPIRV-Tools/source/" .. vendorTable .. ".insts.inc"
    local grammarFile = path.join(ConfigVersionPath,  "extinst." .. vendorTable .. ".grammar.json")

    if os.isfile(instsFile) then
        return
    end

    if not os.isfile(grammarFile) then
        grammarFile = "SPIRV-Tools/source/extinst." .. vendorTable .. ".grammar.json"
    end

    term.setTextColor(term.infoColor)
    print("Generating extended instruction table for " .. vendorTable .. ".")
    term.setTextColor(nil)
    -- TODO Remove dependency to Python ...
    os.execute("python " .. GrammarProcessingScript ..
               " --extinst-vendor-grammar=" .. grammarFile ..
               " --vendor-insts-output=" .. instsFile ..
               " --vendor-operand-kind-prefix=" .. operandKindPrefix)
end

function GenerateGLSLTable()
    local grammarIncFile = "SPIRV-Tools/source/glsl.std.450.insts.inc"

    if os.isfile(grammarIncFile) then
        return
    end

    term.setTextColor(term.infoColor)
    print("Generating info tables for GLSL extended instructions and operands v" .. ConfigVersion .. ".")
    term.setTextColor(nil)
    -- TODO Remove dependency to Python ...
    os.execute("python " .. GrammarProcessingScript ..
               " --extinst-glsl-grammar=" .. GLSLGrammarJSONFile ..
               " --glsl-insts-output=" .. grammarIncFile ..
               " --output-language=c++")
end

function GenerateOpenCLTable()
    local grammarIncFile = "SPIRV-Tools/source/opencl.std.insts.inc"

    if os.isfile(grammarIncFile) then
        return
    end

    term.setTextColor(term.infoColor)
    print("Generating info tables for OpenCL extended instructions and operands v" .. ConfigVersion .. ".")
    term.setTextColor(nil)
    -- TODO Remove dependency to Python ...
    os.execute("python " .. GrammarProcessingScript ..
               " --extinst-opencl-grammar=" .. OpenCLGrammarJSONFile ..
               " --opencl-insts-output=" .. grammarIncFile)
end

function GenerateCoreTable()
    local grammarInstsIncFile = "SPIRV-Tools/source/core.insts-" .. ConfigVersion .. ".inc"
    local grammarKindsIncFile = "SPIRV-Tools/source/operand.kinds-" .. ConfigVersion .. ".inc"

    if os.isfile(grammarInstsIncFile) and os.isfile(grammarKindsIncFile) then
        return
    end

    term.setTextColor(term.infoColor)
    print("Generating info tables for SPIR-V v" .. ConfigVersion .. " core instructions and operands.")
    term.setTextColor(nil)
    -- TODO Remove dependency to Python ...
    os.execute("python " .. GrammarProcessingScript ..
               " --spirv-core-grammar=" .. CoreGrammarJSONFile ..
               " --extinst-debuginfo-grammar=" .. DebugInfoGrammarJSONFile ..
               " --extinst-cldebuginfo100-grammar=" .. OpenCLDebugInfoGrammarJSONFile ..
               " --core-insts-output=" .. grammarInstsIncFile ..
               " --operand-kinds-output=" .. grammarKindsIncFile ..
               " --output-language=c++")
end

function GenerateGeneratorTables()
    local generatorIncFile = "SPIRV-Tools/source/generators.inc"

    if os.isfile(generatorIncFile) then
        return
    end

    term.setTextColor(term.infoColor)
    print("Generating tables based on the SPIR-V XML registry.")
    term.setTextColor(nil)
    -- TODO Remove dependency to Python ...
    os.execute("python " .. XMLRegistryProcessingScript ..
               " --xml=" .. SPIRVXMLRegistryFile ..
               " --generator-output=" .. generatorIncFile)
end

function GenerateBuildVersion()
    local buildVersionInc = "SPIRV-Tools/source/build-version.inc"

    term.setTextColor(term.infoColor)
    -- print("Update build-version.inc in the SPIRV-Tools build directory (if necessary).")
    term.setTextColor(nil)
    -- TODO Remove dependency to Python ...
    os.execute("python " .. UpdateBuildVersionScript ..
               " " .. ChangesFile .. " " .. buildVersionInc)
end

project "SPIRV-Tools"
    kind "StaticLib"
    language "C++"
    warnings "off"

    GenerateCoreTable()
    GenerateEnumStringMapping()
    GenerateOpenCLTable()
    GenerateGLSLTable()
    GenerateVendorTable("spv-amd-shader-explicit-vertex-parameter", "spv-amd-sevp", "")
    GenerateVendorTable("spv-amd-shader-trinary-minmax", "spv-amd-stm", "")
    GenerateVendorTable("spv-amd-gcn-shader", "spv-amd-gs", "")
    GenerateVendorTable("spv-amd-shader-ballot", "spv-amd-sb", "")
    GenerateVendorTable("debuginfo", "debuginfo", "")
    GenerateVendorTable("opencl.debuginfo.100", "cldi100", "CLDEBUG100_")
    GenerateVendorTable("nonsemantic.shader.debuginfo.100", "shdi100", "SHDEBUG100_")
    GenerateVendorTable("nonsemantic.clspvreflection", "clspvreflection", "")
    GenerateLangHeader(DebugInfoGrammarJSONFile, "DebugInfo")
    GenerateLangHeader(OpenCLDebugInfoGrammarJSONFile, "OpenCLDebugInfo100")
    GenerateLangHeader(VulkanDebugInfoGrammarJSONFile, "NonSemanticShaderDebugInfo100")

    GenerateGeneratorTables()

    GenerateBuildVersion()

    files
    {
        "SPIRV-Tools/include/libspirv.h",
        "SPIRV-Tools/include/libspirv.hpp",
        "SPIRV-Tools/include/optimizer.hpp",

        "SPIRV-Tools/source/opt/**.h",
        "SPIRV-Tools/source/opt/**.cpp",

        "SPIRV-Tools/source/util/**.h",
        "SPIRV-Tools/source/util/**.cpp",

        "SPIRV-Tools/source/val/**.h",
        "SPIRV-Tools/source/val/**.cpp",

        "SPIRV-Tools/source/*.h",
        "SPIRV-Tools/source/*.cpp",
    }

    includedirs
    {
        "%{IncludeDir.SPIRVTOOLS}",
        "%{IncludeDir.SPIRVTOOLS}/..",
        "%{IncludeDir.SPIRVTOOLS}/../source"
    }
