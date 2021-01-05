#ifndef _TRAP_RENDERTARGET_H_
#define _TRAP_RENDERTARGET_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class RenderTarget
	{
	public:
		static TRAP::Ref<RenderTarget> Create(const RendererAPI::RenderTargetDesc& desc);
		
		RenderTarget();
		virtual ~RenderTarget() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetDepth() const = 0;
		virtual uint32_t GetArraySize() const = 0;
		virtual uint32_t GetMipLevels() const = 0;
		virtual RendererAPI::SampleCount GetSampleCount() const = 0;
		virtual uint32_t GetSampleQuality() const = 0;
		virtual RendererAPI::ImageFormat GetImageFormat() const = 0;
		virtual RendererAPI::ClearValue GetClearValue() const = 0;
		virtual RendererAPI::DescriptorType GetDescriptorType() const = 0;

	protected:
		TRAP::Ref<API::VulkanTexture> m_texture; //TODO

		RendererAPI::ClearValue m_clearValue;
		uint32_t m_arraySize;
		uint32_t m_depth;
		uint32_t m_width;
		uint32_t m_height;
		RendererAPI::DescriptorType m_descriptors;
		uint32_t m_mipLevels;
		uint32_t m_sampleQuality;
		RendererAPI::ImageFormat m_format;
		RendererAPI::SampleCount m_sampleCount;
	};
}

#endif /*_TRAP_RENDERTARGET_H_*/