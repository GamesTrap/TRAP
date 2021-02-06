#include "TRAPPCH.h"
#include "BufferLayout.h"

uint32_t TRAP::Graphics::ShaderDataTypeSize(const ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:  return 4;
	case ShaderDataType::Float2: return 4 * 2;
	case ShaderDataType::Float3: return 4 * 3;
	case ShaderDataType::Float4: return 4 * 4;

	//case ShaderDataType::Mat3:   return 4 * 3 * 3;
	//case ShaderDataType::Mat4:   return 4 * 4 * 4;

	case ShaderDataType::Int:    return 4;
	case ShaderDataType::Int2:   return 4 * 2;
	case ShaderDataType::Int3:   return 4 * 3;
	case ShaderDataType::Int4:   return 4 * 4;

	case ShaderDataType::Bool:   return 1;

	default:
		TRAP_CORE_ASSERT(false, "[BufferLayout] Unknown ShaderDataType!");
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::BufferElement::BufferElement(const ShaderDataType type, std::string name, const bool normalized)
	: Name(std::move(name)), Type(type), Size(ShaderDataTypeSize(type)), Normalized(normalized)
{
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::BufferElement::GetComponentCount() const
{
	switch (Type)
	{
	case ShaderDataType::Float:   return 1;
	case ShaderDataType::Float2:  return 2;
	case ShaderDataType::Float3:  return 3;
	case ShaderDataType::Float4:  return 4;
	case ShaderDataType::Mat3:    return 3; //3 * float3
	case ShaderDataType::Mat4:    return 4; //4 * float4
	case ShaderDataType::Int:     return 1;
	case ShaderDataType::Int2:    return 2;
	case ShaderDataType::Int3:    return 3;
	case ShaderDataType::Int4:    return 4;

	default:
		TRAP_CORE_ASSERT(false, "Unknown/Invalid ShaderDataType!");
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements)
	: m_elements(elements)
{
	CalculateOffsetsAndStride();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::BufferLayout::GetStride() const
{
	return m_stride;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Graphics::BufferElement>& TRAP::Graphics::BufferLayout::GetElements() const
{
	return m_elements;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Graphics::BufferElement>::iterator TRAP::Graphics::BufferLayout::begin()
{
	return m_elements.begin();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Graphics::BufferElement>::iterator TRAP::Graphics::BufferLayout::end()
{
	return m_elements.end();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Graphics::BufferElement>::const_iterator TRAP::Graphics::BufferLayout::begin() const
{
	return m_elements.begin();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Graphics::BufferElement>::const_iterator TRAP::Graphics::BufferLayout::end() const
{
	return m_elements.end();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::BufferLayout::CalculateOffsetsAndStride()
{
	uint32_t offset = 0;
	m_stride = 0;

	for (auto& element : m_elements)
	{
		element.Offset = offset;
		offset += element.Size;
		m_stride += element.Size;
	}
}