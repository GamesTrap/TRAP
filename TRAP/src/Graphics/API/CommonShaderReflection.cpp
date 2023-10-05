#include "TRAPPCH.h"
#include "ShaderReflection.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::API::ShaderReflection::PipelineReflection> TRAP::Graphics::API::ShaderReflection::CreatePipelineReflection(
	const std::vector<ShaderReflection>& reflection)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(reflection.empty())
	{
		TP_ERROR(Log::ShaderPrefix, "Parameter 'reflection' is empty");
		return nullptr;
	}

	//Sanity checks to make sure we don't have repeated stages.
	RendererAPI::ShaderStage combinedShaderStages = RendererAPI::ShaderStage::None;
	for(const ShaderReflection& i : reflection)
	{
		if((combinedShaderStages & i.ShaderStage) != RendererAPI::ShaderStage::None)
		{
			TP_ERROR(Log::ShaderPrefix, "Duplicate shader stage was detected in shader reflection array.");
			return nullptr;
		}
		combinedShaderStages |= i.ShaderStage;
	}

	TRAP::Ref<PipelineReflection> out = TRAP::MakeRef<PipelineReflection>();

	out->StageReflections = reflection;
	out->ShaderStages = combinedShaderStages;

	//Combine all shaders
	//This will have a large amount of looping
	//1. count number of resources
	std::vector<ShaderResource*> uniqueResources{};
	std::vector<RendererAPI::ShaderStage> shaderUsage{};
	std::vector<ShaderVariable*> uniqueVariable{};
	std::vector<ShaderResource*> uniqueVariableParent{};
	uniqueResources.reserve(512);
	shaderUsage.reserve(512);
	uniqueVariable.reserve(512);
	uniqueVariableParent.reserve(512);

	for(uint32_t i = 0; i < out->StageReflections.size(); ++i)
	{
		ShaderReflection& srcRef = out->StageReflections[i];

		if (srcRef.ShaderStage == RendererAPI::ShaderStage::Vertex)
			out->VertexStageIndex = i;
		else if (srcRef.ShaderStage == RendererAPI::ShaderStage::TessellationControl)
			out->TessellationControlStageIndex = i;
		else if (srcRef.ShaderStage == RendererAPI::ShaderStage::TessellationEvaluation)
			out->TessellationEvaluationStageIndex = i;
		else if (srcRef.ShaderStage == RendererAPI::ShaderStage::Geometry)
			out->GeometryStageIndex = i;
		else if (srcRef.ShaderStage == RendererAPI::ShaderStage::Fragment)
			out->FragmentStageIndex = i;

		//Loop through all shader resources
		for (auto& ShaderResource : srcRef.ShaderResources)
		{
			bool unique = true;

			//Go through all already added shader resources to see if this shader
			//resource was already added from a different shader stage.
			//If we find a duplicate shader resource, we add the shader stage
			//to the shader stage mask of that resource instead.
			//TODO Replace with std::find_if ?
			for(std::size_t k = 0; k < uniqueResources.size(); ++k)
			{
				unique = ShaderResource != *uniqueResources[k];
				if(!unique)
				{
					shaderUsage[k] |= ShaderResource.UsedStages;
					break;
				}
			}

			//If it's unique, we add it to the list of shader resources
			if(unique)
			{
				shaderUsage.push_back(ShaderResource.UsedStages);
				uniqueResources.push_back(&ShaderResource);
			}
		}

		//Loop through all shader variables (constant/uniform buffer members)
		for(std::size_t j = 0; j < srcRef.Variables.size(); ++j)
		{
			bool unique = true;

			//Go through all already added shader variables to see if this shader
			//variable was already added from a different shader stage.
			//If we find a duplicate shader variable, we don't add it.
			//TODO Replace with std::find_if ?
			for(const ShaderVariable* const k : uniqueVariable)
			{
				unique = srcRef.Variables[j] != *k;
				if (!unique)
					break;
			}

			//If it's unique we add it to the list of shader variables
			if(unique)
			{
				uniqueVariableParent.push_back(&srcRef.ShaderResources[srcRef.Variables[j].ParentIndex]);
				uniqueVariable.push_back(&srcRef.Variables[j]);
			}
		}
	}

	//Copy over the shader resources in a dynamic array of the correct size
	out->ShaderResources.resize(uniqueResources.size());
	for(std::size_t i = 0; i < uniqueResources.size(); ++i)
	{
		out->ShaderResources[i] = *uniqueResources[i];
		out->ShaderResources[i].UsedStages = shaderUsage[i];
	}

	//Copy over the shader variables in a dynamic array of the correct size
	out->Variables.resize(uniqueVariable.size());
	for(std::size_t i = 0; i < out->Variables.size(); ++i)
	{
		out->Variables[i] = *uniqueVariable[i];
		const ShaderResource* const parentResource = uniqueVariableParent[i];
		//Look for parent
		//TODO std::find_if?
		for(std::size_t j = 0; j < out->ShaderResources.size(); ++j)
		{
			if(out->ShaderResources[j] == *parentResource)
			{
				out->Variables[i].ParentIndex = j;
				break;
			}
		}
	}

	return out;
}