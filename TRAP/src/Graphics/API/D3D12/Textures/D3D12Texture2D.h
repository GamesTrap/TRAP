#ifndef _TRAP_D3D12TEXTURE_H_
#define _TRAP_D3D12TEXTURE_H_

#ifdef TRAP_PLATFORM_WINDOWS

#include "Graphics/Textures/Texture2D.h"

namespace TRAP::Graphics::API
{
	class D3D12Texture2D final : public Texture2D
	{
	public:
		explicit D3D12Texture2D(TextureParameters parameters);
		D3D12Texture2D(ImageFormat format, uint32_t width, uint32_t height, TextureParameters parameters);
		D3D12Texture2D(std::string name, const std::string& filepath, TextureParameters parameters);
		~D3D12Texture2D();
		D3D12Texture2D(const D3D12Texture2D&) = delete;
		D3D12Texture2D& operator=(const D3D12Texture2D&) = delete;
		D3D12Texture2D(D3D12Texture2D&&) = default;
		D3D12Texture2D& operator=(D3D12Texture2D&&) = default;

		void Bind(uint32_t slot = 0) const override;
		void Unbind(uint32_t slot = 0) const override;

		std::string GetName() const override;
		std::string GetFilePath() const override;
		TextureParameters GetParameters() override;

		void SetWrap(TextureWrap wrap) override;
		void SetFilter(TextureFilter filter) override;

	private:
		std::string m_filePath;
		//Scope<Image> m_image;
		std::string m_name;
		TextureParameters m_parameters;
		uint32_t m_handle;
	};
}

#endif

#endif /*_TRAP_D3D12TEXTURE_H_*/