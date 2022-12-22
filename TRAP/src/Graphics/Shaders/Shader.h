#ifndef TRAP_SHADER_H
#define TRAP_SHADER_H

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
//SPIRV
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
//SPIRV to GLSL
#include <spirv_glsl.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#include "Application.h"
#include "Graphics/API/RendererAPI.h"

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
		Shader() noexcept = default;
	public:
		/// <summary>
		/// Copy constructor.
		/// </summary>
		Shader(const Shader&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Shader& operator=(const Shader&) = delete;
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
		[[nodiscard]] std::string GetName() const noexcept;

		/// <summary>
		/// Retrieve the file path of the shader.
		/// </summary>
		/// <returns>File path of the shader.</returns>
		[[nodiscard]] std::filesystem::path GetFilePath() const noexcept;

		/// <summary>
		/// Retrieve the shader stages of the shader.
		/// <summary>
		/// <returns>Shader stages of the shader.</returns>
		[[nodiscard]] RendererAPI::ShaderStage GetShaderStages() const noexcept;

		/// <summary>
		/// Retrieve the used macros of the shader.
		/// </summary>
		/// <returns>Used macros.</returns>
		[[nodiscard]] const std::vector<Macro>& GetMacros() const noexcept;

		/// <summary>
		/// Retrieve the root signature of the shader.
		/// </summary>
		/// <returns>Root signature of the shader.</returns>
		[[nodiscard]] TRAP::Ref<RootSignature> GetRootSignature() const noexcept;
		/// <summary>
		/// Retrieve the descriptor sets of the shader.
		/// </summary>
		/// <returns>Descriptor sets of the shader.</returns>
		[[nodiscard]] const std::array<TRAP::Scope<DescriptorSet>, RendererAPI::MaxDescriptorSets>& GetDescriptorSets() const noexcept;

		/// <summary>
		/// Retrieve whether the shader is valid (i.e. loaded and compiled) or not.
		/// </summary>
		/// <returns>True if shader is valid, false otherwise.</returns>
		[[nodiscard]] bool IsShaderValid() const noexcept;

		/// <summary>
		/// Use shader for rendering on the given window.
		/// </summary>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		virtual void Use(const Window* const window = TRAP::Application::GetWindow()) = 0;
		/// <summary>
		/// Use texture with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the texture with.</param>
		/// <param name="binding">Binding point of the texture.</param>
		/// <param name="texture">Texture to use.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		virtual void UseTexture(uint32_t set, uint32_t binding, Ref<TRAP::Graphics::Texture> texture,
		                        const Window* const window = TRAP::Application::GetWindow()) const = 0;
		/// <summary>
		/// Use multiple textures with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the textures with.</param>
		/// <param name="binding">Binding point of the textures.</param>
		/// <param name="textures">Textures to use.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		virtual void UseTextures(uint32_t set, uint32_t binding,
		                         const std::vector<Ref<TRAP::Graphics::Texture>>& textures,
								 const Window* const window = TRAP::Application::GetWindow()) const = 0;
		/// <summary>
		/// Use sampler with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the sampler with.</param>
		/// <param name="binding">Binding point of the sampler.</param>
		/// <param name="sampler">Sampler to use.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		virtual void UseSampler(uint32_t set, uint32_t binding, TRAP::Graphics::Sampler* const sampler,
		                        const Window* const window = TRAP::Application::GetWindow()) const = 0;
		/// <summary>
		/// Use multiple samplers with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the samplers with.</param>
		/// <param name="binding">Binding point of the samplers.</param>
		/// <param name="samplers">Samplers to use.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		virtual void UseSamplers(uint32_t set, uint32_t binding,
		                         const std::vector<TRAP::Graphics::Sampler*>& samplers,
								 const Window* const window = TRAP::Application::GetWindow()) const = 0;
		/// <summary>
		/// Use uniform buffer object with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the UBO with.</param>
		/// <param name="binding">Binding point of the UBO.</param>
		/// <param name="uniformBuffer">Uniform buffer to use.</param>
		/// <param name="size">Size of the UBO.</param>
		/// <param name="offset">Offset of the UBO.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		virtual void UseUBO(uint32_t set, uint32_t binding, const TRAP::Graphics::UniformBuffer* const uniformBuffer,
							uint64_t size = 0, uint64_t offset = 0, const Window* const window = TRAP::Application::GetWindow()) const = 0;
		/// <summary>
		/// Use shader storage buffer object with this shader on the given window.
		/// </summary>
		/// <param name="set">Descriptor set to use the SSBO with.</param>
		/// <param name="binding">Binding point of the SSBO.</param>
		/// <param name="storageBuffer">Storage buffer to use.</param>
		/// <param name="size">Size of the SSBO.</param>
		/// <param name="window">Window to use the shader for. Default: Main Window.</param>
		virtual void UseSSBO(uint32_t set, uint32_t binding, const TRAP::Graphics::StorageBuffer* const storageBuffer,
							 uint64_t size = 0, const Window* const window = TRAP::Application::GetWindow()) const = 0;

		/// <summary>
		/// Retrieve the shaders thread count per work group.
		/// </summary>
		/// <returns>Shaders thread count per work group.</returns>
		[[nodiscard]] virtual const std::array<uint32_t, 3>& GetNumThreadsPerGroup() const noexcept = 0;

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

		static std::array<std::string, 2> SupportedShaderFormatSuffixes;

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
		bool m_valid;

	private:
		/// <summary>
		/// Check if given file contains SPIRV magic number.
		/// </summary>
		/// <param name="filePath">File path of the shader.</param>
		/// <returns>True if file has SPIRV magic number, false otherwise.</returns>
		[[nodiscard]] static bool CheckSPIRVMagicNumber(const std::filesystem::path& filePath);

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
		/// <param name="shaderStages">Output: Used shader stages.</param>
		/// <param name="userMacros">Optional: User defined macros.</param>
		/// <returns>True if pre processing was successful, false otherwise.</returns>
		[[nodiscard]] static bool PreProcessGLSL(const std::string& glslSource,
		                                         std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)>& shaders,
		                                         RendererAPI::ShaderStage& shaderStages,
								                 const std::vector<Macro>* userMacros);
		/// <summary>
		/// Pre process GLSL for SPIRV conversion with glslang.
		/// Creates glslang::TShader objects needed for SPIRV conversion.
		/// </summary>
		/// <param name="source">Final GLSL source code.</param>
		/// <param name="stage">Shader stages contained in the GLSL source code.</param>
		/// <param name="preProcessedSource">Output: Pre processed GLSL source code.</param>
		/// <returns>glslang::TShader object.</returns>
		[[nodiscard]] static TRAP::Scope<glslang::TShader> PreProcessGLSLForSPIRVConversion(const char* source,
		                                                                 	                RendererAPI::ShaderStage stage,
																		 	                std::string& preProcessedSource);
		/// <summary>
		/// Parse a glslang::TShader object.
		/// </summary>
		/// <param name="shader">glslang::TShader object.</param>
		/// <returns>True if parsing was successful, false otherwise.</returns>
		[[nodiscard]] static bool ParseGLSLang(glslang::TShader* shader);
		/// <summary>
		/// Link a glslang::TShader to a glslang::TProgram object.
		/// </summary>
		/// <param name="shader">glslang::TShader object.</param>
		/// <param name="program">glslang::TProgram object to link with.</param>
		/// <returns>True if linking was successful, false otherwise.</returns>
		[[nodiscard]] static bool LinkGLSLang(glslang::TShader* shader, glslang::TProgram& program);
		/// <summary>
		/// Validate that the given shader stages are a valid combination.
		///
		/// 1. Checks if Rasterizer shaders are combined with Compute shader.
		/// 2. Checks if Rasterizer shaders are combined with RayTracing shaders
		/// 3. Checks if Compute shader is combined with RayTracing shaders.
		/// 4. Checks if Vertex and Fragment shaders are combined.
		/// </summary>
		/// <param name="shaderStages">Shader stages to validate.</param>
		/// <returns>True if validation was successful, false otherwise.</returns>
		[[nodiscard]] static bool ValidateShaderStages(const RendererAPI::ShaderStage& shaderStages);
		/// <summary>
		/// Convert GLSL shaders to SPIRV.
		/// </summary>
		/// <param name="shaders">GLSL shader(s) to convert.</param>
		/// <param name="shaderStages">Shader stages contained in shaders parameter.</param>
		/// <returns>RendererAPI::BinaryShaderDesc containing SPIRV binary data.</returns>
		[[nodiscard]] static RendererAPI::BinaryShaderDesc ConvertGLSLToSPIRV(const std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)>& shaders,
									                                          const RendererAPI::ShaderStage& shaderStages);
		/// <summary>
		/// Convert a glslang::TProgram object to SPIRV binary data.
		/// </summary>
		/// <param name="stage">Shader stage to convert.</param>
		/// <param name="program">glslang::TProgram object to convert.</param>
		/// <returns>SPIRV binary data on success, empty vector otherwise.</returns>
		[[nodiscard]] static std::vector<uint32_t> ConvertToSPIRV(RendererAPI::ShaderStage stage,
		                                                          glslang::TProgram& program);
		/// <summary>
		/// Load SPIRV binary data into RendererAPI::BinaryShaderDesc.
		/// </summary>
		/// <param name="SPIRV">SPIRV binary data.</param>
		/// <returns>RendererAPI::BinaryShaderDesc containing loaded SPIRV binary data.</returns>
		[[nodiscard]] static RendererAPI::BinaryShaderDesc LoadSPIRV(std::vector<uint32_t>& SPIRV);
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

		static bool s_glslangInitialized;

		//Macros which are always provided by default.
		inline static std::array<Macro, 2> s_defaultShaderMacrosVulkan
		{
			{
				{"UpdateFreqStatic", "set = 0"},
				{"UpdateFreqDynamic", "set = 1"}
			}
		};

		[[nodiscard]] inline static constexpr TBuiltInResource GetDefaultTBuiltInResource() //TODO Set these values with VkPhysicalDevice limits or min spec limits?!
		{
			return TBuiltInResource
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
				/* .maxDualSourceDrawBuffersEXT = */ 1,

				{
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
				}
			};
		}
	};
}

#endif /*TRAP_SHADER_H*/