#ifndef _TRAP_INDEXBUFFER_H_
#define _TRAP_INDEXBUFFER_H_

#include "Graphics/API/ResourceLoader.h"

namespace TRAP::Graphics
{
	enum class BufferUsage;

	class IndexBuffer
	{
	protected:
		IndexBuffer();
		IndexBuffer(const IndexBuffer&) = default;
		IndexBuffer& operator=(const IndexBuffer&) = default;
		IndexBuffer(IndexBuffer&&) = default;
		IndexBuffer& operator=(IndexBuffer&&) = default;

	public:
		~IndexBuffer();

		uint64_t GetCount() const;
		uint64_t GetSize() const;
		BufferUsage GetBufferUsage() const;

		void Use(Window* window = nullptr) const;

		void SetData(uint16_t* indices, uint32_t size, uint32_t offset = 0);
		void SetData(uint32_t* indices, uint32_t size, uint32_t offset = 0);

		bool IsLoaded() const;
		void AwaitLoading() const;

		static Scope<IndexBuffer> Create(uint16_t* indices, uint32_t size, BufferUsage usage);
		static Scope<IndexBuffer> Create(uint32_t* indices, uint32_t size, BufferUsage usage);
		static Scope<IndexBuffer> Create(uint32_t size, BufferUsage usage);

	private:
		TRAP::Ref<TRAP::Graphics::Buffer> m_indexBuffer;

		API::SyncToken m_token;

		RendererAPI::IndexType m_indexType;
	};
}

#endif /*_TRAP_INDEXBUFFER_H_*/