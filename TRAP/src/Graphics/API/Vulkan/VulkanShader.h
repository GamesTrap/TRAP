#ifndef _TRAP_VULKANSHADER_H_
#define _TRAP_VULKANSHADER_H_

#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ShaderManager.h"

namespace TRAP::Graphics::API
{
	struct VulkanShaderErrorInfo
	{
		uint32_t Shader{};
		std::array<std::string, 6> Message;
		std::array<uint32_t, 6> Line{};
	};

	class VulkanShader final : public Shader
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

		const std::string& GetName() const override;
		
		const std::string& GetFilePath() const override;

		const std::string& GetVSSource() const override;
		const std::string& GetFSSource() const override;
		const std::string& GetGSSource() const override;
		const std::string& GetTCSSource() const override;
		const std::string& GetTESSource() const override;
		const std::string& GetCSSource() const override;

	private:
		void Compile(std::array<std::string*, 6> & shaders, VulkanShaderErrorInfo& info);
		static void PreProcessGLSL(const std::string& source, std::array<std::string*, 6>& shaders);
		static Scope<glslang::TShader> PreProcess(const char* source, uint32_t shaderType, std::string& preProcessedSource);
		static bool Parse(glslang::TShader* shader);
		static bool Link(glslang::TShader* VShader, glslang::TShader* FShader, glslang::TShader* GShader, glslang::TShader* TCShader, glslang::TShader* TEShader, glslang::TShader* CShader, glslang::TProgram& program);
		static std::vector<std::vector<uint32_t>> ConvertToSPIRV(glslang::TShader* VShader, glslang::TShader* FShader, glslang::TShader* GShader, glslang::TShader* TCShader, glslang::TShader* TEShader, glslang::TShader* CShader, glslang::TProgram& program);
		static bool CreateShaderModule(VkShaderModule& shaderModule, std::vector<uint32_t>& SPIRVCode);

		friend class ::TRAP::Graphics::Shader;
		friend class ::TRAP::Graphics::ShaderManager;

		std::string m_name, m_filepath, m_source;
		std::string m_VSSource, m_FSSource, m_GSSource, m_TCSSource, m_TESSource, m_CSSource;

		static bool s_glslangInitialized;

		VkShaderModule m_VShaderModule;
		VkShaderModule m_FShaderModule;
		VkShaderModule m_GShaderModule;
		VkShaderModule m_TCShaderModule;
		VkShaderModule m_TEShaderModule;
		VkShaderModule m_CShaderModule;

		std::vector<VkPipelineShaderStageCreateInfo> m_graphicsShaderStages;
		VkPipelineShaderStageCreateInfo m_computeShaderStage;
	};
}

#endif /*_TRAP_VULKANSHADER_H_*/