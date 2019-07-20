#include "TRAPPCH.h"
#include "ShaderUniform.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ShaderStruct::ShaderStruct(std::string name)
	: m_name(std::move(name)), m_size(0), m_offset(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ShaderStruct::AddField(std::unique_ptr<ShaderUniformDeclaration>& field)
{
	m_size += field->GetSize();
	unsigned int offset = 0;
	if (!m_fields.empty())
		offset = m_fields.back()->GetOffset() + m_fields.back()->GetSize();
	field->SetOffset(offset);
	m_fields.emplace_back(std::move(field));
}