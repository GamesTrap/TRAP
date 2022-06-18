#include "TRAPPCH.h"

#include "VulkanRenderPass.h"

#include "VulkanDevice.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/Internals/VulkanInitializers.h"
#include "VulkanSurface.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSwapchain.h"

TRAP::Graphics::API::Vulkan::RenderPass::RenderPass(const Scope<Device>& device, Surface& surface)
	: m_renderPass(), m_device(device.get()), m_clearValues({ {{{0.1f, 0.1f, 0.1f, 1.0f}}} }), m_recording(false)
{
	//std::vector<VkAttachmentDescription> attachments(2);
	std::vector<VkAttachmentDescription> attachments(1);
	//Color attachment
	attachments[0].flags = 0;
	attachments[0].format = surface.GetOptimalSurfaceFormat().format;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	//Depth attachment
	/*attachments[1].flags = 0;
	attachments[1].format = *surface.GetOptimalDepthFormat();
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;*/

	VkAttachmentReference colorReference
	{
		0,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	/*VkAttachmentReference depthReference
	{
		1,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};*/

	VkSubpassDescription subPassDescription
	{
		0,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0,
		nullptr,
		1,
		&colorReference,
		nullptr,
		//&depthReference,
		nullptr,
		0,
		nullptr
	};

	//SubPass dependencies for layout transitions
	std::vector<VkSubpassDependency> dependencies(1);
	//std::vector<VkSubpassDependency> dependencies(2);

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = 0;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = 0;

	VkRenderPassCreateInfo info = Initializers::RenderPassCreateInfo(attachments, subPassDescription, dependencies);

	VkCall(vkCreateRenderPass(m_device->GetDevice(), &info, nullptr, &m_renderPass));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::RenderPass::~RenderPass()
{
	if(m_renderPass)
	{
		vkDestroyRenderPass(m_device->GetDevice(), m_renderPass, nullptr);
		m_renderPass = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkRenderPass& TRAP::Graphics::API::Vulkan::RenderPass::GetRenderPass()
{
	return m_renderPass;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::RenderPass::SetClearColor(const VkClearColorValue& color)
{
	m_clearValues[0].color = color;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::RenderPass::StartRenderPass(const Scope<CommandBuffer>& graphicsCommandBuffer)
{
	if (!m_recording)
	{
		VkRenderPassBeginInfo renderPassBeginInfo = Initializers::RenderPassBeginInfo(m_renderPass,
			VulkanRenderer::GetCurrentSwapchain().GetFrameBuffers()[VulkanRenderer::GetCurrentSwapchain().GetCurrentFrameBufferIndex()],
			VulkanRenderer::GetCurrentSwapchain().GetExtent().width,
			VulkanRenderer::GetCurrentSwapchain().GetExtent().height,
			m_clearValues);

		vkCmdBeginRenderPass(graphicsCommandBuffer->GetCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		m_recording = true;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::RenderPass::StartRenderPass(const Scope<Swapchain>& swapchain, CommandBuffer& graphicsCommandBuffer)
{
	if (swapchain && !m_recording)
	{
		VkRenderPassBeginInfo renderPassBeginInfo = Initializers::RenderPassBeginInfo(m_renderPass,
			swapchain->GetFrameBuffers()[swapchain->GetCurrentFrameBufferIndex()],
			swapchain->GetExtent().width,
			swapchain->GetExtent().height,
			m_clearValues);

		vkCmdBeginRenderPass(graphicsCommandBuffer.GetCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		m_recording = true;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::RenderPass::EndRenderPass(const Scope<CommandBuffer>& graphicsCommandBuffer)
{
	if(m_recording)
	{
		vkCmdEndRenderPass(graphicsCommandBuffer->GetCommandBuffer());

		m_recording = false;
	}
}