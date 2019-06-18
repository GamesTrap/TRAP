#ifndef _TRAP_VERTEXARRAY_H_
#define _TRAP_VERTEXARRAY_H_

namespace TRAP::Graphics::API
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

		virtual std::vector<std::unique_ptr<VertexBuffer>>& GetVertexBuffers() = 0;
		virtual IndexBuffer* GetIndexBuffer() = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void Draw() const = 0;

		static const VertexArray* s_CurrentlyBound;
	};
}

#endif /*_TRAP_VERTEXARRAY_H_*/