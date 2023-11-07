#ifndef TRAP_SHADERREFLECTION_H
#define TRAP_SHADERREFLECTION_H

#include "RendererAPI.h"

namespace TRAP::Graphics::API::ShaderReflection
{
	inline constexpr u32 MaxShaderStageCount = 5;

	enum class TextureDimension : u32
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
		u64 Size;

		//Resource name
		std::string Name;
	};

	struct ShaderResource
	{
		//Resource Type
		RendererAPI::DescriptorType Type;

		//The resource set for binding frequency
		u32 Set{};

		//The resource binding location
		u32 Reg{};

		//The size of the resource.
		//This will be the DescriptorInfo array size for textures
		u64 Size{};

		//What stages use this resource
		RendererAPI::ShaderStage UsedStages;

		//Resource name
		std::string Name;

		//1D / 2D / Array / MSAA / ...
		TextureDimension Dim;

		[[nodiscard]] constexpr bool operator==(const ShaderResource& other) const noexcept
		{
			return this->Type == other.Type && this->Set == other.Set && this->Reg == other.Reg;
		}
		[[nodiscard]] constexpr bool operator!=(const ShaderResource& other) const noexcept = default;
	};

	struct ShaderVariable
	{
		//Parents resource index
		u64 ParentIndex{};

		//The offset of the Variable.
		u32 Offset{};

		//The size of the Variable.
		u64 Size{};

		//Variable name
		std::string Name;

		[[nodiscard]] constexpr bool operator==(const ShaderVariable& other) const noexcept
		{
			return this->Offset == other.Offset && this->Size == other.Size &&
			       this->Name.size() == other.Name.size() && this->Name == other.Name;
		}
		[[nodiscard]] constexpr bool operator!=(const ShaderVariable& other) const noexcept = default;
	};


	struct ShaderReflection
	{
		RendererAPI::ShaderStage ShaderStage;

		std::vector<VertexInput> VertexInputs;

		std::vector<ShaderResource> ShaderResources;

		std::vector<ShaderVariable> Variables;

		//Thread group size for compute shader
		std::array<u32, 3> NumThreadsPerGroup{};

		//Number of tessellation control points
		u32 NumControlPoint{};

		std::string EntryPoint;
	};

	struct PipelineReflection
	{
		RendererAPI::ShaderStage ShaderStages;
		//The individual stages reflection data
		std::vector<ShaderReflection> StageReflections;

		std::optional<u32> VertexStageIndex = std::nullopt;
		std::optional<u32> TessellationControlStageIndex = std::nullopt;
		std::optional<u32> TessellationEvaluationStageIndex = std::nullopt;
		std::optional<u32> GeometryStageIndex = std::nullopt;
		std::optional<u32> FragmentStageIndex = std::nullopt;

		std::vector<ShaderResource> ShaderResources;

		std::vector<ShaderVariable> Variables;
	};

	/// @brief Create a pipeline reflection from shader reflection data.
	/// @param reflection Shader reflection data.
	/// @return Pipeline reflection.
	[[nodiscard]] TRAP::Ref<PipelineReflection> CreatePipelineReflection(const std::vector<ShaderReflection>& reflection);
}

#endif /*TRAP_SHADERREFLECTION_H*/