#ifndef TRAP_VULKANINITS_H
#define TRAP_VULKANINITS_H

#include "Graphics/API/RendererAPI.h"
#include "VulkanMemoryAllocator.h"

#include "Graphics/API/ImageFormat.h"
#include "Maths/Math.h"

namespace TRAP::Graphics::API::VulkanInits
{
	/// <summary>
	/// Create a Vulkan application info from app name.
	/// </summary>
	/// <param name="appName">Application name.</param>
	/// <returns>VkApplicationInfo.</returns>
	[[nodiscard]] VkApplicationInfo ApplicationInfo(std::string_view appName);

	/// <summary>
	/// Create a Vulkan instance create info from app info, instance layers and extensions.
	/// </summary>
	/// <param name="appInfo">Vulkan application info.</param>
	/// <param name="instanceLayers">Instance layers to use.</param>
	/// <param name="instanceExtensions">Instance extensions to use.</param>
	/// <returns>VkInstanceCreateInfo.</returns>
	[[nodiscard]] constexpr VkInstanceCreateInfo InstanceCreateInfo(const VkApplicationInfo& appInfo,
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
	                                                                     std::string_view name);

	/// <summary>
	/// Create a Vulkan debug marker object name info.
	/// </summary>
	/// <param name="type">Vulkan debug report object type.</param>
	/// <param name="handle">Vulkan handle.</param>
	/// <param name="name">Debug object name to set.</param>
	/// <returns>VkDebugMarkerObjectNameInfoEXT.</returns>
	[[nodiscard]] VkDebugMarkerObjectNameInfoEXT DebugMarkerObjectNameInfo(VkDebugReportObjectTypeEXT type, uint64_t handle,
	                                                                       std::string_view name);

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
	[[nodiscard]] constexpr VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(const std::vector<VkDescriptorSetLayoutBinding>& bindings) noexcept;

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
		                                                                                  const VkDescriptorUpdateTemplateEntry* entries,
		                                                                                  VkPipelineBindPoint bindPoint,
		                                                                                  VkPipelineLayout pipelineLayout,
		                                                                                  uint32_t setIndex);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan fence create info.
	/// </summary>
	/// <returns>VkFenceCreateInfo.</returns>
	[[nodiscard]] constexpr VkFenceCreateInfo FenceCreateInfo() noexcept;

	/// <summary>
	/// Create a Vulkan semaphore create info.
	/// </summary>
	/// <returns>VkSemaphoreCreateInfo.</returns>
	[[nodiscard]] constexpr VkSemaphoreCreateInfo SemaphoreCreateInfo() noexcept;

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
	[[nodiscard]] constexpr VkSubpassDescription SubPassDescription(VkPipelineBindPoint bindPoint,
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
	[[nodiscard]] constexpr VkSubpassDescription SubPassDescription(VkPipelineBindPoint bindPoint,
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
	[[nodiscard]] constexpr VkSubpassDescription2KHR SubPassDescription(VkPipelineBindPoint bindPoint,
	                                                                    const std::vector<VkAttachmentReference2KHR>& inputAttachments,
		                                                                const std::vector<VkAttachmentReference2KHR>& colorAttachments,
		                                                                VkAttachmentReference2KHR& depthStencilAttachment,
												                        const VkFragmentShadingRateAttachmentInfoKHR* shadingRateAttachment) noexcept;

	/// <summary>
	/// Create a Vulkan subpass description.
	/// </summary>
	/// <param name="bindPoint">Vulkan pipeline bind point.</param>
	/// <param name="inputAttachments">Vulkan input attachments.</param>
	/// <param name="colorAttachments">Vulkan color attachments.</param>
	/// <returns>VkSubpassDescription2KHR.</returns>
	[[nodiscard]] constexpr VkSubpassDescription2KHR SubPassDescription(VkPipelineBindPoint bindPoint,
		                                                                const std::vector<VkAttachmentReference2KHR>& inputAttachments,
		                                                                const std::vector<VkAttachmentReference2KHR>& colorAttachments,
												                        const VkFragmentShadingRateAttachmentInfoKHR* shadingRateAttachment) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan render pass create info.
	/// </summary>
	/// <param name="attachmentDescriptions">List of Vulkan attachment descriptions.</param>
	/// <param name="subpassDescription">Vulkan subpass description.</param>
	/// <returns>VkRenderPassCreateInfo.</returns>
	[[nodiscard]] constexpr VkRenderPassCreateInfo RenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachmentDescriptions,
		                                                                const VkSubpassDescription& subpassDescription) noexcept;

	/// <summary>
	/// Create a Vulkan render pass create info.
	/// </summary>
	/// <param name="attachmentDescriptions">List of Vulkan attachment descriptions.</param>
	/// <param name="subpassDescription">Vulkan subpass description.</param>
	/// <returns>VkRenderPassCreateInfo2KHR.</returns>
	[[nodiscard]] constexpr VkRenderPassCreateInfo2KHR RenderPassCreateInfo(const std::vector<VkAttachmentDescription2KHR>& attachmentDescriptions,
		                                                                    const VkSubpassDescription2KHR& subpassDescription) noexcept;

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
	[[nodiscard]] constexpr VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queueFamilyIndex) noexcept;

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
	[[nodiscard]] constexpr VkCommandBufferBeginInfo CommandBufferBeginInfo() noexcept;

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
	[[nodiscard]] constexpr VkSamplerCreateInfo SamplerCreateInfo(VkFilter magFilter, VkFilter minFilter,
	                                                              VkSamplerMipmapMode mipMapMode, VkSamplerAddressMode u,
																  VkSamplerAddressMode v, VkSamplerAddressMode w,
																  float mipLodBias, float minLod, float maxLod,
																  float maxAnisotropy, VkCompareOp compareOp);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan pipeline layout create info.
	/// </summary>
	/// <param name="layoutCount">Number of descriptor set layouts.</param>
	/// <param name="layouts">Vulkan descriptor set layouts.</param>
	/// <param name="pushConstantRangeCount">Number of push constant ranges.</param>
	/// <param name="pushConstants">Vulkan push constant ranges.</param>
	/// <returns>VkPipelineLayoutCreateInfo.</returns>
	[[nodiscard]] constexpr VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo(uint32_t layoutCount, const VkDescriptorSetLayout* layouts,
	                                                                            uint32_t pushConstantRangeCount,
														                        const VkPushConstantRange* pushConstants) noexcept;

	/// <summary>
	/// Create a Vulkan pipeline color blend state create info.
	/// </summary>
	/// <param name="attachments">Vulkan pipeline color blend attachment states.</param>
	/// <returns>VkPipelineColorBlendStateCreateInfo.</returns>
	[[nodiscard]] constexpr VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& attachments) noexcept;

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
	[[nodiscard]] constexpr VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(VkLogicOp logicOp,
		                                                                                          const std::vector<VkPipelineColorBlendAttachmentState>& attachments,
		                                                                                          float blendConstR, float blendConstG,
		                                                                                          float blendConstB, float blendConstA) noexcept;

	/// <summary>
	/// Create a Vulkan pipeline cache create info.
	/// </summary>
	/// <param name="data">Vulkan pipeline cache data.</param>
	/// <param name="flags">Vulkan pipeline cache create flags.</param>
	/// <returns>VkPipelineCacheCreateInfo.</returns>
	[[nodiscard]] constexpr VkPipelineCacheCreateInfo PipelineCacheCreateInfo(std::span<const uint8_t> data,
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
	[[nodiscard]] constexpr VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo(uint32_t inputBindingCount,
		                                                                                            const VkVertexInputBindingDescription* inputBindings,
		                                                                                            uint32_t inputAttributeCount,
		                                                                                            const VkVertexInputAttributeDescription* inputAttributes) noexcept;

	/// <summary>
	/// Create a Vulkan pipeline input assembly state create info.
	/// </summary>
	/// <param name="topology">Vulkan primitive topology.</param>
	/// <param name="primitiveRestart">True to enable primitive restart.</param>
	/// <returns>VkPipelineInputAssemblyStateCreateInfo.</returns>
	[[nodiscard]] constexpr VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology,
	                                                                                                    bool primitiveRestart) noexcept;

	/// <summary>
	/// Create a Vulkan pipeline tessellation state create info.
	/// </summary>
	/// <param name="patchControlPoints">Number of patch control points.</param>
	/// <returns>VkPipelineTessellationStateCreateInfo.</returns>
	[[nodiscard]] constexpr VkPipelineTessellationStateCreateInfo PipelineTessellationStateCreateInfo(uint32_t patchControlPoints) noexcept;

	/// <summary>
	/// Create a Vulkan pipeline viewport state create info.
	/// </summary>
	/// <returns>VkPipelineViewportStateCreateInfo.</returns>
	[[nodiscard]] constexpr VkPipelineViewportStateCreateInfo PipelineViewportStateCreateInfo() noexcept;

	/// <summary>
	/// Create a Vulkan pipeline multisample state create info.
	/// </summary>
	/// <param name="sampleCount">Number of samples.</param>
	/// <param name="sampleShading">True to enable sample shading.</param>
	/// <param name="sampleShadingRate">Rate used for sample shading if enabled.</param>
	/// <returns>VkPipelineMultisampleStateCreateInfo.</returns>
	[[nodiscard]] constexpr VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo(VkSampleCountFlagBits sampleCount,
	                                                                                                bool sampleShading,
																			                        float sampleShadingRate) noexcept;

	/// <summary>
	/// Create a Vulkan fragment shading rate state create info.
	/// </summary>
	/// <param name="fragmentSize">Shading rate/Fragment size to use.</param>
	/// <param name="rateCombiners">Shading rate combiners.</param>
	/// <returns>VkPipelineFragmentShadingRateStateCreateInfoKHR</returns>
	[[nodiscard]] constexpr VkPipelineFragmentShadingRateStateCreateInfoKHR PipelineFragmentShadingRateStateCreateInfo(VkExtent2D fragmentSize,
	                                                                                                                   const std::array<VkFragmentShadingRateCombinerOpKHR, 2>& rateCombiners) noexcept;

	/// <summary>
	/// Create a Vulkan pipeline dynamic state create info.
	/// </summary>
	/// <param name="dynamicStates">Vulkan dynamic state to use.</param>
	/// <returns>VkPipelineDynamicStateCreateInfo.</returns>
	[[nodiscard]] constexpr VkPipelineDynamicStateCreateInfo PipelineDynamicStateCreateInfo(const std::vector<VkDynamicState>& dynamicStates) noexcept;

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
	[[nodiscard]] constexpr VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo(uint32_t stageCount,
															                        const VkPipelineShaderStageCreateInfo* stages,
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
	[[nodiscard]] constexpr VkSubmitInfo SubmitInfo(const std::vector<VkSemaphore>& waitSemaphores,
							                        uint32_t waitCount,
	                                                const std::vector<VkPipelineStageFlags>& waitMasks,
	                                                const std::vector<VkCommandBuffer>& cmds,
	                                                const std::vector<VkSemaphore>& signalSemaphore,
							                        uint32_t signalCount) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan query pool create info.
	/// </summary>
	/// <param name="count">Number of queries to manage by the pool.</param>
	/// <param name="type">Vulkan query type.</param>
	/// <returns>VkQueryPoolCreateInfo.</returns>
	[[nodiscard]] constexpr VkQueryPoolCreateInfo QueryPoolCreateInfo(uint32_t count, VkQueryType queryType) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan swapchain create info.
	/// </summary>
	/// <param name="surface">Vulkan surface to use.</param>
	/// <param name="imageCount">Number of images to create.</param>
	/// <param name="surfaceFormat">Vulkan surface format to use.</param>
	/// <param name="imageExtent">Vulkan image extent to use.</param>
	/// <param name="sharingMode">Vulkan sharing mode to use.</param>
	/// <param name="queueFamilyIndices">
	/// Queue families that have access to the image(s) of the swapchain when sharingMode is VK_SHARING_MODE_CONCURRENT.
	/// </param>
	/// <param name="preTransform">Vulkan pre-transform to use.</param>
	/// <param name="compositeAlpha">Vulkan composite alpha to use.</param>
	/// <param name="presentMode">Vulkan present mode to use.</param>
	/// <param name="oldSwapchain">Old Vulkan swapchain to aid in resource reusage. Default. VK_NULL_HANDLE</param>
	/// <returns>VkSwapchainCreateInfoKHR.</returns>
	[[nodiscard]] VkSwapchainCreateInfoKHR SwapchainCreateInfoKHR(VkSurfaceKHR surface, uint32_t imageCount,
	                                                              VkSurfaceFormatKHR surfaceFormat, VkExtent2D imageExtent,
	                                                              VkSharingMode sharingMode,
	                                                              const std::vector<uint32_t>& queueFamilyIndices,
	                                                              VkSurfaceTransformFlagBitsKHR preTransform,
													              VkCompositeAlphaFlagBitsKHR compositeAlpha,
													              VkPresentModeKHR presentMode,
																  VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan present info.
	/// </summary>
	/// <param name="waitSemaphores">Vulkan semaphore(s) to wait on.</param>
	/// <param name="swapChain">Vulkan swapchain to present.</param>
	/// <param name="presentIndex">Vulkan swapchain image index to present.</param>
	/// <returns>VkPresentInfoKHR.</returns>
	[[nodiscard]] constexpr VkPresentInfoKHR PresentInfo(const std::vector<VkSemaphore>& waitSemaphores, const VkSwapchainKHR& swapChain,
	                                                     uint32_t& presentIndex) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a Vulkan memory allocate info.
	/// </summary>
	/// <param name="allocSize">Size of the allocation in bytes.</param>
	/// <param name="memoryTypeIndex">Index of the memory type to allocate.</param>
	/// <returns>VkMemoryAllocateInfo.</returns>
	[[nodiscard]] constexpr VkMemoryAllocateInfo MemoryAllocateInfo(VkDeviceSize allocSize, uint32_t memoryTypeIndex) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr VkClearColorValue ClearColorValue(const RendererAPI::Color& color, ImageFormat format);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkInstanceCreateInfo TRAP::Graphics::API::VulkanInits::InstanceCreateInfo(const VkApplicationInfo& appInfo,
	                                                                                              const std::vector<const char*>& instanceLayers,
	                                                                                              const std::vector<const char*>& instanceExtensions) noexcept
{
	return VkInstanceCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = NumericCast<uint32_t>(instanceLayers.size()),
		.ppEnabledLayerNames = instanceLayers.data(),
		.enabledExtensionCount = NumericCast<uint32_t>(instanceExtensions.size()),
		.ppEnabledExtensionNames = instanceExtensions.data()
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkDescriptorSetLayoutCreateInfo TRAP::Graphics::API::VulkanInits::DescriptorSetLayoutCreateInfo(const std::vector<VkDescriptorSetLayoutBinding>& bindings) noexcept
{
	return VkDescriptorSetLayoutCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = NumericCast<uint32_t>(bindings.size()),
		.pBindings = bindings.data()
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkFenceCreateInfo TRAP::Graphics::API::VulkanInits::FenceCreateInfo() noexcept
{
	return VkFenceCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSemaphoreCreateInfo TRAP::Graphics::API::VulkanInits::SemaphoreCreateInfo() noexcept
{
	return VkSemaphoreCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSubpassDescription TRAP::Graphics::API::VulkanInits::SubPassDescription(const VkPipelineBindPoint bindPoint,
	                                                                                              const std::vector<VkAttachmentReference>& inputAttachments,
	                                                                                              const std::vector<VkAttachmentReference>& colorAttachments,
	                                                                                              VkAttachmentReference& depthStencilAttachment) noexcept
{
	return VkSubpassDescription
	{
		.flags = 0,
		.pipelineBindPoint = bindPoint,
		.inputAttachmentCount = NumericCast<uint32_t>(inputAttachments.size()),
		.pInputAttachments = inputAttachments.data(),
		.colorAttachmentCount = NumericCast<uint32_t>(colorAttachments.size()),
		.pColorAttachments = colorAttachments.data(),
		.pResolveAttachments = nullptr,
		.pDepthStencilAttachment = &depthStencilAttachment,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSubpassDescription TRAP::Graphics::API::VulkanInits::SubPassDescription(const VkPipelineBindPoint bindPoint,
	                                                                                              const std::vector<VkAttachmentReference>& inputAttachments,
	                                                                                              const std::vector<VkAttachmentReference>& colorAttachments) noexcept
{
	return VkSubpassDescription
	{
		.flags = 0,
		.pipelineBindPoint = bindPoint,
		.inputAttachmentCount = NumericCast<uint32_t>(inputAttachments.size()),
		.pInputAttachments = inputAttachments.data(),
		.colorAttachmentCount = NumericCast<uint32_t>(colorAttachments.size()),
		.pColorAttachments = colorAttachments.data(),
		.pResolveAttachments = nullptr,
		.pDepthStencilAttachment = nullptr,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSubpassDescription2KHR TRAP::Graphics::API::VulkanInits::SubPassDescription(const VkPipelineBindPoint bindPoint,
	                                                                                                  const std::vector<VkAttachmentReference2KHR>& inputAttachments,
	                                                                                                  const std::vector<VkAttachmentReference2KHR>& colorAttachments,
	                                                                                                  VkAttachmentReference2KHR& depthStencilAttachment,
												                                                      const VkFragmentShadingRateAttachmentInfoKHR* const shadingRateAttachment) noexcept
{
	return VkSubpassDescription2KHR
	{
		.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2_KHR,
		.pNext = shadingRateAttachment != nullptr ? shadingRateAttachment : nullptr,
		.flags = 0,
		.pipelineBindPoint = bindPoint,
		.viewMask = 0,
		.inputAttachmentCount = NumericCast<uint32_t>(inputAttachments.size()),
		.pInputAttachments = inputAttachments.data(),
		.colorAttachmentCount = NumericCast<uint32_t>(colorAttachments.size()),
		.pColorAttachments = colorAttachments.data(),
		.pResolveAttachments = nullptr,
		.pDepthStencilAttachment = &depthStencilAttachment,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSubpassDescription2KHR TRAP::Graphics::API::VulkanInits::SubPassDescription(const VkPipelineBindPoint bindPoint,
	                                                                                                  const std::vector<VkAttachmentReference2KHR>& inputAttachments,
	                                                                                                  const std::vector<VkAttachmentReference2KHR>& colorAttachments,
																			                          const VkFragmentShadingRateAttachmentInfoKHR* const shadingRateAttachment) noexcept
{
	return VkSubpassDescription2KHR
	{
		.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2_KHR,
		.pNext = shadingRateAttachment != nullptr ? shadingRateAttachment : nullptr,
		.flags = 0,
		.pipelineBindPoint = bindPoint,
		.viewMask = 0,
		.inputAttachmentCount = NumericCast<uint32_t>(inputAttachments.size()),
		.pInputAttachments = inputAttachments.data(),
		.colorAttachmentCount = NumericCast<uint32_t>(colorAttachments.size()),
		.pColorAttachments = colorAttachments.data(),
		.pResolveAttachments = nullptr,
		.pDepthStencilAttachment = nullptr,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkRenderPassCreateInfo TRAP::Graphics::API::VulkanInits::RenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachmentDescriptions,
	                                                                                                  const VkSubpassDescription& subpassDescription) noexcept
{
	return VkRenderPassCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.attachmentCount = NumericCast<uint32_t>(attachmentDescriptions.size()),
		.pAttachments = attachmentDescriptions.data(),
		.subpassCount = 1,
		.pSubpasses = &subpassDescription,
		.dependencyCount = 0,
		.pDependencies = nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkRenderPassCreateInfo2KHR TRAP::Graphics::API::VulkanInits::RenderPassCreateInfo(const std::vector<VkAttachmentDescription2KHR>& attachmentDescriptions,
	                                                                                                      const VkSubpassDescription2KHR& subpassDescription) noexcept
{
	return VkRenderPassCreateInfo2KHR
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2_KHR,
		.pNext = nullptr,
		.flags = 0,
		.attachmentCount = NumericCast<uint32_t>(attachmentDescriptions.size()),
		.pAttachments = attachmentDescriptions.data(),
		.subpassCount = 1,
		.pSubpasses = &subpassDescription,
		.dependencyCount = 0,
		.pDependencies = nullptr,
		.correlatedViewMaskCount = 0,
		.pCorrelatedViewMasks = nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkCommandPoolCreateInfo TRAP::Graphics::API::VulkanInits::CommandPoolCreateInfo(const uint32_t queueFamilyIndex) noexcept
{
	return VkCommandPoolCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queueFamilyIndex = queueFamilyIndex
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkCommandBufferBeginInfo TRAP::Graphics::API::VulkanInits::CommandBufferBeginInfo() noexcept
{
	return VkCommandBufferBeginInfo
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pInheritanceInfo = nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSamplerCreateInfo TRAP::Graphics::API::VulkanInits::SamplerCreateInfo(const VkFilter magFilter,
	                                                                                            const VkFilter minFilter,
	                                                                                            const VkSamplerMipmapMode mipMapMode,
	                                                                                            const VkSamplerAddressMode u,
	                                                                                            const VkSamplerAddressMode v,
	                                                                                            const VkSamplerAddressMode w,
	                                                                                            const float mipLodBias,
																		                        const float minLod,
																		                        const float maxLod,
	                                                                                            const float maxAnisotropy,
	                                                                                            const VkCompareOp compareOp)
{
	TRAP_ASSERT(minLod <= maxLod, "VulkanInits::SamplerCreateInfo(): minLod can't be greater than maxLod!");

	return VkSamplerCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.magFilter = magFilter,
		.minFilter = minFilter,
		.mipmapMode = mipMapMode,
		.addressModeU = u,
		.addressModeV = v,
		.addressModeW = w,
		.mipLodBias = mipLodBias,
		.anisotropyEnable = (maxAnisotropy > 0.0f) ? VK_TRUE : VK_FALSE,
		.maxAnisotropy = maxAnisotropy,
		.compareEnable = (compareOp != VK_COMPARE_OP_NEVER) ? VK_TRUE : VK_FALSE,
		.compareOp = compareOp,
		.minLod = minLod,
		.maxLod = maxLod,
		.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
		.unnormalizedCoordinates = VK_FALSE
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineLayoutCreateInfo TRAP::Graphics::API::VulkanInits::PipelineLayoutCreateInfo(const uint32_t layoutCount,
	                                                                                                          const VkDescriptorSetLayout* const layouts,
	                                                                                                          const uint32_t pushConstantRangeCount,
	                                                                                                          const VkPushConstantRange* const pushConstants) noexcept
{
	return VkPipelineLayoutCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.setLayoutCount = layoutCount,
		.pSetLayouts = layouts,
		.pushConstantRangeCount = pushConstantRangeCount,
		.pPushConstantRanges = pushConstants
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineColorBlendStateCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& attachments) noexcept
{
	return VkPipelineColorBlendStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_CLEAR,
		.attachmentCount = NumericCast<uint32_t>(attachments.size()),
		.pAttachments = attachments.data(),
		.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineColorBlendStateCreateInfo(const VkLogicOp logicOp,
	                                                                                                                            const std::vector<VkPipelineColorBlendAttachmentState>& attachments,
	                                                                                                                            const float blendConstR,
	                                                                                                                            const float blendConstG,
	                                                                                                                            const float blendConstB,
	                                                                                                                            const float blendConstA) noexcept
{
	return VkPipelineColorBlendStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.logicOpEnable = VK_TRUE,
		.logicOp = logicOp,
		.attachmentCount = NumericCast<uint32_t>(attachments.size()),
		.pAttachments = attachments.data(),
		.blendConstants = {blendConstR, blendConstG, blendConstB, blendConstA}
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineCacheCreateInfo TRAP::Graphics::API::VulkanInits::PipelineCacheCreateInfo(const std::span<const uint8_t> data,
	                                                                                                        const VkPipelineCacheCreateFlags flags) noexcept
{
	return VkPipelineCacheCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		.pNext = nullptr,
		.flags = flags,
		.initialDataSize = data.size(),
		.pInitialData = data.data(),
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineDynamicStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineDynamicStateCreateInfo(const std::vector<VkDynamicState>& dynamicStates) noexcept
{
	return VkPipelineDynamicStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.dynamicStateCount = NumericCast<uint32_t>(dynamicStates.size()),
		.pDynamicStates = dynamicStates.data(),
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkGraphicsPipelineCreateInfo TRAP::Graphics::API::VulkanInits::GraphicsPipelineCreateInfo(const uint32_t stageCount,
	                                                                                                              const VkPipelineShaderStageCreateInfo* const stages,
	                                                                                                              const VkPipelineVertexInputStateCreateInfo& vi,
	                                                                                                              const VkPipelineInputAssemblyStateCreateInfo& ia,
	                                                                                                              const VkPipelineViewportStateCreateInfo& vs,
	                                                                                                              const VkPipelineRasterizationStateCreateInfo& rs,
	                                                                                                              const VkPipelineMultisampleStateCreateInfo& ms,
	                                                                                                              const VkPipelineDepthStencilStateCreateInfo& ds,
	                                                                                                              const VkPipelineColorBlendStateCreateInfo& cb,
	                                                                                                              const VkPipelineDynamicStateCreateInfo& dy,
	                                                                                                              VkPipelineLayout layout,
	                                                                                                              VkRenderPass renderPass) noexcept
{
	return VkGraphicsPipelineCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.stageCount = stageCount,
		.pStages = stages,
		.pVertexInputState = &vi,
		.pInputAssemblyState = &ia,
		.pTessellationState = nullptr,
		.pViewportState = &vs,
		.pRasterizationState = &rs,
		.pMultisampleState = &ms,
		.pDepthStencilState = &ds,
		.pColorBlendState = &cb,
		.pDynamicState = &dy,
		.layout = layout,
		.renderPass = renderPass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSubmitInfo TRAP::Graphics::API::VulkanInits::SubmitInfo(const std::vector<VkSemaphore>& waitSemaphores,
														                          const uint32_t waitCount,
                                                                                  const std::vector<VkPipelineStageFlags>& waitMasks,
                                                                                  const std::vector<VkCommandBuffer>& cmds,
                                                                                  const std::vector<VkSemaphore>& signalSemaphores,
														                          const uint32_t signalCount) noexcept
{
	return VkSubmitInfo
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = waitCount,
		.pWaitSemaphores = waitSemaphores.data(),
		.pWaitDstStageMask = waitMasks.data(),
		.commandBufferCount = NumericCast<uint32_t>(cmds.size()),
		.pCommandBuffers = cmds.data(),
		.signalSemaphoreCount = signalCount,
		.pSignalSemaphores = signalSemaphores.data()
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkQueryPoolCreateInfo TRAP::Graphics::API::VulkanInits::QueryPoolCreateInfo(const uint32_t count,
                                                                                                    const VkQueryType queryType) noexcept
{
	return VkQueryPoolCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queryType = queryType,
		.queryCount = count,
		.pipelineStatistics = 0
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPresentInfoKHR TRAP::Graphics::API::VulkanInits::PresentInfo(const std::vector<VkSemaphore>& waitSemaphores,
                                                                                       const VkSwapchainKHR& swapChain,
															                           uint32_t& presentIndex) noexcept
{
	return VkPresentInfoKHR
	{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = nullptr,
		.waitSemaphoreCount = NumericCast<uint32_t>(waitSemaphores.size()),
		.pWaitSemaphores = !waitSemaphores.empty() ? waitSemaphores.data() : nullptr,
		.swapchainCount = 1,
		.pSwapchains = &swapChain,
		.pImageIndices = &presentIndex,
		.pResults = nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkMemoryAllocateInfo TRAP::Graphics::API::VulkanInits::MemoryAllocateInfo(const VkDeviceSize allocSize,
                                                                                                  const uint32_t memoryTypeIndex) noexcept
{
	return VkMemoryAllocateInfo
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = nullptr,
		.allocationSize = allocSize,
		.memoryTypeIndex = memoryTypeIndex
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkClearColorValue TRAP::Graphics::API::VulkanInits::ClearColorValue(const RendererAPI::Color& color,
                                                                                            const ImageFormat format)
{
	VkClearColorValue clearColor{};

	if(ImageFormatIsNormalized(format))
	{
		if(!ImageFormatIsSigned(format))
		{
			clearColor.float32[0] = TRAP::Math::Clamp(NumericCast<float>(color.Red), 0.0f, 1.0f);
			clearColor.float32[1] = TRAP::Math::Clamp(NumericCast<float>(color.Green), 0.0f, 1.0f);
			clearColor.float32[2] = TRAP::Math::Clamp(NumericCast<float>(color.Blue), 0.0f, 1.0f);
			clearColor.float32[3] = TRAP::Math::Clamp(NumericCast<float>(color.Alpha), 0.0f, 1.0f);
		}
		else /*if(ImageFormatIsSigned(format))*/
		{
			clearColor.float32[0] = TRAP::Math::Clamp(NumericCast<float>(color.Red), -1.0f, 1.0f);
			clearColor.float32[1] = TRAP::Math::Clamp(NumericCast<float>(color.Green), -1.0f, 1.0f);
			clearColor.float32[2] = TRAP::Math::Clamp(NumericCast<float>(color.Blue), -1.0f, 1.0f);
			clearColor.float32[3] = TRAP::Math::Clamp(NumericCast<float>(color.Alpha), -1.0f, 1.0f);
		}
	}
	else /*if(!ImageFormatIsNormalized(format))*/
	{
		if(!ImageFormatIsFloat(format))
		{
			if(!ImageFormatIsSigned(format))
			{
				clearColor.uint32[0] = NumericCast<uint32_t>(TRAP::Math::Round(color.Red));
				clearColor.uint32[1] = NumericCast<uint32_t>(TRAP::Math::Round(color.Green));
				clearColor.uint32[2] = NumericCast<uint32_t>(TRAP::Math::Round(color.Blue));
				clearColor.uint32[3] = NumericCast<uint32_t>(TRAP::Math::Round(color.Alpha));
			}
			else /*if(ImageFormatIsSigned(format))*/
			{
				clearColor.int32[0] = NumericCast<int32_t>(TRAP::Math::Round(color.Red));
				clearColor.int32[1] = NumericCast<int32_t>(TRAP::Math::Round(color.Green));
				clearColor.int32[2] = NumericCast<int32_t>(TRAP::Math::Round(color.Blue));
				clearColor.int32[3] = NumericCast<int32_t>(TRAP::Math::Round(color.Alpha));
			}
		}
		else
		{
			clearColor.float32[0] = NumericCast<float>(color.Red);
			clearColor.float32[1] = NumericCast<float>(color.Green);
			clearColor.float32[2] = NumericCast<float>(color.Blue);
			clearColor.float32[3] = NumericCast<float>(color.Alpha);
		}
	}

	return clearColor;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineVertexInputStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineVertexInputStateCreateInfo(const uint32_t inputBindingCount,
	                                                                                                                              const VkVertexInputBindingDescription* const inputBindings,
	                                                                                                                              const uint32_t inputAttributeCount,
	                                                                                                                              const VkVertexInputAttributeDescription* const inputAttributes) noexcept
{
	return VkPipelineVertexInputStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.vertexBindingDescriptionCount = inputBindingCount,
		.pVertexBindingDescriptions = inputBindings,
		.vertexAttributeDescriptionCount = inputAttributeCount,
		.pVertexAttributeDescriptions = inputAttributes
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineInputAssemblyStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineInputAssemblyStateCreateInfo(const VkPrimitiveTopology topology,
                                                                                                                                      const bool primitiveRestart) noexcept
{
	return VkPipelineInputAssemblyStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.topology = topology,
		.primitiveRestartEnable = primitiveRestart ? VK_TRUE : VK_FALSE
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineTessellationStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineTessellationStateCreateInfo(const uint32_t patchControlPoints) noexcept
{
	return VkPipelineTessellationStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.patchControlPoints = patchControlPoints
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineViewportStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineViewportStateCreateInfo() noexcept
{
	return VkPipelineViewportStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.viewportCount = 1, //We are using dynamic viewports but we must set the viewportCount to 1
		.pViewports = nullptr,
		.scissorCount = 1,
		.pScissors = nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineMultisampleStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineMultisampleStateCreateInfo(const VkSampleCountFlagBits sampleCount,
	                                                                                                                              const bool sampleShading,
																										                          const float sampleShadingRate) noexcept
{
	return VkPipelineMultisampleStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.rasterizationSamples = sampleCount,
		.sampleShadingEnable = sampleShading ? VK_TRUE : VK_FALSE,
		.minSampleShading = sampleShading ? sampleShadingRate : 0.0f,
		.pSampleMask = nullptr,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineFragmentShadingRateStateCreateInfoKHR TRAP::Graphics::API::VulkanInits::PipelineFragmentShadingRateStateCreateInfo(const VkExtent2D fragmentSize,
	                                                                                                                                                 const std::array<VkFragmentShadingRateCombinerOpKHR, 2>& rateCombiners) noexcept
{
	return VkPipelineFragmentShadingRateStateCreateInfoKHR
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_FRAGMENT_SHADING_RATE_STATE_CREATE_INFO_KHR,
		.pNext = nullptr,
		.fragmentSize = fragmentSize,
		.combinerOps = {std::get<0>(rateCombiners), std::get<1>(rateCombiners)}
	};
}

#endif /*TRAP_VULKANINITS_H*/