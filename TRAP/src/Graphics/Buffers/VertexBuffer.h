#ifndef TRAP_VERTEXBUFFER_H
#define TRAP_VERTEXBUFFER_H

#include "Application.h"
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
		VertexBuffer() noexcept;
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
		~VertexBuffer();

		/// <summary>
		/// Retrieve the vertex buffer layout of this buffer.
		/// </summary>
		/// <returns>Vertex buffer layout.</returns>
		[[nodiscard]] const VertexBufferLayout& GetLayout() const noexcept;
		/// <summary>
		/// Set the vertex buffer layout to be used by this buffer.
		/// </summary>
		/// <param name="layout">New vertex buffer layout.</param>
		void SetLayout(const VertexBufferLayout& layout);
		/// <summary>
		/// Retrieve the total byte size of the buffer.
		/// </summary>
		/// <returns>Total buffer byte size.</returns>
		[[nodiscard]] uint64_t GetSize() const noexcept;
		/// <summary>
		/// Retrieve the count of vertex elements inside this buffer.
		/// </summary>
		/// <returns>Count of vertex elements.</returns>
		[[nodiscard]] uint32_t GetCount() const noexcept;
		/// <summary>
		/// Retrieve the update frequency used by this buffer.
		/// </summary>
		/// <returns>Update frequency.</returns>
		[[nodiscard]] UpdateFrequency GetUpdateFrequency() const noexcept;

		/// <summary>
		/// Update the buffers vertex data.
		/// </summary>
		/// <param name="data">Pointer to the updated data.</param>
		/// <param name="size">Size of the updated data.</param>
		/// <param name="offset">Byte offset into the currently used vertex data.</param>
		void SetData(const float* data, uint64_t size, uint64_t offset = 0);

		/// <summary>
		/// Use this buffer for rendering on the given window.
		/// </summary>
		/// <param name="window">Window to use vertex buffer on. Default: Main Window.</param>
		void Use(const Window* const window = TRAP::Application::GetWindow()) const;

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
		/// <param name="size">Byte size of the data to upload.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New vertex buffer.</returns>
		[[nodiscard]] static Scope<VertexBuffer> Create(const float* vertices, uint64_t size, UpdateFrequency updateFrequency);
		/// <summary>
		/// Create a new vertex buffer and set its size.
		/// </summary>
		/// <param name="size">Byte size for the vertex buffer.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New vertex buffer.</returns>
		[[nodiscard]] static Scope<VertexBuffer> Create(uint64_t size, UpdateFrequency updateFrequency);

	private:
		/// <summary>
		/// Initialize vertex buffer with given data.
		/// </summary>
		/// <param name="vertices">Pointer to the data to upload.</param>
		/// <param name="size">Byte size of the data to upload.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New vertex buffer.</returns>
		[[nodiscard]] static Scope<VertexBuffer> Init(const float* vertices, uint64_t size, UpdateFrequency updateFrequency);

		TRAP::Ref<TRAP::Graphics::Buffer> m_vertexBuffer;

		VertexBufferLayout m_bufferLayout;
		API::SyncToken m_token;
	};
}

#endif /*TRAP_VERTEXBUFFER_H*/