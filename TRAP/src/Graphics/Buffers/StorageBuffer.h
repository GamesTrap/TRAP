#ifndef TRAP_STORAGEBUFFER_H
#define TRAP_STORAGEBUFFER_H

#include "Graphics/API/ResourceLoader.h"
#include "Graphics/API/Objects/Buffer.h"
#include "Graphics/RenderCommand.h"
#include "Application.h"

namespace TRAP
{
	class Window;
}

namespace TRAP::Graphics
{
	class StorageBuffer
	{
	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="updateFrequency">Update frequency for the storage buffer.</param>
		constexpr explicit StorageBuffer(RendererAPI::DescriptorUpdateFrequency updateFrequency);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr StorageBuffer(const StorageBuffer&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr StorageBuffer& operator=(const StorageBuffer&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr StorageBuffer(StorageBuffer&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr StorageBuffer& operator=(StorageBuffer&&) noexcept = default;

	public:
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~StorageBuffer() = default;

		/// <summary>
		/// Retrieve the byte size of the SSBO.
		/// </summary>
		/// <returns>Byte size of the SSBO.</returns>
		[[nodiscard]] uint64_t GetSize() const noexcept;
		/// <summary>
		/// Retrieve the update frequency of the SSBO.
		/// </summary>
		/// <returns>Update frequency of the SSBO.</returns>
		[[nodiscard]] constexpr UpdateFrequency GetUpdateFrequency() const noexcept;
		/// <summary>
		/// Retrieve the underlying buffers.
		/// </summary>
		/// <returns>Underlying buffers.</returns>
		[[nodiscard]] constexpr const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& GetSSBOs() const noexcept;

		/// <summary>
		/// Update data of the SSBO.
		/// </summary>
		/// <param name="data">New data.</param>
		/// <param name="size">Byte size of the data.</param>
		/// <param name="offset">Offset into the currently used data.</param>
		void SetData(const void* data, uint64_t size, uint64_t offset = 0);
#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Retrieve data of the SSBO.
		/// </summary>
		/// <param name="data">Pointer to store data in.</param>
		/// <param name="size">Byte size for data storage.</param>
		/// <param name="offset">Offset into the currently used data.</param>
		/// <param name="window">Window to use for the data retrieval. Default: Main Window.</param>
		void GetData(const auto* data, uint64_t size, uint64_t offset = 0,
		             const Window* window = TRAP::Application::GetWindow());
#else
		/// <summary>
		/// Retrieve data of the SSBO.
		/// </summary>
		/// <param name="data">Pointer to store data in.</param>
		/// <param name="size">Byte size for data storage.</param>
		/// <param name="offset">Offset into the currently used data.</param>
		void GetData(const auto* data, uint64_t size, uint64_t offset = 0);
#endif /*TRAP_HEADLESS_MODE*/

		/// <summary>
		/// Check whether uploading data to the GPU has finished.
		/// </summary>
		/// <returns>True if uploading data to the GPU has finished.</returns>
		[[nodiscard]] bool IsLoaded() const;
		/// <summary>
		/// Wait until uploading data to the GPU has finished.
		/// </summary>
		void AwaitLoading() const;

		/// <summary>
		/// Calculate the aligned size of the SSBO.
		/// </summary>
		/// <param name="byteSize">Byte size of the SSBO.</param>
		[[nodiscard]] static uint64_t CalculateAlignedSize(uint64_t byteSize) noexcept;

		/// <summary>
		/// Create a new shader storage buffer and set its size.
		/// </summary>
		/// <param name="size">Byte size for the uniform buffer.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New shader storage buffer.</returns>
		[[nodiscard]] static Scope<StorageBuffer> Create(uint64_t size, UpdateFrequency updateFrequency);
		/// <summary>
		/// Create a new shader storage buffer and set its data.
		/// </summary>
		/// <param name="data">Pointer to the data to upload.</param>
		/// <param name="size">Byte size of the data to upload.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New shader storage buffer.</returns>
		[[nodiscard]] static Scope<StorageBuffer> Create(const void* data, uint64_t size, UpdateFrequency updateFrequency);

	private:
		/// <summary>
		/// Initialize shader storage buffer with given data.
		/// </summary>
		/// <param name="data">Pointer to the data to upload.</param>
		/// <param name="size">Byte size of the data to upload.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New shader storage buffer.</returns>
		[[nodiscard]] static Scope<StorageBuffer> Init(const void* data, uint64_t size, UpdateFrequency updateFrequency);

		std::vector<TRAP::Ref<TRAP::Graphics::Buffer>> m_storageBuffers;

		std::vector<API::SyncToken> m_tokens;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::StorageBuffer::StorageBuffer(const RendererAPI::DescriptorUpdateFrequency updateFrequency)
{
	m_tokens.resize(updateFrequency == UpdateFrequency::Static ? 1 : RendererAPI::ImageCount);
	m_storageBuffers.resize(updateFrequency == UpdateFrequency::Static ? 1 : RendererAPI::ImageCount);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::UpdateFrequency TRAP::Graphics::StorageBuffer::GetUpdateFrequency() const noexcept
{
	return m_storageBuffers.size() == 1 ? UpdateFrequency::Static : UpdateFrequency::Dynamic;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
inline void TRAP::Graphics::StorageBuffer::GetData(const auto* const data, const uint64_t size, const uint64_t offset, const Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(size + offset <= m_storageBuffers[0]->GetSize(), "StorageBuffer::GetData(): Out of bounds!");
	TRAP_ASSERT(window, "StorageBuffer::GetData(): Window is nullptr");

	RendererAPI::BufferUpdateDesc desc{};
	const uint32_t imageIndex = GetUpdateFrequency() ==
								RendererAPI::DescriptorUpdateFrequency::Static ?
									0 : RendererAPI::GetCurrentImageIndex(window);
	desc.Buffer = m_storageBuffers[imageIndex];
	desc.DstOffset = offset;
	API::ResourceLoader::BeginUpdateResource(desc);
	std::copy_n(static_cast<uint8_t*>(desc.MappedData), size, data);
	RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_tokens[imageIndex]);
}
#else
inline void TRAP::Graphics::StorageBuffer::GetData(const auto* const data, const uint64_t size, const uint64_t offset)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(size + offset <= m_storageBuffers[0]->GetSize(), "StorageBuffer::GetData(): Out of bounds!");

	RendererAPI::BufferUpdateDesc desc{};
	const uint32_t imageIndex = GetUpdateFrequency() ==
								RendererAPI::DescriptorUpdateFrequency::Static ?
									0 : RendererAPI::GetCurrentImageIndex();
	desc.Buffer = m_storageBuffers[imageIndex];
	desc.DstOffset = offset;
	API::ResourceLoader::BeginUpdateResource(desc);
	std::copy_n(static_cast<uint8_t*>(desc.MappedData), size, data);
	RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_tokens[imageIndex]);
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& TRAP::Graphics::StorageBuffer::GetSSBOs() const noexcept
{
	return m_storageBuffers;
}

#endif /*TRAP_STORAGEBUFFER_H*/