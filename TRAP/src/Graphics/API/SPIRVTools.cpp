#include "TRAPPCH.h"
#include "SPIRVTools.h"

#include "RendererAPI.h"

void ReflectBoundResources(spirv_cross::Compiler& compiler,
	                       const spirv_cross::SmallVector<spirv_cross::Resource>& allResources,
	                       const std::unordered_set<spirv_cross::VariableID>& usedResources,
	                       std::vector<TRAP::Graphics::API::SPIRVTools::Resource>& resources,
	                       usize& currentResource,
	                       const TRAP::Graphics::API::SPIRVTools::ResourceType SPIRVtype)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	for(const auto& input : allResources)
	{
		TRAP::Graphics::API::SPIRVTools::Resource& resource = resources[currentResource++];

		resource.SPIRVCode.ID = input.id;
		resource.SPIRVCode.TypeID = input.type_id;
		resource.SPIRVCode.BaseTypeID = input.base_type_id;

		resource.Type = SPIRVtype;

		resource.IsUsed = (usedResources.contains(resource.SPIRVCode.ID));

		resource.Set = compiler.get_decoration(resource.SPIRVCode.ID, spv::DecorationDescriptorSet);
		resource.Binding = compiler.get_decoration(resource.SPIRVCode.ID, spv::DecorationBinding);

		const spirv_cross::SPIRType& type = compiler.get_type(resource.SPIRVCode.TypeID);

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
		//else if(!type.member_types.empty())
		//	resource.Size = compiler.get_declared_struct_size(type); //Used by UBO creation from Shader
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

TRAP::Graphics::API::SPIRVTools::CrossCompiler::CrossCompiler(const u32* const SPIRVBinary, const usize binarySize)
	: m_compiler(spirv_cross::Compiler(SPIRVBinary, binarySize))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::SPIRVTools::CrossCompiler::ReflectEntryPoint()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const auto entryPoints = m_compiler.get_entry_points_and_stages();

	if(!entryPoints.empty())
		m_entryPoint = entryPoints[0].name;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::SPIRVTools::CrossCompiler::ReflectShaderResources()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	//1. Get all shader resources
	spirv_cross::ShaderResources allResources;
	std::unordered_set<spirv_cross::VariableID> usedResources;

	allResources = m_compiler.get_shader_resources();
	usedResources = m_compiler.get_active_interface_variables();

	//2. Count number of resources and allocate array
	usize resourceCount = 0;

	//Resources we want to reflect
	resourceCount += allResources.stage_inputs.size(); //inputs
	resourceCount += allResources.stage_outputs.size(); //outputs
	resourceCount += allResources.uniform_buffers.size(); //const buffers
	resourceCount += allResources.storage_buffers.size(); //buffers
	resourceCount += allResources.separate_images.size(); //textures
	resourceCount += allResources.storage_images.size(); //uav textures
	resourceCount += allResources.separate_samplers.size(); //samplers
	resourceCount += allResources.sampled_images.size(); //combined samplers
	resourceCount += allResources.push_constant_buffers.size(); //push constants
	resourceCount += allResources.subpass_inputs.size(); //input attachments
	resourceCount += allResources.acceleration_structures.size(); //raytracing structures

	//These we dont care about right
	//subpass_inputs - we are not going to use this
	//atomic_counters - not useable in Vulkan

	//Allocate array for resources
	std::vector<Resource> resources(resourceCount);
	usize currentResource = 0;

	//3. Start by reflecting the shader inputs
	for(const auto& input : allResources.stage_inputs)
	{
		Resource& resource = resources[currentResource++];

		resource.SPIRVCode.ID = input.id;
		resource.SPIRVCode.TypeID = input.type_id;
		resource.SPIRVCode.BaseTypeID = input.base_type_id;

		resource.Type = ResourceType::Inputs;

		resource.IsUsed = usedResources.contains(resource.SPIRVCode.ID);

		resource.Set = std::numeric_limits<u32>::max(); //Stage inputs dont have sets
		//Location is the binding point for inputs
		resource.Binding = m_compiler.get_decoration(resource.SPIRVCode.ID, spv::DecorationLocation);

		const spirv_cross::SPIRType type = m_compiler.get_type(resource.SPIRVCode.TypeID);
		//bit width * vecsize = size
		resource.Size = NumericCast<u64>(type.width / 8u) * type.vecsize;

		resource.Name = input.name;
	}

	//4. Reflect output
	for(const auto& output : allResources.stage_outputs)
	{
		Resource& resource = resources[currentResource++];

		resource.SPIRVCode.ID = output.id;
		resource.SPIRVCode.TypeID = output.type_id;
		resource.SPIRVCode.BaseTypeID = output.base_type_id;

		resource.Type = ResourceType::Outputs;

		resource.IsUsed = usedResources.contains(resource.SPIRVCode.ID);

		resource.Set = std::numeric_limits<u32>::max();
		//Location is the binding point for outputs
		resource.Binding = m_compiler.get_decoration(resource.SPIRVCode.ID, spv::DecorationLocation);

		const spirv_cross::SPIRType type = m_compiler.get_type(resource.SPIRVCode.TypeID);
		//bit width * vecsize = size
		resource.Size = (NumericCast<u64>(type.width / 8u)) * type.vecsize;

		resource.Name = output.name;
	}

	//5. Reflect the "normal" resources
	ReflectBoundResources(m_compiler, allResources.uniform_buffers, usedResources, resources,
	                      currentResource, ResourceType::UniformBuffers);
	ReflectBoundResources(m_compiler, allResources.storage_buffers, usedResources, resources,
	                      currentResource, ResourceType::StorageBuffers);
	ReflectBoundResources(m_compiler, allResources.storage_images, usedResources, resources,
	                      currentResource, ResourceType::StorageImages);
	ReflectBoundResources(m_compiler, allResources.separate_images, usedResources, resources,
	                      currentResource, ResourceType::Images);
	ReflectBoundResources(m_compiler, allResources.separate_samplers, usedResources, resources,
	                      currentResource, ResourceType::Samplers);
	ReflectBoundResources(m_compiler, allResources.sampled_images, usedResources, resources,
	                      currentResource, ResourceType::CombinedSamplers);
	ReflectBoundResources(m_compiler, allResources.subpass_inputs, usedResources, resources,
	                      currentResource, ResourceType::SubpassInputs);
	ReflectBoundResources(m_compiler, allResources.subpass_inputs, usedResources, resources,
	                      currentResource, ResourceType::SubpassInputs);
	ReflectBoundResources(m_compiler, allResources.acceleration_structures, usedResources, resources,
	                      currentResource, ResourceType::AccelerationStructures);

	//6. Reflect push buffers
	for(const auto& input : allResources.push_constant_buffers)
	{
		Resource& resource = resources[currentResource++];

		resource.SPIRVCode.ID = input.id;
		resource.SPIRVCode.TypeID = input.type_id;
		resource.SPIRVCode.BaseTypeID = input.base_type_id;

		resource.Type = ResourceType::PushConstant;

		resource.IsUsed = usedResources.contains(resource.SPIRVCode.ID);

		resource.Set = std::numeric_limits<u32>::max(); //Push constants dont have sets
		resource.Binding = std::numeric_limits<u32>::max(); //Push constants dont have bindings

		const spirv_cross::SPIRType type = m_compiler.get_type(resource.SPIRVCode.TypeID);
		resource.Size = m_compiler.get_declared_struct_size(type);

		resource.Name = input.name;
		resource.Dimension = ResourceTextureDimension::Undefined;
	}

	m_shaderResources = resources;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::SPIRVTools::CrossCompiler::ReflectShaderVariables()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if (m_shaderResources.empty())
		return; //Error code here

	//1. Count number of variables we have
	usize variableCount = 0;

	for(const Resource& resource : m_shaderResources)
	{
		if(resource.Type == ResourceType::UniformBuffers || resource.Type == ResourceType::PushConstant)
		{
			const spirv_cross::SPIRType type = m_compiler.get_type(resource.SPIRVCode.TypeID);
			variableCount += type.member_types.size();
		}
	}

	//2. Allocate memory
	std::vector<Variable> variables(variableCount);
	usize currentVariable = 0;

	//3. Reflect
	for(usize i = 0; i < m_shaderResources.size(); ++i)
	{
		const Resource& resource = m_shaderResources[i];

		if(resource.Type != ResourceType::UniformBuffers && resource.Type != ResourceType::PushConstant)
			continue;

		const usize startOfBlock = currentVariable;

		const spirv_cross::SPIRType type = m_compiler.get_type(resource.SPIRVCode.TypeID);
		for(usize j = 0; j < type.member_types.size(); ++j)
		{
			Variable& variable = variables[currentVariable++];

			variable.SPIRVTypeID = type.member_types[j];

			variable.ParentSPIRVCode = resource.SPIRVCode;
			variable.ParentIndex = i;

			variable.IsUsed = false;

			variable.Size = m_compiler.get_declared_struct_member_size(type, NumericCast<u32>(j));
			variable.Offset = m_compiler.get_member_decoration(resource.SPIRVCode.BaseTypeID, NumericCast<u32>(j),
																		spv::DecorationOffset);

			variable.Name = m_compiler.get_member_name(resource.SPIRVCode.BaseTypeID, NumericCast<u32>(j));
		}

		const spirv_cross::SmallVector<spirv_cross::BufferRange> range = m_compiler.get_active_buffer_ranges(resource.SPIRVCode.ID);

		for(const auto& j : range)
			variables[startOfBlock + j.index].IsUsed = true;
	}

	m_uniformVariables = variables;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::array<u32, 3> TRAP::Graphics::API::SPIRVTools::CrossCompiler::ReflectComputeShaderWorkGroupSize() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	const spirv_cross::SPIREntryPoint& entryPoint = m_compiler.get_entry_point(m_entryPoint, m_compiler.get_execution_model());

	return {entryPoint.workgroup_size.x, entryPoint.workgroup_size.y, entryPoint.workgroup_size.z};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u32 TRAP::Graphics::API::SPIRVTools::CrossCompiler::ReflectTessellationControlShaderControlPoint() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	u32 controlPoints = m_compiler.get_entry_point(m_entryPoint, m_compiler.get_execution_model()).output_vertices;

	if(controlPoints > RendererAPI::GPUSettings.MaxTessellationControlPoints)
	{
		TP_ERROR(Log::ShaderSPIRVPrefix, "Tessellation patch control points ", controlPoints,
		        " is higher than maximum allowed ", RendererAPI::GPUSettings.MaxTessellationControlPoints, ". "
				"Setting to maximum allowed value.");
		controlPoints = RendererAPI::GPUSettings.MaxTessellationControlPoints;
	}

	return controlPoints;
}
