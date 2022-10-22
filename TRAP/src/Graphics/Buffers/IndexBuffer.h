#ifndef TRAP_INDEXBUFFER_H
#define TRAP_INDEXBUFFER_H

#include "Application.h"
#include "Graphics/API/ResourceLoader.h"
#include "Graphics/API/Objects/Buffer.h"

namespace TRAP::Graphics
{
	using UpdateFrequency = RendererAPI::DescriptorUpdateFrequency;

	class IndexBuffer
	{
	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="indexType">Index type of the index buffer.</param>
		explicit IndexBuffer(RendererAPI::IndexType indexType);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		IndexBuffer(const IndexBuffer&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		IndexBuffer& operator=(const IndexBuffer&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		IndexBuffer(IndexBuffer&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		IndexBuffer& operator=(IndexBuffer&&) = default;

	public:
		/// <summary>
		/// Destructor.
		/// </summary>
		~IndexBuffer();

		/// <summary>
		/// Retrieve the count of indices inside this buffer.
		/// </summary>
		/// <returns>Count of indices.</returns>
		uint32_t GetCount() const;
		/// <summary>
		/// Retrieve the total byte size of the buffer.
		/// </summary>
		/// <returns>Total buffer byte size.</returns>
		uint64_t GetSize() const;
		/// <summary>
		/// Retrieve the update frequency used by this buffer.
		/// </summary>
		/// <returns>Update frequency.</returns>
		UpdateFrequency GetUpdateFrequency() const;

		/// <summary>
		/// Use this buffer for rendering on the given window.
		/// </summary>
		/// <param name="window">Window to use index buffer on. Default: Main Window.</param>
		void Use(const Window* const window = TRAP::Application::GetWindow()) const;

		/// <summary>
		/// Update the buffers index data.
		/// </summary>
		/// <param name="indices">Pointer to the updated data.</param>
		/// <param name="size">Size of the updated data.</param>
		/// <param name="offset">Byte offset into the currently used index data.</param>
		void SetData(const uint16_t* indices, uint64_t size, uint64_t offset = 0);
		/// <summary>
		/// Update the buffers index data.
		/// </summary>
		/// <param name="indices">Pointer to the updated data.</param>
		/// <param name="size">Size of the updated data.</param>
		/// <param name="offset">Byte offset into the currently used index data.</param>
		void SetData(const uint32_t* indices, uint64_t size, uint64_t offset = 0);

		/// <summary>
		/// Check whether uploading data to the GPU has finished.
		/// </summary>
		/// <returns>True if uploading data to the GPU has finished.</returns>
		bool IsLoaded() const;
		/// <summary>
		/// Wait until uploading data to the GPU has finished.
		/// </summary>
		void AwaitLoading() const;

		/// <summary>
		/// Create a new index buffer and set its data.
		/// </summary>
		/// <param name="indices">Pointer to the data to upload.</param>
		/// <param name="size">Byte size of the data to upload.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New index buffer.</returns>
		static Scope<IndexBuffer> Create(const uint16_t* indices, uint64_t size, UpdateFrequency updateFrequency);
		/// <summary>
		/// Create a new index buffer and set its data.
		/// </summary>
		/// <param name="indices">Pointer to the data to upload.</param>
		/// <param name="size">Byte size of the data to upload.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New index buffer.</returns>
		static Scope<IndexBuffer> Create(const uint32_t* indices, uint64_t size, UpdateFrequency updateFrequency);
		/// <summary>
		/// Create a new index buffer and set its size.
		/// </summary>
		/// <param name="size">Byte size for the index buffer.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New index buffer.</returns>
		static Scope<IndexBuffer> Create(uint64_t size, UpdateFrequency updateFrequency);

	private:
		/// <summary>
		/// Initialize index buffer with given data.
		/// </summary>
		/// <param name="indices">Pointer to the data to upload.</param>
		/// <param name="size">Byte size of the data to upload.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New index buffer.</returns>
		template<typename T>
		static TRAP::Scope<IndexBuffer> Init(const T* indices, const uint64_t size, const UpdateFrequency updateFrequency);

		/// <summary>
		/// Set new index buffer data.
		/// </summary>
		/// <param name="indices">Pointer to the data to upload.</param>
		/// <param name="size">Byte size of the data to upload.</param>
		/// <param name="offset">Byte offset into the currently used index data.</param>
		template<typename T>
		void SetDataInternal(const T* indices, uint64_t size, uint64_t offset = 0);

		TRAP::Ref<TRAP::Graphics::Buffer> m_indexBuffer;

		API::SyncToken m_token;

		RendererAPI::IndexType m_indexType;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Init(const T* const indices, const uint64_t size,
                                                                           const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	static_assert(std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t>,
	              "Trying to initialize index buffer with wrong index type!");

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
void TRAP::Graphics::IndexBuffer::SetDataInternal(const T* const indices, const uint64_t size,
                                                  const uint64_t offset)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	static_assert(std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t>,
	              "Trying to initialize index buffer with wrong index type!");

	TRAP_ASSERT(indices);
	TRAP_ASSERT(size + offset <= m_indexBuffer->GetSize());

	RendererAPI::BufferUpdateDesc desc{};
	desc.Buffer = m_indexBuffer;
	desc.DstOffset = offset;
	RendererAPI::GetResourceLoader()->BeginUpdateResource(desc);
	std::copy_n(indices, size / sizeof(T), static_cast<T*>(desc.MappedData));
	RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_token);

	if constexpr(std::is_same_v<T, uint16_t>)
		m_indexType = RendererAPI::IndexType::UInt16;
	else if constexpr(std::is_same_v<T, uint32_t>)
		m_indexType = RendererAPI::IndexType::UInt32;
}

#endif /*TRAP_INDEXBUFFER_H*/