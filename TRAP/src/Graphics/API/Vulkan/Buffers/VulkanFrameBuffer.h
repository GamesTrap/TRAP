#ifndef TRAP_VULKANFRAMEBUFFER2D_H
#define TRAP_VULKANFRAMEBUFFER2D_H

#include "Graphics/Buffers/FrameBuffer.h"

namespace TRAP::Graphics::API
{
	/*class VulkanFrameBuffer final : public FrameBuffer
	{
	public:
		explicit VulkanFrameBuffer(const FrameBufferProps& props);
		virtual ~VulkanFrameBuffer();
		VulkanFrameBuffer(const VulkanFrameBuffer&) = delete;
		VulkanFrameBuffer& operator=(const VulkanFrameBuffer&) = delete;
		VulkanFrameBuffer(VulkanFrameBuffer&&) = default;
		VulkanFrameBuffer& operator=(VulkanFrameBuffer&&) = default;

		void Bind() override;
		void Unbind() override;

		void Resize(uint32_t width, uint32_t height) override;

		uint32_t GetColorAttachmentRendererID() const override;

		const FrameBufferProps& GetProps() const override;

	private:
		FrameBufferProps m_props;
	};*/
}

#endif /*TRAP_VULKANFRAMEBUFFER2D_H*/