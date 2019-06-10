#ifndef _TRAP_VULKANSHADER_H_
#define _TRAP_VULKANSHADER_H_

namespace TRAP::Graphics::API
{
	struct VulkanShaderErrorInfo
	{
		unsigned int Shader{};
		std::array<std::string, 6> Message;
		std::array<unsigned int, 6> Line{};
	};

	class VulkanShader : public Shader
	{
	public:
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

		void SetVSSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot = 0) override;
		void SetFSSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot = 0) override;
		void SetGSSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot = 0) override;
		void SetTCSSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot = 0) override;
		void SetTESSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot = 0) override;
		void SetCSSystemUniformBuffer(uint8_t* data, unsigned int size, unsigned int slot = 0) override;

		void SetVSUserUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetFSUserUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetGSUserUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetTCSUserUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetTESUserUniformBuffer(uint8_t* data, unsigned int size) override;
		void SetCSUserUniformBuffer(uint8_t* data, unsigned int size) override;

		const ShaderUniformBufferList& GetVSSystemUniforms() const override;
		const ShaderUniformBufferList& GetFSSystemUniforms() const override;
		const ShaderUniformBufferList& GetGSSystemUniforms() const override;
		const ShaderUniformBufferList& GetTCSSystemUniforms() const override;
		const ShaderUniformBufferList& GetTESSystemUniforms() const override;
		const ShaderUniformBufferList& GetCSSystemUniforms() const override;
		const ShaderUniformBufferDeclaration* GetVSUserUniformBuffer() const override;
		const ShaderUniformBufferDeclaration* GetFSUserUniformBuffer() const override;
		const ShaderUniformBufferDeclaration* GetGSUserUniformBuffer() const override;
		const ShaderUniformBufferDeclaration* GetTCSUserUniformBuffer() const override;
		const ShaderUniformBufferDeclaration* GetTESUserUniformBuffer() const override;
		const ShaderUniformBufferDeclaration* GetCSUserUniformBuffer() const override;

		const ShaderResourceList& GetResource() const override;

		const std::string& GetName() const override;
		
		const std::string& GetVSFilePath() const override;
		const std::string& GetFSFilePath() const override;
		const std::string& GetGSFilePath() const override;
		const std::string& GetTCSFilePath() const override;
		const std::string& GetTESFilePath() const override;
		const std::string& GetCSFilePath() const override;

		const std::string& GetVSSource() const override;
		const std::string& GetFSSource() const override;
		const std::string& GetGSSource() const override;
		const std::string& GetTCSSource() const override;
		const std::string& GetTESSource() const override;
		const std::string& GetCSSource() const override;

	private:
		void Compile(std::array<std::string*, 6> & shaders, VulkanShaderErrorInfo& info);
		static std::unique_ptr<glslang::TShader> PreProcess(const char* source, unsigned int shaderType, std::string& preProcessedSource);
		static bool Parse(glslang::TShader* shader);
		static bool Link(glslang::TShader* VShader, glslang::TShader* FShader, glslang::TShader* GShader, glslang::TShader* TCShader, glslang::TShader* TEShader, glslang::TShader* CShader, glslang::TProgram& program);
		static std::vector<std::vector<unsigned int>> ConvertToSPIRV(glslang::TShader* VShader, glslang::TShader* FShader, glslang::TShader* GShader, glslang::TShader* TCShader, glslang::TShader* TEShader, glslang::TShader* CShader, glslang::TProgram& program);
		static bool CreateShaderModule(VkShaderModule& shaderModule, std::vector<unsigned int>& SPIRVCode);

		friend class Shader;
		friend class ::TRAP::Graphics::ShaderManager;

		//unsigned int m_handle;
		std::string m_name, m_VSFilepath, m_FSFilepath, m_GSFilepath, m_TCSFilepath, m_TESFilepath, m_CSFilepath;
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

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::VulkanShader::GetVSSystemUniforms() const
{
	return m_VSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::VulkanShader::GetFSSystemUniforms() const
{
	return m_FSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::VulkanShader::GetGSSystemUniforms() const
{
	return m_GSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::VulkanShader::GetTCSSystemUniforms() const
{
	return m_TCSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::VulkanShader::GetTESSystemUniforms() const
{
	return m_TESUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferList& TRAP::Graphics::API::VulkanShader::GetCSSystemUniforms() const
{
	return m_CSUniformBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::VulkanShader::GetVSUserUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::VulkanShader::GetFSUserUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::VulkanShader::GetGSUserUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::VulkanShader::GetTCSUserUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::VulkanShader::GetTESUserUniformBuffer() const
{
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const TRAP::Graphics::API::ShaderUniformBufferDeclaration* TRAP::Graphics::API::VulkanShader::GetCSUserUniformBuffer() const
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

inline const std::string& TRAP::Graphics::API::VulkanShader::GetVSFilePath() const
{
	return m_VSFilepath;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::VulkanShader::GetFSFilePath() const
{
	return m_FSFilepath;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::VulkanShader::GetGSFilePath() const
{
	return m_GSFilepath;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::VulkanShader::GetTCSFilePath() const
{
	return m_TCSFilepath;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::VulkanShader::GetTESFilePath() const
{
	return m_TESFilepath;
}

//-------------------------------------------------------------------------------------------------------------------//

inline const std::string& TRAP::Graphics::API::VulkanShader::GetCSFilePath() const
{
	return m_CSFilepath;
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

#endif /*_TRAP_VULKANSHADER_H_*/