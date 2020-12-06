#ifndef _TRAP_SHADERREFLECTION_H_
#define _TRAP_SHADERREFLECTION_H_
#include "RendererAPI.h"

namespace TRAP::Graphics::API::ShaderReflection
{
	static const uint32_t MaxShaderStageCount = 5;
	
	enum class TextureDimension
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
		TextureCimCount,

		TextureDimUndefined
	};
	
	struct VertexInput
	{
		//The size of the attribute
		uint32_t Size;

		//Resource name
		const char* Name;
		uint32_t NameSize;
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
		uint32_t Size{};

		//What stages use this resource
		RendererAPI::ShaderStage UsedStages;

		//Resource name
		const char* Name;
		uint32_t NameSize;

		//1D / 2D / Array / MSAA / ...
		TextureDimension Dim;
	};
	
	struct ShaderVariable
	{
		//Parents resource index
		uint32_t ParentIndex{};

		//The offset of the Variable.
		uint32_t Offset{};

		//The size of the Variable.
		uint32_t Size{};

		//Variable name
		const char* Name;
		uint32_t NameSize;
	};
	
	struct ShaderReflection
	{
		RendererAPI::ShaderStage ShaderStage;

		//Single large allocation for names to reduce number of allocations
		std::vector<char> NamePool{};

		std::vector<VertexInput> VertexInputs;

		std::vector<ShaderResource> ShaderResources;

		std::vector<ShaderVariable> Variables;

		//Thread group size for compute shader
		std::array<uint32_t, 3> NumThreadsPerGroup{};

		//Number of tessellation control points
		uint32_t NumControlPoint{};

		char* EntryPoint;
	};
	
	struct PipelineReflection
	{
		RendererAPI::ShaderStage ShaderStages;
		//The individual stages reflection data
		std::array<ShaderReflection, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)> StageReflections;
		uint32_t StageReflectionCount{};

		uint32_t VertexStageIndex{};
		uint32_t TessellationControlStageIndex{};
		uint32_t TessellationEvaluationStageIndex{};
		uint32_t GeometryStageIndex{};
		uint32_t FragmentStageIndex{};

		std::vector<ShaderResource> ShaderResources;
		
		std::vector<ShaderVariable> Variables;
	};

	TRAP::Scope<PipelineReflection> CreatePipelineReflection(const std::array<ShaderReflection, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)>& reflection, uint32_t stageCount);
}

#endif /*_TRAP_SHADERREFLECTION_H_*/