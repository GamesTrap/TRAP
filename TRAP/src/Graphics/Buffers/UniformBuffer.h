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
		/// @brief Constructor.
		/// @param updateFrequency Update frequency of the uniform buffer.
		constexpr explicit UniformBuffer(RendererAPI::DescriptorUpdateFrequency updateFrequency);
		/// @brief Copy constructor.
		constexpr UniformBuffer(const UniformBuffer&) noexcept = default;
		/// @brief Copy assignment operator.
		constexpr UniformBuffer& operator=(const UniformBuffer&) noexcept = default;
		/// @brief Move constructor.
		constexpr UniformBuffer(UniformBuffer&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr UniformBuffer& operator=(UniformBuffer&&) noexcept = default;

	public:
		/// @brief Destructor.
		constexpr ~UniformBuffer() = default;

		/// @brief Retrieve the byte size of the UBO.
		/// @return Byte size of the UBO.
		[[nodiscard]] u64 GetSize() const noexcept;
		/// @brief Retrieve the update frequency of the UBO.
		/// @return Update frequency of the UBO.
		[[nodiscard]] constexpr UpdateFrequency GetUpdateFrequency() const noexcept;
		/// @brief Retrieve the underlying buffers.
		/// @return Underlying buffers.
		[[nodiscard]] constexpr const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& GetUBOs() const noexcept;

		/// @brief Update data of the UBO.
		/// @param data New data.
		/// @param size Byte size of the data.
		/// @param offset Offset into the currently used data.
		void SetData(const void* data, u64 size, u64 offset = 0);

		/// @brief Check whether uploading data to the GPU has finished.
		/// @return True if uploading data to the GPU has finished.
		[[nodiscard]] bool IsLoaded() const;
		/// @brief Wait until uploading data to the GPU has finished.
		void AwaitLoading() const;

		/// @brief Calculate the aligned size of the UBO.
		/// @param byteSize Byte size of the UBO.
		[[nodiscard]] static u64 CalculateAlignedSize(u64 byteSize) noexcept;

		/// @brief Create a new uniform buffer and set its size.
		/// @param size Byte size for the uniform buffer.
		/// @param updateFrequency Update frequency for the buffer.
		/// @return New uniform buffer.
		[[nodiscard]] static Scope<UniformBuffer> Create(u64 size, UpdateFrequency updateFrequency);
		/// @brief Create a new uniform buffer and set its data.
		/// @param data Pointer to the data to upload.
		/// @param size Byte size of the data to upload.
		/// @param updateFrequency Update frequency for the buffer.
		/// @return New uniform buffer.
		[[nodiscard]] static Scope<UniformBuffer> Create(const void* data, u64 size, UpdateFrequency updateFrequency);

	private:
		/// @brief Initialize uniform buffer with given data.
		/// @param data Pointer to the data to upload.
		/// @param size Byte size of the data to upload.
		/// @param updateFrequency Update frequency for the buffer.
		/// @return New uniform buffer.
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