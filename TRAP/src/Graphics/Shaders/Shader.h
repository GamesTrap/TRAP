#ifndef TRAP_SHADER_H
#define TRAP_SHADER_H

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
//SPIRV
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
//SPIRV to GLSL
#include <spirv_glsl.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

#include "Core/Base.h"
#include "Application.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/DescriptorSet.h"

namespace TRAP::Graphics
{
	class UniformBuffer;
	class StorageBuffer;

	class Shader
	{
	public:
		/// @brief Struct defining a shader macro.
		struct Macro
		{
			std::string Definition{};
			std::string Value{};
		};

	protected:
		/// @brief Constructor.
		Shader(std::string name, bool valid, RendererAPI::ShaderStage stages, const std::vector<Macro>& userMacros = {}, const std::filesystem::path& filepath = "");
	public:
		/// @brief Copy constructor.
		consteval Shader(const Shader&) = delete;
		/// @brief Copy assignment operator.
		consteval Shader& operator=(const Shader&) = delete;
		/// @brief Move constructor.
		Shader(Shader&&) noexcept = default;
		/// @brief Move assignment operator.
		Shader& operator=(Shader&&) noexcept = default;
		/// @brief Destructor.
		virtual ~Shader() = default;

		/// @brief Reload shader.
		/// @return True on successful reload (valid shader), else (invalid shader) otherwise.
		bool Reload();

		/// @brief Retrieve the unique identifier of the shader.
		/// @note The ID of the shader changes when reloaded.
		[[nodiscard]] constexpr virtual usize GetID() const noexcept = 0;

		/// @brief Retrieve the name of the shader.
		/// @return Name of the shader.
		[[nodiscard]] constexpr std::string GetName() const noexcept;

		/// @brief Retrieve the file path of the shader.
		/// @return File path of the shader.
		[[nodiscard]] std::filesystem::path GetFilePath() const noexcept;

		/// @brief Retrieve the shader stages of the shader.
		/// @brief @return Shader stages of the shader.
		[[nodiscard]] constexpr RendererAPI::ShaderStage GetShaderStages() const noexcept;

		/// @brief Retrieve the used macros of the shader.
		/// @return Used macros.
		[[nodiscard]] constexpr const std::vector<Macro>& GetMacros() const noexcept;

		/// @brief Retrieve the root signature of the shader.
		/// @return Root signature of the shader.
		[[nodiscard]] TRAP::Ref<RootSignature> GetRootSignature() const noexcept;
		/// @brief Retrieve the descriptor sets of the shader.
		/// @return Descriptor sets of the shader.
		[[nodiscard]] constexpr const std::array<TRAP::Scope<DescriptorSet>, RendererAPI::MaxDescriptorSets>& GetDescriptorSets() const noexcept;

		/// @brief Retrieve whether the shader is valid (i.e. loaded and compiled) or not.
		/// @return True if shader is valid, false otherwise.
		[[nodiscard]] constexpr bool IsShaderValid() const noexcept;

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use shader for rendering on the given window.
		/// @param window Window to use the shader for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		virtual void Use(const Window& window = *TRAP::Application::GetWindow()) = 0;
#else
		/// @brief Use shader for rendering.
		/// @remark This function is only available in headless mode.
		virtual void Use() = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Use texture with this shader on the given window.
		/// @param set Descriptor set to use the texture with.
		/// @param binding Binding point of the texture.
		/// @param texture Texture to use.
		/// @param window Window to use the shader for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		virtual void UseTexture(u32 set, u32 binding, const TRAP::Graphics::Texture& texture,
		                        const Window& window = *TRAP::Application::GetWindow()) const = 0;
#else
		/// @brief Use texture with this shader.
		/// @param set Descriptor set to use the texture with.
		/// @param binding Binding point of the texture.
		/// @param texture Texture to use.
		/// @remark This function is only available in headless mode.
		virtual void UseTexture(u32 set, u32 binding, const TRAP::Graphics::Texture& texture) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use multiple textures with this shader on the given window.
		/// @param set Descriptor set to use the textures with.
		/// @param binding Binding point of the textures.
		/// @param textures Textures to use.
		/// @param window Window to use the shader for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		virtual void UseTextures(u32 set, u32 binding,
		                         const std::vector<const TRAP::Graphics::Texture*>& textures,
								 const Window* window = TRAP::Application::GetWindow()) const = 0;
#else
		/// @brief Use multiple textures with this shader.
		/// @param set Descriptor set to use the textures with.
		/// @param binding Binding point of the textures.
		/// @param textures Textures to use.
		/// @remark This function is only available in headless mode.
		virtual void UseTextures(u32 set, u32 binding,
		                         const std::vector<const TRAP::Graphics::Texture*>& textures) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use sampler with this shader on the given window.
		/// @param set Descriptor set to use the sampler with.
		/// @param binding Binding point of the sampler.
		/// @param sampler Sampler to use.
		/// @param window Window to use the shader for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		virtual void UseSampler(u32 set, u32 binding, TRAP::Graphics::Sampler* sampler,
		                        const Window* window = TRAP::Application::GetWindow()) const = 0;
#else
		/// @brief Use sampler with this shader.
		/// @param set Descriptor set to use the sampler with.
		/// @param binding Binding point of the sampler.
		/// @param sampler Sampler to use.
		/// @remark This function is only available in headless mode.
		virtual void UseSampler(u32 set, u32 binding, TRAP::Graphics::Sampler* sampler) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// @brief Use multiple samplers with this shader on the given window.
		/// @param set Descriptor set to use the samplers with.
		/// @param binding Binding point of the samplers.
		/// @param samplers Samplers to use.
		/// @param window Window to use the shader for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		virtual void UseSamplers(u32 set, u32 binding,
		                         const std::vector<TRAP::Graphics::Sampler*>& samplers,
								 const Window* window = TRAP::Application::GetWindow()) const = 0;
#else
		/// @brief Use multiple samplers with this shader.
		/// @param set Descriptor set to use the samplers with.
		/// @param binding Binding point of the samplers.
		/// @param samplers Samplers to use.
		/// @remark This function is only available in headless mode.
		virtual void UseSamplers(u32 set, u32 binding,
		                         const std::vector<TRAP::Graphics::Sampler*>& samplers) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use uniform buffer object with this shader on the given window.
		/// @param set Descriptor set to use the UBO with.
		/// @param binding Binding point of the UBO.
		/// @param uniformBuffer Uniform buffer to use.
		/// @param size Size of the UBO.
		/// @param offset Offset of the UBO.
		/// @param window Window to use the shader for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		virtual void UseUBO(u32 set, u32 binding, const TRAP::Graphics::UniformBuffer* uniformBuffer,
							u64 size = 0, u64 offset = 0, const Window* window = TRAP::Application::GetWindow()) const = 0;
#else
		/// @brief Use uniform buffer object with this shader.
		/// @param set Descriptor set to use the UBO with.
		/// @param binding Binding point of the UBO.
		/// @param uniformBuffer Uniform buffer to use.
		/// @param size Size of the UBO.
		/// @param offset Offset of the UBO.
		/// @remark This function is only available in headless mode.
		virtual void UseUBO(u32 set, u32 binding, const TRAP::Graphics::UniformBuffer* uniformBuffer,
							u64 size = 0, u64 offset = 0) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use shader storage buffer object with this shader on the given window.
		/// @param set Descriptor set to use the SSBO with.
		/// @param binding Binding point of the SSBO.
		/// @param storageBuffer Storage buffer to use.
		/// @param size Size of the SSBO.
		/// @param window Window to use the shader for. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		virtual void UseSSBO(u32 set, u32 binding, const TRAP::Graphics::StorageBuffer* storageBuffer,
							 u64 size = 0, const Window* window = TRAP::Application::GetWindow()) const = 0;
#else
		/// @brief Use shader storage buffer object with this shader.
		/// @param set Descriptor set to use the SSBO with.
		/// @param binding Binding point of the SSBO.
		/// @param storageBuffer Storage buffer to use.
		/// @param size Size of the SSBO.
		/// @remark This function is only available in headless mode.
		virtual void UseSSBO(u32 set, u32 binding, const TRAP::Graphics::StorageBuffer* storageBuffer,
							 u64 size = 0) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Retrieve the shaders thread count per work group.
		/// @return Shaders thread count per work group.
		[[nodiscard]] virtual constexpr std::array<u32, 3> GetNumThreadsPerGroup() const noexcept = 0;

		/// @brief Create a shader from file.
		/// @param name Name for the shader.
		/// @param filePath File path of the shader.
		/// @param userMacros Optional user defined macros. Default: nullptr.
		/// @return Loaded Shader on success, Fallback Shader otherwise.
		[[nodiscard]] static Ref<Shader> CreateFromFile(const std::string& name, const std::filesystem::path& filePath,
		                                                const std::vector<Macro>& userMacros = {});
		/// @brief Create a shader from file.
		/// File name will be used as the shader name.
		/// @param filePath File path of the shader.
		/// @param userMacros Optional user defined macros. Default: nullptr.
		/// @return Loaded Shader on success, Fallback Shader otherwise.
		[[nodiscard]] static Ref<Shader> CreateFromFile(const std::filesystem::path& filePath,
		                                                const std::vector<Macro>& userMacros = {});
		/// @brief Create a shader from GLSL source.
		/// @param name Name for the shader.
		/// @param glslSource GLSL Source code.
		/// @param userMacros Optional user defined macros. Default: nullptr.
		/// @return Loaded Shader on success, Fallback Shader otherwise.
		[[nodiscard]] static Ref<Shader> CreateFromSource(const std::string& name, const std::string& glslSource,
		                                                  const std::vector<Macro>& userMacros = {});

		static constexpr std::array<std::string_view, 3> SupportedShaderFormatSuffixes{".shader", ".glsl", ".tp-spv"};
		static constexpr u32 SPIRVMagicNumber = 0x07230203u;
		static constexpr std::string_view ShaderMagicNumber = "TRAP_SPV";
		static constexpr usize ShaderHeaderOffset = ShaderMagicNumber.size() + sizeof(u32) +
		                                                         sizeof(u8) + sizeof(usize) + sizeof(u8);

	protected:
		/// @brief Initialize API dependent shader.
		/// @param desc Binary shader description.
		virtual void Init(const RendererAPI::BinaryShaderDesc& desc) = 0;
		/// @brief Shutdown API dependent shader.
		virtual void Shutdown() = 0;

		std::string m_name;
		std::filesystem::path m_filepath;
		RendererAPI::ShaderStage m_shaderStages{};
		TRAP::Ref<RootSignature> m_rootSignature;
		std::array<TRAP::Scope<DescriptorSet>, RendererAPI::MaxDescriptorSets> m_descriptorSets{};
		std::vector<Macro> m_macros;
		bool m_valid{};

	private:
		/// @brief Check if given file contains the TRAP Shader magic number.
		/// @param filePath File path of the shader.
		/// @return True if file has TRAP Shader magic number, false otherwise.
		[[nodiscard]] static bool CheckTRAPShaderMagicNumber(const std::filesystem::path& filePath);

		/// @brief Pre process GLSL source code.
		///
		/// 1. Splits different shader types like vertex, fragment, etc.
		/// 2. Checks for duplicate shader types.
		/// 3. Automatically adds #version directive.
		/// 4. Inject default macros.
		/// 4. Inject user defined macros.
		/// @param glslSource GLSL source code.
		/// @param shaders Output: Splitted shaders.
		/// @param userMacros Optional: User defined macros.
		/// @return True if pre processing was successful, false otherwise.
		[[nodiscard]] static bool PreProcessGLSL(const std::string& glslSource,
		                                         std::vector<std::pair<std::string, RendererAPI::ShaderStage>>& shaders,
								                 std::span<const Macro> userMacros);
		/// @brief Parse a glslang::TShader object.
		/// @param shader glslang::TShader object.
		/// @return True if parsing was successful, false otherwise.
		[[nodiscard]] static bool ParseGLSLang(glslang::TShader& shader);
		/// @brief Link a glslang::TShader to a glslang::TProgram object.
		/// @param shader glslang::TShader object.
		/// @param program glslang::TProgram object to link with.
		/// @return True if linking was successful, false otherwise.
		[[nodiscard]] static bool LinkGLSLang(glslang::TShader& shader, glslang::TProgram& program);
		/// @brief Validate that the given shader stages are a valid combination.
		///
		/// 1. Checks if Rasterizer shaders are combined with Compute shader.
		/// 2. Checks if Rasterizer shaders are combined with RayTracing shaders
		/// 3. Checks if Compute shader is combined with RayTracing shaders.
		/// 4. Checks if Vertex and Fragment shaders are combined.
		/// @param shaders Shader stages to validate.
		/// @return True if validation was successful, false otherwise.
		[[nodiscard]] static constexpr bool ValidateShaderStages(const std::vector<std::pair<std::string, RendererAPI::ShaderStage>>& shaders);
		/// @brief Convert GLSL shaders to SPIRV.
		/// @param shaders GLSL shader(s) to convert.
		/// @return RendererAPI::BinaryShaderDesc containing SPIRV binary data.
		[[nodiscard]] static RendererAPI::BinaryShaderDesc ConvertGLSLToSPIRV(const std::vector<std::pair<std::string, RendererAPI::ShaderStage>>& shaders);
		/// @brief Convert a glslang::TProgram object to SPIRV binary data.
		/// @param stage Shader stage to convert.
		/// @param program glslang::TProgram object to convert.
		/// @return SPIRV binary data on success, empty vector otherwise.
		[[nodiscard]] static std::vector<u32> ConvertToSPIRV(RendererAPI::ShaderStage stage,
		                                                          glslang::TProgram& program);
		/// @brief Load SPIRV binary data into RendererAPI::BinaryShaderDesc.
		/// @param SPIRV SPIRV binary data.
		/// @return RendererAPI::BinaryShaderDesc containing loaded SPIRV binary data.
		[[nodiscard]] static RendererAPI::BinaryShaderDesc LoadSPIRV(std::vector<u8>& SPIRV);
		/// @brief Check if the ending of the given path is a supported shader file ending.
		/// @param filePath File path to check.
		/// @return True if file ending is supported, false otherwise.
		[[nodiscard]] static bool IsFileEndingSupported(const std::filesystem::path& filePath);
		/// @brief Shader pre initialization.
		///
		/// 1. Checks if file type is supported shader type.
		/// 2. Loads shader file from disk.
		/// 3. If GLSL convert to SPIRV else loads SPIRV code.
		/// 4. Check for invalid shader stages.
		/// @param name Name of the shader.
		/// @param filePath File path of the shader.
		/// @param userMacros Optional user provided macros.
		/// @param outShaderDesc Output binary shader description.
		/// @param outFailShader Optional Output used if pre initialization failed.
		/// @return True on successful pre initialization, false otherwise.
		/// If false outFailShader may be filled with a fail shader.
		[[nodiscard]] static bool PreInit(const std::string& name, const std::filesystem::path& filePath, const std::vector<Macro>& userMacros, RendererAPI::BinaryShaderDesc& outShaderDesc, Ref<Shader>& outFailShader);

		inline constinit static bool s_glslangInitialized = false;

		//Macros which are always provided by default.
		inline static std::array<Macro, 2> s_defaultShaderMacrosVulkan
		{
			{
				{"UpdateFreqStatic", "set = 0"},
				{"UpdateFreqDynamic", "set = 1"}
			}
		};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Graphics::Shader::GetName() const noexcept
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::ShaderStage TRAP::Graphics::Shader::GetShaderStages() const noexcept
{
	return m_shaderStages;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Graphics::Shader::Macro>& TRAP::Graphics::Shader::GetMacros() const noexcept
{
	return m_macros;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::array<TRAP::Scope<TRAP::Graphics::DescriptorSet>,
                                         TRAP::Graphics::RendererAPI::MaxDescriptorSets>& TRAP::Graphics::Shader::GetDescriptorSets() const noexcept
{
	return m_descriptorSets;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Graphics::Shader::IsShaderValid() const noexcept
{
	return m_valid;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Graphics::Shader::ValidateShaderStages(const std::vector<std::pair<std::string, RendererAPI::ShaderStage>>& shaders)
{
	RendererAPI::ShaderStage combinedStages = RendererAPI::ShaderStage::None;
	for(const auto& [glsl, stage] : shaders)
		combinedStages |= stage;

	//Check if any Shader Stage is set
	if (RendererAPI::ShaderStage::None == combinedStages)
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "No shader stage found!");
		return false;
	}

	//Check for "Normal" Shader Stages combined with Compute
	if((((RendererAPI::ShaderStage::Vertex & combinedStages) != RendererAPI::ShaderStage::None) ||
		((RendererAPI::ShaderStage::Fragment & combinedStages) != RendererAPI::ShaderStage::None) ||
		((RendererAPI::ShaderStage::TessellationControl & combinedStages) != RendererAPI::ShaderStage::None) ||
		((RendererAPI::ShaderStage::TessellationEvaluation & combinedStages) != RendererAPI::ShaderStage::None) ||
		((RendererAPI::ShaderStage::Geometry & combinedStages) != RendererAPI::ShaderStage::None)) &&
		((RendererAPI::ShaderStage::Compute & combinedStages) != RendererAPI::ShaderStage::None))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Rasterizer shader stages combined with compute stage!");
		return false;
	}

	//Check for "Normal" Shader Stages combined with RayTracing
	if((((RendererAPI::ShaderStage::Vertex & combinedStages) != RendererAPI::ShaderStage::None) ||
	    ((RendererAPI::ShaderStage::Fragment & combinedStages) != RendererAPI::ShaderStage::None) ||
		((RendererAPI::ShaderStage::TessellationControl & combinedStages) != RendererAPI::ShaderStage::None) ||
		((RendererAPI::ShaderStage::TessellationEvaluation & combinedStages) != RendererAPI::ShaderStage::None) ||
		((RendererAPI::ShaderStage::Geometry & combinedStages) != RendererAPI::ShaderStage::None)) &&
		((RendererAPI::ShaderStage::RayTracing & combinedStages) != RendererAPI::ShaderStage::None))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Rasterizer shader stages combined with ray tracing stage!");
		return false;
	}

	//Check for Compute Shader Stage combined with RayTracing
	if (((RendererAPI::ShaderStage::Compute & combinedStages) != RendererAPI::ShaderStage::None) &&
		((RendererAPI::ShaderStage::RayTracing & combinedStages) != RendererAPI::ShaderStage::None))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Compute shader stage combined with ray tracing stage!");
		return false;
	}

	//Check for Vertex Shader Stage & required Fragment/Pixel Shader Stage
	if(((RendererAPI::ShaderStage::Vertex & combinedStages) != RendererAPI::ShaderStage::None) &&
	   (((RendererAPI::ShaderStage::Fragment & combinedStages)) == RendererAPI::ShaderStage::None))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Only vertex shader stage provided! Missing fragment/pixel shader stage");
		return false;
	}
	//Check for Fragment/Pixel Shader Stage & required Vertex Shader Stage
	if(((RendererAPI::ShaderStage::Fragment & combinedStages) != RendererAPI::ShaderStage::None) &&
	   (((RendererAPI::ShaderStage::Vertex & combinedStages)) == RendererAPI::ShaderStage::None))
	{
		TP_ERROR(Log::ShaderGLSLPrefix, "Only fragment/pixel shader stage provided! Missing vertex shader stage");
		return false;
	}

	//Shader Stages should be valid
	return true;
}

#endif /*TRAP_SHADER_H*/