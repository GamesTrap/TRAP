#ifndef _TRAP_D3D12TEXTURECUBE_H_
#define _TRAP_D3D12TEXTURECUBE_H_

#include "Graphics/Textures/TextureCube.h"

namespace TRAP::Graphics::API
{
	class D3D12TextureCube final : public TextureCube
	{
	public:
		D3D12TextureCube(TextureParameters parameters);
		D3D12TextureCube(std::string name, const std::array<std::string, 6>& filepaths, TextureParameters parameters);
		D3D12TextureCube(std::string name, std::string filepath, InputFormat format, TextureParameters parameters);
		~D3D12TextureCube();
		D3D12TextureCube(const D3D12TextureCube&) = default;
		D3D12TextureCube& operator=(const D3D12TextureCube&) = default;
		D3D12TextureCube(D3D12TextureCube&&) = default;
		D3D12TextureCube& operator=(D3D12TextureCube&&) = default;

		void Bind(unsigned int slot) const override;
		void Unbind(unsigned int slot) const override;

		std::string GetName() const override;
		Image* GetImage() override;
		TextureParameters GetParameters() override;

		void SetWrap(const TextureWrap wrap) override;
		void SetFilter(const TextureFilter filter) override;

		InputFormat GetInputFormat() const override;
		std::array<Image*, 6> GetImages() const override;

	private:
		std::string m_name;
		std::vector<std::unique_ptr<Image>> m_images;
		TextureParameters m_parameters;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string TRAP::Graphics::API::D3D12TextureCube::GetName() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Image* TRAP::Graphics::API::D3D12TextureCube::GetImage()
{
	return m_images[0].get();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::TextureParameters TRAP::Graphics::API::D3D12TextureCube::GetParameters()
{
	return m_parameters;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::array<TRAP::Image*, 6> TRAP::Graphics::API::D3D12TextureCube::GetImages() const
{
	return { m_images[0].get(), m_images[1].get(), m_images[2].get(), m_images[3].get(), m_images[4].get(), m_images[5].get(), };
}

#endif /*_TRAP_D3D12TEXTURECUBE_H_*/