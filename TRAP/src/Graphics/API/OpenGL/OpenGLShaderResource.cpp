#include "TRAPPCH.h"
#include "OpenGLShaderResource.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShaderResourceDeclaration::OpenGLShaderResourceDeclaration(const Type type, const std::string& name, const unsigned int count)
	: m_name(name), m_register(0), m_count(count), m_type(type)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShaderResourceDeclaration::Type TRAP::Graphics::API::OpenGLShaderResourceDeclaration::StringToType(const std::string& type)
{
	if (type == "sampler2D")
		return Type::TEXTURE2D;
	if (type == "samplerCube")
		return Type::TEXTURECUBE;
	if (type == "samplerShadow")
		return Type::TEXTURESHADOW;

	return Type::NONE;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::OpenGLShaderResourceDeclaration::TypeToString(const Type type)
{
	switch (type)
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