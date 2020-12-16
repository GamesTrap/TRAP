#include "TRAPPCH.h"
#include "ShaderReflection.h"

bool ShaderResourceCmp(TRAP::Graphics::API::ShaderReflection::ShaderResource& a, TRAP::Graphics::API::ShaderReflection::ShaderResource& b)
{
	bool isSame = true;

	isSame = isSame && (a.Type == b.Type);
	isSame = isSame && (a.Set == b.Set);
	isSame = isSame && (a.Reg == b.Reg);

	return isSame;
}

//-------------------------------------------------------------------------------------------------------------------//

bool ShaderVariableCmp(TRAP::Graphics::API::ShaderReflection::ShaderVariable& a, TRAP::Graphics::API::ShaderReflection::ShaderVariable& b)
{
	bool isSame = true;

	isSame = isSame && (a.Offset == b.Offset);
	isSame = isSame && (a.Size == b.Size);
	isSame = isSame && (a.NameSize == b.NameSize);

	//Early exit before string cmp
	if (isSame == false)
		return isSame;

	isSame = a.Name == b.Name;

	return isSame;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::API::ShaderReflection::PipelineReflection> TRAP::Graphics::API::ShaderReflection::CreatePipelineReflection(
	const std::array<ShaderReflection, static_cast<uint32_t>(RendererAPI::ShaderStage::SHADER_STAGE_COUNT)>& reflection,
	const uint32_t stageCount)
{
	if(stageCount == 0)
	{
		TP_ERROR(Log::RendererVulkanShaderPrefix, "Parameter 'stageCount' is 0");
		return nullptr;
	}

	//Sanity checks to make sure we don't have repeated stages.
	RendererAPI::ShaderStage combinedShaderStages = static_cast<RendererAPI::ShaderStage>(0);
	for(uint32_t i = 0; i < stageCount; ++i)
	{
		if(static_cast<uint32_t>(combinedShaderStages & reflection[i].ShaderStage) != 0)
		{
			TP_ERROR(Log::RendererVulkanShaderPrefix, "Duplicate shader stage was detected in shader reflection array.");
			return nullptr;
		}
		combinedShaderStages = combinedShaderStages | reflection[i].ShaderStage;
	}

	//Combine all shaders
	//This will have a large amount of looping
	//1. count number of resources
	uint32_t vertexStageIndex = ~0u;
	uint32_t tessellationControlStageIndex = ~0u;
	uint32_t tessellationEvaluationStageIndex = ~0u;
	uint32_t geometryStageIndex = ~0u;
	uint32_t fragmentStageIndex = ~0u;
	std::vector<ShaderResource> resources;
	uint32_t resourceCount = 0;
	std::vector<ShaderVariable> variables;
	uint32_t variableCount = 0;

	TRAP::Ref<PipelineReflection> out = TRAP::MakeRef<PipelineReflection>();
	
	std::array<ShaderResource*, 512> uniqueResources{};
	std::array<RendererAPI::ShaderStage, 512> shaderUsage{};
	std::array<ShaderVariable*, 512> uniqueVariable{};
	std::array<ShaderResource*, 512> uniqueVariableParent{};
	for(uint32_t i = 0; i < stageCount; ++i)
	{
		out->StageReflections[i] = reflection[i];
		ShaderReflection& srcRef = out->StageReflections[i];

		if (srcRef.ShaderStage == RendererAPI::ShaderStage::Vertex)
			vertexStageIndex = i;
		else if (srcRef.ShaderStage == RendererAPI::ShaderStage::TessellationControl)
			tessellationControlStageIndex = i;
		else if (srcRef.ShaderStage == RendererAPI::ShaderStage::TessellationEvaluation)
			tessellationEvaluationStageIndex = i;
		else if (srcRef.ShaderStage == RendererAPI::ShaderStage::Geometry)
			geometryStageIndex = i;
		else if (srcRef.ShaderStage == RendererAPI::ShaderStage::Fragment)
			fragmentStageIndex = i;

		//Loop through all shader resources
		for(uint32_t j = 0; j < srcRef.ShaderResources.size(); ++j)
		{
			bool unique = true;

			//Go through all already added shader resources to see if this shader
			//resource was already added from a different shader stage.
			//If we find a duplicate shader resource, we add the shader stage
			//to the shader stage mask of that resource instead.
			for(uint32_t k = 0; k < resourceCount; ++k)
			{
				unique = !ShaderResourceCmp(srcRef.ShaderResources[j], *uniqueResources[k]);
				if(unique == false)
				{
					shaderUsage[k] |= srcRef.ShaderResources[j].UsedStages;
					break;
				}
			}

			//If it's unique, we add it to the list of shader resources
			if(unique == true)
			{
				shaderUsage[resourceCount] = srcRef.ShaderResources[j].UsedStages;
				uniqueResources[resourceCount] = &srcRef.ShaderResources[j];
				resourceCount++;
			}
		}

		//Loop through all shader variables (constant/uniform buffer members)
		for(uint32_t j = 0; j < srcRef.Variables.size(); ++j)
		{
			bool unique = true;

			//Go through all already added shader variables to see if this shader
			//variable was already added from a different shader stage.
			//If we find a duplicate shader variable, we don't add it.
			for(uint32_t k = 0; k < variableCount; ++k)
			{
				unique = !ShaderVariableCmp(srcRef.Variables[j], *uniqueVariable[k]);
				if (unique == false)
					break;
			}

			//If it's unique we add it to the list of shader variables
			if(unique)
			{
				uniqueVariableParent[variableCount] = &srcRef.ShaderResources[srcRef.Variables[j].ParentIndex];
				uniqueVariable[variableCount] = &srcRef.Variables[j];
				variableCount++;
			}
		}
	}

	//Copy over the shader resources in a dynamic array of the correct size
	if(resourceCount)
	{
		resources.resize(resourceCount);

		for(uint32_t i = 0; i < resourceCount; ++i)
		{
			resources[i] = *uniqueResources[i];
			resources[i].UsedStages = shaderUsage[i];
		}
	}

	//Copy over the shader variables in a dynamic array of the correct size
	if(variableCount)
	{
		variables.resize(variableCount);

		for(uint32_t i = 0; i < variableCount; ++i)
		{
			variables[i] = *uniqueVariable[i];
			ShaderResource* parentResource = uniqueVariableParent[i];
			//Look for parent
			for(uint32_t j = 0; j < resourceCount; ++j)
			{
				if(ShaderResourceCmp(resources[j], *parentResource))
				{
					variables[i].ParentIndex = j;
					break;
				}
			}
		}
	}

	//All reflection struct should be built now
	out->ShaderStages = combinedShaderStages;

	out->StageReflectionCount = stageCount;

	out->VertexStageIndex = vertexStageIndex;
	out->TessellationControlStageIndex= tessellationControlStageIndex;
	out->TessellationEvaluationStageIndex = tessellationEvaluationStageIndex;
	out->GeometryStageIndex = geometryStageIndex;
	out->FragmentStageIndex = fragmentStageIndex;

	out->ShaderResources = resources;

	out->Variables = variables;

	return out;
}