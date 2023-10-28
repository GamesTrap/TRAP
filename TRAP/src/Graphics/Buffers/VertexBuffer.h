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
		/// <summary>
		/// Constructor.
		/// </summary>
		constexpr VertexBuffer() noexcept = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		VertexBuffer(const VertexBuffer&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VertexBuffer& operator=(const VertexBuffer&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VertexBuffer(VertexBuffer&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VertexBuffer& operator=(VertexBuffer&&) noexcept = default;

	public:
		/// <summary>
		/// Destructor.
		/// </summary>
		~VertexBuffer() = default;

		/// <summary>
		/// Retrieve the vertex buffer layout of this buffer.
		/// </summary>
		/// <returns>Vertex buffer layout.</returns>
		[[nodiscard]] constexpr const VertexBufferLayout& GetLayout() const noexcept;
		/// <summary>
		/// Set the vertex buffer layout to be used by this buffer.
		/// </summary>
		/// <param name="layout">New vertex buffer layout.</param>
		constexpr void SetLayout(const VertexBufferLayout& layout);
		/// <summary>
		/// Retrieve the total byte size of the buffer.
		/// </summary>
		/// <returns>Total buffer byte size.</returns>
		[[nodiscard]] u64 GetSize() const noexcept;
		/// <summary>
		/// Retrieve the count of vertex elements inside this buffer.
		/// </summary>
		/// <returns>Count of vertex elements.</returns>
		[[nodiscard]] u32 GetCount() const noexcept;
		/// <summary>
		/// Retrieve the update frequency used by this buffer.
		/// </summary>
		/// <returns>Update frequency.</returns>
		[[nodiscard]] UpdateFrequency GetUpdateFrequency() const noexcept;

		/// <summary>
		/// Update the buffers vertex data.
		/// </summary>
		/// <param name="data">Pointer to the updated data.</param>
		/// <param name="offset">Byte offset into the currently used vertex data.</param>
		void SetData(std::span<const f32> data, u64 offset = 0);

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Use this buffer for rendering on the given window.
		/// </summary>
		/// <param name="window">Window to use vertex buffer on. Default: Main Window.</param>
		void Use(const Window* window = TRAP::Application::GetWindow()) const;
#else
		/// <summary>
		/// Use this buffer for rendering.
		/// </summary>
		void Use() const;
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
		/// Create a new vertex buffer and set its data.
		/// </summary>
		/// <param name="vertices">Pointer to the data to upload.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New vertex buffer.</returns>
		[[nodiscard]] static Scope<VertexBuffer> Create(std::span<const f32> vertices, UpdateFrequency updateFrequency);
		/// <summary>
		/// Create a new vertex buffer and set its size.
		/// </summary>
		/// <param name="size">Byte size for the vertex buffer.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New vertex buffer.</returns>
		[[nodiscard]] static Scope<VertexBuffer> Create(u64 size, UpdateFrequency updateFrequency);

	private:
		/// <summary>
		/// Initialize vertex buffer with given data.
		/// </summary>
		/// <param name="vertices">Pointer to the data to upload.</param>
		/// <param name="size">Byte size of the data to upload.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New vertex buffer.</returns>
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