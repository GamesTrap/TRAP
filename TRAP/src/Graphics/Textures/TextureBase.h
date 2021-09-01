#ifndef _TRAP_TEXTUREBASE_H_
#define _TRAP_TEXTUREBASE_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
    class TextureBase
    {
    public:
        static TRAP::Ref<TextureBase> Create(const RendererAPI::TextureDesc& desc);

        virtual ~TextureBase() = default;

        uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetDepth() const;
		uint32_t GetMipLevels() const;
		uint32_t GetArraySize() const;
		TRAP::Graphics::API::ImageFormat GetImageFormat() const;
		uint32_t GetAspectMask() const;
		RendererAPI::DescriptorType GetUAV() const;
		bool OwnsImage() const;

        virtual void SetTextureName(const std::string& name) const = 0;

    protected:
        TextureBase() = default;

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

#endif /*_TRAP_TEXTUREBASE_H_*/