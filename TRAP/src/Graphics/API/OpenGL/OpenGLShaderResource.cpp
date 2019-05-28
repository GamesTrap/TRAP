#include "TRAPPCH.h"
#include "OpenGLShaderResource.h"

namespace TRAP::Graphics::API
{
	OpenGLShaderResourceDeclaration::OpenGLShaderResourceDeclaration(const Type type, const std::string& name, const unsigned int count)
		: m_name(name), m_register(0), m_count(count), m_type(type)
	{		
	}

	OpenGLShaderResourceDeclaration::Type OpenGLShaderResourceDeclaration::StringToType(const std::string& type)
	{
		if (type == "sampler2D")
			return Type::TEXTURE2D;
		if (type == "samplerCube")
			return Type::TEXTURECUBE;
		if (type == "samplerShadow")
			return Type::TEXTURESHADOW;

		return Type::NONE;
	}

	std::string OpenGLShaderResourceDeclaration::TypeToString(const Type type)
	{
		switch(type)
		{
		case Type::TEXTURE2D:
			return "sampler2D";

		case Type::TEXTURECUBE:
			return "samplerCube";

		case Type::TEXTURESHADOW:
			return "samplerShadow";

		default:
			return "Invalid Type";
		}
	}

	const std::string& OpenGLShaderResourceDeclaration::GetName() const
	{
		return m_name;
	}

	unsigned int OpenGLShaderResourceDeclaration::GetRegister() const
	{
		return m_register;
	}

	unsigned int OpenGLShaderResourceDeclaration::GetCount() const
	{
		return m_count;
	}

	OpenGLShaderResourceDeclaration::Type OpenGLShaderResourceDeclaration::GetType() const
	{
		return m_type;
	}
}