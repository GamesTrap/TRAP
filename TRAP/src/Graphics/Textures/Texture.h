#ifndef _TRAP_TEXTURE_H_
#define _TRAP_TEXTURE_H_

#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/API/ResourceLoader.h"

namespace TRAP
{
	class Image;
}


namespace TRAP::Graphics
{
	class TextureBase;

	enum class TextureType
	{
		Texture2D,
		TextureCube
	};

	using TextureUsage = BufferUsage;

	class Texture
	{
	protected:
		Texture();

	public:
		~Texture() = default;
		Texture(const Texture&) = default;
		Texture& operator=(const Texture&) = default;
		Texture(Texture&&) = default;
		Texture& operator=(Texture&&) = default;

		const std::string& GetName() const;
		TextureType GetType() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		virtual uint32_t GetDepth() const = 0;
		virtual uint32_t GetArraySize() const = 0;
		uint32_t GetMipLevels() const;
		TextureUsage GetTextureUsage() const;

		TRAP::Ref<TRAP::Graphics::TextureBase> GetTexture() const;

		bool IsLoaded() const;
		void AwaitLoading() const;

		virtual void UploadImage(const TRAP::Scope<TRAP::Image>& image) = 0;

	protected:
		std::string m_name;
		TextureType m_textureType;
		TextureUsage m_textureUsage;
		API::SyncToken m_syncToken;

		TRAP::Ref<TRAP::Graphics::TextureBase> m_texture;
	};
}

#endif /*_TRAP_TEXTURE_H_*/