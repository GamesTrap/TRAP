#ifndef TRAP_TEXTUREBASE_H
#define TRAP_TEXTUREBASE_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
    class TextureBase
    {
    public:
		/// <summary>
		/// Create a texture base.
		/// </summary>
		/// <param name="desc">Texture description.</param>
		/// <returns>New texture base.</returns>
        static TRAP::Ref<TextureBase> Create(const RendererAPI::TextureDesc& desc);

		/// <summary>
		/// Destructor.
		/// </summary>
        virtual ~TextureBase() = default;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		TextureBase(const TextureBase&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		TextureBase& operator=(const TextureBase&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		TextureBase(TextureBase&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		TextureBase& operator=(TextureBase&&) = default;

		/// <summary>
		/// Retrieve texture width.
		/// </summary>
		/// <returns>Width.</returns>
        uint32_t GetWidth() const;
		/// <summary>
		/// Retrieve texture height.
		/// </summary>
		/// <returns>Height.</returns>
		uint32_t GetHeight() const;
		/// <summary>
		/// Retrieve texture depth.
		/// </summary>
		/// <returns>Depth.</returns>
		uint32_t GetDepth() const;
		/// <summary>
		/// Retrieve texture mip levels.
		/// </summary>
		/// <returns>Mip levels count.</returns>
		uint32_t GetMipLevels() const;
		/// <summary>
		/// Retrieve texture array size.
		/// </summary>
		/// <returns>Array size.</returns>
		uint32_t GetArraySize() const;
		/// <summary>
		/// Retrieve textures image format.
		/// </summary>
		/// <returns>Image format.</returns>
		TRAP::Graphics::API::ImageFormat GetImageFormat() const;
		/// <summary>
		/// Retrieve textures aspect mask.
		/// Aspect mask specifies which aspects (Color, Depth, Stencil) are included in the texture.
		/// </summary>
		/// <returns>Aspect mask.</returns>
		uint32_t GetAspectMask() const;
		/// <summary>
		/// Retrieve texture unordered access view format.
		/// </summary>
		/// <returns>Unordered access view format.</returns>
		RendererAPI::DescriptorType GetUAV() const;
		/// <summary>
		/// Retrieve whether texture owns the image data.
		/// </summary>
		/// <returns>True if texture owns the image data, false otherwise.</returns>
		bool OwnsImage() const;

		/// <summary>
		/// Set texture name.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
        virtual void SetTextureName(const std::string_view name) const = 0;

    protected:
		/// <summary>
		/// Constructor.
		/// </summary>
        TextureBase() = default;

		/// <summary>
		/// Validate texture limits.
		/// </summary>
		/// <param name="desc">Texture description.</param>
		/// <returns>True if texture is inside limits, false otherwise.</returns>
		static bool ValidateLimits(const RendererAPI::TextureDesc& desc);

        //Current state of the buffer
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_depth;
		uint32_t m_mipLevels;
		uint32_t m_arraySize;
		TRAP::Graphics::API::ImageFormat m_format;
		//Flags specifying which aspects (Color, Depth, Stencil) are included in the m_vkImageView
		uint32_t m_aspectMask;
		RendererAPI::DescriptorType m_UAV;
		//This value will be false if the underlying resource is not owned by the texture (swapchain textures,...)
		bool m_ownsImage;
    };
}

#endif /*TRAP_TEXTUREBASE_H*/