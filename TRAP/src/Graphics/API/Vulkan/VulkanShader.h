#ifndef _TRAP_VULKANSHADER_H_
#define _TRAP_VULKANSHADER_H_

#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ShaderUniform.h"
#include "Graphics/Shaders/ShaderResource.h"
#include "Graphics/Shaders/ShaderManager.h"

namespace TRAP::Graphics::API
{
	struct VulkanShaderErrorInfo
	{
		unsigned int Shader{};
		std::array<std::string, 6> Message;
		std::array<unsigned int, 6> Line{};
	};

	class VulkanShader final : public Graphics::Shader
	{
	public:
		VulkanShader(std::string name, std::string source);
		VulkanShader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource);
		VulkanShader(const VulkanShader&) = default;
		VulkanShader& operator=(const VulkanShader&) = default;
		VulkanShader(VulkanShader&&) = default;
		VulkanShader& operator=(VulkanShader&&) = default;
		~VulkanShader();

		void Init();
		void Shutdown() const;

		void Bind() const override;
		void Unbind() const override;

		void SetVSUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetFSUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetGSUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetTCSUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetTESUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetCSUniformBuffer(uint8_t* data, unsigned int size) override;

		const ShaderUniformBufferList& GetVSUniforms() const override;
		const ShaderUniformBufferList& GetFSUniforms() const override;
		const ShaderUniformBufferList& GetGSUniforms() const override;
		const ShaderUniformBufferList& GetTCSUniforms() const override;
		const ShaderUniformBufferList& GetTESUniforms() const override;
		const ShaderUniformBufferList& GetCSUniforms() const override;
		const ShaderUniformBufferDeclaration* GetVSUniformBuffer() const override;
		const ShaderUniformBufferDeclaration* GetFSUniformBuffer() const override;
		const ShaderUniformBufferDeclaration* GetGSUniformBuffer() const override;
		const ShaderUniformBufferDeclaration* GetTCSUniformBuffer() const override;
		const ShaderUniformBufferDeclaration* GetTESUniformBuffer() const override;
		const ShaderUniformBufferDeclaration* GetCSUniformBuffer() const override;

		const ShaderResourceList& GetResource() const override;

		const std::string& GetName() const override;
		
		const std::string& GetFilePath() const override;

		const std::string& GetVSSource() const override;
		const std::string& GetFSSource() const override;
		const std::string& GetGSSource() const override;
		const std::string& GetTCSSource() const override;
		const std::string& GetTESSource() const override;
		const std::string& GetCSSource() const override;

		std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStageCreateInfos();

	private:
		void Compile(std::array<std::string*, 6> & shaders, VulkanShaderErrorInfo& info);
		static void PreProcessGLSL(const std::string& source, std::array<std::string*, 6>& shaders);
		static std::unique_ptr<glslang::TShader> PreProcess(const char* source, unsigned int shaderType, std::string& preProcessedSource);
		static bool Parse(glslang::TShader* shader);
		static bool Link(glslang::TShader* VShader, glslang::TShader* FShader, glslang::TShader* GShader, glslang::TShader* TCShader, glslang::TShader* TEShader, glslang::TShader* CShader, glslang::TProgram& program);
		static std::vector<std::vector<unsigned int>> ConvertToSPIRV(glslang::TShader* VShader, glslang::TShader* FShader, glslang::TShader* GShader, glslang::TShader* TCShader, glslang::TShader* TEShader, glslang::TShader* CShader, glslang::TProgram& program);
		static bool CreateShaderModule(VkShaderModule& shaderModule, std::vector<unsigned int>& SPIRVCode);

		friend class ::TRAP::Graphics::Shader;
		friend class ::TRAP::Graphics::ShaderManager;

		//unsigned int m_handle;
		std::string m_name, m_filepath, m_source;
		std::string m_VSSource, m_FSSource, m_GSSource, m_TCSSource, m_TESSource, m_CSSource;

		ShaderUniformBufferList m_VSUniformBuffers;
		ShaderUniformBufferList m_FSUniformBuffers;
		ShaderUniformBufferList m_GSUniformBuffers;
		ShaderUniformBufferList m_TCSUniformBuffers;
		ShaderUniformBufferList m_TESUniformBuffers;
		ShaderUniformBufferList m_CSUniformBuffers;
		/*std::unique_ptr<VulkanShaderUniformBufferDeclaration> m_VSUserUniformBuffer;
		std::unique_ptr<VulkanShaderUniformBufferDeclaration> m_FSUserUniformBuffer;
		std::unique_ptr<VulkanShaderUniformBufferDeclaration> m_GSUserUniformBuffer;
		std::unique_ptr<VulkanShaderUniformBufferDeclaration> m_TCSUserUniformBuffer;
		std::unique_ptr<VulkanShaderUniformBufferDeclaration> m_TESUserUniformBuffer;
		std::unique_ptr<VulkanShaderUniformBufferDeclaration> m_CSUserUniformBuffer;*/
		ShaderResourceList m_resources;
		ShaderStructList m_structs;

		static bool s_glslangInitialized;

		VkShaderModule m_VShaderModule;
		VkShaderModule m_FShaderModule;
		VkShaderModule m_GShaderModule;
		VkShaderModule m_TCShaderModule;
		VkShaderModule m_TEShaderModule;
		VkShaderModule m_CShaderModule;

		std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::VulkanShader::GetVSUniforms() const
{
	return m_VSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::VulkanShader::GetFSUniforms() const
{
	return m_FSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::VulkanShader::GetGSUniforms() const
{
	return m_GSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::VulkanShader::GetTCSUniforms() const
{
	return m_TCSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::VulkanShader::GetTESUniforms() const
{
	return m_TESUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::VulkanShader::GetCSUniforms() const
{
	return m_CSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::VulkanShader::GetVSUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::VulkanShader::GetFSUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::VulkanShader::GetGSUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::VulkanShader::GetTCSUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::VulkanShader::GetTESUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::VulkanShader::GetCSUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderResourceList& TRAP::Graphics::API::VulkanShader::GetResource() const
{
	return m_resources;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::VulkanShader::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::VulkanShader::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::VulkanShader::GetVSSource() const
{
	return m_VSSource;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::VulkanShader::GetFSSource() const
{
	return m_FSSource;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::VulkanShader::GetGSSource() const
{
	return m_GSSource;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::VulkanShader::GetTCSSource() const
{
	return m_TCSSource;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::VulkanShader::GetTESSource() const
{	
	return m_TESSource;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::VulkanShader::GetCSSource() const
{
	return m_CSSource;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::vector<VkPipelineShaderStageCreateInfo>& TRAP::Graphics::API::VulkanShader::GetShaderStageCreateInfos()
{
	return m_shaderStages;
}

#endif /*_TRAP_VULKANSHADER_H_*/