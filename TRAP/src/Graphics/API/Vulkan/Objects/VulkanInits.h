#ifndef TRAP_VULKANINITS_H
#define TRAP_VULKANINITS_H

#include "Graphics/API/RendererAPI.h"
#include "VulkanMemoryAllocator.h"

#include "Graphics/API/ImageFormat.h"
#include "Maths/Math.h"

namespace TRAP::Graphics::API::VulkanInits
{
	/// @brief Create a Vulkan application info from app name.
	/// @param appName Application name.
	/// @return VkApplicationInfo.
	[[nodiscard]] VkApplicationInfo ApplicationInfo(std::string_view appName);

	/// @brief Create a Vulkan instance create info from app info, instance layers and extensions.
	/// @param appInfo Vulkan application info.
	/// @param instanceLayers Instance layers to use.
	/// @param instanceExtensions Instance extensions to use.
	/// @return VkInstanceCreateInfo.
	[[nodiscard]] constexpr VkInstanceCreateInfo InstanceCreateInfo(const VkApplicationInfo& appInfo,
									                                const std::vector<const char*>& instanceLayers,
	                                                                const std::vector<const char*>& instanceExtensions) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan debug utils messenger create info.
	/// @param callback Debug callback.
	/// @return VkDebugUtilsMessengerCreateInfoEXT.
	[[nodiscard]] VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo(PFN_vkDebugUtilsMessengerCallbackEXT callback);

	/// @brief Create a Vulkan debug report callback create info.
	/// @param callback Debug callback.
	/// @return VkDebugReportCallbackCreateInfoEXT.
	[[nodiscard]] VkDebugReportCallbackCreateInfoEXT DebugReportCallbackCreateInfo(PFN_vkDebugReportCallbackEXT callback);

	/// @brief Create a Vulkan debug utils object name info.
	/// @param type Vulkan object type.
	/// @param handle Vulkan handle.
	/// @param name Debug object name to set.
	/// @return VkDebugUtilsObjectNameInfoEXT.
	[[nodiscard]] VkDebugUtilsObjectNameInfoEXT DebugUtilsObjectNameInfo(VkObjectType type, u64 handle,
	                                                                     std::string_view name);

	/// @brief Create a Vulkan debug marker object name info.
	/// @param type Vulkan debug report object type.
	/// @param handle Vulkan handle.
	/// @param name Debug object name to set.
	/// @return VkDebugMarkerObjectNameInfoEXT.
	[[nodiscard]] VkDebugMarkerObjectNameInfoEXT DebugMarkerObjectNameInfo(VkDebugReportObjectTypeEXT type, u64 handle,
	                                                                       std::string_view name);

	/// @brief Create a Vulkan debug utils label.
	/// @param r Red color.
	/// @param g Green color.
	/// @param b Blue color.
	/// @param name Name for the label.
	/// @return VkDebugUtilsLabelEXT.
	[[nodiscard]] VkDebugUtilsLabelEXT DebugUtilsLabelExt(f32 r, f32 g, f32 b, std::string_view name);

	/// @brief Create a Vulkan debug marker.
	/// @param r Red color.
	/// @param g Green color.
	/// @param b Blue color.
	/// @param name Name for the label.
	/// @return VkDebugMarkerMarkerInfoEXT.
	[[nodiscard]] VkDebugMarkerMarkerInfoEXT DebugMarkerMarkerInfo(f32 r, f32 g, f32 b, std::string_view name);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a logical Vulkan device create info.
	/// @param pNext Optional next pointer.
	/// @param queueCreateInfos Vulkan device queue create infos.
	/// @param deviceExtensions Device extensions to use.
	/// @return VkDeviceCreateInfo.
	[[nodiscard]] VkDeviceCreateInfo DeviceCreateInfo(const void* pNext,
	                                                  const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
	                                                  const std::vector<const char*>& deviceExtensions);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan memory allocator (VMA) allocator create info.
	/// @param device Logical Vulkan device.
	/// @param physicalDevice Physical Vulkan device.
	/// @param instance Vulkan instance.
	/// @param vulkanFunctions VMA Vulkan functions.
	/// @return VmaAllocatorCreateInfo.
	[[nodiscard]] VmaAllocatorCreateInfo VMAAllocatorCreateInfo(VkDevice device, VkPhysicalDevice physicalDevice,
	                                                            VkInstance instance, const VmaVulkanFunctions& vulkanFunctions);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan descriptor pool create info.
	/// @param descriptorPoolSizes Descriptor pool sizes to allocate by the pool.
	/// @param numDescriptorSets Max number of descriptor sets which should be allocated from the pool.
	/// @return VkDescriptorPoolCreateInfo.
	[[nodiscard]] VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo(const std::vector<VkDescriptorPoolSize>& descriptorPoolSizes,
	                                                                  u32 numDescriptorSets);

	/// @brief Create a Vulkan descriptor set allocation info.
	/// @param descriptorPool Vulkan descriptor pool to use.
	/// @param descriptorLayout Vulkan descriptor set layout to use.
	/// @return VkDescriptorSetAllocateInfo.
	[[nodiscard]] VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo(VkDescriptorPool descriptorPool,
														                const VkDescriptorSetLayout& descriptorLayout);

	/// @brief Create a Vulkan descriptor set layout create info.
	/// @param bindings List of Vulkan descriptor set layout bindings.
	/// @return VkDescriptorSetLayoutCreateInfo.
	[[nodiscard]] constexpr VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(const std::vector<VkDescriptorSetLayoutBinding>& bindings) noexcept;

	/// @brief Create a Vulkan descriptor update template create info.
	/// @param descriptorSetLayout Vulkan descriptor set layout to use.
	/// @param entryCount Number of entries in the descriptor update template.
	/// @param entries Vulkan descriptor update template entries.
	/// @param bindPoint Vulkan pipeline bind point.
	/// @param pipelineLayout Vulkan pipeline layout to use.
	/// @param setIndex Set index of the Descriptor set.
	/// @return VkDescriptorUpdateTemplateCreateInfo.
	[[nodiscard]] VkDescriptorUpdateTemplateCreateInfo DescriptorUpdateTemplateCreateInfo(VkDescriptorSetLayout descriptorSetLayout,
		                                                                                  u32 entryCount,
		                                                                                  const VkDescriptorUpdateTemplateEntry* entries,
		                                                                                  VkPipelineBindPoint bindPoint,
		                                                                                  VkPipelineLayout pipelineLayout,
		                                                                                  u32 setIndex);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan fence create info.
	/// @return VkFenceCreateInfo.
	[[nodiscard]] constexpr VkFenceCreateInfo FenceCreateInfo() noexcept;

	/// @brief Create a Vulkan semaphore create info.
	/// @return VkSemaphoreCreateInfo.
	[[nodiscard]] constexpr VkSemaphoreCreateInfo SemaphoreCreateInfo() noexcept;

	/// @brief Create a Vulkan semaphore wait info.
	/// @param semaphore Semaphore to wait on.
	/// @param value Signal value to set.
	/// @return VkSemaphoreWaitInfoKHR
	[[nodiscard]] VkSemaphoreWaitInfoKHR SemaphoreWaitInfo(VkSemaphore semaphore, const u64& value);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan fragment shading rate attachment info.
	/// @param shadingRateAttachment Shading rate attachment.
	/// @param texelSize Size of the portion of the framebuffer corresponding to each texel.
	/// @return VkFragmentShadingRateAttachmentInfoKHR.
	[[nodiscard]] VkFragmentShadingRateAttachmentInfoKHR FragmentShadingRateAttachmentInfo(VkAttachmentReference2KHR& shadingRateAttachment,
	                                                                                       VkExtent2D texelSize);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan attachment description.
	/// @param format Vulkan format.
	/// @param sampleCount Number of samples.
	/// @param loadOp Vulkan Attachment load operation.
	/// @param storeOp Vulkan Attachment store operation.
	/// @param stencilLoadOp Vulkan Attachment stencil load operation.
	/// @param stencilStoreOp Vulkan Attachment stencil store operation.
	/// @param layout Initial Vulkan image layout used when render pass starts.
	/// @param finalLayout Final Vulkan image layout to transition to when render pass ends.
	/// @return VkAttachmentDescription.
	[[nodiscard]] VkAttachmentDescription AttachmentDescription(VkFormat format, VkSampleCountFlagBits sampleCount,
	                                                            VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
	                                                            VkAttachmentLoadOp stencilLoadOp,
												                VkAttachmentStoreOp stencilStoreOp,
	                                                            VkImageLayout layout, VkImageLayout finalLayout);

	/// @brief Create a Vulkan attachment description.
	/// @param format Vulkan format.
	/// @param sampleCount Number of samples.
	/// @param loadOp Vulkan Attachment load operation.
	/// @param storeOp Vulkan Attachment store operation.
	/// @param stencilLoadOp Vulkan Attachment stencil load operation.
	/// @param stencilStoreOp Vulkan Attachment stencil store operation.
	/// @param layout Initial Vulkan image layout used when render pass starts.
	/// @param finalLayout Final Vulkan image layout to transition to when render pass ends.
	/// @return VkAttachmentDescription2KHR.
	[[nodiscard]] VkAttachmentDescription2KHR AttachmentDescription2(VkFormat format, VkSampleCountFlagBits sampleCount,
	                                                                 VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
	                                                                 VkAttachmentLoadOp stencilLoadOp,
												                     VkAttachmentStoreOp stencilStoreOp,
	                                                                 VkImageLayout layout, VkImageLayout finalLayout);

	/// @brief Create a Vulkan subpass description.
	/// @param bindPoint Vulkan pipeline bind point.
	/// @param inputAttachments Vulkan input attachments.
	/// @param colorAttachments Vulkan color attachments.
	/// @param depthStencilAttachment Vulkan depth stencil attachment.
	/// @return VkSubpassDescription.
	[[nodiscard]] constexpr VkSubpassDescription SubPassDescription(VkPipelineBindPoint bindPoint,
	                                                                const std::vector<VkAttachmentReference>& inputAttachments,
		                                                            const std::vector<VkAttachmentReference>& colorAttachments,
		                                                            VkAttachmentReference& depthStencilAttachment) noexcept;

	/// @brief Create a Vulkan subpass description.
	/// @param bindPoint Vulkan pipeline bind point.
	/// @param inputAttachments Vulkan input attachments.
	/// @param colorAttachments Vulkan color attachments.
	/// @return VkSubpassDescription.
	[[nodiscard]] constexpr VkSubpassDescription SubPassDescription(VkPipelineBindPoint bindPoint,
		                                                            const std::vector<VkAttachmentReference>& inputAttachments,
		                                                            const std::vector<VkAttachmentReference>& colorAttachments) noexcept;

	/// @brief Create a Vulkan subpass description.
	/// @param bindPoint Vulkan pipeline bind point.
	/// @param inputAttachments Vulkan input attachments.
	/// @param colorAttachments Vulkan color attachments.
	/// @param depthStencilAttachment Vulkan depth stencil attachment.
	/// @param shadingRateAttachment Optional: Vulkan fragment shading rate attchment.
	/// @return VkSubpassDescription2KHR.
	[[nodiscard]] constexpr VkSubpassDescription2KHR SubPassDescription(VkPipelineBindPoint bindPoint,
	                                                                    const std::vector<VkAttachmentReference2KHR>& inputAttachments,
		                                                                const std::vector<VkAttachmentReference2KHR>& colorAttachments,
		                                                                VkAttachmentReference2KHR& depthStencilAttachment,
												                        const VkFragmentShadingRateAttachmentInfoKHR* shadingRateAttachment) noexcept;

	/// @brief Create a Vulkan subpass description.
	/// @param bindPoint Vulkan pipeline bind point.
	/// @param inputAttachments Vulkan input attachments.
	/// @param colorAttachments Vulkan color attachments.
	/// @param shadingRateAttachment Optional: Vulkan fragment shading rate attchment.
	/// @return VkSubpassDescription2KHR.
	[[nodiscard]] constexpr VkSubpassDescription2KHR SubPassDescription(VkPipelineBindPoint bindPoint,
		                                                                const std::vector<VkAttachmentReference2KHR>& inputAttachments,
		                                                                const std::vector<VkAttachmentReference2KHR>& colorAttachments,
												                        const VkFragmentShadingRateAttachmentInfoKHR* shadingRateAttachment) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan render pass create info.
	/// @param attachmentDescriptions List of Vulkan attachment descriptions.
	/// @param subpassDescription Vulkan subpass description.
	/// @return VkRenderPassCreateInfo.
	[[nodiscard]] constexpr VkRenderPassCreateInfo RenderPassCreateInfo(const std::vector<VkAttachmentDescription>& attachmentDescriptions,
		                                                                const VkSubpassDescription& subpassDescription) noexcept;

	/// @brief Create a Vulkan render pass create info.
	/// @param attachmentDescriptions List of Vulkan attachment descriptions.
	/// @param subpassDescription Vulkan subpass description.
	/// @return VkRenderPassCreateInfo2KHR.
	[[nodiscard]] constexpr VkRenderPassCreateInfo2KHR RenderPassCreateInfo(const std::vector<VkAttachmentDescription2KHR>& attachmentDescriptions,
		                                                                    const VkSubpassDescription2KHR& subpassDescription) noexcept;

	/// @brief Create a Vulkan render pass begin info.
	/// @param renderPass Vulkan render pass to use.
	/// @param frameBuffer Vulkan frame buffer to use.
	/// @param renderArea Vulkan render area.
	/// @param colorValues List of Vulkan clear values for color attachments.
	/// @return VkRenderPassBeginInfo.
	[[nodiscard]] VkRenderPassBeginInfo RenderPassBeginInfo(VkRenderPass renderPass, VkFramebuffer frameBuffer,
	                                                        VkRect2D renderArea, const std::vector<VkClearValue>& colorValues);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan framebuffer create info.
	/// @param renderPass Vulkan render pass to use.
	/// @param attachments List of Vulkan image views.
	/// @param width Width of the frame buffer.
	/// @param height Height of the frame buffer.
	/// @param layerCount Number of layers in the frame buffer.
	/// @return VkFramebufferCreateInfo.
	[[nodiscard]] VkFramebufferCreateInfo FramebufferCreateInfo(VkRenderPass renderPass,
	                                                            const std::vector<VkImageView>& attachments, u32 width,
		                                                        u32 height, u32 layerCount);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan image view create info.
	/// @param image Vulkan image to use.
	/// @param imageViewType Vulkan image view type.
	/// @param format Vulkan image format.
	/// @param levelCount Number of mip levels.
	/// @param layerCount Number of layers.
	/// @return VkImageViewCreateInfo.
	[[nodiscard]] VkImageViewCreateInfo ImageViewCreateInfo(VkImage image, VkImageViewType imageViewType, VkFormat format,
		                                                    u32 levelCount, u32 layerCount);

	/// @brief Create a Vulkan image create info.
	/// @param imageType Vulkan image type.
	/// @param imageFormat Vulkan image format.
	/// @param width Width of the image.
	/// @param height Height of the image.
	/// @param depth Depth of the image.
	/// @param mipLevels Number of mip levels.
	/// @param arrayLayers Number of array layers.
	/// @param sampleCount Vulkan sample count.
	/// @param tiling Vulkan image tiling.
	/// @param usage Vulkan image usage.
	/// @return VkImageCreateInfo.
	[[nodiscard]] VkImageCreateInfo ImageCreateInfo(VkImageType imageType, VkFormat imageFormat, u32 width, u32 height,
		                                            u32 depth, u32 mipLevels, u32 arrayLayers,
		                                            VkSampleCountFlagBits sampleCount, VkImageTiling tiling,
									                VkImageUsageFlags usage);

	/// @brief Create a Vulkan buffer image copy.
	/// @param bufferRowLength Row length of the buffer.
	/// @param width Width of the image.
	/// @param height Height of the image.
	/// @param depth Depth of the image.
	/// @param layers Number of Vulkan image subresource layers.
	[[nodiscard]] VkBufferImageCopy ImageCopy(u32 bufferRowLength, u32 width, u32 height, u32 depth,
							                  VkImageSubresourceLayers layers);

	/// @brief Create a Vulkan buffer image copy.
	/// @param bufferOffset Start offset of the buffer.
	/// @param bufferRowLength Row length of the buffer.
	/// @param bufferImageHeight Image height of the buffer.
	/// @param width Width of the image.
	/// @param height Height of the image.
	/// @param depth Depth of the image.
	/// @param layers Number of Vulkan image subresource layers.
	[[nodiscard]] VkBufferImageCopy ImageCopy(u64 bufferOffset, u32 bufferRowLength, u32 bufferImageHeight,
							                  u32 width, u32 height, u32 depth, VkImageSubresourceLayers layers);


	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan buffer create info.
	/// @param allocationSize Size of the buffer.
	/// @param usageFlags Vulkan buffer usage flags.
	/// @return VkBufferCreateInfo.
	[[nodiscard]] VkBufferCreateInfo BufferCreateInfo(u64 allocationSize, VkBufferUsageFlags usageFlags);

	/// @brief Create a Vulkan buffer view create info.
	/// @param buffer Vulkan buffer to use.
	/// @param format Vulkan buffer format.
	/// @param offset Offset of the buffer.
	/// @param range Range of the buffer.
	/// @return VkBufferViewCreateInfo.
	[[nodiscard]] VkBufferViewCreateInfo BufferViewCreateInfo(VkBuffer buffer, VkFormat format, u64 offset, u64 range);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan command pool create info.
	/// @param queueFamilyIndex Queue family index.
	/// @return VkCommandPoolCreateInfo.
	[[nodiscard]] constexpr VkCommandPoolCreateInfo CommandPoolCreateInfo(u32 queueFamilyIndex) noexcept;

	/// @brief Create a Vulkan command buffer allocate info.
	/// @param commandPool Vulkan command pool to use.
	/// @param secondary True to allocate as a secondary command buffer.
	/// @return VkCommandBufferAllocateInfo.
	[[nodiscard]] VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool commandPool, bool secondary);

	/// @brief Create a Vulkan command buffer begin info.
	/// @param oneTimeSubmit Specify that each recording of the command buffer will only be submitted once, and the
	///                      command buffer will be reset and recorded again between each submission.
	/// @return VkCommandBufferBeginInfo.
	[[nodiscard]] constexpr VkCommandBufferBeginInfo CommandBufferBeginInfo(bool oneTimeSubmit) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan sampler create info.
	/// @param magFilter Vulkan magnification filter.
	/// @param minFilter Vulkan minification filter.
	/// @param mipMapMode Vulkan mip map mode.
	/// @param u Vulkan sampler address mode.
	/// @param v Vulkan sampler address mode.
	/// @param w Vulkan sampler address mode.
	/// @param mipLodBias Vulkan mip map lod bias.
	/// @param minLod Minimum lod.
	/// @param maxLod Maximum lod.
	/// @param maxAnisotropy Vulkan max anisotropy.
	/// @param compareOp Vulkan compare op.
	/// @return VkSamplerCreateInfo.
	[[nodiscard]] constexpr VkSamplerCreateInfo SamplerCreateInfo(VkFilter magFilter, VkFilter minFilter,
	                                                              VkSamplerMipmapMode mipMapMode, VkSamplerAddressMode u,
																  VkSamplerAddressMode v, VkSamplerAddressMode w,
																  f32 mipLodBias, f32 minLod, f32 maxLod,
																  f32 maxAnisotropy, VkCompareOp compareOp);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan pipeline layout create info.
	/// @param layoutCount Number of descriptor set layouts.
	/// @param layouts Vulkan descriptor set layouts.
	/// @param pushConstantRangeCount Number of push constant ranges.
	/// @param pushConstants Vulkan push constant ranges.
	/// @return VkPipelineLayoutCreateInfo.
	[[nodiscard]] constexpr VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo(u32 layoutCount, const VkDescriptorSetLayout* layouts,
	                                                                            u32 pushConstantRangeCount,
														                        const VkPushConstantRange* pushConstants) noexcept;

	/// @brief Create a Vulkan pipeline color blend state create info.
	/// @param attachments Vulkan pipeline color blend attachment states.
	/// @return VkPipelineColorBlendStateCreateInfo.
	[[nodiscard]] constexpr VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(const std::vector<VkPipelineColorBlendAttachmentState>& attachments) noexcept;

	/// @brief Create a Vulkan pipeline color blend state create info.
	/// @param logicOp Vulkan pipeline logic op.
	/// @param attachments Vulkan pipeline color blend attachment states.
	/// @param blendConstR Blend constant red color.
	/// @param blendConstG Blend constant green color.
	/// @param blendConstB Blend constant blue color.
	/// @param blendConstA Blend constant alpha value.
	/// @return VkPipelineColorBlendStateCreateInfo.
	[[nodiscard]] constexpr VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(VkLogicOp logicOp,
		                                                                                          const std::vector<VkPipelineColorBlendAttachmentState>& attachments,
		                                                                                          f32 blendConstR, f32 blendConstG,
		                                                                                          f32 blendConstB, f32 blendConstA) noexcept;

	/// @brief Create a Vulkan pipeline cache create info.
	/// @param data Vulkan pipeline cache data.
	/// @param flags Vulkan pipeline cache create flags.
	/// @return VkPipelineCacheCreateInfo.
	[[nodiscard]] constexpr VkPipelineCacheCreateInfo PipelineCacheCreateInfo(std::span<const u8> data,
	                                                                          VkPipelineCacheCreateFlags flags) noexcept;

	/// @brief Create a Vulkan pipeline shader stage create info.
	/// @param stage Vulkan shader stage.
	/// @param module Vulkan shader module.
	/// @param name Shader entry point name.
	/// @return VkPipelineShaderStageCreateInfo.
	[[nodiscard]] VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module,
	                                                                            std::string_view name);

	/// @brief Create a Vulkan pipeline vertex input state create info.
	/// @param inputBindingCount Number of vertex input bindings.
	/// @param inputBindings Vulkan vertex input binding descrptions.
	/// @param inputAttributeCount Number of vertex input attributes.
	/// @param inputAttributes Vulkan vertex input attribute descriptions.
	/// @return VkPipelineVertexInputStateCreateInfo.
	[[nodiscard]] constexpr VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo(u32 inputBindingCount,
		                                                                                            const VkVertexInputBindingDescription* inputBindings,
		                                                                                            u32 inputAttributeCount,
		                                                                                            const VkVertexInputAttributeDescription* inputAttributes) noexcept;

	/// @brief Create a Vulkan pipeline input assembly state create info.
	/// @param topology Vulkan primitive topology.
	/// @param primitiveRestart True to enable primitive restart.
	/// @return VkPipelineInputAssemblyStateCreateInfo.
	[[nodiscard]] constexpr VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology,
	                                                                                                    bool primitiveRestart) noexcept;

	/// @brief Create a Vulkan pipeline tessellation state create info.
	/// @param patchControlPoints Number of patch control points.
	/// @return VkPipelineTessellationStateCreateInfo.
	[[nodiscard]] constexpr VkPipelineTessellationStateCreateInfo PipelineTessellationStateCreateInfo(u32 patchControlPoints) noexcept;

	/// @brief Create a Vulkan pipeline viewport state create info.
	/// @return VkPipelineViewportStateCreateInfo.
	[[nodiscard]] constexpr VkPipelineViewportStateCreateInfo PipelineViewportStateCreateInfo() noexcept;

	/// @brief Create a Vulkan pipeline multisample state create info.
	/// @param sampleCount Number of samples.
	/// @param sampleShading True to enable sample shading.
	/// @param sampleShadingRate Rate used for sample shading if enabled.
	/// @return VkPipelineMultisampleStateCreateInfo.
	[[nodiscard]] constexpr VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo(VkSampleCountFlagBits sampleCount,
	                                                                                                bool sampleShading,
																			                        f32 sampleShadingRate) noexcept;

	/// @brief Create a Vulkan fragment shading rate state create info.
	/// @param fragmentSize Shading rate/Fragment size to use.
	/// @param rateCombiners Shading rate combiners.
	/// @return VkPipelineFragmentShadingRateStateCreateInfoKHR
	[[nodiscard]] constexpr VkPipelineFragmentShadingRateStateCreateInfoKHR PipelineFragmentShadingRateStateCreateInfo(VkExtent2D fragmentSize,
	                                                                                                                   const std::array<VkFragmentShadingRateCombinerOpKHR, 2>& rateCombiners) noexcept;

	/// @brief Create a Vulkan pipeline dynamic state create info.
	/// @param dynamicStates Vulkan dynamic state to use.
	/// @return VkPipelineDynamicStateCreateInfo.
	[[nodiscard]] constexpr VkPipelineDynamicStateCreateInfo PipelineDynamicStateCreateInfo(const std::vector<VkDynamicState>& dynamicStates) noexcept;

	/// @brief Create a Vulkan compute pipeline create info.
	/// @param stage Vulkan pipeline shader stage create info.
	/// @param layout Vulkan pipeline layout.
	/// @return VkComputePipelineCreateInfo.
	[[nodiscard]] VkComputePipelineCreateInfo ComputePipelineCreateInfo(const VkPipelineShaderStageCreateInfo& stage,
	                                                                    VkPipelineLayout layout);

	/// @brief Create a Vulkan graphics pipeline create info.
	/// @param stageCount Number of shader stages.
	/// @param stages Pointer to Vulkan pipeline shader stage create info(s).
	/// @param vi Vulkan pipeline vertex input state create info.
	/// @param ia Vulkan pipeline input assembly state create info.
	/// @param vs Vulkan pipeline viewport state create info.
	/// @param rs Vulkan pipeline rasterization state create info.
	/// @param ms Vulkan pipeline multisample state create info.
	/// @param ds Vulkan pipeline depth stencil state create info.
	/// @param cb Vulkan pipeline color blend state create info.
	/// @param dy Vulkan pipeline dynamic state create info.
	/// @param layout Vulkan pipeline layout.
	/// @param renderPass Vulkan render pass.
	/// @return VkGraphicsPipelineCreateInfo.
	[[nodiscard]] constexpr VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo(u32 stageCount,
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

	/// @brief Create a Vulkan submit info.
	/// @param waitSemaphores Vulkan semaphore(s) to wait on.
	/// @param waitCount Vulkan semaphore wait count.
	/// @param waitMasks Vulkan pipeline stage(s) to wait on.
	/// @param cmds Vulkan command buffer(s) to submit.
	/// @param signalSemaphore Vulkan semaphore to signal.
	/// @param signalCount Vulkan semaphore signal count.
	/// @return VkSubmitInfo.
	[[nodiscard]] constexpr VkSubmitInfo SubmitInfo(const std::vector<VkSemaphore>& waitSemaphores,
							                        u32 waitCount,
	                                                const std::vector<VkPipelineStageFlags>& waitMasks,
	                                                const std::vector<VkCommandBuffer>& cmds,
	                                                const std::vector<VkSemaphore>& signalSemaphore,
							                        u32 signalCount) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan query pool create info.
	/// @param count Number of queries to manage by the pool.
	/// @param queryType Vulkan query type.
	/// @return VkQueryPoolCreateInfo.
	[[nodiscard]] constexpr VkQueryPoolCreateInfo QueryPoolCreateInfo(u32 count, VkQueryType queryType) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan swapchain create info.
	/// @param surface Vulkan surface to use.
	/// @param imageCount Number of images to create.
	/// @param surfaceFormat Vulkan surface format to use.
	/// @param imageExtent Vulkan image extent to use.
	/// @param sharingMode Vulkan sharing mode to use.
	/// @param queueFamilyIndices
	/// Queue families that have access to the image(s) of the swapchain when sharingMode is VK_SHARING_MODE_CONCURRENT.
	/// @param preTransform Vulkan pre-transform to use.
	/// @param compositeAlpha Vulkan composite alpha to use.
	/// @param presentMode Vulkan present mode to use.
	/// @param oldSwapchain Old Vulkan swapchain to aid in resource reusage. Default. VK_NULL_HANDLE
	/// @return VkSwapchainCreateInfoKHR.
	[[nodiscard]] VkSwapchainCreateInfoKHR SwapchainCreateInfoKHR(VkSurfaceKHR surface, u32 imageCount,
	                                                              VkSurfaceFormatKHR surfaceFormat, VkExtent2D imageExtent,
	                                                              VkSharingMode sharingMode,
	                                                              const std::vector<u32>& queueFamilyIndices,
	                                                              VkSurfaceTransformFlagBitsKHR preTransform,
													              VkCompositeAlphaFlagBitsKHR compositeAlpha,
													              VkPresentModeKHR presentMode,
																  VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan present info.
	/// @param waitSemaphores Vulkan semaphore(s) to wait on.
	/// @param swapChain Vulkan swapchain to present.
	/// @param presentIndex Vulkan swapchain image index to present.
	/// @return VkPresentInfoKHR.
	[[nodiscard]] constexpr VkPresentInfoKHR PresentInfo(const std::vector<VkSemaphore>& waitSemaphores, const VkSwapchainKHR& swapChain,
	                                                     u32& presentIndex) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Create a Vulkan memory allocate info.
	/// @param allocSize Size of the allocation in bytes.
	/// @param memoryTypeIndex Index of the memory type to allocate.
	/// @return VkMemoryAllocateInfo.
	[[nodiscard]] constexpr VkMemoryAllocateInfo MemoryAllocateInfo(VkDeviceSize allocSize, u32 memoryTypeIndex) noexcept;

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
		.enabledLayerCount = NumericCast<u32>(instanceLayers.size()),
		.ppEnabledLayerNames = instanceLayers.data(),
		.enabledExtensionCount = NumericCast<u32>(instanceExtensions.size()),
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
		.bindingCount = NumericCast<u32>(bindings.size()),
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
		.inputAttachmentCount = NumericCast<u32>(inputAttachments.size()),
		.pInputAttachments = inputAttachments.data(),
		.colorAttachmentCount = NumericCast<u32>(colorAttachments.size()),
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
		.inputAttachmentCount = NumericCast<u32>(inputAttachments.size()),
		.pInputAttachments = inputAttachments.data(),
		.colorAttachmentCount = NumericCast<u32>(colorAttachments.size()),
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
		.inputAttachmentCount = NumericCast<u32>(inputAttachments.size()),
		.pInputAttachments = inputAttachments.data(),
		.colorAttachmentCount = NumericCast<u32>(colorAttachments.size()),
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
		.inputAttachmentCount = NumericCast<u32>(inputAttachments.size()),
		.pInputAttachments = inputAttachments.data(),
		.colorAttachmentCount = NumericCast<u32>(colorAttachments.size()),
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
		.attachmentCount = NumericCast<u32>(attachmentDescriptions.size()),
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
		.attachmentCount = NumericCast<u32>(attachmentDescriptions.size()),
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

[[nodiscard]] constexpr VkCommandPoolCreateInfo TRAP::Graphics::API::VulkanInits::CommandPoolCreateInfo(const u32 queueFamilyIndex) noexcept
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

[[nodiscard]] constexpr VkCommandBufferBeginInfo TRAP::Graphics::API::VulkanInits::CommandBufferBeginInfo(const bool oneTimeSubmit) noexcept
{
	VkCommandBufferUsageFlags usageFlags = 0u;
	if(oneTimeSubmit)
		usageFlags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	return VkCommandBufferBeginInfo
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = usageFlags,
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
	                                                                                            const f32 mipLodBias,
																		                        const f32 minLod,
																		                        const f32 maxLod,
	                                                                                            const f32 maxAnisotropy,
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

[[nodiscard]] constexpr VkPipelineLayoutCreateInfo TRAP::Graphics::API::VulkanInits::PipelineLayoutCreateInfo(const u32 layoutCount,
	                                                                                                          const VkDescriptorSetLayout* const layouts,
	                                                                                                          const u32 pushConstantRangeCount,
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
		.attachmentCount = NumericCast<u32>(attachments.size()),
		.pAttachments = attachments.data(),
		.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineColorBlendStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineColorBlendStateCreateInfo(const VkLogicOp logicOp,
	                                                                                                                            const std::vector<VkPipelineColorBlendAttachmentState>& attachments,
	                                                                                                                            const f32 blendConstR,
	                                                                                                                            const f32 blendConstG,
	                                                                                                                            const f32 blendConstB,
	                                                                                                                            const f32 blendConstA) noexcept
{
	return VkPipelineColorBlendStateCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.logicOpEnable = VK_TRUE,
		.logicOp = logicOp,
		.attachmentCount = NumericCast<u32>(attachments.size()),
		.pAttachments = attachments.data(),
		.blendConstants = {blendConstR, blendConstG, blendConstB, blendConstA}
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineCacheCreateInfo TRAP::Graphics::API::VulkanInits::PipelineCacheCreateInfo(const std::span<const u8> data,
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
		.dynamicStateCount = NumericCast<u32>(dynamicStates.size()),
		.pDynamicStates = dynamicStates.data(),
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkGraphicsPipelineCreateInfo TRAP::Graphics::API::VulkanInits::GraphicsPipelineCreateInfo(const u32 stageCount,
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
														                          const u32 waitCount,
                                                                                  const std::vector<VkPipelineStageFlags>& waitMasks,
                                                                                  const std::vector<VkCommandBuffer>& cmds,
                                                                                  const std::vector<VkSemaphore>& signalSemaphores,
														                          const u32 signalCount) noexcept
{
	return VkSubmitInfo
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = waitCount,
		.pWaitSemaphores = waitSemaphores.data(),
		.pWaitDstStageMask = waitMasks.data(),
		.commandBufferCount = NumericCast<u32>(cmds.size()),
		.pCommandBuffers = cmds.data(),
		.signalSemaphoreCount = signalCount,
		.pSignalSemaphores = signalSemaphores.data()
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkQueryPoolCreateInfo TRAP::Graphics::API::VulkanInits::QueryPoolCreateInfo(const u32 count,
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
															                           u32& presentIndex) noexcept
{
	return VkPresentInfoKHR
	{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = nullptr,
		.waitSemaphoreCount = NumericCast<u32>(waitSemaphores.size()),
		.pWaitSemaphores = !waitSemaphores.empty() ? waitSemaphores.data() : nullptr,
		.swapchainCount = 1,
		.pSwapchains = &swapChain,
		.pImageIndices = &presentIndex,
		.pResults = nullptr
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkMemoryAllocateInfo TRAP::Graphics::API::VulkanInits::MemoryAllocateInfo(const VkDeviceSize allocSize,
                                                                                                  const u32 memoryTypeIndex) noexcept
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
			clearColor.float32[0] = TRAP::Math::Clamp(NumericCast<f32>(color.Red), 0.0f, 1.0f);
			clearColor.float32[1] = TRAP::Math::Clamp(NumericCast<f32>(color.Green), 0.0f, 1.0f);
			clearColor.float32[2] = TRAP::Math::Clamp(NumericCast<f32>(color.Blue), 0.0f, 1.0f);
			clearColor.float32[3] = TRAP::Math::Clamp(NumericCast<f32>(color.Alpha), 0.0f, 1.0f);
		}
		else /*if(ImageFormatIsSigned(format))*/
		{
			clearColor.float32[0] = TRAP::Math::Clamp(NumericCast<f32>(color.Red), -1.0f, 1.0f);
			clearColor.float32[1] = TRAP::Math::Clamp(NumericCast<f32>(color.Green), -1.0f, 1.0f);
			clearColor.float32[2] = TRAP::Math::Clamp(NumericCast<f32>(color.Blue), -1.0f, 1.0f);
			clearColor.float32[3] = TRAP::Math::Clamp(NumericCast<f32>(color.Alpha), -1.0f, 1.0f);
		}
	}
	else /*if(!ImageFormatIsNormalized(format))*/
	{
		if(!ImageFormatIsFloat(format))
		{
			if(!ImageFormatIsSigned(format))
			{
				clearColor.uint32[0] = NumericCast<u32>(TRAP::Math::Round(color.Red));
				clearColor.uint32[1] = NumericCast<u32>(TRAP::Math::Round(color.Green));
				clearColor.uint32[2] = NumericCast<u32>(TRAP::Math::Round(color.Blue));
				clearColor.uint32[3] = NumericCast<u32>(TRAP::Math::Round(color.Alpha));
			}
			else /*if(ImageFormatIsSigned(format))*/
			{
				clearColor.int32[0] = NumericCast<i32>(TRAP::Math::Round(color.Red));
				clearColor.int32[1] = NumericCast<i32>(TRAP::Math::Round(color.Green));
				clearColor.int32[2] = NumericCast<i32>(TRAP::Math::Round(color.Blue));
				clearColor.int32[3] = NumericCast<i32>(TRAP::Math::Round(color.Alpha));
			}
		}
		else
		{
			clearColor.float32[0] = NumericCast<f32>(color.Red);
			clearColor.float32[1] = NumericCast<f32>(color.Green);
			clearColor.float32[2] = NumericCast<f32>(color.Blue);
			clearColor.float32[3] = NumericCast<f32>(color.Alpha);
		}
	}

	return clearColor;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPipelineVertexInputStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineVertexInputStateCreateInfo(const u32 inputBindingCount,
	                                                                                                                              const VkVertexInputBindingDescription* const inputBindings,
	                                                                                                                              const u32 inputAttributeCount,
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

[[nodiscard]] constexpr VkPipelineTessellationStateCreateInfo TRAP::Graphics::API::VulkanInits::PipelineTessellationStateCreateInfo(const u32 patchControlPoints) noexcept
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
																										                          const f32 sampleShadingRate) noexcept
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