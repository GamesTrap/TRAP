#ifndef _TRAP_VULKANSHADER_H_
#define _TRAP_VULKANSHADER_H_

#include <algorithm>
#include <algorithm>
#include <algorithm>
#include <algorithm>

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
		VulkanShader(std::string name, std::string_view source);
		VulkanShader(std::string name, const std::vector<uint32_t>& source);
		VulkanShader(std::string name, std::string_view VSSource, std::string_view FSSource, std::string_view GSSource, std::string_view TCSSource, std::string_view TESSource, std::string_view CSSource);
		VulkanShader(const VulkanShader&) = default;
		VulkanShader& operator=(const VulkanShader&) = default;
		VulkanShader(VulkanShader&&) = default;
		VulkanShader& operator=(VulkanShader&&) = default;
		~VulkanShader();
		
		void Shutdown() const;

		void Bind() const override;
		void Unbind() const override;

	private:
		void InitSPIRV(const std::vector<uint32_t>& source);
		void InitGLSL(std::string_view source);
		void InitGLSL(std::string_view VSSource, std::string_view FSSource, std::string_view GSSource, std::string_view TCSSource, std::string_view TESSource, std::string_view CSSource);
		
		void CompileGLSL(const std::array<std::string_view, 6>& shaders);
		void LoadSPIRV(const std::array<std::vector<uint32_t>, 6>& shaders);
		static void PreProcessGLSL(std::string_view source, std::array<std::string, 6>& shaders);
		static Scope<glslang::TShader> PreProcess(const char* source, uint32_t shaderType, std::string& preProcessedSource);
		static bool Parse(glslang::TShader* shader);
		static bool Link(glslang::TShader* VShader, glslang::TShader* FShader, glslang::TShader* GShader, glslang::TShader* TCShader, glslang::TShader* TEShader, glslang::TShader* CShader, glslang::TProgram& program);
		static std::vector<std::vector<uint32_t>> ConvertToSPIRV(glslang::TShader* VShader, glslang::TShader* FShader, glslang::TShader* GShader, glslang::TShader* TCShader, glslang::TShader* TEShader, glslang::TShader* CShader, glslang::TProgram& program);
		bool CreateShaderModule(VkShaderModule& shaderModule, const std::vector<uint32_t>& SPIRVCode, VkShaderStageFlagBits stage);

		void Reflect(const std::vector<uint32_t>& SPIRVCode, VkShaderStageFlagBits stage);
		static void PrintResources(std::string_view typeName, const spirv_cross::SmallVector<spirv_cross::Resource>& resources, spirv_cross::CompilerGLSL& compiler);
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