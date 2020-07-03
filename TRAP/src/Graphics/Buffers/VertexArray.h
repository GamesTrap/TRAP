#ifndef _TRAP_VERTEXARRAY_H_
#define _TRAP_VERTEXARRAY_H_

#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace TRAP::Graphics
{
	class VertexArray
	{
	public:
		VertexArray() = default;
		virtual ~VertexArray() = default;
		VertexArray(const VertexArray&) = default;
		VertexArray& operator=(const VertexArray&) = default;
		VertexArray(VertexArray&&) = default;
		VertexArray& operator=(VertexArray&&) = default;

		static Scope<VertexArray> Create();

		virtual void SetVertexBuffer(Scope<VertexBuffer>& buffer) = 0;
		virtual void SetIndexBuffer(Scope<IndexBuffer>& buffer) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		
		virtual const Scope<VertexBuffer>& GetVertexBuffer() = 0;
		virtual const Scope<IndexBuffer>& GetIndexBuffer() = 0;

	protected:
		const static VertexArray* s_CurrentlyBound;
	};
}

#endif /*_TRAP_VERTEXARRAY_H_*/