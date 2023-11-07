#ifndef TRAP_VERTEXBUFFER_H
#define TRAP_VERTEXBUFFER_H

#include "Application.h"
#include "Graphics/RenderCommand.h"
#include "VertexBufferLayout.h"
#include "Graphics/API/ResourceLoader.h"

namespace TRAP::Graphics
{
	class Buffer;

	class VertexBuffer
	{
	protected:
		/// @brief Constructor.
		constexpr VertexBuffer() noexcept = default;
		/// @brief Copy constructor.
		VertexBuffer(const VertexBuffer&) noexcept = default;
		/// @brief Copy assignment operator.
		VertexBuffer& operator=(const VertexBuffer&) noexcept = default;
		/// @brief Move constructor.
		VertexBuffer(VertexBuffer&&) noexcept = default;
		/// @brief Move assignment operator.
		VertexBuffer& operator=(VertexBuffer&&) noexcept = default;

	public:
		/// @brief Destructor.
		~VertexBuffer() = default;

		/// @brief Retrieve the vertex buffer layout of this buffer.
		/// @return Vertex buffer layout.
		[[nodiscard]] constexpr const VertexBufferLayout& GetLayout() const noexcept;
		/// @brief Set the vertex buffer layout to be used by this buffer.
		/// @param layout New vertex buffer layout.
		constexpr void SetLayout(const VertexBufferLayout& layout);
		/// @brief Retrieve the total byte size of the buffer.
		/// @return Total buffer byte size.
		[[nodiscard]] u64 GetSize() const noexcept;
		/// @brief Retrieve the count of vertex elements inside this buffer.
		/// @return Count of vertex elements.
		[[nodiscard]] u32 GetCount() const noexcept;
		/// @brief Retrieve the update frequency used by this buffer.
		/// @return Update frequency.
		[[nodiscard]] UpdateFrequency GetUpdateFrequency() const noexcept;

		/// @brief Update the buffers vertex data.
		/// @param data Pointer to the updated data.
		/// @param offset Byte offset into the currently used vertex data.
		void SetData(std::span<const f32> data, u64 offset = 0);

#ifndef TRAP_HEADLESS_MODE
		/// @brief Use this buffer for rendering on the given window.
		/// @param window Window to use vertex buffer on. Default: Main Window.
		/// @remark @headless This function is not available in headless mode.
		void Use(const Window* window = TRAP::Application::GetWindow()) const;
#else
		/// @brief Use this buffer for rendering.
		/// @remark This function is only available in headless mode.
		void Use() const;
#endif /*TRAP_HEADLESS_MODE*/

		/// @brief Check whether uploading data to the GPU has finished.
		/// @return True if uploading data to the GPU has finished.
		[[nodiscard]] bool IsLoaded() const;
		/// @brief Wait until uploading data to the GPU has finished.
		void AwaitLoading() const;

		/// @brief Create a new vertex buffer and set its data.
		/// @param vertices Pointer to the data to upload.
		/// @param updateFrequency Update frequency for the buffer.
		/// @return New vertex buffer.
		[[nodiscard]] static Scope<VertexBuffer> Create(std::span<const f32> vertices, UpdateFrequency updateFrequency);
		/// @brief Create a new vertex buffer and set its size.
		/// @param size Byte size for the vertex buffer.
		/// @param updateFrequency Update frequency for the buffer.
		/// @return New vertex buffer.
		[[nodiscard]] static Scope<VertexBuffer> Create(u64 size, UpdateFrequency updateFrequency);

	private:
		/// @brief Initialize vertex buffer with given data.
		/// @param vertices Pointer to the data to upload.
		/// @param size Byte size of the data to upload.
		/// @param updateFrequency Update frequency for the buffer.
		/// @return New vertex buffer.
		[[nodiscard]] static Scope<VertexBuffer> Init(const f32* vertices, u64 size, UpdateFrequency updateFrequency);

		TRAP::Ref<TRAP::Graphics::Buffer> m_vertexBuffer = nullptr;

		VertexBufferLayout m_bufferLayout;
		API::SyncToken m_token{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const TRAP::Graphics::VertexBufferLayout& TRAP::Graphics::VertexBuffer::GetLayout() const noexcept
{
	return m_bufferLayout;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::VertexBuffer::SetLayout(const VertexBufferLayout& layout)
{
	m_bufferLayout = layout;
}

#endif /*TRAP_VERTEXBUFFER_H*/