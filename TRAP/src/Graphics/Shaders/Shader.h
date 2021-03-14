#ifndef _TRAP_SHADER_H_
#define _TRAP_SHADER_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Shader
	{
	public:
		struct Macro
		{
			std::string Definition{};
			std::string Value{};
		};

	protected:
		Shader();
	public:
		Shader(const Shader&) = default;
		Shader& operator=(const Shader&) = default;
		Shader(Shader&&) = default;
		Shader& operator=(Shader&&) = default;
		virtual ~Shader();

		const std::string& GetName() const;

		const std::string& GetFilePath() const;

		RendererAPI::ShaderStage GetShaderStages() const;

		virtual void Use(Window* window = nullptr) = 0;
		
		static Scope<Shader> CreateFromFile(const std::string& name, const std::string& filePath, const std::vector<Macro>* userMacros = nullptr);
		static Scope<Shader> CreateFromFile(const std::string& filepath, const std::vector<Macro>* userMacros = nullptr);
		static Scope<Shader> CreateFromSource(const std::string& name, const std::string& glslSource, const std::vector<Macro>* userMacros = nullptr);

	protected:
		std::string m_name;
		std::string m_filepath;
		RendererAPI::ShaderStage m_shaderStages{};
		
	private:
		static bool CheckSPIRVMagicNumber(std::string_view filePath);
		static std::vector<uint32_t> Convert8To32(const std::vector<uint8_t>& source);

		static bool PreProcessGLSL(const std::string& glslSource,
		                           std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)>& shaders,
		                           RendererAPI::ShaderStage& shaderStages,
								   const std::vector<Macro>* userMacros);
		static TRAP::Scope<glslang::TShader> PreProcessGLSLForConversion(const char* source, RendererAPI::ShaderStage stage, std::string& preProcessedSource);
		static bool ParseGLSLang(glslang::TShader* shader);
		static bool LinkGLSLang(glslang::TShader* shader, glslang::TProgram& program);
		static bool ValidateShaderStages(const RendererAPI::ShaderStage& shaderStages);
		static RendererAPI::BinaryShaderDesc ConvertGLSLToSPIRV(const std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)>& shaders,
									   const RendererAPI::ShaderStage& shaderStages);
		static std::vector<uint32_t> ConvertToSPIRV(glslang::TShader* shader, RendererAPI::ShaderStage stage, glslang::TProgram& program);
		static RendererAPI::BinaryShaderDesc LoadSPIRV(const std::vector<uint32_t>& SPIRV);

		static bool s_glslangInitialized;

		inline static std::array<Macro, 4> s_defaultShaderMacrosVulkan
		{
			{
				{"UpdateFreqNone", "set = 0"},
				{"UpdateFreqPerFrame", "set = 1"},
				{"UpdateFreqPerBatch", "set = 2"},
				{"UpdateFreqPerDraw", "set = 3"}
			}
		};
	};
}

#endif /*_TRAP_SHADER_H_*/