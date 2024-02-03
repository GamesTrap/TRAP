#ifndef TRAP_INDEXBUFFER_H
#define TRAP_INDEXBUFFER_H

#include "Application.h"
#include "Graphics/API/ResourceLoader.h"
#include "Graphics/API/Objects/Buffer.h"
#include "Graphics/RenderCommand.h"

namespace TRAP::Graphics
{
	class IndexBuffer
	{
	protected:
		/// @brief Constructor.
		/// @param indexType Index type of the index buffer.
		constexpr explicit IndexBuffer(RendererAPI::IndexType indexType) noexcept;
		/// @brief Move constructor.
		IndexBuffer(IndexBuffer&&) noexcept = default;
		/// @brief Move assignment operator.
		IndexBuffer& operator=(IndexBuffer&&) noexcept = default;

	public:
		/// @brief Copy constructor.
		consteval IndexBuffer(const IndexBuffer&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval IndexBuffer& operator=(const IndexBuffer&) noexcept = delete;

		/// @brief Destructor.
		~IndexBuffer() = default;

		/// @brief Retrieve the count of indices inside this buffer.
		/// @return Count of indices.
		[[nodiscard]] u32 GetCount() const noexcept;
		/// @brief Retrieve the total byte size of the buffer.
		/// @return Total buffer byte size.
		[[nodiscard]] u64 GetSize() const noexcept;
		/// @brief Retrieve the update frequency used by this buffer.
		/// @return Update frequency.
		[[nodiscard]] UpdateFrequency GetUpdateFrequency() const noexcept;

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use this buffer for rendering on the given window.
		/// @param window Window to use index buffer on. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void Use(const Window* window = TRAP::Application::GetWindow()) const;
#else
		/// @brief Use this buffer for rendering.
		/// @remark This function is only available in headless mode.
		void Use() const;
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Update the buffers index data.
		/// @param indices Pointer to the updated data.
		/// @param offset Byte offset into the currently used index data.
		void SetData(std::span<const u16> indices, u64 offset = 0);
		/// @brief Update the buffers index data.
		/// @param indices Pointer to the updated data.
		/// @param offset Byte offset into the currently used index data.
		void SetData(std::span<const u32> indices, u64 offset = 0);

		/// @brief Check whether uploading data to the GPU has finished.
		/// @return True if uploading data to the GPU has finished.
		[[nodiscard]] bool IsLoaded() const;
		/// @brief Wait until uploading data to the GPU has finished.
		void AwaitLoading() const;

		/// @brief Create a new index buffer and set its data.
		/// @param indices Pointer to the data to upload.
		/// @param updateFrequency Update frequency for the buffer.
		/// @return New index buffer.
		[[nodiscard]] static Scope<IndexBuffer> Create(std::span<const u16> indices, UpdateFrequency updateFrequency);
		/// @brief Create a new index buffer and set its data.
		/// @param indices Pointer to the data to upload.
		/// @param updateFrequency Update frequency for the buffer.
		/// @return New index buffer.
		[[nodiscard]] static Scope<IndexBuffer> Create(std::span<const u32> indices, UpdateFrequency updateFrequency);
		/// @brief Create a new index buffer and set its size.
		/// @param size Byte size for the index buffer.
		/// @param updateFrequency Update frequency for the buffer.
		/// @return New index buffer.
		[[nodiscard]] static Scope<IndexBuffer> Create(u64 size, UpdateFrequency updateFrequency);

	private:
		/// @brief Initialize index buffer with given data.
		/// @param indices Pointer to the data to upload.
		/// @param size Byte size of the data to upload.
		/// @param updateFrequency Update frequency for the buffer.
		/// @return New index buffer.
		template<typename T>
		requires std::same_as<T, u16> || std::same_as<T, u32>
		[[nodiscard]] static TRAP::Scope<IndexBuffer> Init(const T* indices, u64 size, UpdateFrequency updateFrequency);

		/// @brief Set new index buffer data.
		/// @param indices Pointer to the data to upload.
		/// @param offset Byte offset into the currently used index data.
		template<typename T, usize Size = std::dynamic_extent>
		requires std::same_as<T, u16> || std::same_as<T, u32>
		void SetDataInternal(std::span<const T, Size> indices, u64 offset = 0);

		TRAP::Ref<TRAP::Graphics::Buffer> m_indexBuffer = nullptr;

		API::SyncToken m_token{};

		RendererAPI::IndexType m_indexType;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::IndexBuffer::IndexBuffer(const RendererAPI::IndexType indexType) noexcept
	: m_indexType(indexType)
{
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::same_as<T, u16> || std::same_as<T, u32>
[[nodiscard]] inline TRAP::Scope<TRAP::Graphics::IndexBuffer> TRAP::Graphics::IndexBuffer::Init(const T* const indices,
                                                                                                const u64 size,
                                                                                                const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	RendererAPI::IndexType indexType = {};
	if(indices)
	{
		if constexpr(std::same_as<T, u16>)
			indexType = RendererAPI::IndexType::UInt16;
		else if constexpr(std::same_as<T, u32>)
			indexType = RendererAPI::IndexType::UInt32;
	}
	else
	{
		if(size / sizeof(u16) < std::numeric_limits<u16>::max())
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
	desc.Desc.StructStride = sizeof(T);
	desc.Desc.ElementCount = size / desc.Desc.StructStride;
	desc.Desc.Flags = (updateFrequency != UpdateFrequency::Static) ? RendererAPI::BufferCreationFlags::PersistentMap :
	                                                                 RendererAPI::BufferCreationFlags::None;
	desc.Data = indices;

	RendererAPI::GetResourceLoader()->AddResource(desc, &buffer->m_token);

	buffer->m_indexBuffer = desc.Buffer;

	return buffer;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T, usize Size>
requires std::same_as<T, u16> || std::same_as<T, u32>
void TRAP::Graphics::IndexBuffer::SetDataInternal(const std::span<const T, Size> indices,
                                                  const u64 offset)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	TRAP_ASSERT(!indices.empty(), "IndexBuffer::SetDataInternal(): Indices is empty!");
	TRAP_ASSERT(indices.size_bytes() + offset <= m_indexBuffer->GetSize(), "IndexBuffer::SetDataInternal(): Out of bounds!");

	RendererAPI::BufferUpdateDesc desc{};
	desc.Buffer = m_indexBuffer;
	desc.DstOffset = offset;
	API::ResourceLoader::BeginUpdateResource(desc);
	std::ranges::copy(indices, reinterpret_cast<T*>(desc.MappedData.data()));
	RendererAPI::GetResourceLoader()->EndUpdateResource(desc, &m_token);

	if constexpr(std::same_as<T, u16>)
		m_indexType = RendererAPI::IndexType::UInt16;
	else if constexpr(std::same_as<T, u32>)
		m_indexType = RendererAPI::IndexType::UInt32;
}

#endif /*TRAP_INDEXBUFFER_H*/