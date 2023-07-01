#ifndef TRAP_SHADERREFLECTION_H
#define TRAP_SHADERREFLECTION_H

#include "RendererAPI.h"

namespace TRAP::Graphics::API::ShaderReflection
{
	inline constexpr uint32_t MaxShaderStageCount = 5;

	enum class TextureDimension : uint32_t
	{
		TextureDim1D,
		TextureDim2D,
		TextureDim2DMS,
		TextureDim3D,
		TextureDimCube,
		TextureDim1DArray,
		TextureDim2DArray,
		TextureDim2DMSArray,
		TextureDimCubeArray,
		TextureDimCount,

		TextureDimUndefined
	};

	struct VertexInput
	{
		//The size of the attribute
		uint64_t Size;

		//Resource name
		std::string Name;
	};

	struct ShaderResource
	{
		//Resource Type
		RendererAPI::DescriptorType Type;

		//The resource set for binding frequency
		uint32_t Set{};

		//The resource binding location
		uint32_t Reg{};

		//The size of the resource.
		//This will be the DescriptorInfo array size for textures
		uint64_t Size{};

		//What stages use this resource
		RendererAPI::ShaderStage UsedStages;

		//Resource name
		std::string Name;

		//1D / 2D / Array / MSAA / ...
		TextureDimension Dim;
	};

	struct ShaderVariable
	{
		//Parents resource index
		uint64_t ParentIndex{};

		//The offset of the Variable.
		uint32_t Offset{};

		//The size of the Variable.
		uint64_t Size{};

		//Variable name
		std::string Name;
	};

	struct ShaderReflection
	{
		RendererAPI::ShaderStage ShaderStage;

		std::vector<VertexInput> VertexInputs;

		std::vector<ShaderResource> ShaderResources;

		std::vector<ShaderVariable> Variables;

		//Thread group size for compute shader
		std::array<uint32_t, 3> NumThreadsPerGroup{};

		//Number of tessellation control points
		uint32_t NumControlPoint{};

		std::string EntryPoint;
	};

	struct PipelineReflection
	{
		RendererAPI::ShaderStage ShaderStages;
		//The individual stages reflection data
		std::array<ShaderReflection,
		           std::to_underlying(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> StageReflections;
		uint32_t StageReflectionCount{};

		uint32_t VertexStageIndex{};
		uint32_t TessellationControlStageIndex{};
		uint32_t TessellationEvaluationStageIndex{};
		uint32_t GeometryStageIndex{};
		uint32_t FragmentStageIndex{};

		std::vector<ShaderResource> ShaderResources;

		std::vector<ShaderVariable> Variables;
	};

	/// <summary>
	/// Create a pipeline reflection from shader reflection data.
	/// </summary>
	/// <param name="reflection">Shader reflection data.</param>
	/// <param name="stageCount">Number of used shader stages.</param>
	/// <returns>Pipeline reflection.</returns>
	[[nodiscard]] TRAP::Ref<PipelineReflection> CreatePipelineReflection(const std::array<ShaderReflection,
	                                                                     std::to_underlying(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)>& reflection,
														                 uint32_t stageCount);
}

#endif /*TRAP_SHADERREFLECTION_H*/