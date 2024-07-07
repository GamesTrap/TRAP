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
		/// @param syncToken Synchronization token of the index buffer.
		/// @param indexBuffer Interal index buffer object.
		IndexBuffer(RendererAPI::IndexType indexType, API::SyncToken syncToken, const TRAP::Ref<TRAP::Graphics::Buffer>& indexBuffer) noexcept;

	public:
		/// @brief Move constructor.
		IndexBuffer(IndexBuffer&&) noexcept = default;
		/// @brief Move assignment operator.
		IndexBuffer& operator=(IndexBuffer&&) noexcept = default;
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
		void Use(const Window& window = *TRAP::Application::GetWindow()) const;
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

		TRAP::Ref<TRAP::Graphics::Buffer> m_indexBuffer = nullptr;

		API::SyncToken m_token{};

		RendererAPI::IndexType m_indexType;
	};
}

#endif /*TRAP_INDEXBUFFER_H*/
