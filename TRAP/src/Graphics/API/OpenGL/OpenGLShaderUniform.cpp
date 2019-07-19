#include "TRAPPCH.h"
#include "OpenGLShaderUniform.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(const Type type, std::string name, const unsigned int count)
	: m_name(std::move(name)), m_size(SizeOfUniformType(type) * count), m_count(count), m_offset(0), m_type(type), m_struct(nullptr), m_location(0)
{
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(std::unique_ptr<ShaderStruct>& uniformStruct, std::string name, const unsigned int count)
	: m_name(std::move(name)), m_size(0), m_count(count), m_offset(0), m_type(Type::STRUCT), m_struct(std::move(uniformStruct)), m_location(0)
{
	m_size = m_struct->GetSize() * count;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShaderUniformDeclaration::SetOffset(const unsigned int offset)
{
	if (m_type == Type::STRUCT)
		m_struct->SetOffset(offset);

	m_offset = offset;
}

//-------------------------------------------------------------------------------------------------------------------//

unsigned int TRAP::Graphics::API::OpenGLShaderUniformDeclaration::SizeOfUniformType(const Type type)
{
	switch (type)
	{
	case Type::INT32:
		return 4;

	case Type::FLOAT32:
		return 4;

	case Type::VEC2:
		return  4 * 2;

	case Type::VEC3:
		return 4 * 3;

	case Type::VEC4:
		return 4 * 4;

	case Type::MAT3:
		return 4 * 3 * 3;

	case Type::MAT4:
		return 4 * 4 * 4;

	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShaderUniformDeclaration::Type TRAP::Graphics::API::OpenGLShaderUniformDeclaration::StringToType(const std::string_view type)
{
	if (type == "int32")
		return Type::INT32;
	if (type == "float")
		return Type::FLOAT32;
	if (type == "vec2")
		return Type::VEC2;
	if (type == "vec3")
		return Type::VEC3;
	if (type == "vec4")
		return Type::VEC4;
	if (type == "mat3")
		return Type::MAT3;
	if (type == "mat4")
		return Type::MAT4;

	return Type::NONE;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::OpenGLShaderUniformDeclaration::TypeToString(const Type type)
{
	switch (type)
	{
	case Type::INT32:
		return "int32";

	case Type::FLOAT32:
		return "float";

	case Type::VEC2:
		return "vec2";

	case Type::VEC3:
		return "vec3";

	case Type::VEC4:
		return "vec4";

	case Type::MAT3:
		return "mat3";

	case Type::MAT4:
		return "mat4";

	default:
		return "Invalid Type";
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShaderUniformBufferDeclaration::OpenGLShaderUniformBufferDeclaration(std::string name, const unsigned int shaderType)
	: m_name(std::move(name)), m_register(0), m_size(0), m_shaderType(shaderType)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShaderUniformBufferDeclaration::PushUniform(std::unique_ptr<OpenGLShaderUniformDeclaration>& uniform)
{
	unsigned int offset = 0;
	if (!m_uniforms.empty())
	{
		auto* previous = dynamic_cast<OpenGLShaderUniformDeclaration*>(m_uniforms.back().get());
		offset = previous->m_offset + previous->m_size;
	}
	uniform->SetOffset(offset);
	m_size += uniform->GetSize();
	m_uniforms.emplace_back(std::move(uniform));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ShaderUniformDeclaration* TRAP::Graphics::API::OpenGLShaderUniformBufferDeclaration::FindUniform(const std::string& name)
{
	for (const auto& uniform : m_uniforms)
		if (uniform->GetName() == name)
			return uniform.get();

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::API::ShaderStruct& TRAP::Graphics::API::OpenGLShaderUniformDeclaration::GetShaderUniformStruct() const
{
	TP_ASSERT(m_struct.get(), "[Shader] Struct is invalid!");

	return *m_struct;
}