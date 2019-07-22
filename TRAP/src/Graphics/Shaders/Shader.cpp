#include "TRAPPCH.h"
#include "Shader.h"

#include "VFS/VFS.h"
#include "Graphics/API/Context.h"
#include "Graphics/API/D3D12/D3D12Shader.h"
#include "Graphics/API/OpenGL/OpenGLShader.h"
#include "Graphics/API/Vulkan/VulkanShader.h"

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::API::Shader* TRAP::Graphics::API::Shader::s_CurrentlyBound = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::API::Shader> TRAP::Graphics::API::Shader::CreateFromFile(const std::string& name, const std::string& filePath, Shader* address)
{
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

	switch (Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case RenderAPI::D3D12:
	{
		std::unique_ptr<D3D12Shader> result;
		if (address != nullptr)
		{
			address = std::make_unique<D3D12Shader>(name, source).get();
			result = std::unique_ptr<D3D12Shader>(dynamic_cast<D3D12Shader*>(address));
		}
		else
			result = std::make_unique<D3D12Shader>(name, source);
		result->m_filepath = VFSFilePath;
		return result;
	}
#endif

	case RenderAPI::VULKAN:
	{
		std::unique_ptr<VulkanShader> result;
		if (address != nullptr)
		{
			address = std::make_unique<VulkanShader>(name, source).get();
			result = std::unique_ptr<VulkanShader>(dynamic_cast<VulkanShader*>(address));
		}
		else
			result = std::make_unique<VulkanShader>(name, source);
		result->m_filepath = VFSFilePath;
		return result;
	}

	case RenderAPI::OPENGL:
	{
		std::unique_ptr<OpenGLShader> result;
		if (address != nullptr)
		{
			address = std::make_unique<OpenGLShader>(name, source).get();
			result = std::unique_ptr<OpenGLShader>(dynamic_cast<OpenGLShader*>(address));
		}
		else
			result = std::make_unique<OpenGLShader>(name, source);
		result->m_filepath = VFSFilePath;
		return result;
	}

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::API::Shader> TRAP::Graphics::API::Shader::CreateFromSource(const std::string& name, const std::string& VSSource, const std::string& FSSource, const std::string& GSSource, const std::string& TCSSource, const std::string& TESSource, const std::string& CSSource)
{
	switch (Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case RenderAPI::D3D12:
	{
		std::unique_ptr<D3D12Shader> result = std::make_unique<D3D12Shader>(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);

		return result;
	}
#endif

	case RenderAPI::VULKAN:
	{
		std::unique_ptr<VulkanShader> result = std::make_unique<VulkanShader>(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);

		return result;
	}

	case RenderAPI::OPENGL:
	{
		std::unique_ptr<OpenGLShader> result = std::make_unique<OpenGLShader>(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);

		return result;
	}

	default:
		return nullptr;
	}
}