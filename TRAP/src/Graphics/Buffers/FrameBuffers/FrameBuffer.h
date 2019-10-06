#ifndef _TRAP_FRAMEBUFFER_H_
#define _TRAP_FRAMEBUFFER_H_

#include "Graphics/Textures/Texture.h"

namespace TRAP::Graphics
{
	class FrameBuffer
	{
	public:
		FrameBuffer() = default;
		virtual ~FrameBuffer() = default;
		FrameBuffer(const FrameBuffer&) = default;
		FrameBuffer& operator=(const FrameBuffer&) = default;
		FrameBuffer(FrameBuffer&&) = default;
		FrameBuffer& operator=(FrameBuffer&&) = default;
		
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Clear() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual const Scope<Texture>& GetTexture() const = 0;

	protected:
		virtual void Init();
	};
}

#endif /*_TRAP_FRAMEBUFFER_H_*/