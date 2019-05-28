#include "TRAPPCH.h"
#include "VulkanShader.h"

namespace TRAP::Graphics::API
{
	bool VulkanShader::s_glslangInitialized = false;

	VulkanShader::VulkanShader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource)
		: m_name(std::move(name)), m_VSSource(std::move(VSSource)), m_FSSource(std::move(FSSource)), m_GSSource(std::move(GSSource)), m_TCSSource(std::move(TCSSource)), m_TESSource(std::move(TESSource)), m_CSSource(std::move(CSSource))/*, m_VSUserUniformBuffer(nullptr), m_FSUserUniformBuffer(nullptr)*/
	{
		Init();
	}

	VulkanShader::~VulkanShader()
	{
		Shutdown();
	}

	void VulkanShader::Init()
	{
		std::array<std::string*, 6> shaders{ &m_VSSource, &m_FSSource, &m_GSSource, &m_TCSSource, &m_TESSource, &m_CSSource };
		VulkanShaderErrorInfo error;
		TP_DEBUG("[Shader][Vulkan] Compiling: \"", m_name, "\"");
		Compile(shaders, error);

	}

	void VulkanShader::Shutdown()
	{
	}

	void VulkanShader::Compile(std::array<std::string*, 6>& shaders, VulkanShaderErrorInfo& info)
	{
		if(!s_glslangInitialized)
		{
			glslang::InitializeProcess();
			s_glslangInitialized = true;
		}

		if (!shaders[0]->empty())
		{
			const char* VSSource = shaders[0]->c_str();
			TP_DEBUG("[Shader][Vulkan] Pre-Processing Vertex Shader");
			std::string preProcessedSource;
			std::unique_ptr<glslang::TShader> shader = PreProcess(VSSource, 0, preProcessedSource);
			if(preProcessedSource.empty() || !shader)
				return;
			const char* preProcessedCStr = preProcessedSource.c_str();
			shader->setStrings(&preProcessedCStr, 1);

			//TP_DEBUG("[Shader][Vulkan] Compiling Vertex Shader");
		}

		if (!shaders[1]->empty())
		{
			const char* FSSource = shaders[1]->c_str();
			TP_DEBUG("[Shader][Vulkan] Pre-Processing Fragment Shader");
			std::string preProcessedSource;
			std::unique_ptr<glslang::TShader> shader = PreProcess(FSSource, 1, preProcessedSource);
			if (preProcessedSource.empty())
				return;
			const char* preProcessedCStr = preProcessedSource.c_str();
			shader->setStrings(&preProcessedCStr, 1);
		}

		if (!shaders[2]->empty())
		{
			const char* GSSource = shaders[2]->c_str();
			TP_DEBUG("[Shader][Vulkan] Pre-Processing Geometry Shader");
			std::string preProcessedSource;
			std::unique_ptr<glslang::TShader> shader = PreProcess(GSSource, 2, preProcessedSource);
			if (preProcessedSource.empty())
				return;
			const char* preProcessedCStr = preProcessedSource.c_str();
			shader->setStrings(&preProcessedCStr, 1);
		}

		if (!shaders[3]->empty())
		{
			const char* TCSSource = shaders[3]->c_str();
			TP_DEBUG("[Shader][Vulkan] Pre-Processing Tessellation Control Shader");
			std::string preProcessedSource;
			std::unique_ptr<glslang::TShader> shader = PreProcess(TCSSource, 3, preProcessedSource);
			if (preProcessedSource.empty())
				return;
			const char* preProcessedCStr = preProcessedSource.c_str();
			shader->setStrings(&preProcessedCStr, 1);
		}

		if (!shaders[4]->empty())
		{
			const char* TESSource = shaders[4]->c_str();
			TP_DEBUG("[Shader][Vulkan] Pre-Processing Tessellation Evaluation Shader");
			std::string preProcessedSource;
			std::unique_ptr<glslang::TShader> shader = PreProcess(TESSource, 4, preProcessedSource);
			if (preProcessedSource.empty())
				return;
			const char* preProcessedCStr = preProcessedSource.c_str();
			shader->setStrings(&preProcessedCStr, 1);
		}

		if (!shaders[5]->empty())
		{
			const char* CSSource = shaders[5]->c_str();
			TP_DEBUG("[Shader][Vulkan] Pre-Processing Compute Shader");
			std::string preProcessedSource;
			std::unique_ptr<glslang::TShader> shader = PreProcess(CSSource, 5, preProcessedSource);
			if (preProcessedSource.empty())
				return;
			const char* preProcessedCStr = preProcessedSource.c_str();
			shader->setStrings(&preProcessedCStr, 1);
		}
	}

	std::unique_ptr<glslang::TShader> VulkanShader::PreProcess(const char* source, unsigned int shaderType, std::string& preProcessedSource)
	{
		std::unique_ptr<glslang::TShader> shader;
		if(shaderType == 0)
		{
			shader = std::make_unique<glslang::TShader>(EShLangVertex);
			shader->setStrings(&source, 1);
			shader->setEnvInput(glslang::EShSourceGlsl, EShLangVertex, glslang::EShClientVulkan, glslang::EShTargetOpenGL_450);	
		}
		else if(shaderType == 1)
		{
			shader = std::make_unique<glslang::TShader>(EShLangFragment);
			shader->setStrings(&source, 1);
			shader->setEnvInput(glslang::EShSourceGlsl, EShLangFragment, glslang::EShClientVulkan, glslang::EShTargetOpenGL_450);
		}
		else if(shaderType == 2)
		{
			shader = std::make_unique<glslang::TShader>(EShLangGeometry);
			shader->setStrings(&source, 1);
			shader->setEnvInput(glslang::EShSourceGlsl, EShLangGeometry, glslang::EShClientVulkan, glslang::EShTargetOpenGL_450);
		}
		else if(shaderType == 3)
		{
			shader = std::make_unique<glslang::TShader>(EShLangTessControl);
			shader->setStrings(&source, 1);
			shader->setEnvInput(glslang::EShSourceGlsl, EShLangTessControl, glslang::EShClientVulkan, glslang::EShTargetOpenGL_450);
		}
		else if(shaderType == 4)
		{
			shader = std::make_unique<glslang::TShader>(EShLangTessEvaluation);
			shader->setStrings(&source, 1);
			shader->setEnvInput(glslang::EShSourceGlsl, EShLangTessEvaluation, glslang::EShClientVulkan, glslang::EShTargetOpenGL_450);
		}
		else if(shaderType == 5)
		{
			shader = std::make_unique<glslang::TShader>(EShLangCompute);
			shader->setStrings(&source, 1);
			shader->setEnvInput(glslang::EShSourceGlsl, EShLangCompute, glslang::EShClientVulkan, glslang::EShTargetOpenGL_450);
		}

		shader->setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_1);
		shader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_4);
		const EShMessages messages = static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules);
		DirStackFileIncluder includer;
		if (!shader->preprocess(&glslang::DefaultTBuiltInResource, glslang::EShTargetOpenGL_450, ECoreProfile, true, false, messages, &preProcessedSource, includer))
		{
			TP_ERROR("[Shader}[Vulkan] GLSL Preprocessing failed!");
			TP_ERROR("[Shader][Vulkan] ", shader->getInfoLog());
			TP_ERROR("[Shader][Vulkan] ", shader->getInfoDebugLog());

			return nullptr;
		}

		return shader;
	}

	void VulkanShader::Bind() const
	{		
	}

	void VulkanShader::Unbind() const
	{		
	}

	void VulkanShader::SetVSSystemUniformBuffer(uint8_t* data, unsigned size, unsigned slot)
	{		
	}

	void VulkanShader::SetFSSystemUniformBuffer(uint8_t* data, unsigned size, unsigned slot)
	{
	}

	void VulkanShader::SetGSSystemUniformBuffer(uint8_t* data, unsigned size, unsigned slot)
	{
	}

	void VulkanShader::SetTCSSystemUniformBuffer(uint8_t* data, unsigned size, unsigned slot)
	{
	}

	void VulkanShader::SetTESSystemUniformBuffer(uint8_t* data, unsigned size, unsigned slot)
	{
	}

	void VulkanShader::SetCSSystemUniformBuffer(uint8_t* data, unsigned size, unsigned slot)
	{
	}

	void VulkanShader::SetVSUserUniformBuffer(uint8_t* data, unsigned size)
	{		
	}

	void VulkanShader::SetFSUserUniformBuffer(uint8_t* data, unsigned size)
	{
	}

	void VulkanShader::SetGSUserUniformBuffer(uint8_t* data, unsigned size)
	{
	}

	void VulkanShader::SetTCSUserUniformBuffer(uint8_t* data, unsigned size)
	{
	}

	void VulkanShader::SetTESUserUniformBuffer(uint8_t* data, unsigned size)
	{
	}

	void VulkanShader::SetCSUserUniformBuffer(uint8_t* data, unsigned size)
	{
	}

	const ShaderUniformBufferList& VulkanShader::GetVSSystemUniforms() const
	{
		return m_VSUniformBuffers;
	}

	const ShaderUniformBufferList& VulkanShader::GetFSSystemUniforms() const
	{
		return m_FSUniformBuffers;
	}

	const ShaderUniformBufferList& VulkanShader::GetGSSystemUniforms() const
	{
		return m_GSUniformBuffers;
	}

	const ShaderUniformBufferList& VulkanShader::GetTCSSystemUniforms() const
	{
		return m_TCSUniformBuffers;
	}

	const ShaderUniformBufferList& VulkanShader::GetTESSystemUniforms() const
	{
		return m_TESUniformBuffers;
	}

	const ShaderUniformBufferList& VulkanShader::GetCSSystemUniforms() const
	{
		return m_CSUniformBuffers;
	}

	const ShaderUniformBufferDeclaration* VulkanShader::GetVSUserUniformBuffer() const
	{
		return nullptr;
	}

	const ShaderUniformBufferDeclaration* VulkanShader::GetFSUserUniformBuffer() const
	{
		return nullptr;
	}

	const ShaderUniformBufferDeclaration* VulkanShader::GetGSUserUniformBuffer() const
	{
		return nullptr;
	}

	const ShaderUniformBufferDeclaration* VulkanShader::GetTCSUserUniformBuffer() const
	{
		return nullptr;
	}

	const ShaderUniformBufferDeclaration* VulkanShader::GetTESUserUniformBuffer() const
	{
		return nullptr;
	}

	const ShaderUniformBufferDeclaration* VulkanShader::GetCSUserUniformBuffer() const
	{
		return nullptr;
	}

	const ShaderResourceList& VulkanShader::GetResource() const
	{
		return m_resources;
	}

	const std::string& VulkanShader::GetName() const
	{
		return m_name;
	}

	const std::string& VulkanShader::GetVSFilePath() const
	{
		return m_VSFilepath;
	}

	const std::string& VulkanShader::GetFSFilePath() const
	{
		return m_FSFilepath;
	}

	const std::string& VulkanShader::GetGSFilePath() const
	{
		return m_GSFilepath;
	}

	const std::string& VulkanShader::GetTCSFilePath() const
	{
		return m_TCSFilepath;
	}

	const std::string& VulkanShader::GetTESFilePath() const
	{
		return m_TESFilepath;
	}

	const std::string& VulkanShader::GetCSFilePath() const
	{
		return m_CSFilepath;
	}

	const std::string& VulkanShader::GetVSSource() const
	{
		return m_VSSource;
	}

	const std::string& VulkanShader::GetFSSource() const
	{
		return m_FSSource;
	}

	const std::string& VulkanShader::GetGSSource() const
	{
		return m_GSSource;
	}

	const std::string& VulkanShader::GetTCSSource() const
	{
		return m_TCSSource;
	}

	const std::string& VulkanShader::GetTESSource() const
	{
		return m_TESSource;
	}

	const std::string& VulkanShader::GetCSSource() const
	{
		return m_CSSource;
	}
}