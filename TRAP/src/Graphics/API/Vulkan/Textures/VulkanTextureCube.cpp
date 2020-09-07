#include "TRAPPCH.h"
#include "VulkanTextureCube.h"

#include "VFS/VFS.h"

TRAP::Graphics::API::VulkanTextureCube::VulkanTextureCube(const TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();
	
	m_name = "FallbackCube";
	m_textureParameters = parameters;
	
	TP_WARN(Log::TextureCubeVulkanPrefix, "WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTextureCube::VulkanTextureCube(std::string name, const std::string_view filepath, InputFormat format, const TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	m_filepaths[0] = VFS::MakeVirtualPathCompatible(filepath);
	m_textureParameters = parameters;
	m_inputFormat = format;
	
	TP_WARN(Log::TextureCubeVulkanPrefix, "WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTextureCube::VulkanTextureCube(std::string name, const std::array<std::string_view, 6>& filepaths, const TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	m_filepaths = 
	{
		VFS::MakeVirtualPathCompatible(filepaths[0]),
		VFS::MakeVirtualPathCompatible(filepaths[1]),
		VFS::MakeVirtualPathCompatible(filepaths[2]),
		VFS::MakeVirtualPathCompatible(filepaths[3]),
		VFS::MakeVirtualPathCompatible(filepaths[4]),
		VFS::MakeVirtualPathCompatible(filepaths[5])
	};
	m_textureParameters = parameters;
	
	TP_WARN(Log::TextureCubeVulkanPrefix, "WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTextureCube::VulkanTextureCube(std::string name, const Scope<Image>& img, const InputFormat format, const TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	m_textureParameters = parameters;
	m_inputFormat = format;

	if(img)
	{
		m_filepaths[0] = VFS::MakeVirtualPathCompatible(img->GetFilePath());
	}

	TP_WARN(Log::TextureCubeVulkanPrefix, "WIP");
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

void TRAP::Graphics::API::VulkanTextureCube::SetWrap(TextureWrap wrap)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTextureCube::SetFilter(TextureFilter filter)
{
	TP_PROFILE_FUNCTION();
}