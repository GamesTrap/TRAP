#include "TRAPPCH.h"
#include "VulkanPhysicalDevice.h"

#include "Utils/UniqueResource.h"
#include "VulkanInstance.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/Objects/VulkanSurface.h"
#include "Graphics/API/Vulkan/Utils/VulkanLoader.h"
#include "Utils/ErrorCodes/ErrorCodes.h"
#include "Maths/Math.h"

namespace
{
	//Multiple physical devices can theoretically have the same score, so we use a multimap here.
	std::multimap<u32, TRAP::Graphics::API::RatedVulkanPhysicalDevice> RatedPhysicalDevices{};

	[[nodiscard]] TRAP::Optional<std::pair<u32, TRAP::Graphics::API::RatedVulkanPhysicalDevice>> RatePhysicalDevice(VkPhysicalDevice physicalDevice, [[maybe_unused]] VkInstance instance);

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] std::vector<VkExtensionProperties> LoadAllPhysicalDeviceExtensions(VkPhysicalDevice physicalDevice)
	{
		std::vector<VkExtensionProperties> deviceExtensions{};

		u32 extensionsCount = 0;
		VkCall(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, nullptr));
		deviceExtensions.resize(extensionsCount);
		VkCall(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, deviceExtensions.data()));

		return deviceExtensions;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr u32 GetMaxUsableMSAASampleCount(const VkPhysicalDeviceProperties& devProps)
	{
		VkSampleCountFlags sampleCounts = TRAP::Math::Min(devProps.limits.framebufferColorSampleCounts,
														  devProps.limits.framebufferDepthSampleCounts);
		sampleCounts = TRAP::Math::Min(sampleCounts, devProps.limits.framebufferStencilSampleCounts);

		if((sampleCounts & VK_SAMPLE_COUNT_64_BIT) != 0u)
			return VK_SAMPLE_COUNT_64_BIT;
		if((sampleCounts & VK_SAMPLE_COUNT_32_BIT) != 0u)
			return VK_SAMPLE_COUNT_32_BIT;
		if((sampleCounts & VK_SAMPLE_COUNT_16_BIT) != 0u)
			return VK_SAMPLE_COUNT_16_BIT;
		if((sampleCounts & VK_SAMPLE_COUNT_8_BIT) != 0u)
			return VK_SAMPLE_COUNT_8_BIT;
		if((sampleCounts & VK_SAMPLE_COUNT_4_BIT) != 0u)
			return VK_SAMPLE_COUNT_4_BIT;
		if((sampleCounts & VK_SAMPLE_COUNT_2_BIT) != 0u)
			return VK_SAMPLE_COUNT_2_BIT;

		return VK_SAMPLE_COUNT_1_BIT;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] std::vector<VkPhysicalDevice> GetAllVkPhysicalDevices(VkInstance instance)
	{
		u32 physicalDeviceCount = 0;
		VkCall(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		VkCall(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));

		return physicalDevices;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void SetupRendererAPIGPUSettings(const TRAP::Graphics::API::VulkanPhysicalDevice& physicalDevice)
	{
		// Capabilities for VulkanRenderer
		for (u32 i = 0; i < std::to_underlying(TRAP::Graphics::API::ImageFormat::IMAGE_FORMAT_COUNT); ++i)
		{
			const VkFormat fmt = ImageFormatToVkFormat(static_cast<TRAP::Graphics::API::ImageFormat>(i));
			if (fmt == VK_FORMAT_UNDEFINED)
				continue;

			VkFormatProperties formatSupport{};
			vkGetPhysicalDeviceFormatProperties(physicalDevice.GetVkPhysicalDevice(), fmt, &formatSupport);
			TRAP::Graphics::API::VulkanRenderer::s_GPUCapBits.CanShaderReadFrom[i] = (formatSupport.optimalTilingFeatures &
																                      VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) != 0;
			TRAP::Graphics::API::VulkanRenderer::s_GPUCapBits.CanShaderWriteTo[i] = (formatSupport.optimalTilingFeatures &
																                     VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) != 0;
			TRAP::Graphics::API::VulkanRenderer::s_GPUCapBits.CanRenderTargetWriteTo[i] = (formatSupport.optimalTilingFeatures &
																	                       (VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
																	                        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) != 0;
		}

		const auto& devProps = physicalDevice.GetVkPhysicalDeviceProperties();
		const auto& devFeatures = physicalDevice.GetVkPhysicalDeviceFeatures();
		const auto& devSubGroupProps = physicalDevice.GetVkPhysicalDeviceSubgroupProperties();

		TRAP::Graphics::RendererAPI::GPUSettings.UniformBufferAlignment = devProps.limits.minUniformBufferOffsetAlignment;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxUniformBufferRange = devProps.limits.maxUniformBufferRange;
		TRAP::Graphics::RendererAPI::GPUSettings.StorageBufferAlignment = devProps.limits.minStorageBufferOffsetAlignment;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxStorageBufferRange = devProps.limits.maxStorageBufferRange;
		TRAP::Graphics::RendererAPI::GPUSettings.UploadBufferTextureAlignment = NumericCast<u32>(devProps.limits.optimalBufferCopyOffsetAlignment);
		TRAP::Graphics::RendererAPI::GPUSettings.UploadBufferTextureRowAlignment = NumericCast<u32>(devProps.limits.optimalBufferCopyRowPitchAlignment);
		TRAP::Graphics::RendererAPI::GPUSettings.MaxVertexInputBindings = devProps.limits.maxVertexInputBindings;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxVertexInputAttributes = devProps.limits.maxVertexInputAttributes;
		TRAP::Graphics::RendererAPI::GPUSettings.MultiDrawIndirectSupported = devProps.limits.maxDrawIndirectCount > 1u;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxAnisotropy = devProps.limits.maxSamplerAnisotropy;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxImageDimension2D = devProps.limits.maxImageDimension2D;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxImageDimensionCube = devProps.limits.maxImageDimensionCube;
		TRAP::Graphics::RendererAPI::GPUSettings.FillModeNonSolid = (devFeatures.fillModeNonSolid != 0u);
		TRAP::Graphics::RendererAPI::GPUSettings.MaxPushConstantSize = devProps.limits.maxPushConstantsSize;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxSamplerAllocationCount = devProps.limits.maxSamplerAllocationCount;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxTessellationControlPoints = devProps.limits.maxTessellationPatchSize;
		TRAP::Graphics::RendererAPI::GPUSettings.MaxMSAASampleCount = static_cast<TRAP::Graphics::RendererAPI::SampleCount>(TRAP::Math::Min(GetMaxUsableMSAASampleCount(devProps), static_cast<u32>(VK_SAMPLE_COUNT_16_BIT)));
		TRAP::Graphics::RendererAPI::GPUSettings.MaxColorRenderTargets = devProps.limits.maxColorAttachments;

		// maxBoundDescriptorSets not needed because engine is always limited to 4 descriptor sets

		TRAP::Graphics::RendererAPI::GPUSettings.WaveLaneCount = devSubGroupProps.subgroupSize;
		TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags = TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::None;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_BASIC_BIT) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::Basic;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_VOTE_BIT) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::Vote;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_ARITHMETIC_BIT) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::Arithmetic;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_BALLOT_BIT) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::Ballot;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_SHUFFLE_BIT) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::Shuffle;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::ShuffleRelative;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_CLUSTERED_BIT) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::Clustered;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_QUAD_BIT) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::Quad;
		if ((devSubGroupProps.supportedOperations & VK_SUBGROUP_FEATURE_PARTITIONED_BIT_NV) != 0u)
			TRAP::Graphics::RendererAPI::GPUSettings.WaveOpsSupportFlags |= TRAP::Graphics::RendererAPI::WaveOpsSupportFlags::PartitionedNV;

		TRAP::Graphics::RendererAPI::GPUSettings.TessellationSupported = (devFeatures.tessellationShader != 0u);
		TRAP::Graphics::RendererAPI::GPUSettings.GeometryShaderSupported = (devFeatures.geometryShader != 0u);
		TRAP::Graphics::RendererAPI::GPUSettings.SampleRateShadingSupported = (devFeatures.sampleRateShading != 0u);

#ifndef TRAP_HEADLESS_MODE
		//Surface and present test is a hard requirement to pass the Physical device rating.
		TRAP::Graphics::RendererAPI::GPUSettings.SurfaceSupported = true;
		TRAP::Graphics::RendererAPI::GPUSettings.PresentSupported = true;
#endif /*TRAP_HEADLESS_MODE*/
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] VkPhysicalDevice FindPhysicalDeviceViaUUID(const TRAP::Graphics::API::VulkanInstance& instance,
	                                                         const TRAP::Utils::UUID& physicalDeviceUUID)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

		const auto physicalDevices = GetAllVkPhysicalDevices(instance.GetVkInstance());

		for (VkPhysicalDevice device : physicalDevices)
		{
			VkPhysicalDeviceIDPropertiesKHR physicalDeviceIDProperties{};
			physicalDeviceIDProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
			VkPhysicalDeviceProperties2 props2{};
			props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			props2.pNext = &physicalDeviceIDProperties;
			vkGetPhysicalDeviceProperties2(device, &props2);

			const TRAP::Utils::UUID testUUID = std::to_array(physicalDeviceIDProperties.deviceUUID);

			if(testUUID == physicalDeviceUUID)
				return device;
		}

		return nullptr;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Rate a list of physical devices.
	///
	/// Does the following checks for rating the device (some are required, others are optional):
	///  1. Must be Vulkan API 1.1 capable
	///  2. Must be iGPU or dGPU (5000 score for dGPU, 2500 score for iGPU).
	///  3. Must support at least 4 simultaneously bound descriptor sets.
	///  4. Retrieves all supported extensions (50 score for each supported extension).
	///  5. Must support swapchain extensions (Disabled for Headless mode).
	///  6. Must be able to create a Vulkan surface test window (Disabled for Headless mode).
	///  7. Must be able to create a Vulkan surface (Disabled for Headless mode).
	///  8. Must have at least one queue family.
	///  9. Must support at least one graphics queue family.
	/// 10. Must support at least one queue family with present support (Disabled for Headless mode).
	/// 11. Must support at least one present mode (Disabled for Headless mode).
	/// 12. Must support at least one surface format (Disabled for Headless mode).
	/// 13. Optionally supports a compute queue family (1000 score for each).
	/// 14. Optionally supports a transfer queue family (1000 score for each).
	/// 15. Optionally supports RayTracing extensions (2000 score).
	/// 16. Optionally supports geometry shaders (1000 score).
	/// 17. Optionally supports tessellation shaders (1000 score).
	/// 18. Optionally supports Variable Rate Shading extensions and Tier 1/Tier 2 (1000 + 100/200 score).
	/// 19. Optionally supports fill mode non solid (250 score).
	/// 20. Optionally does surface has optimal format (B8G8R8A8 Unorm & SRGB Non-linear) (250 score) (Disabled for Headless mode).
	/// 21. Optionally check VRAM size (size / 1 Gigabyte * 100 score).
	/// 22. Optionally check max resolution of 2D and cube images. ((Max image resolution / 32) is score).
	/// 23. Optionally check max sample count for MSAA. (Sample count * 10 score).
	/// 24. Optionally check if anisotropic filtering is supported (500 score).
	/// @param physicalDevices Vulkan physical devices to rate.
	/// @param instance Vulkan instance used to retrieve the physical devices.
	void RatePhysicalDevices(const std::span<const VkPhysicalDevice> physicalDevices, VkInstance instance)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

		// Score each Physical Device and insert into multimap
		for (VkPhysicalDevice physicalDevice : physicalDevices)
		{
			const auto deviceRating = RatePhysicalDevice(physicalDevice, instance);
			if(deviceRating)
				RatedPhysicalDevices.emplace(*deviceRating);
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void UpdateShadingRateCaps(const VkPhysicalDeviceFragmentShadingRateFeaturesKHR& shadingRateFeatures,
	                           VkPhysicalDevice physicalDevice)
	{
		using namespace TRAP::Graphics::API;
		using RendererAPI = TRAP::Graphics::RendererAPI;

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
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanPhysicalDevice::VulkanPhysicalDevice(TRAP::Ref<VulkanInstance> instance,
																const TRAP::Utils::UUID& physicalDeviceUUID)
	: m_instance(std::move(instance))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_instance, "VulkanPhysicalDevice(): Vulkan Instance is nullptr");

	m_physicalDevice = FindPhysicalDeviceViaUUID(*m_instance, physicalDeviceUUID);
	TRAP_ASSERT(m_physicalDevice, "VulkanPhysicalDevice(): Vulkan Physical Device is nullptr!");

	if (m_physicalDevice == nullptr)
		Utils::DisplayError(Utils::ErrorCode::VulkanPhysicalDeviceCreationFailed);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPhysicalDevicePrefix, "Creating PhysicalDevice");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	m_availablePhysicalDeviceExtensions = LoadAllPhysicalDeviceExtensions(m_physicalDevice);

	LoadDevicePropertiesAndFeatures();
	SetupRendererAPIGPUSettings(*this);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPhysicalDevicePrefix, "Destroying PhysicalDevice");
#endif /*VERBOSE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Optional<VkImageFormatProperties> TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceImageFormatProperties(const TRAP::Graphics::API::ImageFormat format,
                                                                                                                                          const VkImageType imageType,
																														                  const VkImageUsageFlags imageUsageFlags) const
{
	VkImageFormatProperties props{};
	VkResult res = vkGetPhysicalDeviceImageFormatProperties(m_physicalDevice, ImageFormatToVkFormat(format),
	                                                        imageType, VK_IMAGE_TILING_OPTIMAL, imageUsageFlags, 0,
															&props);
	VkCall(res);

	if(res != VK_SUCCESS)
		return TRAP::NullOpt;

	return props;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] bool TRAP::Graphics::API::VulkanPhysicalDevice::GetPhysicalDeviceSurfaceSupport(const VulkanSurface& surface, u32 queueFamilyIndex) const
{
	VkBool32 supportsPresent = VK_FALSE;

	VkCall(vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, queueFamilyIndex, surface.GetVkSurface(),
	                                            &supportsPresent));

	return supportsPresent != VK_FALSE;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanPhysicalDevice::LoadDevicePropertiesAndFeatures()
{
	vkGetPhysicalDeviceProperties(m_physicalDevice, &m_physicalDeviceProperties);
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_physicalDeviceMemoryProperties);
	vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_physicalDeviceFeatures);

	VkPhysicalDeviceProperties2 props2{};
	props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

	m_physicalDeviceIDProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
	m_physicalDeviceSubgroupProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES;
	m_physicalDeviceDriverProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;

	VkBaseOutStructure* baseProps = reinterpret_cast<VkBaseOutStructure*>(&props2);
	TRAP::Graphics::API::LinkVulkanStruct(baseProps, m_physicalDeviceIDProperties);
	TRAP::Graphics::API::LinkVulkanStruct(baseProps, m_physicalDeviceSubgroupProperties);
	TRAP::Graphics::API::LinkVulkanStruct(baseProps, m_physicalDeviceDriverProperties);

	vkGetPhysicalDeviceProperties2(m_physicalDevice, &props2);

	u32 queueFamilyPropertyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertyCount, nullptr);
	m_queueFamilyProperties.resize(queueFamilyPropertyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertyCount,
											 m_queueFamilyProperties.data());

	for (u32 i = 0; i < std::to_underlying(TRAP::Graphics::API::ImageFormat::IMAGE_FORMAT_COUNT); ++i)
	{
		const VkFormat fmt = ImageFormatToVkFormat(static_cast<TRAP::Graphics::API::ImageFormat>(i));
		if (fmt == VK_FORMAT_UNDEFINED)
			continue;

		vkGetPhysicalDeviceFormatProperties(m_physicalDevice, fmt, &m_physicalDeviceFormatProperties[i]);
	}

	VkPhysicalDeviceFeatures2 devFeatures2{};
	devFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

	VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT fragmentShaderInterlockFeatures{};
	fragmentShaderInterlockFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADER_INTERLOCK_FEATURES_EXT;
	VkPhysicalDeviceSamplerYcbcrConversionFeatures ycbcrFeatures{};
	ycbcrFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES;
	VkPhysicalDeviceShaderDrawParametersFeatures shaderDrawParametersFeatures{};
	shaderDrawParametersFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
	VkPhysicalDeviceBufferDeviceAddressFeaturesKHR bufferDeviceAddressFeatures{};
	bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR;
	VkPhysicalDeviceFragmentShadingRateFeaturesKHR shadingRateFeatures{};
	shadingRateFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
	VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timelineSemaphoreFeatures{};
	timelineSemaphoreFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR;

	VkBaseOutStructure* baseFeatures = reinterpret_cast<VkBaseOutStructure*>(&devFeatures2);

	if(IsExtensionSupported(VulkanPhysicalDeviceExtension::FragmentShaderInterlock))
		TRAP::Graphics::API::LinkVulkanStruct(baseFeatures, fragmentShaderInterlockFeatures);
	TRAP::Graphics::API::LinkVulkanStruct(baseFeatures, ycbcrFeatures);
	TRAP::Graphics::API::LinkVulkanStruct(baseFeatures, shaderDrawParametersFeatures);
	//RayTracing
	if (IsExtensionSupported(VulkanPhysicalDeviceExtension::BufferDeviceAddress))
		TRAP::Graphics::API::LinkVulkanStruct(baseFeatures, bufferDeviceAddressFeatures);
	//Shading rate
	if(IsExtensionSupported(VulkanPhysicalDeviceExtension::ShadingRate))
		TRAP::Graphics::API::LinkVulkanStruct(baseFeatures, shadingRateFeatures);
	//Timeline semaphore
	if(IsExtensionSupported(VulkanPhysicalDeviceExtension::TimelineSemaphore))
		TRAP::Graphics::API::LinkVulkanStruct(baseFeatures, timelineSemaphoreFeatures);

	vkGetPhysicalDeviceFeatures2(m_physicalDevice, &devFeatures2);

	RendererAPI::GPUSettings.ROVsSupported = fragmentShaderInterlockFeatures.fragmentShaderPixelInterlock;
	m_deviceFeatures.SamplerYcbcrConversion = (ycbcrFeatures.samplerYcbcrConversion != 0u);
	m_deviceFeatures.ShaderDrawParameters = (shaderDrawParametersFeatures.shaderDrawParameters != 0u);
	m_deviceFeatures.BufferDeviceAddress = (bufferDeviceAddressFeatures.bufferDeviceAddress != 0u);
	m_deviceFeatures.TimelineSemaphore = (timelineSemaphoreFeatures.timelineSemaphore != 0u);
	m_deviceFeatures.NsightAftermath =  IsExtensionSupported(VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME) &&
		                                IsExtensionSupported(VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME);

	UpdateShadingRateCaps(shadingRateFeatures, m_physicalDevice);
}

//-------------------------------------------------------------------------------------------------------------------//

/// @brief Retrieve a list of all rated physical devices.
/// Key is the devices score, value is the UUID of the physical device.
/// @param instance Vulkan instance handle.
/// @return List of rated physical devices.
[[nodiscard]] const std::multimap<u32, TRAP::Graphics::API::RatedVulkanPhysicalDevice>& TRAP::Graphics::API::VulkanPhysicalDevice::GetAllRatedPhysicalDevices(VkInstance instance)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if (!RatedPhysicalDevices.empty())
		return RatedPhysicalDevices;

	const std::vector<VkPhysicalDevice> physicalDevices = GetAllVkPhysicalDevices(instance);

	if (!physicalDevices.empty())
		RatePhysicalDevices(physicalDevices, instance);
	else
		TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::VulkanNoPhysicalDeviceFound);

	return RatedPhysicalDevices;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::multimap<u32, TRAP::Graphics::API::RatedVulkanPhysicalDevice>& TRAP::Graphics::API::VulkanPhysicalDevice::GetAllRatedPhysicalDevices(const VulkanInstance& instance)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	return GetAllRatedPhysicalDevices(instance.GetVkInstance());
}

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
	[[nodiscard]] constexpr bool RatePhysicalDevice_CheckVulkanVersion(const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Vulkan 1.1 or higher
		if (devProps.apiVersion < VK_API_VERSION_1_1)
		{
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed Vulkan version test!");
			return false;
		}

		return true;
	}

	[[nodiscard]] constexpr TRAP::Optional<u32> RatePhysicalDevice_CheckDeviceType(const VkPhysicalDeviceProperties& devProps)
	{
		// Required (dGPU or iGPU): Discrete GPUs have a significant performance advantage.
		// Headless mode: Any device type is allowed (even software implementations).
		if(devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			return 5000;
		if(devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
			return 2500;

#ifndef TRAP_HEADLESS_MODE
		TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed dedicated/integrated GPU test!");
		return TRAP::NullOpt;
#else
		return 0;
#endif /*TRAP_HEADLESS_MODE*/
	}

	[[nodiscard]] constexpr bool RatePhysicalDevice_CheckMaxBoundDescriptorSet(const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Support for at least 4 simultaneously bound descriptor sets
		if(devProps.limits.maxBoundDescriptorSets < 4)
		{
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed max bound descriptor set test!");
			return false;
		}

		return true;
	}

	[[nodiscard]] std::pair<u32, std::vector<VkExtensionProperties>> RatePhysicalDevice_RateDeviceExtensions(VkPhysicalDevice physicalDevice)
	{
		std::pair<u32, std::vector<VkExtensionProperties>> result{};
		auto& [score, extensions] = result;

		//Get all physical device extensions
		u32 supportedExtensionsCount = 0;
		VkCall(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedExtensionsCount, nullptr));
		extensions.resize(supportedExtensionsCount);
		VkCall(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedExtensionsCount, extensions.data()));

		score = supportedExtensionsCount * 50;

		return result;
	}

	[[nodiscard]] constexpr bool RatePhysicalDevice_CheckSPIRV1_4Support(const std::span<const VkExtensionProperties> extensions, const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Check if physical device supports SPIRV 1.4 extensions
		const bool supportsSPIRV1_4Ext = std::ranges::find_if(extensions, [](const auto& props){return std::string_view(VK_KHR_SPIRV_1_4_EXTENSION_NAME) == props.extensionName;}) != extensions.end();
		const bool supportsShaderFloatControlsExt = std::ranges::find_if(extensions, [](const auto& props){return std::string_view(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME) == props.extensionName;}) != extensions.end();

		if(!supportsSPIRV1_4Ext || !supportsShaderFloatControlsExt)
		{
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed required SPIRV 1.4 extensions test!");
			return false;
		}

		return true;
	}

#ifndef TRAP_HEADLESS_MODE
	[[nodiscard]] constexpr bool RatePhysicalDevice_CheckSwapChainSupport(const std::span<const VkExtensionProperties> extensions, const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Check if physical device supports swapchains

		const bool swapChainSupported = std::ranges::find_if(extensions, [](const auto& props){return std::string_view(VK_KHR_SWAPCHAIN_EXTENSION_NAME) == props.extensionName;}) != extensions.end();
		if(!swapChainSupported)
		{
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed required swapchain extension test!");
			return false;
		}

		return true;
	}
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
	using TestWindowHandle = TRAP::UniqueResource<TRAP::INTERNAL::WindowingAPI::InternalWindow*, decltype(&TRAP::INTERNAL::WindowingAPI::DestroyWindow)>;
	[[nodiscard]] TestWindowHandle RatePhysicalDevice_CreateSurfaceTestWindow(const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Create Vulkan surface test window
		// Headless mode: Dont use this function

		if(!TRAP::INTERNAL::WindowingAPI::Init())
			TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::WindowingAPIFailedInitialization);

		TRAP::INTERNAL::WindowingAPI::WindowHint(TRAP::INTERNAL::WindowingAPI::Hint::Visible, false);
		TRAP::INTERNAL::WindowingAPI::WindowHint(TRAP::INTERNAL::WindowingAPI::Hint::Focused, false);
		auto* const vulkanTestWindow = TRAP::INTERNAL::WindowingAPI::CreateWindow(400, 400, "TRAP Vulkan Surface Tester", nullptr);

		TRAP::INTERNAL::WindowingAPI::DefaultWindowHints();
		if(vulkanTestWindow == nullptr)
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed Vulkan test window creation!");

		return TRAP::MakeUniqueResourceChecked(vulkanTestWindow, nullptr, &TRAP::INTERNAL::WindowingAPI::DestroyWindow);
	}
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
	[[nodiscard]] auto RatePhysicalDevice_CheckSurfaceSupport(VkInstance instance, const TRAP::INTERNAL::WindowingAPI::InternalWindow& window,
                                                              const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Check if surface can be created

		VkSurfaceKHR surface = VK_NULL_HANDLE;
		const VkResult res = TRAP::INTERNAL::WindowingAPI::CreateWindowSurface(instance, window, nullptr, surface);
		VkCall(res);

		if((surface == VK_NULL_HANDLE) || res != VK_SUCCESS)
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed surface creation test!");

		return TRAP::MakeUniqueResourceChecked(surface, VK_NULL_HANDLE, [instance](VkSurfaceKHR s){vkDestroySurfaceKHR(instance, s, nullptr);});
	}
#endif /*TRAP_HEADLESS_MODE*/

	[[nodiscard]] std::vector<VkQueueFamilyProperties> RatePhysicalDevice_GetQueueFamilies(VkPhysicalDevice physicalDevice,
	                                                                                       const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Get queue families
		u32 queueFamilyPropertyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());
		if (queueFamilyProperties.empty())
		{
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName,
					 "\" failed querying queue family properties!");
			return {};
		}

		return queueFamilyProperties;
	}

	[[nodiscard]] constexpr bool RatePhysicalDevice_CheckGraphicsQueue(const std::span<const VkQueueFamilyProperties> queueFamilyProps,
	                                                                   const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Check if ühysical device supports graphics queue

		for (const VkQueueFamilyProperties& props : queueFamilyProps)
		{
			if ((props.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0u)
				return true;
		}

		TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed graphics queue test!");
		return false;
	}

#ifndef TRAP_HEADLESS_MODE
	[[nodiscard]] bool RatePhysicalDevice_CheckPresentSupport(const std::span<const VkQueueFamilyProperties> queueFamilyProps, VkPhysicalDevice physicalDevice,
	                                                          VkSurfaceKHR surface, const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Check if physical device supports presenting
		// Disabled in Headless mode.

		VkBool32 foundPresentSupport = VK_FALSE;
		for (usize i = 0; i < queueFamilyProps.size(); i++)
		{
			VkCall(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, NumericCast<u32>(i), surface, &foundPresentSupport));
			if (foundPresentSupport != 0u)
				return true;
		}

		TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed present queue test!");
		return false;
	}
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
	[[nodiscard]] bool RatePhysicalDevice_CheckSurfacePresentModes(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
	                                                               const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Check if surface contains present modes
		// Disabled in Headless mode.

		u32 surfacePresentModeCount = 0;
		VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount, nullptr));
		std::vector<VkPresentModeKHR> presentModes(surfacePresentModeCount);
		VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount,
														 presentModes.data()));
		if (presentModes.empty())
		{
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed present mode test!");
			return false;
		}

		return true;
	}
#endif /*TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
	[[nodiscard]] std::vector<VkSurfaceFormatKHR> RatePhysicalDevice_CheckSurfaceFormats(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
	                                                                                     const VkPhysicalDeviceProperties& devProps)
	{
		// Required: Check if surface contains formats
		// Disabled in Headless mode.

		u32 surfaceFormatCount = 0;
		VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr));
		std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
		VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data()));
		if (surfaceFormats.empty())
			TP_ERROR(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed surface format test!");

		return surfaceFormats;
	}
#endif /*TRAP_HEADLESS_MODE*/

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckComputeQueue(const std::span<const VkQueueFamilyProperties> queueFamilyProps,
                                                                     const VkPhysicalDeviceProperties& devProps)
	{
		// Big Optionally: Check if physical device supports compute queue

		for (const VkQueueFamilyProperties &props : queueFamilyProps)
		{
			if ((props.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0u)
				return 1000;
		}

		TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed compute queue test!");
		return 0;
	}

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckTransferQueue(const std::span<const VkQueueFamilyProperties> queueFamilyProps,
	                                                                  const VkPhysicalDeviceProperties& devProps)
	{
		// Big Optionally: Check if physical device supports transfer queue

		for (const VkQueueFamilyProperties &props : queueFamilyProps)
		{
			if ((props.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0u)
				return 1000;
		}

		TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed transfer queue test!");
		return 0;
	}

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckRayTracingSupport(const std::span<const VkExtensionProperties> extensions,
	                                                                      const VkPhysicalDeviceProperties& devProps)
	{
		// Big Optionally: Check if ray tracing extensions are supported

		constexpr std::array<std::string_view, 8> raytracingExt =
		{
			VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
			VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
			VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
			VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
			VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
			VK_KHR_SPIRV_1_4_EXTENSION_NAME,
			VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
			VK_KHR_RAY_QUERY_EXTENSION_NAME
		};

		bool raytracing = true;
		for (const std::string_view str : raytracingExt)
		{
			const auto extRes = std::ranges::find_if(extensions,
											 [str](const VkExtensionProperties &props)
											 {
												 return str == props.extensionName;
											 }) != extensions.end();

			if (!extRes)
			{
				raytracing = false;
				break;
			}
		}

		if (raytracing)
			return 2000;

		TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed ray tracing test!");
		return 0;
	}

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckGeometryShaderSupport(const VkPhysicalDeviceFeatures& devFeatures,
	                                                                          const VkPhysicalDeviceProperties& devProps)
	{
		// Big Optionally: Check if geometry shaders are supported

		if (devFeatures.geometryShader != 0u)
			return 1000;

		TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed geometry shader test!");
		return 0;
	}

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckTessellationShaderSupport(const VkPhysicalDeviceFeatures& devFeatures,
	                                                                              const VkPhysicalDeviceProperties& devProps)
	{
		// Big Optionally: Check if tessellation shaders are supported

		if (devFeatures.tessellationShader != 0u)
			return 1000;

		TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed tessellation shader test!");
		return 0;
	}

	[[nodiscard]] u32 RatePhysicalDevice_CheckVariableRateShadingSupport(const std::span<const VkExtensionProperties> extensions,
	                                                                     VkPhysicalDevice physicalDevice,
																		 const VkPhysicalDeviceProperties& devProps)
	{
		// Big Optionally: Check if variable rate shading is supported and Tier 1/Tier 2

		u32 score = 0;
		bool VRS = true;
		static constexpr std::array<std::string_view, 2> VRSExt =
		{
			VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
			VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME
		};
		for (const std::string_view str : VRSExt)
		{
			const auto extRes = std::ranges::find_if(extensions,
											 [str](const VkExtensionProperties &props)
											 {
												 return str == props.extensionName;
											 }) != extensions.end();

			if (!extRes)
			{
				VRS = false;
				break;
			}
		}

		if (VRS)
		{
			//VRS Tier 1/Tier 2
			VkPhysicalDeviceFragmentShadingRateFeaturesKHR fsrFeatures{};
			fsrFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
			VkPhysicalDeviceFeatures2KHR devFeatures2{};
			devFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
			devFeatures2.pNext = &fsrFeatures;
			vkGetPhysicalDeviceFeatures2KHR(physicalDevice, &devFeatures2);

			if((fsrFeatures.pipelineFragmentShadingRate == 0u) && (fsrFeatures.attachmentFragmentShadingRate == 0u))
			{
				TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed variable rate shading test!");
				return 0;
			}

			score += 1000;

			if(fsrFeatures.pipelineFragmentShadingRate != 0u) //Tier 1
				score += 100;
			if(fsrFeatures.attachmentFragmentShadingRate != 0u) //Tier 2
				score += 200;
		}
		else
			TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed variable rate shading test!");

		return score;
	}

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckFillModeNonSolidSupport(const VkPhysicalDeviceFeatures& devFeatures,
	                                                                            const VkPhysicalDeviceProperties& devProps)
	{
		// Optionally: Check if physical device support fill mode non solid
		if (devFeatures.fillModeNonSolid != 0u)
			return 250;

		TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed fillModeNonSolid test!");
		return 0;
	}

#ifndef TRAP_HEADLESS_MODE
	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckOptimalSurfaceFormat(const std::span<const VkSurfaceFormatKHR> surfaceFormats,
	                                                                         const VkPhysicalDeviceProperties& devProps)
	{
		// Optionally: Check if surface has optimal surface format

		for (const auto& format : surfaceFormats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return 250;
		}

		TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed optimal surface format test!");
		return 0;
	}
#endif /*TRAP_HEADLESS_MODE*/

	[[nodiscard]] u32 RatePhysicalDevice_CheckVRAMSize(VkPhysicalDevice physicalDevice)
	{
		// Optionally: Check VRAM size

		VkPhysicalDeviceMemoryProperties memProps{};
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);

		u32 score = 0;
		for (u32 i = 0; i < memProps.memoryHeapCount; i++)
		{
			if ((VK_MEMORY_HEAP_DEVICE_LOCAL_BIT & memProps.memoryHeaps[i].flags) != 0u)
			{
				static constexpr u64 ByteToGigabyte = 1e+9;
				score += NumericCast<u32>(memProps.memoryHeaps[i].size / ByteToGigabyte * 100u);
			}
		}

		return score;
	}

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckMaxImageDimensions(const VkPhysicalDeviceProperties& devProps)
	{
		// Optionally: Check 2D & cube image max size

		u32 score = 0;

		score += devProps.limits.maxImageDimension2D / 32;
		score += devProps.limits.maxImageDimensionCube / 32;

		return score;
	}

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckMaxMSAASampleCount(const VkPhysicalDeviceProperties& devProps)
	{
		// Optionally: Check max supported MSAA sample count

		VkSampleCountFlags sampleCounts = TRAP::Math::Min(devProps.limits.framebufferColorSampleCounts,
														  devProps.limits.framebufferDepthSampleCounts);
		sampleCounts = TRAP::Math::Min(sampleCounts, devProps.limits.framebufferStencilSampleCounts);

		return sampleCounts * 10;
	}

	[[nodiscard]] constexpr u32 RatePhysicalDevice_CheckAnisotropicFiltering(const VkPhysicalDeviceFeatures& devFeatures,
	                                                                         const VkPhysicalDeviceProperties& devProps)
	{
		// Optionally: Check if anisotropic filtering is supported

		if (devFeatures.samplerAnisotropy != 0u)
			return 500;

		TP_WARN(TRAP::Log::RendererVulkanPrefix, "Device: \"", devProps.deviceName, "\" failed anisotropic filtering test!");
		return 0;
	}

	[[nodiscard]] TRAP::Utils::UUID GetPhysicalDeviceUUID(VkPhysicalDevice physicalDevice)
	{
		VkPhysicalDeviceIDPropertiesKHR physicalDeviceIDProperties{};
		physicalDeviceIDProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;

		VkPhysicalDeviceProperties2 props2{};
		props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		props2.pNext = &physicalDeviceIDProperties;

		vkGetPhysicalDeviceProperties2(physicalDevice, &props2);

		return std::to_array(physicalDeviceIDProperties.deviceUUID);
	}

	[[nodiscard]] TRAP::Optional<std::pair<u32, TRAP::Graphics::API::RatedVulkanPhysicalDevice>> RatePhysicalDevice(VkPhysicalDevice physicalDevice, [[maybe_unused]] VkInstance instance)
	{
		std::pair<u32, TRAP::Graphics::API::RatedVulkanPhysicalDevice> rating{};
		auto& [score, devRating] = rating;

		// Get PhysicalDeviceProperties
		VkPhysicalDeviceProperties devProps{};
		vkGetPhysicalDeviceProperties(physicalDevice, &devProps);

		if(!RatePhysicalDevice_CheckVulkanVersion(devProps))
			return TRAP::NullOpt;

		if(const auto devTypeScore = RatePhysicalDevice_CheckDeviceType(devProps))
			score += *devTypeScore;
		else
			return TRAP::NullOpt;

		if(!RatePhysicalDevice_CheckMaxBoundDescriptorSet(devProps))
			return TRAP::NullOpt;

		const auto [extensionScore, supportedExtensions] = RatePhysicalDevice_RateDeviceExtensions(physicalDevice);
		score += extensionScore;

		if(!RatePhysicalDevice_CheckSPIRV1_4Support(supportedExtensions, devProps))
			return TRAP::NullOpt;

#ifndef TRAP_HEADLESS_MODE
		if(!RatePhysicalDevice_CheckSwapChainSupport(supportedExtensions, devProps))
			return TRAP::NullOpt;

		const auto vulkanTestWindow = RatePhysicalDevice_CreateSurfaceTestWindow(devProps);
		if(vulkanTestWindow.Get() == nullptr)
			return TRAP::NullOpt;

		const auto surface = RatePhysicalDevice_CheckSurfaceSupport(instance, *vulkanTestWindow, devProps);
		if(surface.Get() == VK_NULL_HANDLE)
			return TRAP::NullOpt;
#endif /*TRAP_HEADLESS_MODE*/

		const auto queueFamilyProperties = RatePhysicalDevice_GetQueueFamilies(physicalDevice, devProps);
		if(queueFamilyProperties.empty())
			return TRAP::NullOpt;

		if(!RatePhysicalDevice_CheckGraphicsQueue(queueFamilyProperties, devProps))
			return TRAP::NullOpt;

#ifndef TRAP_HEADLESS_MODE
		if(!RatePhysicalDevice_CheckPresentSupport(queueFamilyProperties, physicalDevice, surface.Get(), devProps))
			return TRAP::NullOpt;

		if(!RatePhysicalDevice_CheckSurfacePresentModes(physicalDevice, surface.Get(), devProps))
			return TRAP::NullOpt;

		const std::vector<VkSurfaceFormatKHR> surfaceFormats = RatePhysicalDevice_CheckSurfaceFormats(physicalDevice, surface.Get(), devProps);
		if(surfaceFormats.empty())
			return TRAP::NullOpt;
#endif /*TRAP_HEADLESS_MODE*/

		score += RatePhysicalDevice_CheckComputeQueue(queueFamilyProperties, devProps);
		score += RatePhysicalDevice_CheckTransferQueue(queueFamilyProperties, devProps);
		score += RatePhysicalDevice_CheckRayTracingSupport(supportedExtensions, devProps);

		// Get PhysicalDevice Features
		VkPhysicalDeviceFeatures devFeatures;
		vkGetPhysicalDeviceFeatures(physicalDevice, &devFeatures);

		score += RatePhysicalDevice_CheckGeometryShaderSupport(devFeatures, devProps);
		score += RatePhysicalDevice_CheckTessellationShaderSupport(devFeatures, devProps);
		score += RatePhysicalDevice_CheckVariableRateShadingSupport(supportedExtensions, physicalDevice, devProps);
		score += RatePhysicalDevice_CheckFillModeNonSolidSupport(devFeatures, devProps);
#ifndef TRAP_HEADLESS_MODE
		score += RatePhysicalDevice_CheckOptimalSurfaceFormat(surfaceFormats, devProps);
#endif /*TRAP_HEADLESS_MODE*/
		score += RatePhysicalDevice_CheckVRAMSize(physicalDevice);
		score += RatePhysicalDevice_CheckMaxImageDimensions(devProps);
		score += RatePhysicalDevice_CheckMaxMSAASampleCount(devProps);
		score += RatePhysicalDevice_CheckAnisotropicFiltering(devFeatures, devProps);

		TRAP::Utils::UUID physicalDeviceUUID = GetPhysicalDeviceUUID(physicalDevice);

		TP_INFO(TRAP::Log::RendererVulkanPrefix, "Found GPU: \"", devProps.deviceName, "\"(", TRAP::Utils::UUIDToString(physicalDeviceUUID), ')', " Score: ", score);

		devRating = TRAP::Graphics::API::RatedVulkanPhysicalDevice
		{
			.PhysicalDeviceUUID = physicalDeviceUUID,
			.PhysicalDeviceName = devProps.deviceName,
		};

		return rating;
	}
}
