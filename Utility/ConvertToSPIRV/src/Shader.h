#ifndef CONVERTTOSPIRV_SHADER_H
#define CONVERTTOSPIRV_SHADER_H

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

inline constexpr TBuiltInResource DefaultTBuiltInResource =
{
	/* .MaxLights = */ 32,
	/* .MaxClipPlanes = */ 6,
	/* .MaxTextureUnits = */ 32,
	/* .MaxTextureCoords = */ 32,
	/* .MaxVertexAttribs = */ 64,
	/* .MaxVertexUniformComponents = */ 4096,
	/* .MaxVaryingFloats = */ 64,
	/* .MaxVertexTextureImageUnits = */ 32,
	/* .MaxCombinedTextureImageUnits = */ 80,
	/* .MaxTextureImageUnits = */ 32,
	/* .MaxFragmentUniformComponents = */ 4096,
	/* .MaxDrawBuffers = */ 32,
	/* .MaxVertexUniformVectors = */ 128,
	/* .MaxVaryingVectors = */ 8,
	/* .MaxFragmentUniformVectors = */ 16,
	/* .MaxVertexOutputVectors = */ 16,
	/* .MaxFragmentInputVectors = */ 15,
	/* .MinProgramTexelOffset = */ -8,
	/* .MaxProgramTexelOffset = */ 7,
	/* .MaxClipDistances = */ 8,
	/* .MaxComputeWorkGroupCountX = */ 65535,
	/* .MaxComputeWorkGroupCountY = */ 65535,
	/* .MaxComputeWorkGroupCountZ = */ 65535,
	/* .MaxComputeWorkGroupSizeX = */ 1024,
	/* .MaxComputeWorkGroupSizeY = */ 1024,
	/* .MaxComputeWorkGroupSizeZ = */ 64,
	/* .MaxComputeUniformComponents = */ 1024,
	/* .MaxComputeTextureImageUnits = */ 16,
	/* .MaxComputeImageUniforms = */ 8,
	/* .MaxComputeAtomicCounters = */ 8,
	/* .MaxComputeAtomicCounterBuffers = */ 1,
	/* .MaxVaryingComponents = */ 60,
	/* .MaxVertexOutputComponents = */ 64,
	/* .MaxGeometryInputComponents = */ 64,
	/* .MaxGeometryOutputComponents = */ 128,
	/* .MaxFragmentInputComponents = */ 128,
	/* .MaxImageUnits = */ 8,
	/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
	/* .MaxCombinedShaderOutputResources = */ 8,
	/* .MaxImageSamples = */ 0,
	/* .MaxVertexImageUniforms = */ 0,
	/* .MaxTessControlImageUniforms = */ 0,
	/* .MaxTessEvaluationImageUniforms = */ 0,
	/* .MaxGeometryImageUniforms = */ 0,
	/* .MaxFragmentImageUniforms = */ 8,
	/* .MaxCombinedImageUniforms = */ 8,
	/* .MaxGeometryTextureImageUnits = */ 16,
	/* .MaxGeometryOutputVertices = */ 256,
	/* .MaxGeometryTotalOutputComponents = */ 1024,
	/* .MaxGeometryUniformComponents = */ 1024,
	/* .MaxGeometryVaryingComponents = */ 64,
	/* .MaxTessControlInputComponents = */ 128,
	/* .MaxTessControlOutputComponents = */ 128,
	/* .MaxTessControlTextureImageUnits = */ 16,
	/* .MaxTessControlUniformComponents = */ 1024,
	/* .MaxTessControlTotalOutputComponents = */ 4096,
	/* .MaxTessEvaluationInputComponents = */ 128,
	/* .MaxTessEvaluationOutputComponents = */ 128,
	/* .MaxTessEvaluationTextureImageUnits = */ 16,
	/* .MaxTessEvaluationUniformComponents = */ 1024,
	/* .MaxTessPatchComponents = */ 120,
	/* .MaxPatchVertices = */ 32,
	/* .MaxTessGenLevel = */ 64,
	/* .MaxViewports = */ 16,
	/* .MaxVertexAtomicCounters = */ 0,
	/* .MaxTessControlAtomicCounters = */ 0,
	/* .MaxTessEvaluationAtomicCounters = */ 0,
	/* .MaxGeometryAtomicCounters = */ 0,
	/* .MaxFragmentAtomicCounters = */ 8,
	/* .MaxCombinedAtomicCounters = */ 8,
	/* .MaxAtomicCounterBindings = */ 1,
	/* .MaxVertexAtomicCounterBuffers = */ 0,
	/* .MaxTessControlAtomicCounterBuffers = */ 0,
	/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
	/* .MaxGeometryAtomicCounterBuffers = */ 0,
	/* .MaxFragmentAtomicCounterBuffers = */ 1,
	/* .MaxCombinedAtomicCounterBuffers = */ 1,
	/* .MaxAtomicCounterBufferSize = */ 16384,
	/* .MaxTransformFeedbackBuffers = */ 4,
	/* .MaxTransformFeedbackInterleavedComponents = */ 64,
	/* .MaxCullDistances = */ 8,
	/* .MaxCombinedClipAndCullDistances = */ 8,
	/* .MaxSamples = */ 4,
	/* .maxMeshOutputVerticesNV = */ 256,
	/* .maxMeshOutputPrimitivesNV = */ 512,
	/* .maxMeshWorkGroupSizeX_NV = */ 32,
	/* .maxMeshWorkGroupSizeY_NV = */ 1,
	/* .maxMeshWorkGroupSizeZ_NV = */ 1,
	/* .maxTaskWorkGroupSizeX_NV = */ 32,
	/* .maxTaskWorkGroupSizeY_NV = */ 1,
	/* .maxTaskWorkGroupSizeZ_NV = */ 1,
	/* .maxMeshViewCountNV = */ 4,
	/* .maxDualSourceDrawBuffersEXT = */ 1,

	{
		/* TLimits */
		/* .nonInductiveForLoops = */ true,
		/* .whileLoops = */ true,
		/* .doWhileLoops = */ true,
		/* .generalUniformIndexing = */ true,
		/* .generalAttributeMatrixVectorIndexing = */ true,
		/* .generalVaryingIndexing = */ true,
		/* .generalSamplerIndexing = */ true,
		/* .generalVariableIndexing = */ true,
		/* .generalConstantMatrixVectorIndexing = */ true
	}
};

//-------------------------------------------------------------------------------------------------------------------//

static bool s_glslangInitialized = false;

//-------------------------------------------------------------------------------------------------------------------//

enum class ShaderStage
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

static inline ShaderStage operator|(ShaderStage a, ShaderStage b) { return static_cast<ShaderStage>(static_cast<std::underlying_type<ShaderStage>::type>(a) |
																		                            static_cast<std::underlying_type<ShaderStage>::type>(b)); }
static inline ShaderStage operator&(ShaderStage a, ShaderStage b) { return static_cast<ShaderStage>(static_cast<std::underlying_type<ShaderStage>::type>(a) &
																		                            static_cast<std::underlying_type<ShaderStage>::type>(b)); }
static inline ShaderStage operator|=(ShaderStage& a, ShaderStage b) { return a = (a | b); }
// static inline ShaderStage operator&=(ShaderStage& a, ShaderStage b) { return a = (a & b); }

//-------------------------------------------------------------------------------------------------------------------//

struct Shader
{
	std::string FilePath;
	std::string Source;
	ShaderStage Stages;

    struct SubShader
    {
        std::string Source;
        std::vector<uint32_t> SPIRV{};;
    };

	std::array<SubShader, static_cast<uint32_t>(ShaderStage::SHADER_STAGE_COUNT)> SubShaderSources{};
};

//-------------------------------------------------------------------------------------------------------------------//

static const std::unordered_map<ShaderStage, std::string> ShaderStageToString
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

inline static const std::unordered_map<ShaderStage, uint32_t> ShaderStageToIndex
{
    { ShaderStage::Vertex, 0 },
    { ShaderStage::TessellationControl, 1 },
    { ShaderStage::TessellationEvaluation, 2 },
    { ShaderStage::Geometry, 3 },
    { ShaderStage::Fragment, 4 },
    { ShaderStage::Compute, 5 }
};

//-------------------------------------------------------------------------------------------------------------------//

inline static const std::unordered_map<ShaderStage, EShLanguage> ShaderStageToEShLanguage
{
    { ShaderStage::Vertex, EShLangVertex },
    { ShaderStage::TessellationControl, EShLangTessControl },
    { ShaderStage::TessellationEvaluation, EShLangTessEvaluation },
    { ShaderStage::Geometry, EShLangGeometry },
    { ShaderStage::Fragment, EShLangFragment },
    { ShaderStage::Compute, EShLangCompute }
};

//-------------------------------------------------------------------------------------------------------------------//

inline ShaderStage DetectShaderStage(const std::string_view identifyString)
{
    static const std::unordered_map<std::string, ShaderStage> stages
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

inline bool FindEntryPoint(const std::string& shaderStr)
{
    return ToLower(shaderStr).find("main") != std::string::npos;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool ValidateShaderStages(const Shader& shader)
{
	const ShaderStage stages = shader.Stages;

	//Check if any Shader Stage is set
	if(ShaderStage::None == stages)
	{
		std::cerr << "[GLSL] No Shader Stage found!" << '\n';
		return false;
	}

	//Check for "Normal" Shader Stages combined with Compute
	if ((static_cast<uint32_t>(ShaderStage::Vertex                 & stages) ||
		 static_cast<uint32_t>(ShaderStage::Fragment               & stages) ||
		 static_cast<uint32_t>(ShaderStage::TessellationControl    & stages) ||
		 static_cast<uint32_t>(ShaderStage::TessellationEvaluation & stages) ||
		 static_cast<uint32_t>(ShaderStage::Geometry               & stages)) &&
		 static_cast<uint32_t>(ShaderStage::Compute                & stages))
	{
		std::cerr << "[GLSL] Rasterizer Shader Stages combined with Compute stage!" << '\n';
		return false;
	}

	//Check for Vertex Shader Stage & required Fragment/Pixel Shader Stage
	if (  static_cast<uint32_t>(ShaderStage::Vertex   & stages) &&
		!(static_cast<uint32_t>(ShaderStage::Fragment & stages)))
	{
		std::cerr << "[GLSL] Only Vertex Shader Stage provided! Missing Fragment/Pixel Shader Stage" << '\n';
		return false;
	}

	//Shader Stages should be valid
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool PreProcessGLSL(Shader& shader, const std::vector<std::array<std::string, 2>>& customMacros)
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
			if (static_cast<uint32_t>(shader.Stages & currentShaderStage))
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
           static_cast<uint32_t>(IndexToShaderStage.at(i) & shader.Stages))
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
				shader.SubShaderSources[i].Source += "#define " + macro[0] + " " + macro[1] + '\n';
            for(const auto& macro : customMacros)
                shader.SubShaderSources[i].Source += "#define " + macro[0] + " " + macro[1] + '\n';
			shader.SubShaderSources[i].Source += tmp;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::unique_ptr<glslang::TShader> PreProcessGLSLForConversion(const char* source, const ShaderStage stage,
                                                                     std::string& preProcessedSource)
{
	std::unique_ptr<glslang::TShader> shader = nullptr;

	shader = std::make_unique<glslang::TShader>(ShaderStageToEShLanguage.at(stage));
	shader->setStrings(&source, 1);
	shader->setEnvInput(glslang::EShSourceGlsl, ShaderStageToEShLanguage.at(stage), glslang::EShClientVulkan, 460);

	shader->setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1);
	shader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3);
	glslang::TShader::ForbidIncluder includer;
	if (!shader->preprocess(&DefaultTBuiltInResource,
		460,
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

inline bool ParseGLSL(glslang::TShader* shader)
{
	if (!shader->parse(&DefaultTBuiltInResource, 460, true,
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

inline bool LinkGLSL(glslang::TShader* shader, glslang::TProgram& program)
{
	if (shader)
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

inline std::vector<uint32_t> ConvertToSPIRV(glslang::TShader* shader, const ShaderStage stage,
                                            glslang::TProgram& program)
{
	std::vector<uint32_t> SPIRV{};

    if(!shader)
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

inline bool CompileGLSLToSPIRV(Shader& shader)
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

inline bool SaveSPIRV(Shader& shader, std::filesystem::path customOutput)
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

inline bool LoadShader(const std::filesystem::path& filePath, Shader& outShader)
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