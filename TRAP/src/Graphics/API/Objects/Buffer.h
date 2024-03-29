#ifndef TRAP_BUFFER_H
#define TRAP_BUFFER_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Buffer
	{
	public:
		/// <summary>
		/// Create a new buffer from the given description.
		/// </summary>
		/// <param name="desc">Buffer description.</param>
		/// <returns>Created buffer.</returns>
		[[nodiscard]] static TRAP::Ref<Buffer> Create(const RendererAPI::BufferDesc& desc);

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Buffer();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr Buffer(const Buffer&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Buffer& operator=(const Buffer&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr Buffer(Buffer&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Buffer& operator=(Buffer&&) noexcept = default;

		/// <summary>
		/// Retrieve the size of the buffer in bytes.
		/// </summary>
		/// <returns>Size of the buffer in bytes.</returns>
		[[nodiscard]] constexpr u64 GetSize() const noexcept;
		/// <summary>
		/// Retrieve the type of descriptor.
		/// </summary>
		/// <returns>Type of descriptor.</returns>
		[[nodiscard]] constexpr RendererAPI::DescriptorType GetDescriptors() const noexcept;
		/// <summary>
		/// Retrieve the usage of the buffer memory.
		/// </summary>
		/// <returns>Usage of the buffer memory.</returns>
		[[nodiscard]] constexpr RendererAPI::ResourceMemoryUsage GetMemoryUsage() const noexcept;

		/// <summary>
		/// Retrieve the CPU mapped address of the buffer.
		///
		/// Note: MapBuffer must be called before accessing the buffer on the CPU.
		/// </summary>
		/// <returns>CPU mapped address of the buffer.</returns>
		[[nodiscard]] constexpr void* GetCPUMappedAddress() const noexcept;

		/// <summary>
		/// Map a region of the buffer to the CPU.
		///
		/// Note: MapBuffer must not be called if memory usage is GPU only.
		/// </summary>
		/// <param name="range">Optional range of the buffer to map. Default: Whole buffer.</param>
		virtual void MapBuffer(const RendererAPI::ReadRange* range) = 0;
		/// <summary>
		/// Unmap CPU mapped memory region.
		///
		/// Note: UnMapBuffer must not be called if memory usage is GPU only.
		/// </summary>
		virtual void UnMapBuffer() = 0;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		Buffer(u64 size, RendererAPI::DescriptorType descriptorType, RendererAPI::ResourceMemoryUsage memoryUsage);

		//CPU address of the mapped buffer (applicable to buffers created in CPU accessible heaps
		//(CPU, CPUToGPU, GPUToCPU))
		void* m_CPUMappedAddress = nullptr;

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

[[nodiscard]] constexpr void* TRAP::Graphics::Buffer::GetCPUMappedAddress() const noexcept
{
	return m_CPUMappedAddress;
}

#endif /*TRAP_BUFFER_H*/