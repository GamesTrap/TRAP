#ifndef TRAP_VULKANINITS_H
#define TRAP_VULKANINITS_H

#include "VulkanMemoryAllocator.h"
#include <vulkan/vulkan_core.h>

#include "Graphics/API/ImageFormat.h"
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API::VulkanInits
{
	/// <summary>
	/// Create a Vulkan application info from app name.
	/// </summary>
	/// <param name="appName">Application name.</param>
	/// <returns>VkApplicationInfo.</returns>
	[[nodiscard]] VkApplicationInfo ApplicationInfo(const std::string_view appName);

	/// <summary>
	/// Create a Vulkan instance create info from app info, instance layers and extensions.
	/// </summary>
	/// <param name="appInfo">Vulkan application info.</param>
	/// <param name="instanceLayers">Instance layers to use.</param>
	/// <param name="instanceExtensions">Instance extensions to use.</param>
	/// <returns>VkInstanceCreateInfo.</returns>
	[[nodiscard]] VkInstanceCreateInfo InstanceCreateInfo(const VkApplicationInfo& appInfo,
									                      const std::vector<const char*>& instanceLayers,
	                                                      const std::vector<const char*>& instanceExtensions) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan debug utils messenger create info.
	/// </summary>
	/// <param name="callback">Debug callback.</param>
	/// <returns>VkDebugUtilsMessengerCreateInfoEXT.</returns>
	[[nodiscard]] VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo(PFN_vkDebugUtilsMessengerCallbackEXT callback);

	/// <summary>
	/// Create a Vulkan debug report callback create info.
	/// </summary>
	/// <param name="callback">Debug callback.</param>
	/// <returns>VkDebugReportCallbackCreateInfoEXT.</returns>
	[[nodiscard]] VkDebugReportCallbackCreateInfoEXT DebugReportCallbackCreateInfo(PFN_vkDebugReportCallbackEXT callback);

	/// <summary>
	/// Create a Vulkan debug utils object name info.
	/// </summary>
	/// <param name="type">Vulkan object type.</param>
	/// <param name="handle">Vulkan handle.</param>
	/// <param name="name">Debug object name to set.</param>
	/// <returns>VkDebugUtilsObjectNameInfoEXT.</returns>
	[[nodiscard]] VkDebugUtilsObjectNameInfoEXT DebugUtilsObjectNameInfo(VkObjectType type, uint64_t handle,
	                                                                     const std::string_view name);

	/// <summary>
	/// Create a Vulkan debug marker object name info.
	/// </summary>
	/// <param name="type">Vulkan debug report object type.</param>
	/// <param name="handle">Vulkan handle.</param>
	/// <param name="name">Debug object name to set.</param>
	/// <returns>VkDebugMarkerObjectNameInfoEXT.</returns>
	[[nodiscard]] VkDebugMarkerObjectNameInfoEXT DebugMarkerObjectNameInfo(VkDebugReportObjectTypeEXT type, uint64_t handle,
	                                                                       const std::string_view name);

	/// <summary>
	/// Create a Vulkan debug utils label.
	/// </summary>
	/// <param name="r">Red color.</param>
	/// <param name="g">Green color.</param>
	/// <param name="b">Blue color.</param>
	/// <param name="name">Name for the label.</param>
	/// <returns>VkDebugUtilsLabelEXT.</returns>
	[[nodiscard]] VkDebugUtilsLabelEXT DebugUtilsLabelExt(float r, float g, float b, std::string_view name);

	/// <summary>
	/// Create a Vulkan debug marker.
	/// </summary>
	/// <param name="r">Red color.</param>
	/// <param name="g">Green color.</param>
	/// <param name="b">Blue color.</param>
	/// <param name="name">Name for the label.</param>
	/// <returns>VkDebugMarkerMarkerInfoEXT.</returns>
	[[nodiscard]] VkDebugMarkerMarkerInfoEXT DebugMarkerMarkerInfo(float r, float g, float b, std::string_view name);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a logical Vulkan device create info.
	/// </summary>
	/// <param name="pNext">Optional next pointer.</param>
	/// <param name="queueCreateInfos">Vulkan device queue create infos.</param>
	/// <param name="deviceExtensions">Device extensions to use.</param>
	/// <returns>VkDeviceCreateInfo.</returns>
	[[nodiscard]] VkDeviceCreateInfo DeviceCreateInfo(const void* pNext,
	                                                  const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
	                                                  const std::vector<const char*>& deviceExtensions);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan memory allocator (VMA) allocator create info.
	/// </summary>
	/// <param name="device">Logical Vulkan device.</param>
	/// <param name="physicalDevice">Physical Vulkan device.</param>
	/// <param name="instance">Vulkan instance.</param>
	/// <param name="vulkanFunctions">VMA Vulkan functions.</param>
	/// <returns>VmaAllocatorCreateInfo.</returns>
	[[nodiscard]] VmaAllocatorCreateInfo VMAAllocatorCreateInfo(VkDevice device, VkPhysicalDevice physicalDevice,
	                                                            VkInstance instance, const VmaVulkanFunctions& vulkanFunctions);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan descriptor pool create info.
	/// </summary>
	/// <param name="descriptorPoolSizes">Descriptor pool sizes to allocate by the pool.</param>
	/// <param name="numDescriptorSets">Max number of descriptor sets which should be allocated from the pool.</param>
	/// <returns>VkDescriptorPoolCreateInfo.</returns>
	[[nodiscard]] VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo(const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
	                                                                  uint32_t numDescriptorSets);

	/// <summary>
	/// Create a Vulkan descriptor set allocation info.
	/// </summary>
	/// <param name="descriptorPool">Vulkan descriptor pool to use.</param>
	/// <param name="descriptorLayout">Vulkan descriptor set layout to use.</param>
	/// <returns>VkDescriptorSetAllocateInfo.</returns>
	[[nodiscard]] VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo(VkDescriptorPool descriptorPool,
														                const VkDescriptorSetLayout& descriptorLayout);

	/// <summary>
	/// Create a Vulkan descriptor set layout create info.
	/// </summary>
	/// <param name="bindings">List of Vulkan descriptor set layout bindings.</param>
	/// <returns>VkDescriptorSetLayoutCreateInfo.</returns>
	[[nodiscard]] VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(const std::vector<VkDescriptorSetLayoutBinding>& bindings) noexcept;

	/// <summary>
	/// Create a Vulkan descriptor update template create info.
	/// </summary>
	/// <param name="descriptorSetLayout">Vulkan descriptor set layout to use.</param>
	/// <param name="entryCount">Number of entries in the descriptor update template.</param>
	/// <param name="entries">Vulkan descriptor update template entries.</param>
	/// <param name="bindPoint">Vulkan pipeline bind point.</param>
	/// <param name="pipelineLayout">Vulkan pipeline layout to use.</param>
	/// <param name="setIndex">Set index of the Descriptor set.</param>
	/// <returns>VkDescriptorUpdateTemplateCreateInfo.</returns>
	[[nodiscard]] VkDescriptorUpdateTemplateCreateInfo DescriptorUpdateTemplateCreateInfo(VkDescriptorSetLayout descriptorSetLayout,
		                                                                                  uint32_t entryCount,
		                                                                                  const VkDescriptorUpdateTemplateEntry* const entries,
		                                                                                  VkPipelineBindPoint bindPoint,
		                                                                                  VkPipelineLayout pipelineLayout,
		                                                                                  uint32_t setIndex);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan fence create info.
	/// </summary>
	/// <returns>VkFenceCreateInfo.</returns>
	[[nodiscard]] VkFenceCreateInfo FenceCreateInfo() noexcept;

	/// <summary>
	/// Create a Vulkan semaphore create info.
	/// </summary>
	/// <returns>VkSemaphoreCreateInfo.</returns>
	[[nodiscard]] VkSemaphoreCreateInfo SemaphoreCreateInfo() noexcept;

	/// <summary>
	/// Create a Vulkan semaphore wait info.
	/// </summary>
	/// <param name="semaphore">Semaphore to wait on.</param>
	/// <param name="value">Signal value to set.</param>
	/// <returns>VkSemaphoreWaitInfoKHR</returns>
	[[nodiscard]] VkSemaphoreWaitInfoKHR SemaphoreWaitInfo(VkSemaphore& semaphore, uint64_t& value);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan fragment shading rate attachment info.
	/// </summary>
	/// <param name="shadingRateAttachment">Shading rate attachment.</param>
	/// <param name="texelSize">Size of the portion of the framebuffer corresponding to each texel.</param>
	/// <returns>VkFragmentShadingRateAttachmentInfoKHR.</returns>
	[[nodiscard]] VkFragmentShadingRateAttachmentInfoKHR FragmentShadingRateAttachmentInfo(VkAttachmentReference2KHR& shadingRateAttachment,
	                                                                                       VkExtent2D texelSize);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan attachment description.
	/// </summary>
	/// <param name="format">Vulkan format.</param>
	/// <param name="sampleCount">Number of samples.</param>
	/// <param name="loadOp">Vulkan Attachment load operation.</param>
	/// <param name="storeOp">Vulkan Attachment store operation.</param>
	/// <param name="stencilLoadOp">Vulkan Attachment stencil load operation.</param>
	/// <param name="stencilStoreOp">Vulkan Attachment stencil store operation.</param>
	/// <param name="layout">Initial Vulkan image layout used when render pass starts.</param>
	/// <param name="finalLayout">Final Vulkan image layout to transition to when render pass ends.</param>
	/// <returns>VkAttachmentDescription.</returns>
	[[nodiscard]] VkAttachmentDescription AttachmentDescription(VkFormat format, VkSampleCountFlagBits sampleCount,
	                                                            VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
	                                                            VkAttachmentLoadOp stencilLoadOp,
												                VkAttachmentStoreOp stencilStoreOp,
	                                                            VkImageLayout layout, VkImageLayout finalLayout);

	/// <summary>
	/// Create a Vulkan attachment description.
	/// </summary>
	/// <param name="format">Vulkan format.</param>
	/// <param name="sampleCount">Number of samples.</param>
	/// <param name="loadOp">Vulkan Attachment load operation.</param>
	/// <param name="storeOp">Vulkan Attachment store operation.</param>
	/// <param name="stencilLoadOp">Vulkan Attachment stencil load operation.</param>
	/// <param name="stencilStoreOp">Vulkan Attachment stencil store operation.</param>
	/// <param name="layout">Initial Vulkan image layout used when render pass starts.</param>
	/// <param name="finalLayout">Final Vulkan image layout to transition to when render pass ends.</param>
	/// <returns>VkAttachmentDescription2KHR.</returns>
	[[nodiscard]] VkAttachmentDescription2KHR AttachmentDescription2(VkFormat format, VkSampleCountFlagBits sampleCount,
	                                                                 VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
	                                                                 VkAttachmentLoadOp stencilLoadOp,
												                     VkAttachmentStoreOp stencilStoreOp,
	                                                                 VkImageLayout layout, VkImageLayout finalLayout);

	/// <summary>
	/// Create a Vulkan subpass description.
	/// </summary>
	/// <param name="bindPoint">Vulkan pipeline bind point.</param>
	/// <param name="inputAttachments">Vulkan input attachments.</param>
	/// <param name="colorAttachments">Vulkan color attachments.</param>
	/// <param name="depthStencilAttachments">Vulkan depth stencil attachments.</param>
	/// <returns>VkSubpassDescription.</returns>
	[[nodiscard]] VkSubpassDescription SubPassDescription(VkPipelineBindPoint bindPoint,
	                                                      const std::vector<VkAttachmentReference>& inputAttachments,
		                                                  const std::vector<VkAttachmentReference>& colorAttachments,
		                                                  VkAttachmentReference& depthStencilAttachment) noexcept;

	/// <summary>
	/// Create a Vulkan subpass description.
	/// </summary>
	/// <param name="bindPoint">Vulkan pipeline bind point.</param>
	/// <param name="inputAttachments">Vulkan input attachments.</param>
	/// <param name="colorAttachments">Vulkan color attachments.</param>
	/// <returns>VkSubpassDescription.</returns>
	[[nodiscard]] VkSubpassDescription SubPassDescription(VkPipelineBindPoint bindPoint,
		                                                  const std::vector<VkAttachmentReference>& inputAttachments,
		                                                  const std::vector<VkAttachmentReference>& colorAttachments) noexcept;

	/// <summary>
	/// Create a Vulkan subpass description.
	/// </summary>
	/// <param name="bindPoint">Vulkan pipeline bind point.</param>
	/// <param name="inputAttachments">Vulkan input attachments.</param>
	/// <param name="colorAttachments">Vulkan color attachments.</param>
	/// <param name="depthStencilAttachments">Vulkan depth stencil attachments.</param>
	/// <returns>VkSubpassDescription2KHR.</returns>
	[[nodiscard]] VkSubpassDescription2KHR SubPassDescription(VkPipelineBindPoint bindPoint,
	                                                          const std::vector<VkAttachmentReference2KHR>& inputAttachments,
		                                                      const std::vector<VkAttachmentReference2KHR>& colorAttachments,
		                                                      VkAttachmentReference2KHR& depthStencilAttachment,
												              const VkFragmentShadingRateAttachmentInfoKHR* const shadingRateAttachment) noexcept;

	/// <summary>
	/// Create a Vulkan subpass description.
	/// </summary>
	/// <param name="bindPoint">Vulkan pipeline bind point.</param>
	/// <param name="inputAttachments">Vulkan input attachments.</param>
	/// <param name="colorAttachments">Vulkan color attachments.</param>
	/// <returns>VkSubpassDescription2KHR.</returns>
	[[nodiscard]] VkSubpassDescription2KHR SubPassDescription(VkPipelineBindPoint bindPoint,
		                                                      const std::vector<VkAttachmentReference2KHR>& inputAttachments,
		                                                      const std::vector<VkAttachmentReference2KHR>& colorAttachments,
												              const VkFragmentShadingRateAttachmentInfoKHR* const shadingRateAttachment) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan render pass create info.
	/// </summary>
	/// <param name="attachmentDescriptions">List of Vulkan attachment descriptions.</param>
	/// <param name="subpassDescription">Vulkan subpass description.</param>
	/// <returns>VkRenderPassCreateInfo.</returns>
	[[nodiscard]] VkRenderPassCreateInfo RenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachmentDescriptions,
		                                                      const VkSubpassDescription& subpassDescription) noexcept;

	/// <summary>
	/// Create a Vulkan render pass create info.
	/// </summary>
	/// <param name="attachmentDescriptions">List of Vulkan attachment descriptions.</param>
	/// <returns>VkRenderPassCreateInfo.</returns>
	[[nodiscard]] VkRenderPassCreateInfo RenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachmentDescriptions) noexcept;

	/// <summary>
	/// Create a Vulkan render pass create info.
	/// </summary>
	/// <param name="attachmentDescriptions">List of Vulkan attachment descriptions.</param>
	/// <param name="subpassDescription">Vulkan subpass description.</param>
	/// <returns>VkRenderPassCreateInfo2KHR.</returns>
	[[nodiscard]] VkRenderPassCreateInfo2KHR RenderPassCreateInfo(const std::vector<VkAttachmentDescription2KHR>& attachmentDescriptions,
		                                                          const VkSubpassDescription2KHR& subpassDescription) noexcept;

	/// <summary>
	/// Create a Vulkan render pass create info.
	/// </summary>
	/// <param name="attachmentDescriptions">List of Vulkan attachment descriptions.</param>
	/// <returns>VkRenderPassCreateInfo2KHR.</returns>
	[[nodiscard]] VkRenderPassCreateInfo2KHR RenderPassCreateInfo(const std::vector<VkAttachmentDescription2KHR>& attachmentDescriptions) noexcept;

	/// <summary>
	/// Create a Vulkan render pass begin info.
	/// </summary>
	/// <param name="renderPass">Vulkan render pass to use.</param>
	/// <param name="frameBuffer">Vulkan frame buffer to use.</param>
	/// <param name="renderArea">Vulkan render area.</param>
	/// <param name="colorValues">List of Vulkan clear values for color attachments.</param>
	/// <returns>VkRenderPassBeginInfo.</returns>
	[[nodiscard]] VkRenderPassBeginInfo RenderPassBeginInfo(VkRenderPass renderPass, VkFramebuffer frameBuffer,
	                                                        VkRect2D renderArea, const std::vector<VkClearValue>& colorValues);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan framebuffer create info.
	/// </summary>
	/// <param name="renderPass">Vulkan render pass to use.</param>
	/// <param name="attachments">List of Vulkan image views.</param>
	/// <param name="width">Width of the frame buffer.</param>
	/// <param name="height">Height of the frame buffer.</param>
	/// <param name="layerCount">Number of layers in the frame buffer.</param>
	/// <returns>VkFramebufferCreateInfo.</returns>
	[[nodiscard]] VkFramebufferCreateInfo FramebufferCreateInfo(VkRenderPass renderPass,
	                                                            const std::vector<VkImageView>& attachments, uint32_t width,
		                                                        uint32_t height, uint32_t layerCount);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan image view create info.
	/// </summary>
	/// <param name="image">Vulkan image to use.</param>
	/// <param name="imageViewType">Vulkan image view type.</param>
	/// <param name="format">Vulkan image format.</param>
	/// <param name="levelCount">Number of mip levels.</param>
	/// <param name="layerCount">Number of layers.</param>
	/// <returns>VkImageViewCreateInfo.</returns>
	[[nodiscard]] VkImageViewCreateInfo ImageViewCreateInfo(VkImage image, VkImageViewType imageViewType, VkFormat format,
		                                                    uint32_t levelCount, uint32_t layerCount);

	/// <summary>
	/// Create a Vulkan image create info.
	/// </summary>
	/// <param name="imageType">Vulkan image type.</param>
	/// <param name="imageFormat">Vulkan image format.</param>
	/// <param name="width">Width of the image.</param>
	/// <param name="height">Height of the image.</param>
	/// <param name="depth">Depth of the image.</param>
	/// <param name="mipLevels">Number of mip levels.</param>
	/// <param name="arrayLayers">Number of array layers.</param>
	/// <param name="sampleCount">Vulkan sample count.</param>
	/// <param name="tiling">Vulkan image tiling.</param>
	/// <param name="usage">Vulkan image usage.</param>
	/// <returns>VkImageCreateInfo.</returns>
	[[nodiscard]] VkImageCreateInfo ImageCreateInfo(VkImageType imageType, VkFormat imageFormat, uint32_t width, uint32_t height,
		                                            uint32_t depth, uint32_t mipLevels, uint32_t arrayLayers,
		                                            VkSampleCountFlagBits sampleCount, VkImageTiling tiling,
									                VkImageUsageFlags usage);

	/// <summary>
	/// Create a Vulkan buffer image copy.
	/// </summary>
	/// <param name="bufferRowLength">Row length of the buffer.</param>
	/// <param name="width">Width of the image.</param>
	/// <param name="height">Height of the image.</param>
	/// <param name="depth">Depth of the image.</param>
	/// <param name="layers">Number of Vulkan image subresource layers.</param>
	[[nodiscard]] VkBufferImageCopy ImageCopy(uint32_t bufferRowLength, uint32_t width, uint32_t height, uint32_t depth,
							                  VkImageSubresourceLayers layers);

	/// <summary>
	/// Create a Vulkan buffer image copy.
	/// </summary>
	/// <param name="bufferOffset">Start offset of the buffer.</param>
	/// <param name="bufferRowLength">Row length of the buffer.</param>
	/// <param name="bufferImageHeight">Image height of the buffer.</param>
	/// <param name="width">Width of the image.</param>
	/// <param name="height">Height of the image.</param>
	/// <param name="depth">Depth of the image.</param>
	/// <param name="layers">Number of Vulkan image subresource layers.</param>
	[[nodiscard]] VkBufferImageCopy ImageCopy(uint64_t bufferOffset, uint32_t bufferRowLength, uint32_t bufferImageHeight,
							                  uint32_t width, uint32_t height, uint32_t depth, VkImageSubresourceLayers layers);


	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan buffer create info.
	/// </summary>
	/// <param name="allocationSize">Size of the buffer.</param>
	/// <param name="usageFlags">Vulkan buffer usage flags.</param>
	/// <returns>VkBufferCreateInfo.</returns>
	[[nodiscard]] VkBufferCreateInfo BufferCreateInfo(uint64_t allocationSize, VkBufferUsageFlags usageFlags);

	/// <summary>
	/// Create a Vulkan buffer view create info.
	/// </summary>
	/// <param name="buffer">Vulkan buffer to use.</param>
	/// <param name="format">Vulkan buffer format.</param>
	/// <param name="offset">Offset of the buffer.</param>
	/// <param name="range">Range of the buffer.</param>
	/// <returns>VkBufferViewCreateInfo.</returns>
	[[nodiscard]] VkBufferViewCreateInfo BufferViewCreateInfo(VkBuffer buffer, VkFormat format, uint64_t offset, uint64_t range);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan command pool create info.
	/// </summary>
	/// <param name="queueFamilyIndex">Queue family index.</param>
	/// <returns>VkCommandPoolCreateInfo.</returns>
	[[nodiscard]] VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queueFamilyIndex) noexcept;

	/// <summary>
	/// Create a Vulkan command buffer allocate info.
	/// </summary>
	/// <param name="commandPool">Vulkan command pool to use.</param>
	/// <param name="secondary">True to allocate as a secondary command buffer.</param>
	/// <returns>VkCommandBufferAllocateInfo.</returns>
	[[nodiscard]] VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool commandPool, bool secondary);

	/// <summary>
	/// Create a Vulkan command buffer begin info.
	/// </summary>
	/// <returns>VkCommandBufferBeginInfo.</returns>
	[[nodiscard]] VkCommandBufferBeginInfo CommandBufferBeginInfo() noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan sampler create info.
	/// </summary>
	/// <param name="magFilter">Vulkan magnification filter.</param>
	/// <param name="minFilter">Vulkan minification filter.</param>
	/// <param name="mipMapMode">Vulkan mip map mode.</param>
	/// <param name="u">Vulkan sampler address mode.</param>
	/// <param name="v">Vulkan sampler address mode.</param>
	/// <param name="w">Vulkan sampler address mode.</param>
	/// <param name="mipLodBias">Vulkan mip map lod bias.</param>
	/// <param name="minLod">Minimum lod.</param>
	/// <param name="maxLod">Maximum lod.</param>
	/// <param name="maxAnisotropy">Vulkan max anisotropy.</param>
	/// <param name="compareOp">Vulkan compare op.</param>
	/// <returns>VkSamplerCreateInfo.</returns>
	[[nodiscard]] VkSamplerCreateInfo SamplerCreateInfo(VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipMapMode,
		                                                VkSamplerAddressMode u, VkSamplerAddressMode v, VkSamplerAddressMode w,
		                                                float mipLodBias, float minLod, float maxLod, float maxAnisotropy,
										                VkCompareOp compareOp);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan pipeline layout create info.
	/// </summary>
	/// <param name="layoutCount">Number of descriptor set layouts.</param>
	/// <param name="layouts">Vulkan descriptor set layouts.</param>
	/// <param name="pushConstantRangeCount">Number of push constant ranges.</param>
	/// <param name="pushConstants">Vulkan push constant ranges.</param>
	/// <returns>VkPipelineLayoutCreateInfo.</returns>
	[[nodiscard]] VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo(uint32_t layoutCount, const VkDescriptorSetLayout* const layouts,
	                                                                  uint32_t pushConstantRangeCount,
														              const VkPushConstantRange* const pushConstants) noexcept;

	/// <summary>
	/// Create a Vulkan pipeline color blend state create info.
	/// </summary>
	/// <param name="attachments">Vulkan pipeline color blend attachment states.</param>
	/// <returns>VkPipelineColorBlendStateCreateInfo.</returns>
	[[nodiscard]] VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& attachments) noexcept;

	/// <summary>
	/// Create a Vulkan pipeline color blend state create info.
	/// </summary>
	/// <param name="logicOp">Vulkan pipeline logic op.</param>
	/// <param name="attachments">Vulkan pipeline color blend attachment states.</param>
	/// <param name="blendConstR">Blend constant red color.</param>
	/// <param name="blendConstG">Blend constant green color.</param>
	/// <param name="blendConstB">Blend constant blue color.</param>
	/// <param name="blendConstA">Blend constant alpha value.</param>
	/// <returns>VkPipelineColorBlendStateCreateInfo.</returns>
	[[nodiscard]] VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(VkLogicOp logicOp,
		                                                                                const std::vector<VkPipelineColorBlendAttachmentState>& attachments,
		                                                                                float blendConstR, float blendConstG,
		                                                                                float blendConstB, float blendConstA) noexcept;

	/// <summary>
	/// Create a Vulkan pipeline cache create info.
	/// </summary>
	/// <param name="data">Vulkan pipeline cache data.</param>
	/// <param name="flags">Vulkan pipeline cache create flags.</param>
	/// <returns>VkPipelineCacheCreateInfo.</returns>
	[[nodiscard]] VkPipelineCacheCreateInfo PipelineCacheCreateInfo(const std::vector<uint8_t>& data,
	                                                                VkPipelineCacheCreateFlags flags) noexcept;

	/// <summary>
	/// Create a Vulkan pipeline shader stage create info.
	/// </summary>
	/// <param name="stage">Vulkan shader stage.</param>
	/// <param name="module">Vulkan shader module.</param>
	/// <param name="name">Shader entry point name.</param>
	/// <returns>VkPipelineShaderStageCreateInfo.</returns>
	[[nodiscard]] VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module,
	                                                                            std::string_view name);

	/// <summary>
	/// Create a Vulkan pipeline vertex input state create info.
	/// </summary>
	/// <param name="inputBindingCount">Number of vertex input bindings.</param>
	/// <param name="inputBindings">Vulkan vertex input binding descrptions.</param>
	/// <param name="inputAttributeCount">Number of vertex input attributes.</param>
	/// <param name="inputAttributes">Vulkan vertex input attribute descriptions.</param>
	/// <returns>VkPipelineVertexInputStateCreateInfo.</returns>
	[[nodiscard]] VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo(uint32_t inputBindingCount,
		                                                                                  const VkVertexInputBindingDescription* const inputBindings,
		                                                                                  uint32_t inputAttributeCount,
		                                                                                  const VkVertexInputAttributeDescription* const inputAttributes) noexcept;

	/// <summary>
	/// Create a Vulkan pipeline input assembly state create info.
	/// </summary>
	/// <param name="topology">Vulkan primitive topology.</param>
	/// <param name="primitiveRestart">True to enable primitive restart.</param>
	/// <returns>VkPipelineInputAssemblyStateCreateInfo.</returns>
	[[nodiscard]] VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology,
	                                                                                          bool primitiveRestart) noexcept;

	/// <summary>
	/// Create a Vulkan pipeline tessellation state create info.
	/// </summary>
	/// <param name="patchControlPoints">Number of patch control points.</param>
	/// <returns>VkPipelineTessellationStateCreateInfo.</returns>
	[[nodiscard]] VkPipelineTessellationStateCreateInfo PipelineTessellationStateCreateInfo(uint32_t patchControlPoints) noexcept;

	/// <summary>
	/// Create a Vulkan pipeline viewport state create info.
	/// </summary>
	/// <returns>VkPipelineViewportStateCreateInfo.</returns>
	[[nodiscard]] VkPipelineViewportStateCreateInfo PipelineViewportStateCreateInfo() noexcept;

	/// <summary>
	/// Create a Vulkan pipeline multisample state create info.
	/// </summary>
	/// <param name="sampleCount">Number of samples.</param>
	/// <param name="sampleShading">True to enable sample shading.</param>
	/// <param name="sampleShadingRate">Rate used for sample shading if enabled.</param>
	/// <returns>VkPipelineMultisampleStateCreateInfo.</returns>
	[[nodiscard]] VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo(VkSampleCountFlagBits sampleCount,
	                                                                                      bool sampleShading,
																			              float sampleShadingRate) noexcept;

	/// <summary>
	/// Create a Vulkan fragment shading rate state create info.
	/// </summary>
	/// <param name="fragmentSize">Shading rate/Fragment size to use.</param>
	/// <param name="rateCombiners">Shading rate combiners.</param>
	/// <returns>VkPipelineFragmentShadingRateStateCreateInfoKHR</returns>
	[[nodiscard]] VkPipelineFragmentShadingRateStateCreateInfoKHR PipelineFragmentShadingRateStateCreateInfo(VkExtent2D fragmentSize,
	                                                                                                         const std::array<VkFragmentShadingRateCombinerOpKHR, 2>& rateCombiners) noexcept;

	/// <summary>
	/// Create a Vulkan pipeline dynamic state create info.
	/// </summary>
	/// <param name="dynamicStates">Vulkan dynamic state to use.</param>
	/// <returns>VkPipelineDynamicStateCreateInfo.</returns>
	[[nodiscard]] VkPipelineDynamicStateCreateInfo PipelineDynamicStateCreateInfo(const std::vector<VkDynamicState>& dynamicStates) noexcept;

	/// <summary>
	/// Create a Vulkan compute pipeline create info.
	/// </summary>
	/// <param name="stage">Vulkan pipeline shader stage create info.</param>
	/// <param name="layout">Vulkan pipeline layout.</param>
	/// <returns>VkComputePipelineCreateInfo.</returns>
	[[nodiscard]] VkComputePipelineCreateInfo ComputePipelineCreateInfo(const VkPipelineShaderStageCreateInfo& stage,
	                                                                    VkPipelineLayout layout);

	/// <summary>
	/// Create a Vulkan graphics pipeline create info.
	/// </summary>
	/// <param name="stageCount">Number of shader stages.</param>
	/// <param name="stages">Pointer to Vulkan pipeline shader stage create info(s).</param>
	/// <param name="vi">Vulkan pipeline vertex input state create info.</param>
	/// <param name="ia">Vulkan pipeline input assembly state create info.</param>
	/// <param name="vs">Vulkan pipeline viewport state create info.</param>
	/// <param name="rs">Vulkan pipeline rasterization state create info.</param>
	/// <param name="ms">Vulkan pipeline multisample state create info.</param>
	/// <param name="ds">Vulkan pipeline depth stencil state create info.</param>
	/// <param name="cb">Vulkan pipeline color blend state create info.</param>
	/// <param name="dy">Vulkan pipeline dynamic state create info.</param>
	/// <param name="layout">Vulkan pipeline layout.</param>
	/// <param name="renderPass">Vulkan render pass.</param>
	/// <returns>VkGraphicsPipelineCreateInfo.</returns>
	[[nodiscard]] VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo(uint32_t stageCount,
															              const VkPipelineShaderStageCreateInfo* const stages,
															              const VkPipelineVertexInputStateCreateInfo& vi,
															              const VkPipelineInputAssemblyStateCreateInfo& ia,
															              const VkPipelineViewportStateCreateInfo& vs,
															              const VkPipelineRasterizationStateCreateInfo& rs,
															              const VkPipelineMultisampleStateCreateInfo& ms,
															              const VkPipelineDepthStencilStateCreateInfo& ds,
															              const VkPipelineColorBlendStateCreateInfo& cb,
															              const VkPipelineDynamicStateCreateInfo& dy,
															              VkPipelineLayout layout, VkRenderPass renderPass
	) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan submit info.
	/// </summary>
	/// <param name="waitSemaphores">Vulkan semaphore(s) to wait on.</param>
	/// <param name="waitMasks">Vulkan pipeline stage(s) to wait on.</param>
	/// <param name="cmds">Vulkan command buffer(s) to submit.</param>
	/// <param name="signalSemaphore">Vulkan semaphore to signal.</param>
	/// <returns>VkSubmitInfo.</returns>
	[[nodiscard]] VkSubmitInfo SubmitInfo(const std::vector<VkSemaphore>& waitSemaphores,
							              uint32_t waitCount,
	                                      const std::vector<VkPipelineStageFlags>& waitMasks,
	                                      const std::vector<VkCommandBuffer>& cmds,
	                                      const std::vector<VkSemaphore>& signalSemaphore,
							              const uint32_t signalCount) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan query pool create info.
	/// </summary>
	/// <param name="count">Number of queries to manage by the pool.</param>
	/// <param name="type">Vulkan query type.</param>
	/// <returns>VkQueryPoolCreateInfo.</returns>
	[[nodiscard]] VkQueryPoolCreateInfo QueryPoolCreateInfo(uint32_t count, VkQueryType queryType) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan swapchain create info.
	/// </summary>
	/// <param name="surface">Vulkan surface to use.</param>
	/// <param name="imageCount">Number of images to create.</param>
	/// <param name="surfaceFormat">Vulkan surface format to use.</param>
	/// <param name="imageExtent">Vulkan image extent to use.</param>
	/// <param name="sharingMode">Vulkan sharing mode to use.</param>
	/// <param name="queueFamilyIndexCount">Number of queue family indices to use.</param>
	/// <param name="preTransform">Vulkan pre-transform to use.</param>
	/// <param name="compositeAlpha">Vulkan composite alpha to use.</param>
	/// <param name="presentMode">Vulkan present mode to use.</param>
	/// <returns>VkSwapchainCreateInfoKHR.</returns>
	[[nodiscard]] VkSwapchainCreateInfoKHR SwapchainCreateInfoKHR(VkSurfaceKHR surface, uint32_t imageCount,
	                                                              VkSurfaceFormatKHR surfaceFormat, VkExtent2D imageExtent,
	                                                              VkSharingMode sharingMode, uint32_t queueFamilyIndexCount,
	                                                              const std::array<uint32_t, 2>& queueFamilyIndices,
	                                                              VkSurfaceTransformFlagBitsKHR preTransform,
													              VkCompositeAlphaFlagBitsKHR compositeAlpha,
													              VkPresentModeKHR presentMode);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan present info.
	/// </summary>
	/// <param name="waitSemaphores">Vulkan semaphore(s) to wait on.</param>
	/// <param name="swapChain">Vulkan swapchain to present.</param>
	/// <param name="presentIndex">Vulkan swapchain image index to present.</param>
	/// <returns>VkPresentInfoKHR.</returns>
	[[nodiscard]] VkPresentInfoKHR PresentInfo(const std::vector<VkSemaphore>& waitSemaphores, const VkSwapchainKHR& swapChain,
	                                           uint32_t& presentIndex) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan memory allocate info.
	/// </summary>
	/// <param name="allocSize">Size of the allocation in bytes.</param>
	/// <param name="memoryTypeIndex">Index of the memory type to allocate.</param>
	/// <returns>VkMemoryAllocateInfo.</returns>
	[[nodiscard]] VkMemoryAllocateInfo MemoryAllocateInfo(VkDeviceSize allocSize, uint32_t memoryTypeIndex) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] VkClearColorValue ClearColorValue(const RendererAPI::Color& color, const ImageFormat format);
}

#endif /*TRAP_VULKANINITS_H*/