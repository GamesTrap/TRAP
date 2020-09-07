#include "TRAPPCH.h"
#include "VulkanRenderer.h"

#include "Application.h"
#include "VulkanCommon.h"
#include "Graphics/RenderCommand.h"
#include "Window/Window.h"
#include "Window/WindowingAPI.h"

#include "VulkanContext.h"
#include "Internals/VulkanInitializers.h"
#include "Internals/Objects/VulkanInstance.h"
#include "Internals/Objects/VulkanPhysicalDevice.h"
#include "Internals/Objects/VulkanDebug.h"
#include "Internals/Objects/VulkanSurface.h"
#include "Internals/Objects/VulkanDevice.h"
#include "Internals/Objects/VulkanSwapchain.h"
#include "Internals/Objects/VulkanRenderPass.h"
#include "Internals/Objects/VulkanFence.h"
#include "Internals/Objects/VulkanSemaphore.h"
#include "Internals/Objects/VulkanCommandBuffer.h"
#include "Utils/Utils.h"

TRAP::Graphics::API::VulkanRenderer* TRAP::Graphics::API::VulkanRenderer::s_renderer = nullptr;
TRAP::Graphics::API::Vulkan::Swapchain* TRAP::Graphics::API::VulkanRenderer::s_currentSwapchain = nullptr;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer::VulkanRenderer()
	: m_debugCallbackSupported(false), m_debug(nullptr)
{
	TP_PROFILE_FUNCTION();
	s_renderer = this;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer::~VulkanRenderer()
{
	TP_PROFILE_FUNCTION();
	
	TP_DEBUG(Log::RendererVulkanPrefix, "Destroying Renderer");

	m_device->WaitIdle();

	//Free everything in order
	//Should happen automagically through Scope deconstructors
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::InitInternal()
{
	TP_PROFILE_FUNCTION();
	
	//Instance Stuff
	std::array<std::vector<std::string>, 2> instanceLayersAndExtensions = SetupInstanceLayersAndExtensions();
	m_instance = MakeScope<Vulkan::Instance>(VulkanContext::GetAppName().data(), VK_MAKE_VERSION(1, 0, 0), instanceLayersAndExtensions[0], instanceLayersAndExtensions[1]);

	//Debug Stuff
#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	if(m_debugCallbackSupported)
		m_debug = MakeScope<Vulkan::Debug>(m_instance);
#endif
	
	//Physical Device Stuff
	std::multimap<int32_t, Vulkan::PhysicalDevice> physicalDevices = Vulkan::PhysicalDevice::GetAllAvailableGraphicPhysicalDevices(
				m_instance, VulkanContext::GetCurrentWindow());
	for(auto& device : physicalDevices)
		m_deviceNamesAndUUIDs.emplace_back(device.second.GetPhysicalDeviceName(), device.second.GetUUID());
	const std::string_view GPUUUIDString = Application::GetConfig().Get<std::string_view>("VulkanGPU");
	const std::vector<uint8_t> GPUUUID = Utils::UUIDFromString(GPUUUIDString);
	if(GPUUUID.empty())
	{
		auto lastPhysicalDevice = (--physicalDevices.end())->second;
		m_physicalDevice = MakeScope<Vulkan::PhysicalDevice>(std::move(lastPhysicalDevice));
	}
	else if(GPUUUID.size() != 16)
	{
		TP_ERROR(Log::RendererVulkanPrefix, "Invalid GPU UUID: \"", GPUUUIDString, "\"!");
		TP_ERROR(Log::RendererVulkanPrefix, "Falling back to score based system");
		auto lastPhysicalDevice = (--physicalDevices.end())->second;
		m_physicalDevice = MakeScope<Vulkan::PhysicalDevice>(std::move(lastPhysicalDevice));
	}
	else
	{
		for(const auto& [score, physicalDevice] : physicalDevices)
		{
			bool sameGPU = true;
			std::vector<uint8_t> physicalDeviceUUID = physicalDevice.GetUUID();

			//Check if a GPU with the same UUID exists
			uint32_t i = 0;
			for(; i < physicalDeviceUUID.size(); i++)
			{
				if (GPUUUID[i] != physicalDeviceUUID[i])
				{
					sameGPU = false;
					break;
				}
			}
			
			if(sameGPU) //GPU UUID found so use it
			{
				m_physicalDevice = MakeScope<Vulkan::PhysicalDevice>(physicalDevice);
				break;
			}
		}

		if(!m_physicalDevice) //Fall back to scoring system if m_physicalDevice is still nullptr
		{
			TP_ERROR(Log::RendererVulkanPrefix, "Could not find a GPU with UUID: \"", GPUUUIDString, "\"!");
			TP_ERROR(Log::RendererVulkanPrefix, "Falling back to score based system");
			auto lastPhysicalDevice = (--physicalDevices.end())->second;
			m_physicalDevice = MakeScope<Vulkan::PhysicalDevice>(std::move(lastPhysicalDevice));
		}
	}

	//Device
	std::vector<std::string> deviceExtensions = SetupDeviceExtensions(m_physicalDevice);
	m_device = MakeScope<Vulkan::Device>(m_physicalDevice, deviceExtensions);
	
	//Main Window Surface
	Scope<Vulkan::Surface> surface = MakeScope<Vulkan::Surface>(m_instance, *m_physicalDevice, VulkanContext::GetCurrentWindow());
	
	//Main Window Swapchain
	m_swapchains.emplace_back(MakeScope<Vulkan::Swapchain>(VulkanContext::GetCurrentWindow(), surface, m_device));
	s_currentSwapchain = m_swapchains[0].get();
	s_currentSwapchain->CreateSwapchain();

	//Prepare next frame
	s_currentSwapchain->PrepareNextFrame();
	
	//Start CommandBuffers recording & Start RenderPass
	s_currentSwapchain->StartGraphicCommandBuffersAndRenderPass();

	//Set Viewport and scissor
	VkViewport viewport{ 0.0f, 0.0f, static_cast<float>(VulkanContext::GetCurrentWindow()->Width), static_cast<float>(VulkanContext::GetCurrentWindow()->Height), 0.0f, 1.0f };
	VkRect2D scissor{ {0, 0}, {static_cast<uint32_t>(VulkanContext::GetCurrentWindow()->Width), static_cast<uint32_t>(VulkanContext::GetCurrentWindow()->Height)} };
	s_currentSwapchain->GetGraphicsCommandBuffer().SetViewport(viewport);
	s_currentSwapchain->GetGraphicsCommandBuffer().SetScissor(scissor);
	
	SetDepthTesting(true);
	SetBlend(true);
	SetBlendFunction(RendererBlendFunction::Source_Alpha, RendererBlendFunction::One_Minus_Source_Alpha);
	//////////////////

	TP_INFO(Log::RendererVulkanPrefix, "----------------------------------");
	TP_INFO(Log::RendererVulkanPrefix, "Vulkan:");
	TP_INFO(Log::RendererVulkanPrefix, "Version:  ", VK_VERSION_MAJOR(m_physicalDevice->GetPhysicalDeviceProperties().apiVersion), '.', VK_VERSION_MINOR(m_physicalDevice->GetPhysicalDeviceProperties().apiVersion), '.', VK_VERSION_PATCH(m_physicalDevice->GetPhysicalDeviceProperties().apiVersion));
	TP_INFO(Log::RendererVulkanPrefix, "Renderer: ", m_physicalDevice->GetPhysicalDeviceName());
	TP_INFO(Log::RendererVulkanPrefix, "Driver:   ", VK_VERSION_MAJOR(m_physicalDevice->GetPhysicalDeviceProperties().driverVersion), '.', VK_VERSION_MINOR(m_physicalDevice->GetPhysicalDeviceProperties().driverVersion), '.', VK_VERSION_PATCH(m_physicalDevice->GetPhysicalDeviceProperties().driverVersion));
	TP_INFO(Log::RendererVulkanPrefix, "----------------------------------");

	m_rendererTitle = "[Vulkan " + std::to_string(VK_VERSION_MAJOR(m_physicalDevice->GetPhysicalDeviceProperties().apiVersion)) + '.' + std::to_string(VK_VERSION_MINOR(m_physicalDevice->GetPhysicalDeviceProperties().apiVersion)) + '.' + std::to_string(VK_VERSION_PATCH(m_physicalDevice->GetPhysicalDeviceProperties().apiVersion)) + ']';
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Clear(RendererBufferType buffer)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Present(const Scope<Window>& window)
{
	if (!window->IsMinimized())
	{
		if (window->GetInternalWindow() == VulkanContext::GetCurrentWindow())
		{
			//Ending recording CommandBuffers & RenderPass
			s_currentSwapchain->EndGraphicsCommandBufferAndRenderPass();

			//////////////
			//Presenting//
			//////////////
			//Pipeline stage at which the queue submission will wait (via pWaitSemaphores)
			VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

			VkSubmitInfo submitInfo = Vulkan::Initializers::SubmitInfo(s_currentSwapchain->GetPresentSemaphore()->GetSemaphore(),
				waitStageMask,
				s_currentSwapchain->GetGraphicsCommandBuffer().GetCommandBuffer(),
				s_currentSwapchain->GetRenderSemaphore()->GetSemaphore());

			//Submit queue
			vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, s_currentSwapchain->GetWaitFence()->GetFence());

			//Present
			const VkResult present = s_currentSwapchain->QueuePresent(m_device->GetPresentQueue(), s_currentSwapchain->GetRenderSemaphore());
			if (!((present == VK_SUCCESS) || (present == VK_SUBOPTIMAL_KHR)))
			{
				if (present == VK_ERROR_OUT_OF_DATE_KHR)
					s_currentSwapchain->RecreateSwapchain();
				else
				{
					VkCall(present);
				}
			}
			//////////////

			//Prepare next frame
			s_currentSwapchain->PrepareNextFrame();

			//Start CommandBuffer recording & Start RenderPass
			s_currentSwapchain->StartGraphicCommandBuffersAndRenderPass();
		}
		else
		{			
			//Use swapchain associated with the window for this time
			INTERNAL::WindowingAPI::InternalWindow* internalWindow = static_cast<INTERNAL::WindowingAPI::InternalWindow*>(window->GetInternalWindow());

			//Search for swapchain associated with the window
			int32_t index = -1;
			for (uint32_t i = 0; i < m_swapchains.size(); i++)
			{
				if (m_swapchains[i]->GetBoundWindow() == internalWindow)
				{
					index = i;
					break;
				}
			}

			//No Swapchain found with this window currently bound so create one
			if (index == -1)
			{
				//No swapchain was found :C
				Vulkan::Swapchain* swapchain = CreateWindowSwapchain(internalWindow);

				//Start CommandBuffer recording & Start RenderPass
				if (!m_swapchains[index]->GetGraphicsCommandBuffer().IsRecording())
				{
					m_swapchains[index]->GetGraphicsCommandBuffer().StartRecording();
					m_swapchains[index]->GetRenderPass().StartRenderPass(m_swapchains[index], m_swapchains[index]->GetGraphicsCommandBuffer());
				}

				//Set Viewport and scissor
				VkViewport viewport{ 0.0f, 0.0f, static_cast<float>(VulkanContext::GetCurrentWindow()->Width), static_cast<float>(VulkanContext::GetCurrentWindow()->Height), 0.0f, 1.0f };
				VkRect2D scissor{ {0, 0}, {static_cast<uint32_t>(VulkanContext::GetCurrentWindow()->Width), static_cast<uint32_t>(VulkanContext::GetCurrentWindow()->Height)} };
				swapchain->GetGraphicsCommandBuffer().SetViewport(viewport);
				swapchain->GetGraphicsCommandBuffer().SetScissor(scissor);

				index = static_cast<int32_t>(m_swapchains.size()) - 1;
			}

			//Present
			if (m_swapchains[index]->GetBoundWindow() == internalWindow)
			{
				//Ending recording CommandBuffers & RenderPass
				m_swapchains[index]->EndGraphicsCommandBufferAndRenderPass();

				//////////////
				//Presenting//
				//////////////
				//Pipeline stage at which the queue submission will wait (via pWaitSemaphores)
				VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

				VkSubmitInfo submitInfo = Vulkan::Initializers::SubmitInfo(m_swapchains[index]->GetPresentSemaphore()->GetSemaphore(),
					waitStageMask,
					m_swapchains[index]->GetGraphicsCommandBuffer().GetCommandBuffer(),
					m_swapchains[index]->GetRenderSemaphore()->GetSemaphore());

				//Submit queue
				vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, m_swapchains[index]->GetWaitFence()->GetFence());

				//Present
				const VkResult present = m_swapchains[index]->QueuePresent(m_device->GetPresentQueue(), m_swapchains[index]->GetRenderSemaphore());
				if (!((present == VK_SUCCESS) || (present == VK_SUBOPTIMAL_KHR)))
				{
					if (present == VK_ERROR_OUT_OF_DATE_KHR)
						m_swapchains[index]->RecreateSwapchain();
					else
					{
						VkCall(present);
					}
				}
				//////////////

				//Prepare next frame
				m_swapchains[index]->PrepareNextFrame();

				if (!m_swapchains[index]->GetGraphicsCommandBuffer().IsRecording())
				{
					m_swapchains[index]->GetGraphicsCommandBuffer().StartRecording();
					m_swapchains[index]->GetRenderPass().StartRenderPass(m_swapchains[index], m_swapchains[index]->GetGraphicsCommandBuffer());
				}
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetClearColor(const Math::Vec4& color)
{
	TP_PROFILE_FUNCTION();

	s_currentSwapchain->GetRenderPass().SetClearColor({{color[0], color[1], color[2], color[3]}});
}

//------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthTesting(bool enabled)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthMasking(bool enabled)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetDepthFunction(RendererFunction function)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilTesting(bool enabled)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilMasking(uint32_t mask)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilMaskingSeparate(RendererFaceMode face, uint32_t mask)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilFunction(RendererFunction function, int32_t reference, uint32_t mask)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilFunctionSeparate(RendererFaceMode face, RendererFunction function, int32_t reference, uint32_t mask)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilOperation(RendererOperation stencilFail, RendererOperation depthFail, RendererOperation pass)
{
	TP_PROFILE_FUNCTION();	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetStencilOperationSeparate(RendererFaceMode face, RendererOperation stencilFail, RendererOperation depthFail, RendererOperation pass)
{
	TP_PROFILE_FUNCTION();		
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlend(const bool enabled)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetCull(const bool enabled)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetFrontFace(const RendererFrontFace frontFace)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetWireFrame(const bool enabled)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
{
	TP_PROFILE_FUNCTION();

	//Only set if width and height are bigger than 0
	if (width == 0 || height == 0)
		return;

	//Set Viewport and scissor
	VkViewport viewport{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
	VkRect2D scissor{ {0, 0}, {static_cast<uint32_t>(width), static_cast<uint32_t>(height)} };
	s_currentSwapchain->GetGraphicsCommandBuffer().SetViewport(viewport);
	s_currentSwapchain->GetGraphicsCommandBuffer().SetScissor(scissor);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendFunction(const RendererBlendFunction source, const RendererBlendFunction destination)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendFunctionSeparate(RendererBlendFunction sourceRGB,
	                                                               RendererBlendFunction sourceAlpha,
	                                                               RendererBlendFunction destinationRGB,
	                                                               RendererBlendFunction destinationAlpha)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendEquation(RendererBlendEquation blendEquation)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetBlendEquationSeparate(RendererBlendEquation blendEquationRGB, RendererBlendEquation blendEquationAlpha)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetCullMode(const RendererFaceMode cullMode)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::DrawIndexed(const Scope<VertexArray>& vertexArray, uint32_t indexCount, const RendererPrimitive primitive)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Draw(const Scope<VertexArray>& vertexArray, const RendererPrimitive primitive)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::API::VulkanRenderer::GetTitle() const
{
	return m_rendererTitle;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::Graphics::API::VulkanRenderer::GetCurrentGPUUUID()
{
	return m_physicalDevice->GetUUID();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::API::VulkanRenderer::GetCurrentGPUName()
{
	return m_physicalDevice->GetPhysicalDeviceName();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::pair<std::string, std::vector<uint8_t>>> TRAP::Graphics::API::VulkanRenderer::GetAllGPUs()
{
	return m_deviceNamesAndUUIDs;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer* TRAP::Graphics::API::VulkanRenderer::Get()
{
	return s_renderer;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetVSyncIntervalInternal(const uint32_t interval)
{
	s_currentSwapchain->SetVsync(interval == 0 ? false : true);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<std::unique_ptr<TRAP::Graphics::API::Vulkan::Swapchain>>& TRAP::Graphics::API::VulkanRenderer::GetAllSwapchains()
{
	return Get()->m_swapchains;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::SetCurrentSwapchain(Vulkan::Swapchain* swapchain)
{
	s_currentSwapchain = swapchain;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Device& TRAP::Graphics::API::VulkanRenderer::GetDevice()
{
	return *Get()->m_device;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Swapchain& TRAP::Graphics::API::VulkanRenderer::GetCurrentSwapchain()
{
	return *s_currentSwapchain;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::PhysicalDevice& TRAP::Graphics::API::VulkanRenderer::GetPhysicalDevice()
{
	return *Get()->m_physicalDevice;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::BindGraphicsPipeline()
{
	s_currentSwapchain->GetGraphicsCommandBuffer().BindPipeline(s_currentSwapchain->GetPipeline(), VK_PIPELINE_BIND_POINT_GRAPHICS);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::Vulkan::Swapchain* TRAP::Graphics::API::VulkanRenderer::CreateWindowSwapchain(INTERNAL::WindowingAPI::InternalWindow* window)
{
	//Window Surface
	Scope<Vulkan::Surface> surface = MakeScope<Vulkan::Surface>(Get()->m_instance, *(Get()->m_physicalDevice), window);
	
	//Window Swapchain
	Get()->m_swapchains.emplace_back(MakeScope<Vulkan::Swapchain>(window, surface, Get()->m_device));

	//Create Swapchain
	Get()->m_swapchains.back()->CreateSwapchain();

	//Prepare next frame
	Get()->m_swapchains.back()->PrepareNextFrame();
	
	return Get()->m_swapchains.back().get();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::DeleteWindowSwapchain(INTERNAL::WindowingAPI::InternalWindow* window)
{
	for (uint32_t index = 0; index < Get()->m_swapchains.size(); index++)
	{
		if (Get()->m_swapchains[index]->GetBoundWindow() == window)
		{
			Get()->m_swapchains[index].reset();
			Get()->m_swapchains[index] = std::move(Get()->m_swapchains.back());
			Get()->m_swapchains.pop_back();

			return;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<std::vector<std::string>, 2> TRAP::Graphics::API::VulkanRenderer::SetupInstanceLayersAndExtensions()
{
	std::vector<std::string> instanceLayers{};
#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	if (Vulkan::Instance::IsLayerSupported("VK_LAYER_KHRONOS_validation"))
		instanceLayers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif

	std::array<std::string, 2> requiredInstanceExtensions = INTERNAL::WindowingAPI::GetRequiredInstanceExtensions();
	std::vector<std::string> instanceExtensions{ requiredInstanceExtensions[0], requiredInstanceExtensions[1] };
#if defined(TRAP_DEBUG) || defined(TRAP_RELWITHDEBINFO)
	if (Vulkan::Instance::IsExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
	{
		instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		m_debugCallbackSupported = true;
	}
#endif

	return { instanceLayers, instanceExtensions };
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Graphics::API::VulkanRenderer::SetupDeviceExtensions(const Scope<Vulkan::PhysicalDevice>& physicalDevice)
{
	std::vector<std::string> deviceExtensions{};

	//Swapchain
	//Must be supported or Engine would already have stopped
	if(physicalDevice->IsExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
		deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	//RayTracing
	//Nice addition :D
	if(physicalDevice->IsRayTracingSupported())
	{
		deviceExtensions.emplace_back("VK_KHR_ray_tracing");
		deviceExtensions.emplace_back("VK_KHR_deferred_host_operations");
		deviceExtensions.emplace_back("VK_KHR_pipeline_library");
	}
	
	return deviceExtensions;
}