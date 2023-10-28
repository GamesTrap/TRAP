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

	for(u32 i = 0; i < out->StageReflections.size(); ++i)
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
			//Go through all already added shader resources to see if this shader
			//resource was already added from a different shader stage.
			//If we find a duplicate shader resource, we add the shader stage
			//to the shader stage mask of that resource instead.
			const auto it = std::ranges::find_if(uniqueResources, [&ShaderResource](const auto* const r){return *r == ShaderResource;});
			if(it != uniqueResources.end()) //Not unique
			{
				const auto sharedIndex = it - uniqueResources.begin();
				shaderUsage[NumericCast<usize>(sharedIndex)] |= ShaderResource.UsedStages;
			}
			else //Unique, Add it to the list of shader resources
			{
				shaderUsage.push_back(ShaderResource.UsedStages);
				uniqueResources.push_back(&ShaderResource);
			}
		}

		//Loop through all shader variables (constant/uniform buffer members)
		for(usize j = 0; j < srcRef.Variables.size(); ++j)
		{
			//Go through all already added shader variables to see if this shader
			//variable was already added from a different shader stage.
			//If we find a duplicate shader variable, we don't add it.

			const auto it = std::ranges::find_if(uniqueVariable, [&srcRef, j](const ShaderVariable* const v){return *v == srcRef.Variables[j];});
			if(it == uniqueVariable.end()) //Unique
			{
				//Add it to the list of shader variables
				uniqueVariableParent.push_back(&srcRef.ShaderResources[srcRef.Variables[j].ParentIndex]);
				uniqueVariable.push_back(&srcRef.Variables[j]);
			}
		}
	}

	//Copy over the shader resources in a dynamic array of the correct size
	out->ShaderResources.resize(uniqueResources.size());
	for(usize i = 0; i < uniqueResources.size(); ++i)
	{
		out->ShaderResources[i] = *uniqueResources[i];
		out->ShaderResources[i].UsedStages = shaderUsage[i];
	}

	//Copy over the shader variables in a dynamic array of the correct size
	out->Variables.resize(uniqueVariable.size());
	for(usize i = 0; i < out->Variables.size(); ++i)
	{
		out->Variables[i] = *uniqueVariable[i];
		const ShaderResource* const parentResource = uniqueVariableParent[i];
		//Look for parent
		const auto it = std::ranges::find_if(out->ShaderResources, [parentResource](const ShaderResource& r){return r == *parentResource; });
		if(it != out->ShaderResources.end())
		{
			out->Variables[i].ParentIndex = NumericCast<u64>(it - out->ShaderResources.begin());
		}
	}

	return out;
}