#ifndef TRAP_VULKANBUFFER_H
#define TRAP_VULKANBUFFER_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Objects/VulkanMemoryAllocator.h"
#include "Graphics/API/Objects/Buffer.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace TRAP::Graphics::API
{
	class VulkanMemoryAllocator;
	class VulkanDevice;

	class VulkanBuffer final : public Buffer
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="desc">Buffer description.</param>
		explicit VulkanBuffer(const RendererAPI::BufferDesc& desc);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanBuffer() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanBuffer(const VulkanBuffer&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanBuffer& operator=(const VulkanBuffer&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanBuffer(VulkanBuffer&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanBuffer& operator=(VulkanBuffer&&) noexcept = default;

		/// <summary>
		/// Retrieve the VkBuffer handle.
		/// </summary>
		/// <returns>VkBuffer handle.</returns>
		[[nodiscard]] constexpr VkBuffer GetVkBuffer() const noexcept;
		/// <summary>
		/// Retrieve the storage texel view.
		/// </summary>
		/// <returns>VkBufferView for storage texel.</returns>
		[[nodiscard]] constexpr VkBufferView GetStorageTexelView() const noexcept;
		/// <summary>
		/// Retrieve the uniform texel view.
		/// </summary>
		/// <returns>VkBufferView for uniform texel.</returns>
		[[nodiscard]] constexpr VkBufferView GetUniformTexelView() const noexcept;
		/// <summary>
		/// Retrieve the byte offset to the first element in the buffer.
		/// </summary>
		/// <returns>Byte offset to the first element in the buffer.</returns>
		[[nodiscard]] constexpr uint64_t GetOffset() const noexcept;
		/// <summary>
		/// Retrieve the VkDeviceMemory handle.
		/// </summary>
		/// <returns>VkDeviceMemory handle.</returns>
		[[nodiscard]] VkDeviceMemory GetVkDeviceMemory() const;
		/// <summary>
		/// Retrieve the VkDeviceMemory offset to the start of the buffer.
		/// </summary>
		/// <returns>VkDeviceMemory offset.</returns>
		[[nodiscard]] uint64_t GetVkDeviceMemoryOffset() const;

		/// <summary>
		/// Map a region of the buffer to the CPU.
		///
		/// Note: MapBuffer must not be called if memory usage is GPU only.
		/// </summary>
		/// <param name="range">Optional range of the buffer to map. Default: Whole buffer.</param>
		void MapBuffer(const RendererAPI::ReadRange* range) override;
		/// <summary>
		/// Unmap CPU mapped memory region.
		///
		/// Note: UnMapBuffer must not be called if memory usage is GPU only.
		/// </summary>
		void UnMapBuffer() override;

	private:
		/// <summary>
		/// Set a name for the buffer.
		/// </summary>
		/// <param name="name">Name for the buffer.</param>
		void SetBufferName(std::string_view name) const;

		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();
		TRAP::Ref<VulkanMemoryAllocator> m_VMA = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetVMA();

		//Native handle of the underlying resource
		VkBuffer m_vkBuffer = VK_NULL_HANDLE;
		//Buffer view
		VkBufferView m_vkStorageTexelView = VK_NULL_HANDLE;
		VkBufferView m_vkUniformTexelView = VK_NULL_HANDLE;
		//Contains resource allocation info such as parent heap, offset in heap
		VmaAllocation m_allocation = VK_NULL_HANDLE;
		uint64_t m_offset = 0;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkBuffer TRAP::Graphics::API::VulkanBuffer::GetVkBuffer() const noexcept
{
	return m_vkBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkBufferView TRAP::Graphics::API::VulkanBuffer::GetStorageTexelView() const noexcept
{
	return m_vkStorageTexelView;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkBufferView TRAP::Graphics::API::VulkanBuffer::GetUniformTexelView() const noexcept
{
	return m_vkUniformTexelView;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint64_t TRAP::Graphics::API::VulkanBuffer::GetOffset() const noexcept
{
	return m_offset;
}

#endif /*TRAP_VULKANBUFFER_H*/