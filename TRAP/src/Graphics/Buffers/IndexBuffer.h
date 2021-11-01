#ifndef _TRAP_INDEXBUFFER_H_
#define _TRAP_INDEXBUFFER_H_

#include "Graphics/API/ResourceLoader.h"
#include "Graphics/API/Objects/Buffer.h"

namespace TRAP::Graphics
{
	using UpdateFrequency = RendererAPI::DescriptorUpdateFrequency;

	class IndexBuffer
	{
	protected:
		explicit IndexBuffer(RendererAPI::IndexType indexType);
		IndexBuffer(const IndexBuffer&) = default;
		IndexBuffer& operator=(const IndexBuffer&) = default;
		IndexBuffer(IndexBuffer&&) = default;
		IndexBuffer& operator=(IndexBuffer&&) = default;

	public:
		~IndexBuffer();

		uint32_t GetCount() const;
		uint64_t GetSize() const;
		UpdateFrequency GetUpdateFrequency() const;

		void Use(Window* window = nullptr) const;

		void SetData(uint16_t* indices, uint64_t size, uint64_t offset = 0);
		void SetData(uint32_t* indices, uint64_t size, uint64_t offset = 0);

		bool IsLoaded() const;
		void AwaitLoading() const;

		static Scope<IndexBuffer> Create(uint16_t* indices, uint64_t size, UpdateFrequency updateFrequency);
		static Scope<IndexBuffer> Create(uint32_t* indices, uint64_t size, UpdateFrequency updateFrequency);
		static Scope<IndexBuffer> Create(uint64_t size, UpdateFrequency updateFrequency);

	private:
		template<typename T>
		static TRAP::Scope<IndexBuffer> Init(T* indices, const uint64_t size, const UpdateFrequency updateFrequency);

		template<typename T>
		void SetDataInternal(const T* indices, uint64_t size, uint64_t offset = 0);

		TRAP::Ref<TRAP::Graphics::Buffer> m_indexBuffer;

		API::SyncToken m_token;

		RendererAPI::IndexType m_indexType;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Init(T* indices, const uint64_t size,
                                                                           const UpdateFrequency updateFrequency)
{
	static_assert(std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t>,
	              "Trying to initialize IndexBuffer with wrong index type!");

	RendererAPI::IndexType indexType = {};
	if(indices)
	{
		if constexpr(std::is_same_v<T, uint16_t>)
			indexType = RendererAPI::IndexType::UInt16;
		else if constexpr(std::is_same_v<T, uint32_t>)
			indexType = RendererAPI::IndexType::UInt32;
	}
	else
	{
		if(size / sizeof(uint16_t) < std::numeric_limits<uint16_t>::max())
			indexType = RendererAPI::IndexType::UInt16;
		else
			indexType = RendererAPI::IndexType::UInt32;
	}

	TRAP::Scope<IndexBuffer> buffer = TRAP::Scope<IndexBuffer>(new IndexBuffer(indexType));

	RendererAPI::BufferLoadDesc desc{};
	desc.Desc.MemoryUsage = (updateFrequency == UpdateFrequency::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly :
	                                                                       RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Desc.Descriptors = RendererAPI::DescriptorType::IndexBuffer;
	desc.Desc.Size = size;
	desc.Desc.Flags = (updateFrequency != UpdateFrequency::Static) ? RendererAPI::BufferCreationFlags::PersistentMap :
	                                                                 RendererAPI::BufferCreationFlags::None;
	desc.Data = indices;

	RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_token);

	buffer->m_indexBuffer = desc.Buffer;

	return buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
void TRAP::Graphics::IndexBuffer::SetDataInternal(const T* indices, const uint64_t size,
                                                  const uint64_t offset)
{
	static_assert(std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t>,
	              "Trying to initialize IndexBuffer with wrong index type!");

	TRAP_ASSERT(indices);
	TRAP_ASSERT(size + offset <= m_indexBuffer->GetSize());

	RendererAPI::BufferUpdateDesc desc{};
	desc.Buffer = m_indexBuffer;
	desc.DstOffset = offset;
	RendererAPI::GetResourceLoader()->BeginUpdateResource(desc);
	std::memcpy(desc.MappedData, indices, size);
	RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_token);

	if constexpr(std::is_same_v<T, uint16_t>)
		m_indexType = RendererAPI::IndexType::UInt16;
	else if constexpr(std::is_same_v<T, uint32_t>)
		m_indexType = RendererAPI::IndexType::UInt32;
}

#endif /*_TRAP_INDEXBUFFER_H_*/