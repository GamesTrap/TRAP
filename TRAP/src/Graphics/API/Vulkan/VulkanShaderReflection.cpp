#include "TRAPPCH.h"
#include "VulkanShaderReflection.h"

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/ShaderReflection.h"
#include "Graphics/API/SPIRVTools.h"

namespace
{
	[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::DescriptorType SPIRVToDescriptorType(const TRAP::Graphics::API::SPIRVTools::ResourceType resourceType)
	{
		using namespace TRAP::Graphics;
		using namespace TRAP::Graphics::API::SPIRVTools;

		switch(resourceType)
		{
		case ResourceType::Inputs:
			return RendererAPI::DescriptorType::Undefined;

		case ResourceType::Outputs:
			return RendererAPI::DescriptorType::Undefined;

		case ResourceType::UniformBuffers:
			return RendererAPI::DescriptorType::UniformBuffer;

		case ResourceType::StorageBuffers:
			return RendererAPI::DescriptorType::RWBuffer;

		case ResourceType::Images:
			return RendererAPI::DescriptorType::Texture;

		case ResourceType::StorageImages:
			return RendererAPI::DescriptorType::RWTexture;

		case ResourceType::Samplers:
			return RendererAPI::DescriptorType::Sampler;

		case ResourceType::PushConstant:
			return RendererAPI::DescriptorType::RootConstant;

		case ResourceType::SubpassInputs:
			return RendererAPI::DescriptorType::InputAttachment;

		case ResourceType::UniformTexelBuffers:
			return RendererAPI::DescriptorType::TexelBuffer;

		case ResourceType::StorageTexelBuffers:
			return RendererAPI::DescriptorType::RWTexelBuffer;

		case ResourceType::AccelerationStructures:
			return RendererAPI::DescriptorType::RayTracing;

		case ResourceType::CombinedSamplers:
			return RendererAPI::DescriptorType::CombinedImageSampler;
		}

		TRAP_ASSERT(false, "SPIRVToDescriptorType(): Unknown TRAP::Graphics::API::SPIRVTools::ResourceType!");
		return RendererAPI::DescriptorType::Undefined;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr TRAP::Graphics::API::ShaderReflection::TextureDimension SPIRVToTextureDimension(const TRAP::Graphics::API::SPIRVTools::ResourceTextureDimension resourceTextureDimension)
	{
		using namespace TRAP::Graphics::API::ShaderReflection;
		using namespace TRAP::Graphics::API::SPIRVTools;

		switch(resourceTextureDimension)
		{
		case ResourceTextureDimension::Undefined:
			return TextureDimension::TextureDimUndefined;

		case ResourceTextureDimension::Buffer:
			return TextureDimension::TextureDimUndefined;

		case ResourceTextureDimension::Texture1D:
			return TextureDimension::TextureDim1D;

		case ResourceTextureDimension::Texture1DArray:
			return TextureDimension::TextureDim1DArray;

		case ResourceTextureDimension::Texture2D:
			return TextureDimension::TextureDim2D;

		case ResourceTextureDimension::Texture2DArray:
			return TextureDimension::TextureDim2DArray;

		case ResourceTextureDimension::Texture2DMS:
			return TextureDimension::TextureDim2DMS;

		case ResourceTextureDimension::Texture2DMSArray:
			return TextureDimension::TextureDim2DMSArray;

		case ResourceTextureDimension::Texture3D:
			return TextureDimension::TextureDim3D;

		case ResourceTextureDimension::TextureCube:
			return TextureDimension::TextureDimCube;

		case ResourceTextureDimension::TextureCubeArray:
			return TextureDimension::TextureDimCubeArray;
		}

		TRAP_ASSERT(false, "SPIRVToTextureDimension(): Unknown TRAP::Graphics::API::SPIRVTools::ResourceTextureDimension!");
		return TextureDimension::TextureDimUndefined;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr bool FilterResource(const TRAP::Graphics::API::SPIRVTools::Resource& resource,
												const TRAP::Graphics::RendererAPI::ShaderStage currentStage) noexcept
	{
		bool filter = false;

		//Check for invalid PushConstant
		filter |= (resource.Type == TRAP::Graphics::API::SPIRVTools::ResourceType::PushConstant &&
				   resource.Size > TRAP::Graphics::RendererAPI::GPUSettings.MaxPushConstantSize);

		//Remove unused resources
		filter |= !resource.IsUsed;

		//Remove stage outputs
		filter |= (resource.Type == TRAP::Graphics::API::SPIRVTools::ResourceType::Outputs);

		//Remove stage inputs that are not on the vertex shader
		filter |= (resource.Type == TRAP::Graphics::API::SPIRVTools::ResourceType::Inputs &&
				   currentStage != TRAP::Graphics::RendererAPI::ShaderStage::Vertex);

		return filter;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr std::vector<TRAP::Graphics::API::ShaderReflection::VertexInput> GetVertexInputs(const std::span<const TRAP::Graphics::API::SPIRVTools::Resource> shaderResources)
	{
		std::vector<TRAP::Graphics::API::ShaderReflection::VertexInput> vertexInputs{};
		vertexInputs.reserve(shaderResources.size());

		for(const auto& resource : shaderResources)
		{
			//Filter out what we don't use
			if(!FilterResource(resource, TRAP::Graphics::RendererAPI::ShaderStage::Vertex) &&
			   resource.Type == TRAP::Graphics::API::SPIRVTools::ResourceType::Inputs)
			{
				vertexInputs.emplace_back(resource.Size, resource.Name);
			}
		}

		vertexInputs.shrink_to_fit();

		return vertexInputs;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr std::vector<TRAP::Graphics::API::ShaderReflection::ShaderResource> GetShaderResources(const TRAP::Graphics::RendererAPI::ShaderStage shaderStage,
	                                                                                                              const std::span<const TRAP::Graphics::API::SPIRVTools::Resource> shaderResources)
	{
		std::vector<TRAP::Graphics::API::ShaderReflection::ShaderResource> resources{};
		resources.reserve(shaderResources.size());

		for(const TRAP::Graphics::API::SPIRVTools::Resource& resource : shaderResources)
		{
			//Filter out what we don't use
			if(!FilterResource(resource, shaderStage) &&
			   resource.Type != TRAP::Graphics::API::SPIRVTools::ResourceType::Inputs)
			{
				resources.emplace_back(SPIRVToDescriptorType(resource.Type), resource.Set, resource.Binding,
				                       resource.Size, shaderStage, resource.Name,
									   SPIRVToTextureDimension(resource.Dimension));
			}
		}

		resources.shrink_to_fit();

		return resources;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr std::vector<u32> GetIndexMap(const TRAP::Graphics::RendererAPI::ShaderStage shaderStage,
	                                                     const std::span<const TRAP::Graphics::API::SPIRVTools::Resource> shaderResources)
	{
		std::vector<u32> indexRemap(shaderResources.size(), std::numeric_limits<u32>::max());

		for(usize i = 0, j = 0; i < shaderResources.size(); ++i)
		{
			const TRAP::Graphics::API::SPIRVTools::Resource& resource = shaderResources[i];

			//Filter out what we don't use
			if(!FilterResource(resource, shaderStage) && resource.Type != TRAP::Graphics::API::SPIRVTools::ResourceType::Inputs)
			{
				//Set new index
				indexRemap[i] = NumericCast<u32>(j);
				++j;
			}
		}

		return indexRemap;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr std::vector<TRAP::Graphics::API::ShaderReflection::ShaderVariable> GetShaderVariables(const TRAP::Graphics::RendererAPI::ShaderStage shaderStage,
	                                                                                                              const std::span<const TRAP::Graphics::API::SPIRVTools::Variable> uniformVariables,
																												  const std::span<const TRAP::Graphics::API::SPIRVTools::Resource> shaderResources)
	{
		const std::vector<u32> indexRemap = GetIndexMap(shaderStage, shaderResources);

		std::vector<TRAP::Graphics::API::ShaderReflection::ShaderVariable> variables{};
		variables.reserve(uniformVariables.size());

		//Now do variables
		for(const auto& variable : uniformVariables)
		{
			//Check if parent buffer was filtered out
			const bool parentFiltered = FilterResource(shaderResources[variable.ParentIndex], shaderStage);

			//Filter out what we don't use
			if(variable.IsUsed && !parentFiltered)
				variables.emplace_back(indexRemap[variable.ParentIndex], variable.Offset, variable.Size, variable.Name);
		}

		variables.shrink_to_fit();

		return variables;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::ShaderReflection::ShaderReflection TRAP::Graphics::API::VkCreateShaderReflection(const std::span<const u32> shaderCode,
                                                                                                                    const RendererAPI::ShaderStage shaderStage)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	SPIRVTools::CrossCompiler cc(shaderCode);

	cc.ReflectEntryPoint();
	cc.ReflectShaderResources();
	cc.ReflectShaderVariables();

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
	}

	ShaderReflection::ShaderReflection out
	{
		.ShaderStage = shaderStage,
		.ShaderResources = GetShaderResources(shaderStage, cc.GetShaderResources()),
		.Variables = GetShaderVariables(shaderStage, cc.GetUniformVariables(), cc.GetShaderResources()),
		.EntryPoint = cc.GetEntryPoint()
	};

	if(shaderStage == RendererAPI::ShaderStage::Vertex)
		out.VertexInputs = GetVertexInputs(cc.GetShaderResources());
	else if (shaderStage == RendererAPI::ShaderStage::Compute)
		out.NumThreadsPerGroup = cc.GetComputeShaderWorkGroupSize();
	else if (shaderStage == RendererAPI::ShaderStage::TessellationControl)
		out.NumControlPoint = cc.GetTessellationControlShaderControlPoint();

	return out;
}
