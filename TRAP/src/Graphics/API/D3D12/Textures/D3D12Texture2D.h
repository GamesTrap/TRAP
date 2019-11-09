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
		D3D12Texture2D(uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format, TextureParameters parameters);
		D3D12Texture2D(std::string name, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format, const std::vector<uint8_t>& pixelData, TextureParameters parameters);
		D3D12Texture2D(std::string name, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format, const std::vector<uint16_t>& pixelData, TextureParameters parameters);
		D3D12Texture2D(std::string name, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format, const std::vector<float>& pixelData, TextureParameters parameters);
		D3D12Texture2D(std::string name, const std::string& filepath, TextureParameters parameters);
		~D3D12Texture2D();
		D3D12Texture2D(const D3D12Texture2D&) = delete;
		D3D12Texture2D& operator=(const D3D12Texture2D&) = delete;
		D3D12Texture2D(D3D12Texture2D&&) = default;
		D3D12Texture2D& operator=(D3D12Texture2D&&) = default;

		void Bind(uint32_t slot) const override;
		void Unbind(uint32_t slot) const override;

		std::string_view GetName() const override;
		std::string_view GetFilePath() const override;
		TextureParameters GetParameters() override;

		void SetWrap(TextureWrap wrap) override;
		void SetFilter(TextureFilter filter) override;

	private:
		std::string m_name;
		std::string m_filePath;
		TextureParameters m_parameters;
	};
}

#endif

#endif /*_TRAP_D3D12TEXTURE_H_*/