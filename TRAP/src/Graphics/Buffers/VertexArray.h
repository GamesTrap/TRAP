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

		static std::unique_ptr<VertexArray> Create();

		virtual void AddVertexBuffer(std::unique_ptr<VertexBuffer>& buffer) = 0;
		virtual void SetIndexBuffer(std::unique_ptr<IndexBuffer>& buffer) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		
		virtual std::vector<std::unique_ptr<VertexBuffer>>& GetVertexBuffers() = 0;
		virtual IndexBuffer* GetIndexBuffer() = 0;
		virtual uint32_t GetIndexCount() const = 0;

	protected:
		const static VertexArray* s_CurrentlyBound;
	};
}

#endif /*_TRAP_VERTEXARRAY_H_*/