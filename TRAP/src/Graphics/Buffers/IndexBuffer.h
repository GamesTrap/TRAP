#ifndef _TRAP_INDEXBUFFER_H_
#define _TRAP_INDEXBUFFER_H_

namespace TRAP::Graphics
{
	enum class BufferUsage;

	class IndexBuffer
	{
	public:
		IndexBuffer() = default;
		IndexBuffer(const IndexBuffer&) = default;
		IndexBuffer& operator=(const IndexBuffer&) = default;
		IndexBuffer(IndexBuffer&&) = default;
		IndexBuffer& operator=(IndexBuffer&&) = default;
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static std::unique_ptr<IndexBuffer> Create(uint32_t* indices, uint32_t size, BufferUsage usage);
	};
}

#endif /*_TRAP_INDEXBUFFER_H_*/