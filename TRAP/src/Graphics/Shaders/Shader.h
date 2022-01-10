#ifndef TRAP_SHADER_H
#define TRAP_SHADER_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/Buffers/UniformBuffer.h"
#include "Graphics/Buffers/StorageBuffer.h"
#include "Graphics/Textures/Texture.h"

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

		struct DescriptorSets
		{
			DescriptorSet* StaticDescriptors;
			DescriptorSet* PerFrameDescriptors;
			DescriptorSet* PerBatchDescriptors;
			DescriptorSet* PerDrawDescriptors;
		};

	protected:
		Shader() = default;
	public:
		Shader(const Shader&) = default;
		Shader& operator=(const Shader&) = default;
		Shader(Shader&&) = default;
		Shader& operator=(Shader&&) = default;
		virtual ~Shader() = default;

		const std::string& GetName() const;

		const std::filesystem::path& GetFilePath() const;

		RendererAPI::ShaderStage GetShaderStages() const;

		TRAP::Ref<RootSignature> GetRootSignature() const;
		const std::array<DescriptorSet*, RendererAPI::MaxDescriptorSets>& GetDescriptorSets() const;

		virtual void Use(Window* window = nullptr) = 0;
		virtual void UseTexture(uint32_t set, uint32_t binding, TRAP::Graphics::Texture* const texture,
		                        Window* window = nullptr) = 0;
		virtual void UseTextures(uint32_t set, uint32_t binding,
		                         const std::vector<TRAP::Graphics::Texture*>& textures,
								 Window* window = nullptr) = 0;
		virtual void UseSampler(uint32_t set, uint32_t binding, TRAP::Graphics::Sampler* const sampler,
		                        Window* window = nullptr) = 0;
		virtual void UseSamplers(uint32_t set, uint32_t binding,
		                         const std::vector<TRAP::Graphics::Sampler*>& samplers,
								 Window* window = nullptr) = 0;
		virtual void UseUBO(uint32_t set, uint32_t binding, TRAP::Graphics::UniformBuffer* const uniformBuffer,
							uint64_t size = 0, uint64_t offset = 0, Window* window = nullptr) = 0;
		virtual void UseSSBO(uint32_t set, uint32_t binding, TRAP::Graphics::StorageBuffer* const storageBuffer,
							 uint64_t size = 0, Window* window = nullptr) = 0;

		static Scope<Shader> CreateFromFile(const std::string& name, const std::filesystem::path& filePath,
		                                    const std::vector<Macro>* userMacros = nullptr);
		static Scope<Shader> CreateFromFile(const std::filesystem::path& filepath,
		                                    const std::vector<Macro>* userMacros = nullptr);
		static Scope<Shader> CreateFromSource(const std::string& name, const std::string& glslSource,
		                                      const std::vector<Macro>* userMacros = nullptr);

		static std::array<std::string, 2> SupportedShaderFormatSuffixes;

	protected:
		std::string m_name;
		std::filesystem::path m_filepath;
		RendererAPI::ShaderStage m_shaderStages{};
		TRAP::Ref<RootSignature> m_rootSignature;
		std::array<DescriptorSet*, RendererAPI::MaxDescriptorSets> m_descriptorSets{};

	private:
		static bool CheckSPIRVMagicNumber(const std::filesystem::path& filePath);
		static std::vector<uint32_t> Convert8To32(const std::vector<uint8_t>& source);

		static bool PreProcessGLSL(const std::string& glslSource,
		                           std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)>& shaders,
		                           RendererAPI::ShaderStage& shaderStages,
								   const std::vector<Macro>* userMacros);
		static TRAP::Scope<glslang::TShader> PreProcessGLSLForConversion(const char* source,
		                                                                 RendererAPI::ShaderStage stage,
																		 std::string& preProcessedSource);
		static bool ParseGLSLang(glslang::TShader* shader);
		static bool LinkGLSLang(glslang::TShader* shader, glslang::TProgram& program);
		static bool ValidateShaderStages(const RendererAPI::ShaderStage& shaderStages);
		static RendererAPI::BinaryShaderDesc ConvertGLSLToSPIRV(const std::array<std::string, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)>& shaders,
									                            const RendererAPI::ShaderStage& shaderStages);
		static std::vector<uint32_t> ConvertToSPIRV(glslang::TShader* shader, RendererAPI::ShaderStage stage,
		                                            glslang::TProgram& program);
		static RendererAPI::BinaryShaderDesc LoadSPIRV(const std::vector<uint32_t>& SPIRV);

		static bool s_glslangInitialized;

		inline static std::array<Macro, 2> s_defaultShaderMacrosVulkan
		{
			{
				{"UpdateFreqStatic", "set = 0"},
				{"UpdateFreqDynamic", "set = 1"}
			}
		};

		inline static constexpr TBuiltInResource GetDefaultTBuiltInResource() //TODO Set these values with VkPhysicalDevice limits or min spec limits?!
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
			};
		}
	};
}

#endif /*TRAP_SHADER_H*/