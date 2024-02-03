#ifndef TRAP_BUFFER_H
#define TRAP_BUFFER_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Buffer
	{
	public:
		/// @brief Create a new buffer from the given description.
		/// @param desc Buffer description.
		/// @return Created buffer.
		[[nodiscard]] static TRAP::Ref<Buffer> Create(const RendererAPI::BufferDesc& desc);

		/// @brief Destructor.
		virtual ~Buffer();

		/// @brief Copy constructor.
		consteval Buffer(const Buffer&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval Buffer& operator=(const Buffer&) noexcept = delete;
		/// @brief Move constructor.
		constexpr Buffer(Buffer&&) noexcept = default;
		/// @brief Move assignment operator.
		Buffer& operator=(Buffer&&) noexcept = default;

		/// @brief Retrieve the size of the buffer in bytes.
		/// @return Size of the buffer in bytes.
		[[nodiscard]] constexpr u64 GetSize() const noexcept;
		/// @brief Retrieve the type of descriptor.
		/// @return Type of descriptor.
		[[nodiscard]] constexpr RendererAPI::DescriptorType GetDescriptors() const noexcept;
		/// @brief Retrieve the usage of the buffer memory.
		/// @return Usage of the buffer memory.
		[[nodiscard]] constexpr RendererAPI::ResourceMemoryUsage GetMemoryUsage() const noexcept;

		/// @brief Retrieve the CPU mapped memory range of the buffer.
		/// @return CPU mapped memory range of the buffer.
		/// @note MapBuffer must be called before accessing the buffer on the CPU.
		template<typename T = u8>
		[[nodiscard]] constexpr std::span<T> GetCPUMappedAddress() const noexcept;

		/// @brief Map a region of the buffer to the CPU.
		/// @param range Optional range of the buffer to map. Default: Whole buffer.
		/// @return Returns true if the buffer was successfully mapped, false otherwise.
		/// @note MapBuffer must not be called if memory usage is GPU only.
		[[nodiscard]] virtual bool MapBuffer(const RendererAPI::ReadRange& range = {}) = 0;
		/// @brief Unmap CPU mapped memory region.
		/// @note UnMapBuffer must not be called if memory usage is GPU only.
		virtual void UnMapBuffer() = 0;

	protected:
		/// @brief Constructor.
		Buffer(u64 size, RendererAPI::DescriptorType descriptorType, RendererAPI::ResourceMemoryUsage memoryUsage);

		//CPU address of the mapped buffer (applicable to buffers created in CPU accessible heaps
		//(CPU, CPUToGPU, GPUToCPU))
		std::span<u8> m_CPUMappedAddress{};

		u64 m_size = 0;
		RendererAPI::DescriptorType m_descriptors = RendererAPI::DescriptorType::Undefined;
		RendererAPI::ResourceMemoryUsage m_memoryUsage = RendererAPI::ResourceMemoryUsage::Unknown;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u64 TRAP::Graphics::Buffer::GetSize() const noexcept
{
	return m_size;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::Buffer::GetDescriptors() const noexcept
{
	return m_descriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::ResourceMemoryUsage TRAP::Graphics::Buffer::GetMemoryUsage() const noexcept
{
	return m_memoryUsage;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] constexpr std::span<T> TRAP::Graphics::Buffer::GetCPUMappedAddress() const noexcept
{
	TRAP_ASSERT((m_CPUMappedAddress.size() % sizeof(T)) == 0,
	            "Buffer::GetCPUMappedAddress(): CPU mapped buffer length is not divisible with sizeof(T)!");

	return {reinterpret_cast<T*>(m_CPUMappedAddress.data()), m_CPUMappedAddress.size() / sizeof(T)};
}

#endif /*TRAP_BUFFER_H*/
