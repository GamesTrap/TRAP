#include "TRAPPCH.h"
#include "VulkanDevice.h"

#include "Utils/String/String.h"
#include "VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Objects/AftermathTracker.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/Objects/VulkanSemaphore.h"

namespace
{
#ifdef ENABLE_GRAPHICS_DEBUG
	void SetDeviceName(const std::string_view name, VkDevice device)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

		if(!TRAP::Graphics::API::VulkanRenderer::s_debugMarkerSupport)
			return;

	#ifdef ENABLE_DEBUG_UTILS_EXTENSION
		TRAP::Graphics::API::VkSetObjectName(device, std::bit_cast<u64>(device), VK_OBJECT_TYPE_DEVICE, name);
	#else
		TRAP::Graphics::API::VkSetObjectName(device, std::bit_cast<u64>(device), VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT, name);
	#endif
	}
#endif /*ENABLE_GRAPHICS_DEBUG*/

#ifdef VERBOSE_GRAPHICS_DEBUG
	void DebugPrintActiveDeviceExtensions(const std::span<const std::string> deviceExtensions,
	                                      const TRAP::Graphics::API::VulkanPhysicalDevice& physicalDevice)
	{
		if (deviceExtensions.empty())
			return;

		TP_DEBUG(TRAP::Log::RendererVulkanDevicePrefix, "Loading Device Extension(s):");
		for (const std::string_view str : deviceExtensions)
		{
			const auto extensionProps = physicalDevice.GetPhysicalDeviceExtensionProperties(str);
			if(extensionProps)
			{
				TP_DEBUG(TRAP::Log::RendererVulkanDevicePrefix, "    ", str, " Spec: ",
						VK_API_VERSION_MAJOR(extensionProps->specVersion), '.', VK_API_VERSION_MINOR(extensionProps->specVersion), '.',
						VK_API_VERSION_PATCH(extensionProps->specVersion), '.', VK_API_VERSION_VARIANT(extensionProps->specVersion));
			}
			else
				TP_DEBUG(TRAP::Log::RendererVulkanDevicePrefix, "    ", str);
		}
	}
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	template<typename T>
	constexpr void LinkFeatureStruct(VkBaseOutStructure*& base, T& toLink)
	{
		base->pNext = reinterpret_cast<VkBaseOutStructure* const>(&toLink);
		base = base->pNext;
	}

	void EnableNsightAftermath(VkBaseOutStructure*& base, VkDeviceDiagnosticsConfigCreateInfoNV& diagnosticsCreateInfoNV)
	{
		if(TRAP::Graphics::RendererAPI::s_diagnosticCheckPointsSupport && TRAP::Graphics::RendererAPI::s_diagnosticsConfigSupport)
			TRAP::Graphics::RendererAPI::s_aftermathSupport = true;

		if(!TRAP::Graphics::RendererAPI::s_aftermathSupport)
			return;

		LinkFeatureStruct(base, diagnosticsCreateInfoNV);
		//Enable Nsight Aftermath GPU crash dump creation.
		//This needs to be done before the Vulkan device is created.
		TRAP::Graphics::AftermathTracker::Initialize();
	}

	void LoadShadingRateCaps(const VkPhysicalDeviceFragmentShadingRateFeaturesKHR& shadingRateFeatures, VkPhysicalDevice physicalDevice)
	{
		using namespace TRAP::Graphics::API;
		using RendererAPI = TRAP::Graphics::RendererAPI;

		if(!VulkanRenderer::s_shadingRate)
			return;

		if(shadingRateFeatures.pipelineFragmentShadingRate != 0u)
			RendererAPI::GPUSettings.ShadingRateCaps |= RendererAPI::ShadingRateCaps::PerDraw;
		if(shadingRateFeatures.primitiveFragmentShadingRate != 0u)
			RendererAPI::GPUSettings.ShadingRateCaps |= RendererAPI::ShadingRateCaps::PerPrimitive;
		if(shadingRateFeatures.attachmentFragmentShadingRate != 0u)
			RendererAPI::GPUSettings.ShadingRateCaps |= RendererAPI::ShadingRateCaps::PerTile;

		if(RendererAPI::GPUSettings.ShadingRateCaps == RendererAPI::ShadingRateCaps::NotSupported)
			return;

		VkPhysicalDeviceFragmentShadingRatePropertiesKHR fragmentShadingRateProperties{};
		fragmentShadingRateProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;
		VkPhysicalDeviceProperties2 deviceProperties2{};
		deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		deviceProperties2.pNext = &fragmentShadingRateProperties;
		vkGetPhysicalDeviceProperties2(physicalDevice, &deviceProperties2);

		RendererAPI::GPUSettings.ShadingRateTexelWidth = fragmentShadingRateProperties.maxFragmentShadingRateAttachmentTexelSize.width;
		RendererAPI::GPUSettings.ShadingRateTexelHeight = fragmentShadingRateProperties.maxFragmentShadingRateAttachmentTexelSize.height;

		RendererAPI::GPUSettings.ShadingRateCombiner |= RendererAPI::ShadingRateCombiner::Passthrough;
		RendererAPI::GPUSettings.ShadingRateCombiner |= RendererAPI::ShadingRateCombiner::Override;
		if(fragmentShadingRateProperties.fragmentShadingRateNonTrivialCombinerOps != 0u)
		{
			RendererAPI::GPUSettings.ShadingRateCombiner |= RendererAPI::ShadingRateCombiner::Min;
			RendererAPI::GPUSettings.ShadingRateCombiner |= RendererAPI::ShadingRateCombiner::Max;
			RendererAPI::GPUSettings.ShadingRateCombiner |= RendererAPI::ShadingRateCombiner::Sum;
		}

		u32 fragmentShadingRatesCount = 0;
		VkCall(vkGetPhysicalDeviceFragmentShadingRatesKHR(physicalDevice, &fragmentShadingRatesCount, nullptr));
		std::vector<VkPhysicalDeviceFragmentShadingRateKHR> fragmentShadingRates(fragmentShadingRatesCount);
		for(auto& rate : fragmentShadingRates)
		{
			rate.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_KHR;
			rate.pNext = nullptr;
		}
		VkCall(vkGetPhysicalDeviceFragmentShadingRatesKHR(physicalDevice, &fragmentShadingRatesCount, fragmentShadingRates.data()));
		for(const auto& rate : fragmentShadingRates)
		{
			if(rate.fragmentSize.width == 1 && rate.fragmentSize.height == 2)
				RendererAPI::GPUSettings.ShadingRates |= RendererAPI::ShadingRate::OneXTwo;
			if(rate.fragmentSize.width == 2 && rate.fragmentSize.height == 1)
				RendererAPI::GPUSettings.ShadingRates |= RendererAPI::ShadingRate::TwoXOne;
			if(rate.fragmentSize.width == 2 && rate.fragmentSize.height == 4)
				RendererAPI::GPUSettings.ShadingRates |= RendererAPI::ShadingRate::TwoXFour;
			if(rate.fragmentSize.width == 4 && rate.fragmentSize.height == 2)
				RendererAPI::GPUSettings.ShadingRates |= RendererAPI::ShadingRate::FourXTwo;
			if(rate.fragmentSize.width == 4 && rate.fragmentSize.height == 4)
				RendererAPI::GPUSettings.ShadingRates |= RendererAPI::ShadingRate::Quarter;
			if(rate.fragmentSize.width == 8 && rate.fragmentSize.height == 8)
				RendererAPI::GPUSettings.ShadingRates |= RendererAPI::ShadingRate::Eighth;
			if(rate.fragmentSize.width == 2 && rate.fragmentSize.height == 2)
				RendererAPI::GPUSettings.ShadingRates |= RendererAPI::ShadingRate::Half;
			if(rate.fragmentSize.width == 1 && rate.fragmentSize.height == 1)
				RendererAPI::GPUSettings.ShadingRates |= RendererAPI::ShadingRate::Full;
		}
	}

	void SetVulkanRendererFeatures(VkPhysicalDevice physicalDevice,
		                           const VkPhysicalDeviceBufferDeviceAddressFeaturesKHR& bufferDeviceAddressFeatures,
								   const VkPhysicalDeviceSamplerYcbcrConversionFeatures& ycbcrFeatures,
								   const VkPhysicalDeviceShaderDrawParametersFeatures& shaderDrawParametersFeatures,
								   const VkPhysicalDeviceFragmentShadingRateFeaturesKHR& shadingRateFeatures,
								   const VkPhysicalDeviceTimelineSemaphoreFeaturesKHR& timelineSemaphoreFeatures)
	{
		TRAP::Graphics::API::VulkanRenderer::s_debugMarkerSupport = ((vkCmdBeginDebugUtilsLabelEXT) != nullptr) && ((vkCmdEndDebugUtilsLabelEXT) != nullptr) &&
											                        ((vkCmdInsertDebugUtilsLabelEXT) != nullptr) && ((vkSetDebugUtilsObjectNameEXT) != nullptr);
		TRAP::Graphics::API::VulkanRenderer::s_bufferDeviceAddressExtension = (bufferDeviceAddressFeatures.bufferDeviceAddress != 0u);
		TRAP::Graphics::API::VulkanRenderer::s_samplerYcbcrConversionExtension = (ycbcrFeatures.samplerYcbcrConversion != 0u);
		TRAP::Graphics::API::VulkanRenderer::s_shaderDrawParameters = (shaderDrawParametersFeatures.shaderDrawParameters != 0u);
		TRAP::Graphics::API::VulkanRenderer::s_timelineSemaphore = (timelineSemaphoreFeatures.timelineSemaphore != 0u);
		LoadShadingRateCaps(shadingRateFeatures, physicalDevice);
	}

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	void InitNVIDIAReflex(VkSemaphore& reflexSemaphore, const TRAP::Graphics::RendererAPI::GPUVendor gpuVendor, VkDevice device)
	{
		if (gpuVendor == RendererAPI::GPUVendor::NVIDIA &&
			TRAP::Graphics::API::VulkanRenderer::s_timelineSemaphore &&
			TRAP::Utils::IsWindows10BuildOrGreaterWin32(10240))
		{
			const NvLL_VK_Status status = NvLL_VK_InitLowLatencyDevice(device, reinterpret_cast<HANDLE*>(&reflexSemaphore));
			VkReflexCall(status);
			if(status == NVLL_VK_OK)
				TRAP::Graphics::RendererAPI::GPUSettings.ReflexSupported = true;
		}
	}
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDevice::VulkanDevice(TRAP::Scope<VulkanPhysicalDevice> physicalDevice,
                                                std::vector<std::string> deviceExtensions)
	: m_physicalDevice(std::move(physicalDevice)), m_deviceExtensions(std::move(deviceExtensions))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_physicalDevice, "VulkanDevice(): Vulkan PhysicalDevice is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Creating Device");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	//Remove all extensions that are unsupported by this device
	std::erase_if(m_deviceExtensions, [this](const std::string_view extension){return !m_physicalDevice->IsExtensionSupported(extension);});

#ifdef VERBOSE_GRAPHICS_DEBUG
	DebugPrintActiveDeviceExtensions(m_deviceExtensions, *m_physicalDevice);
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if(std::ranges::contains(m_deviceExtensions, VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME))
		m_physicalDevice->LoadPhysicalDeviceFragmentShaderInterlockFeatures();

	//Enable features of the device extensions
	VkPhysicalDeviceFeatures2 devFeatures2{};
	devFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

	VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT fragmentShaderInterlockFeatures{};
	fragmentShaderInterlockFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT;
	VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptorIndexingFeatures{};
	descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
	VkPhysicalDeviceSamplerYcbcrConversionFeatures ycbcrFeatures{};
	ycbcrFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES;
	VkPhysicalDeviceShaderDrawParametersFeatures shaderDrawParametersFeatures{};
	shaderDrawParametersFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
	VkPhysicalDeviceBufferDeviceAddressFeaturesKHR bufferDeviceAddressFeatures{};
	bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR;
	VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{};
	rayTracingPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
	VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{};
	accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures{};
	rayQueryFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
	VkPhysicalDeviceFragmentShadingRateFeaturesKHR shadingRateFeatures{};
	shadingRateFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
	VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timelineSemaphoreFeatures{};
	timelineSemaphoreFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR;
	VkDeviceDiagnosticsConfigCreateInfoNV diagnosticsCreateInfoNV{};
	diagnosticsCreateInfoNV.sType = VK_STRUCTURE_TYPE_DEVICE_DIAGNOSTICS_CONFIG_CREATE_INFO_NV;
	diagnosticsCreateInfoNV.flags = VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_SHADER_DEBUG_INFO_BIT_NV |
			                        VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_RESOURCE_TRACKING_BIT_NV |
								    VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_AUTOMATIC_CHECKPOINTS_BIT_NV;

	VkBaseOutStructure* base = reinterpret_cast<VkBaseOutStructure*>(&devFeatures2);

	if(VulkanRenderer::s_fragmentShaderInterlockExtension)
		LinkFeatureStruct(base, fragmentShaderInterlockFeatures);
	if (VulkanRenderer::s_descriptorIndexingExtension)
		LinkFeatureStruct(base, descriptorIndexingFeatures);
	if (VulkanRenderer::s_samplerYcbcrConversionExtension)
		LinkFeatureStruct(base, ycbcrFeatures);
	LinkFeatureStruct(base, shaderDrawParametersFeatures);
	//RayTracing
	if (VulkanRenderer::s_bufferDeviceAddressExtension)
		LinkFeatureStruct(base, bufferDeviceAddressFeatures);
	if (VulkanRenderer::s_rayTracingExtension)
		LinkFeatureStruct(base, rayTracingPipelineFeatures);
	if (VulkanRenderer::s_rayTracingExtension)
		LinkFeatureStruct(base, accelerationStructureFeatures);
	if (VulkanRenderer::s_rayTracingExtension)
		LinkFeatureStruct(base, rayQueryFeatures);
	//Shading rate
	if(VulkanRenderer::s_shadingRate)
		LinkFeatureStruct(base, shadingRateFeatures);
	//Timeline semaphore
	if(VulkanRenderer::s_timelineSemaphore)
		LinkFeatureStruct(base, timelineSemaphoreFeatures);

	vkGetPhysicalDeviceFeatures2(m_physicalDevice->GetVkPhysicalDevice(), &devFeatures2);

	//Need a queuePriority for each queue in the queue family we create
	const std::vector<VkQueueFamilyProperties>& queueFamilyProperties = m_physicalDevice->GetQueueFamilyProperties();
	std::vector<std::vector<f32>> queueFamilyPriorities(queueFamilyProperties.size());
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

	for(usize i = 0; i < queueFamilyProperties.size(); ++i)
	{
		const auto& queueFamilyProps = queueFamilyProperties[i];

		queueFamilyPriorities[i].resize(queueFamilyProps.queueCount);

		const VkDeviceQueueCreateInfo info
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueFamilyIndex = NumericCast<u32>(i),
			.queueCount = queueFamilyProps.queueCount,
			.pQueuePriorities = queueFamilyPriorities[i].data()
		};
		queueCreateInfos.push_back(info);

		m_availableQueueCount[queueFamilyProps.queueFlags] = queueFamilyProps.queueCount;
	}

	EnableNsightAftermath(base, diagnosticsCreateInfoNV);

	std::vector<const char*> extensions(m_deviceExtensions.size());
	std::ranges::transform(m_deviceExtensions, extensions.begin(), [](const std::string_view ext){return ext.data();});

	const VkDeviceCreateInfo deviceCreateInfo = VulkanInits::DeviceCreateInfo(&devFeatures2, queueCreateInfos,
	                                                                          extensions);

	VkCall(vkCreateDevice(m_physicalDevice->GetVkPhysicalDevice(), &deviceCreateInfo, nullptr, &m_device));
	TRAP_ASSERT(m_device, "VulkanDevice(): No Vulkan device!");

	VkLoadDevice(m_device);

	SetVulkanRendererFeatures(m_physicalDevice->GetVkPhysicalDevice(), bufferDeviceAddressFeatures,
	                          ycbcrFeatures, shaderDrawParametersFeatures, shadingRateFeatures,
							  timelineSemaphoreFeatures);

	FindQueueFamilyIndices();

#ifdef ENABLE_GRAPHICS_DEBUG
	if (m_physicalDevice->GetVkPhysicalDeviceProperties().deviceName[0] != '\0')
		SetDeviceName(m_physicalDevice->GetVkPhysicalDeviceProperties().deviceName, m_device);
#endif /*ENABLE_GRAPHICS_DEBUG*/

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	InitNVIDIAReflex(m_reflexSemaphore, m_physicalDevice->GetVendor(), m_device);
#endif /*defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanDevice::~VulkanDevice()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Destroying Device");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	if(RendererAPI::GPUSettings.ReflexSupported)
		vkDestroySemaphore(m_device, m_reflexSemaphore, nullptr);
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

	if(m_device != VK_NULL_HANDLE)
		vkDestroyDevice(m_device, nullptr);

#ifdef ENABLE_NSIGHT_AFTERMATH
	if(RendererAPI::s_aftermathSupport)
		TRAP::Graphics::AftermathTracker::Shutdown();
#endif /*ENABLE_NSIGHT_AFTERMATH*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const TRAP::Graphics::API::VulkanPhysicalDevice& TRAP::Graphics::API::VulkanDevice::GetPhysicalDevice() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return *m_physicalDevice;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::FindQueueFamilyIndices()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	[[maybe_unused]] VkQueueFamilyProperties qfp{};

	FindQueueFamilyIndex(RendererAPI::QueueType::Graphics, qfp, m_graphicsQueueFamilyIndex, m_graphicsQueueIndex);
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Using Graphics Queue Family Index ",
	         NumericCast<u32>(m_graphicsQueueFamilyIndex));
#endif /*VERBOSE_GRAPHICS_DEBUG*/
	FindQueueFamilyIndex(RendererAPI::QueueType::Compute, qfp, m_computeQueueFamilyIndex, m_computeQueueIndex);
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Using Compute Queue Family Index ",
	         NumericCast<u32>(m_computeQueueFamilyIndex));
#endif /*VERBOSE_GRAPHICS_DEBUG*/
	FindQueueFamilyIndex(RendererAPI::QueueType::Transfer, qfp, m_transferQueueFamilyIndex, m_transferQueueIndex);
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanDevicePrefix, "Using Transfer Queue Family Index ",
	         NumericCast<u32>(m_transferQueueFamilyIndex));
#endif /*VERBOSE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::WaitIdle() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	VkCall(vkDeviceWaitIdle(m_device));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanDevice::FindQueueFamilyIndex(const RendererAPI::QueueType queueType,
                                                             VkQueueFamilyProperties& queueFamilyProperties,
															 u8& queueFamilyIndex, u8& queueIndex)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	u32 qfi = std::numeric_limits<u32>::max();
	u32 qi = std::numeric_limits<u32>::max();
	const VkQueueFlags requiredFlags = QueueTypeToVkQueueFlags(queueType);
	bool found = false;

	const std::vector<VkQueueFamilyProperties>& props = m_physicalDevice->GetQueueFamilyProperties();

	u32 minQueueFlag = std::numeric_limits<u32>::max();

	//Try to find a dedicated queue of this type
	for (u32 index = 0; index < props.size(); ++index)
	{
		const VkQueueFlags queueFlags = props[index].queueFlags;
		const bool graphicsQueue = (queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
		const u32 flagAnd = (queueFlags & requiredFlags);
		//Usually only one graphics queue so choose the first one with graphics bit
		if (queueType == RendererAPI::QueueType::Graphics && graphicsQueue)
		{
			found = true;
			qfi = index;
			qi = 0;
			break;
		}
		//The only flag set on this queue family is the one required - Most optimal choice
		//Example: Required flag is VK_QUEUE_COMPUTE_BIT and the queue family has only VK_QUEUE_COMPUTE_BIT set.
		if (((queueFlags & requiredFlags) != 0u) && ((queueFlags & ~requiredFlags) == 0) &&
			m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
		{
			found = true;
			qfi = index;
			qi = m_usedQueueCount[queueFlags];
			break;
		}
		//Queue family has flags other than the required flag(s) - Choose the family with the least number of other flags.
		//Example: Required flag is VK_QUEUE_GRAPHICS_BIT
		//Two queue families available:
		// - Queue family 1 has VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT
		// - Queue family 2 has VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT
		// Since queue family 1 has less flags, we choose queue family 1.
		if ((flagAnd != 0u) && ((queueFlags - flagAnd) < minQueueFlag) && !graphicsQueue &&
			m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
		{
			found = true;
			minQueueFlag = (queueFlags - flagAnd);
			qfi = index;
			qi = m_usedQueueCount[queueFlags];
		}
	}

	//If hardware doesn't provide a dedicated queue try to find a non-dedicated one
	if (!found)
	{
		for (u32 index = 0; index < props.size(); ++index)
		{
			const VkQueueFlags queueFlags = props[index].queueFlags;
			if (((queueFlags & requiredFlags) != 0u) && m_usedQueueCount[queueFlags] < m_availableQueueCount[queueFlags])
			{
				found = true;
				qfi = index;
				qi = m_usedQueueCount[queueFlags];
				break;
			}
		}
	}

	if (!found)
	{
		qfi = 0;
		qi = 0;

		TP_WARN(Log::RendererVulkanDevicePrefix, "Could not find queue of type ", queueType, '(',
		        std::to_underlying(queueType), "). Using default queue");
	}

	queueFamilyProperties = props[qfi];
	queueFamilyIndex = NumericCast<u8>(qfi);
	queueIndex = NumericCast<u8>(qi);
}
