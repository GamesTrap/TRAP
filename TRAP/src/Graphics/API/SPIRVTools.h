#ifndef TRAP_SPIRVTOOLS_H
#define TRAP_SPIRVTOOLS_H

#include <array>
#include <cstdint>
#include <span>
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

	/// @brief Type of resource.
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
		CombinedSamplers
	};

	/// @brief Texture dimension.
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
		TextureCubeArray = 10
	};

	/// @brief Struct describing a shader resource.
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

	/// @brief Struct describing a shader variable.
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

	/// @brief SPIRV cross compiler data
	class CrossCompiler
	{
	public:
		/// @brief Create a new SPIRV cross compiler instance.
		/// @param SPIRVBinary SPIRV binary data.
		explicit CrossCompiler(std::span<const u32> SPIRVBinary);

		/// @brief Destructor.
		~CrossCompiler() = default;

		consteval CrossCompiler(const CrossCompiler&) = delete;
		CrossCompiler(CrossCompiler&&) noexcept = default;
		consteval CrossCompiler& operator=(const CrossCompiler&) noexcept = delete;
		CrossCompiler& operator=(CrossCompiler&&) = default;

		/// @brief Reflect the shaders entry point.
		void ReflectEntryPoint();
		/// @brief Reflect the shaders used resources.
		void ReflectShaderResources();
		/// @brief Reflect the shaders uniform variables.
		void ReflectShaderVariables();

		/// @brief Retrieve the compute shader work group size.
		/// @return Compute shader work group size.
		[[nodiscard]] std::array<u32, 3> GetComputeShaderWorkGroupSize() const;
		/// @brief Retrieve tessellation control shader control point count.
		/// @return Tessellation control shader control point count.
		[[nodiscard]] u32 GetTessellationControlShaderControlPoint() const;

		/// @brief Retrieve the shader resources.
		/// @return Shader resources.
		[[nodiscard]] constexpr const std::vector<Resource>& GetShaderResources() const noexcept;

		/// @brief Retrieve the uniform variables.
		/// @return Uniform variables.
		[[nodiscard]] constexpr const std::vector<Variable>& GetUniformVariables() const noexcept;

		/// @brief Rertieve the name of the entry point.
		/// @return Name of entry point.
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
