#ifndef TRAP_UNIFORMBUFFER_H
#define TRAP_UNIFORMBUFFER_H

#include "Graphics/API/ResourceLoader.h"
#include "Graphics/RenderCommand.h"

namespace TRAP
{
	class Window;
}

namespace TRAP::Graphics
{
	class UniformBuffer
	{
	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="updateFrequency">Update frequency of the uniform buffer.</param>
		constexpr explicit UniformBuffer(RendererAPI::DescriptorUpdateFrequency updateFrequency);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr UniformBuffer(const UniformBuffer&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr UniformBuffer& operator=(const UniformBuffer&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr UniformBuffer(UniformBuffer&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr UniformBuffer& operator=(UniformBuffer&&) noexcept = default;

	public:
		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~UniformBuffer() = default;

		/// <summary>
		/// Retrieve the byte size of the UBO.
		/// </summary>
		/// <returns>Byte size of the UBO.</returns>
		[[nodiscard]] u64 GetSize() const noexcept;
		/// <summary>
		/// Retrieve the update frequency of the UBO.
		/// </summary>
		/// <returns>Update frequency of the UBO.</returns>
		[[nodiscard]] constexpr UpdateFrequency GetUpdateFrequency() const noexcept;
		/// <summary>
		/// Retrieve the underlying buffers.
		/// </summary>
		/// <returns>Underlying buffers.</returns>
		[[nodiscard]] constexpr const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& GetUBOs() const noexcept;

		/// <summary>
		/// Update data of the UBO.
		/// </summary>
		/// <param name="data">New data.</param>
		/// <param name="size">Byte size of the data.</param>
		/// <param name="offset">Offset into the currently used data.</param>
		void SetData(const void* data, u64 size, u64 offset = 0);

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
		/// Calculate the aligned size of the UBO.
		/// </summary>
		/// <param name="byteSize">Byte size of the UBO.</param>
		[[nodiscard]] static u64 CalculateAlignedSize(u64 byteSize) noexcept;

		/// <summary>
		/// Create a new uniform buffer and set its size.
		/// </summary>
		/// <param name="size">Byte size for the uniform buffer.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New uniform buffer.</returns>
		[[nodiscard]] static Scope<UniformBuffer> Create(u64 size, UpdateFrequency updateFrequency);
		/// <summary>
		/// Create a new uniform buffer and set its data.
		/// </summary>
		/// <param name="data">Pointer to the data to upload.</param>
		/// <param name="size">Byte size of the data to upload.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New uniform buffer.</returns>
		[[nodiscard]] static Scope<UniformBuffer> Create(const void* data, u64 size, UpdateFrequency updateFrequency);

	private:
		/// <summary>
		/// Initialize uniform buffer with given data.
		/// </summary>
		/// <param name="data">Pointer to the data to upload.</param>
		/// <param name="size">Byte size of the data to upload.</param>
		/// <param name="updateFrequency">Update frequency for the buffer.</param>
		/// <returns>New uniform buffer.</returns>
		[[nodiscard]] static Scope<UniformBuffer> Init(const void* data, u64 size, UpdateFrequency updateFrequency);

		std::vector<TRAP::Ref<TRAP::Graphics::Buffer>> m_uniformBuffers;

		std::vector<API::SyncToken> m_tokens;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::UniformBuffer::UniformBuffer(const RendererAPI::DescriptorUpdateFrequency updateFrequency)
	: m_uniformBuffers(updateFrequency == UpdateFrequency::Static ? 1 : RendererAPI::ImageCount),
	  m_tokens(updateFrequency == UpdateFrequency::Static ? 1 : RendererAPI::ImageCount)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::UpdateFrequency TRAP::Graphics::UniformBuffer::GetUpdateFrequency() const noexcept
{
	return m_uniformBuffers.size() == 1 ? UpdateFrequency::Static : UpdateFrequency::Dynamic;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& TRAP::Graphics::UniformBuffer::GetUBOs() const noexcept
{
	return m_uniformBuffers;
}

#endif /*TRAP_UNIFORMBUFFER_H*/