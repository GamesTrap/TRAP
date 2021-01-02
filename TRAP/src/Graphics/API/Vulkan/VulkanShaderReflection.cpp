#include "TRAPPCH.h"
#include "VulkanShaderReflection.h"

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/ShaderReflection.h"
#include "Graphics/API/SPIRVTools.h"

std::array<TRAP::Graphics::RendererAPI::DescriptorType, static_cast<uint32_t>(TRAP::Graphics::API::SPIRVTools::ResourceType::RESOURCE_TYPE_COUNT)> SPIRVToDescriptorType =
{
	TRAP::Graphics::RendererAPI::DescriptorType::Undefined, TRAP::Graphics::RendererAPI::DescriptorType::Undefined,
	TRAP::Graphics::RendererAPI::DescriptorType::UniformBuffer, TRAP::Graphics::RendererAPI::DescriptorType::RWBuffer,
	TRAP::Graphics::RendererAPI::DescriptorType::Texture, TRAP::Graphics::RendererAPI::DescriptorType::RWTexture,
	TRAP::Graphics::RendererAPI::DescriptorType::Sampler, TRAP::Graphics::RendererAPI::DescriptorType::RootConstant,
	TRAP::Graphics::RendererAPI::DescriptorType::InputAttachment, TRAP::Graphics::RendererAPI::DescriptorType::TexelBuffer,
	TRAP::Graphics::RendererAPI::DescriptorType::RWTexelBuffer, TRAP::Graphics::RendererAPI::DescriptorType::RayTracing,
};

//-------------------------------------------------------------------------------------------------------------------//

std::array<TRAP::Graphics::API::ShaderReflection::TextureDimension, static_cast<uint32_t>(TRAP::Graphics::API::SPIRVTools::ResourceTextureDimension::RESOURCE_TEXTURE_DIMENSION_COUNT)> SPIRVToTextureDimension =
{
	TRAP::Graphics::API::ShaderReflection::TextureDimension::TextureDimUndefined,
	TRAP::Graphics::API::ShaderReflection::TextureDimension::TextureDimUndefined,
	TRAP::Graphics::API::ShaderReflection::TextureDimension::TextureDim1D,
	TRAP::Graphics::API::ShaderReflection::TextureDimension::TextureDim1DArray,
	TRAP::Graphics::API::ShaderReflection::TextureDimension::TextureDim2D,
	TRAP::Graphics::API::ShaderReflection::TextureDimension::TextureDim2DArray,
	TRAP::Graphics::API::ShaderReflection::TextureDimension::TextureDim2DMS,
	TRAP::Graphics::API::ShaderReflection::TextureDimension::TextureDim2DMSArray,
	TRAP::Graphics::API::ShaderReflection::TextureDimension::TextureDim3D,
	TRAP::Graphics::API::ShaderReflection::TextureDimension::TextureDimCube,
	TRAP::Graphics::API::ShaderReflection::TextureDimension::TextureDimCubeArray,
};

//-------------------------------------------------------------------------------------------------------------------//

bool FilterResource(const TRAP::Graphics::API::SPIRVTools::Resource& resource, const TRAP::Graphics::RendererAPI::ShaderStage currentStage)
{
	bool filter = false;

	//Remove used resources
	filter = filter || (resource.IsUsed == false);

	//Remove stage outputs
	filter = filter || (resource.Type == TRAP::Graphics::API::SPIRVTools::ResourceType::Outputs);

	//Remove stage inputs that are not on the vertex shader
	filter = filter || (resource.Type == TRAP::Graphics::API::SPIRVTools::ResourceType::Inputs && currentStage != TRAP::Graphics::RendererAPI::ShaderStage::Vertex);

	return filter;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ShaderReflection::ShaderReflection TRAP::Graphics::API::VkCreateShaderReflection(const std::vector<uint8_t>& shaderCode, RendererAPI::ShaderStage shaderStage)
{
	SPIRVTools::CrossCompiler cc = SPIRVTools::CreateCrossCompiler(reinterpret_cast<const uint32_t*>(shaderCode.data()), static_cast<uint32_t>(shaderCode.size() / sizeof(uint32_t)));

	ReflectEntryPoint(cc);
	ReflectShaderResources(cc);
	ReflectShaderVariables(cc);

	ShaderReflection::ShaderReflection out{};
	
	if (shaderStage == RendererAPI::ShaderStage::Compute)
		out.NumThreadsPerGroup = ReflectComputeShaderWorkGroupSize(cc);
	else if (shaderStage == RendererAPI::ShaderStage::TessellationControl)
		out.NumControlPoint = ReflectTessellationControlShaderControlPoint(cc);

	uint32_t vertexInputCount = 0;
	uint32_t resourceCount = 0;
	uint32_t variablesCount = 0;

	for(uint32_t i = 0; i < cc.ShaderResources.size(); ++i)
	{
		SPIRVTools::Resource& resource = cc.ShaderResources[i];

		//Filter out what we don't use
		if(!FilterResource(resource, shaderStage))
		{
			if (resource.Type == SPIRVTools::ResourceType::Inputs && shaderStage == TRAP::Graphics::RendererAPI::ShaderStage::Vertex)
				++vertexInputCount;
			else
				++resourceCount;
		}
	}

	for(uint32_t i = 0; i < cc.UniformVariables.size(); ++i)
	{
		SPIRVTools::Variable& variable = cc.UniformVariables[i];

		//Check if parent buffer was filtered out
		const bool parentFiltered = FilterResource(cc.ShaderResources[variable.ParentIndex], shaderStage);

		//Filter out what we don't use
		if(variable.IsUsed && !parentFiltered)
			++variablesCount;
	}

	out.EntryPoint = cc.EntryPoint;

	std::vector<ShaderReflection::VertexInput> vertexInputs{};
	//Start with the vertex input
	if(shaderStage == RendererAPI::ShaderStage::Vertex && vertexInputCount > 0)
	{
		vertexInputs.resize(vertexInputCount);

		uint32_t j = 0;
		for(uint32_t i = 0; i < cc.ShaderResources.size(); ++i)
		{
			SPIRVTools::Resource& resource = cc.ShaderResources[i];

			//Filter out what we don't use
			if(!FilterResource(resource, shaderStage) && resource.Type == TRAP::Graphics::API::SPIRVTools::ResourceType::Inputs)
			{
				vertexInputs[j].Size = resource.Size;
				vertexInputs[j].Name = resource.Name;
				vertexInputs[j].NameSize = static_cast<uint32_t>(resource.Name.size());

				++j;
			}
		}
	}

	std::vector<uint32_t> indexRemap{};
	std::vector<ShaderReflection::ShaderResource> resources{};
	//Continue with resources
	if(resourceCount)
	{
		indexRemap.resize(cc.ShaderResources.size());
		resources.resize(resourceCount);

		uint32_t j = 0;
		for(uint32_t i = 0; i < cc.ShaderResources.size(); ++i)
		{
			//Set index remap
			indexRemap[i] = static_cast<uint32_t>(-1);

			SPIRVTools::Resource& resource = cc.ShaderResources[i];

			//Filter out what we don't use
			if(!FilterResource(resource, shaderStage) && resource.Type != SPIRVTools::ResourceType::Inputs)
			{
				//Set new index
				indexRemap[i] = j;

				resources[j].Type = SPIRVToDescriptorType[static_cast<uint32_t>(resource.Type)];
				resources[j].Set = resource.Set;
				resources[j].Reg = resource.Binding;
				resources[j].Size = resource.Size;
				resources[j].UsedStages = shaderStage;

				resources[j].Name = resource.Name;
				resources[j].Dim = SPIRVToTextureDimension[static_cast<uint32_t>(resource.Dimension)];

				++j;
			}
		}
	}

	std::vector<ShaderReflection::ShaderVariable> variables{};
	//Now do variables
	if(variablesCount)
	{
		variables.resize(variablesCount);

		uint32_t j = 0;
		for(uint32_t i = 0; i < cc.UniformVariables.size(); ++i)
		{
			SPIRVTools::Variable& variable = cc.UniformVariables[i];

			//Check if parent buffer was filtered out
			bool parentFiltered = FilterResource(cc.ShaderResources[variable.ParentIndex], shaderStage);

			//Filter out what we don't use
			if(variable.IsUsed && !parentFiltered)
			{
				variables[j].Offset = variable.Offset;
				variables[j].Size = variable.Size;
				variables[j].ParentIndex = indexRemap[variable.ParentIndex];

				variables[j].Name = variable.Name;

				++j;
			}
		}
	}

	indexRemap.clear();
	DestroyCrossCompiler(cc);

	//All reflection struct should be built now
	out.ShaderStage = shaderStage;
	
	out.VertexInputs = vertexInputs;
	out.ShaderResources = resources;
	out.Variables = variables;

	return out;
}