#include "TRAPPCH.h"
#include "ShaderReflection.h"

namespace
{
	[[nodiscard]] constexpr TRAP::Optional<TRAP::Graphics::RendererAPI::ShaderStage> CollectShaderStages(const std::span<const TRAP::Graphics::API::ShaderReflection::ShaderReflection> reflection)
	{
		//Sanity checks to make sure we don't have repeated stages.
		TRAP::Graphics::RendererAPI::ShaderStage combinedShaderStages = TRAP::Graphics::RendererAPI::ShaderStage::None;

		for(const TRAP::Graphics::API::ShaderReflection::ShaderReflection& i : reflection)
		{
			if((combinedShaderStages & i.ShaderStage) != TRAP::Graphics::RendererAPI::ShaderStage::None)
			{
				TP_ERROR(TRAP::Log::ShaderPrefix, "Duplicate shader stage was detected in shader reflection array.");
				return TRAP::NullOpt;
			}

			combinedShaderStages |= i.ShaderStage;
		}

		return combinedShaderStages;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	constexpr void UpdateShaderStageIndices(TRAP::Graphics::API::ShaderReflection::PipelineReflection& pipelineReflection)
	{
		for(u32 i = 0; i < pipelineReflection.StageReflections.size(); ++i)
		{
			const TRAP::Graphics::API::ShaderReflection::ShaderReflection& srcRef = pipelineReflection.StageReflections[i];

			switch(srcRef.ShaderStage)
			{
			case TRAP::Graphics::RendererAPI::RendererAPI::ShaderStage::Vertex:
				pipelineReflection.VertexStageIndex = i;
				break;
			case TRAP::Graphics::RendererAPI::RendererAPI::ShaderStage::TessellationControl:
				pipelineReflection.TessellationControlStageIndex = i;
				break;
			case TRAP::Graphics::RendererAPI::RendererAPI::ShaderStage::TessellationEvaluation:
				pipelineReflection.TessellationControlStageIndex = i;
				break;
			case TRAP::Graphics::RendererAPI::RendererAPI::ShaderStage::Geometry:
				pipelineReflection.GeometryStageIndex = i;
				break;
			case TRAP::Graphics::RendererAPI::RendererAPI::ShaderStage::Fragment:
				pipelineReflection.FragmentStageIndex = i;
				break;

			case TRAP::Graphics::RendererAPI::RendererAPI::ShaderStage::Compute: //Compute always has index 0
				break;
			case TRAP::Graphics::RendererAPI::RendererAPI::ShaderStage::RayTracing:
				//TODO
				break;

			case TRAP::Graphics::RendererAPI::RendererAPI::ShaderStage::None:
				[[fallthrough]];
			case TRAP::Graphics::RendererAPI::RendererAPI::ShaderStage::AllGraphics:
				break;
			}
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr std::vector<TRAP::Graphics::API::ShaderReflection::ShaderResource> CollectShaderResources(const std::span<const TRAP::Graphics::API::ShaderReflection::ShaderReflection> reflection)
	{
		std::vector<TRAP::Graphics::API::ShaderReflection::ShaderResource> shaderResources{};
		shaderResources.reserve(512);

		for(const TRAP::Graphics::API::ShaderReflection::ShaderReflection& srcRef : reflection)
		{
			//Loop through all shader resources
			for (const TRAP::Graphics::API::ShaderReflection::ShaderResource& shaderRes : srcRef.ShaderResources)
			{
				//Go through all already added shader resources to see if this shader
				//resource was already added from a different shader stage.
				//If we find a duplicate shader resource, we add the shader stage
				//to the shader stage mask of that resource instead.
				if(const auto it = std::ranges::find(shaderResources, shaderRes); it != shaderResources.end()) //Not unique, Add new shader stage
					it->UsedStages |= shaderRes.UsedStages;
				else //Unique, Add it to the list of shader resources
					shaderResources.emplace_back(shaderRes);
			}
		}

		shaderResources.shrink_to_fit();

		return shaderResources;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr std::vector<TRAP::Graphics::API::ShaderReflection::ShaderVariable> CollectShaderVariables(const std::span<const TRAP::Graphics::API::ShaderReflection::ShaderReflection> reflection,
	                                                                                                                  const std::span<const TRAP::Graphics::API::ShaderReflection::ShaderResource> shaderResources)
	{
		std::vector<TRAP::Graphics::API::ShaderReflection::ShaderVariable> uniqueVariable{};
		uniqueVariable.reserve(512);

		for(const TRAP::Graphics::API::ShaderReflection::ShaderReflection& srcRef : reflection)
		{
			//Loop through all shader variables (constant/uniform buffer members)
			for(const TRAP::Graphics::API::ShaderReflection::ShaderVariable& var : srcRef.Variables)
			{
				//Go through all already added shader variables to see if this shader
				//variable was already added from a different shader stage.
				//If we find a duplicate shader variable, we don't add it.

				if(std::ranges::find(uniqueVariable, var) != uniqueVariable.end()) //Not unique
					continue;

				//Add it to the list of shader variables

				const std::ptrdiff_t newParentIndex = std::distance(shaderResources.begin(), std::ranges::find(shaderResources, srcRef.ShaderResources[var.ParentIndex]));
				uniqueVariable.emplace_back(var).ParentIndex = NumericCast<u64>(newParentIndex);
			}
		}

		uniqueVariable.shrink_to_fit();

		return uniqueVariable;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::API::ShaderReflection::PipelineReflection> TRAP::Graphics::API::ShaderReflection::CreatePipelineReflection(
	const std::vector<ShaderReflection>& reflection)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(reflection.empty())
	{
		TP_ERROR(Log::ShaderPrefix, "Parameter 'reflection' is empty");
		return nullptr;
	}

	const auto combinedShaderStages = CollectShaderStages(reflection);
	if(!combinedShaderStages)
		return nullptr;

	TRAP::Ref<PipelineReflection> out = TRAP::MakeRef<PipelineReflection>();

	out->StageReflections = reflection;
	out->ShaderStages = *combinedShaderStages;
	out->ShaderResources = CollectShaderResources(reflection);
	out->Variables = CollectShaderVariables(reflection, out->ShaderResources);
	UpdateShaderStageIndices(*out);

	return out;
}
