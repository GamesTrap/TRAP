#ifndef CONVERTTOSPIRV_SHADER_H
#define CONVERTTOSPIRV_SHADER_H

#include <fmt/format.h>
#include <fmt/std.h>

#include <glslang/Public/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <GlslangToSpv.h>

#include <array>
#include <string>
#include <span>

#include "Types.h"

#include "Utils.h"

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

//Format Specification as of 03.05.2023:

//1. Magic Number ("TRAP_SPV")
//2. Version number (uint32_t)
//2. Number of contained shader stages (uint8_t)
//For each SPIRV shader:
//    3. Size of SPIRV bytecode in bytes (std::size_t)
//    4. Shader type (ShaderStage/uint32_t)
//    5. SPIRV bytecode (std::size_t)

//The TRAP_SPV file may not contain:
//    - Multiple SPIRV shaders of the same type (i.e. 2 Vertex shaders in a single file).
//    - Incompatible shader types (i.e. a Vertex and a Compute shader).
//    - Empty SPIRV shaders (i.e. no bytecode).

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

inline constexpr std::string_view GLSLPrefix = "[GLSL] ";
inline constexpr std::string_view SPIRVPrefix = "[SPIRV] ";

using Macro = std::pair<std::string, std::string>;

//-------------------------------------------------------------------------------------------------------------------//

inline constexpr std::array<std::pair<std::string_view, std::string_view>, 2> DefaultShaderMacros
{
	{
		{"UpdateFreqStatic", "set = 0"},
		{"UpdateFreqDynamic", "set = 1"}
	}
};

//-------------------------------------------------------------------------------------------------------------------//

inline constinit static bool s_glslangInitialized = false;

//-------------------------------------------------------------------------------------------------------------------//

inline constexpr std::string_view MagicNumber = "TRAP_SPV";
inline constexpr uint32_t VersionNumber = 1u;
inline constexpr int32_t GLSLVersion = 460;
inline constexpr std::string_view ShaderFileEnding = "tp-spv";
inline constexpr uint32_t SPIRVMagicNumber = 0x07230203u;

//-------------------------------------------------------------------------------------------------------------------//

enum class ShaderStage : uint32_t
{
	None                   = 0,
	Vertex                 = BIT(0u),
	TessellationControl    = BIT(1u),
	TessellationEvaluation = BIT(2u),
	Geometry               = BIT(3u),
	Fragment               = BIT(4u),
	Compute                = BIT(5u),
	// RayTracing             = BIT(6u),

	AllGraphics = (static_cast<uint32_t>(Vertex) | static_cast<uint32_t>(TessellationControl) |
	               static_cast<uint32_t>(TessellationEvaluation) | static_cast<uint32_t>(Geometry) |
		           static_cast<uint32_t>(Fragment)),
	Hull = TessellationControl,
	Domain = TessellationEvaluation,

	SHADER_STAGE_COUNT = 7
};

[[nodiscard]] static inline constexpr ShaderStage operator|(ShaderStage a, ShaderStage b) noexcept { return static_cast<ShaderStage>(std::to_underlying(a) |
																		                                                             std::to_underlying(b)); }
[[nodiscard]] static inline constexpr ShaderStage operator&(ShaderStage a, ShaderStage b) noexcept { return static_cast<ShaderStage>(std::to_underlying(a) &
																		                                                             std::to_underlying(b)); }
static inline constexpr ShaderStage operator|=(ShaderStage& a, ShaderStage b) noexcept { return a = (a | b); }

//-------------------------------------------------------------------------------------------------------------------//

struct Shader
{
	std::string FilePath;
	std::string Source;
	ShaderStage Stages;

    struct SubShader
    {
		ShaderStage Stage = ShaderStage::None;
        std::string Source{};
        std::vector<uint32_t> SPIRV{};
    };

	std::vector<SubShader> SubShaderSources{};
};

//-------------------------------------------------------------------------------------------------------------------//

struct ShaderStageData
{
	ShaderStage Stage;
	std::string_view StageString;
	std::vector<std::string_view> StageIdentifierStrs;
	EShLanguage StageGLSLang;
};

inline const std::array<ShaderStageData, std::to_underlying(ShaderStage::SHADER_STAGE_COUNT)> ShaderStages
{
	{
		{ShaderStage::Vertex, "Vertex", {"vertex"}, EShLanguage::EShLangVertex},
		{ShaderStage::TessellationControl, "TessellationControl", {"tessellationcontrol", "tessellation control"}, EShLanguage::EShLangTessControl},
		{ShaderStage::TessellationEvaluation, "TessellationEvaluation", {"tessellationevaluation", "tessellation evaluation"}, EShLanguage::EShLangTessEvaluation},
		{ShaderStage::Geometry, "Geometry", {"geometry"}, EShLanguage::EShLangGeometry},
		{ShaderStage::Fragment, "Fragment", {"fragment", "pixel"}, EShLanguage::EShLangFragment},
		{ShaderStage::Compute, "Compute", {"compute"}, EShLanguage::EShLangCompute}
	}
};

//-------------------------------------------------------------------------------------------------------------------//

inline std::string_view ShaderStageToString(const ShaderStage stage)
{
	const auto it = std::ranges::find_if(ShaderStages, [stage](const auto& element){return stage == element.Stage;});
	return it->StageString;
}

//-------------------------------------------------------------------------------------------------------------------//

inline EShLanguage ShaderStageToEShLanguage(const ShaderStage stage)
{
	const auto it = std::ranges::find_if(ShaderStages, [stage](const auto& element){return stage == element.Stage;});
	return it->StageGLSLang;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline std::optional<ShaderStage> DetectShaderStage(const std::string_view identifyString)
{
    //Detect shader type
    for(const auto& shaderStageData : ShaderStages)
    {
		for(const std::string_view shaderStageIdentifier : shaderStageData.StageIdentifierStrs)
		{
			if(Contains(identifyString, shaderStageIdentifier))
				return shaderStageData.Stage;
		}
    }

    return std::nullopt;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline constexpr bool FindEntryPoint(const std::string& shaderStr)
{
	return Contains(ToLower(shaderStr), "main");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool ValidateShaderStages(const Shader& shader)
{
	const ShaderStage stages = shader.Stages;

	//Check if any Shader Stage is set
	if(ShaderStage::None == stages)
	{
		fmt::print(fg(fmt::color::red), "{}No Shader Stage found!\n", GLSLPrefix);
		return false;
	}

	//Check for "Normal" Shader Stages combined with Compute
	if (((ShaderStage::Vertex                 & stages) != ShaderStage::None ||
		 (ShaderStage::Fragment               & stages) != ShaderStage::None ||
		 (ShaderStage::TessellationControl    & stages) != ShaderStage::None ||
		 (ShaderStage::TessellationEvaluation & stages) != ShaderStage::None ||
		 (ShaderStage::Geometry               & stages) != ShaderStage::None) &&
		 (ShaderStage::Compute                & stages) != ShaderStage::None)
	{
		fmt::print(fg(fmt::color::red), "{}Rasterizer Shader Stages combined with Compute stage!\n", GLSLPrefix);
		return false;
	}

	//Check for Vertex Shader Stage & required Fragment/Pixel Shader Stage
	if ((ShaderStage::Vertex   & stages) != ShaderStage::None &&
		(ShaderStage::Fragment & stages) == ShaderStage::None)
	{
		fmt::print(fg(fmt::color::red), "{}Only Vertex Shader Stage provided! Missing Fragment/Pixel Shader Stage\n", GLSLPrefix);
		return false;
	}

	//Shader Stages should be valid
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool PreProcessGLSL(Shader& shader,
                                         const std::vector<Macro>& customMacros)
{
	std::optional<ShaderStage> currentShaderStage = std::nullopt;
	const std::vector<std::string> lines = GetLines(shader.Source);

	//Go through every line of the shader source
	for (std::size_t i = 0; i < lines.size(); ++i)
	{
		//Make it easier to parse
		const std::string lowerLine = ToLower(lines[i]);

		//Search for a shader type tag
		if (StartsWith(lowerLine, "#shader"))
		{
            //Get Shader stage type
            currentShaderStage = DetectShaderStage(lowerLine);
            if(!currentShaderStage)
            {
				fmt::print(fg(fmt::color::red), "{}Failed to detect valid shader stage\n", GLSLPrefix);
				fmt::println("{}Skipping unknown shader stage", GLSLPrefix);
                continue;
            }

			fmt::println("{}Adding Shader to \"{}\"", GLSLPrefix, ShaderStageToString(*currentShaderStage), shader.FilePath);

			//Check for duplicate "#shader XXX" defines
			if (static_cast<uint32_t>(shader.Stages & *currentShaderStage) != 0u)
			{
				fmt::print(fg(fmt::color::yellow), "{}Found duplicate \"#shader\" define: {}\n", GLSLPrefix, lines[i]);
				fmt::println("{}Skipping duplicated shader stage", GLSLPrefix);
				currentShaderStage = std::nullopt;
				continue;
			}

			shader.Stages |= *currentShaderStage;
			shader.SubShaderSources.push_back(Shader::SubShader{*currentShaderStage, "", std::vector<uint32_t>{}});
		}
		else if (Contains(lowerLine, "#version")) //Check for unnecessary "#version" define
			fmt::println("{}Found Tag: \"{}\" this is unnecessary! Skipping Line: {}", GLSLPrefix, lines[i], i);
		else if (currentShaderStage) //Add shader code to detected shader stage
            shader.SubShaderSources.back().Source.append(lines[i] + '\n');
	}

	for(auto& subShader : shader.SubShaderSources)
	{
        if(!FindEntryPoint(subShader.Source))
        {
			fmt::print(fg(fmt::color::red), "{}{} Shader Couldn't find \"main\" function!\n", GLSLPrefix, ShaderStageToString(subShader.Stage));
            return false;
        }

		//Found main function
		//Add GLSL version before any shader code
		std::string tmp = subShader.Source;
		subShader.Source = "#version 460 core\n";
		for(const auto& [name, value] : DefaultShaderMacros)
			subShader.Source += fmt::format("#define {} {}\n", name, value);
		for(const auto& [name, value] : customMacros)
			subShader.Source += fmt::format("#define {} {}\n", name, value);
		subShader.Source += tmp;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool ParseGLSL(glslang::TShader& shader)
{
	const TBuiltInResource* const DefaultTBuiltInResource = GetDefaultResources();
	if (!shader.parse(DefaultTBuiltInResource, GLSLVersion, true,
                      static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules)))
	{
		fmt::print(fg(fmt::color::red), "{}Parsing failed:\n", GLSLPrefix);
		fmt::print(fg(fmt::color::red), "{}{}\n", GLSLPrefix, shader.getInfoLog());
		fmt::print(fg(fmt::color::red), "{}{}\n", GLSLPrefix, shader.getInfoDebugLog());

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool LinkGLSL(glslang::TShader& shader, glslang::TProgram& program)
{
	program.addShader(&shader);

	if (!program.link(static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules)))
	{
		fmt::print(fg(fmt::color::red), "{}Linking failed:\n", GLSLPrefix);
		fmt::print(fg(fmt::color::red), "{}{}\n", GLSLPrefix, program.getInfoLog());
		fmt::print(fg(fmt::color::red), "{}{}\n", GLSLPrefix, program.getInfoDebugLog());

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline std::vector<uint32_t> ConvertToSPIRV(const ShaderStage stage, glslang::TProgram& program)
{
	std::vector<uint32_t> SPIRV{};

    spv::SpvBuildLogger logger{};
    glslang::SpvOptions spvOptions{};
    spvOptions.disableOptimizer = false;
    spvOptions.optimizeSize = true;

#ifdef TRAP_PLATFORM_DEBUG
	spvOptions.validate = true;
#endif /*TRAP_PLATFORM_DEBUG*/

    glslang::GlslangToSpv(*program.getIntermediate(ShaderStageToEShLanguage(stage)), SPIRV, &logger, &spvOptions);
    if (logger.getAllMessages().length() > 0)
		fmt::println("{}{} Shader: {}", SPIRVPrefix, ShaderStageToString(stage), logger.getAllMessages());

    return SPIRV;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool InitializeGLSLang()
{
	if (!s_glslangInitialized)
	{
		if (!glslang::InitializeProcess())
		{
			fmt::print(fg(fmt::color::red), "{}GLSLang initialization failed!\n", GLSLPrefix);
			return false;
		}
		s_glslangInitialized = true;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool CompileGLSLToSPIRV(Shader& shader)
{
	if(!InitializeGLSLang())
		return false;

	for (auto& subShaderSource : shader.SubShaderSources)
	{
        glslang::TProgram program;

		const char* srcCStr = subShaderSource.Source.c_str();

		glslang::TShader glslShader = glslang::TShader(ShaderStageToEShLanguage(subShaderSource.Stage));
		glslShader.setStrings(&srcCStr, 1);
		glslShader.setEnvInput(glslang::EShSourceGlsl, ShaderStageToEShLanguage(subShaderSource.Stage), glslang::EShClientVulkan, GLSLVersion);
		glslShader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1);
		glslShader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_4);

		fmt::println("{}Parsing {} Shader", GLSLPrefix, ShaderStageToString(subShaderSource.Stage));
        if (!ParseGLSL(glslShader))
            return false;

		fmt::println("{}Linking {} Shader", GLSLPrefix, ShaderStageToString(subShaderSource.Stage));
        if (!LinkGLSL(glslShader, program))
            return false;

		fmt::println("{}Converting GLSL -> SPIR-V", SPIRVPrefix);
        subShaderSource.SPIRV = ConvertToSPIRV(subShaderSource.Stage, program);
		if(subShaderSource.SPIRV.empty())
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool SaveSPIRV(Shader& shader, std::optional<std::filesystem::path> customOutput)
{
	if(shader.Stages == ShaderStage::None || shader.SubShaderSources.empty())
	{
		fmt::print(fg(fmt::color::red), "Can't save empty shader!\n");
		return false;
	}

	if(!customOutput)
        customOutput = shader.FilePath;

	customOutput->replace_extension(ShaderFileEnding);

	fmt::println("{}Saving Shader: {}", SPIRVPrefix, *customOutput);

	std::ofstream file(*customOutput, std::ios::binary);
	if (!file.is_open())
    {
		fmt::print(fg(fmt::color::red), "Couldn't open file: {}\n", *customOutput);
        return false;
    }

	file.write(MagicNumber.data(), MagicNumber.size());
	file.write(reinterpret_cast<const char*>(&VersionNumber), sizeof(VersionNumber));

    const uint8_t SPIRVSubShadersCount = static_cast<uint8_t>(shader.SubShaderSources.size());
    file.write(reinterpret_cast<const char*>(&SPIRVSubShadersCount), sizeof(SPIRVSubShadersCount));

    for(const auto& subShader : shader.SubShaderSources)
    {
		const std::size_t SPIRVSize = subShader.SPIRV.size();
		const uint8_t type = static_cast<uint8_t>(std::to_underlying(subShader.Stage));

		file.write(reinterpret_cast<const char*>(&SPIRVSize), sizeof(SPIRVSize));
		file.write(reinterpret_cast<const char*>(&type), sizeof(type));
		file.write(reinterpret_cast<const char*>(subShader.SPIRV.data()), static_cast<std::streamsize>(subShader.SPIRV.size() * sizeof(decltype(subShader.SPIRV)::value_type)));
    }

    file.close();

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool LoadShader(const std::filesystem::path& filePath, Shader& outShader)
{
	if (!FileOrFolderExists(filePath))
	{
		fmt::print(fg(fmt::color::red), "Couldn't find file or folder: {}\n", filePath);
		return false;
	}

	const std::optional<std::string> source = ReadTextFile(filePath);
	if (!source)
		return false;

	std::string newPath = filePath.string();
	newPath = newPath.substr(0, newPath.size() - 7);
	outShader = Shader{newPath, *source, ShaderStage::None};

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline constexpr bool CheckShaderMagicNumber(const std::span<const uint8_t> shaderData)
{
	return (shaderData.size() >= MagicNumber.size() &&
	        std::string_view(reinterpret_cast<const char*>(shaderData.data()), MagicNumber.size()) == MagicNumber);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool ParseTPSPVShader(const std::span<const uint8_t> shaderData, Shader& outShader, uint32_t& outVersion)
{
	if(shaderData.size() <= (MagicNumber.size() + sizeof(VersionNumber) + sizeof(uint8_t)))
	{
		fmt::print(fg(fmt::color::red), "Invalid or corrupted shader!\n");
		return false;
	}
	if(!CheckShaderMagicNumber(shaderData))
	{
		fmt::print(fg(fmt::color::red), "Invalid or corrupted shader!\n");
		return false;
	}

	std::size_t currIndex = MagicNumber.size();

	outVersion = ConvertByte<uint32_t>(&shaderData[currIndex]);
	currIndex += sizeof(VersionNumber);

	const uint8_t shaderCount = shaderData[currIndex++];

	outShader.SubShaderSources.resize(shaderCount);

	for(uint8_t i = 0; i < shaderCount; ++i)
	{
		if(shaderData.size() <= (currIndex + sizeof(std::size_t) + sizeof(uint8_t)))
			break;

		Shader::SubShader& subShader = outShader.SubShaderSources[i];

		const std::size_t SPIRVSize = ConvertByte<std::size_t>(&shaderData[currIndex]);
		currIndex += sizeof(SPIRVSize);

		subShader.SPIRV.resize(SPIRVSize);
		subShader.Stage = static_cast<ShaderStage>(shaderData[currIndex++]);

		outShader.Stages |= subShader.Stage;

		if(shaderData.size() < (currIndex + (subShader.SPIRV.size() * sizeof(decltype(subShader.SPIRV)::value_type))))
			break;

		const uint32_t SPIRVMagic = ConvertByte<uint32_t>(&shaderData[currIndex]);
		if(SPIRVMagic != SPIRVMagicNumber)
			fmt::print(fg(fmt::color::yellow), "{}shader stage {} contains invalid magic number!", SPIRVPrefix, ShaderStageToString(subShader.Stage));

		currIndex += subShader.SPIRV.size() * sizeof(decltype(subShader.SPIRV)::value_type);
	}

	return true;
}

#endif /*CONVERTTOSPIRV_SHADER_H*/
