#include "TRAPPCH.h"
#include "D3D12TextureCube.h"

#include "VFS/VFS.h"

TRAP::Graphics::API::D3D12TextureCube::D3D12TextureCube(const TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	m_name = "FallbackCube";
	m_textureParameters = parameters;

	TP_WARN("[TextureCube][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12TextureCube::D3D12TextureCube(std::string name, const std::string& filepath, InputFormat format, const TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	m_filepaths[0] = VFS::MakeVirtualPathCompatible(filepath);
	m_textureParameters = parameters;
	m_inputFormat = format;

	TP_WARN("[TextureCube][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12TextureCube::D3D12TextureCube(std::string name, const std::array<std::string, 6>& filepaths, const TextureParameters parameters)
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

	TP_WARN("[TextureCube][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12TextureCube::D3D12TextureCube(std::string name, const Scope<Image>& img, InputFormat format, TextureParameters parameters)
{
	TP_PROFILE_FUNCTION();

	m_name = std::move(name);
	m_textureParameters = parameters;
	m_inputFormat = format;

	if(img)
	{
		m_filepaths[0] = VFS::MakeVirtualPathCompatible(std::string(img->GetFilePath()));		
	}

	TP_WARN("[TextureCube][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12TextureCube::~D3D12TextureCube()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12TextureCube::Bind(uint32_t slot) const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12TextureCube::Unbind(uint32_t slot) const
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12TextureCube::SetWrap(const TextureWrap wrap)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12TextureCube::SetFilter(const TextureFilter filter)
{
	TP_PROFILE_FUNCTION();
}