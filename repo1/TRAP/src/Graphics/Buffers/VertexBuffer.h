#ifndef _TRAP_VERTEXBUFFER_H_
#define _TRAP_VERTEXBUFFER_H_

#include "BufferLayout.h"

namespace TRAP::Graphics
{
	class VertexBuffer
	{
	public:
		VertexBuffer() = default;
		VertexBuffer(const VertexBuffer&) = default;
		VertexBuffer& operator=(const VertexBuffer&) = default;
		VertexBuffer(VertexBuffer&&) = default;
		VertexBuffer& operator=(VertexBuffer&&) = default;
		virtual ~VertexBuffer() = default;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual uint32_t GetVertexCount() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		static Scope<VertexBuffer> Create(float* vertices, uint32_t size);
		static Scope<VertexBuffer> Create(uint32_t size);
	};
}

#endif /*_TRAP_VERTEXBUFFER_H_*/