#ifndef _TRAP_INDEXBUFFER_H_
#define _TRAP_INDEXBUFFER_H_

namespace TRAP::Graphics
{
	class IndexBuffer
	{
	public:
		IndexBuffer() = default;
		IndexBuffer(const IndexBuffer&) = default;
		IndexBuffer& operator=(const IndexBuffer&) = default;
		IndexBuffer(IndexBuffer&&) = default;
		IndexBuffer& operator=(IndexBuffer&&) = default;
		virtual ~IndexBuffer() = default;

		virtual uint32_t GetCount() const = 0;

		static Scope<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};
}

#endif /*_TRAP_INDEXBUFFER_H_*/