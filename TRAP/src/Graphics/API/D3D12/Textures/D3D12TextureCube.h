#ifndef _TRAP_D3D12TEXTURECUBE_H_
#define _TRAP_D3D12TEXTURECUBE_H_

#include "Graphics/Textures/TextureCube.h"

namespace TRAP::Graphics::API
{
	class D3D12TextureCube final : public TextureCube
	{
	public:
		explicit D3D12TextureCube(TextureParameters parameters);
		D3D12TextureCube(std::string name, const std::array<std::string, 6>& filepaths, TextureParameters parameters);
		D3D12TextureCube(std::string name, std::string filepath, InputFormat format, TextureParameters parameters);
		~D3D12TextureCube();
		D3D12TextureCube(const D3D12TextureCube&) = default;
		D3D12TextureCube& operator=(const D3D12TextureCube&) = default;
		D3D12TextureCube(D3D12TextureCube&&) = default;
		D3D12TextureCube& operator=(D3D12TextureCube&&) = default;

		void Bind(uint32_t slot) const override;
		void Unbind(uint32_t slot) const override;

		std::string GetName() const override;
		Image* GetImage() override;
		TextureParameters GetParameters() override;

		void SetWrap(const TextureWrap wrap) override;
		void SetFilter(const TextureFilter filter) override;

		InputFormat GetInputFormat() const override;
		std::array<Image*, 6> GetImages() const override;

	private:
		std::string m_name;
		std::vector<Scope<Image>> m_images;
		TextureParameters m_parameters;
	};
}

#endif /*_TRAP_D3D12TEXTURECUBE_H_*/