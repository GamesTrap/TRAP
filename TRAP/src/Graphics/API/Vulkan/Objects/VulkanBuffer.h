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
		/// @brief Constructor.
		/// @param desc Buffer description.
		explicit VulkanBuffer(const RendererAPI::BufferDesc& desc);
		/// @brief Destructor.
		~VulkanBuffer() override;

		/// @brief Copy constructor.
		consteval VulkanBuffer(const VulkanBuffer&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanBuffer& operator=(const VulkanBuffer&) noexcept = delete;
		/// @brief Move constructor.
		VulkanBuffer(VulkanBuffer&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanBuffer& operator=(VulkanBuffer&&) noexcept = default;

		/// @brief Retrieve the VkBuffer handle.
		/// @return VkBuffer handle.
		[[nodiscard]] constexpr VkBuffer GetVkBuffer() const noexcept;
		/// @brief Retrieve the storage texel view.
		/// @return VkBufferView for storage texel.
		[[nodiscard]] constexpr VkBufferView GetStorageTexelView() const noexcept;
		/// @brief Retrieve the uniform texel view.
		/// @return VkBufferView for uniform texel.
		[[nodiscard]] constexpr VkBufferView GetUniformTexelView() const noexcept;
		/// @brief Retrieve the (aligned) byte offset to the first element in the buffer.
		/// @return Byte offset to the first element in the buffer.
		[[nodiscard]] constexpr u64 GetOffset() const noexcept;

		/// @brief Map a region of the buffer to the CPU.
		/// @param range Optional range of the buffer to map. Default: Whole buffer.
		/// @return Returns true if the buffer was successfully mapped, false otherwise.
		/// @note MapBuffer must not be called if memory usage is GPU only.
		[[nodiscard]] bool MapBuffer(const RendererAPI::ReadRange& range = {}) override;
		/// @brief Unmap CPU mapped memory region.
		/// @note UnMapBuffer must not be called if memory usage is GPU only.
		void UnMapBuffer() override;

	private:
		/// @brief Set a name for the buffer.
		/// @param name Name for the buffer.
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

[[nodiscard]] constexpr u64 TRAP::Graphics::API::VulkanBuffer::GetOffset() const noexcept
{
	return 0u;
}

#endif /*TRAP_VULKANBUFFER_H*/
