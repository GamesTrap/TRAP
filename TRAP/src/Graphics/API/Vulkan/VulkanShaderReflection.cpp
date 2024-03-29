#include "TRAPPCH.h"
#include "VulkanShaderReflection.h"

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/ShaderReflection.h"
#include "Graphics/API/SPIRVTools.h"

constexpr std::array<TRAP::Graphics::RendererAPI::DescriptorType,
                     std::to_underlying(TRAP::Graphics::API::SPIRVTools::ResourceType::RESOURCE_TYPE_COUNT)> SPIRVToDescriptorType =
{
	TRAP::Graphics::RendererAPI::DescriptorType::Undefined, TRAP::Graphics::RendererAPI::DescriptorType::Undefined,
	TRAP::Graphics::RendererAPI::DescriptorType::UniformBuffer,
	TRAP::Graphics::RendererAPI::DescriptorType::RWBuffer, TRAP::Graphics::RendererAPI::DescriptorType::Texture,
	TRAP::Graphics::RendererAPI::DescriptorType::RWTexture, TRAP::Graphics::RendererAPI::DescriptorType::Sampler,
	TRAP::Graphics::RendererAPI::DescriptorType::RootConstant,
	TRAP::Graphics::RendererAPI::DescriptorType::InputAttachment,
	TRAP::Graphics::RendererAPI::DescriptorType::TexelBuffer,
	TRAP::Graphics::RendererAPI::DescriptorType::RWTexelBuffer,
	TRAP::Graphics::RendererAPI::DescriptorType::RayTracing,
	TRAP::Graphics::RendererAPI::DescriptorType::CombinedImageSampler
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr std::array<TRAP::Graphics::API::ShaderReflection::TextureDimension,
                     std::to_underlying(TRAP::Graphics::API::SPIRVTools::ResourceTextureDimension::RESOURCE_TEXTURE_DIMENSION_COUNT)> SPIRVToTextureDimension =
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

[[nodiscard]] constexpr bool FilterResource(const TRAP::Graphics::API::SPIRVTools::Resource& resource,
                                            const TRAP::Graphics::RendererAPI::ShaderStage currentStage) noexcept
{
	bool filter = false;

	//Check for invalid PushConstant
	filter = filter || (resource.Type == TRAP::Graphics::API::SPIRVTools::ResourceType::PushConstant &&
		                resource.Size > TRAP::Graphics::RendererAPI::GPUSettings.MaxPushConstantSize);

	//Remove unused resources
	filter = filter || (!resource.IsUsed);

	//Remove stage outputs
	filter = filter || (resource.Type == TRAP::Graphics::API::SPIRVTools::ResourceType::Outputs);

	//Remove stage inputs that are not on the vertex shader
	filter = filter || (resource.Type == TRAP::Graphics::API::SPIRVTools::ResourceType::Inputs &&
	                    currentStage != TRAP::Graphics::RendererAPI::ShaderStage::Vertex);

	return filter;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::ShaderReflection::ShaderReflection TRAP::Graphics::API::VkCreateShaderReflection(const std::vector<u32>& shaderCode,
                                                                                                                    const RendererAPI::ShaderStage shaderStage)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	SPIRVTools::CrossCompiler cc(shaderCode.data(), shaderCode.size());

	cc.ReflectEntryPoint();
	cc.ReflectShaderResources();
	cc.ReflectShaderVariables();

	ShaderReflection::ShaderReflection out{};

	if (shaderStage == RendererAPI::ShaderStage::Compute)
		out.NumThreadsPerGroup = cc.ReflectComputeShaderWorkGroupSize();
	else if (shaderStage == RendererAPI::ShaderStage::TessellationControl)
		out.NumControlPoint = cc.ReflectTessellationControlShaderControlPoint();

	usize vertexInputCount = 0;
	usize resourceCount = 0;
	usize variablesCount = 0;

	for(const auto& resource : cc.GetShaderResources())
	{
		if(!resource.IsUsed)
		{
			TP_WARN(TRAP::Log::ShaderSPIRVPrefix, "Found unused resource with name: ", resource.Name, "!");
		}
		if(resource.Type == TRAP::Graphics::API::SPIRVTools::ResourceType::PushConstant &&
		   resource.Size > TRAP::Graphics::RendererAPI::GPUSettings.MaxPushConstantSize)
		{
			TRAP_ASSERT(false, "VkCreateShaderReflection(): PushConstant size is bigger than device supports!");
			TP_ERROR(Log::ShaderSPIRVPrefix, "Found PushConstants with invalid size: ", resource.Size,
				     " max allowed size: ", RendererAPI::GPUSettings.MaxPushConstantSize, "!");
		}

		//Filter out what we don't use
		if(!FilterResource(resource, shaderStage))
		{
			if (resource.Type == SPIRVTools::ResourceType::Inputs &&
			    shaderStage == TRAP::Graphics::RendererAPI::ShaderStage::Vertex)
				++vertexInputCount;
			else
				++resourceCount;
		}
	}

	for(const auto& variable : cc.GetUniformVariables())
	{
		//Check if parent buffer was filtered out
		const bool parentFiltered = FilterResource(cc.GetShaderResources()[variable.ParentIndex], shaderStage);

		//Filter out what we don't use
		if(variable.IsUsed && !parentFiltered)
			++variablesCount;
	}

	out.EntryPoint = cc.GetEntryPoint();

	std::vector<ShaderReflection::VertexInput> vertexInputs{};
	//Start with the vertex input
	if(shaderStage == RendererAPI::ShaderStage::Vertex && vertexInputCount > 0)
	{
		vertexInputs.resize(vertexInputCount);

		usize j = 0;
		for(const auto& resource : cc.GetShaderResources())
		{
			//Filter out what we don't use
			if(!FilterResource(resource, shaderStage) &&
			   resource.Type == TRAP::Graphics::API::SPIRVTools::ResourceType::Inputs)
			{
				vertexInputs[j].Size = resource.Size;
				vertexInputs[j].Name = resource.Name;

				++j;
			}
		}
	}

	std::vector<u32> indexRemap{};
	std::vector<ShaderReflection::ShaderResource> resources{};
	//Continue with resources
	if(resourceCount != 0u)
	{
		indexRemap.resize(cc.GetShaderResources().size());
		resources.resize(resourceCount);

		usize j = 0;
		for(usize i = 0; i < cc.GetShaderResources().size(); ++i)
		{
			//Set index remap
			indexRemap[i] = std::numeric_limits<u32>::max();

			const SPIRVTools::Resource& resource = cc.GetShaderResources()[i];

			//Filter out what we don't use
			if(!FilterResource(resource, shaderStage) && resource.Type != SPIRVTools::ResourceType::Inputs)
			{
				//Set new index
				indexRemap[i] = NumericCast<u32>(j);

				resources[j].Type = SPIRVToDescriptorType[std::to_underlying(resource.Type)];
				resources[j].Set = resource.Set;
				resources[j].Reg = resource.Binding;
				resources[j].Size = resource.Size;
				resources[j].UsedStages = shaderStage;

				resources[j].Name = resource.Name;
				resources[j].Dim = SPIRVToTextureDimension[std::to_underlying(resource.Dimension)];

				++j;
			}
		}
	}

	std::vector<ShaderReflection::ShaderVariable> variables{};
	//Now do variables
	if(variablesCount != 0u)
	{
		variables.resize(variablesCount);

		usize j = 0;
		for(const auto& variable : cc.GetUniformVariables())
		{
			//Check if parent buffer was filtered out
			const bool parentFiltered = FilterResource(cc.GetShaderResources()[variable.ParentIndex], shaderStage);

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

	//All reflection struct should be built now
	out.ShaderStage = shaderStage;

	out.VertexInputs = vertexInputs;
	out.ShaderResources = resources;
	out.Variables = variables;

	return out;
}