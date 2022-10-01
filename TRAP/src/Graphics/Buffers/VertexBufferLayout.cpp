#include "TRAPPCH.h"
#include "VertexBufferLayout.h"

TRAP::Graphics::VertexBufferElement::VertexBufferElement(const ShaderDataType type, std::string name,
                                                         const bool normalized)
	: Name(std::move(name)), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
{
	ZoneScoped;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::VertexBufferElement::GetComponentCount() const
{
	ZoneScoped;

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
		TRAP_ASSERT(false, "Unknown/Invalid shader data type!");
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::VertexBufferLayout::VertexBufferLayout(const std::initializer_list<VertexBufferElement>& elements)
	: m_elements(elements)
{
	ZoneScoped;

	CalculateOffsetsAndStride();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::VertexBufferLayout::GetStride() const
{
	ZoneScoped;

	return m_stride;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Graphics::VertexBufferElement>& TRAP::Graphics::VertexBufferLayout::GetElements() const
{
	ZoneScoped;

	return m_elements;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Graphics::VertexBufferElement>::iterator TRAP::Graphics::VertexBufferLayout::begin()
{
	ZoneScoped;

	return m_elements.begin();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Graphics::VertexBufferElement>::iterator TRAP::Graphics::VertexBufferLayout::end()
{
	ZoneScoped;

	return m_elements.end();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Graphics::VertexBufferElement>::const_iterator TRAP::Graphics::VertexBufferLayout::begin() const
{
	ZoneScoped;

	return m_elements.begin();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Graphics::VertexBufferElement>::const_iterator TRAP::Graphics::VertexBufferLayout::end() const
{
	ZoneScoped;

	return m_elements.end();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::VertexBufferLayout::CalculateOffsetsAndStride()
{
	ZoneScoped;

	uint32_t offset = 0;
	m_stride = 0;

	for (auto& element : m_elements)
	{
		element.Offset = offset;
		offset += element.Size;
		m_stride += element.Size;
	}
}