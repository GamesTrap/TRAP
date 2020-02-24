#include "TRAPPCH.h"
#include "OpenGLShader.h"

#include "OpenGLCommon.h"
#include "Utils/String.h"
#include "Embed.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShader::OpenGLShader(std::string name, const std::string& source)
	: m_handle(0), m_name(std::move(name))
{
	TP_PROFILE_FUNCTION();
	
	InitGLSL(source);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShader::OpenGLShader(std::string name, std::vector<uint32_t>& source)
	: m_handle(0), m_name(std::move(name))
{
	TP_PROFILE_FUNCTION();
	
	InitSPIRV(source);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShader::OpenGLShader(std::string name, std::string VSSource, std::string FSSource, std::string GSSource, std::string TCSSource, std::string TESSource, std::string CSSource)
	: m_handle(0), m_name(std::move(name))
{
	TP_PROFILE_FUNCTION();
	
	InitGLSL(std::move(VSSource), std::move(FSSource), std::move(GSSource), std::move(TCSSource), std::move(TESSource), std::move(CSSource));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShader::~OpenGLShader()
{
	TP_PROFILE_FUNCTION();
	
	TP_DEBUG("[Shader][OpenGL] Destroying Shader: \"", m_name, "\"");
	Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::Shutdown() const
{
	TP_PROFILE_FUNCTION();

	if (m_handle)
	{
		OpenGLCall(glDeleteProgram(m_handle));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::Bind() const
{
	TP_PROFILE_FUNCTION();
	
	if (s_CurrentlyBound != this)
	{
		if (m_handle)
		{
			OpenGLCall(glUseProgram(m_handle));
			s_CurrentlyBound = this;
		}
		else
			ShaderManager::Get("Fallback")->Bind();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::Unbind() const
{
	TP_PROFILE_FUNCTION();
	
	OpenGLCall(glUseProgram(0));
	s_CurrentlyBound = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::API::OpenGLShader::GetName() const
{
	TP_PROFILE_FUNCTION();

	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::API::OpenGLShader::GetFilePath() const
{
	TP_PROFILE_FUNCTION();

	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLShader::GetHandle() const
{
	TP_PROFILE_FUNCTION();

	return m_handle;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::InitGLSL(const std::string& source)
{
	TP_DEBUG("[Shader][OpenGL][GLSL] Compiling: \"", m_name, "\"");
	
	std::array<std::string, 6> shaders{};
	if (!source.empty())
		PreProcess(source, shaders);
	
	bool isEmpty = true;
	for(const auto& shaderSource : shaders)
	{
		if(!shaderSource.empty())
		{
			isEmpty = false;
			break;
		}
	}
	if(isEmpty)
	{
		TP_WARN("[Shader][OpenGL][GLSL] Shader: \"", m_name, "\" using fallback Shader: \"Fallback\"");
		return;
	}
	
	m_handle = CompileGLSL(shaders);
	if (!m_handle)
	{
		TP_WARN("[Shader][OpenGL][GLSL] Shader: \"", m_name, "\" using fallback Shader: \"Fallback\"");
		return;
	}
	CheckForUniforms();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::InitGLSL(std::string VSSource,
                                                 std::string FSSource,
                                                 std::string GSSource,
                                                 std::string TCSSource,
                                                 std::string TESSource,
                                                 std::string CSSource)
{
	TP_DEBUG("[Shader][OpenGL][GLSL] Compiling: \"", m_name, "\"");
	
	std::array<std::string, 6> shaders{ std::move(VSSource),
										std::move(FSSource),
										std::move(GSSource),
										std::move(TCSSource),
										std::move(TESSource),
										std::move(CSSource)
	};	

	bool isEmpty = true;
	for (const auto& shaderSource : shaders)
	{
		if (!shaderSource.empty())
		{
			isEmpty = false;
			break;
		}
	}
	if (isEmpty)
	{
		TP_WARN("[Shader][OpenGL][GLSL] Shader: \"", m_name, "\" using fallback Shader: \"Fallback\"");
		return;
	}
	
	m_handle = CompileGLSL(shaders);
	if (!m_handle)
	{
		TP_WARN("[Shader][OpenGL][GLSL] Shader: \"", m_name, "\" using fallback Shader: \"Fallback\"");
		return;
	}
	CheckForUniforms();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::InitSPIRV(std::vector<uint32_t>& source)
{
	std::array<std::vector<uint32_t>, 6> SPIRVShaders{};
	uint32_t index = 0;
	const uint32_t SPIRVSubShaderCount = source[index++];	

	for(uint32_t i = 0; i < SPIRVSubShaderCount; i++)
	{
		const uint32_t SPIRVSize = source[index++];
		uint32_t shaderType = source[index++];

		const std::vector<uint32_t> tempSPIRV = std::vector<uint32_t>(source.begin() + index, source.begin() + index + SPIRVSize);
		index += SPIRVSize;

		switch(static_cast<ShaderType>(shaderType))
		{
		case ShaderType::Vertex:
			SPIRVShaders[0] = tempSPIRV;
			break;

		case ShaderType::Fragment:
			SPIRVShaders[1] = tempSPIRV;
			break;

		case ShaderType::Geometry:
			SPIRVShaders[2] = tempSPIRV;
			break;

		case ShaderType::Tessellation_Control:
			SPIRVShaders[3] = tempSPIRV;
			break;

		case ShaderType::Tessellation_Evaluation:
			SPIRVShaders[4] = tempSPIRV;
			break;

		case ShaderType::Compute:
			SPIRVShaders[5] = tempSPIRV;
			break;
			
		default:
			break;
		}
	}

	TP_DEBUG("[Shader][OpenGL][SPIR-V] Converting: \"", m_name, "\" to GLSL");
	std::array<std::string, 6> GLSLShaders = ConvertSPIRVToGLSL(SPIRVShaders);
	TP_DEBUG("[Shader][OpenGL][GLSL] Compiling: \"", m_name, "\"");
	m_handle = CompileGLSL(GLSLShaders);
	if (!m_handle)
	{
		TP_WARN("[Shader][OpenGL][GLSL] Shader: \"", m_name, "\" using fallback Shader: \"Fallback\"");
		return;
	}
	CheckForUniforms();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLShader::CompileGLSL(std::array<std::string, 6>& shaders)
{
	uint32_t program = 0;
	uint32_t vertex = 0, fragment = 0, geometry = 0, tessControl = 0, tessEval = 0, compute = 0;
	int32_t linkResult = 0, validateResult = 0;
	
	if (!CreateGLSLProgram(shaders, vertex, fragment, geometry, tessControl, tessEval, compute, program))
		return 0;

	LinkGLSLProgram(linkResult, validateResult, program);

	DeleteGLSLShaders(shaders, vertex, fragment, geometry, tessControl, tessEval, compute, program);

	if (!linkResult || !validateResult)
	{
		OpenGLCall(glDeleteProgram(program));
		return 0;
	}
	
	return program;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLShader::CompileGLSLShader(const ShaderType type, const char* source, uint32_t& handle)
{
	OpenGLCall(handle = glCreateShader(ShaderTypeToOpenGL(type)));

	TP_DEBUG("[Shader][OpenGL][GLSL] Compiling ", ShaderTypeToString(type));
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
		TP_ERROR("[Shader][OpenGL][GLSL][", ShaderTypeToString(type), "] Failed to compile Shader!");
		TP_ERROR("[Shader][OpenGL][GLSL][", ShaderTypeToString(type), "] ", errorMessage);
		OpenGLCall(glDeleteShader(handle));

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLShader::CreateGLSLProgram(std::array<std::string, 6>& shaders,
	uint32_t& vertex,
	uint32_t& fragment,
	uint32_t& geometry,
	uint32_t& tessControl,
	uint32_t& tessEval,
	uint32_t& compute,
	uint32_t& handle)
{
	OpenGLCall(handle = glCreateProgram());

	if (!shaders[0].empty())
	{
		if (!CompileGLSLShader(ShaderType::Vertex, shaders[0].c_str(), vertex))
			return false;

		OpenGLCall(glAttachShader(handle, vertex));
	}

	if (!shaders[1].empty())
	{
		if (!CompileGLSLShader(ShaderType::Fragment, shaders[1].c_str(), fragment))
			return false;

		OpenGLCall(glAttachShader(handle, fragment));
	}

	if (!shaders[2].empty())
	{
		if (!CompileGLSLShader(ShaderType::Geometry, shaders[2].c_str(), geometry))
			return false;

		OpenGLCall(glAttachShader(handle, geometry));
	}

	if (!shaders[3].empty())
	{
		if (!CompileGLSLShader(ShaderType::Tessellation_Control, shaders[3].c_str(), tessControl))
			return false;

		OpenGLCall(glAttachShader(handle, tessControl));
	}

	if (!shaders[4].empty())
	{
		if (!CompileGLSLShader(ShaderType::Tessellation_Evaluation, shaders[4].c_str(), tessEval))
			return false;

		OpenGLCall(glAttachShader(handle, tessEval));
	}

	if (!shaders[5].empty())
	{
		if (!CompileGLSLShader(ShaderType::Compute, shaders[5].c_str(), compute))
			return false;

		OpenGLCall(glAttachShader(handle, compute));
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::LinkGLSLProgram(int32_t& linkResult, int32_t& validateResult, const uint32_t& handle)
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
		TP_ERROR("[Shader][OpenGL][GLSL][Program] Failed to link Program!");
		TP_ERROR("[Shader][OpenGL][GLSL][Program] ", errorMessage);
	}

	if (linkResult == GL_TRUE)
	{
		OpenGLCall(glValidateProgram(handle));
		OpenGLCall(glGetProgramiv(handle, GL_VALIDATE_STATUS, &validateResult));
		if (validateResult == GL_FALSE)
			TP_ERROR("[Shader][OpenGL][GLSL][Program] Failed to validate Program!");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::DeleteGLSLShaders(std::array<std::string, 6>& shaders,
	const uint32_t& vertex,
	const uint32_t& fragment,
	const uint32_t& geometry,
	const uint32_t& tessControl,
	const uint32_t& tessEval,
	const uint32_t& compute,
	const uint32_t& handle)
{
	if (!shaders[0].empty())
	{
		OpenGLCall(glDetachShader(handle, vertex));
		OpenGLCall(glDeleteShader(vertex));
	}

	if (!shaders[1].empty())
	{
		OpenGLCall(glDetachShader(handle, fragment));
		OpenGLCall(glDeleteShader(fragment));
	}

	if (!shaders[2].empty())
	{
		OpenGLCall(glDetachShader(handle, geometry));
		OpenGLCall(glDeleteShader(geometry));
	}

	if (!shaders[3].empty())
	{
		OpenGLCall(glDetachShader(handle, tessControl));
		OpenGLCall(glDeleteShader(tessControl));
	}

	if (!shaders[4].empty())
	{
		OpenGLCall(glDetachShader(handle, tessEval));
		OpenGLCall(glDeleteShader(tessEval));
	}

	if (!shaders[5].empty())
	{
		OpenGLCall(glDetachShader(handle, compute));
		OpenGLCall(glDeleteShader(compute));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<std::string, 6> TRAP::Graphics::API::OpenGLShader::ConvertSPIRVToGLSL(std::array<std::vector<uint32_t>, 6>& SPIRVShaders)
{
	std::array<std::string, 6> GLSLShaders{};
	
	for(uint32_t i = 0; i < SPIRVShaders.size(); i++)
	{
		if(!SPIRVShaders[i].empty())
		{
			spirv_cross::CompilerGLSL glsl(SPIRVShaders[i]);

			glsl.set_common_options({
				                        460,
				                        false,
				                        false,
				                        false,
				                        false,
				                        false,
				                        true,
				                        false,
				                        false,
				                        false,
				                        {false, false, true},
				                        {spirv_cross::CompilerGLSL::Options::Precision::Mediump, spirv_cross::CompilerGLSL::Options::Precision::Highp}
			});

			GLSLShaders[i] = glsl.compile();
		}
	}

	return GLSLShaders;
}

//-------------------------------------------------------------------------------------------------------------------//

GLenum TRAP::Graphics::API::OpenGLShader::ShaderTypeToOpenGL(const ShaderType type)
{
	switch (type)
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
					TP_ERROR("[Shader][OpenGL][GLSL] Non-Opaque Uniform: \"", uniformName.get(), "\" found! This is unsupported because of SPIR-V support!");
					error = true;
				}
			}
		}

		if (error)
		{
			TP_WARN("[Shader][OpenGL][GLSL] Shader: \"", m_name, "\" using fallback Shader: \"Fallback\"");
			Unbind();
			Shutdown();
			m_handle = 0;
			return;
		}
	}

	Unbind();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::PreProcess(const std::string& source, std::array<std::string, 6>& shaders)
{
	ShaderType type = ShaderType::Unknown;

	std::vector<std::string> lines = Utils::String::GetLines(source);

	//Get Shader Type
	for (uint32_t i = 0; i < lines.size(); i++)
	{
		if (Utils::String::StartsWith(Utils::String::ToLower(lines[i]), "#shader"))
		{
			if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "vertex"))
			{
				type = ShaderType::Vertex;
			}
			else if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "fragment"))
			{
				type = ShaderType::Fragment;
			}
			else if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "geometry"))
			{
				type = ShaderType::Geometry;
			}
			else if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "tessellation"))
			{
				if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "control"))
				{
					type = ShaderType::Tessellation_Control;
				}
				else if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "evaluation"))
				{
					type = ShaderType::Tessellation_Evaluation;
				}
			}
			else if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "compute"))
			{
				type = ShaderType::Compute;				
			}
		}
		else if(Utils::String::StartsWith(Utils::String::ToLower(lines[i]), "#version"))
		{
			TP_WARN("[Shader][OpenGL][GLSL] Found Tag: \"", lines[i], "\" this is unnecessary! Skipping Line: ", i);
		}
		else if (type != ShaderType::Unknown)
		{
			shaders[static_cast<int32_t>(type) - 1].append(lines[i]);
			shaders[static_cast<int32_t>(type) - 1].append("\n");
		}
	}

	for(uint32_t i = 0; i < shaders.size(); i++)
	{
		if (!shaders[i].empty())
		{
			if (Utils::String::ToLower(shaders[i]).find("main") == std::string::npos)
			{				
				TP_ERROR("[Shader][OpenGL][GLSL] ", ShaderTypeToString(static_cast<ShaderType>(i + 1)), ": Couldn't find \"main\" function!");
				shaders[i] = "";
			}
			else //Found main function
				shaders[i] = "#version 460 core\n" + shaders[i];
		}
	}
}