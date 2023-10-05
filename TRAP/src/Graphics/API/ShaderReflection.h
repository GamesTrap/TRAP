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

	[[nodiscard]] constexpr bool operator==(const ShaderResource& lhs, const ShaderResource& rhs)
	{
		return lhs.Type == rhs.Type && lhs.Set == rhs.Set && lhs.Reg == rhs.Reg;
	}
	[[nodiscard]] constexpr bool operator!=(const ShaderResource& lhs, const ShaderResource& rhs)
	{
		return !(lhs == rhs);
	}

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

	[[nodiscard]] constexpr bool operator==(const ShaderVariable& lhs, const ShaderVariable& rhs)
	{
		return lhs.Offset == rhs.Offset && lhs.Size == rhs.Size && lhs.Name.size() == rhs.Name.size() && lhs.Name == rhs.Name;
	}
	[[nodiscard]] constexpr bool operator!=(const ShaderVariable& lhs, const ShaderVariable& rhs)
	{
		return !(lhs == rhs);
	}

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
		std::vector<ShaderReflection> StageReflections;

		std::optional<uint32_t> VertexStageIndex = std::nullopt;
		std::optional<uint32_t> TessellationControlStageIndex = std::nullopt;
		std::optional<uint32_t> TessellationEvaluationStageIndex = std::nullopt;
		std::optional<uint32_t> GeometryStageIndex = std::nullopt;
		std::optional<uint32_t> FragmentStageIndex = std::nullopt;

		std::vector<ShaderResource> ShaderResources;

		std::vector<ShaderVariable> Variables;
	};

	/// <summary>
	/// Create a pipeline reflection from shader reflection data.
	/// </summary>
	/// <param name="reflection">Shader reflection data.</param>
	/// <returns>Pipeline reflection.</returns>
	[[nodiscard]] TRAP::Ref<PipelineReflection> CreatePipelineReflection(const std::vector<ShaderReflection>& reflection);
}

#endif /*TRAP_SHADERREFLECTION_H*/