#ifndef TRAP_UNIFORMBUFFER_H
#define TRAP_UNIFORMBUFFER_H

#include "Graphics/API/ResourceLoader.h"
#include "Graphics/RenderCommand.h"

namespace TRAP::Graphics
{
	class UniformBuffer
	{
	protected:
		/// @brief Constructor.
		/// @param storageBuffers Internal uniform buffer objects.
		/// @param syncTokens Synchronization tokens.
		constexpr UniformBuffer(const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& uniformBuffers, const std::vector<API::SyncToken>& syncTokens);

	public:
		/// @brief Move constructor.
		constexpr UniformBuffer(UniformBuffer&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr UniformBuffer& operator=(UniformBuffer&&) noexcept = default;
		/// @brief Copy constructor.
		consteval UniformBuffer(const UniformBuffer&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval UniformBuffer& operator=(const UniformBuffer&) noexcept = delete;

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
		template<typename T>
		[[nodiscard]] static Scope<UniformBuffer> Create(const T* data, u64 size, UpdateFrequency updateFrequency);

	private:
		/// @brief Initialize uniform buffer with given data.
		/// @param data Pointer to the data to upload.
		/// @param size Byte size of the data to upload.
		/// @param updateFrequency Update frequency for the buffer.
		/// @return New uniform buffer.
		template<typename T>
		[[nodiscard]] static Scope<UniformBuffer> Init(const T* data, u64 size, UpdateFrequency updateFrequency);

		std::vector<TRAP::Ref<TRAP::Graphics::Buffer>> m_uniformBuffers;

		std::vector<API::SyncToken> m_tokens;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::UniformBuffer::UniformBuffer(const std::vector<TRAP::Ref<TRAP::Graphics::Buffer>>& uniformBuffers,
                                                       const std::vector<API::SyncToken>& syncTokens)
	: m_uniformBuffers(uniformBuffers), m_tokens(syncTokens)
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

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Create(const T* const data,
																				               const u64 size,
																				               const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return Init(data, size, updateFrequency);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] TRAP::Scope<TRAP::Graphics::UniformBuffer> TRAP::Graphics::UniformBuffer::Init(const T* const data, const u64 size,
																			                 const UpdateFrequency updateFrequency)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	const RendererAPI::BufferDesc bufferDesc
	{
		.Size = size,
		.MemoryUsage = (updateFrequency == UpdateFrequency::Static) ? RendererAPI::ResourceMemoryUsage::GPUOnly :
	                                                                  RendererAPI::ResourceMemoryUsage::CPUToGPU,
		.Flags = (updateFrequency == UpdateFrequency::Static) ? RendererAPI::BufferCreationFlags::None :
																RendererAPI::BufferCreationFlags::PersistentMap,
		.ElementCount = size / sizeof(T),
		.StructStride = sizeof(T),
		.Descriptors = RendererAPI::DescriptorType::UniformBuffer
	};

	RendererAPI::BufferLoadDesc desc
	{
		.Data = data,
		.Desc = bufferDesc
	};

	std::vector<TRAP::Ref<TRAP::Graphics::Buffer>> uniformBuffers((updateFrequency == UpdateFrequency::Static) ? 1 : RendererAPI::ImageCount);
	std::vector<API::SyncToken> syncTokens(uniformBuffers.size());
	for(u32 i = 0; i < uniformBuffers.size(); ++i)
	{
		RendererAPI::GetResourceLoader()->AddResource(desc, &syncTokens[i]);
		uniformBuffers[i] = desc.Buffer;
	}

	return TRAP::Scope<UniformBuffer>(new UniformBuffer(uniformBuffers, syncTokens));
}

#endif /*TRAP_UNIFORMBUFFER_H*/
