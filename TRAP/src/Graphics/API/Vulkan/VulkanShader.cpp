#include "TRAPPCH.h"
#include "VulkanShader.h"

#include "VulkanCommon.h"
#include "VulkanRenderer.h"
#include "Utils/String.h"
#include "Embed.h"

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanShader::s_glslangInitialized = false;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, const std::string& source)
	: m_name(std::move(name)),
	m_VShaderModule(nullptr),
	m_FShaderModule(nullptr),
	m_GShaderModule(nullptr),
	m_TCShaderModule(nullptr),
	m_TEShaderModule(nullptr),
	m_CShaderModule(nullptr),
	m_computeShaderStage()
{
	InitGLSL(source);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, std::vector<uint32_t>& source)
	: m_name(std::move(name)),
	  m_VShaderModule(nullptr),
	  m_FShaderModule(nullptr),
	  m_GShaderModule(nullptr),
	  m_TCShaderModule(nullptr),
	  m_TEShaderModule(nullptr),
	  m_CShaderModule(nullptr),
	  m_computeShaderStage()
{
	InitSPIRV(source);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource)
	: m_name(std::move(name)),
	  m_VShaderModule(nullptr),
	  m_FShaderModule(nullptr),
	  m_GShaderModule(nullptr),
	  m_TCShaderModule(nullptr),
	  m_TEShaderModule(nullptr),
	  m_CShaderModule(nullptr),
	  m_computeShaderStage()
{
	InitGLSL(std::move(VSSource), std::move(FSSource), std::move(GSSource), std::move(TCSSource), std::move(TESSource), std::move(CSSource));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::~VulkanShader()
{
	TP_DEBUG("[Shader][Vulkan] Destroying Shader: \"", m_name, "\"");
	Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Shutdown() const
{
	if (m_VShaderModule)
		vkDestroyShaderModule(VulkanRenderer::Get()->GetDevice(), m_VShaderModule, nullptr);
	if (m_FShaderModule)
		vkDestroyShaderModule(VulkanRenderer::Get()->GetDevice(), m_FShaderModule, nullptr);
	if (m_GShaderModule)
		vkDestroyShaderModule(VulkanRenderer::Get()->GetDevice(), m_GShaderModule, nullptr);
	if (m_TCShaderModule)
		vkDestroyShaderModule(VulkanRenderer::Get()->GetDevice(), m_TCShaderModule, nullptr);
	if (m_TEShaderModule)
		vkDestroyShaderModule(VulkanRenderer::Get()->GetDevice(), m_TEShaderModule, nullptr);
	if (m_CShaderModule)
		vkDestroyShaderModule(VulkanRenderer::Get()->GetDevice(), m_CShaderModule, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Bind() const
{
	if (s_CurrentlyBound != this)
	{
		if (!m_graphicsShaderStages.empty())
		{
			s_CurrentlyBound = this;
			VulkanRenderer::Get()->InitGraphicsPipeline(m_graphicsShaderStages);
		}
		else
			ShaderManager::Get("Passthrough")->Bind();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Unbind() const
{
	s_CurrentlyBound = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::API::VulkanShader::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::API::VulkanShader::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::InitSPIRV(std::vector<uint32_t>& source)
{
	std::array<std::vector<uint32_t>, 6> shaders{};
	uint32_t index = 0;
	const uint32_t SPIRVSubShaderCount = source[index++];

	for (uint32_t i = 0; i < SPIRVSubShaderCount; i++)
	{
		const uint32_t SPIRVSize = source[index++];
		uint32_t shaderType = source[index++];

		const std::vector<uint32_t> tempSPIRV = std::vector<uint32_t>(source.begin() + index, source.begin() + index + SPIRVSize);
		index += SPIRVSize;

		switch (static_cast<ShaderType>(shaderType))
		{
		case ShaderType::Vertex:
			shaders[0] = tempSPIRV;
			break;

		case ShaderType::Fragment:
			shaders[1] = tempSPIRV;
			break;

		case ShaderType::Geometry:
			shaders[2] = tempSPIRV;
			break;

		case ShaderType::Tessellation_Control:
			shaders[3] = tempSPIRV;
			break;

		case ShaderType::Tessellation_Evaluation:
			shaders[4] = tempSPIRV;
			break;

		case ShaderType::Compute:
			shaders[5] = tempSPIRV;
			break;

		default:
			break;
		}
	}

	VulkanShaderErrorInfo error;
	TP_DEBUG("[Shader][Vulkan][SPIR-V] Compiling: \"", m_name, "\"");
	CompileSPIRV(shaders, error);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::InitGLSL(const std::string& source)
{
	TP_DEBUG("[Shader][Vulkan][GLSL] Compiling: \"", m_name, "\"");
	
	std::array<std::string, 6> shaders{};
	if (!source.empty())
		PreProcessGLSL(source, shaders);

	VulkanShaderErrorInfo error{};
	
	bool isEmpty = true;
	for (const auto& shaderSource : shaders)
	{
		if (!shaderSource.empty())
		{
			isEmpty = false;
			break;
		}
	}
	if (isEmpty)
	{
		TP_WARN("[Shader][Vulkan][GLSL] Shader: \"", m_name, "\" using fallback Shader: \"Passthrough\"");
		return;
	}	
	
	CompileGLSL(shaders, error);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::InitGLSL(std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource)
{
	TP_DEBUG("[Shader][Vulkan][GLSL] Compiling: \"", m_name, "\"");
	
	std::array<std::string, 6> shaders{ std::move(VSSource),
	                                    std::move(FSSource),
	                                    std::move(GSSource),
	                                    std::move(TCSSource),
	                                    std::move(TESSource),
	                                    std::move(CSSource)
	};
	VulkanShaderErrorInfo error{};

	bool isEmpty = true;
	for (const auto& shaderSource : shaders)
	{
		if (!shaderSource.empty())
		{
			isEmpty = false;
			break;
		}
	}
	if (isEmpty)
	{
		TP_WARN("[Shader][Vulkan][GLSL] Shader: \"", m_name, "\" using fallback Shader: \"Passthrough\"");
		return;
	}
	
	CompileGLSL(shaders, error);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::CompileGLSL(std::array<std::string, 6> & shaders, VulkanShaderErrorInfo& info)
{
	if (!s_glslangInitialized)
	{
		glslang::InitializeProcess();
		s_glslangInitialized = true;
	}

	glslang::TProgram program;
	Scope<glslang::TShader> VShader = nullptr;
	Scope<glslang::TShader> FShader = nullptr;
	Scope<glslang::TShader> GShader = nullptr;
	Scope<glslang::TShader> TCShader = nullptr;
	Scope<glslang::TShader> TEShader = nullptr;
	Scope<glslang::TShader> CShader = nullptr;

	if (!shaders[0].empty())
	{
		const char* VSSource = shaders[0].c_str();
		TP_DEBUG("[Shader][Vulkan][GLSL] Pre-Processing Vertex Shader");
		std::string preProcessedSource;
		VShader = PreProcess(VSSource, 0, preProcessedSource);
		if (preProcessedSource.empty() || !VShader)
			return;
		const char* preProcessedCStr = preProcessedSource.c_str();
		VShader->setStrings(&preProcessedCStr, 1);

		TP_DEBUG("[Shader][Vulkan][GLSL] Parsing Vertex Shader");
		if (!Parse(VShader.get()))
			return;
	}

	if (!shaders[1].empty())
	{
		const char* FSSource = shaders[1].c_str();
		TP_DEBUG("[Shader][Vulkan][GLSL] Pre-Processing Fragment Shader");
		std::string preProcessedSource;
		FShader = PreProcess(FSSource, 1, preProcessedSource);
		if (preProcessedSource.empty())
			return;
		const char* preProcessedCStr = preProcessedSource.c_str();
		FShader->setStrings(&preProcessedCStr, 1);

		TP_DEBUG("[Shader][Vulkan][GLSL] Parsing Fragment Shader");
		if (!Parse(FShader.get()))
			return;
	}

	if (!shaders[2].empty())
	{
		const char* GSSource = shaders[2].c_str();
		TP_DEBUG("[Shader][Vulkan][GLSL] Pre-Processing Geometry Shader");
		std::string preProcessedSource;
		GShader = PreProcess(GSSource, 2, preProcessedSource);
		if (preProcessedSource.empty())
			return;
		const char* preProcessedCStr = preProcessedSource.c_str();
		GShader->setStrings(&preProcessedCStr, 1);

		TP_DEBUG("[Shader][Vulkan][GLSL] Parsing Geometry Shader");
		if (!Parse(GShader.get()))
			return;
	}

	if (!shaders[3].empty())
	{
		const char* TCSSource = shaders[3].c_str();
		TP_DEBUG("[Shader][Vulkan][GLSL] Pre-Processing Tessellation Control Shader");
		std::string preProcessedSource;
		TCShader = PreProcess(TCSSource, 3, preProcessedSource);
		if (preProcessedSource.empty())
			return;
		const char* preProcessedCStr = preProcessedSource.c_str();
		TCShader->setStrings(&preProcessedCStr, 1);

		TP_DEBUG("[Shader][Vulkan][GLSL] Parsing Tessellation Control Shader");
		if (!Parse(TCShader.get()))
			return;
	}

	if (!shaders[4].empty())
	{
		const char* TESSource = shaders[4].c_str();
		TP_DEBUG("[Shader][Vulkan][GLSL] Pre-Processing Tessellation Evaluation Shader");
		std::string preProcessedSource;
		TEShader = PreProcess(TESSource, 4, preProcessedSource);
		if (preProcessedSource.empty())
			return;
		const char* preProcessedCStr = preProcessedSource.c_str();
		TEShader->setStrings(&preProcessedCStr, 1);

		TP_DEBUG("[Shader][Vulkan][GLSL] Parsing Tessellation Evaluation Shader");
		if (!Parse(TEShader.get()))
			return;
	}

	if (!shaders[5].empty())
	{
		const char* CSSource = shaders[5].c_str();
		TP_DEBUG("[Shader][Vulkan][GLSL] Pre-Processing Compute Shader");
		std::string preProcessedSource;
		CShader = PreProcess(CSSource, 5, preProcessedSource);
		if (preProcessedSource.empty())
			return;
		const char* preProcessedCStr = preProcessedSource.c_str();
		CShader->setStrings(&preProcessedCStr, 1);

		TP_DEBUG("[Shader][Vulkan][GLSL] Parsing Compute Shader");
		if (!Parse(CShader.get()))
			return;
	}

	TP_DEBUG("[Shader][Vulkan][GLSL] Linking Shaders");
	if (!Link(VShader.get(), FShader.get(), GShader.get(), TCShader.get(), TEShader.get(), CShader.get(), program))
		return;

	TP_DEBUG("[Shader][Vulkan][SPIR-V] Converting Shaders to SPIRV");
	std::vector<std::vector<uint32_t>> SPIRV = ConvertToSPIRV(VShader.get(), FShader.get(), GShader.get(), TCShader.get(), TEShader.get(), CShader.get(), program);

	if (!SPIRV[0].empty())
	{
		if (!CreateShaderModule(m_VShaderModule, SPIRV[0]))
			return;

		VkPipelineShaderStageCreateInfo shaderStageInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_VERTEX_BIT,
			m_VShaderModule,
			"main",
			nullptr
		};
		m_graphicsShaderStages.push_back(shaderStageInfo);
	}
	if(!SPIRV[1].empty())
	{
		if (!CreateShaderModule(m_FShaderModule, SPIRV[1]))
			return;

		VkPipelineShaderStageCreateInfo shaderStageInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			m_FShaderModule,
			"main",
			nullptr
		};
		m_graphicsShaderStages.push_back(shaderStageInfo);
	}
	if(!SPIRV[2].empty())
	{
		if (!CreateShaderModule(m_GShaderModule, SPIRV[2]))
			return;

		VkPipelineShaderStageCreateInfo shaderStageInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_GEOMETRY_BIT,
			m_GShaderModule,
			"main",
			nullptr
		};
		m_graphicsShaderStages.push_back(shaderStageInfo);
	}
	if(!SPIRV[3].empty())
	{
		if (!CreateShaderModule(m_TCShaderModule, SPIRV[3]))
			return;

		VkPipelineShaderStageCreateInfo shaderStageInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
			m_TCShaderModule,
			"main",
			nullptr
		};
		m_graphicsShaderStages.push_back(shaderStageInfo);
	}
	if(!SPIRV[4].empty())
	{
		if (!CreateShaderModule(m_TEShaderModule, SPIRV[4]))
			return;

		VkPipelineShaderStageCreateInfo shaderStageInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
			m_TEShaderModule,
			"main",
			nullptr
		};
		m_graphicsShaderStages.push_back(shaderStageInfo);
	}
	if (!SPIRV[5].empty())
	{
		if (!CreateShaderModule(m_CShaderModule, SPIRV[5]))
			return;

		VkPipelineShaderStageCreateInfo shaderStageInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_COMPUTE_BIT,
			m_CShaderModule,
			"main",
			nullptr
		};
		m_computeShaderStage = shaderStageInfo;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::CompileSPIRV(std::array<std::vector<uint32_t>, 6>& shaders, VulkanShaderErrorInfo& info)
{
	if (!shaders[0].empty())
	{
		TP_DEBUG("[Shader][Vulkan][SPIR-V] Loading Vertex Shader");
		if (!CreateShaderModule(m_VShaderModule, shaders[0]))
			return;

		const VkPipelineShaderStageCreateInfo shaderStageInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_VERTEX_BIT,
			m_VShaderModule,
			"main",
			nullptr
		};
		m_graphicsShaderStages.push_back(shaderStageInfo);
	}
	if (!shaders[1].empty())
	{
		TP_DEBUG("[Shader][Vulkan][SPIR-V] Loading Fragment Shader");
		if (!CreateShaderModule(m_FShaderModule, shaders[1]))
			return;

		const VkPipelineShaderStageCreateInfo shaderStageInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			m_FShaderModule,
			"main",
			nullptr
		};
		m_graphicsShaderStages.push_back(shaderStageInfo);
	}
	if (!shaders[2].empty())
	{
		TP_DEBUG("[Shader][Vulkan][SPIR-V] Loading Geometry Shader");
		if (!CreateShaderModule(m_GShaderModule, shaders[2]))
			return;

		const VkPipelineShaderStageCreateInfo shaderStageInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_GEOMETRY_BIT,
			m_GShaderModule,
			"main",
			nullptr
		};
		m_graphicsShaderStages.push_back(shaderStageInfo);
	}
	if (!shaders[3].empty())
	{
		TP_DEBUG("[Shader][Vulkan][SPIR-V] Loading Tessellation Control Shader");
		if (!CreateShaderModule(m_TCShaderModule, shaders[3]))
			return;

		const VkPipelineShaderStageCreateInfo shaderStageInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
			m_TCShaderModule,
			"main",
			nullptr
		};
		m_graphicsShaderStages.push_back(shaderStageInfo);
	}
	if (!shaders[4].empty())
	{
		TP_DEBUG("[Shader][Vulkan][SPIR-V] Loading Tessellation Evaluation Shader");
		if (!CreateShaderModule(m_TEShaderModule, shaders[4]))
			return;

		const VkPipelineShaderStageCreateInfo shaderStageInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
			m_TEShaderModule,
			"main",
			nullptr
		};
		m_graphicsShaderStages.push_back(shaderStageInfo);
	}
	if (!shaders[5].empty())
	{
		TP_DEBUG("[Shader][Vulkan][SPIR-V] Loading Compute Shader");
		if (!CreateShaderModule(m_CShaderModule, shaders[5]))
			return;

		const VkPipelineShaderStageCreateInfo shaderStageInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_COMPUTE_BIT,
			m_CShaderModule,
			"main",
			nullptr
		};
		m_computeShaderStage = shaderStageInfo;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::PreProcessGLSL(const std::string& source, std::array<std::string, 6>& shaders)
{
	ShaderType type = ShaderType::Unknown;

	std::vector<std::string> lines = Utils::String::GetLines(source);
	//Get Shader Type
	for (uint32_t i = 0; i < lines.size(); i++)
	{
		if (Utils::String::StartsWith(Utils::String::ToLower(lines[i]), "#shader"))
		{
			if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "vertex"))
				type = ShaderType::Vertex;
			else if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "fragment"))
				type = ShaderType::Fragment;
			else if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "geometry"))
				type = ShaderType::Geometry;
			else if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "tessellation"))
			{
				if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "control"))
					type = ShaderType::Tessellation_Control;
				else if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "evaluation"))
					type = ShaderType::Tessellation_Evaluation;
			}
			else if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "compute"))
				type = ShaderType::Compute;
		}
		else if (Utils::String::StartsWith(Utils::String::ToLower(lines[i]), "#version"))
		{
			TP_WARN("[Shader][OpenGL][GLSL] Found Tag: \"", lines[i], "\" this is unnecessary! Skipping Line: ", i);
		}
		else if (type != ShaderType::Unknown)
		{
			shaders[static_cast<int32_t>(type) - 1].append(lines[i]);
			shaders[static_cast<int32_t>(type) - 1].append("\n");
		}
	}

	for (uint32_t i = 0; i < shaders.size(); i++)
	{
		if (!shaders[i].empty())
		{
			if (Utils::String::ToLower(shaders[i]).find("main") == std::string::npos)
			{
				TP_ERROR("[Shader][OpenGL][GLSL] ", ShaderTypeToString(static_cast<ShaderType>(i + 1)), ": Couldn't find \"main\" function!");
				shaders[i] = "";
			}
			else //Found main function
				shaders[i] = "#version 460 core\n" + shaders[i];
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<glslang::TShader> TRAP::Graphics::API::VulkanShader::PreProcess(const char* source, const uint32_t shaderType, std::string& preProcessedSource)
{
	Scope<glslang::TShader> shader;
	if (shaderType == 0)
	{
		shader = MakeScope<glslang::TShader>(EShLangVertex);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangVertex, glslang::EShClientVulkan, 460);
	}
	else if (shaderType == 1)
	{
		shader = MakeScope<glslang::TShader>(EShLangFragment);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangFragment, glslang::EShClientVulkan, 460);
	}
	else if (shaderType == 2)
	{
		shader = MakeScope<glslang::TShader>(EShLangGeometry);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangGeometry, glslang::EShClientVulkan, 460);
	}
	else if (shaderType == 3)
	{
		shader = MakeScope<glslang::TShader>(EShLangTessControl);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangTessControl, glslang::EShClientVulkan, 460);
	}
	else if (shaderType == 4)
	{
		shader = MakeScope<glslang::TShader>(EShLangTessEvaluation);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangTessEvaluation, glslang::EShClientVulkan, 460);
	}
	else if (shaderType == 5)
	{
		shader = MakeScope<glslang::TShader>(EShLangCompute);
		shader->setStrings(&source, 1);
		shader->setEnvInput(glslang::EShSourceGlsl, EShLangCompute, glslang::EShClientVulkan, 460);
	}

	shader->setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1);
	shader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3); //TODO Update to SPIRV 1.4
	DirStackFileIncluder includer;
	if (!shader->preprocess(&glslang::DefaultTBuiltInResource,
	                        460,
	                        ECoreProfile,
	                        true,
	                        true,
	                        static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules),
	                        &preProcessedSource,
	                        includer))
	{
		TP_ERROR("[Shader}[Vulkan][GLSL] Preprocessing failed!");
		TP_ERROR("[Shader][Vulkan][GLSL] ", shader->getInfoLog());
		TP_ERROR("[Shader][Vulkan][GLSL] ", shader->getInfoDebugLog());

		return nullptr;
	}

	return shader;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanShader::Parse(glslang::TShader* shader)
{
	if (!shader->parse(&glslang::DefaultTBuiltInResource, 460, true, static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules)))
	{
		TP_ERROR("[Shader][Vulkan][GLSL] Parsing failed: ");
		TP_ERROR("[Shader][Vulkan][GLSL] ", shader->getInfoLog());
		TP_ERROR("[Shader][Vulkan][GLSL] ", shader->getInfoDebugLog());

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanShader::Link(glslang::TShader* VShader,
                                             glslang::TShader* FShader,
                                             glslang::TShader* GShader,
                                             glslang::TShader* TCShader,
                                             glslang::TShader* TEShader,
                                             glslang::TShader* CShader,
                                             glslang::TProgram& program)
{
	if(VShader)
		program.addShader(VShader);
	if (FShader)
		program.addShader(FShader);
	if (GShader)
		program.addShader(GShader);
	if (TCShader)
		program.addShader(TCShader);
	if (TEShader)
		program.addShader(TEShader);
	if (CShader)
		program.addShader(CShader);

	if (!program.link(static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules)))
	{
		TP_ERROR("[Shader][Vulkan][SPIR-V] Linking failed: ");
		TP_ERROR("[Shader][Vulkan][SPIR-V] ", program.getInfoLog());
		TP_ERROR("[Shader][Vulkan][SPIR-V] ", program.getInfoDebugLog());

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::vector<uint32_t>> TRAP::Graphics::API::VulkanShader::ConvertToSPIRV(
										glslang::TShader* VShader,
										glslang::TShader* FShader,
										glslang::TShader* GShader,
										glslang::TShader* TCShader,
										glslang::TShader* TEShader,
										glslang::TShader* CShader,
										glslang::TProgram& program)
{
	std::vector<std::vector<uint32_t>> SPIRV(6);

	if(VShader)
	{
		spv::SpvBuildLogger logger{};
		glslang::SpvOptions spvOptions{};
		GlslangToSpv(*program.getIntermediate(EShLangVertex), SPIRV[0], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			TP_ERROR("[Shader][Vulkan][SPIR-V] Vertex Shader: ", logger.getAllMessages());
	}

	if (FShader)
	{
		spv::SpvBuildLogger logger{};
		glslang::SpvOptions spvOptions{};
		GlslangToSpv(*program.getIntermediate(EShLangFragment), SPIRV[1], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			TP_ERROR("[Shader][Vulkan][SPIR-V] Fragment Shader: ", logger.getAllMessages());
	}

	if (GShader)
	{
		spv::SpvBuildLogger logger{};
		glslang::SpvOptions spvOptions{};
		GlslangToSpv(*program.getIntermediate(EShLangGeometry), SPIRV[2], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			TP_ERROR("[Shader][Vulkan][SPIR-V] Geometry Shader: ", logger.getAllMessages());
	}

	if (TCShader)
	{
		spv::SpvBuildLogger logger{};
		glslang::SpvOptions spvOptions{};
		GlslangToSpv(*program.getIntermediate(EShLangTessControl), SPIRV[3], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			TP_ERROR("[Shader][Vulkan][SPIR-V] Tessellation Control Shader: ", logger.getAllMessages());
	}

	if (TEShader)
	{
		spv::SpvBuildLogger logger{};
		glslang::SpvOptions spvOptions{};
		GlslangToSpv(*program.getIntermediate(EShLangTessEvaluation), SPIRV[4], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			TP_ERROR("[Shader][Vulkan][SPIR-V] Tessellation Evaluation Shader: ", logger.getAllMessages());
	}

	if (CShader)
	{
		spv::SpvBuildLogger logger{};
		glslang::SpvOptions spvOptions{};
		GlslangToSpv(*program.getIntermediate(EShLangCompute), SPIRV[5], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			TP_ERROR("[Shader][Vulkan][SPIR-V] Compute Shader: ", logger.getAllMessages());
	}

	return SPIRV;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanShader::CreateShaderModule(VkShaderModule& shaderModule, std::vector<uint32_t>& SPIRVCode)
{
	VkShaderModuleCreateInfo shaderModuleCreateInfo
	{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		SPIRVCode.size() * sizeof(uint32_t),
		SPIRVCode.data()
	};

	VkResult success;
	VkCall(success = vkCreateShaderModule(Graphics::API::VulkanRenderer::Get()->GetDevice(), &shaderModuleCreateInfo, nullptr, &shaderModule));
	
	return success == VK_SUCCESS;
}