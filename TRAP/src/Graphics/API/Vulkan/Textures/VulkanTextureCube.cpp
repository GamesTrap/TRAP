#include "TRAPPCH.h"
#include "VulkanTextureCube.h"

#include "VFS/VFS.h"

TRAP::Graphics::API::VulkanTextureCube::VulkanTextureCube(const TextureParameters parameters)
	: m_name("FallbackCube"), m_parameters(parameters)
{
	TP_PROFILE_FUNCTION();
	
	TP_WARN("[TextureCube][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTextureCube::VulkanTextureCube(std::string name, const std::string& filepath, InputFormat format, const TextureParameters parameters)
	: m_name(std::move(name)), m_filePaths{VFS::MakeVirtualPathCompatible(filepath)}, m_parameters(parameters)
{
	TP_PROFILE_FUNCTION();
	
	TP_WARN("[TextureCube][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTextureCube::VulkanTextureCube(std::string name, const std::array<std::string, 6>& filepaths, const TextureParameters parameters)
	: m_name(std::move(name)),
	  m_filePaths{
		  VFS::MakeVirtualPathCompatible(filepaths[0]),
		  VFS::MakeVirtualPathCompatible(filepaths[1]),
		  VFS::MakeVirtualPathCompatible(filepaths[2]),
		  VFS::MakeVirtualPathCompatible(filepaths[3]),
		  VFS::MakeVirtualPathCompatible(filepaths[4]),
		  VFS::MakeVirtualPathCompatible(filepaths[5])
	  },
	  m_parameters(parameters)
{
	TP_PROFILE_FUNCTION();
	
	TP_WARN("[TextureCube][Vulkan] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTextureCube::~VulkanTextureCube()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTextureCube::Bind(uint32_t slot) const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTextureCube::Unbind(uint32_t slot) const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::API::VulkanTextureCube::GetName() const
{
	TP_PROFILE_FUNCTION();

	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Graphics::API::VulkanTextureCube::GetFilePath() const
{
	TP_PROFILE_FUNCTION();

	return m_filePaths[0];
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureParameters TRAP::Graphics::API::VulkanTextureCube::GetParameters()
{
	TP_PROFILE_FUNCTION();

	return m_parameters;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTextureCube::SetWrap(TextureWrap wrap)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTextureCube::SetFilter(TextureFilter filter)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::InputFormat TRAP::Graphics::API::VulkanTextureCube::GetInputFormat() const
{
	TP_PROFILE_FUNCTION();

	return InputFormat::NONE;
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<std::string, 6> TRAP::Graphics::API::VulkanTextureCube::GetFilePaths() const
{
	TP_PROFILE_FUNCTION();

	return m_filePaths;
}