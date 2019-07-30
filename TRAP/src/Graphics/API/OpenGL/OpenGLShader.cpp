#include "TRAPPCH.h"
#include "OpenGLShader.h"

#include "OpenGLCommon.h"
#include "Utils/String.h"
#include "OpenGLShaderResource.h"

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
	Parse(*shaders[0], *shaders[1], *shaders[2], *shaders[3], *shaders[4], *shaders[5]);
	TP_DEBUG("[Shader][OpenGL] Compiling: \"", m_name, "\"");
	m_handle = Compile(shaders);
	if (!m_handle)
	{
		TP_WARN("[Shader][OpenGL] Shader: \"", m_name, "\" using fallback Shader: \"Passthrough\"");

		return;
	}
	ResolveUniforms();
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
		if (m_handle != 0)
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

void TRAP::Graphics::API::OpenGLShader::Parse(const std::string& VSSource, const std::string& FSSource, const std::string& GSSource, const std::string& TCSSource, const std::string& TESSource, const std::string& CSSource)
{
	m_VSUniformBuffers.push_back(std::make_unique<OpenGLShaderUniformBufferDeclaration>("Global", ShaderType::VERTEX));
	m_FSUniformBuffers.push_back(std::make_unique<OpenGLShaderUniformBufferDeclaration>("Global", ShaderType::FRAGMENT));
	m_GSUniformBuffers.push_back(std::make_unique<OpenGLShaderUniformBufferDeclaration>("Global", ShaderType::GEOMETRY));
	m_TCSUniformBuffers.push_back(std::make_unique<OpenGLShaderUniformBufferDeclaration>("Global", ShaderType::TESSELLATIONCONTROL));
	m_TESUniformBuffers.push_back(std::make_unique<OpenGLShaderUniformBufferDeclaration>("Global", ShaderType::TESSELLATIONEVALUATION));
	m_CSUniformBuffers.push_back(std::make_unique<OpenGLShaderUniformBufferDeclaration>("Global", ShaderType::COMPUTE));

	const char* token;
	const char* vstr;
	const char* fstr;
	const char* gstr;
	const char* tcstr;
	const char* testr;
	const char* cstr;

	//Vertex Shader
	vstr = VSSource.c_str();
	while ((token = Utils::String::FindToken(vstr, "struct")))
		ParseUniformStruct(Utils::String::GetBlock(token, &vstr));

	vstr = VSSource.c_str();
	while ((token = Utils::String::FindToken(vstr, "uniform")))
		ParseUniform(Utils::String::GetStatement(token, &vstr), ShaderType::VERTEX);

	//Fragment Shader
	fstr = FSSource.c_str();
	while ((token = Utils::String::FindToken(fstr, "struct")))
		ParseUniformStruct(Utils::String::GetBlock(token, &fstr));

	fstr = FSSource.c_str();
	while ((token = Utils::String::FindToken(fstr, "uniform")))
		ParseUniform(Utils::String::GetStatement(token, &fstr), ShaderType::FRAGMENT);

	//Geometry Shader
	gstr = GSSource.c_str();
	while ((token = Utils::String::FindToken(gstr, "struct")))
		ParseUniformStruct(Utils::String::GetBlock(token, &gstr));

	gstr = GSSource.c_str();
	while ((token = Utils::String::FindToken(gstr, "uniform")))
		ParseUniform(Utils::String::GetStatement(token, &gstr), ShaderType::GEOMETRY);

	//Tessellation Control Shader
	tcstr = TCSSource.c_str();
	while ((token = Utils::String::FindToken(tcstr, "struct")))
		ParseUniformStruct(Utils::String::GetBlock(token, &tcstr));

	tcstr = TCSSource.c_str();
	while ((token = Utils::String::FindToken(tcstr, "uniform")))
		ParseUniform(Utils::String::GetStatement(token, &tcstr), ShaderType::TESSELLATIONCONTROL);

	//Tessellation Evaluation Shader
	testr = TESSource.c_str();
	while ((token = Utils::String::FindToken(testr, "struct")))
		ParseUniformStruct(Utils::String::GetBlock(token, &testr));

	testr = TESSource.c_str();
	while ((token = Utils::String::FindToken(testr, "uniform")))
		ParseUniform(Utils::String::GetStatement(token, &testr), ShaderType::TESSELLATIONEVALUATION);

	//Compute Shader
	cstr = CSSource.c_str();
	while ((token = Utils::String::FindToken(cstr, "struct")))
		ParseUniformStruct(Utils::String::GetBlock(token, &cstr));

	cstr = CSSource.c_str();
	while ((token = Utils::String::FindToken(cstr, "uniform")))
		ParseUniform(Utils::String::GetStatement(token, &cstr), ShaderType::COMPUTE);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::ParseUniform(const std::string& statement, const ShaderType shaderType)
{
	//Check if Uniform Buffer Block
	if(Utils::String::FindToken(statement, "{"))
		//Ignore Uniform Buffer Block - They get handled by UniformBufferObject
		return;

	std::vector<std::string> tokens = Utils::String::Tokenize(statement);
	unsigned int index = 0;

	index++; //"uniform"
	const std::string typeString = tokens[index++];
	std::string name = tokens[index];
	//Strip; from name if present
	if (const char* s = strstr(name.c_str(), ";"))
		name = std::string(name.c_str(), s - name.c_str());

	const std::string n(name);
	int32_t count = 1;
	const char* namestr = n.c_str();
	if (const char* s = strstr(namestr, "["))
	{
		name = std::string(namestr, s - namestr);

		const char* end = strstr(namestr, "]");
		const std::string c(s + 1, end - s);
		count = std::stoi(c);
	}

	if (IsTypeStringResource(typeString))
	{
		std::unique_ptr<ShaderResourceDeclaration> declaration = std::make_unique<OpenGLShaderResourceDeclaration>(OpenGLShaderResourceDeclaration::StringToType(typeString), name, count);
		m_resources.push_back(std::move(declaration));
	}
	else
	{
		const OpenGLShaderUniformDeclaration::Type type = OpenGLShaderUniformDeclaration::StringToType(typeString);
		std::unique_ptr<OpenGLShaderUniformDeclaration> declaration;

		if (type == OpenGLShaderUniformDeclaration::Type::NONE)
		{
			//Find struct
			ShaderStruct* Struct = FindStruct(typeString);
			if(Struct)
				declaration = std::make_unique<OpenGLShaderUniformDeclaration>(std::unique_ptr<ShaderStruct>(Struct), name, count);
		}
		else
			declaration = std::make_unique<OpenGLShaderUniformDeclaration>(type, name, count);

			if (shaderType == ShaderType::VERTEX)
			{
				if (m_VSUniformBuffer == nullptr)
					m_VSUniformBuffer = std::make_unique<OpenGLShaderUniformBufferDeclaration>("", ShaderType::VERTEX);

				m_VSUniformBuffer->PushUniform(declaration);
			}
			else if (shaderType == ShaderType::FRAGMENT)
			{
				if (m_FSUniformBuffer == nullptr)
					m_FSUniformBuffer = std::make_unique<OpenGLShaderUniformBufferDeclaration>("", ShaderType::FRAGMENT);

				m_FSUniformBuffer->PushUniform(declaration);
			}
			else if (shaderType == ShaderType::GEOMETRY)
			{
				if (m_GSUniformBuffer == nullptr)
					m_GSUniformBuffer = std::make_unique<OpenGLShaderUniformBufferDeclaration>("", ShaderType::GEOMETRY);

				m_GSUniformBuffer->PushUniform(declaration);
			}
			else if (shaderType == ShaderType::TESSELLATIONCONTROL)
			{
				if (m_TCSUniformBuffer == nullptr)
					m_TCSUniformBuffer = std::make_unique<OpenGLShaderUniformBufferDeclaration>("", ShaderType::TESSELLATIONCONTROL);

				m_TCSUniformBuffer->PushUniform(declaration);
			}
			else if (shaderType == ShaderType::TESSELLATIONEVALUATION)
			{
				if (m_TESUniformBuffer == nullptr)
					m_TESUniformBuffer = std::make_unique<OpenGLShaderUniformBufferDeclaration>("", ShaderType::TESSELLATIONEVALUATION);

				m_TESUniformBuffer->PushUniform(declaration);
			}
			else if (shaderType == ShaderType::COMPUTE)
			{
				if (m_CSUniformBuffer == nullptr)
					m_CSUniformBuffer = std::make_unique<OpenGLShaderUniformBufferDeclaration>("", ShaderType::COMPUTE);

				m_CSUniformBuffer->PushUniform(declaration);
			}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::ParseUniformStruct(const std::string& block)
{
	std::vector<std::string> tokens = Utils::String::Tokenize(block);

	unsigned int index = 0;
	index++; //Struct
	const std::string name = tokens[index++];
	std::unique_ptr<ShaderStruct> uniformStruct = std::make_unique<ShaderStruct>(name);
	index++; //{
	while (index < tokens.size())
	{
		if (tokens[index] == "}")
			break;

		std::string type = tokens[index++];
		std::string name1 = tokens[index++];

		//Strip ; from name if present
		if (const char* s = strstr(name1.c_str(), ";"))
			name1 = std::string(name1.c_str(), s - name1.c_str());

		unsigned int count = 1;
		const char* nameStr = name1.c_str();
		if (const char* s = strstr(nameStr, "["))
		{
			name1 = std::string(nameStr, s - nameStr);

			const char* end = strstr(nameStr, "]");
			std::string c(s + 1, end - s);
			count = std::stoi(c);
		}
		std::unique_ptr<ShaderUniformDeclaration> field = std::make_unique<OpenGLShaderUniformDeclaration>(OpenGLShaderUniformDeclaration::StringToType(type), name1, count);
		uniformStruct->AddField(field);
	}
	m_structs.push_back(std::move(uniformStruct));
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLShader::IsTypeStringResource(const std::string& type)
{
	if (type == "sampler2D")
		return true;
	if (type == "samplerCube")
		return true;
	if (type == "sampler2DShadow")
		return true;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::ResolveUniforms()
{
	Bind();

	for (const auto& VSBuffer : m_VSUniformBuffers)
	{
		auto* VSDecl = dynamic_cast<OpenGLShaderUniformBufferDeclaration*>(VSBuffer.get());
		const ShaderUniformList& VSUniforms = VSDecl->GetUniformDeclarations();
		for (const auto& VSUniform : VSUniforms)
		{
			auto* convertedVSUniform = dynamic_cast<OpenGLShaderUniformDeclaration*>(VSUniform.get());
			if (convertedVSUniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
			{
				const ShaderStruct& VSStruct = convertedVSUniform->GetShaderUniformStruct();
				const auto& VSFields = VSStruct.GetFields();
				for (const auto& VSField : VSFields)
				{
					auto* convertedVSField = dynamic_cast<OpenGLShaderUniformDeclaration*>(VSField.get());
					convertedVSField->m_location = GetUniformLocation(convertedVSUniform->m_name + "." + convertedVSField->m_name);
				}
			}
			else
			{
				TP_WARN("[Shader][OpenGL][VertexShader] Shader: ", this->m_name, " Uniform: ", convertedVSUniform->m_name, " Non-Opaque Uniforms outside a block are incompatible with Vulkan!");
				convertedVSUniform->m_location = GetUniformLocation(convertedVSUniform->m_name);
			}
		}
	}

	for (const auto& FSBuffer : m_FSUniformBuffers)
	{
		auto* FSDecl = dynamic_cast<OpenGLShaderUniformBufferDeclaration*>(FSBuffer.get());
		const ShaderUniformList& FSUniforms = FSDecl->GetUniformDeclarations();
		for (const auto& FSUniform : FSUniforms)
		{
			auto* convertedFSUniform = dynamic_cast<OpenGLShaderUniformDeclaration*>(FSUniform.get());
			if (convertedFSUniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
			{
				const ShaderStruct& FSStruct = convertedFSUniform->GetShaderUniformStruct();
				const auto& FSFields = FSStruct.GetFields();
				for (const auto& FSField : FSFields)
				{
					auto* convertedFSField = dynamic_cast<OpenGLShaderUniformDeclaration*>(FSField.get());
					convertedFSField->m_location = GetUniformLocation(convertedFSUniform->m_name + "." + convertedFSField->m_name);
				}
			}
			else
			{
				TP_WARN("[Shader][OpenGL][FragmentShader] Shader: ", this->m_name, " Uniform: ", convertedFSUniform->m_name, " Non-Opaque Uniforms outside a block are incompatible with Vulkan!");
				convertedFSUniform->m_location = GetUniformLocation(convertedFSUniform->m_name);
			}
		}
	}

	for (const auto& GSBuffer : m_GSUniformBuffers)
	{
		auto* GSDecl = dynamic_cast<OpenGLShaderUniformBufferDeclaration*>(GSBuffer.get());
		const ShaderUniformList& GSUniforms = GSDecl->GetUniformDeclarations();
		for (const auto& GSUniform : GSUniforms)
		{
			auto* convertedGSUniform = dynamic_cast<OpenGLShaderUniformDeclaration*>(GSUniform.get());
			if (convertedGSUniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
			{
				const ShaderStruct& GSStruct = convertedGSUniform->GetShaderUniformStruct();
				const auto& GSFields = GSStruct.GetFields();
				for (const auto& GSField : GSFields)
				{
					auto* convertedGSField = dynamic_cast<OpenGLShaderUniformDeclaration*>(GSField.get());
					convertedGSField->m_location = GetUniformLocation(convertedGSUniform->m_name + "." + convertedGSField->m_name);
				}
			}
			else
			{
				TP_WARN("[Shader][OpenGL][GeometryShader] Shader: ", this->m_name, " Uniform: ", convertedGSUniform->m_name, " Non-Opaque Uniforms outside a block are incompatible with Vulkan!");
				convertedGSUniform->m_location = GetUniformLocation(convertedGSUniform->m_name);
			}
		}
	}

	for (const auto& TCSBuffer : m_TCSUniformBuffers)
	{
		auto* TCSDecl = dynamic_cast<OpenGLShaderUniformBufferDeclaration*>(TCSBuffer.get());
		const ShaderUniformList& TCSUniforms = TCSDecl->GetUniformDeclarations();
		for (const auto& TCSUniform : TCSUniforms)
		{
			auto* convertedTCSUniform = dynamic_cast<OpenGLShaderUniformDeclaration*>(TCSUniform.get());
			if (convertedTCSUniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
			{
				const ShaderStruct& TCSStruct = convertedTCSUniform->GetShaderUniformStruct();
				const auto& TCSFields = TCSStruct.GetFields();
				for (const auto& TCSField : TCSFields)
				{
					auto* convertedFSField = dynamic_cast<OpenGLShaderUniformDeclaration*>(TCSField.get());
					convertedFSField->m_location = GetUniformLocation(convertedTCSUniform->m_name + "." + convertedFSField->m_name);
				}
			}
			else
			{
				TP_WARN("[Shader][OpenGL][TessellationControlShader] Shader: ", " Uniform: ", convertedTCSUniform->m_name, this->m_name, " Non-Opaque Uniforms outside a block are incompatible with Vulkan!");
				convertedTCSUniform->m_location = GetUniformLocation(convertedTCSUniform->m_name);
			}
		}
	}

	for (const auto& TESBuffer : m_TESUniformBuffers)
	{
		auto* TESDecl = dynamic_cast<OpenGLShaderUniformBufferDeclaration*>(TESBuffer.get());
		const ShaderUniformList& TESUniforms = TESDecl->GetUniformDeclarations();
		for (const auto& TESUniform : TESUniforms)
		{
			auto* convertedTESUniform = dynamic_cast<OpenGLShaderUniformDeclaration*>(TESUniform.get());
			if (convertedTESUniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
			{
				const ShaderStruct& TESStruct = convertedTESUniform->GetShaderUniformStruct();
				const auto& TESFields = TESStruct.GetFields();
				for (const auto& TESField : TESFields)
				{
					auto* convertedFSField = dynamic_cast<OpenGLShaderUniformDeclaration*>(TESField.get());
					convertedFSField->m_location = GetUniformLocation(convertedTESUniform->m_name + "." + convertedFSField->m_name);
				}
			}
			else
			{
				TP_WARN("[Shader][OpenGL][TessellationEvaluationShader] Shader: ", " Uniform: ", convertedTESUniform->m_name, this->m_name, " Non-Opaque Uniforms outside a block are incompatible with Vulkan!");
				convertedTESUniform->m_location = GetUniformLocation(convertedTESUniform->m_name);
			}
		}
	}

	for (const auto& CSBuffer : m_CSUniformBuffers)
	{
		auto* CSDecl = dynamic_cast<OpenGLShaderUniformBufferDeclaration*>(CSBuffer.get());
		const ShaderUniformList& CSUniforms = CSDecl->GetUniformDeclarations();
		for (const auto& CSUniform : CSUniforms)
		{
			auto* convertedCSUniform = dynamic_cast<OpenGLShaderUniformDeclaration*>(CSUniform.get());
			if (convertedCSUniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
			{
				const ShaderStruct& CSStruct = convertedCSUniform->GetShaderUniformStruct();
				const auto& CSFields = CSStruct.GetFields();
				for (const auto& CSField : CSFields)
				{
					auto* convertedFSField = dynamic_cast<OpenGLShaderUniformDeclaration*>(CSField.get());
					convertedFSField->m_location = GetUniformLocation(convertedCSUniform->m_name + "." + convertedFSField->m_name);
				}
			}
			else
			{
				TP_WARN("[Shader][OpenGL][ComputeShader] Shader: ", this->m_name, " Uniform: ", convertedCSUniform->m_name, " Non-Opaque Uniforms outside a block are incompatible with Vulkan!");
				convertedCSUniform->m_location = GetUniformLocation(convertedCSUniform->m_name);
			}
		}
	}

	{
		OpenGLShaderUniformBufferDeclaration* VSUserDecl = m_VSUniformBuffer.get();
		if (VSUserDecl)
		{
			const ShaderUniformList& VSUserUniforms = VSUserDecl->GetUniformDeclarations();
			for (const auto& VSUserUniform : VSUserUniforms)
			{
				auto* convertedVSUserUniform = dynamic_cast<OpenGLShaderUniformDeclaration*>(VSUserUniform.get());
				if (convertedVSUserUniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
				{
					const ShaderStruct& VSUserStruct = convertedVSUserUniform->GetShaderUniformStruct();
					const auto& VSUserFields = VSUserStruct.GetFields();
					for (const auto& VSUserField : VSUserFields)
					{
						auto* convertedVSUserField = dynamic_cast<OpenGLShaderUniformDeclaration*>(VSUserField.get());
						convertedVSUserField->m_location = GetUniformLocation(convertedVSUserUniform->m_name + "." + convertedVSUserField->m_name);
					}
				}
				else
				{
					TP_WARN("[Shader][OpenGL][VertexShader] Shader: ", this->m_name, " Uniform: ", convertedVSUserUniform->m_name, " Non-Opaque Uniforms outside a block are incompatible with Vulkan!");
					convertedVSUserUniform->m_location = GetUniformLocation(convertedVSUserUniform->m_name);
				}
			}
		}
	}

	{
		OpenGLShaderUniformBufferDeclaration* FSUserDecl = m_FSUniformBuffer.get();
		if (FSUserDecl)
		{
			const ShaderUniformList& FSUserUniforms = FSUserDecl->GetUniformDeclarations();
			for (const auto& FSUserUniform : FSUserUniforms)
			{
				auto* convertedFSUserUniform = dynamic_cast<OpenGLShaderUniformDeclaration*>(FSUserUniform.get());
				if (convertedFSUserUniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
				{
					const ShaderStruct& FSUserStruct = convertedFSUserUniform->GetShaderUniformStruct();
					const auto& FSUserFields = FSUserStruct.GetFields();
					for (const auto& FSUserField : FSUserFields)
					{
						auto* convertedFSUserField = dynamic_cast<OpenGLShaderUniformDeclaration*>(FSUserField.get());
						convertedFSUserField->m_location = GetUniformLocation(convertedFSUserUniform->m_name + "." + convertedFSUserField->m_name);
					}
				}
				else
				{
					TP_WARN("[Shader][OpenGL][FragmentShader] Shader: ", this->m_name, " Uniform: ", convertedFSUserUniform->m_name, " Non-Opaque Uniforms outside a block are incompatible with Vulkan!");
					convertedFSUserUniform->m_location = GetUniformLocation(convertedFSUserUniform->m_name);
				}
			}
		}
	}

	{
		OpenGLShaderUniformBufferDeclaration* GSUserDecl = m_GSUniformBuffer.get();
		if (GSUserDecl)
		{
			const ShaderUniformList& GSUserUniforms = GSUserDecl->GetUniformDeclarations();
			for (const auto& GSUserUniform : GSUserUniforms)
			{
				auto* convertedGSUserUniform = dynamic_cast<OpenGLShaderUniformDeclaration*>(GSUserUniform.get());
				if (convertedGSUserUniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
				{
					const ShaderStruct& GSUserStruct = convertedGSUserUniform->GetShaderUniformStruct();
					const auto& GSUserFields = GSUserStruct.GetFields();
					for (const auto& GSUserField : GSUserFields)
					{
						auto* convertedGSUserField = dynamic_cast<OpenGLShaderUniformDeclaration*>(GSUserField.get());
						convertedGSUserField->m_location = GetUniformLocation(convertedGSUserUniform->m_name + "." + convertedGSUserField->m_name);
					}
				}
				else
				{
					TP_WARN("[Shader][OpenGL][GeometryShader] Shader: ", this->m_name, " Uniform: ", convertedGSUserUniform->m_name, " Non-Opaque Uniforms outside a block are incompatible with Vulkan!");
					convertedGSUserUniform->m_location = GetUniformLocation(convertedGSUserUniform->m_name);
				}
			}
		}
	}

	{
		OpenGLShaderUniformBufferDeclaration* TCSUserDecl = m_TCSUniformBuffer.get();
		if (TCSUserDecl)
		{
			const ShaderUniformList& TCSUserUniforms = TCSUserDecl->GetUniformDeclarations();
			for (const auto& TCSUserUniform : TCSUserUniforms)
			{
				auto* convertedTCSUserUniform = dynamic_cast<OpenGLShaderUniformDeclaration*>(TCSUserUniform.get());
				if (convertedTCSUserUniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
				{
					const ShaderStruct& TCSUserStruct = convertedTCSUserUniform->GetShaderUniformStruct();
					const auto& TCSUserFields = TCSUserStruct.GetFields();
					for (const auto& TCSUserField : TCSUserFields)
					{
						auto* convertedTCSUserField = dynamic_cast<OpenGLShaderUniformDeclaration*>(TCSUserField.get());
						convertedTCSUserField->m_location = GetUniformLocation(convertedTCSUserUniform->m_name + "." + convertedTCSUserField->m_name);
					}
				}
				else
				{
					TP_WARN("[Shader][OpenGL][TessellationControlShader] Shader: ", this->m_name, " Uniform: ", convertedTCSUserUniform->m_name, " Non-Opaque Uniforms outside a block are incompatible with Vulkan!");
					convertedTCSUserUniform->m_location = GetUniformLocation(convertedTCSUserUniform->m_name);
				}
			}
		}
	}

	{
		OpenGLShaderUniformBufferDeclaration* TESUserDecl = m_TESUniformBuffer.get();
		if (TESUserDecl)
		{
			const ShaderUniformList& TESUserUniforms = TESUserDecl->GetUniformDeclarations();
			for (const auto& TESUserUniform : TESUserUniforms)
			{
				auto* convertedTESUserUniform = dynamic_cast<OpenGLShaderUniformDeclaration*>(TESUserUniform.get());
				if (convertedTESUserUniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
				{
					const ShaderStruct& TESUserStruct = convertedTESUserUniform->GetShaderUniformStruct();
					const auto& TESUserFields = TESUserStruct.GetFields();
					for (const auto& TESUserField : TESUserFields)
					{
						auto* convertedFSUserField = dynamic_cast<OpenGLShaderUniformDeclaration*>(TESUserField.get());
						convertedFSUserField->m_location = GetUniformLocation(convertedTESUserUniform->m_name + "." + convertedFSUserField->m_name);
					}
				}
				else
				{
					TP_WARN("[Shader][OpenGL][TessellationEvaluationShader] Shader: ", this->m_name, " Uniform: ", convertedTESUserUniform->m_name, " Non-Opaque Uniforms outside a block are incompatible with Vulkan!");
					convertedTESUserUniform->m_location = GetUniformLocation(convertedTESUserUniform->m_name);
				}
			}
		}
	}

	{
		OpenGLShaderUniformBufferDeclaration* CSUserDecl = m_CSUniformBuffer.get();
		if (CSUserDecl)
		{
			const ShaderUniformList& CSUserUniforms = CSUserDecl->GetUniformDeclarations();
			for (const auto& CSUserUniform : CSUserUniforms)
			{
				auto* convertedCSUserUniform = dynamic_cast<OpenGLShaderUniformDeclaration*>(CSUserUniform.get());
				if (convertedCSUserUniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
				{
					const ShaderStruct& CSUserStruct = convertedCSUserUniform->GetShaderUniformStruct();
					const auto& CSUserFields = CSUserStruct.GetFields();
					for (const auto& CSUserField : CSUserFields)
					{
						auto* convertedCSUserField = dynamic_cast<OpenGLShaderUniformDeclaration*>(CSUserField.get());
						convertedCSUserField->m_location = GetUniformLocation(convertedCSUserUniform->m_name + "." + convertedCSUserField->m_name);
					}
				}
				else
				{
					TP_WARN("[Shader][OpenGL][ComputeShader] Shader: ", this->m_name, " Uniform: ", convertedCSUserUniform->m_name, " Non-Opaque Uniforms outside a block are incompatible with Vulkan!");
					convertedCSUserUniform->m_location = GetUniformLocation(convertedCSUserUniform->m_name);
				}
			}
		}
	}

	unsigned int sampler = 0;
	for (const auto& resource : m_resources)
	{
		auto* convertedResource = dynamic_cast<OpenGLShaderResourceDeclaration*>(resource.get());
		const unsigned int location = GetUniformLocation(convertedResource->m_name);
		if (resource->GetCount() == 1)
		{
			convertedResource->m_register = sampler;
			SetUniform1i(location, sampler++);
		}
		else if (resource->GetCount() > 1)
		{
			convertedResource->m_register = 0;
			const unsigned int count = resource->GetCount();
			std::vector<int32_t> samplers;
			for (unsigned int s = 0; s < count; s++)
				samplers.push_back(s);
			SetUniform1iv(resource->GetName(), samplers.data(), count);
		}
	}
	Unbind();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::OpenGLShader::IsSystemUniform(ShaderUniformDeclaration* uniform)
{
	return Utils::String::StartsWith(uniform->GetName(), "sys_");
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Graphics::API::OpenGLShader::GetUniformLocation(const std::string& name) const
{
	OpenGLCall(const GLint result = glGetUniformLocation(m_handle, name.c_str()));
	if (result == 255)
		TP_ERROR("[Shader][OpenGL] \"", m_name, "\": Could not find uniform ", name, " in Shader!");

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniformStruct(OpenGLShaderUniformDeclaration* uniform, uint8_t* data, int32_t offset) const
{
	const ShaderStruct& Struct = uniform->GetShaderUniformStruct();
	const auto& fields = Struct.GetFields();
	for (const auto& field : fields)
	{
		auto* convertedField = dynamic_cast<OpenGLShaderUniformDeclaration*>(field.get());
		ResolveAndSetUniformField(*convertedField, data, offset);
		offset += convertedField->m_size;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ShaderUniformDeclaration* TRAP::Graphics::API::OpenGLShader::FindUniformDeclaration(const std::string& name, ShaderUniformBufferDeclaration* buffer)
{
	const ShaderUniformList& uniforms = buffer->GetUniformDeclarations();
	for (const auto& uniform : uniforms)
	{
		if (uniform->GetName() == name)
			return uniform.get();
	}
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ShaderUniformDeclaration* TRAP::Graphics::API::OpenGLShader::FindUniformDeclaration(const std::string& name)
{
	ShaderUniformDeclaration* result;
	for (const auto& VSBuffer : m_VSUniformBuffers)
	{
		result = FindUniformDeclaration(name, VSBuffer.get());
		if (result)
			return result;
	}

	for (const auto& FSBuffer : m_FSUniformBuffers)
	{
		result = FindUniformDeclaration(name, FSBuffer.get());
		if (result)
			return result;
	}

	for (const auto& GSBuffer : m_GSUniformBuffers)
	{
		result = FindUniformDeclaration(name, GSBuffer.get());
		if (result)
			return result;
	}

	for (const auto& TCSBuffer : m_TCSUniformBuffers)
	{
		result = FindUniformDeclaration(name, TCSBuffer.get());
		if (result)
			return result;
	}

	for (const auto& TESBuffer : m_TESUniformBuffers)
	{
		result = FindUniformDeclaration(name, TESBuffer.get());
		if (result)
			return result;
	}

	for (const auto& GSBuffer : m_GSUniformBuffers)
	{
		result = FindUniformDeclaration(name, GSBuffer.get());
		if (result)
			return result;
	}

	result = FindUniformDeclaration(name, m_VSUniformBuffer.get());
	if (result)
		return result;

	result = FindUniformDeclaration(name, m_FSUniformBuffer.get());
	if (result)
		return result;

	result = FindUniformDeclaration(name, m_GSUniformBuffer.get());
	if (result)
		return result;

	result = FindUniformDeclaration(name, m_TCSUniformBuffer.get());
	if (result)
		return result;

	result = FindUniformDeclaration(name, m_TESUniformBuffer.get());
	if (result)
		return result;

	result = FindUniformDeclaration(name, m_CSUniformBuffer.get());
	if (result)
		return result;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetVSUniformBuffer(uint8_t* data, const unsigned int size)
{
	ResolveAndSetUniforms(m_VSUniformBuffer.get(), data);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetFSUniformBuffer(uint8_t* data, const unsigned int size)
{
	ResolveAndSetUniforms(m_FSUniformBuffer.get(), data);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetGSUniformBuffer(uint8_t* data, const unsigned int size)
{
	ResolveAndSetUniforms(m_GSUniformBuffer.get(), data);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetTCSUniformBuffer(uint8_t* data, const unsigned int size)
{
	ResolveAndSetUniforms(m_TCSUniformBuffer.get(), data);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetTESUniformBuffer(uint8_t* data, const unsigned int size)
{
	ResolveAndSetUniforms(m_TESUniformBuffer.get(), data);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetCSUniformBuffer(uint8_t* data, const unsigned int size)
{
	ResolveAndSetUniforms(m_CSUniformBuffer.get(), data);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ShaderStruct* TRAP::Graphics::API::OpenGLShader::FindStruct(const std::string_view name)
{
	for (const auto& s : m_structs)
	{
		if (s->GetName() == name)
			return s.get();
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::ResolveAndSetUniforms(ShaderUniformBufferDeclaration* buffer, uint8_t* data) const
{
	const ShaderUniformList& uniforms = buffer->GetUniformDeclarations();
	for (const auto& i : uniforms)
		ResolveAndSetUniform(dynamic_cast<OpenGLShaderUniformDeclaration*>(i.get()), data);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, uint8_t* data) const
{
	if (uniform->GetLocation() == -1)
		return;

	const unsigned int offset = uniform->GetOffset();
	switch (uniform->GetType())
	{
	case OpenGLShaderUniformDeclaration::Type::FLOAT32:
		SetUniform1f(uniform->GetLocation(), *reinterpret_cast<float*>(&data[offset]));
		break;

	case OpenGLShaderUniformDeclaration::Type::INT32:
		SetUniform1i(uniform->GetLocation(), *reinterpret_cast<int32_t*>(&data[offset]));
		break;

	case OpenGLShaderUniformDeclaration::Type::VEC2:
		SetUniform2f(uniform->GetLocation(), *reinterpret_cast<Maths::Vec2*>(&data[offset]));
		break;

	case OpenGLShaderUniformDeclaration::Type::VEC3:
		SetUniform3f(uniform->GetLocation(), *reinterpret_cast<Maths::Vec3*>(&data[offset]));
		break;

	case OpenGLShaderUniformDeclaration::Type::VEC4:
		SetUniform4f(uniform->GetLocation(), *reinterpret_cast<Maths::Vec4*>(&data[offset]));
		break;

	case OpenGLShaderUniformDeclaration::Type::MAT3:
		SetUniformMat3(uniform->GetLocation(), *reinterpret_cast<Maths::Mat3*>(&data[offset]));
		break;

	case OpenGLShaderUniformDeclaration::Type::MAT4:
		SetUniformMat4(uniform->GetLocation(), *reinterpret_cast<Maths::Mat4*>(&data[offset]));
		break;

	default:
		TP_ASSERT(false, "[Shader][OpenGL] Unknown Type!");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniform(const std::string& name, uint8_t* data)
{
	ShaderUniformDeclaration* uniform = FindUniformDeclaration(name);
	if (!uniform)
	{
		TP_ERROR("[Shader][OpenGL] Cannot find uniform in \"", m_name, "\" Shader with name '", name, "'");
		return;
	}
	ResolveAndSetUniform(dynamic_cast<OpenGLShaderUniformDeclaration*>(uniform), data);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, uint8_t* data, const int32_t offset) const
{
	switch (field.GetType())
	{
	case OpenGLShaderUniformDeclaration::Type::FLOAT32:
		SetUniform1f(field.GetLocation(), *reinterpret_cast<float*>(&data[offset]));
		break;

	case OpenGLShaderUniformDeclaration::Type::INT32:
		SetUniform1i(field.GetLocation(), *reinterpret_cast<int32_t*>(&data[offset]));
		break;

	case OpenGLShaderUniformDeclaration::Type::VEC2:
		SetUniform2f(field.GetLocation(), *reinterpret_cast<Maths::Vec2*>(&data[offset]));
		break;

	case OpenGLShaderUniformDeclaration::Type::VEC3:
		SetUniform3f(field.GetLocation(), *reinterpret_cast<Maths::Vec3*>(&data[offset]));
		break;

	case OpenGLShaderUniformDeclaration::Type::VEC4:
		SetUniform4f(field.GetLocation(), *reinterpret_cast<Maths::Vec4*>(&data[offset]));
		break;

	case OpenGLShaderUniformDeclaration::Type::MAT3:
		SetUniformMat3(field.GetLocation(), *reinterpret_cast<Maths::Mat3*>(&data[offset]));
		break;

	case OpenGLShaderUniformDeclaration::Type::MAT4:
		SetUniformMat4(field.GetLocation(), *reinterpret_cast<Maths::Mat4*>(&data[offset]));
		break;

	default:
		TP_ASSERT(false, "[Shader][OpenGL] Unknown type!");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniform1f(const std::string& name, const float value) const
{
	SetUniform1f(GetUniformLocation(name), value);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniform1fv(const std::string& name, float* value, const int32_t count) const
{
	SetUniform1fv(GetUniformLocation(name), value, count);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniform1i(const std::string& name, const int32_t value) const
{
	SetUniform1i(GetUniformLocation(name), value);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniform1iv(const std::string& name, int32_t* value, const int32_t count) const
{
	SetUniform1iv(GetUniformLocation(name), value, count);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniform2f(const std::string& name, const Maths::Vec2& vector) const
{
	SetUniform2f(GetUniformLocation(name), vector);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniform3f(const std::string& name, const Maths::Vec3& vector) const
{
	SetUniform3f(GetUniformLocation(name), vector);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniform4f(const std::string& name, const Maths::Vec4& vector) const
{
	SetUniform4f(GetUniformLocation(name), vector);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniformMat3(const std::string& name, const Maths::Mat3& matrix) const
{
	SetUniformMat3(GetUniformLocation(name), matrix);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniformMat4(const std::string& name, const Maths::Mat4& matrix) const
{
	SetUniformMat4(GetUniformLocation(name), matrix);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniform1f(const unsigned int location, const float value)
{
	OpenGLCall(glUniform1f(location, value));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniform1fv(const unsigned int location, float* value, const int32_t count)
{
	OpenGLCall(glUniform1fv(location, count, value));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniform1i(const unsigned int location, const int32_t value)
{
	OpenGLCall(glUniform1i(location, value));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniform1iv(const unsigned int location, int32_t* value, const int32_t count)
{
	OpenGLCall(glUniform1iv(location, count, value));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniform2f(const unsigned int location, const Maths::Vec2& vector)
{
	OpenGLCall(glUniform2f(location, vector.x, vector.y));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniform3f(const unsigned int location, const Maths::Vec3& vector)
{
	OpenGLCall(glUniform3f(location, vector.x, vector.y, vector.z));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniform4f(const unsigned int location, const Maths::Vec4& vector)
{
	OpenGLCall(glUniform4f(location, vector.x, vector.y, vector.z, vector.w));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniformMat3(const unsigned int location, const Maths::Mat3& matrix)
{
	OpenGLCall(glUniformMatrix3fv(location, 1, GL_TRUE, matrix.elements.data()));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::SetUniformMat4(const unsigned int location, const Maths::Mat4& matrix)
{
	OpenGLCall(glUniformMatrix4fv(location, 1, GL_TRUE, matrix.elements.data()));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::OpenGLShader::PreProcess(const std::string& source, std::array<std::string*, 6>& shaders)
{
	ShaderType type = ShaderType::UNKNOWN;

	std::vector<std::string> lines = Utils::String::GetLines(source);
	//Get Shader Type
	for(unsigned int i = 0; i < lines.size(); i++)
	{
		if(Utils::String::StartsWith(lines[i], "#shader"))
		{
			if (Utils::String::FindToken(lines[i], "vertex"))
				type = ShaderType::VERTEX;
			else if (Utils::String::FindToken(lines[i], "fragment"))
				type = ShaderType::FRAGMENT;
			else if (Utils::String::FindToken(lines[i], "geometry"))
				type = ShaderType::GEOMETRY;
			else if (Utils::String::FindToken(lines[i], "tessellationcontrol"))
				type = ShaderType::TESSELLATIONCONTROL;
			else if (Utils::String::FindToken(lines[i], "tessellationevaluation"))
				type = ShaderType::TESSELLATIONEVALUATION;
			else if (Utils::String::FindToken(lines[i], "compute"))
				type = ShaderType::COMPUTE;

			//Add version tag if doesnt exist
			if(!Utils::String::StartsWith(lines[i + 1], "#version ") && type != ShaderType::UNKNOWN)
				shaders[static_cast<int32_t>(type) - 1]->append("#version 460 core\n");
		}
		else if(type != ShaderType::UNKNOWN)
		{
			//Ignore comments(Only non-range based)
			//@TODO Also ignore any range based comments
			if(Utils::String::SplitString(lines[i], "//").size() > 1)
			{
				shaders[static_cast<int32_t>(type) - 1]->append(Utils::String::SplitString(lines[i], "//").front());
				shaders[static_cast<int32_t>(type) - 1]->append("\n");
			}
			else
			{
				shaders[static_cast<int32_t>(type) - 1]->append(lines[i]);
				shaders[static_cast<int32_t>(type) - 1]->append("\n");
			}
		}
	}
}