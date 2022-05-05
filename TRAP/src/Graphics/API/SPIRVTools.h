#ifndef TRAP_SPIRVTOOLS_H
#define TRAP_SPIRVTOOLS_H

namespace TRAP::Graphics::API::SPIRVTools
{
	struct Type
	{
		//Resources are identifier with their SPIR-V ID.
		//This is the ID of the OpVariable.
		uint32_t ID;

		//The type ID of the variable which includes arrays and all type modifications.
		//This type ID is not suitable for parsing OpMemberDecoration of a struct and other decorations in general
		//since these modifications typically happen n the BaseTypeId.
		uint32_t TypeID;

		//The base type of the declared resource.
		//This type is the ase type which ignores pointers and arrays of the TypeID.
		//This is mostly useful to parse decorations of the underlying type.
		uint32_t BaseTypeID;
	};

	/// <summary>
	/// Type of resource.
	/// </summary>
	enum class ResourceType
	{
		Inputs = 0,
		Outputs,
		UniformBuffers,
		StorageBuffers,
		Images,
		StorageImages,
		Samplers,
		PushConstant,
		SubpassInputs,
		UniformTexelBuffers,
		StorageTexelBuffers,
		AccelerationStructures,
		CombinedSamplers,

		RESOURCE_TYPE_COUNT
	};

	/// <summary>
	/// Texture dimension.
	/// </summary>
	enum class ResourceTextureDimension
	{
		Undefined = 0,
		Buffer = 1,
		Texture1D = 2,
		Texture1DArray = 3,
		Texture2D = 4,
		Texture2DArray = 5,
		Texture2DMS = 6,
		Texture2DMSArray = 7,
		Texture3D = 8,
		TextureCube = 9,
		TextureCubeArray = 10,

		RESOURCE_TEXTURE_DIMENSION_COUNT = 11
	};

	/// <summary>
	/// Struct describing a shader resource.
	/// </summary>
	struct Resource
	{
		//SPIRV data type
		TRAP::Graphics::API::SPIRVTools::Type SPIRVCode;

		//Resource type
		ResourceType Type;

		//Texture dimension.
		//Undefined if not a Texture.
		ResourceTextureDimension Dimension;

		//If the resource was used in the shader
		bool IsUsed;

		//The resource set if it has one
		uint32_t Set;

		//The resource binding locations
		uint32_t Binding;

		//The size of the resource.
		//This will be the descriptor array size for textures
		uint64_t Size;

		//The declared name (OpName) of the resource.
		//For Buffer blocks, the name actually reflects the externally
		//visible Block name.
		std::string Name;
	};

	/// <summary>
	/// Struct describing a shader variable.
	/// </summary>
	struct Variable
	{
		//SPIRV data type
		uint32_t SPIRVTypeID;

		//Parents SPIRV code
		Type ParentSPIRVCode;

		//Parents resource index
		uint32_t ParentIndex;

		//If the data was used
		bool IsUsed;

		//The offset of the Variable.
		uint32_t Offset;

		//The size of the Variable.
		uint64_t Size;

		//Variable name
		std::string Name;
	};

	/// <summary>
	/// SPIRV cross compiler data
	/// </summary>
	class CrossCompiler
	{
	public:
		/// <summary>
		/// Create a new SPIRV cross compiler instance.
		/// </summary>
		/// <param name="SPIRVBinary">SPIRV binary data.</param>
		/// <param name="binarySize">Size of the SPIRV binary data.</param>
		CrossCompiler(const uint32_t* SPIRVBinary, uint32_t binarySize);

		/// <summary>
		/// Reflect the shaders entry point.
		/// </summary>
		void ReflectEntryPoint();
		/// <summary>
		/// Reflect the shaders used resources.
		/// </summary>
		void ReflectShaderResources();
		/// <summary>
		/// Reflect the shaders uniform variables.
		/// </summary>
		void ReflectShaderVariables();
		/// <summary>
		/// Reflect the compute shader work group size.
		/// </summary>
		/// <returns>Compute shader work group size.</returns>
		std::array<uint32_t, 3> ReflectComputeShaderWorkGroupSize();
		/// <summary>
		/// Reflect tessellation control shader control point count.
		/// </summary>
		/// <returns>Tessellation control shader control point count.</returns>
		uint32_t ReflectTessellationControlShaderControlPoint();

		/// <summary>
		/// Retrieve the shader resources.
		/// </summary>
		/// <returns>Shader resources.</returns>
		const std::vector<Resource>& GetShaderResources() const;

		/// <summary>
		/// Retrieve the uniform variables.
		/// </summary>
		/// <returns>Uniform variables.</returns>
		const std::vector<Variable>& GetUniformVariables() const;

		/// <summary>
		/// Rertieve the name of the entry point.
		/// </summary>
		/// <returns>Name of entry point.</returns>
		std::string GetEntryPoint();

	private:
		//This points to the internal compiler class
		TRAP::Scope<spirv_cross::Compiler> m_compiler;

		//Resources
		std::vector<Resource> m_shaderResources;

		//Uniforms
		std::vector<Variable> m_uniformVariables;

		std::string m_entryPoint;
	};
}

#endif /*TRAP_SPIRVTOOLS_H*/