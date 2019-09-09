#include "TRAPPCH.h"
#include "OpenGLShader.h"

#include "OpenGLCommon.h"
#include "Utils/String.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShader::OpenGLShader(std::string name, std::string source)
	: m_handle(0), m_name(std::move(name)), m_source(std::move(source))
{
	Init();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShader::OpenGLShader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource)
	: m_handle(0), m_name(std::move(name)), m_VSSource(std::move(VSSource)), m_FSSource(std::move(FSSource)), m_GSSource(std::move(GSSource)), m_TCSSource(std::move(TCSSource)), m_TESSource(std::move(TESSource)), m_CSSource(std::move(CSSource))
{
	Init();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShader::~OpenGLShader()
{
	TP_DEBUG("[Shader][OpenGL] Destroying Shader: \"", m_name, "\"");
	Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::Init()
{
	std::array<std::string*, 6> shaders{ &m_VSSource, &m_FSSource, &m_GSSource, &m_TCSSource, &m_TESSource, &m_CSSource };
	if (!m_source.empty())
		PreProcess(m_source, shaders);
	TP_DEBUG("[Shader][OpenGL] Compiling: \"", m_name, "\"");
	m_handle = Compile(shaders);
	if (!m_handle)
	{
		TP_WARN("[Shader][OpenGL] Shader: \"", m_name, "\" using fallback Shader: \"Passthrough\"");
		return;
	}
	CheckForUniforms();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::Shutdown() const
{
	if (m_handle)
	{
		OpenGLCall(glDeleteProgram(m_handle));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLShader::Compile(std::array<std::string*, 6>& shaders)
{
	uint32_t program = 0;
	uint32_t vertex = 0, fragment = 0, geometry = 0, tessControl = 0, tessEval = 0, compute = 0;
	int32_t linkResult = 0, validateResult = 0;
	
	if (!CreateProgram(shaders, vertex, fragment, geometry, tessControl, tessEval, compute, program))
		return 0;

	LinkProgram(linkResult, validateResult, program);

	DeleteShaders(shaders, vertex, fragment, geometry, tessControl, tessEval, compute, program);

	if (!linkResult || !validateResult)
	{
		OpenGLCall(glDeleteProgram(program));
		return 0;
	}
	
	return program;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::Bind() const
{
	if (s_CurrentlyBound != this)
	{
		if (m_handle)
		{
			OpenGLCall(glUseProgram(m_handle));
			s_CurrentlyBound = this;
		}
		else
		{
			ShaderManager::Get("Passthrough")->Bind();
			s_CurrentlyBound = ShaderManager::Get("Passthrough").get();
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::Unbind() const
{
	OpenGLCall(glUseProgram(0));
	s_CurrentlyBound = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLShader::IsTypeOpaque(const GLenum type)
{
	switch (type)
	{
	case GL_SAMPLER_1D:
		return true;
	case GL_SAMPLER_2D:
		return true;
	case GL_SAMPLER_3D:
		return true;
	case GL_SAMPLER_CUBE:
		return true;
	case GL_SAMPLER_2D_RECT:
		return true;
	case GL_SAMPLER_1D_ARRAY:
		return true;
	case GL_SAMPLER_2D_ARRAY:
		return true;
	case GL_SAMPLER_CUBE_MAP_ARRAY:
		return true;
	case GL_SAMPLER_BUFFER:
		return true;
	case GL_SAMPLER_2D_MULTISAMPLE:
		return true;
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
		return true;

	case GL_SAMPLER_1D_SHADOW:
		return true;
	case GL_SAMPLER_2D_SHADOW:
		return true;
	case GL_SAMPLER_CUBE_SHADOW:
		return true;
	case GL_SAMPLER_2D_RECT_SHADOW:
		return true;
	case GL_SAMPLER_1D_ARRAY_SHADOW:
		return true;
	case GL_SAMPLER_2D_ARRAY_SHADOW:
		return true;
	case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:
		return true;

	default:
		return false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::CheckForUniforms()
{
	Bind();

	uint32_t uniformCount = 0;
	OpenGLCall(glGetProgramiv(m_handle, GL_ACTIVE_UNIFORMS, reinterpret_cast<int32_t*>(&uniformCount)));

	if (uniformCount != 0)
	{
		bool error = false;

		uint32_t maxNameLength = 0;
		uint32_t length = 0;
		uint32_t count = 0;
		GLenum type = GL_NONE;
		OpenGLCall(glGetProgramiv(m_handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, reinterpret_cast<int32_t*>(&maxNameLength)));
		const Scope<char[]> uniformName = MakeScope<char[]>(maxNameLength);

		for (uint32_t i = 0; i < uniformCount; ++i)
		{
			OpenGLCall(glGetActiveUniform(m_handle, i, static_cast<int32_t>(maxNameLength), reinterpret_cast<int32_t*>(&length), reinterpret_cast<int32_t*>(&count), &type, uniformName.get()));

			if (Utils::String::GetCount(uniformName.get(), '.') == 0) //Ignore if UBO
			{
				if (!IsTypeOpaque(type)) //Check if is Non-Opaque type
				{
					TP_ERROR("[Shader][OpenGL] Non-Opaque Uniform: \"", uniformName.get(), "\" found! This is unsupported because of SPIR-V support!");
					error = true;
				}
			}
		}

		if (error)
		{
			TP_WARN("[Shader][OpenGL] Shader: \"", m_name, "\" using fallback Shader: \"Passthrough\"");
			Unbind();
			Shutdown();
			m_handle = 0;
			return;
		}
	}

	Unbind();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::PreProcess(const std::string& source, std::array<std::string*, 6> & shaders)
{
	ShaderType type = ShaderType::Unknown;

	std::vector<std::string> lines = Utils::String::GetLines(source);
	//Get Shader Type
	for (uint32_t i = 0; i < lines.size(); i++)
	{
		if (Utils::String::StartsWith(lines[i], "#shader"))
		{
			if (Utils::String::FindToken(lines[i], "vertex"))
				type = ShaderType::Vertex;
			else if (Utils::String::FindToken(lines[i], "fragment") || Utils::String::FindToken(lines[i], "pixel"))
				type = ShaderType::Fragment;
			else if (Utils::String::FindToken(lines[i], "geometry"))
				type = ShaderType::Geometry;
			else if (Utils::String::FindToken(lines[i], "tessellation"))
			{
				if (Utils::String::FindToken(lines[i], "control"))
					type = ShaderType::Tessellation_Control;
				else if (Utils::String::FindToken(lines[i], "evaluation"))
					type = ShaderType::Tessellation_Evaluation;
			}
			else if (Utils::String::FindToken(lines[i], "compute"))
				type = ShaderType::Compute;

			//Add version tag if doesnt exist
			if (!Utils::String::StartsWith(lines[i + 1], "#version ") && type != ShaderType::Unknown)
				shaders[static_cast<int32_t>(type) - 1]->append("#version 460 core\n");
		}
		else if (type != ShaderType::Unknown)
		{
			shaders[static_cast<int32_t>(type) - 1]->append(lines[i]);
			shaders[static_cast<int32_t>(type) - 1]->append("\n");
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLShader::CompileShader(const ShaderType type, const char* source, uint32_t& handle)
{
	OpenGLCall(handle = glCreateShader(ShaderTypeToOpenGL(type)));

	TP_DEBUG("[Shader][OpenGL] Compiling ", ShaderTypeToString(type));
	OpenGLCall(glShaderSource(handle, 1, &source, nullptr));
	OpenGLCall(glCompileShader(handle));

	int32_t result;
	OpenGLCall(glGetShaderiv(handle, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		int32_t length;
		OpenGLCall(glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length));
		std::vector<char> error(length);
		OpenGLCall(glGetShaderInfoLog(handle, length, &length, error.data()));
		const std::string errorMessage(error.data(), length - 1);
		TP_ERROR("[Shader][OpenGL][", ShaderTypeToString(type), "] Failed to compile Shader!");
		TP_ERROR("[Shader][OpenGL][", ShaderTypeToString(type), "] ", errorMessage);
		OpenGLCall(glDeleteShader(handle));

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

GLenum TRAP::Graphics::API::OpenGLShader::ShaderTypeToOpenGL(const ShaderType type)
{
	switch(type)
	{
	case ShaderType::Vertex:
		return GL_VERTEX_SHADER;

	case ShaderType::Fragment:
		return GL_FRAGMENT_SHADER;

	case ShaderType::Geometry:
		return GL_GEOMETRY_SHADER;

	case ShaderType::Tessellation_Control:
		return GL_TESS_CONTROL_SHADER;

	case ShaderType::Tessellation_Evaluation:
		return GL_TESS_EVALUATION_SHADER;

	case ShaderType::Compute:
		return GL_COMPUTE_SHADER;
		
	default:
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLShader::CreateProgram(std::array<std::string*, 6>& shaders,
	                                                  uint32_t& vertex,
                                                      uint32_t& fragment,
                                                      uint32_t& geometry,
                                                      uint32_t& tessControl,
                                                      uint32_t& tessEval,
                                                      uint32_t& compute,
                                                      uint32_t& handle)
{
	OpenGLCall(handle = glCreateProgram());

	if (!shaders[0]->empty())
	{
		if (!CompileShader(ShaderType::Vertex, shaders[0]->c_str(), vertex))
			return false;

		OpenGLCall(glAttachShader(handle, vertex));
	}

	if (!shaders[1]->empty())
	{
		if (!CompileShader(ShaderType::Fragment, shaders[1]->c_str(), fragment))
			return false;

		OpenGLCall(glAttachShader(handle, fragment));
	}

	if (!shaders[2]->empty())
	{
		if (!CompileShader(ShaderType::Geometry, shaders[2]->c_str(), geometry))
			return false;

		OpenGLCall(glAttachShader(handle, geometry));
	}

	if (!shaders[3]->empty())
	{
		if (!CompileShader(ShaderType::Tessellation_Control, shaders[3]->c_str(), tessControl))
			return false;

		OpenGLCall(glAttachShader(handle, tessControl));
	}

	if (!shaders[4]->empty())
	{
		if (!CompileShader(ShaderType::Tessellation_Evaluation, shaders[4]->c_str(), tessEval))
			return false;

		OpenGLCall(glAttachShader(handle, tessEval));
	}

	if (!shaders[5]->empty())
	{
		if (!CompileShader(ShaderType::Compute, shaders[5]->c_str(), compute))
			return false;

		OpenGLCall(glAttachShader(handle, compute));
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::LinkProgram(int32_t& linkResult, int32_t& validateResult, const uint32_t& handle)
{
	OpenGLCall(glLinkProgram(handle));
	OpenGLCall(glGetProgramiv(handle, GL_LINK_STATUS, &linkResult));
	if (linkResult == GL_FALSE)
	{
		int32_t length;
		OpenGLCall(glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length));
		std::vector<char> error(length);
		OpenGLCall(glGetProgramInfoLog(handle, length, &length, error.data()));
		const std::string errorMessage(error.data(), length - 1);
		TP_ERROR("[Shader][OpenGL][Program] Failed to link Program!");
		TP_ERROR("[Shader][OpenGL][Program] ", errorMessage);
	}

	if (linkResult == GL_TRUE)
	{
		OpenGLCall(glValidateProgram(handle));
		OpenGLCall(glGetProgramiv(handle, GL_VALIDATE_STATUS, &validateResult));
		if (validateResult == GL_FALSE)
			TP_ERROR("[Shader][OpenGL][Program] Failed to validate Program!");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::DeleteShaders(std::array<std::string*, 6>& shaders,
                                                      const uint32_t& vertex,
                                                      const uint32_t& fragment,
                                                      const uint32_t& geometry,
                                                      const uint32_t& tessControl,
                                                      const uint32_t& tessEval,
                                                      const uint32_t& compute,
                                                      const uint32_t& handle)
{
	if (!shaders[0]->empty())
	{
		OpenGLCall(glDetachShader(handle, vertex));
		OpenGLCall(glDeleteShader(vertex));
	}

	if (!shaders[1]->empty())
	{
		OpenGLCall(glDetachShader(handle, fragment));
		OpenGLCall(glDeleteShader(fragment));
	}

	if (!shaders[2]->empty())
	{
		OpenGLCall(glDetachShader(handle, geometry));
		OpenGLCall(glDeleteShader(geometry));
	}

	if (!shaders[3]->empty())
	{
		OpenGLCall(glDetachShader(handle, tessControl));
		OpenGLCall(glDeleteShader(tessControl));
	}

	if (!shaders[4]->empty())
	{
		OpenGLCall(glDetachShader(handle, tessEval));
		OpenGLCall(glDeleteShader(tessEval));
	}

	if (!shaders[5]->empty())
	{
		OpenGLCall(glDetachShader(handle, compute));
		OpenGLCall(glDeleteShader(compute));
	}
}
