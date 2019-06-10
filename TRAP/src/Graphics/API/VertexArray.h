#ifndef _TRAP_VERTEXARRAY_H_
#define _TRAP_VERTEXARRAY_H_

namespace TRAP::Graphics::API
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		static std::unique_ptr<VertexArray> Create();

		virtual void AddBuffer(std::unique_ptr<VertexBuffer>& buffer) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void Draw(uint32_t count) const = 0;
	};
}

#endif /*_TRAP_VERTEXARRAY_H_*/