#ifndef TRAP_STORAGEBUFFER_H
#define TRAP_STORAGEBUFFER_H

#include "Graphics/API/ResourceLoader.h"
#include "Graphics/API/RendererAPI.h"
#include "Application.h"

namespace TRAP::Graphics
{
	class Buffer;

	class StorageBuffer
	{
	protected:
		/// @brief Constructor.
		/// @param storageBuffers Internal storage buffer objects.
		/// @param syncTokens Synchronization tokens.
		constexpr StorageBuffer(const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& storageBuffers, const std::vector<API::SyncToken>& syncTokens);

	public:
		/// @brief Move constructor.
		constexpr StorageBuffer(StorageBuffer&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr StorageBuffer& operator=(StorageBuffer&&) noexcept = default;
		/// @brief Copy constructor.
		consteval StorageBuffer(const StorageBuffer&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval StorageBuffer& operator=(const StorageBuffer&) noexcept = delete;

		/// @brief Destructor.
		constexpr ~StorageBuffer() = default;

		/// @brief Retrieve the byte size of the SSBO.
		/// @return Byte size of the SSBO.
		[[nodiscard]] u64 GetSize() const noexcept;
		/// @brief Retrieve the update frequency of the SSBO.
		/// @return Update frequency of the SSBO.
		[[nodiscard]] constexpr DescriptorUpdateFrequency GetUpdateFrequency() const noexcept;
		/// @brief Retrieve the underlying buffers.
		/// @return Underlying buffers.
		[[nodiscard]] constexpr const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& GetSSBOs() const noexcept;

		/// @brief Update data of the SSBO.
		/// @param data New data.
		/// @param size Byte size of the data.
		/// @param offset Offset into the currently used data.
		void SetData(const void* data, u64 size, u64 offset = 0);
#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve data of the SSBO.
		/// @param data Pointer to store data in.
		/// @param size Byte size for data storage.
		/// @param offset Offset into the currently used data.
		/// @param window Window to use for the data retrieval. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void GetData(const auto* data, u64 size, u64 offset = 0,
		             const Window& window = *TRAP::Application::GetWindow());
#else
		/// @brief Retrieve data of the SSBO.
		/// @param data Pointer to store data in.
		/// @param size Byte size for data storage.
		/// @param offset Offset into the currently used data.
		/// @remark This function is only available in headless mode.
		void GetData(const auto* data, u64 size, u64 offset = 0);
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Check whether uploading data to the GPU has finished.
		/// @return True if uploading data to the GPU has finished.
		[[nodiscard]] bool IsLoaded() const;
		/// @brief Wait until uploading data to the GPU has finished.
		void AwaitLoading() const;

		/// @brief Calculate the aligned size of the SSBO.
		/// @param byteSize Byte size of the SSBO.
		[[nodiscard]] static u64 CalculateAlignedSize(u64 byteSize) noexcept;

		/// @brief Create a new shader storage buffer and set its size.
		/// @param size Byte size for the uniform buffer.
		/// @param updateFrequency Update frequency for the buffer.
		/// @return New shader storage buffer.
		[[nodiscard]] static Scope<StorageBuffer> Create(u64 size, DescriptorUpdateFrequency updateFrequency);
		/// @brief Create a new shader storage buffer and set its data.
		/// @param data Pointer to the data to upload.
		/// @param size Byte size of the data to upload.
		/// @param updateFrequency Update frequency for the buffer.
		/// @return New shader storage buffer.
		template<typename T>
		[[nodiscard]] static Scope<StorageBuffer> Create(const T* data, u64 size, DescriptorUpdateFrequency updateFrequency);

	private:
		/// @brief Initialize shader storage buffer with given data.
		/// @param data Pointer to the data to upload.
		/// @param size Byte size of the data to upload.
		/// @param updateFrequency Update frequency for the buffer.
		/// @return New shader storage buffer.
		template<typename T>
		[[nodiscard]] static Scope<StorageBuffer> Init(const T* data, u64 size, DescriptorUpdateFrequency updateFrequency);

		std::vector<TRAP::Ref<TRAP::Graphics::Buffer>> m_storageBuffers;

		std::vector<API::SyncToken> m_tokens;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::StorageBuffer::StorageBuffer(const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& storageBuffers,
                                                       const std::vector<API::SyncToken>& syncTokens)
	: m_storageBuffers(storageBuffers), m_tokens(syncTokens)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::DescriptorUpdateFrequency TRAP::Graphics::StorageBuffer::GetUpdateFrequency() const noexcept
{
	return m_storageBuffers.size() == 1 ? DescriptorUpdateFrequency::Static : DescriptorUpdateFrequency::Dynamic;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& TRAP::Graphics::StorageBuffer::GetSSBOs() const noexcept
{
	return m_storageBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] TRAP::Scope<TRAP::Graphics::StorageBuffer> TRAP::Graphics::StorageBuffer::Create(const T* const data, const u64 size,
																				               const DescriptorUpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return Init(data, size, updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] TRAP::Scope<TRAP::Graphics::StorageBuffer> TRAP::Graphics::StorageBuffer::Init(const T* const data, const u64 size,
																			                 const DescriptorUpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	const BufferDesc bufferDesc
	{
		.Size = size,
		.MemoryUsage = (updateFrequency == DescriptorUpdateFrequency::Static) ? ResourceMemoryUsage::GPUOnly :
	                                                                  ResourceMemoryUsage::CPUToGPU,
		.Flags = BufferCreationFlags::PersistentMap,
		.ElementCount = size / sizeof(T),
		.StructStride = sizeof(T),
		.Descriptors = DescriptorType::RWBuffer
	};

	BufferLoadDesc desc
	{
		.Data = data,
		.Desc = bufferDesc
	};

	std::vector<TRAP::Ref<TRAP::Graphics::Buffer>> storageBuffers((updateFrequency == DescriptorUpdateFrequency::Static) ? 1 : ImageCount);
	std::vector<API::SyncToken> syncTokens(storageBuffers.size());
	for(u32 i = 0; i < storageBuffers.size(); ++i)
	{
		RendererAPI::GetResourceLoader()->AddResource(desc, &syncTokens[i]);
		storageBuffers[i] = desc.Buffer;
	}

	return TRAP::Scope<StorageBuffer>(new StorageBuffer(storageBuffers, syncTokens));
}

#endif /*TRAP_STORAGEBUFFER_H*/
