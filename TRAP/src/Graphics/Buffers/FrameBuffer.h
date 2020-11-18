#ifndef _TRAP_FRAMEBUFFER_H_
#define _TRAP_FRAMEBUFFER_H_

namespace TRAP::Graphics
{
	struct FrameBufferProps
	{
		uint32_t Width = 0, Height = 0;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};
	
	class FrameBuffer
	{
	public:
		FrameBuffer() = default;
		virtual ~FrameBuffer() = default;
		FrameBuffer(const FrameBuffer&) = default;
		FrameBuffer& operator=(const FrameBuffer&) = default;
		FrameBuffer(FrameBuffer&&) = default;
		FrameBuffer& operator=(FrameBuffer&&) = default;

		static Scope<FrameBuffer> Create(const FrameBufferProps& props);

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentRendererID() const = 0;
		
		virtual const FrameBufferProps& GetProps() const = 0;
	};
}

#endif /*_TRAP_FRAMEBUFFER_H_*/