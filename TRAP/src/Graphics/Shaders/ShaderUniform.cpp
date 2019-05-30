#include "TRAPPCH.h"
#include "ShaderUniform.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ShaderStruct::ShaderStruct(std::string name)
	: m_name(std::move(name)), m_size(0), m_offset(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::ShaderStruct::AddField(ShaderUniformDeclaration* field)
{
	m_size += field->GetSize();
	unsigned int offset = 0;
	if (!m_fields.empty())
	{
		const std::unique_ptr<ShaderUniformDeclaration>& previous = m_fields.back();
		offset = previous->GetOffset() + previous->GetSize();
	}
	field->SetOffset(offset);
	m_fields.push_back(std::unique_ptr<ShaderUniformDeclaration>(field));
}