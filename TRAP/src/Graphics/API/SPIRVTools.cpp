#include "TRAPPCH.h"
#include "SPIRVTools.h"

void ReflectBoundResources(spirv_cross::Compiler& compiler,
	                       const spirv_cross::SmallVector<spirv_cross::Resource>& allResources,
	                       const std::unordered_set<spirv_cross::VariableID>& usedResources,
	                       std::vector<TRAP::Graphics::API::SPIRVTools::Resource>& resources,
	                       uint32_t& currentResource,
	                       const TRAP::Graphics::API::SPIRVTools::ResourceType SPIRVtype)
{
	for(std::size_t i = 0; i < allResources.size(); ++i)
	{
		spirv_cross::Resource const& input = allResources[i];
		TRAP::Graphics::API::SPIRVTools::Resource& resource = resources[currentResource++];

		resource.SPIRVCode.ID = input.id;
		resource.SPIRVCode.TypeID = input.type_id;
		resource.SPIRVCode.BaseTypeID = input.base_type_id;

		resource.Type = SPIRVtype;

		resource.IsUsed = (usedResources.count(resource.SPIRVCode.ID) != 0);

		resource.Set = compiler.get_decoration(resource.SPIRVCode.ID, spv::DecorationDescriptorSet);
		resource.Binding = compiler.get_decoration(resource.SPIRVCode.ID, spv::DecorationBinding);

		spirv_cross::SPIRType type = compiler.get_type(resource.SPIRVCode.TypeID);

		//Special case for textureBuffer / imageBuffer
		//textureBuffer is considered as separate images with dimension buffer in SPIRV but they require a
		//buffer descriptor of type uniform texel buffer
		//imageBuffer is considered as storage image with dimension buffer in SPIRV but they require a
		//buffer descriptor of type storage texel buffer
		if(type.image.dim == spv::Dim::DimBuffer)
		{
			if (SPIRVtype == TRAP::Graphics::API::SPIRVTools::ResourceType::Images)
				resource.Type = TRAP::Graphics::API::SPIRVTools::ResourceType::UniformTexelBuffers;
			else if (SPIRVtype == TRAP::Graphics::API::SPIRVTools::ResourceType::StorageImages)
				resource.Type = TRAP::Graphics::API::SPIRVTools::ResourceType::StorageTexelBuffers;
		}

		//Set the texture dimensions
		switch(type.image.dim)
		{
		case spv::DimBuffer:
			resource.Dimension = TRAP::Graphics::API::SPIRVTools::ResourceTextureDimension::Buffer;
			break;

		case spv::Dim1D:
			resource.Dimension = type.image.arrayed
				                     ? TRAP::Graphics::API::SPIRVTools::ResourceTextureDimension::Texture1DArray
				                     : TRAP::Graphics::API::SPIRVTools::ResourceTextureDimension::Texture1D;
			break;

		case spv::Dim2D:
			if (type.image.ms)
				resource.Dimension = type.image.arrayed
					                     ? TRAP::Graphics::API::SPIRVTools::ResourceTextureDimension::Texture2DMSArray
					                     : TRAP::Graphics::API::SPIRVTools::ResourceTextureDimension::Texture2DMS;
			else
				resource.Dimension = type.image.arrayed
					                     ? TRAP::Graphics::API::SPIRVTools::ResourceTextureDimension::Texture2DArray
					                     : TRAP::Graphics::API::SPIRVTools::ResourceTextureDimension::Texture2D;
			break;

		case spv::Dim3D:
			resource.Dimension = TRAP::Graphics::API::SPIRVTools::ResourceTextureDimension::Texture3D;
			break;

		case spv::DimCube:
			resource.Dimension = type.image.arrayed
				                     ? TRAP::Graphics::API::SPIRVTools::ResourceTextureDimension::TextureCubeArray
				                     : TRAP::Graphics::API::SPIRVTools::ResourceTextureDimension::TextureCube;
			break;

		default:
			resource.Dimension = TRAP::Graphics::API::SPIRVTools::ResourceTextureDimension::Undefined;
			break;
		}

		if (!type.array.empty())
			resource.Size = type.array[0];
		else if(!type.member_types.empty())
			resource.Size = compiler.get_declared_struct_size(type); //Used by UBO creation from Shader
		else
			resource.Size = 1;

		//Use the instance name if there is one
		std::string name = compiler.get_name(resource.SPIRVCode.ID);
		if (name.empty())
			name = input.name;

		resource.Name = name;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::SPIRVTools::CrossCompiler TRAP::Graphics::API::SPIRVTools::CreateCrossCompiler(const uint32_t* SPIRVBinary,
                                                                                                    const uint32_t binarySize)
{
	CrossCompiler comp;

	//Build the compiler
	comp.Compiler = TRAP::MakeScope<spirv_cross::Compiler>(SPIRVBinary, binarySize);

	comp.ShaderResources = {};

	comp.UniformVariables = {};

	return comp;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::SPIRVTools::DestroyCrossCompiler(CrossCompiler& compiler)
{
	compiler.Compiler.reset();

	compiler.ShaderResources.clear();
	compiler.UniformVariables.clear();

	compiler.Compiler = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::SPIRVTools::ReflectEntryPoint(CrossCompiler& compiler)
{
	compiler.EntryPoint = compiler.Compiler->get_entry_points_and_stages()[0].name;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::SPIRVTools::ReflectShaderResources(CrossCompiler& compiler)
{
	//1. Get all shader resources
	spirv_cross::ShaderResources allResources;
	std::unordered_set<spirv_cross::VariableID> usedResources;

	allResources = compiler.Compiler->get_shader_resources();
	usedResources = compiler.Compiler->get_active_interface_variables();

	//2. Count number of resources and allocate array
	uint32_t resourceCount = 0;

	//Resources we want to reflect
	resourceCount += static_cast<uint32_t>(allResources.stage_inputs.size()); //inputs
	resourceCount += static_cast<uint32_t>(allResources.stage_outputs.size()); //outputs
	resourceCount += static_cast<uint32_t>(allResources.uniform_buffers.size()); //const buffers
	resourceCount += static_cast<uint32_t>(allResources.storage_buffers.size()); //buffers
	resourceCount += static_cast<uint32_t>(allResources.separate_images.size()); //textures
	resourceCount += static_cast<uint32_t>(allResources.storage_images.size()); //uav textures
	resourceCount += static_cast<uint32_t>(allResources.separate_samplers.size()); //samplers
	resourceCount += static_cast<uint32_t>(allResources.sampled_images.size()); //combined samplers
	resourceCount += static_cast<uint32_t>(allResources.push_constant_buffers.size()); //push constants
	resourceCount += static_cast<uint32_t>(allResources.subpass_inputs.size()); //input attachments
	resourceCount += static_cast<uint32_t>(allResources.acceleration_structures.size()); //raytracing structures

	//These we dont care about right
	//subpass_inputs - we are not going to use this
	//atomic_counters - not useable in Vulkan

	//Allocate array for resources
	std::vector<Resource> resources(resourceCount);
	uint32_t currentResource = 0;

	//3. Start by reflecting the shader inputs
	for(std::size_t i = 0; i < allResources.stage_inputs.size(); ++i)
	{
		spirv_cross::Resource const& input = allResources.stage_inputs[i];
		Resource& resource = resources[currentResource++];

		resource.SPIRVCode.ID = input.id;
		resource.SPIRVCode.TypeID = input.type_id;
		resource.SPIRVCode.BaseTypeID = input.base_type_id;

		resource.Type = ResourceType::Inputs;

		resource.IsUsed = usedResources.count(resource.SPIRVCode.ID) != 0;

		resource.Set = static_cast<uint32_t>(-1); //Stage inputs dont have sets
		//Location is the binding point for inputs
		resource.Binding = compiler.Compiler->get_decoration(resource.SPIRVCode.ID, spv::DecorationLocation);

		spirv_cross::SPIRType type = compiler.Compiler->get_type(resource.SPIRVCode.TypeID);
		//bit width * vecsize = size
		resource.Size = (type.width / 8) * type.vecsize;

		resource.Name = input.name;
	}

	//4. Reflect output
	for(std::size_t i = 0; i < allResources.stage_outputs.size(); ++i)
	{
		spirv_cross::Resource const& input = allResources.stage_outputs[i];
		Resource& resource = resources[currentResource++];

		resource.SPIRVCode.ID = input.id;
		resource.SPIRVCode.TypeID = input.type_id;
		resource.SPIRVCode.BaseTypeID = input.base_type_id;

		resource.Type = ResourceType::Outputs;

		resource.IsUsed = usedResources.count(resource.SPIRVCode.ID) != 0;

		resource.Set = static_cast<uint32_t>(-1);
		//Location is the binding point for outputs
		resource.Binding = compiler.Compiler->get_decoration(resource.SPIRVCode.ID, spv::DecorationLocation);

		spirv_cross::SPIRType type = compiler.Compiler->get_type(resource.SPIRVCode.TypeID);
		//bit width * vecsize = size
		resource.Size = (type.width / 8) * type.vecsize;

		resource.Name = input.name;
	}

	//5. Reflect the "normal" resources
	ReflectBoundResources(*compiler.Compiler, allResources.uniform_buffers, usedResources, resources,
	                      currentResource, ResourceType::UniformBuffers);
	ReflectBoundResources(*compiler.Compiler, allResources.storage_buffers, usedResources, resources,
	                      currentResource, ResourceType::StorageBuffers);
	ReflectBoundResources(*compiler.Compiler, allResources.storage_images, usedResources, resources,
	                      currentResource, ResourceType::StorageImages);
	ReflectBoundResources(*compiler.Compiler, allResources.separate_images, usedResources, resources,
	                      currentResource, ResourceType::Images);
	ReflectBoundResources(*compiler.Compiler, allResources.separate_samplers, usedResources, resources,
	                      currentResource, ResourceType::Samplers);
	ReflectBoundResources(*compiler.Compiler, allResources.sampled_images, usedResources, resources,
	                      currentResource, ResourceType::CombinedSamplers);
	ReflectBoundResources(*compiler.Compiler, allResources.subpass_inputs, usedResources, resources,
	                      currentResource, ResourceType::SubpassInputs);
	ReflectBoundResources(*compiler.Compiler, allResources.subpass_inputs, usedResources, resources,
	                      currentResource, ResourceType::SubpassInputs);
	ReflectBoundResources(*compiler.Compiler, allResources.acceleration_structures, usedResources, resources,
	                      currentResource, ResourceType::AccelerationStructures);

	//6. Reflect push buffers
	for(std::size_t i = 0; i < allResources.push_constant_buffers.size(); ++i)
	{
		spirv_cross::Resource const& input = allResources.push_constant_buffers[i];
		Resource& resource = resources[currentResource++];

		resource.SPIRVCode.ID = input.id;
		resource.SPIRVCode.TypeID = input.type_id;
		resource.SPIRVCode.BaseTypeID = input.base_type_id;

		resource.Type = ResourceType::PushConstant;

		resource.IsUsed = usedResources.count(resource.SPIRVCode.ID) != 0;

		resource.Set = static_cast<uint32_t>(-1); //Push constants dont have sets
		resource.Binding = static_cast<uint32_t>(-1); //Push constants dont have bindings

		spirv_cross::SPIRType type = compiler.Compiler->get_type(resource.SPIRVCode.TypeID);
		resource.Size = compiler.Compiler->get_declared_struct_size(type);

		resource.Name = input.name;
		resource.Dimension = ResourceTextureDimension::Undefined;
	}

	compiler.ShaderResources = resources;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::SPIRVTools::ReflectShaderVariables(CrossCompiler& compiler)
{
	if (compiler.ShaderResources.empty())
		return; //Error code here

	//1. Count number of variables we have
	std::size_t variableCount = 0;

	for(std::size_t i = 0; i < compiler.ShaderResources.size(); ++i)
	{
		Resource& resource = compiler.ShaderResources[i];

		if(resource.Type == ResourceType::UniformBuffers || resource.Type == ResourceType::PushConstant)
		{
			spirv_cross::SPIRType type = compiler.Compiler->get_type(resource.SPIRVCode.TypeID);
			variableCount += type.member_types.size();
		}
	}

	//2. Allocate memory
	std::vector<Variable> variables(variableCount);
	std::size_t currentVariable = 0;

	//3. Reflect
	for(std::size_t i = 0; i < compiler.ShaderResources.size(); ++i)
	{
		Resource& resource = compiler.ShaderResources[i];

		if(resource.Type != ResourceType::UniformBuffers && resource.Type != ResourceType::PushConstant)
			continue;

		std::size_t startOfBlock = currentVariable;

		spirv_cross::SPIRType type = compiler.Compiler->get_type(resource.SPIRVCode.TypeID);
		for(std::size_t j = 0; j < type.member_types.size(); ++j)
		{
			Variable& variable = variables[currentVariable++];

			variable.SPIRVTypeID = type.member_types[j];

			variable.ParentSPIRVCode = resource.SPIRVCode;
			variable.ParentIndex = i;

			variable.IsUsed = false;

			variable.Size = compiler.Compiler->get_declared_struct_member_size(type, j);
			variable.Offset = compiler.Compiler->get_member_decoration(resource.SPIRVCode.BaseTypeID, j,
																		spv::DecorationOffset);

			variable.Name = compiler.Compiler->get_member_name(resource.SPIRVCode.BaseTypeID, j);
		}

		spirv_cross::SmallVector<spirv_cross::BufferRange> range = compiler.Compiler->get_active_buffer_ranges(resource.SPIRVCode.ID);

		for(std::size_t j = 0; j < range.size(); ++j)
			variables[startOfBlock + range[j].index].IsUsed = true;
	}

	compiler.UniformVariables = variables;
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<uint32_t, 3> TRAP::Graphics::API::SPIRVTools::ReflectComputeShaderWorkGroupSize(CrossCompiler& compiler)
{
	std::array<uint32_t, 3> res{};

	spirv_cross::SPIREntryPoint& entryPoint = compiler.Compiler->get_entry_point(compiler.EntryPoint,
	                                                                             compiler.Compiler->get_execution_model());

	res[0] = entryPoint.workgroup_size.x;
	res[1] = entryPoint.workgroup_size.y;
	res[2] = entryPoint.workgroup_size.z;

	return res;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::SPIRVTools::ReflectTessellationControlShaderControlPoint(CrossCompiler& compiler)
{
	return compiler.Compiler->get_entry_point(compiler.EntryPoint,
	                                          compiler.Compiler->get_execution_model()).output_vertices;
}