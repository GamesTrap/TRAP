#include "TRAPPCH.h"
#include "VulkanShader.h"

#include "VulkanCommon.h"
#include "VulkanRenderer.h"
#include "Utils/String/String.h"
#include "Embed.h"
#include "Internals/Objects/VulkanDevice.h"
#include "Internals/Objects/VulkanSwapchain.h"
#include "Internals/Objects/VulkanPipeline.h"
#include "Graphics/Shaders/ShaderManager.h"

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanShader::s_glslangInitialized = false;
const std::map<spv::ExecutionModel, VkShaderStageFlagBits> s_model2Stage =
{
	{spv::ExecutionModelVertex, VK_SHADER_STAGE_VERTEX_BIT},
	{spv::ExecutionModelFragment, VK_SHADER_STAGE_FRAGMENT_BIT},
	{spv::ExecutionModelGeometry, VK_SHADER_STAGE_GEOMETRY_BIT},
	{spv::ExecutionModelTessellationControl, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT},
	{spv::ExecutionModelTessellationEvaluation, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT},
	{spv::ExecutionModelGLCompute, VK_SHADER_STAGE_COMPUTE_BIT}
};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, const std::string_view source)
	: m_VShaderModule(nullptr),
	m_FShaderModule(nullptr),
	m_GShaderModule(nullptr),
	m_TCShaderModule(nullptr),
	m_TEShaderModule(nullptr),
	m_CShaderModule(nullptr),
	m_computeShaderStage(),
	m_stride(0)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	
	InitGLSL(source);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name, const std::vector<uint32_t>& source)
	: m_VShaderModule(nullptr),
	  m_FShaderModule(nullptr),
	  m_GShaderModule(nullptr),
	  m_TCShaderModule(nullptr),
	  m_TEShaderModule(nullptr),
	  m_CShaderModule(nullptr),
	  m_computeShaderStage(),
	  m_stride(0)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	
	InitSPIRV(source);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::VulkanShader(std::string name,
                                                const std::string_view VSSource,
                                                const std::string_view FSSource,
                                                const std::string_view GSSource,
                                                const std::string_view TCSSource,
                                                const std::string_view TESSource,
                                                const std::string_view CSSource)
	: m_VShaderModule(nullptr),
	  m_FShaderModule(nullptr),
	  m_GShaderModule(nullptr),
	  m_TCShaderModule(nullptr),
	  m_TEShaderModule(nullptr),
	  m_CShaderModule(nullptr),
	  m_computeShaderStage(),
	  m_stride(0)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	
	InitGLSL(VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanShader::~VulkanShader()
{
	TP_PROFILE_FUNCTION();
	
	TP_DEBUG(Log::ShaderVulkanPrefix, "Destroying Shader: \"", m_name, "\"");
	Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Shutdown() const
{
	TP_PROFILE_FUNCTION();

	if (m_VShaderModule)
		vkDestroyShaderModule(VulkanRenderer::GetDevice().GetDevice(), m_VShaderModule, nullptr);
	if (m_FShaderModule)
		vkDestroyShaderModule(VulkanRenderer::GetDevice().GetDevice(), m_FShaderModule, nullptr);
	if (m_GShaderModule)
		vkDestroyShaderModule(VulkanRenderer::GetDevice().GetDevice(), m_GShaderModule, nullptr);
	if (m_TCShaderModule)
		vkDestroyShaderModule(VulkanRenderer::GetDevice().GetDevice(), m_TCShaderModule, nullptr);
	if (m_TEShaderModule)
		vkDestroyShaderModule(VulkanRenderer::GetDevice().GetDevice(), m_TEShaderModule, nullptr);
	if (m_CShaderModule)
		vkDestroyShaderModule(VulkanRenderer::GetDevice().GetDevice(), m_CShaderModule, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Bind() const
{
	TP_PROFILE_FUNCTION();
	
	bool isSameShader = true;
	auto currentShaders = VulkanRenderer::GetCurrentSwapchain().GetPipeline().GetShaders();
	if (currentShaders.size() == m_graphicsShaderStages.size())
	{
		for (uint32_t i = 0; i < m_graphicsShaderStages.size(); i++)
		{
			if (m_graphicsShaderStages[i].module != currentShaders[i].module)
			{
				isSameShader = false;
				break;
			}
		}
	}
	else
		isSameShader = false;

	if(isSameShader)
		return;

	if (!m_graphicsShaderStages.empty())
		VulkanRenderer::GetCurrentSwapchain().GetPipeline().SetShaders(m_graphicsShaderStages, m_attributeDescriptions, m_stride);
	else
		ShaderManager::Get("Fallback")->Bind();

	VulkanRenderer::BindGraphicsPipeline();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Unbind() const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::InitSPIRV(const std::vector<uint32_t>& source)
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
	TP_DEBUG(Log::ShaderVulkanSPIRVPrefix, "Loading: \"", m_name, "\"");
	LoadSPIRV(shaders);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::InitGLSL(const std::string_view source)
{
	TP_DEBUG(Log::ShaderVulkanGLSLPrefix, "Compiling: \"", m_name, "\"");
	
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
		TP_WARN(Log::ShaderVulkanGLSLPrefix, "Shader: \"", m_name, "\" using fallback Shader: \"Fallback\"");
		return;
	}	
	
	CompileGLSL({shaders[0], shaders[1], shaders[2], shaders[3], shaders[4], shaders[5]});
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::InitGLSL(const std::string_view VSSource,
                                                 const std::string_view FSSource,
                                                 const std::string_view GSSource,
                                                 const std::string_view TCSSource,
                                                 const std::string_view TESSource,
                                                 const std::string_view CSSource)
{
	TP_DEBUG(Log::ShaderVulkanGLSLPrefix, "Compiling: \"", m_name, "\"");
	
	std::array<std::string_view, 6> shaders{ VSSource,
	                                    FSSource,
	                                    GSSource,
	                                    TCSSource,
	                                    TESSource,
	                                    CSSource
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
		TP_WARN(Log::ShaderVulkanGLSLPrefix, "Shader: \"", m_name, "\" using fallback Shader: \"Fallback\"");
		return;
	}
	
	CompileGLSL(shaders);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::CompileGLSL(const std::array<std::string_view, 6>& shaders)
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
		const char* VSSource = shaders[0].data();
		TP_DEBUG(Log::ShaderVulkanGLSLPrefix, "Pre-Processing Vertex Shader");
		std::string preProcessedSource;
		VShader = PreProcess(VSSource, 0, preProcessedSource);
		if (preProcessedSource.empty() || !VShader)
			return;
		const char* preProcessedCStr = preProcessedSource.c_str();
		VShader->setStrings(&preProcessedCStr, 1);

		TP_DEBUG(Log::ShaderVulkanGLSLPrefix, "Parsing Vertex Shader");
		if (!Parse(VShader.get()))
			return;
	}

	if (!shaders[1].empty())
	{
		const char* FSSource = shaders[1].data();
		TP_DEBUG(Log::ShaderVulkanGLSLPrefix, "Pre-Processing Fragment Shader");
		std::string preProcessedSource;
		FShader = PreProcess(FSSource, 1, preProcessedSource);
		if (preProcessedSource.empty())
			return;
		const char* preProcessedCStr = preProcessedSource.c_str();
		FShader->setStrings(&preProcessedCStr, 1);

		TP_DEBUG(Log::ShaderVulkanGLSLPrefix, "Parsing Fragment Shader");
		if (!Parse(FShader.get()))
			return;
	}

	if (!shaders[2].empty())
	{
		const char* GSSource = shaders[2].data();
		TP_DEBUG(Log::ShaderVulkanGLSLPrefix, "Pre-Processing Geometry Shader");
		std::string preProcessedSource;
		GShader = PreProcess(GSSource, 2, preProcessedSource);
		if (preProcessedSource.empty())
			return;
		const char* preProcessedCStr = preProcessedSource.c_str();
		GShader->setStrings(&preProcessedCStr, 1);

		TP_DEBUG(Log::ShaderVulkanGLSLPrefix, "Parsing Geometry Shader");
		if (!Parse(GShader.get()))
			return;
	}

	if (!shaders[3].empty())
	{
		const char* TCSSource = shaders[3].data();
		TP_DEBUG(Log::ShaderVulkanGLSLPrefix, "Pre-Processing Tessellation Control Shader");
		std::string preProcessedSource;
		TCShader = PreProcess(TCSSource, 3, preProcessedSource);
		if (preProcessedSource.empty())
			return;
		const char* preProcessedCStr = preProcessedSource.c_str();
		TCShader->setStrings(&preProcessedCStr, 1);

		TP_DEBUG(Log::ShaderVulkanGLSLPrefix, "Parsing Tessellation Control Shader");
		if (!Parse(TCShader.get()))
			return;
	}

	if (!shaders[4].empty())
	{
		const char* TESSource = shaders[4].data();
		TP_DEBUG(Log::ShaderVulkanGLSLPrefix, "Pre-Processing Tessellation Evaluation Shader");
		std::string preProcessedSource;
		TEShader = PreProcess(TESSource, 4, preProcessedSource);
		if (preProcessedSource.empty())
			return;
		const char* preProcessedCStr = preProcessedSource.c_str();
		TEShader->setStrings(&preProcessedCStr, 1);

		TP_DEBUG(Log::ShaderVulkanGLSLPrefix, "Parsing Tessellation Evaluation Shader");
		if (!Parse(TEShader.get()))
			return;
	}

	if (!shaders[5].empty())
	{
		const char* CSSource = shaders[5].data();
		TP_DEBUG(Log::ShaderVulkanGLSLPrefix, "Pre-Processing Compute Shader");
		std::string preProcessedSource;
		CShader = PreProcess(CSSource, 5, preProcessedSource);
		if (preProcessedSource.empty())
			return;
		const char* preProcessedCStr = preProcessedSource.c_str();
		CShader->setStrings(&preProcessedCStr, 1);

		TP_DEBUG(Log::ShaderVulkanGLSLPrefix, "Parsing Compute Shader");
		if (!Parse(CShader.get()))
			return;
	}

	TP_DEBUG(Log::ShaderVulkanGLSLPrefix, "Linking Shaders");
	if (!Link(VShader.get(), FShader.get(), GShader.get(), TCShader.get(), TEShader.get(), CShader.get(), program))
		return;

	TP_DEBUG(Log::ShaderVulkanSPIRVPrefix, "Converting Shaders to SPIRV");
	std::vector<std::vector<uint32_t>> SPIRV = ConvertToSPIRV(VShader.get(), FShader.get(), GShader.get(), TCShader.get(), TEShader.get(), CShader.get(), program);

	if (!SPIRV[0].empty())
	{
		if (!CreateShaderModule(m_VShaderModule, SPIRV[0], VK_SHADER_STAGE_VERTEX_BIT))
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
		if (!CreateShaderModule(m_FShaderModule, SPIRV[1], VK_SHADER_STAGE_FRAGMENT_BIT))
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
		if (!CreateShaderModule(m_GShaderModule, SPIRV[2], VK_SHADER_STAGE_GEOMETRY_BIT))
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
		if (!CreateShaderModule(m_TCShaderModule, SPIRV[3], VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT))
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
		if (!CreateShaderModule(m_TEShaderModule, SPIRV[4], VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT))
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
		if (!CreateShaderModule(m_CShaderModule, SPIRV[5], VK_SHADER_STAGE_COMPUTE_BIT))
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

void TRAP::Graphics::API::VulkanShader::LoadSPIRV(const std::array<std::vector<uint32_t>, 6>& shaders)
{
	if (!shaders[0].empty())
	{
		TP_DEBUG(Log::ShaderVulkanSPIRVPrefix, "Loading Vertex Shader");
		if (!CreateShaderModule(m_VShaderModule, shaders[0], VK_SHADER_STAGE_VERTEX_BIT))
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
		TP_DEBUG(Log::ShaderVulkanSPIRVPrefix, "Loading Fragment Shader");
		if (!CreateShaderModule(m_FShaderModule, shaders[1], VK_SHADER_STAGE_FRAGMENT_BIT))
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
		TP_DEBUG(Log::ShaderVulkanSPIRVPrefix, "Loading Geometry Shader");
		if (!CreateShaderModule(m_GShaderModule, shaders[2], VK_SHADER_STAGE_GEOMETRY_BIT))
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
		TP_DEBUG(Log::ShaderVulkanSPIRVPrefix, "Loading Tessellation Control Shader");
		if (!CreateShaderModule(m_TCShaderModule, shaders[3], VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT))
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
		TP_DEBUG(Log::ShaderVulkanSPIRVPrefix, "Loading Tessellation Evaluation Shader");
		if (!CreateShaderModule(m_TEShaderModule, shaders[4], VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT))
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
		TP_DEBUG(Log::ShaderVulkanSPIRVPrefix, "Loading Compute Shader");
		if (!CreateShaderModule(m_CShaderModule, shaders[5], VK_SHADER_STAGE_COMPUTE_BIT))
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

void TRAP::Graphics::API::VulkanShader::PreProcessGLSL(const std::string_view source, std::array<std::string, 6>& shaders)
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
			TP_WARN(Log::ShaderVulkanGLSLPrefix, "Found Tag: \"", lines[i], "\" this is unnecessary! Skipping Line: ", i);
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
				TP_ERROR(Log::ShaderVulkanGLSLPrefix, ShaderTypeToString(static_cast<ShaderType>(i + 1)), ": Couldn't find \"main\" function!");
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

	if (shader)
	{
		shader->setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_2);
		shader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_5);
		glslang::TShader::ForbidIncluder includer;
		constexpr TBuiltInResource DefaultTBuiltInResource = 
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
		};
		if (!shader->preprocess(&DefaultTBuiltInResource,
			460,
			ECoreProfile,
			true,
			true,
			static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules),
			&preProcessedSource,
			includer))
		{
			TP_ERROR(Log::ShaderVulkanGLSLPrefix, "Preprocessing failed!");
			TP_ERROR(Log::ShaderVulkanGLSLPrefix, shader->getInfoLog());
			TP_ERROR(Log::ShaderVulkanGLSLPrefix, shader->getInfoDebugLog());

			return nullptr;
		}
		
		return shader;
	}
	
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanShader::Parse(glslang::TShader* shader)
{
	constexpr TBuiltInResource DefaultTBuiltInResource =
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
	};
	
	if (!shader->parse(&DefaultTBuiltInResource, 460, true, static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules)))
	{
		TP_ERROR(Log::ShaderVulkanGLSLPrefix, "Parsing failed: ");
		TP_ERROR(Log::ShaderVulkanGLSLPrefix, shader->getInfoLog());
		TP_ERROR(Log::ShaderVulkanGLSLPrefix, shader->getInfoDebugLog());

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
		TP_ERROR(Log::ShaderVulkanSPIRVPrefix, "Linking failed: ");
		TP_ERROR(Log::ShaderVulkanSPIRVPrefix, program.getInfoLog());
		TP_ERROR(Log::ShaderVulkanSPIRVPrefix, program.getInfoDebugLog());

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
#ifdef TRAP_DEBUG
		spvOptions.validate = true;
#else
		spvOptions.disableOptimizer = false;
		spvOptions.optimizeSize = true;
#endif		
		GlslangToSpv(*program.getIntermediate(EShLangVertex), SPIRV[0], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			TP_ERROR(Log::ShaderVulkanSPIRVPrefix, "Vertex Shader: ", logger.getAllMessages());
	}

	if (FShader)
	{
		spv::SpvBuildLogger logger{};
		glslang::SpvOptions spvOptions{};
#ifdef TRAP_DEBUG
		spvOptions.validate = true;
#else
		spvOptions.disableOptimizer = false;
		spvOptions.optimizeSize = true;
#endif		
		GlslangToSpv(*program.getIntermediate(EShLangFragment), SPIRV[1], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			TP_ERROR(Log::ShaderVulkanSPIRVPrefix, "Fragment Shader: ", logger.getAllMessages());
	}

	if (GShader)
	{
		spv::SpvBuildLogger logger{};
		glslang::SpvOptions spvOptions{};
#ifdef TRAP_DEBUG
		spvOptions.validate = true;
#else
		spvOptions.disableOptimizer = false;
		spvOptions.optimizeSize = true;
#endif		
		GlslangToSpv(*program.getIntermediate(EShLangGeometry), SPIRV[2], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			TP_ERROR(Log::ShaderVulkanSPIRVPrefix, "Geometry Shader: ", logger.getAllMessages());
	}

	if (TCShader)
	{
		spv::SpvBuildLogger logger{};
		glslang::SpvOptions spvOptions{};
#ifdef TRAP_DEBUG
		spvOptions.validate = true;
#else
		spvOptions.disableOptimizer = false;
		spvOptions.optimizeSize = true;
#endif		
		GlslangToSpv(*program.getIntermediate(EShLangTessControl), SPIRV[3], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			TP_ERROR(Log::ShaderVulkanSPIRVPrefix, "Tessellation Control Shader: ", logger.getAllMessages());
	}

	if (TEShader)
	{
		spv::SpvBuildLogger logger{};
		glslang::SpvOptions spvOptions{};
#ifdef TRAP_DEBUG
		spvOptions.validate = true;
#else
		spvOptions.disableOptimizer = false;
		spvOptions.optimizeSize = true;
#endif		
		GlslangToSpv(*program.getIntermediate(EShLangTessEvaluation), SPIRV[4], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			TP_ERROR(Log::ShaderVulkanSPIRVPrefix, "Tessellation Evaluation Shader: ", logger.getAllMessages());
	}

	if (CShader)
	{
		spv::SpvBuildLogger logger{};
		glslang::SpvOptions spvOptions{};
#ifdef TRAP_DEBUG
		spvOptions.validate = true;
#else
		spvOptions.disableOptimizer = false;
		spvOptions.optimizeSize = true;
#endif		
		GlslangToSpv(*program.getIntermediate(EShLangCompute), SPIRV[5], &logger, &spvOptions);
		if (logger.getAllMessages().length() > 0)
			TP_ERROR(Log::ShaderVulkanSPIRVPrefix, "Compute Shader: ", logger.getAllMessages());
	}

	return SPIRV;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanShader::CreateShaderModule(VkShaderModule& shaderModule, const std::vector<uint32_t>& SPIRVCode, const VkShaderStageFlagBits stage)
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
	VkCall(success = vkCreateShaderModule(Graphics::API::VulkanRenderer::GetDevice().GetDevice(), &shaderModuleCreateInfo, nullptr, &shaderModule));

	if(success == VK_SUCCESS)
	{
		Reflect(SPIRVCode, stage);
		
		return true;
	}
	
	return false;;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::Reflect(const std::vector<uint32_t>& SPIRVCode, const VkShaderStageFlagBits stage)
{
	//TODO Get VkVertexAttributeBindingDescriptions and stride

	spirv_cross::CompilerGLSL compiler(SPIRVCode);
	const spirv_cross::SmallVector<spirv_cross::EntryPoint> eps = compiler.get_entry_points_and_stages();
	if(eps.empty())
	{
		//TODO Entry point not found! Use Fallback Shader
		TP_ERROR(Log::ShaderVulkanSPIRVPrefix, VkShaderStageFlagBitsToString(stage), ": Couldn't find \"main\" function!");
		return;
	}
	//Use reflection to read the shader layouts
	const spirv_cross::ShaderResources resources = compiler.get_shader_resources();
#ifdef TRAP_DEBUG_SHADERS
	TP_DEBUG(VulkanShaderSPIRVPrefix, VkShaderStageFlagBitsToString(stage), ": Reflecting...");
	PrintResources("Uniform Buffers", resources.uniform_buffers, compiler);
	PrintResources("Storage Buffers", resources.storage_buffers, compiler);
	PrintResources("Storage Images", resources.storage_images, compiler);
	PrintResources("Samples Images", resources.sampled_images, compiler);
	PrintResources("Push Constant Buffers", resources.push_constant_buffers, compiler);
	PrintResources("Atomic Counters", resources.atomic_counters, compiler);
	PrintResources("Separate Images", resources.separate_images, compiler);
	PrintResources("Separate Samplers", resources.separate_samplers, compiler);
#endif

	std::vector<VulkanShaderResourceTypeMap> resourceTypeMap
	{
		{VK_DESCRIPTOR_TYPE_SAMPLER, resources.separate_samplers},
		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, resources.sampled_images},
		{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, resources.separate_images},
		{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, resources.storage_images},
		//{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, ?}, //No matching vector
		//{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, ?}, //No matching vector
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, resources.uniform_buffers},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, resources.storage_buffers},
		//VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC and VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC are only created if
		//your application want to enable a dynamic uniform or storage buffer for a particular buffer.
		//{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, ? } //Not applicable
	};
	for(auto& r : resourceTypeMap)
	{
		if(!ReflectResource(stage, compiler, r))
		{
			//TODO Failed don't use shader!
			TP_ERROR(Log::ShaderVulkanSPIRVPrefix, "ReflectResource(", VkDescriptorTypeToString(r.DescriptorType), " (", static_cast<uint32_t>(r.DescriptorType), ")) failed!");
			return;
		}
	}

	//TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::PrintResources(const std::string_view typeName, const spirv_cross::SmallVector<spirv_cross::Resource>& resources, spirv_cross::CompilerGLSL& compiler)
{
	for(uint32_t i = 0; i < resources.size(); i++)
	{
		TP_TRACE(Log::ShaderVulkanSPIRVPrefix, typeName, "[", i, "]:");
		PrintResource(compiler, resources[i]);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::PrintResource(spirv_cross::CompilerGLSL& compiler, const spirv_cross::Resource& res)
{
	TP_TRACE(Log::ShaderVulkanSPIRVPrefix, "    ID = ", res.id, " BaseTypeID = ", res.base_type_id);
	std::stringstream out;
	PrintType(out, compiler, compiler.get_type(res.base_type_id));
	std::vector<std::string_view> strs = TRAP::Utils::String::SplitStringView(out.str(), '\n');
	for(std::string_view str : strs)
		TP_TRACE(Log::ShaderVulkanSPIRVPrefix, str);
	const spv::StorageClass sc = compiler.get_storage_class(res.id);
	TP_TRACE(Log::ShaderVulkanSPIRVPrefix, "    ID = ", res.id, " StorageClass = ", static_cast<uint32_t>(sc), " (", StorageClassToString(sc), ")");
	std::string str = "    ";
	compiler.get_decoration_bitset(res.id).for_each_bit([&](const uint32_t dec)
	{
		const spv::Decoration d = static_cast<decltype(d)>(dec);
		str += DecorationToString(d) + " = " + std::to_string(compiler.get_decoration(res.id, d)) + " ";
	});
	TP_TRACE(Log::ShaderVulkanSPIRVPrefix, str);
	TP_TRACE(Log::ShaderVulkanSPIRVPrefix, "    Name = \"", res.name, "\"");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanShader::PrintType(std::stringstream& out, spirv_cross::CompilerGLSL& compiler, const spirv_cross::SPIRType& type)
{
	out << '(' << BaseTypeToString(type.basetype) << ") sizeof = ";
	out << type.width << " x " << type.vecsize << " x " << type.columns;
	if (type.basetype == spirv_cross::SPIRType::Image || type.basetype == spirv_cross::SPIRType::SampledImage)
		out << " (Image" << (static_cast<uint32_t>(type.image.dim) + 1) << "D)";
	else if(type.basetype == spirv_cross::SPIRType::Struct)
	{
		for(std::size_t i = 0; i < type.member_types.size(); i++)
		{
			out << "\n    m[" << i << "]: ";
			PrintType(out, compiler, compiler.get_type(type.member_types[i]));
		}
	}

	if(type.type_alias)
	{
		out << "\n    alias = " << type.type_alias << ": ";
		PrintType(out, compiler, compiler.get_type(type.type_alias));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanShader::ReflectResource(VkShaderStageFlagBits stage, spirv_cross::CompilerGLSL& compiler, VulkanShaderResourceTypeMap& rtm)
{
	return true; //TODO
	
	return false;
}