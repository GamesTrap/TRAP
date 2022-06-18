#ifndef TRAP_STORAGEBUFFER_H
#define TRAP_STORAGEBUFFER_H

#include "Graphics/API/ResourceLoader.h"
#include "Graphics/API/Objects/Buffer.h"
#include "Application.h"

namespace TRAP
{
	class Window;
}

namespace TRAP::Graphics
{
	using UpdateFrequency = RendererAPI::DescriptorUpdateFrequency;

	class StorageBuffer
	{
	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="updateFrequency">Update frequency for the storage buffer.</param>
		explicit StorageBuffer(RendererAPI::DescriptorUpdateFrequency updateFrequency);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		StorageBuffer(const StorageBuffer&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		StorageBuffer& operator=(const StorageBuffer&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		StorageBuffer(StorageBuffer&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		StorageBuffer& operator=(StorageBuffer&&) = default;

	public:
		/// <summary>
		/// Destructor.
		/// </summary>
		~StorageBuffer();

		/// <summary>
		/// Retrieve the byte size of the SSBO.
		/// </summary>
		/// <returns>Byte size of the SSBO.</returns>
		uint64_t GetSize() const;
		/// <summary>
		/// Retrieve the update frequency of the SSBO.
		/// </summary>
		/// <returns>Update frequency of the SSBO.</returns>
		UpdateFrequency GetUpdateFrequency() const;
		/// <summary>
		/// Retrieve the underlying buffers.
		/// </summary>
		/// <returns>Underlying buffers.</returns>
		const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& GetSSBOs() const;

		/// <summary>
		/// Update data of the SSBO.
		/// </summary>
		/// <param name="data">New data.</param>
		/// <param name="size">Byte size of the data.</param>
		/// <param name="offset">Offset into the currently used data.</param>
		void SetData(const void* data, uint64_t size, uint64_t offset = 0);
		/// <summary>
		/// Retrieve data of the SSBO.
		/// </summary>
		/// <param name="data">Pointer to store data in.</param>
		/// <param name="size">Byte size for data storage.</param>
		/// <param name="offset">Offset into the currently used data.</param>
		/// <param name="window">Window to use for the data retrieval.</param>
		template<typename T>
		void GetData(const T* data, uint64_t size, const uint64_t offset = 0,
		             Window* window = nullptr);

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
		/// Calculate the aligned size of the SSBO.
		/// </summary>
		/// <param name="byteSize">Byte size of the SSBO.</param>
		static uint64_t CalculateAlignedSize(const uint64_t byteSize);

		/// <summary>
		/// Create a new shader storage buffer and set its size.
		/// </summary>
		/// <param name="size">Byte size for the uniform buffer.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New shader storage buffer.</returns>
		static Scope<StorageBuffer> Create(uint64_t size, UpdateFrequency updateFrequency);
		/// <summary>
		/// Create a new shader storage buffer and set its data.
		/// </summary>
		/// <param name="data">Pointer to the data to upload.</param>
		/// <param name="size">Byte size of the data to upload.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New shader storage buffer.</returns>
		static Scope<StorageBuffer> Create(void* data, uint64_t size, UpdateFrequency updateFrequency);

	private:
		/// <summary>
		/// Initialize shader storage buffer with given data.
		/// </summary>
		/// <param name="data">Pointer to the data to upload.</param>
		/// <param name="size">Byte size of the data to upload.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New shader storage buffer.</returns>
		static Scope<StorageBuffer> Init(void* data, uint64_t size, UpdateFrequency updateFrequency);

		std::vector<TRAP::Ref<TRAP::Graphics::Buffer>> m_storageBuffers;

		std::vector<API::SyncToken> m_tokens;
	};
}

template<typename T>
inline void TRAP::Graphics::StorageBuffer::GetData(const T* data, const uint64_t size, const uint64_t offset, Window* window)
{
	TRAP_ASSERT(size + offset <= m_storageBuffers[0]->GetSize());

	if(!window)
		window = TRAP::Application::GetWindow();

	RendererAPI::BufferUpdateDesc desc{};
	const uint32_t imageIndex = GetUpdateFrequency() ==
								RendererAPI::DescriptorUpdateFrequency::Static ?
									0 : RendererAPI::GetCurrentImageIndex(window);
	desc.Buffer = m_storageBuffers[imageIndex];
	desc.DstOffset = offset;
	RendererAPI::GetResourceLoader()->BeginUpdateResource(desc);
	std::memcpy(data, desc.MappedData, size);
	RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_tokens[imageIndex]);
}

#endif /*TRAP_STORAGEBUFFER_H*/