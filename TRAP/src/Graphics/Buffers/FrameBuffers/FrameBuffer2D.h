#ifndef _TRAP_FRAMEBUFFER2D_H_
#define _TRAP_FRAMEBUFFER2D_H_

#include "FrameBuffer.h"
#include "Maths/Maths.h"

namespace TRAP::Graphics
{
	class FrameBuffer2D : public FrameBuffer
	{
	public:
		virtual void SetClearColor(const Math::Vec4& color = { 0.1f,0.1f,0.1f,1.0f }) = 0;

		static Scope<FrameBuffer2D> Create(uint32_t width, uint32_t height);
	};
}

#endif /*_TRAP_FRAMEBUFFER2D_H_*/