#ifndef _TRAP_TEXTURE_H_
#define _TRAP_TEXTURE_H_

#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/API/ResourceLoader.h"

namespace TRAP
{
	class Image;
}
namespace TRAP::Graphics::API
{
	class VulkanTexture;
}

namespace TRAP::Graphics
{
	enum class TextureType
	{
		Texture2D,
		TextureCube
	};

	using TextureUsage = BufferUsage;

	//TODO Move to Sampler class
	// using TextureWrap = RendererAPI::AddressMode;
	// using TextureFilter = RendererAPI::FilterType;
	// struct TextureParameters
	// {
	// 	TextureFilter Filter;
	// 	TextureWrap Wrap;

	// 	TextureParameters();
	// 	explicit TextureParameters(TextureFilter filter);
	// 	TextureParameters(TextureFilter filter, TextureWrap wrap);
	// };

	class Texture
	{
	protected:
		Texture();

	public:
		virtual ~Texture();
		Texture(const Texture&) = default;
		Texture& operator=(const Texture&) = default;
		Texture(Texture&&) = default;
		Texture& operator=(Texture&&) = default;

		const std::string& GetName() const;
		TextureType GetType() const;

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetDepth() const;
		uint32_t GetMipLevels() const;
		TextureUsage GetTextureUsage() const;

		bool IsLoaded() const;
		void AwaitLoading() const;

		//TODO Getters for other things?

		TRAP::Ref<TRAP::Graphics::API::VulkanTexture> GetTexture() const;//TODO Remove

		virtual void UploadImage(const TRAP::Scope<TRAP::Image>& image) = 0;

	protected:
		std::string m_name;
		TextureType m_textureType;
		TextureUsage m_textureUsage;
		API::SyncToken m_syncToken;

		TRAP::Ref<TRAP::Graphics::API::VulkanTexture> m_texture;
	};
}

#endif /*_TRAP_TEXTURE_H_*/