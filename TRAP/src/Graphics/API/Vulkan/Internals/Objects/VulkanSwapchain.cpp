#include "TRAPPCH.h"

#include "VulkanSwapchain.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanPipeline.h"
#include "VulkanCommandPool.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/Internals/VulkanInitializers.h"
#include "VulkanSemaphore.h"
#include "VulkanCommandBuffer.h"
#include "VulkanFence.h"

TRAP::Graphics::API::Vulkan::Swapchain::Swapchain(INTERNAL::WindowingAPI::InternalWindow* window, Scope<Surface>& surface, const Scope<Device>& device)
	: m_boundWindow(window),
	  m_swapchain(),
	  m_presentMode(surface->GetOptimalPresentMode()),
	  m_surface(std::move(surface)),
	  m_extent(),
      m_device(device.get()),
	  m_oldSwapchain(nullptr),
	  m_currentFrameBuffer(0)
{
	const VkSurfaceCapabilitiesKHR capabilities = m_surface->GetSurfaceCapabilities();

	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		m_extent = capabilities.currentExtent;
	else
	{
		m_extent = { static_cast<uint32_t>(m_boundWindow->Width), static_cast<uint32_t>(m_boundWindow->Height) };

		m_extent.width = Math::Max(capabilities.minImageExtent.width, Math::Min(capabilities.maxImageExtent.width, m_extent.width));
		m_extent.height = Math::Max(capabilities.minImageExtent.height, Math::Min(capabilities.maxImageExtent.height, m_extent.height));
	}

	m_imageCount = capabilities.minImageCount + 1;
	//Clamp Image Count
	if (capabilities.maxImageCount > 0 && m_imageCount > capabilities.maxImageCount)
		m_imageCount = capabilities.maxImageCount;

	//Synchronization Stuff
	//Fences
	m_waitFence = MakeScope<Fence>(m_device, true);
	//Semaphores
	m_presentCompleteSemaphore = MakeScope<Semaphore>(m_device);
	m_renderCompleteSemaphore = MakeScope<Semaphore>(m_device);

	m_graphicsCommandPool = MakeScope<CommandPool>(device, device->GetQueueFamilyIndices().GraphicsIndices);
	
	m_renderPass = MakeScope<RenderPass>(device, *m_surface);
	
	m_pipeline = MakeScope<Pipeline>(device, m_renderPass.get());
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Swapchain::~Swapchain()
{
	if (m_swapchain)
	{
		for (VkFramebuffer& frameBuffer : m_swapchainFrameBuffers)
			vkDestroyFramebuffer(m_device->GetDevice(), frameBuffer, nullptr);
		m_swapchainFrameBuffers.clear();

		for (VkImageView& imageView : m_swapchainImageViews)
			vkDestroyImageView(m_device->GetDevice(), imageView, nullptr);
		m_swapchainImageViews.clear();

		//Sync
		//Fences
		m_waitFence.reset();
		//Semaphores
		m_presentCompleteSemaphore.reset();
		m_renderCompleteSemaphore.reset();

		//Swapchain
		vkDestroySwapchainKHR(m_device->GetDevice(), m_swapchain, nullptr);
		m_swapchain = nullptr;
		m_swapchainImages.clear();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::Swapchain::SetVsync(const bool vsync)
{
	//Check if VSync even changed
	if (vsync && m_presentMode == VK_PRESENT_MODE_FIFO_KHR)
		return;
	if ((!vsync) && m_presentMode == m_surface->GetOptimalPresentMode())
		return;

	//Apply VSync change
	if (vsync)
		m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
	else
		m_presentMode = m_surface->GetOptimalPresentMode();

	if (m_swapchain)
	{
		//Stop CommandBuffer recording & Stop RenderPass
		EndGraphicsCommandBufferAndRenderPass();
		
		//Recreate swapchain with new present mode
		RecreateSwapchain();

		//Prepare next frame
		PrepareNextFrame();

		//Start Recording
		StartGraphicCommandBuffersAndRenderPass();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::WindowingAPI::InternalWindow* TRAP::Graphics::API::Vulkan::Swapchain::GetBoundWindow() const
{
	return m_boundWindow;
}

//-------------------------------------------------------------------------------------------------------------------//

VkSwapchainKHR& TRAP::Graphics::API::Vulkan::Swapchain::GetSwapchain()
{
	return m_swapchain;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Surface& TRAP::Graphics::API::Vulkan::Swapchain::GetSurface() const
{
	return *m_surface;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::RenderPass& TRAP::Graphics::API::Vulkan::Swapchain::GetRenderPass() const
{
	return *m_renderPass;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Pipeline& TRAP::Graphics::API::Vulkan::Swapchain::GetPipeline() const
{
	return *m_pipeline;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<VkFramebuffer>& TRAP::Graphics::API::Vulkan::Swapchain::GetFrameBuffers()
{
	return m_swapchainFrameBuffers;
}

//-------------------------------------------------------------------------------------------------------------------//

VkExtent2D TRAP::Graphics::API::Vulkan::Swapchain::GetExtent() const
{
	return m_extent;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::CommandPool& TRAP::Graphics::API::Vulkan::Swapchain::GetGraphicsCommandPool() const
{
	return *m_graphicsCommandPool;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::CommandBuffer& TRAP::Graphics::API::Vulkan::Swapchain::GetGraphicsCommandBuffer() const
{
	return *m_graphicsCommandBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

VkResult TRAP::Graphics::API::Vulkan::Swapchain::AcquireNextImage(const Scope<Semaphore>& presentCompleteSemaphore)
{
	return vkAcquireNextImageKHR(m_device->GetDevice(), m_swapchain, std::numeric_limits<uint64_t>::max(), presentCompleteSemaphore->GetSemaphore(), static_cast<VkFence>(nullptr), &m_currentFrameBuffer);
}

//-------------------------------------------------------------------------------------------------------------------//

VkResult TRAP::Graphics::API::Vulkan::Swapchain::QueuePresent(VkQueue& presentQueue, const Scope<Semaphore>& waitSemaphore)
{	
	VkPresentInfoKHR presentInfo = Initializers::PresentInfo(waitSemaphore->GetSemaphore(), m_swapchain, m_currentFrameBuffer);
	
	return vkQueuePresentKHR(presentQueue, &presentInfo);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t& TRAP::Graphics::API::Vulkan::Swapchain::GetCurrentFrameBufferIndex()
{
	return m_currentFrameBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::Swapchain::CreateSwapchain()
{
	//Swapchain
	VkSwapchainCreateInfoKHR info = Initializers::SwapchainCreateInfo(m_surface->GetSurface(), m_surface->GetOptimalSurfaceFormat(),
	                                                                  m_imageCount, m_extent, m_device->GetQueueFamilyIndices(),
	                                                                  m_surface->GetSurfaceCapabilities().currentTransform,
	                                                                  m_presentMode, m_surface->GetSurfaceCapabilities().supportedCompositeAlpha, m_oldSwapchain);
	VkCall(vkCreateSwapchainKHR(m_device->GetDevice(), &info, nullptr, &m_swapchain));

	//Swapchain Images
	VkCall(vkGetSwapchainImagesKHR(m_device->GetDevice(), m_swapchain, &m_imageCount, nullptr));
	m_swapchainImages.resize(m_imageCount);
	VkCall(vkGetSwapchainImagesKHR(m_device->GetDevice(), m_swapchain, &m_imageCount, m_swapchainImages.data()));

	//Swapchain Image Views
	m_swapchainImageViews.resize(m_swapchainImages.size());
	for(uint32_t i = 0; i < m_swapchainImageViews.size(); i++)
	{
		VkImageViewCreateInfo imageViewCreateInfo = Initializers::ImageViewCreateInfo(m_swapchainImages[i], VK_IMAGE_VIEW_TYPE_2D, m_surface->GetOptimalSurfaceFormat().format);
		VkCall(vkCreateImageView(m_device->GetDevice(), &imageViewCreateInfo, nullptr, &m_swapchainImageViews[i]));
	}

	//Frame Buffers
	m_swapchainFrameBuffers.resize(m_imageCount);
	for (uint32_t i = 0; i < m_swapchainFrameBuffers.size(); i++)
	{
		VkFramebufferCreateInfo frameBufferCreateInfo = Initializers::FrameBufferCreateInfo(m_renderPass->GetRenderPass(), m_swapchainImageViews[i], m_extent);
		VkCall(vkCreateFramebuffer(m_device->GetDevice(), &frameBufferCreateInfo, nullptr, &m_swapchainFrameBuffers[i]));
	}

	//CommandBuffer
	if(!m_graphicsCommandBuffer)
		m_graphicsCommandBuffer = m_graphicsCommandPool->AllocateCommandBuffer();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::Swapchain::RecreateSwapchain()
{
    //Get new extent from surface capabilities
    m_extent = m_surface->GetSurfaceCapabilities().currentExtent;

	//Only recreate swapchain if new size is > 0
	//This should not be triggered normally
	if (m_extent.width == 0 || m_extent.height == 0)
		return;
	
	//Make sure resources are not in use
	m_device->WaitIdle();

	m_graphicsCommandPool->FreeCommandBuffer(std::move(m_graphicsCommandBuffer));
	m_graphicsCommandBuffer = nullptr;

	//Destroy old Swapchain
	for (VkFramebuffer& frameBuffer : m_swapchainFrameBuffers)
		vkDestroyFramebuffer(m_device->GetDevice(), frameBuffer, nullptr);
	m_swapchainFrameBuffers.clear();

	for (VkImageView& imageView : m_swapchainImageViews)
		vkDestroyImageView(m_device->GetDevice(), imageView, nullptr);
	m_swapchainImageViews.clear();
	
	m_oldSwapchain = m_swapchain;

	//Destroy Sync objects
	//Fences
	m_waitFence.reset();
	//Semaphores
	m_presentCompleteSemaphore.reset();
	m_renderCompleteSemaphore.reset();

	//Recreate Sync objects
	//Fences
	m_waitFence = MakeScope<Fence>(m_device, true);
	//Semaphores
	m_presentCompleteSemaphore = MakeScope<Semaphore>(m_device);
	m_renderCompleteSemaphore = MakeScope<Semaphore>(m_device);
	
	//Create new Swapchain
	CreateSwapchain();
	m_oldSwapchain = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::Swapchain::PrepareNextFrame()
{
	const VkResult acquire = AcquireNextImage(m_presentCompleteSemaphore);
	if ((acquire == VK_ERROR_OUT_OF_DATE_KHR) || (acquire == VK_SUBOPTIMAL_KHR))
	{
		RecreateSwapchain();
		PrepareNextFrame();
		
		return;
	}

	VkCall(acquire)

	//Sync
	VkCall(vkWaitForFences(m_device->GetDevice(), 1, &m_waitFence->GetFence(), VK_TRUE, std::numeric_limits<uint64_t>::max()));
	VkCall(vkResetFences(m_device->GetDevice(), 1, &m_waitFence->GetFence()));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::Swapchain::StartGraphicCommandBuffersAndRenderPass() const
{
	if(!m_graphicsCommandBuffer->IsRecording())
	{
		m_graphicsCommandBuffer->StartRecording();
		m_renderPass->StartRenderPass(m_graphicsCommandBuffer);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::Vulkan::Swapchain::EndGraphicsCommandBufferAndRenderPass() const
{
	if(m_graphicsCommandBuffer->IsRecording())
	{
		m_renderPass->EndRenderPass(m_graphicsCommandBuffer);
		m_graphicsCommandBuffer->EndRecording();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::API::Vulkan::Fence>& TRAP::Graphics::API::Vulkan::Swapchain::GetWaitFence() const
{
	return m_waitFence;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::API::Vulkan::Semaphore>& TRAP::Graphics::API::Vulkan::Swapchain::GetRenderSemaphore() const
{
	return m_renderCompleteSemaphore;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Scope<TRAP::Graphics::API::Vulkan::Semaphore>& TRAP::Graphics::API::Vulkan::Swapchain::GetPresentSemaphore() const
{
	return m_presentCompleteSemaphore;
}