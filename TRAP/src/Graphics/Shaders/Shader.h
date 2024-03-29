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
		/// <summary>
		/// Struct defining a shader macro.
		/// </summary>
		struct Macro
		{
			std::string Definition{};
			std::string Value{};
		};

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		Shader(std::string name, bool valid, RendererAPI::ShaderStage stages, const std::vector<Macro>* userMacros = nullptr, const std::filesystem::path& filepath = "");
	public:
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr Shader(const Shader&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr Shader& operator=(const Shader&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Shader(Shader&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Shader& operator=(Shader&&) noexcept = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Shader() = default;

		/// <summary>
		/// Reload shader.
		/// </summary>
		/// <returns>True on successful reload (valid shader), else (invalid shader) otherwise.</returns>
		bool Reload();

		/// <summary>
		/// Retrieve the name of the shader.
		/// </summary>
		/// <returns>Name of the shader.</returns>
		[[nodiscard]] constexpr std::string GetName() const noexcept;

		/// <summary>
		/// Retrieve the file path of the shader.
		/// </summary>
		/// <returns>File path of the shader.</returns>
		[[nodiscard]] std::filesystem::path GetFilePath() const noexcept;

		/// <summary>
		/// Retrieve the shader stages of the shader.
		/// <summary>
		/// <returns>Shader stages of the shader.</returns>
		[[nodiscard]] constexpr RendererAPI::ShaderStage GetShaderStages() const noexcept;

		/// <summary>
		/// Retrieve the used macros of the shader.
		/// </summary>
		/// <returns>Used macros.</returns>
		[[nodiscard]] constexpr const std::vector<Macro>& GetMacros() const noexcept;

		/// <summary>
		/// Retrieve the root signature of the shader.
		/// </summary>
		/// <returns>Root signature of the shader.</returns>
		[[nodiscard]] TRAP::Ref<RootSignature> GetRootSignature() const noexcept;
		/// <summary>
		/// Retrieve the descriptor sets of the shader.
		/// </summary>
		/// <returns>Descriptor sets of the shader.</returns>
		[[nodiscard]] constexpr const std::array<TRAP::Scope<DescriptorSet>, RendererAPI::MaxDescriptorSets>& GetDescriptorSets() const noexcept;

		/// <summary>
		/// Retrieve whether the shader is valid (i.e. loaded and compiled) or not.
		/// </summary>
		/// <returns>True if shader is valid, false otherwise.</returns>
		[[nodiscard]] constexpr bool IsShaderValid() const noexcept;

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Use shader for rendering on the given window.
		/// </summary>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		virtual void Use(const Window* window = TRAP::Application::GetWindow()) = 0;
#else
		/// <summary>
		/// Use shader for rendering.
		/// </summary>
		virtual void Use() = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Use texture with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the texture with.</param>
		/// <param name="binding">Binding point of the texture.</param>
		/// <param name="texture">Texture to use.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		virtual void UseTexture(u32 set, u32 binding, Ref<TRAP::Graphics::Texture> texture,
		                        const Window* window = TRAP::Application::GetWindow()) const = 0;
#else
		/// <summary>
		/// Use texture with this shader.
		/// </summary>
		/// <param name="set">Descriptor set to use the texture with.</param>
		/// <param name="binding">Binding point of the texture.</param>
		/// <param name="texture">Texture to use.</param>
		virtual void UseTexture(u32 set, u32 binding, Ref<TRAP::Graphics::Texture> texture) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Use multiple textures with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the textures with.</param>
		/// <param name="binding">Binding point of the textures.</param>
		/// <param name="textures">Textures to use.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		virtual void UseTextures(u32 set, u32 binding,
		                         const std::vector<Ref<TRAP::Graphics::Texture>>& textures,
								 const Window* window = TRAP::Application::GetWindow()) const = 0;
#else
		/// <summary>
		/// Use multiple textures with this shader.
		/// </summary>
		/// <param name="set">Descriptor set to use the textures with.</param>
		/// <param name="binding">Binding point of the textures.</param>
		/// <param name="textures">Textures to use.</param>
		virtual void UseTextures(u32 set, u32 binding,
		                         const std::vector<Ref<TRAP::Graphics::Texture>>& textures) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Use sampler with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the sampler with.</param>
		/// <param name="binding">Binding point of the sampler.</param>
		/// <param name="sampler">Sampler to use.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		virtual void UseSampler(u32 set, u32 binding, TRAP::Graphics::Sampler* sampler,
		                        const Window* window = TRAP::Application::GetWindow()) const = 0;
#else
		/// <summary>
		/// Use sampler with this shader.
		/// </summary>
		/// <param name="set">Descriptor set to use the sampler with.</param>
		/// <param name="binding">Binding point of the sampler.</param>
		/// <param name="sampler">Sampler to use.</param>
		virtual void UseSampler(u32 set, u32 binding, TRAP::Graphics::Sampler* sampler) const = 0;
#endif /*TRAP_HEADLESS_MODE*/
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Use multiple samplers with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the samplers with.</param>
		/// <param name="binding">Binding point of the samplers.</param>
		/// <param name="samplers">Samplers to use.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		virtual void UseSamplers(u32 set, u32 binding,
		                         const std::vector<TRAP::Graphics::Sampler*>& samplers,
								 const Window* window = TRAP::Application::GetWindow()) const = 0;
#else
		/// <summary>
		/// Use multiple samplers with this shader.
		/// </summary>
		/// <param name="set">Descriptor set to use the samplers with.</param>
		/// <param name="binding">Binding point of the samplers.</param>
		/// <param name="samplers">Samplers to use.</param>
		virtual void UseSamplers(u32 set, u32 binding,
		                         const std::vector<TRAP::Graphics::Sampler*>& samplers) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Use uniform buffer object with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the UBO with.</param>
		/// <param name="binding">Binding point of the UBO.</param>
		/// <param name="uniformBuffer">Uniform buffer to use.</param>
		/// <param name="size">Size of the UBO.</param>
		/// <param name="offset">Offset of the UBO.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		virtual void UseUBO(u32 set, u32 binding, const TRAP::Graphics::UniformBuffer* uniformBuffer,
							u64 size = 0, u64 offset = 0, const Window* window = TRAP::Application::GetWindow()) const = 0;
#else
		/// <summary>
		/// Use uniform buffer object with this shader.
		/// </summary>
		/// <param name="set">Descriptor set to use the UBO with.</param>
		/// <param name="binding">Binding point of the UBO.</param>
		/// <param name="uniformBuffer">Uniform buffer to use.</param>
		/// <param name="size">Size of the UBO.</param>
		/// <param name="offset">Offset of the UBO.</param>
		virtual void UseUBO(u32 set, u32 binding, const TRAP::Graphics::UniformBuffer* uniformBuffer,
							u64 size = 0, u64 offset = 0) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Use shader storage buffer object with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the SSBO with.</param>
		/// <param name="binding">Binding point of the SSBO.</param>
		/// <param name="storageBuffer">Storage buffer to use.</param>
		/// <param name="size">Size of the SSBO.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		virtual void UseSSBO(u32 set, u32 binding, const TRAP::Graphics::StorageBuffer* storageBuffer,
							 u64 size = 0, const Window* window = TRAP::Application::GetWindow()) const = 0;
#else
		/// <summary>
		/// Use shader storage buffer object with this shader.
		/// </summary>
		/// <param name="set">Descriptor set to use the SSBO with.</param>
		/// <param name="binding">Binding point of the SSBO.</param>
		/// <param name="storageBuffer">Storage buffer to use.</param>
		/// <param name="size">Size of the SSBO.</param>
		virtual void UseSSBO(u32 set, u32 binding, const TRAP::Graphics::StorageBuffer* storageBuffer,
							 u64 size = 0) const = 0;
#endif /*TRAP_HEADLESS_MODE*/

		/// <summary>
		/// Retrieve the shaders thread count per work group.
		/// </summary>
		/// <returns>Shaders thread count per work group.</returns>
		[[nodiscard]] virtual constexpr std::array<u32, 3> GetNumThreadsPerGroup() const noexcept = 0;

		/// <summary>
		/// Create a shader from file.
		/// </summary>
		/// <param name="name">Name for the shader.</param>
		/// <param name="filePath">File path of the shader.</param>
		/// <param name="userMacros">Optional user defined macros. Default: nullptr.</param>
		/// <returns>Loaded Shader on success, Fallback Shader otherwise.</returns>
		[[nodiscard]] static Ref<Shader> CreateFromFile(const std::string& name, const std::filesystem::path& filePath,
		                                                const std::vector<Macro>* userMacros = nullptr);
		/// <summary>
		/// Create a shader from file.
		/// File name will be used as the shader name.
		/// </summary>
		/// <param name="filePath">File path of the shader.</param>
		/// <param name="userMacros">Optional user defined macros. Default: nullptr.</param>
		/// <returns>Loaded Shader on success, Fallback Shader otherwise.</returns>
		[[nodiscard]] static Ref<Shader> CreateFromFile(const std::filesystem::path& filePath,
		                                                const std::vector<Macro>* userMacros = nullptr);
		/// <summary>
		/// Create a shader from GLSL source.
		/// </summary>
		/// <param name="name">Name for the shader.</param>
		/// <param name="glslSource">GLSL Source code.</param>
		/// <param name="userMacros">Optional user defined macros. Default: nullptr.</param>
		/// <returns>Loaded Shader on success, Fallback Shader otherwise.</returns>
		[[nodiscard]] static Ref<Shader> CreateFromSource(const std::string& name, const std::string& glslSource,
		                                                  const std::vector<Macro>* userMacros = nullptr);

		inline static constexpr std::array<std::string_view, 3> SupportedShaderFormatSuffixes{".shader", ".glsl", ".tp-spv"};
		inline static constexpr u32 SPIRVMagicNumber = 0x07230203u;
		inline static constexpr std::string_view ShaderMagicNumber = "TRAP_SPV";
		inline static constexpr usize ShaderHeaderOffset = ShaderMagicNumber.size() + sizeof(u32) +
		                                                         sizeof(u8) + sizeof(usize) + sizeof(u8);

	protected:
		/// <summary>
		/// Initialize API dependent shader.
		/// </summary>
		/// <param name="desc">Binary shader description.</param>
		virtual void Init(const RendererAPI::BinaryShaderDesc& desc) = 0;
		/// <summary>
		/// Shutdown API dependent shader.
		/// </summary>
		virtual void Shutdown() = 0;

		std::string m_name;
		std::filesystem::path m_filepath;
		RendererAPI::ShaderStage m_shaderStages{};
		TRAP::Ref<RootSignature> m_rootSignature;
		std::array<TRAP::Scope<DescriptorSet>, RendererAPI::MaxDescriptorSets> m_descriptorSets{};
		std::vector<Macro> m_macros;
		bool m_valid{};

	private:
		/// <summary>
		/// Check if given file contains the TRAP Shader magic number.
		/// </summary>
		/// <param name="filePath">File path of the shader.</param>
		/// <returns>True if file has TRAP Shader magic number, false otherwise.</returns>
		[[nodiscard]] static bool CheckTRAPShaderMagicNumber(const std::filesystem::path& filePath);

		/// <summary>
		/// Pre process GLSL source code.
		///
		/// 1. Splits different shader types like vertex, fragment, etc.
		/// 2. Checks for duplicate shader types.
		/// 3. Automatically adds #version directive.
		/// 4. Inject default macros.
		/// 4. Inject user defined macros.
		/// </summary>
		/// <param name="glslSource">GLSL source code.</param>
		/// <param name="shaders"Output: Splitted shaders.</param>
		/// <param name="userMacros">Optional: User defined macros.</param>
		/// <returns>True if pre processing was successful, false otherwise.</returns>
		[[nodiscard]] static bool PreProcessGLSL(const std::string& glslSource,
		                                         std::vector<std::pair<std::string, RendererAPI::ShaderStage>>& shaders,
								                 const std::vector<Macro>* userMacros);
		/// <summary>
		/// Parse a glslang::TShader object.
		/// </summary>
		/// <param name="shader">glslang::TShader object.</param>
		/// <returns>True if parsing was successful, false otherwise.</returns>
		[[nodiscard]] static bool ParseGLSLang(glslang::TShader& shader);
		/// <summary>
		/// Link a glslang::TShader to a glslang::TProgram object.
		/// </summary>
		/// <param name="shader">glslang::TShader object.</param>
		/// <param name="program">glslang::TProgram object to link with.</param>
		/// <returns>True if linking was successful, false otherwise.</returns>
		[[nodiscard]] static bool LinkGLSLang(glslang::TShader& shader, glslang::TProgram& program);
		/// <summary>
		/// Validate that the given shader stages are a valid combination.
		///
		/// 1. Checks if Rasterizer shaders are combined with Compute shader.
		/// 2. Checks if Rasterizer shaders are combined with RayTracing shaders
		/// 3. Checks if Compute shader is combined with RayTracing shaders.
		/// 4. Checks if Vertex and Fragment shaders are combined.
		/// </summary>
		/// <param name="shaders">Shader stages to validate.</param>
		/// <returns>True if validation was successful, false otherwise.</returns>
		[[nodiscard]] static constexpr bool ValidateShaderStages(const std::vector<std::pair<std::string, RendererAPI::ShaderStage>>& shaders);
		/// <summary>
		/// Convert GLSL shaders to SPIRV.
		/// </summary>
		/// <param name="shaders">GLSL shader(s) to convert.</param>
		/// <returns>RendererAPI::BinaryShaderDesc containing SPIRV binary data.</returns>
		[[nodiscard]] static RendererAPI::BinaryShaderDesc ConvertGLSLToSPIRV(const std::vector<std::pair<std::string, RendererAPI::ShaderStage>>& shaders);
		/// <summary>
		/// Convert a glslang::TProgram object to SPIRV binary data.
		/// </summary>
		/// <param name="stage">Shader stage to convert.</param>
		/// <param name="program">glslang::TProgram object to convert.</param>
		/// <returns>SPIRV binary data on success, empty vector otherwise.</returns>
		[[nodiscard]] static std::vector<u32> ConvertToSPIRV(RendererAPI::ShaderStage stage,
		                                                          glslang::TProgram& program);
		/// <summary>
		/// Load SPIRV binary data into RendererAPI::BinaryShaderDesc.
		/// </summary>
		/// <param name="SPIRV">SPIRV binary data.</param>
		/// <returns>RendererAPI::BinaryShaderDesc containing loaded SPIRV binary data.</returns>
		[[nodiscard]] static RendererAPI::BinaryShaderDesc LoadSPIRV(std::vector<u8>& SPIRV);
		/// <summary>
		/// Check if the ending of the given path is a supported shader file ending.
		/// </summary>
		/// <param name="filePath">File path to check.</param>
		/// <returns>True if file ending is supported, false otherwise.</returns>
		[[nodiscard]] static bool IsFileEndingSupported(const std::filesystem::path& filePath);
		/// <summary>
		/// Shader pre initialization.
		///
		/// 1. Checks if file type is supported shader type.
		/// 2. Loads shader file from disk.
		/// 3. If GLSL convert to SPIRV else loads SPIRV code.
		/// 4. Check for invalid shader stages.
		/// </summary>
		/// <param name="name">Name of the shader.</param>
		/// <param name="filePath">File path of the shader.</param>
		/// <param name="userMacros">Optional user provided macros.</param>
		/// <param name="outShaderDesc">Output binary shader description.</param>
		/// <param name="outFailShader">Optional Output used if pre initialization failed.</param>
		/// <returns>
		/// True on successful pre initialization, false otherwise.
		/// If false outFailShader may be filled with a fail shader.
		/// </returns>
		[[nodiscard]] static bool PreInit(const std::string& name, const std::filesystem::path& filePath, const std::vector<Macro>* userMacros, RendererAPI::BinaryShaderDesc& outShaderDesc, Ref<Shader>& outFailShader);

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