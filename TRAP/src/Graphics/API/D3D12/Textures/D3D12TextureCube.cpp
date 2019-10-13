#include "TRAPPCH.h"
#include "D3D12TextureCube.h"

#include "VFS/VFS.h"

TRAP::Graphics::API::D3D12TextureCube::D3D12TextureCube(const TextureParameters parameters)
	: m_name("FallbackCube"), m_parameters(parameters)
{
	TP_WARN("[TextureCube][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12TextureCube::D3D12TextureCube(std::string name, const std::string& filepath, InputFormat format, const TextureParameters parameters)
	: m_name(std::move(name)), m_filePaths{ VFS::MakeVirtualPathCompatible(filepath) }, m_parameters(parameters)
{
	TP_WARN("[TextureCube][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12TextureCube::D3D12TextureCube(std::string name, const std::array<std::string, 6>& filepaths, const TextureParameters parameters)
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
	TP_WARN("[TextureCube][D3D12] WIP");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::D3D12TextureCube::~D3D12TextureCube() = default;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12TextureCube::Bind(uint32_t slot) const
{	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12TextureCube::Unbind(uint32_t slot) const
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::D3D12TextureCube::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::D3D12TextureCube::GetFilePath() const
{
	return m_filePaths[0];
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::TextureParameters TRAP::Graphics::API::D3D12TextureCube::GetParameters()
{
	return m_parameters;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12TextureCube::SetWrap(const TextureWrap wrap)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::D3D12TextureCube::SetFilter(const TextureFilter filter)
{	
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::InputFormat TRAP::Graphics::API::D3D12TextureCube::GetInputFormat() const
{
	return InputFormat::NONE;
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<std::string, 6> TRAP::Graphics::API::D3D12TextureCube::GetFilePaths() const
{
	return m_filePaths;
}