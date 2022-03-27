#ifndef TRAP_VERTEXBUFFER_H
#define TRAP_VERTEXBUFFER_H

#include "VertexBufferLayout.h"
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

		const VertexBufferLayout& GetLayout() const;
		void SetLayout(const VertexBufferLayout& layout);
		uint64_t GetSize() const;
		uint32_t GetCount() const;
		UpdateFrequency GetUpdateFrequency() const;

		void SetData(float* data, uint64_t size, uint64_t offset = 0);

		void Use(Window* window = nullptr) const;

		bool IsLoaded() const;
		void AwaitLoading() const;

		static Scope<VertexBuffer> Create(float* vertices, uint64_t size, UpdateFrequency updateFrequency);
		static Scope<VertexBuffer> Create(uint64_t size, UpdateFrequency updateFrequency);

	private:
		static Scope<VertexBuffer> Init(const float* vertices, uint64_t size, UpdateFrequency updateFrequency);

		TRAP::Ref<TRAP::Graphics::Buffer> m_vertexBuffer;

		VertexBufferLayout m_bufferLayout;
		API::SyncToken m_token;
	};
}

#endif /*TRAP_VERTEXBUFFER_H*/