#ifndef _TRAP_VULKANSHADER_H_
#define _TRAP_VULKANSHADER_H_

#include "Graphics/Shaders/Shader.h"

namespace TRAP::Graphics::API
{
	struct VulkanShaderErrorInfo
	{
		uint32_t Shader{};
		std::array<std::string, 6> Message;
		std::array<uint32_t, 6> Line{};
	};

	struct VulkanShaderResourceTypeMap
	{
		VkDescriptorType DescriptorType{};
		const spirv_cross::SmallVector<spirv_cross::Resource>& Resources;
	};
	
	class VulkanShader final : public Shader
	{
	public:
		VulkanShader(std::string name, const std::string& source);
		VulkanShader(std::string name, std::vector<uint32_t>& source);
		VulkanShader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource);
		VulkanShader(const VulkanShader&) = default;
		VulkanShader& operator=(const VulkanShader&) = default;
		VulkanShader(VulkanShader&&) = default;
		VulkanShader& operator=(VulkanShader&&) = default;
		~VulkanShader();
		
		void Shutdown() const;

		void Bind() const override;
		void Unbind() const override;

	private:
		void InitSPIRV(std::vector<uint32_t>& source);
		void InitGLSL(const std::string& source);
		void InitGLSL(std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource);
		
		void CompileGLSL(std::array<std::string, 6> & shaders, VulkanShaderErrorInfo& info);
		void LoadSPIRV(std::array<std::vector<uint32_t>, 6>& shaders, VulkanShaderErrorInfo& info);
		static void PreProcessGLSL(const std::string& source, std::array<std::string, 6>& shaders);
		static Scope<glslang::TShader> PreProcess(const char* source, uint32_t shaderType, std::string& preProcessedSource);
		static bool Parse(glslang::TShader* shader);
		static bool Link(glslang::TShader* VShader, glslang::TShader* FShader, glslang::TShader* GShader, glslang::TShader* TCShader, glslang::TShader* TEShader, glslang::TShader* CShader, glslang::TProgram& program);
		static std::vector<std::vector<uint32_t>> ConvertToSPIRV(glslang::TShader* VShader, glslang::TShader* FShader, glslang::TShader* GShader, glslang::TShader* TCShader, glslang::TShader* TEShader, glslang::TShader* CShader, glslang::TProgram& program);
		bool CreateShaderModule(VkShaderModule& shaderModule, std::vector<uint32_t>& SPIRVCode, VkShaderStageFlagBits stage);

		void Reflect(const std::vector<uint32_t>& SPIRVCode, VkShaderStageFlagBits stage);
		static void PrintResources(const std::string& typeName, const spirv_cross::SmallVector<spirv_cross::Resource>& resources, spirv_cross::CompilerGLSL& compiler);
		static void PrintResource(spirv_cross::CompilerGLSL& compiler, const spirv_cross::Resource& res);
		static void PrintType(std::stringstream& out, spirv_cross::CompilerGLSL& compiler, const spirv_cross::SPIRType& type);
		static bool ReflectResource(VkShaderStageFlagBits stage, spirv_cross::CompilerGLSL& compiler, VulkanShaderResourceTypeMap& rtm);

		static bool s_glslangInitialized;

		VkShaderModule m_VShaderModule;
		VkShaderModule m_FShaderModule;
		VkShaderModule m_GShaderModule;
		VkShaderModule m_TCShaderModule;
		VkShaderModule m_TEShaderModule;
		VkShaderModule m_CShaderModule;

		std::vector<VkPipelineShaderStageCreateInfo> m_graphicsShaderStages;
		VkPipelineShaderStageCreateInfo m_computeShaderStage;

		static const std::map<spv::ExecutionModel, VkShaderStageFlagBits> s_model2Stage;
		uint32_t m_stride;
		std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;
	};
}

#endif /*_TRAP_VULKANSHADER_H_*/