#include "TRAPPCH.h"
#include "OpenGLShader.h"

#include "Utils/String/String.h"
#include "Graphics/Shaders/ShaderManager.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShader::OpenGLShader(std::string name, const std::string_view source)
	: m_handle(0)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	
	InitGLSL(source);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShader::OpenGLShader(std::string name, const std::vector<uint32_t>& source)
	: m_handle(0)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	
	InitSPIRV(source);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShader::OpenGLShader(std::string name,
                                                const std::string_view VSSource,
                                                const std::string_view FSSource,
                                                const std::string_view GSSource,
                                                const std::string_view TCSSource,
                                                const std::string_view TESSource,
                                                const std::string_view CSSource)
	: m_handle(0)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);

	const std::array<std::string_view, 6> shaderSources{ VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource };
	
	InitGLSL(shaderSources);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::OpenGLShader::~OpenGLShader()
{
	TP_PROFILE_FUNCTION();
	
	TP_DEBUG(Log::ShaderOpenGLPrefix, "Destroying Shader: \"", m_name, "\"");
	Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::Shutdown() const
{
	TP_PROFILE_FUNCTION();

	if (s_CurrentlyBound == this)
		s_CurrentlyBound->Unbind();
	
	if (m_handle)
		glDeleteProgram(m_handle);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::Bind() const
{
	TP_PROFILE_FUNCTION();
	
	if (s_CurrentlyBound != this)
	{
		if (m_handle)
		{
			glUseProgram(m_handle);
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
	
	glUseProgram(0);
	s_CurrentlyBound = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLShader::GetHandle() const
{
	TP_PROFILE_FUNCTION();

	return m_handle;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::InitGLSL(const std::string_view source)
{
	TP_DEBUG(Log::ShaderOpenGLGLSLPrefix, "Compiling: \"", m_name, "\"");
	
	std::array<std::string, 6> shaders{};
	if (!source.empty())
	{
		if(!PreProcess(source, shaders))
		{
			TP_WARN(Log::ShaderOpenGLGLSLPrefix, "Shader: \"", m_name, "\" using fallback Shader: \"Fallback\"");
				return;
		}		
	}
	
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
		TP_WARN(Log::ShaderOpenGLGLSLPrefix, "Shader: \"", m_name, "\" using fallback Shader: \"Fallback\"");
		return;
	}
	
	m_handle = CompileGLSL({shaders[0],shaders[1], shaders[2], shaders[3], shaders[4], shaders[5]});
	if (!m_handle)
	{
		TP_WARN(Log::ShaderOpenGLGLSLPrefix, "Shader: \"", m_name, "\" using fallback Shader: \"Fallback\"");
		return;
	}
	CheckForUniforms();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::InitGLSL(const std::array<std::string_view, 6>& shaderSources)
{
	TP_DEBUG(Log::ShaderOpenGLGLSLPrefix, "Compiling: \"", m_name, "\"");

	bool isEmpty = true;
	for (const auto& shaderSource : shaderSources)
	{
		if (!shaderSource.empty())
		{
			isEmpty = false;
			break;
		}
	}
	if (isEmpty)
	{
		TP_WARN(Log::ShaderOpenGLGLSLPrefix, "Shader: \"", m_name, "\" using fallback Shader: \"Fallback\"");
		return;
	}

	m_handle = CompileGLSL(shaderSources);
	if (!m_handle)
	{
		TP_WARN(Log::ShaderOpenGLGLSLPrefix, "Shader: \"", m_name, "\" using fallback Shader: \"Fallback\"");
		return;
	}
	CheckForUniforms();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::InitSPIRV(const std::vector<uint32_t>& source)
{
	std::array<std::vector<uint32_t>, 6> SPIRVShaders{};
	uint32_t index = 0;
	const uint32_t SPIRVSubShaderCount = source[index++];	

	for(uint32_t i = 0; i < SPIRVSubShaderCount; i++)
	{
		const uint32_t SPIRVSize = source[index++];
		ShaderType shaderType = static_cast<ShaderType>(source[index++]);

		const std::vector<uint32_t> tempSPIRV(source.begin() + index, source.begin() + index + SPIRVSize);
		index += SPIRVSize;

		if (shaderType >= ShaderType::Vertex &&
		    shaderType <= ShaderType::Tessellation_Evaluation)
			SPIRVShaders[static_cast<uint32_t>(shaderType) - 1] = tempSPIRV;
	}

	TP_DEBUG(Log::ShaderOpenGLSPIRVPrefix, "Converting: \"", m_name, "\" to GLSL");
	const std::array<std::string, 6> GLSLShaders = ConvertSPIRVToGLSL(SPIRVShaders);
	TP_DEBUG(Log::ShaderOpenGLGLSLPrefix, "Compiling: \"", m_name, "\"");
	m_handle = CompileGLSL({GLSLShaders[0], GLSLShaders[1], GLSLShaders[2], GLSLShaders[3], GLSLShaders[4], GLSLShaders[5]});
	if (!m_handle)
	{
		TP_WARN(Log::ShaderOpenGLGLSLPrefix, "Shader: \"", m_name, "\" using fallback Shader: \"Fallback\"");
		return;
	}
	CheckForUniforms();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::OpenGLShader::CompileGLSL(const std::array<std::string_view, 6>& shaders)
{
	uint32_t program = 0;
	std::array<uint32_t, 6> shaderHandles{0, 0, 0, 0, 0, 0};
	int32_t linkResult = 0, validateResult = 0;
	
	if (!CreateGLSLProgram(shaders, shaderHandles, program))
		return 0;

	LinkGLSLProgram(linkResult, validateResult, program);

	DeleteGLSLShaders(shaders, shaderHandles, program);
	
	if (!linkResult || !validateResult)
	{
		glDeleteProgram(program);
		return 0;
	}
	
	return program;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLShader::CompileGLSLShader(const ShaderType type, const char* source, uint32_t& handle)
{
	handle = glCreateShader(ShaderTypeToOpenGL(type));

	TP_DEBUG(Log::ShaderOpenGLGLSLPrefix, "Compiling ", ShaderTypeToString(type));
	glShaderSource(handle, 1, &source, nullptr);
	glCompileShader(handle);

	int32_t result;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int32_t length;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> error(length);
		glGetShaderInfoLog(handle, length, &length, error.data());
		const std::string_view errorMessage(error.data(), length - 1);
		TP_ERROR("[Shader][OpenGL][GLSL][", ShaderTypeToString(type), "] Failed to compile Shader!");
		TP_ERROR("[Shader][OpenGL][GLSL][", ShaderTypeToString(type), "] ", errorMessage);
		glDeleteShader(handle);

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLShader::CreateGLSLProgram(const std::array<std::string_view, 6>& shaders,
														  std::array<uint32_t, 6>& shaderHandles,
														  uint32_t& handle)
{
	handle = glCreateProgram();

	for(uint32_t i = 0; i < shaders.size(); i++)
	{
		if(!shaders[i].empty())
		{
			if (!CompileGLSLShader(static_cast<ShaderType>(i + 1), shaders[i].data(), shaderHandles[i]))
				return false;

			glAttachShader(handle, shaderHandles[i]);
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::LinkGLSLProgram(int32_t& linkResult, int32_t& validateResult, const uint32_t& handle)
{
	glLinkProgram(handle);
	glGetProgramiv(handle, GL_LINK_STATUS, &linkResult);
	if (linkResult == GL_FALSE)
	{
		int32_t length;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> error(length);
		glGetProgramInfoLog(handle, length, &length, error.data());
		const std::string_view errorMessage(error.data(), length - 1);
		TP_ERROR(Log::ShaderOpenGLGLSLProgramPrefix, "Failed to link Program!");
		TP_ERROR(Log::ShaderOpenGLGLSLProgramPrefix, errorMessage);
	}

	if (linkResult == GL_TRUE)
	{
		glValidateProgram(handle);
		glGetProgramiv(handle, GL_VALIDATE_STATUS, &validateResult);
		if (validateResult == GL_FALSE)
			TP_ERROR(Log::ShaderOpenGLGLSLProgramPrefix, "Failed to validate Program!");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::DeleteGLSLShaders(const std::array<std::string_view, 6>& shaders,
														  std::array<uint32_t, 6>& shaderHandles,
														  const uint32_t& handle)
{
	for(uint32_t i = 0; i < shaders.size(); i++)
	{
		if(!shaders[i].empty())
		{
			glDetachShader(handle, shaderHandles[i]);
			glDeleteShader(shaderHandles[i]);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<std::string, 6> TRAP::Graphics::API::OpenGLShader::ConvertSPIRVToGLSL(const std::array<std::vector<uint32_t>, 6>& SPIRVShaders)
{
	std::array<std::string, 6> GLSLShaders{};
	
	for(uint32_t i = 0; i < SPIRVShaders.size(); i++)
	{
		if(!SPIRVShaders[i].empty())
		{
			spirv_cross::CompilerGLSL glsl(SPIRVShaders[i]);

			spirv_cross::CompilerGLSL::Options options;
			options.version = 460;
			 
			glsl.set_common_options(options);

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

	case ShaderType::Unknown:
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
	case GL_SAMPLER_2D:
	case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_2D_RECT:
	case GL_SAMPLER_1D_ARRAY:
	case GL_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_CUBE_MAP_ARRAY:
	case GL_SAMPLER_BUFFER:
	case GL_SAMPLER_2D_MULTISAMPLE:
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_SAMPLER_1D_SHADOW:
	case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_CUBE_SHADOW:
	case GL_SAMPLER_2D_RECT_SHADOW:
	case GL_SAMPLER_1D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_ARRAY_SHADOW:
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
	glGetProgramiv(m_handle, GL_ACTIVE_UNIFORMS, reinterpret_cast<int32_t*>(&uniformCount));

	if (uniformCount != 0)
	{
		bool error = false;

		uint32_t maxNameLength = 0;
		uint32_t length = 0;
		uint32_t count = 0;
		GLenum type = GL_NONE;
		glGetProgramiv(m_handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, reinterpret_cast<int32_t*>(&maxNameLength));
		const Scope<char[]> uniformName = MakeScope<char[]>(maxNameLength);

		for (uint32_t i = 0; i < uniformCount; ++i)
		{
			glGetActiveUniform(m_handle, i, static_cast<int32_t>(maxNameLength), reinterpret_cast<int32_t*>(&length), reinterpret_cast<int32_t*>(&count), &type, uniformName.get());

			if (Utils::String::GetCount(std::string_view(uniformName.get()), '.') == 0) //Ignore if UBO
			{
				if (!IsTypeOpaque(type)) //Check if is Non-Opaque type
				{
					TP_ERROR(Log::ShaderOpenGLGLSLPrefix, "Non-Opaque Uniform: \"", uniformName.get(), "\" found! This is unsupported because of SPIR-V support!");
					error = true;
				}
			}
		}

		if (error)
		{
			TP_WARN(Log::ShaderOpenGLGLSLPrefix, "Shader: \"", m_name, "\" using fallback Shader: \"Fallback\"");
			Unbind();
			Shutdown();
			m_handle = 0;
			return;
		}
	}

	Unbind();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLShader::PreProcess(const std::string_view source, std::array<std::string, 6>& shaders)
{
	ShaderType type = ShaderType::Unknown;

	std::vector<std::string> lines = Utils::String::GetLines(source);

	//Get Shader Type
	for (uint32_t i = 0; i < lines.size(); i++)
	{
		if (Utils::String::StartsWith(Utils::String::ToLower(lines[i]), "#shader"))
		{
			if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "vertex"))
				type = ShaderType::Vertex;
			else if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "fragment"))
				type = ShaderType::Fragment;
			else if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "geometry"))
				type = ShaderType::Geometry;
			else if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "tessellation"))
			{
				if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "control"))
					type = ShaderType::Tessellation_Control;
				else if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "evaluation"))
					type = ShaderType::Tessellation_Evaluation;
			}
			else if (Utils::String::FindToken(Utils::String::ToLower(lines[i]), "compute"))
				type = ShaderType::Compute;				
		}
		else if(Utils::String::StartsWith(Utils::String::ToLower(lines[i]), "#version"))
			TP_WARN(Log::ShaderOpenGLGLSLPrefix, "Found Tag: \"", lines[i], "\" this is unnecessary! Skipping Line: ", i);
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
				TP_ERROR("[Shader][OpenGL][GLSL][", ShaderTypeToString(static_cast<ShaderType>(i + 1)), "] Couldn't find \"main\" function!");
				shaders[i] = "";
				return false;
			}
			
			//Found main function
			shaders[i] = "#version 460 core\n" + shaders[i];
		}
	}

	return true;
}