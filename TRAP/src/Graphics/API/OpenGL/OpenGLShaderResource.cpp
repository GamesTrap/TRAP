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
		return Type::Texture2D;
	if (type == "samplerCube")
		return Type::TextureCube;
	if (type == "samplerShadow")
		return Type::TextureShadow;

	return Type::NONE;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::OpenGLShaderResourceDeclaration::TypeToString(const Type type)
{
	switch (type)
	{
	case Type::Texture2D:
		return "sampler2D";

	case Type::TextureCube:
		return "samplerCube";

	case Type::TextureShadow:
		return "samplerShadow";

	default:
		return "Invalid Type";
	}
}