#include "TRAPPCH.h"
#include "Shader.h"

#include "VFS/VFS.h"
#include "Graphics/API/Context.h"
#include "Graphics/API/D3D12/D3D12Shader.h"
#include "Graphics/API/OpenGL/OpenGLShader.h"
#include "Graphics/API/Vulkan/VulkanShader.h"

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::Shader* TRAP::Graphics::Shader::s_CurrentlyBound = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromFile(const std::string& name, const std::string& filePath)
{
	if(name.empty())
	{
		TP_WARN("[Shader] Name is empty! Using Filename as Shader Name!");
		return CreateFromFile(filePath);
	}
	
	std::string source;
	std::string VFSFilePath;
	if (!filePath.empty())
	{
		source = VFS::Get()->SilentReadTextFile(filePath);
		VFSFilePath = VFS::MakeVirtualPathCompatible(filePath);
	}

	if (!filePath.empty() && source.empty())
		TP_ERROR("[Shader] Shader: \"", name, "\" couldn't load Shader!");

	if (source.empty())
	{
		TP_WARN("[Shader] Shader: \"", name, "\" using fallback Shader: \"Passthrough\"");
		return nullptr;
	}

	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
	{
		std::unique_ptr<API::D3D12Shader> result = std::make_unique<API::D3D12Shader>(name, source);
		result->m_filepath = VFSFilePath;
		return result;
	}
#endif

	case API::RenderAPI::Vulkan:
	{
		std::unique_ptr<API::VulkanShader> result = std::make_unique<API::VulkanShader>(name, source);
		result->m_filepath = VFSFilePath;
		return result;
	}

	case API::RenderAPI::OpenGL:
	{
		std::unique_ptr<API::OpenGLShader> result = std::make_unique<API::OpenGLShader>(name, source);
		result->m_filepath = VFSFilePath;
		return result;
	}

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromFile(const std::string& filePath)
{
	std::string source;
	std::string VFSFilePath;
	std::string name;
	if (!filePath.empty())
	{
		source = VFS::Get()->SilentReadTextFile(filePath);
		VFSFilePath = VFS::MakeVirtualPathCompatible(filePath);
		name = VFS::GetFileName(VFSFilePath);
	}

	if (!filePath.empty() && source.empty())
		TP_ERROR("[Shader] Shader couldn't load Shader!");

	if (source.empty())
	{
		TP_WARN("[Shader] Shader using fallback Shader: \"Passthrough\"");
		return nullptr;
	}

	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
	{
		std::unique_ptr<API::D3D12Shader> result = std::make_unique<API::D3D12Shader>(name, source);
		result->m_filepath = VFSFilePath;
		return result;
	}
#endif

	case API::RenderAPI::Vulkan:
	{
		std::unique_ptr<API::VulkanShader> result = std::make_unique<API::VulkanShader>(name, source);
		result->m_filepath = VFSFilePath;
		return result;
	}

	case API::RenderAPI::OpenGL:
	{
		std::unique_ptr<API::OpenGLShader> result = std::make_unique<API::OpenGLShader>(name, source);
		result->m_filepath = VFSFilePath;
		return result;
	}

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromSource(const std::string& name, const std::string& VSSource, const std::string& FSSource, const std::string& GSSource, const std::string& TCSSource, const std::string& TESSource, const std::string& CSSource)
{
	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
	{
		std::unique_ptr<API::D3D12Shader> result = std::make_unique<API::D3D12Shader>(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);
		return result;
	}
#endif

	case API::RenderAPI::Vulkan:
	{
		std::unique_ptr<API::VulkanShader> result = std::make_unique<API::VulkanShader>(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);
		return result;
	}

	case API::RenderAPI::OpenGL:
	{
		std::unique_ptr<API::OpenGLShader> result = std::make_unique<API::OpenGLShader>(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);
		return result;
	}

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::Shader::ShaderTypeToString(const ShaderType type)
{
	switch(type)
	{
	case ShaderType::Vertex:
		return "VertexShader";

	case ShaderType::Fragment:
		return "FragmentShader";

	case ShaderType::Geometry:
		return "GeometryShade";

	case ShaderType::Tessellation_Control:
		return "TessellationControlShader";

	case ShaderType::Tessellation_Evaluation:
		return "TessellationEvaluationShader";

	case ShaderType::Compute:
		return "ComputeShader";
		
	default:
		return "";
	}
}