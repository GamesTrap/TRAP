#ifndef CONVERTTOSPIRV_SHADER_H
#define CONVERTTOSPIRV_SHADER_H

#include <glslang/Public/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <GlslangToSpv.h>

#include <array>
#include <string>

#include "Utils.h"

inline std::array<std::array<std::string, 2>, 2> DefaultShaderMacros
{
	{
		{"UpdateFreqStatic", "set = 0"},
		{"UpdateFreqDynamic", "set = 1"}
	}
};

//-------------------------------------------------------------------------------------------------------------------//

static bool s_glslangInitialized = false;

//-------------------------------------------------------------------------------------------------------------------//

enum class ShaderStage : uint32_t
{
	None                   = 0,
	Vertex                 = BIT(0),
	TessellationControl    = BIT(1),
	TessellationEvaluation = BIT(2),
	Geometry               = BIT(3),
	Fragment               = BIT(4),
	Compute                = BIT(5),
	// RayTracing             = BIT(6),

	AllGraphics = (static_cast<uint32_t>(Vertex) | static_cast<uint32_t>(TessellationControl) |
	               static_cast<uint32_t>(TessellationEvaluation) | static_cast<uint32_t>(Geometry) |
		           static_cast<uint32_t>(Fragment)),
	Hull = TessellationControl,
	Domain = TessellationEvaluation,

	SHADER_STAGE_COUNT = 7
};

static inline ShaderStage operator|(ShaderStage a, ShaderStage b) noexcept { return static_cast<ShaderStage>(static_cast<std::underlying_type<ShaderStage>::type>(a) |
																		                                     static_cast<std::underlying_type<ShaderStage>::type>(b)); }
static inline ShaderStage operator&(ShaderStage a, ShaderStage b) noexcept { return static_cast<ShaderStage>(static_cast<std::underlying_type<ShaderStage>::type>(a) &
																		                                     static_cast<std::underlying_type<ShaderStage>::type>(b)); }
static inline ShaderStage operator|=(ShaderStage& a, ShaderStage b) noexcept { return a = (a | b); }
// static inline ShaderStage operator&=(ShaderStage& a, ShaderStage b) noexcept { return a = (a & b); }

//-------------------------------------------------------------------------------------------------------------------//

struct Shader
{
	std::string FilePath;
	std::string Source;
	ShaderStage Stages;

    struct SubShader
    {
        std::string Source;
        std::vector<uint32_t> SPIRV{};
    };

	std::array<SubShader, static_cast<uint32_t>(ShaderStage::SHADER_STAGE_COUNT)> SubShaderSources{};
};

//-------------------------------------------------------------------------------------------------------------------//

inline const std::unordered_map<ShaderStage, std::string_view> ShaderStageToString
{
    {ShaderStage::Vertex, "Vertex"},
    {ShaderStage::Fragment, "Fragment"},
    {ShaderStage::Geometry, "Geometry"},
    {ShaderStage::TessellationControl, "TessellationControl"},
    {ShaderStage::TessellationEvaluation, "TessellationEvaluation"},
    {ShaderStage::Compute, "Compute"}
};

//-------------------------------------------------------------------------------------------------------------------//

inline constexpr std::array<ShaderStage, 7> IndexToShaderStage
{
    ShaderStage::Vertex,
    ShaderStage::TessellationControl,
    ShaderStage::TessellationEvaluation,
    ShaderStage::Geometry,
    ShaderStage::Fragment,
    ShaderStage::Compute
};

//-------------------------------------------------------------------------------------------------------------------//

inline const std::unordered_map<ShaderStage, uint32_t> ShaderStageToIndex
{
    { ShaderStage::Vertex, 0 },
    { ShaderStage::TessellationControl, 1 },
    { ShaderStage::TessellationEvaluation, 2 },
    { ShaderStage::Geometry, 3 },
    { ShaderStage::Fragment, 4 },
    { ShaderStage::Compute, 5 }
};

//-------------------------------------------------------------------------------------------------------------------//

inline const std::unordered_map<ShaderStage, EShLanguage> ShaderStageToEShLanguage
{
    { ShaderStage::Vertex, EShLangVertex },
    { ShaderStage::TessellationControl, EShLangTessControl },
    { ShaderStage::TessellationEvaluation, EShLangTessEvaluation },
    { ShaderStage::Geometry, EShLangGeometry },
    { ShaderStage::Fragment, EShLangFragment },
    { ShaderStage::Compute, EShLangCompute }
};

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline ShaderStage DetectShaderStage(const std::string_view identifyString)
{
    static const std::unordered_map<std::string_view, ShaderStage> stages
    {
        {"vertex", ShaderStage::Vertex},
        {"fragment", ShaderStage::Fragment},
        {"pixel", ShaderStage::Fragment},
        {"geometry", ShaderStage::Geometry},
        {"tessellationcontrol", ShaderStage::TessellationControl},
        {"tessellation control", ShaderStage::TessellationControl},
        {"tessellationevaluation", ShaderStage::TessellationEvaluation},
        {"tessellation evaluation", ShaderStage::TessellationEvaluation},
        {"compute", ShaderStage::Compute}
    };

    //Detect shader type
    for(const auto& [str, stage] : stages)
    {
        if(identifyString.find(str) != std::string_view::npos)
            return stage;
    }

    return ShaderStage::None;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool FindEntryPoint(const std::string& shaderStr)
{
    return ToLower(shaderStr).find("main") != std::string::npos;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool ValidateShaderStages(const Shader& shader)
{
	const ShaderStage stages = shader.Stages;

	//Check if any Shader Stage is set
	if(ShaderStage::None == stages)
	{
		std::cerr << "[GLSL] No Shader Stage found!" << '\n';
		return false;
	}

	//Check for "Normal" Shader Stages combined with Compute
	if (((static_cast<uint32_t>(ShaderStage::Vertex                 & stages) != 0u) ||
		 (static_cast<uint32_t>(ShaderStage::Fragment               & stages) != 0u) ||
		 (static_cast<uint32_t>(ShaderStage::TessellationControl    & stages) != 0u) ||
		 (static_cast<uint32_t>(ShaderStage::TessellationEvaluation & stages) != 0u) ||
		 (static_cast<uint32_t>(ShaderStage::Geometry               & stages) != 0u)) &&
		 (static_cast<uint32_t>(ShaderStage::Compute                & stages) != 0u))
	{
		std::cerr << "[GLSL] Rasterizer Shader Stages combined with Compute stage!" << '\n';
		return false;
	}

	//Check for Vertex Shader Stage & required Fragment/Pixel Shader Stage
	if (  (static_cast<uint32_t>(ShaderStage::Vertex   & stages) != 0u) &&
		((static_cast<uint32_t>(ShaderStage::Fragment & stages)) == 0u))
	{
		std::cerr << "[GLSL] Only Vertex Shader Stage provided! Missing Fragment/Pixel Shader Stage" << '\n';
		return false;
	}

	//Shader Stages should be valid
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool PreProcessGLSL(Shader& shader, const std::vector<std::array<std::string, 2>>& customMacros)
{
	ShaderStage currentShaderStage = ShaderStage::None;
	const std::vector<std::string> lines = GetLines(shader.Source);

	//Go through every line of the shader source
	for (uint32_t i = 0; i < lines.size(); ++i)
	{
		//Make it easier to parse
		const std::string lowerLine = ToLower(lines[i]);

		//Search for a shader type tag
		if (StartsWith(lowerLine, "#shader"))
		{
            //Get Shader stage type
            currentShaderStage = DetectShaderStage(lowerLine);
            if(currentShaderStage != ShaderStage::None)
                std::cout << "[GLSL] Adding " << ShaderStageToString.at(currentShaderStage) << " Shader to \"" << shader.FilePath << "\"" << '\n';
            else
            {
                std::cerr << "[GLSL] Failed to detect valid shader stage\n";
                std::cerr << "[GLSL] Skipping unknown shader stage\n";
                continue;
            }

			//Check for duplicate "#shader XXX" defines
			if (static_cast<uint32_t>(shader.Stages & currentShaderStage) != 0u)
			{
				std::cerr << "[GLSL] Found duplicate \"#shader\" define: " << lines[i] << '\n';
                std::cerr << "[GLSL] Skipping duplicated shader stage\n";
				continue;
			}

			shader.Stages |= currentShaderStage;
		}
		else if (lowerLine.find("#version") != std::string::npos) //Check for unnecessary "#version" define
			std::cout << "[GLSL] Found Tag: \"" << lines[i] << "\" this is unnecessary! Skipping Line: " << i << '\n';
		else if (currentShaderStage != ShaderStage::None) //Add shader code to detected shader stage
            shader.SubShaderSources[ShaderStageToIndex.at(currentShaderStage)].Source.append(lines[i] + '\n');
	}

	for(std::size_t i = 0; i < shader.SubShaderSources.size(); ++i)
	{
        if(!FindEntryPoint(shader.SubShaderSources[i].Source) &&
           (static_cast<uint32_t>(IndexToShaderStage.at(i) & shader.Stages) != 0u))
        {
            std::cerr << "[GLSL] " << ShaderStageToString.at(IndexToShaderStage.at(i)) << " Shader Couldn't find \"main\" function!\n";
            return false;
        }

		if (!shader.SubShaderSources[i].Source.empty())
		{
			//Found main function
			//Add GLSL version before any shader code
			std::string tmp = shader.SubShaderSources[i].Source;
			shader.SubShaderSources[i].Source = "#version 460 core\n";
			for(const auto& macro : DefaultShaderMacros)
				shader.SubShaderSources[i].Source += "#define " + std::get<0>(macro) + " " + std::get<1>(macro) + '\n';
            for(const auto& macro : customMacros)
                shader.SubShaderSources[i].Source += "#define " + std::get<0>(macro) + " " + std::get<1>(macro) + '\n';
			shader.SubShaderSources[i].Source += tmp;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

inline constexpr int32_t GLSLVersion = 460;

[[nodiscard]] inline std::unique_ptr<glslang::TShader> PreProcessGLSLForConversion(const char* source, const ShaderStage stage,
                                                                                   std::string& preProcessedSource)
{
	std::unique_ptr<glslang::TShader> shader = nullptr;

	shader = std::make_unique<glslang::TShader>(ShaderStageToEShLanguage.at(stage));
	shader->setStrings(&source, 1);
	shader->setEnvInput(glslang::EShSourceGlsl, ShaderStageToEShLanguage.at(stage), glslang::EShClientVulkan, GLSLVersion);

	shader->setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1);
	shader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3);
	glslang::TShader::ForbidIncluder includer;
	const TBuiltInResource* const DefaultTBuiltInResource = GetDefaultResources();
	if (!shader->preprocess(DefaultTBuiltInResource,
		GLSLVersion,
		ECoreProfile,
		true,
		true,
		static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules),
		&preProcessedSource,
		includer))
	{
		std::cerr << "[GLSL] GLSL -> SPIR-V Preprocessing failed!\n";
		std::cerr << "[GLSL] " << shader->getInfoLog() << '\n';
		std::cerr << "[GLSL] " << shader->getInfoDebugLog() << '\n';

		return nullptr;
	}

	return shader;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool ParseGLSL(glslang::TShader* shader)
{
	const TBuiltInResource* const DefaultTBuiltInResource = GetDefaultResources();
	if (!shader->parse(DefaultTBuiltInResource, GLSLVersion, true,
                       static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules)))
	{
		std::cerr << "[GLSL] Parsing failed:\n";
		std::cerr << "[GLSL] " << shader->getInfoLog() << '\n';
		std::cerr << "[GLSL] " << shader->getInfoDebugLog() << '\n';

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool LinkGLSL(glslang::TShader* shader, glslang::TProgram& program)
{
	if (shader != nullptr)
		program.addShader(shader);

	if (!program.link(static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules)))
	{
		std::cerr << "[GLSL] Linking failed:\n";
		std::cerr << "[GLSL] " << program.getInfoLog() << '\n';
		std::cerr << "[GLSL] " << program.getInfoDebugLog() << '\n';

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline std::vector<uint32_t> ConvertToSPIRV(glslang::TShader* shader, const ShaderStage stage,
                                                          glslang::TProgram& program)
{
	std::vector<uint32_t> SPIRV{};

    if(shader == nullptr)
        return SPIRV;

    spv::SpvBuildLogger logger{};
    glslang::SpvOptions spvOptions{};
    spvOptions.disableOptimizer = false;
    spvOptions.optimizeSize = true;

    glslang::GlslangToSpv(*program.getIntermediate(ShaderStageToEShLanguage.at(stage)), SPIRV, &logger, &spvOptions);
    if (logger.getAllMessages().length() > 0)
        std::cout << "[SPIRV] " << ShaderStageToString.at(stage) << " Shader: " << logger.getAllMessages() << '\n';

    return SPIRV;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool CompileGLSLToSPIRV(Shader& shader)
{
	if (!s_glslangInitialized)
	{
		if (!glslang::InitializeProcess())
		{
			std::cerr << "[GLSL] GLSLang initialization failed!\n";
			return false;
		}
		s_glslangInitialized = true;
	}

	std::array<std::unique_ptr<glslang::TShader>, static_cast<uint32_t>(ShaderStage::SHADER_STAGE_COUNT)> glslShaders{};
	for (uint32_t i = 0; i < shader.SubShaderSources.size(); ++i)
	{
        if(shader.SubShaderSources[i].Source.empty())
            continue;

        glslang::TProgram program;

        std::string preProcessedSource;

        std::cout << "[GLSL] Pre-Processing " << ShaderStageToString.at(IndexToShaderStage.at(i)) << " Shader\n";
        glslShaders[i] = PreProcessGLSLForConversion(shader.SubShaderSources[i].Source.c_str(), IndexToShaderStage.at(i), preProcessedSource);
        if (preProcessedSource.empty())
            return false;

        const char* preProcessedCStr = preProcessedSource.c_str();
        glslShaders[i]->setStrings(&preProcessedCStr, 1);

        std::cout << "[GLSL] Parsing " << ShaderStageToString.at(IndexToShaderStage.at(i)) << " Shader\n";
        if (!ParseGLSL(glslShaders[i].get()))
            return false;

        std::cout << "[GLSL] Linking " << ShaderStageToString.at(IndexToShaderStage.at(i)) << " Shader\n";
        if (!LinkGLSL(glslShaders[i].get(), program))
            return false;

        std::cout << "[SPIRV] Converting GLSL -> SPIR-V\n";
        shader.SubShaderSources[i].SPIRV = ConvertToSPIRV(glslShaders[i].get(), IndexToShaderStage.at(i), program);
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool SaveSPIRV(Shader& shader, const std::filesystem::path& customOutput)
{
    std::filesystem::path filePath{};
    if(!customOutput.empty())
    {
        filePath = customOutput;
        if(!customOutput.has_extension())
            filePath += ".spirv";
    }
    else
        filePath = shader.FilePath + ".spirv";

    std::cout << "[SPIRV] Saving Shader: \"" << filePath.u8string() << "\"" << '\n';

	std::ofstream file(filePath, std::ios::binary);
	if (!file.is_open())
    {
		std::cerr << "Couldn't open file: " << filePath.u8string() << '\n';
        return false;
    }

    uint32_t SPIRVSubShadersCount = 0;
    for(const auto& subShader : shader.SubShaderSources)
    {
        if (!subShader.SPIRV.empty())
            ++SPIRVSubShadersCount;
    }
    file.write(reinterpret_cast<char*>(&SPIRVSubShadersCount), sizeof(uint32_t));

    for(std::size_t i = 0; i < shader.SubShaderSources.size(); ++i)
    {
        if(!shader.SubShaderSources[i].SPIRV.empty())
        {
            const uint32_t SPIRVSize = static_cast<uint32_t>(shader.SubShaderSources[i].SPIRV.size());
            const uint32_t type = static_cast<uint32_t>(IndexToShaderStage.at(i));

            file.write(reinterpret_cast<const char*>(&SPIRVSize), sizeof(uint32_t));
            file.write(reinterpret_cast<const char*>(&type), sizeof(uint32_t));
            file.write(reinterpret_cast<const char*>(shader.SubShaderSources[i].SPIRV.data()), static_cast<std::streamsize>(shader.SubShaderSources[i].SPIRV.size() * sizeof(uint32_t)));
        }
    }

    file.close();

    return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool LoadShader(const std::filesystem::path& filePath, Shader& outShader)
{
	if (!FileOrFolderExists(filePath))
	{
		std::cerr << "Couldn't find file or folder: \"" << filePath.u8string() << "\"\n";
		return false;
	}

	if (ToLower(GetSuffix(filePath.u8string())) == "shader")
	{
		std::optional<std::string> source = ReadTextFile(filePath);
		if (source)
		{
			std::string newPath = filePath.u8string();
			newPath = newPath.substr(0, newPath.size() - 7);
			outShader = Shader{newPath, *source, ShaderStage::None};
		}
		else
			return false;
	}
	else
	{
		std::cerr << "File: \"" << filePath.u8string() << "\" suffix is not \".shader\"!\n";
		return false;
	}

	return true;
}

#endif /*CONVERTTOSPIRV_SHADER_H*/