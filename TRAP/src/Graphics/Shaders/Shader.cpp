#include "TRAPPCH.h"
#include "Shader.h"

#include "VFS/VFS.h"
#include "Graphics/API/Context.h"
#include "Graphics/API/D3D12/D3D12Shader.h"
#include "Graphics/API/OpenGL/OpenGLShader.h"
#include "Graphics/API/Vulkan/VulkanShader.h"
#include "VFS/FileSystem.h"
#include "Utils/String/String.h"

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Graphics::Shader* TRAP::Graphics::Shader::s_CurrentlyBound = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::Shader::GetName() const
{
	TP_PROFILE_FUNCTION();

	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::Shader::GetFilePath() const
{
	TP_PROFILE_FUNCTION();

	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromFile(const std::string& name, const std::string& filePath)
{
	TP_PROFILE_FUNCTION();

	if(name.empty())
	{
		TP_WARN("[Shader] Name is empty! Using Filename as Shader Name!");
		return CreateFromFile(filePath);
	}
	
	std::string source;
	std::string VFSFilePath;
	std::vector<uint32_t> SPIRVSource;
	bool isSPIRV = false;
	if (!filePath.empty())
	{
		if (Utils::String::ToLower(Utils::String::GetSuffix(filePath)) != "spirv" && Utils::String::ToLower(Utils::String::GetSuffix(filePath)) != "shader")
		{
			TP_ERROR("[Shader] File: \"", filePath, "\" suffix is not \"*.spirv\" or \"*.shader\"!");
			TP_WARN("[Shader] Shader using fallback Shader: \"Fallback\"");
			return nullptr;
		}
		
		isSPIRV = CheckSPIRVMagicNumber(filePath);
		
		if (!isSPIRV)
			source = VFS::SilentReadTextFile(filePath);
		else
			SPIRVSource = Convert8To32(VFS::SilentReadFile(filePath));

		VFSFilePath = VFS::MakeVirtualPathCompatible(filePath);
	}

	if(isSPIRV)
	{
		if(!filePath.empty() && SPIRVSource.empty())
			TP_ERROR("[Shader][SPIR-V] Couldn't load Shader: \"", name, "\"!");
	}
	else
		if (!filePath.empty() && source.empty())
			TP_ERROR("[Shader] Couldn't load Shader: \"", name, "\"!");

	if ((source.empty() && !isSPIRV) || (SPIRVSource.empty() && isSPIRV))
	{
		TP_WARN("[Shader] Shader using fallback Shader: \"Fallback\"");
		return nullptr;
	}

	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
	{
		Scope<API::D3D12Shader> result;
		if (isSPIRV)
			result = MakeScope<API::D3D12Shader>(name, SPIRVSource);
		else
			result = MakeScope<API::D3D12Shader>(name, source);
			
		result->m_filepath = VFSFilePath;
		return result;
	}
#endif

	case API::RenderAPI::Vulkan:
	{
		Scope<API::VulkanShader> result;
		if (isSPIRV)
			result = MakeScope<API::VulkanShader>(name, SPIRVSource);
		else
			result = MakeScope<API::VulkanShader>(name, source);
			
		result->m_filepath = VFSFilePath;
		return result;
	}

	case API::RenderAPI::OpenGL:
	{
		Scope<API::OpenGLShader> result;
		if (isSPIRV)
			result = MakeScope<API::OpenGLShader>(name, SPIRVSource);
		else
			result = MakeScope<API::OpenGLShader>(name, source);
			
		result->m_filepath = VFSFilePath;
		return result;
	}

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromFile(const std::string& filePath)
{
	TP_PROFILE_FUNCTION();

	std::string source;
	std::string VFSFilePath;
	std::string name;
	std::vector<uint32_t> SPIRVSource;
	bool isSPIRV = false;
	if (!filePath.empty())
	{
		if (Utils::String::ToLower(Utils::String::GetSuffix(filePath)) != "spirv" && Utils::String::ToLower(Utils::String::GetSuffix(filePath)) != "shader")
		{
			TP_ERROR("[Shader] File: \"", filePath, "\" suffix is not \"*.spirv\" or \"*.shader\"!");
			TP_WARN("[Shader] Shader using fallback Shader: \"Fallback\"");
			return nullptr;
		}
		
		isSPIRV = CheckSPIRVMagicNumber(filePath);

		if (!isSPIRV)
			source = VFS::SilentReadTextFile(filePath);
		else
			SPIRVSource = Convert8To32(VFS::SilentReadFile(filePath));
		
		VFSFilePath = VFS::MakeVirtualPathCompatible(filePath);
		name = VFS::GetFileName(VFSFilePath);
	}

	if (isSPIRV)
	{
		if (!filePath.empty() && SPIRVSource.empty())
			TP_ERROR("[Shader][SPIR-V] Couldn't load Shader: \"", name, "\"!");
	}
	else
		if (!filePath.empty() && source.empty())
			TP_ERROR("[Shader] Couldn't load Shader: \"", name, "\"!");

	if ((source.empty() && !isSPIRV) || (SPIRVSource.empty() && isSPIRV))
	{
		TP_WARN("[Shader] Shader using fallback Shader: \"Fallback\"");
		return nullptr;
	}

	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
	{
		Scope<API::D3D12Shader> result;
		if (isSPIRV)
			result = MakeScope<API::D3D12Shader>(name, SPIRVSource);
		else
			result = MakeScope<API::D3D12Shader>(name, source);

		result->m_filepath = VFSFilePath;
		return result;
	}
#endif

	case API::RenderAPI::Vulkan:
	{
		Scope<API::VulkanShader> result;
		if (isSPIRV)
			result = MakeScope<API::VulkanShader>(name, SPIRVSource);
		else
			result = MakeScope<API::VulkanShader>(name, source);

		result->m_filepath = VFSFilePath;
		return result;
	}

	case API::RenderAPI::OpenGL:
	{
		Scope<API::OpenGLShader> result;
		if (isSPIRV)
			result = MakeScope<API::OpenGLShader>(name, SPIRVSource);
		else
			result = MakeScope<API::OpenGLShader>(name, source);

		result->m_filepath = VFSFilePath;
		return result;
	}

	default:
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Shader> TRAP::Graphics::Shader::CreateFromSource(const std::string& name, const std::string& VSSource, const std::string& FSSource, const std::string& GSSource, const std::string& TCSSource, const std::string& TESSource, const std::string& CSSource)
{
	TP_PROFILE_FUNCTION();

	switch (API::Context::GetRenderAPI())
	{
#ifdef TRAP_PLATFORM_WINDOWS
	case API::RenderAPI::D3D12:
		return MakeScope<API::D3D12Shader>(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);
#endif

	case API::RenderAPI::Vulkan:
		return MakeScope<API::VulkanShader>(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);

	case API::RenderAPI::OpenGL:
		return MakeScope<API::OpenGLShader>(name, VSSource, FSSource, GSSource, TCSSource, TESSource, CSSource);

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

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Shader::CheckSPIRVMagicNumber(const std::string& filePath)
{
	//Check SPIRV Magic Number
	std::filesystem::path physicalPath;
	if (VFS::SilentResolveReadPhysicalPath(filePath, physicalPath))
	{
		if (!FileSystem::SilentFileOrFolderExists(physicalPath))
		{
			std::ifstream file(physicalPath, std::ios::binary);
			if (file.is_open())
			{
				//SPIRV Magic Number:
				//0x07230203

				uint32_t magicNumber = 0;
				file.read(reinterpret_cast<char*>(&magicNumber), sizeof(uint32_t)); //Number of SubShaders
				file.read(reinterpret_cast<char*>(&magicNumber), sizeof(uint32_t)); //Size of the current SubShader in uint32_ts
				file.read(reinterpret_cast<char*>(&magicNumber), sizeof(uint32_t)); //Type of the current SubShader
				file.read(reinterpret_cast<char*>(&magicNumber), sizeof(uint32_t)); //SPIRV Magic Number
				file.close();

				if (magicNumber == 0x07230203)
					return true;

				return false;
			}

			TP_ERROR("[FileSystem] Could not open File: ", physicalPath);
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint32_t> TRAP::Graphics::Shader::Convert8To32(const std::vector<std::byte>& source)
{
	std::vector<uint32_t> data{};
	data.resize(source.size() / 4);

	uint32_t j = 0;
	for(uint32_t i = 0; i < source.size(); i += 4)
	{
		const uint32_t val = static_cast<uint8_t>(source[i]) |
			                 (static_cast<uint8_t>(source[i + 1]) << 8) |
			                 (static_cast<uint8_t>(source[i + 2]) << 16) |
			                 (static_cast<uint8_t>(source[i + 3]) << 24);
		data[j++] = val;
	}
	
	return data;
}