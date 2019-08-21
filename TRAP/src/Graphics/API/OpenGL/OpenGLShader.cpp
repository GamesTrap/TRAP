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
	if(!m_source.empty())
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

unsigned int TRAP::Graphics::API::OpenGLShader::Compile(std::array<std::string*, 6> & shaders)
{
	OpenGLCall(const unsigned int program = glCreateProgram());

	const char* VSSource;
	unsigned int vertex = 0;
	if (!shaders[0]->empty())
	{
		VSSource = shaders[0]->c_str();
		OpenGLCall(vertex = glCreateShader(GL_VERTEX_SHADER));

		TP_DEBUG("[Shader][OpenGL] Compiling Vertex Shader");
		OpenGLCall(glShaderSource(vertex, 1, &VSSource, nullptr));
		OpenGLCall(glCompileShader(vertex));

		int result;
		OpenGLCall(glGetShaderiv(vertex, GL_COMPILE_STATUS, &result));
		if (result == GL_FALSE)
		{
			int length;
			OpenGLCall(glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &length));
			std::vector<char> error(length);
			OpenGLCall(glGetShaderInfoLog(vertex, length, &length, error.data()));
			const std::string errorMessage(error.data(), length - 1);
			TP_ERROR("[Shader][OpenGL][VertexShader] Failed to compile Shader!");
			TP_ERROR("[Shader][OpenGL][VertexShader] ", errorMessage);
			OpenGLCall(glDeleteShader(vertex));

			return 0;
		}

		OpenGLCall(glAttachShader(program, vertex));
	}

	const char* FSSource;
	unsigned int fragment = 0;
	if (!shaders[1]->empty())
	{
		FSSource = shaders[1]->c_str();
		OpenGLCall(fragment = glCreateShader(GL_FRAGMENT_SHADER));

		TP_DEBUG("[Shader][OpenGL] Compiling Fragment Shader");
		OpenGLCall(glShaderSource(fragment, 1, &FSSource, nullptr));
		OpenGLCall(glCompileShader(fragment));

		int result;
		OpenGLCall(glGetShaderiv(fragment, GL_COMPILE_STATUS, &result));
		if (result == GL_FALSE)
		{
			int length;
			OpenGLCall(glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &length));
			std::vector<char> error(length);
			OpenGLCall(glGetShaderInfoLog(fragment, length, &length, error.data()));
			const std::string errorMessage(error.data(), length - 1);
			TP_ERROR("[Shader][OpenGL][FragmentShader] Failed to compile Shader!");
			TP_ERROR("[Shader][OpenGL][FragmentShader] ", errorMessage);
			OpenGLCall(glDeleteShader(fragment));

			return 0;
		}

		OpenGLCall(glAttachShader(program, fragment));
	}

	const char* GSSource;
	unsigned int geometry = 0;
	if (!shaders[2]->empty())
	{
		GSSource = shaders[2]->c_str();
		OpenGLCall(geometry = glCreateShader(GL_GEOMETRY_SHADER));

		TP_DEBUG("[Shader][OpenGL] Compiling Geometry Shader");
		OpenGLCall(glShaderSource(geometry, 1, &GSSource, nullptr));
		OpenGLCall(glCompileShader(geometry));

		int result;
		OpenGLCall(glGetShaderiv(geometry, GL_COMPILE_STATUS, &result));
		if (result == GL_FALSE)
		{
			int length;
			OpenGLCall(glGetShaderiv(geometry, GL_INFO_LOG_LENGTH, &length));
			std::vector<char> error(length);
			OpenGLCall(glGetShaderInfoLog(geometry, length, &length, error.data()));
			const std::string errorMessage(error.data(), length - 1);
			TP_ERROR("[Shader][OpenGL][GeometryShader] Failed to compile Shader!");
			TP_ERROR("[Shader][OpenGL][GeometryShader] ", errorMessage);
			OpenGLCall(glDeleteShader(geometry));

			return 0;
		}

		OpenGLCall(glAttachShader(program, geometry));
	}

	const char* TCSSource;
	unsigned int tesscontrol = 0;
	if (!shaders[3]->empty())
	{
		TCSSource = shaders[3]->c_str();
		OpenGLCall(tesscontrol = glCreateShader(GL_TESS_CONTROL_SHADER));

		TP_DEBUG("[Shader][OpenGL] Compiling Tessellation Control Shader");
		OpenGLCall(glShaderSource(tesscontrol, 1, &TCSSource, nullptr));
		OpenGLCall(glCompileShader(tesscontrol));

		int result;
		OpenGLCall(glGetShaderiv(tesscontrol, GL_COMPILE_STATUS, &result));
		if (result == GL_FALSE)
		{
			int length;
			OpenGLCall(glGetShaderiv(tesscontrol, GL_INFO_LOG_LENGTH, &length));
			std::vector<char> error(length);
			OpenGLCall(glGetShaderInfoLog(tesscontrol, length, &length, error.data()));
			const std::string errorMessage(error.data(), length - 1);
			TP_ERROR("[Shader][OpenGL][TessellationControlShader] Failed to compile Shader!");
			TP_ERROR("[Shader][OpenGL][TessellationControlShader] ", errorMessage);
			OpenGLCall(glDeleteShader(tesscontrol));

			return 0;
		}

		OpenGLCall(glAttachShader(program, tesscontrol));
	}

	const char* TESSource;
	unsigned int tesseval = 0;
	if (!shaders[4]->empty())
	{
		TESSource = shaders[4]->c_str();
		OpenGLCall(tesseval = glCreateShader(GL_TESS_EVALUATION_SHADER));

		TP_DEBUG("[Shader][OpenGL] Compiling Tessellation Evaluation Shader");
		OpenGLCall(glShaderSource(tesseval, 1, &TESSource, nullptr));
		OpenGLCall(glCompileShader(tesseval));

		int result;
		OpenGLCall(glGetShaderiv(tesseval, GL_COMPILE_STATUS, &result));
		if (result == GL_FALSE)
		{
			int length;
			OpenGLCall(glGetShaderiv(tesseval, GL_INFO_LOG_LENGTH, &length));
			std::vector<char> error(length);
			OpenGLCall(glGetShaderInfoLog(tesseval, length, &length, error.data()));
			const std::string errorMessage(error.data(), length - 1);
			TP_ERROR("[Shader][OpenGL][TessellationEvaluationShader] Failed to compile Shader!");
			TP_ERROR("[Shader][OpenGL][TessellationEvaluationShader] ", errorMessage);
			OpenGLCall(glDeleteShader(tesseval));

			return 0;
		}

		OpenGLCall(glAttachShader(program, tesseval));
	}

	const char* CSSSource;
	unsigned int compute = 0;
	if (!shaders[5]->empty())
	{
		CSSSource = shaders[5]->c_str();
		OpenGLCall(compute = glCreateShader(GL_COMPUTE_SHADER));

		TP_DEBUG("[Shader][OpenGL] Compiling Compute Shader");
		OpenGLCall(glShaderSource(compute, 1, &CSSSource, nullptr));
		OpenGLCall(glCompileShader(compute));

		int result;
		OpenGLCall(glGetShaderiv(compute, GL_COMPILE_STATUS, &result));
		if (result == GL_FALSE)
		{
			int length;
			OpenGLCall(glGetShaderiv(compute, GL_INFO_LOG_LENGTH, &length));
			std::vector<char> error(length);
			OpenGLCall(glGetShaderInfoLog(compute, length, &length, error.data()));
			const std::string errorMessage(error.data(), length - 1);
			TP_ERROR("[Shader][OpenGL][ComputeShader] Failed to compile Shader!");
			TP_ERROR("[Shader][OpenGL][ComputeShader] ", errorMessage);
			OpenGLCall(glDeleteShader(compute));

			return 0;
		}

		OpenGLCall(glAttachShader(program, compute));
	}

	OpenGLCall(glLinkProgram(program));
	int linkResult;
	OpenGLCall(glGetProgramiv(program, GL_LINK_STATUS, &linkResult));
	if (linkResult == GL_FALSE)
	{
		int length;
		OpenGLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
		std::vector<char> error(length);
		OpenGLCall(glGetProgramInfoLog(program, length, &length, error.data()));
		const std::string errorMessage(error.data(), length - 1);
		TP_ERROR("[Shader][OpenGL][Program] Failed to link Program!");
		TP_ERROR("[Shader][OpenGL][Program] ", errorMessage);
	}

	int validateResult = 0;
	if (linkResult == GL_TRUE)
	{
		OpenGLCall(glValidateProgram(program));
		OpenGLCall(glGetProgramiv(program, GL_VALIDATE_STATUS, &validateResult));
		if (validateResult == GL_FALSE)
			TP_ERROR("[Shader][OpenGL][Program] Failed to validate Program!");
	}

	if (!shaders[0]->empty())
	{
		OpenGLCall(glDetachShader(program, vertex));
		OpenGLCall(glDeleteShader(vertex));
	}

	if (!shaders[1]->empty())
	{
		OpenGLCall(glDetachShader(program, fragment));
		OpenGLCall(glDeleteShader(fragment));
	}

	if (!shaders[2]->empty())
	{
		OpenGLCall(glDetachShader(program, geometry));
		OpenGLCall(glDeleteShader(geometry));
	}

	if (!shaders[3]->empty())
	{
		OpenGLCall(glDetachShader(program, tesscontrol));
		OpenGLCall(glDeleteShader(tesscontrol));
	}

	if (!shaders[4]->empty())
	{
		OpenGLCall(glDetachShader(program, tesseval));
		OpenGLCall(glDeleteShader(tesseval));
	}

	if (!shaders[5]->empty())
	{
		OpenGLCall(glDetachShader(program, compute));
		OpenGLCall(glDeleteShader(compute));
	}

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
			s_CurrentlyBound = ShaderManager::Get("Passthrough");
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
	switch(type)
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

	if(uniformCount != 0)
	{
		bool error = false;
		
		uint32_t maxNameLength = 0;
		uint32_t length = 0;
		uint32_t count = 0;
		GLenum type = GL_NONE;
		OpenGLCall(glGetProgramiv(m_handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, reinterpret_cast<int32_t*>(&maxNameLength)));
		const std::unique_ptr<char[]> uniformName = std::make_unique<char[]>(maxNameLength);

		for(uint32_t i = 0; i < uniformCount; ++i)
		{
			OpenGLCall(glGetActiveUniform(m_handle, i, static_cast<int32_t>(maxNameLength), reinterpret_cast<int32_t*>(&length), reinterpret_cast<int32_t*>(&count), &type, uniformName.get()));

			if(Utils::String::GetCount(uniformName.get(), '.') == 0) //Ignore if UBO
			{
				if(!IsTypeOpaque(type)) //Check if is Non-Opaque type
				{
					TP_ERROR("[Shader][OpenGL] Non-Opaque Uniform: \"", uniformName, "\" found! This is unsupported because of SPIR-V support!");
					error = true;
				}
			}
		}

		if(error)
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

void TRAP::Graphics::API::OpenGLShader::PreProcess(const std::string& source, std::array<std::string*, 6>& shaders)
{
	ShaderType type = ShaderType::Unknown;

	std::vector<std::string> lines = Utils::String::GetLines(source);
	//Get Shader Type
	for(unsigned int i = 0; i < lines.size(); i++)
	{
		if(Utils::String::StartsWith(lines[i], "#shader"))
		{
			if (Utils::String::FindToken(lines[i], "vertex"))
				type = ShaderType::Vertex;
			else if (Utils::String::FindToken(lines[i], "fragment"))
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
			if(!Utils::String::StartsWith(lines[i + 1], "#version ") && type != ShaderType::Unknown)
				shaders[static_cast<int32_t>(type) - 1]->append("#version 460 core\n");
		}
		else if(type != ShaderType::Unknown)
		{
			shaders[static_cast<int32_t>(type) - 1]->append(lines[i]);
			shaders[static_cast<int32_t>(type) - 1]->append("\n");
		}
	}
}