#include "TRAPPCH.h"
#include "Shader.h"

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::API::Shader* TRAP::Graphics::API::Shader::s_CurrentlyBound = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

std::unique_ptr<TRAP::Graphics::API::Shader> TRAP::Graphics::API::Shader::CreateFromFile(const std::string& name, const std::string& VSFilePath, const std::string& FSFilePath, const std::string& GSFilePath, const std::string& TCSFilePath, const std::string& TESFilePath, const std::string& CSFilePath, Shader* address)
{
	std::string VSSource;
	std::string VFSVSFilePath;
	if (!VSFilePath.empty())
	{
		VSSource = VFS::Get()->SilentReadTextFile(VSFilePath);
		VFSVSFilePath = VFS::MakeVirtualPathCompatible(VSFilePath);
	}
	std::string FSSource;
	std::string VFSFSFilePath;
	if (!FSFilePath.empty())
	{
		FSSource = VFS::Get()->SilentReadTextFile(FSFilePath);
		VFSFSFilePath = VFS::MakeVirtualPathCompatible(FSFilePath);
	}
	std::string GSSource;
	std::string VFSGSFilePath;
	if (!GSFilePath.empty())
	{
		GSSource = VFS::Get()->SilentReadTextFile(GSFilePath);
		VFSGSFilePath = VFS::MakeVirtualPathCompatible(GSFilePath);
	}
	std::string TCSSource;
	std::string VFSTCSFilePath;
	if (!TCSFilePath.empty())
	{
		TCSSource = VFS::Get()->SilentReadTextFile(TCSFilePath);
		VFSTCSFilePath = VFS::MakeVirtualPathCompatible(TCSFilePath);
	}
	std::string TESSource;
	std::string VFSTESFilePath;
	if (!TESFilePath.empty())
	{
		TESSource = VFS::Get()->SilentReadTextFile(TESFilePath);
		VFSTESFilePath = VFS::MakeVirtualPathCompatible(TESFilePath);
	}
	std::string CSSource;
	std::string VFSCSFilePath;
	if (!CSFilePath.empty())
	{
		CSSource = VFS::Get()->SilentReadTextFile(CSFilePath);
		VFSCSFilePath = VFS::MakeVirtualPathCompatible(CSFilePath);
	}

	if (!VSFilePath.empty() && VSSource.empty())
		TP_ERROR("[Shader] Shader: \"", name, "\" couldn't load Vertex Shader!");
	if (!FSFilePath.empty() && FSSource.empty())
		TP_ERROR("[Shader] Shader: \"", name, "\" couldn't load Fragment Shader!");
	if (!GSFilePath.empty() && GSSource.empty())
		TP_ERROR("[Shader] Shader: \"", name, "\" couldn't load Geometry Shader!");
	if (!TCSFilePath.empty() && TCSSource.empty())
		TP_ERROR("[Shader] Shader: \"", name, "\" couldn't load Tessellation Control Shader!");
	if (!TESFilePath.empty() && TESSource.empty())
		TP_ERROR("[Shader] Shader: \"", name, "\" couldn't load Tessellation Evaluation Shader!");
	if (!CSFilePath.empty() && CSSource.empty())
		TP_ERROR("[Shader] Shader: \"", name, "\" couldn't load Compute Shader!");

	if (VSSource.empty() && FSSource.empty() && GSSource.empty() && TCSSource.empty() && TESSource.empty() && CSSource.empty())
	{
		TP_ERROR("[Shader] Shader: \"", name, "\" all sources are empty!");
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
			address = std::make_unique<D3D12Shader>(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource).get();
			result = std::unique_ptr<D3D12Shader>(dynamic_cast<D3D12Shader*>(address));
		}
		else
			result = std::make_unique<D3D12Shader>(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);
		result->m_VSFilepath = VFSVSFilePath;
		result->m_FSFilepath = VFSFSFilePath;
		result->m_GSFilepath = VFSGSFilePath;
		result->m_TCSFilepath = VFSTCSFilePath;
		result->m_TESFilepath = VFSTESFilePath;
		result->m_CSFilepath = VFSCSFilePath;
		return result;
	}
#endif

	case RenderAPI::VULKAN:
	{
		std::unique_ptr<VulkanShader> result;
		if (address != nullptr)
		{
			address = std::make_unique<VulkanShader>(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource).get();
			result = std::unique_ptr<VulkanShader>(dynamic_cast<VulkanShader*>(address));
		}
		else
			result = std::make_unique<VulkanShader>(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);
		result->m_VSFilepath = VFSVSFilePath;
		result->m_FSFilepath = VFSFSFilePath;
		result->m_GSFilepath = VFSGSFilePath;
		result->m_TCSFilepath = VFSTCSFilePath;
		result->m_TESFilepath = VFSTESFilePath;
		result->m_CSFilepath = VFSCSFilePath;
		return result;
	}

	case RenderAPI::OPENGL:
	{
		std::unique_ptr<OpenGLShader> result;
		if (address != nullptr)
		{
			address = std::make_unique<OpenGLShader>(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource).get();
			result = std::unique_ptr<OpenGLShader>(dynamic_cast<OpenGLShader*>(address));
		}
		else
			result = std::make_unique<OpenGLShader>(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);
		result->m_VSFilepath = VFSVSFilePath;
		result->m_FSFilepath = VFSFSFilePath;
		result->m_GSFilepath = VFSGSFilePath;
		result->m_TCSFilepath = VFSTCSFilePath;
		result->m_TESFilepath = VFSTESFilePath;
		result->m_CSFilepath = VFSCSFilePath;
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