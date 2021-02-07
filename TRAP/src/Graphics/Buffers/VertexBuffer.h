#ifndef _TRAP_VERTEXBUFFER_H_
#define _TRAP_VERTEXBUFFER_H_

#include "BufferLayout.h"
#include "Graphics/API/ResourceLoader.h"

namespace TRAP::Graphics
{
	class Buffer;

	class VertexBuffer
	{
	protected:
		VertexBuffer();
		VertexBuffer(const VertexBuffer&) = default;
		VertexBuffer& operator=(const VertexBuffer&) = default;
		VertexBuffer(VertexBuffer&&) = default;
		VertexBuffer& operator=(VertexBuffer&&) = default;

	public:
		~VertexBuffer();

		const BufferLayout& GetLayout() const;
		void SetLayout(const BufferLayout& layout);
		uint64_t GetSize() const;
		uint64_t GetCount() const;
		BufferUsage GetBufferUsage() const;

		void SetData(float* data, uint64_t size, uint64_t offset = 0);

		void Use(Window* window = nullptr) const;
		
		bool IsLoaded() const;
		void AwaitLoading() const;

		static Scope<VertexBuffer> Create(float* vertices, uint64_t size, BufferUsage usage);
		static Scope<VertexBuffer> Create(uint64_t size, BufferUsage usage);

	private:
		TRAP::Ref<TRAP::Graphics::Buffer> m_vertexBuffer;

		BufferLayout m_bufferLayout;
		API::SyncToken m_token;
	};
}

#endif /*_TRAP_VERTEXBUFFER_H_*/