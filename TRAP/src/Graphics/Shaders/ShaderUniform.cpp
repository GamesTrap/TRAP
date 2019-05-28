#include "TRAPPCH.h"
#include "ShaderUniform.h"

namespace TRAP::Graphics::API
{
	ShaderStruct::ShaderStruct(std::string name)
		: m_name(std::move(name)), m_size(0), m_offset(0)
	{		
	}

	void ShaderStruct::AddField(ShaderUniformDeclaration* field)
	{
		m_size += field->GetSize();
		unsigned int offset = 0;
		if(!m_fields.empty())
		{
			const std::unique_ptr<ShaderUniformDeclaration>& previous = m_fields.back();
			offset = previous->GetOffset() + previous->GetSize();
		}
		field->SetOffset(offset);
		m_fields.push_back(std::unique_ptr<ShaderUniformDeclaration>(field));
	}

	void ShaderStruct::SetOffset(const unsigned int offset)
	{
		m_offset = offset;
	}

	std::string_view ShaderStruct::GetName() const
	{
		return m_name;
	}

	unsigned int ShaderStruct::GetSize() const
	{
		return m_size;
	}

	unsigned int ShaderStruct::GetOffset() const
	{
		return m_offset;
	}

	const std::vector<std::unique_ptr<ShaderUniformDeclaration>>& ShaderStruct::GetFields() const
	{
		return m_fields;
	}
}