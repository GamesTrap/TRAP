#include "TRAPPCH.h"
#include "BufferLayout.h"

uint32_t TRAP::Graphics::BufferElement::GetComponentCount() const
{
	switch (Type)
	{
	case ShaderDataType::Float:   return 1;
	case ShaderDataType::Float2:  return 2;
	case ShaderDataType::Float3:  return 3;
	case ShaderDataType::Float4:  return 4;

	case ShaderDataType::Int:     return 1;
	case ShaderDataType::Int2:    return 2;
	case ShaderDataType::Int3:    return 3;
	case ShaderDataType::Int4:    return 4;

	default:
		TP_CORE_ASSERT(false, "Unknown/Invalid ShaderDataType!");
		return 0;
	}
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