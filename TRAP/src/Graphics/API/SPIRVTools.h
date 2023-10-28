#ifndef TRAP_SPIRVTOOLS_H
#define TRAP_SPIRVTOOLS_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
//SPIRV
#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>
//SPIRV to GLSL
#include <spirv_glsl.hpp>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

#include "Core/Types.h"

namespace TRAP::Graphics::API::SPIRVTools
{
	struct Type
	{
		//Resources are identifier with their SPIR-V ID.
		//This is the ID of the OpVariable.
		u32 ID;

		//The type ID of the variable which includes arrays and all type modifications.
		//This type ID is not suitable for parsing OpMemberDecoration of a struct and other decorations in general
		//since these modifications typically happen n the BaseTypeId.
		u32 TypeID;

		//The base type of the declared resource.
		//This type is the ase type which ignores pointers and arrays of the TypeID.
		//This is mostly useful to parse decorations of the underlying type.
		u32 BaseTypeID;
	};

	/// <summary>
	/// Type of resource.
	/// </summary>
	enum class ResourceType : u32
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
	enum class ResourceTextureDimension : u32
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
		u32 Set;

		//The resource binding locations
		u32 Binding;

		//The size of the resource.
		//This will be the descriptor array size for textures
		u64 Size;

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
		u32 SPIRVTypeID;

		//Parents SPIRV code
		Type ParentSPIRVCode;

		//Parents resource index
		u64 ParentIndex;

		//If the data was used
		bool IsUsed;

		//The offset of the Variable.
		u32 Offset;

		//The size of the Variable.
		u64 Size;

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
		CrossCompiler(const u32* SPIRVBinary, usize binarySize);

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
		[[nodiscard]] std::array<u32, 3> ReflectComputeShaderWorkGroupSize() const;
		/// <summary>
		/// Reflect tessellation control shader control point count.
		/// </summary>
		/// <returns>Tessellation control shader control point count.</returns>
		[[nodiscard]] u32 ReflectTessellationControlShaderControlPoint() const;

		/// <summary>
		/// Retrieve the shader resources.
		/// </summary>
		/// <returns>Shader resources.</returns>
		[[nodiscard]] constexpr const std::vector<Resource>& GetShaderResources() const noexcept;

		/// <summary>
		/// Retrieve the uniform variables.
		/// </summary>
		/// <returns>Uniform variables.</returns>
		[[nodiscard]] constexpr const std::vector<Variable>& GetUniformVariables() const noexcept;

		/// <summary>
		/// Rertieve the name of the entry point.
		/// </summary>
		/// <returns>Name of entry point.</returns>
		[[nodiscard]] constexpr std::string GetEntryPoint() const noexcept;

	private:
		//This points to the internal compiler class
		spirv_cross::Compiler m_compiler;

		//Resources
		std::vector<Resource> m_shaderResources{};

		//Uniforms
		std::vector<Variable> m_uniformVariables{};

		std::string m_entryPoint{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Graphics::API::SPIRVTools::Resource>& TRAP::Graphics::API::SPIRVTools::CrossCompiler::GetShaderResources() const noexcept
{
	return m_shaderResources;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Graphics::API::SPIRVTools::Variable>& TRAP::Graphics::API::SPIRVTools::CrossCompiler::GetUniformVariables() const noexcept
{
	return m_uniformVariables;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Graphics::API::SPIRVTools::CrossCompiler::GetEntryPoint() const noexcept
{
	return m_entryPoint;
}

#endif /*TRAP_SPIRVTOOLS_H*/