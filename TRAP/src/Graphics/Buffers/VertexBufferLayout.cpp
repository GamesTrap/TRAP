#include "TRAPPCH.h"
#include "VertexBufferLayout.h"

TRAP::Graphics::VertexBufferElement::VertexBufferElement(const ShaderDataType type, std::string name,
                                                         const bool normalized) noexcept
	: Name(std::move(name)), Type(type), Size(ShaderDataTypeSize(type)), Normalized(normalized)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::ShaderDataTypeSize(const ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:  return 4;
	case ShaderDataType::Float2: return 4 * 2;
	case ShaderDataType::Float3: return 4 * 3;
	case ShaderDataType::Float4: return 4 * 4;

	case ShaderDataType::Mat3:   return 4 * 3 * 3;
	case ShaderDataType::Mat4:   return 4 * 4 * 4;

	case ShaderDataType::Int:    return 4;
	case ShaderDataType::Int2:   return 4 * 2;
	case ShaderDataType::Int3:   return 4 * 3;
	case ShaderDataType::Int4:   return 4 * 4;

	case ShaderDataType::Bool:   return 1;

	default:
		TRAP_ASSERT(false, "ShaderDataTypeSize(): Unknown shader data type!");
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::VertexBufferElement::GetComponentCount() const
{
	switch (Type)
	{
	case ShaderDataType::Float:   return 1;
	case ShaderDataType::Float2:  return 2;
	case ShaderDataType::Float3:  return 3;
	case ShaderDataType::Float4:  return 4;
	case ShaderDataType::Mat3:    return 3 * 3;
	case ShaderDataType::Mat4:    return 4 * 4;
	case ShaderDataType::Int:     return 1;
	case ShaderDataType::Int2:    return 2;
	case ShaderDataType::Int3:    return 3;
	case ShaderDataType::Int4:    return 4;
	case ShaderDataType::Bool:    return 1;

	default:
		TRAP_ASSERT(false, "VertexBufferElement::GetComponentCount(): Unknown/Invalid shader data type!");
		return 0;
	}
}
