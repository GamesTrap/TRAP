#include "TRAPPCH.h"
#include "VulkanRenderer.h"

#include "Application.h"
#include "Window/Window.h"
#include "Window/WindowingAPI.h"
#include "Utils/Utils.h"
#include "Utils/ErrorCodes/ErrorCodes.h"
#include "FileSystem/FileSystem.h"
#include "VulkanCommon.h"
#include "Layers/ImGui/ImGuiLayer.h"

#include "Objects/VulkanSemaphore.h"
#include "Objects/VulkanFence.h"
#include "Objects/VulkanSampler.h"
#include "Objects/VulkanBuffer.h"
#include "Objects/VulkanCommandPool.h"
#include "Objects/VulkanCommandBuffer.h"
#include "Objects/VulkanDescriptorPool.h"
#include "Objects/VulkanMemoryAllocator.h"
#include "Objects/VulkanDevice.h"
#include "Objects/VulkanPhysicalDevice.h"
#include "Objects/VulkanInstance.h"
#include "Objects/VulkanTexture.h"
#include "Objects/VulkanInits.h"
#include "Objects/VulkanShader.h"

#include "Graphics/API/ResourceLoader.h"
#include "Graphics/API/PipelineDescHash.h"
#include "Graphics/API/Objects/Pipeline.h"
#include "Graphics/API/Objects/RenderTarget.h"
#include "Graphics/API/Objects/PipelineCache.h"
#include "Graphics/API/Objects/QueryPool.h"
#include "Graphics/API/Objects/SwapChain.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ShaderManager.h"
#include "Graphics/Textures/Texture.h"

#ifndef TRAP_HEADLESS_MODE
	#define GETPERVIEWPORTDATA s_perViewportDataMap.at(&window)
	#define HEADLESS_CONSTEXPR
#else
	#define GETPERVIEWPORTDATA s_perViewportData
	#define HEADLESS_CONSTEXPR constexpr
#endif

TRAP::Scope<TRAP::Graphics::API::VulkanRenderer::NullDescriptors> TRAP::Graphics::API::VulkanRenderer::s_NullDescriptors = nullptr;

namespace
{
	std::unordered_map<u64, TRAP::Ref<TRAP::Graphics::Pipeline>> Pipelines{};
	std::unordered_map<u64, TRAP::Ref<TRAP::Graphics::PipelineCache>> PipelineCaches{};

	struct RenderPassData
	{
		//RenderPass map per thread (this will make lookups lock free and we only need a lock when inserting
		//a RenderPass Map for the first time)
		std::unordered_map<std::thread::id, TRAP::Graphics::API::VulkanRenderer::RenderPassMap> ThreadRenderPassMap;
		//FrameBuffer map per thread (this will make lookups lock free and we only need a lock when inserting
		//a FrameBuffer Map for the first time)
		std::unordered_map<std::thread::id, TRAP::Graphics::API::VulkanRenderer::FrameBufferMap> ThreadFrameBufferMap;
	};
	TRAP::Utils::Safe<RenderPassData> SafeRenderPassData{};

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat ShaderDataTypeToImageFormat(const TRAP::Graphics::ShaderDataType s)
	{
		switch(s)
		{
		case TRAP::Graphics::ShaderDataType::Float:
			return TRAP::Graphics::API::ImageFormat::R32_SFLOAT;

		case TRAP::Graphics::ShaderDataType::Float2:
			return TRAP::Graphics::API::ImageFormat::R32G32_SFLOAT;

		case TRAP::Graphics::ShaderDataType::Float3:
			return TRAP::Graphics::API::ImageFormat::R32G32B32_SFLOAT;

		case TRAP::Graphics::ShaderDataType::Float4:
			return TRAP::Graphics::API::ImageFormat::R32G32B32A32_SFLOAT;

		case TRAP::Graphics::ShaderDataType::Int:
			return TRAP::Graphics::API::ImageFormat::R32_SINT;

		case TRAP::Graphics::ShaderDataType::Int2:
			return TRAP::Graphics::API::ImageFormat::R32G32_SINT;

		case TRAP::Graphics::ShaderDataType::Int3:
			return TRAP::Graphics::API::ImageFormat::R32G32B32_SINT;

		case TRAP::Graphics::ShaderDataType::Int4:
			return TRAP::Graphics::API::ImageFormat::R32G32B32A32_SINT;

		case TRAP::Graphics::ShaderDataType::UInt:
			return TRAP::Graphics::API::ImageFormat::R32_SINT;

		case TRAP::Graphics::ShaderDataType::UInt2:
			return TRAP::Graphics::API::ImageFormat::R32G32_SINT;

		case TRAP::Graphics::ShaderDataType::UInt3:
			return TRAP::Graphics::API::ImageFormat::R32G32B32_SINT;

		case TRAP::Graphics::ShaderDataType::UInt4:
			return TRAP::Graphics::API::ImageFormat::R32G32B32A32_SINT;
		}

		TRAP_ASSERT(false, "ShaderDataTypeToImageFormat(): Unknown ShaderDataType!");
		return TRAP::Graphics::API::ImageFormat::Undefined;
	};

	//-------------------------------------------------------------------------------------------------------------------//

	void SavePipelineCaches()
	{
		const auto tempFolder = TRAP::FileSystem::GetGameTempFolderPath();
		if(!tempFolder)
			return;

		for(const auto& [hash, cache] : PipelineCaches)
		{
			if(!cache)
				continue;

			cache->Save(*tempFolder / fmt::format("{}.cache", hash));
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
	[[nodiscard]] u32 AcquireNextSwapchainImage(TRAP::Graphics::SwapChain& swapchain, TRAP::Graphics::Semaphore& acquireSemaphore)
	{
		std::optional<u32> acquiredImage = swapchain.AcquireNextImage(acquireSemaphore);

		if(!acquiredImage)
		{
			swapchain.UpdateFramebufferSize();
			acquiredImage = swapchain.AcquireNextImage(acquireSemaphore);
		}

		//Try again, if it also fails with the updated swapchain, quit engine
		if(!acquiredImage)
			TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::VulkanSwapchainImageAcquireFailed);

		return *acquiredImage;
	}
#endif /*TRAP_HEADLESS_MODE*/

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Ref<TRAP::Graphics::RenderTarget> GetShadingRateTexture(TRAP::Graphics::RendererAPI::PerViewportData& p)
	{
		//Get Shading rate texture (check if texture is big enough, else it would create errors so just deactivate it)

#ifndef TRAP_HEADLESS_MODE
		const auto internalRes = TRAP::Graphics::RendererAPI::GetInternalRenderResolution(*p.Window);
#else
		const auto internalRes = TRAP::Graphics::RendererAPI::GetInternalRenderResolution();
#endif /*TRAP_HEADLESS_MODE*/

		if(p.NewShadingRateTexture &&
			p.NewShadingRateTexture->GetWidth() >= NumericCast<u32>(TRAP::Math::Ceil(NumericCast<f32>(internalRes.x()) / NumericCast<f32>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateTexelWidth))) &&
			p.NewShadingRateTexture->GetHeight() >= NumericCast<u32>(TRAP::Math::Ceil(NumericCast<f32>(internalRes.y()) / NumericCast<f32>(TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateTexelHeight))))
		{
			p.CachedShadingRateTextures[p.ImageIndex] = p.NewShadingRateTexture;
			return p.NewShadingRateTexture;
		}

		return nullptr;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat FindSupportedDepthStencilFormat(const std::span<const TRAP::Graphics::API::ImageFormat> depthStencilFormats,
	                                                                                         const TRAP::Graphics::API::VulkanDevice& device)
	{
		using namespace TRAP::Graphics::API;

		for(const ImageFormat depthStencilFormat : depthStencilFormats)
		{
			const auto formatProps = device.GetPhysicalDevice().GetVkPhysicalDeviceFormatProperties(depthStencilFormat);
			if((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
				return depthStencilFormat;
		}

		TRAP_ASSERT(false, "VulkanRenderer::FindSupportedDepthStencilFormat(): Failed to find suitable depth/stencil format!");
		return ImageFormat::Undefined;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat PickDepthStencilFormat(const bool depthEnabled,
	                                                                                const bool stencilEnabled,
	                                                                                const TRAP::Graphics::API::VulkanDevice& device)
	{
		using namespace TRAP::Graphics::API;

		if(!depthEnabled && !stencilEnabled)
		{
			TRAP_ASSERT(false, "VulkanRenderer::PickDepthStencilFormat(): Depth and stencil are disabled!");
			return ImageFormat::Undefined;
		}

		std::vector<ImageFormat> depthStencilFormatCandidates{};
		depthStencilFormatCandidates.reserve(3);
		if(!stencilEnabled)
			depthStencilFormatCandidates.emplace_back(ImageFormat::D32_SFLOAT);
		depthStencilFormatCandidates.emplace_back(ImageFormat::D32_SFLOAT_S8_UINT);
		depthStencilFormatCandidates.emplace_back(ImageFormat::D24_UNORM_S8_UINT);

		return FindSupportedDepthStencilFormat(depthStencilFormatCandidates, device);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void CreateOrUpdateDepthStencilTarget(TRAP::Graphics::RendererAPI::PerViewportData& p,
	                                      const TRAP::Graphics::API::VulkanDevice& device)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                           (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

#ifndef TRAP_HEADLESS_MODE
		const TRAP::Math::Vec2ui newFbSize = p.Window->GetFrameBufferSize();
#elif defined(TRAP_HEADLESS_MODE)
		const TRAP::Math::Vec2ui newFbSize = {p.NewWidth, p.NewHeight};
#endif

		if (newFbSize.x() == 0 || newFbSize.y() == 0) //0x0 is an invalid framebuffer size
			return;

		const auto& gpd = std::get<TRAP::Graphics::RendererAPI::GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline);
		const auto& depthState = gpd.DepthState;

		//Nothing to do if depth/stencil testing is disabled
		if(!depthState->DepthTest && !depthState->StencilTest)
			return;

		//Nothing changed so no need to update/recreate depth/stencil attachment
		if(p.DepthStencilTarget != nullptr && p.DepthStencilTarget->GetWidth() == newFbSize.x() &&
		   p.DepthStencilTarget->GetHeight() == newFbSize.y() &&
		   p.DepthStencilTarget->GetImageFormat() == gpd.DepthStencilFormat &&
		   TRAP::Graphics::API::ImageFormatHasStencil(gpd.DepthStencilFormat) == depthState->StencilTest)
		{
			return;
		}

		const TRAP::Graphics::RendererAPI::RenderTargetDesc depthStencilRTDesc
		{
			.Flags{},
			.Width = newFbSize.x(),
			.Height = newFbSize.y(),
			.Depth = 1,
			.ArraySize = 1,
			.MipLevels = 1,
			.SampleCount = TRAP::Graphics::RendererAPI::SampleCount::One,
			.Format = PickDepthStencilFormat(depthState->DepthTest, depthState->StencilTest, device),
			.StartState = TRAP::Graphics::RendererAPI::ResourceState::DepthRead | TRAP::Graphics::RendererAPI::ResourceState::DepthWrite,
			.ClearValue = TRAP::Graphics::RendererAPI::DepthStencil{0.0f, 0},
			.SampleQuality = 0,
			.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::Texture,
			.Name = "Swapchain Depth/Stencil RenderTarget",
			.NativeHandle = nullptr
		};

		p.DepthStencilTarget = TRAP::Graphics::RenderTarget::Create(depthStencilRTDesc);
	}

	[[nodiscard]] HEADLESS_CONSTEXPR u32 RetrieveNextSwapchainImageIndex(const TRAP::Graphics::RendererAPI::PerViewportData& p)
	{
#ifndef TRAP_HEADLESS_MODE
		return AcquireNextSwapchainImage(*p.SwapChain, *p.ImageAcquiredSemaphores[p.ImageIndex]);
#else
		return (p.CurrentSwapChainImageIndex + 1) % TRAP::Graphics::RendererAPI::ImageCount;
#endif
	}

	[[nodiscard]] TRAP::Ref<TRAP::Graphics::RenderTarget> GetCurrentSwapchainRenderTarget(const TRAP::Graphics::RendererAPI::PerViewportData& p)
	{
#ifndef TRAP_HEADLESS_MODE
		return p.SwapChain->GetRenderTargets()[p.CurrentSwapChainImageIndex];
#else
		return p.RenderTargets[p.CurrentSwapChainImageIndex];
#endif
	}

	//-------------------------------------------------------------------------------------------------------------------//

	//Helper function to generate screenshot data. See CaptureScreenshot.
	template<typename T>
	[[nodiscard]] std::vector<T> MapRenderTarget(const TRAP::Ref<TRAP::Graphics::Queue>& graphicsQueue,
		                                         const TRAP::Graphics::RenderTarget& renderTarget,
	                                             const TRAP::Graphics::ResourceState currResState)
	requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

		TRAP_ASSERT(graphicsQueue, "VulkanRenderer::MapRenderTarget(): Graphics queue is nullptr!");

		const TRAP::Graphics::RendererAPI::CommandPoolDesc cmdPoolDesc
		{
			.Queue = graphicsQueue,
			.CreateFlags = TRAP::Graphics::RendererAPI::CommandPoolCreateFlags::Transient,
			.Name = fmt::format("MapRenderTarget CommandPool (Transient, QueueType: \"Graphics\")")
		};
		const TRAP::Ref<TRAP::Graphics::API::VulkanCommandPool> cmdPool = TRAP::MakeRef<TRAP::Graphics::API::VulkanCommandPool>(cmdPoolDesc);

		const std::string cmdBufferName = fmt::format("MapRenderTarget CommandBuffer (RenderTarget: \"{}\")", renderTarget.GetTexture()->GetName());
		TRAP::Graphics::CommandBuffer& cmd = cmdPool->GetCommandBuffer(false, cmdBufferName);

		//Add a staging buffer
		const u32 formatByteWidth = ImageFormatBitSizeOfBlock(renderTarget.GetImageFormat()) / 8u;
		const u64 pixelDataByteSize = NumericCast<u64>(renderTarget.GetWidth()) * renderTarget.GetHeight() * formatByteWidth;

		const TRAP::Graphics::RendererAPI::BufferDesc bufferDesc
		{
			.Size = pixelDataByteSize,
			.MemoryUsage = TRAP::Graphics::RendererAPI::ResourceMemoryUsage::GPUToCPU,
			.Flags = TRAP::Graphics::RendererAPI::BufferCreationFlags::PersistentMap | TRAP::Graphics::RendererAPI::BufferCreationFlags::NoDescriptorViewCreation,
			.QueueType = TRAP::Graphics::QueueType::Graphics,
			.StartState = TRAP::Graphics::ResourceState::CopyDestination,
			.ElementCount = bufferDesc.Size / formatByteWidth,
			.StructStride = formatByteWidth,
			.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::RWBuffer
		};
		const TRAP::Ref<TRAP::Graphics::Buffer> buffer = TRAP::Graphics::Buffer::Create(bufferDesc);

		//Start recording
		cmd.Begin();

		//Transition the render target to the correct state
		TRAP::Graphics::RendererAPI::RenderTargetBarrier srcBarrier{renderTarget, currResState, TRAP::Graphics::ResourceState::CopySource};
		cmd.ResourceBarrier(nullptr, nullptr, &srcBarrier);

		const u32 rowPitch = renderTarget.GetWidth() * formatByteWidth;
		const TRAP::Graphics::API::ImageFormat fmt = renderTarget.GetTexture()->GetImageFormat();
		const u32 numBlocksWide = rowPitch / (ImageFormatBitSizeOfBlock(fmt) >> 3u);
		const u32 slicePitch = numBlocksWide * ImageFormatWidthOfBlock(fmt);

		const TRAP::Graphics::RendererAPI::SubresourceDataDesc subResDesc
		{
			.SrcOffset = 0,
			.MipLevel = 0,
			.ArrayLayer = 0,
			.RowPitch = rowPitch,
			.SlicePitch = slicePitch
		};
		cmd.CopySubresource(*buffer, *renderTarget.GetTexture(), subResDesc);

		//Transition the render target back to the previous state
		srcBarrier = {renderTarget, TRAP::Graphics::ResourceState::CopySource, currResState};
		cmd.ResourceBarrier(nullptr, nullptr, &srcBarrier);

		//End recording
		cmd.End();

		//Submit the command buffer
		const TRAP::Graphics::RendererAPI::QueueSubmitDesc submitDesc
		{
			.Cmds = {cmd}
		};
		graphicsQueue->Submit(submitDesc);

		//Wait for work to finish on the GPU
		graphicsQueue->WaitQueueIdle();

		const std::span<u8> gpuPixelData = buffer->GetCPUMappedAddress();

		//Copy to CPU memory.
		std::vector<T> pixelData(pixelDataByteSize / sizeof(T));
		const std::span outDataBytes = TRAP::Utils::AsWritableBytes(std::span{pixelData});
		std::ranges::copy(gpuPixelData.subspan(0, pixelDataByteSize), outDataBytes.begin());

		return pixelData;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] std::vector<std::string> SetupInstanceLayers()
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

		std::vector<std::string> layers{};

#ifdef ENABLE_GRAPHICS_DEBUG
		//Turn on all validations
		if (TRAP::Graphics::API::VulkanInstance::IsLayerSupported("VK_LAYER_KHRONOS_validation"))
			layers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif /*ENABLE_GRAPHICS_DEBUG*/

		return layers;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the list of instance extensions to use.
	///
	/// 1. Surface extension(s) (Mandatory when not using Headless mode).
	/// 2. Debug extension (if supported) (Only active when ENABLE_GRAPHICS_DEBUG is defined).
	/// 3. HDR extension (if supported).
	/// 4. VR extension(s) (if supported).
	/// @return List of instance extensions.
	[[nodiscard]] std::vector<std::string> SetupInstanceExtensions()
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

		std::vector<std::string> extensions{};

#ifndef TRAP_HEADLESS_MODE
		const auto reqExt = TRAP::INTERNAL::WindowingAPI::GetRequiredInstanceExtensions();

		if(!TRAP::Graphics::API::VulkanInstance::IsExtensionSupported(std::get<0>(reqExt)) ||
		   !TRAP::Graphics::API::VulkanInstance::IsExtensionSupported(std::get<1>(reqExt)))
			TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::VulkanSurfaceExtensionsUnsupported);
		else
		{
			extensions.push_back(std::get<0>(reqExt));
			extensions.push_back(std::get<1>(reqExt));
		}
#endif /*TRAP_HEADLESS_MODE*/

		//Vulkan 1.1 core
		//VK_KHR_get_physical_device_properties2
		//VK_KHR_external_memory_capabilities
		//VK_KHR_external_semaphore_capabilities
		//VK_KHR_external_fence_capabilities

#ifdef ENABLE_GRAPHICS_DEBUG

		if(TRAP::Graphics::API::VulkanInstance::IsExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
			extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		if(TRAP::Graphics::API::VulkanInstance::IsExtensionSupported(VK_EXT_DEBUG_REPORT_EXTENSION_NAME))
			extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

#ifdef ENABLE_GPU_BASED_VALIDATION
		if(TRAP::Graphics::API::VulkanInstance::IsExtensionSupported(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME))
			extensions.emplace_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
#endif /*ENABLE_GPU_BASED_VALIDATION*/

#endif /*ENABLE_GRAPHICS_DEBUG*/

#ifndef TRAP_HEADLESS_MODE
		///HDR support (requires surface extension)
		if (TRAP::Graphics::API::VulkanInstance::IsExtensionSupported(VK_KHR_SURFACE_EXTENSION_NAME) &&
			TRAP::Graphics::API::VulkanInstance::IsExtensionSupported(VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME))
		{
			extensions.emplace_back(VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME);
		}
#endif /*TRAP_HEADLESS_MODE*/

		return extensions;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the list of device extensions to use.
	///
	/// 1. Swapchain extension (Mandatory when not using Headless mode)
	/// 2. Indirect drawing extension (if supported).
	/// 3. Fragment shader interlock extension (if supported).
	/// 4. Descriptor indexing extension (if supported).
	/// 5. RayTracing extensions (if supported).
	/// 6. YCbCr conversion extension (if supported).
	/// 7. Shader draw parameters extension (if supported).
	/// @param physicalDevice Physical device to use.
	/// @return List of device extensions.
	[[nodiscard]] std::vector<std::string> SetupDeviceExtensions(const TRAP::Graphics::API::VulkanPhysicalDevice& physicalDevice)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

		std::vector<std::string> extensions{};

		if(physicalDevice.IsExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
			extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	#ifndef TRAP_HEADLESS_MODE
		else
			TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::VulkanSwapchainExtensionsUnsupported);
	#endif /*TRAP_HEADLESS_MODE*/

		//Vulkan 1.1 core
		//VK_KHR_maintenance1
		//VK_KHR_maintenance2
		//VK_KHR_maintenance3
		//VK_KHR_external_memory
		//VK_KHR_external_semaphore
		//VK_KHR_external_fence
		//VK_KHR_bind_memory2
		//VK_KHR_get_memory_requirements2
		//VK_KHR_sampler_ycbcr_conversion (needs check of capability flag)
		//VK_KHR_dedicated_allocation
		//VK_KHR_shader_draw_parameters (needs check of capability flag)
		//VK_KHR_multiview

		//Debug marker extension in case debug utils is not supported
	#ifdef ENABLE_GRAPHICS_DEBUG
		if (TRAP::Graphics::API::VulkanInstance::IsExtensionSupported(VK_EXT_DEBUG_REPORT_EXTENSION_NAME) &&
			physicalDevice.IsExtensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
		{
			extensions.emplace_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
		}
	#endif /*ENABLE_GRAPHICS_DEBUG*/

		if (physicalDevice.IsExtensionSupported(VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME))
			extensions.emplace_back(VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME);

		if (physicalDevice.IsExtensionSupported(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME))
			extensions.emplace_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

		if (physicalDevice.IsExtensionSupported(VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME))
			extensions.emplace_back(VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME);

		if (physicalDevice.IsExtensionSupported(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME))
			extensions.emplace_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);

		if (physicalDevice.IsExtensionSupported(VK_KHR_MAINTENANCE_4_EXTENSION_NAME))
			extensions.emplace_back(VK_KHR_MAINTENANCE_4_EXTENSION_NAME);

		if (physicalDevice.IsExtensionSupported(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME))
			extensions.emplace_back(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);

		if (physicalDevice.IsExtensionSupported(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME))
			extensions.emplace_back(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);

		if (physicalDevice.IsExtensionSupported(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME) &&
			physicalDevice.IsExtensionSupported(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME))
		{
			extensions.emplace_back(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME);
		}

		if (physicalDevice.IsExtensionSupported(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME))
			extensions.emplace_back(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);

		if(physicalDevice.IsExtensionSupported(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME))
			extensions.emplace_back(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);

		if(physicalDevice.IsExtensionSupported(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME) &&
		   physicalDevice.IsExtensionSupported(VK_KHR_SPIRV_1_4_EXTENSION_NAME))
		{
			extensions.emplace_back(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
		}

		//RayTracing
		if(physicalDevice.IsExtensionSupported(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME) &&
		   physicalDevice.IsExtensionSupported(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME) &&
		   physicalDevice.IsExtensionSupported(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) &&
		   physicalDevice.IsExtensionSupported(VK_KHR_SPIRV_1_4_EXTENSION_NAME) &&
		   physicalDevice.IsExtensionSupported(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME) &&
		   physicalDevice.IsExtensionSupported(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME) &&
		   physicalDevice.IsExtensionSupported(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME) &&
		   physicalDevice.IsExtensionSupported(VK_KHR_RAY_QUERY_EXTENSION_NAME))
		{
			extensions.emplace_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
			extensions.emplace_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
			extensions.emplace_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
			extensions.emplace_back(VK_KHR_RAY_QUERY_EXTENSION_NAME);
		}

		return extensions;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderer::~VulkanRenderer()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TP_DEBUG(Log::RendererVulkanPrefix, "Destroying Renderer");

	s_descriptorPool.reset();

	RemoveDefaultResources();

	auto renderPassData = SafeRenderPassData.WriteLock();
	renderPassData->ThreadFrameBufferMap.clear();
	renderPassData->ThreadRenderPassMap.clear();

	Pipelines.clear();

	SavePipelineCaches();
	PipelineCaches.clear();

	//Free everything in order
	//Should happen automagically through Scope destructors
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::StartGraphicRecording(PerViewportData& p) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(p.Recording)
		return;

	const TRAP::Ref<Fence>& renderCompleteFence = p.RenderCompleteFences[p.ImageIndex];

	//Stall if CPU is running "Swap Chain Buffer Count" frames ahead of GPU
	if (renderCompleteFence->GetStatus() == FenceStatus::Incomplete)
		renderCompleteFence->Wait();

	//Start Recording
	p.CurrentSwapChainImageIndex = RetrieveNextSwapchainImageIndex(p);

	//TODO Always use back buffer (InternalRenderTarget), copy/resolve final image to SwapChain RenderTargert as last step in EndGraphicRecording().
	TRAP::Ref<RenderTarget> bindRenderTarget{};
	if(p.RenderScale != 1.0f || p.CurrentAntiAliasing == RendererAPI::AntiAliasing::MSAA)
		bindRenderTarget = p.InternalRenderTargets[p.CurrentSwapChainImageIndex];
	else
		bindRenderTarget = GetCurrentSwapchainRenderTarget(p);

	std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline).ShadingRateTexture = GetShadingRateTexture(p);

	//Reset cmd pool for this frame
	p.GraphicCommandPools[p.ImageIndex]->Reset();

	p.GraphicCommandBuffers[p.ImageIndex]->Begin();

	BeginGPUFrameProfile(QueueType::Graphics, p);

#ifndef TRAP_HEADLESS_MODE
	const RenderTargetBarrier swapChainBarrier{*GetCurrentSwapchainRenderTarget(p), ResourceState::Present, ResourceState::RenderTarget};
	p.GraphicCommandBuffers[p.ImageIndex]->ResourceBarrier(nullptr, nullptr, &swapChainBarrier);
#endif /*TRAP_HEADLESS_MODE*/

	LoadActionsDesc loadActions
	{
		.LoadActionsColor = {LoadActionType::Clear},
		.LoadActionDepth = LoadActionType::Clear,
		.LoadActionStencil = LoadActionType::Clear,
		.ClearColorValues = {p.ClearColor},
		.ClearDepthStencil = p.ClearDepthStencil,
		.StoreActionsColor = {},
		.StoreActionDepth = StoreActionType::Store,
		.StoreActionStencil = StoreActionType::Store
	};
#ifndef TRAP_HEADLESS_MODE
	BindRenderTargets({*bindRenderTarget}, p.DepthStencilTarget.get(), &loadActions, nullptr, nullptr, std::numeric_limits<u32>::max(),
	                  std::numeric_limits<u32>::max(), *p.Window);
#else
	BindRenderTargets({*bindRenderTarget}, p.DepthStencilTarget.get(), &loadActions, nullptr, nullptr, std::numeric_limits<u32>::max(),
	                  std::numeric_limits<u32>::max());
#endif

	//Set Default Dynamic Viewport & Scissor
	const u32 width = bindRenderTarget->GetWidth();
	const u32 height = bindRenderTarget->GetHeight();

	p.GraphicCommandBuffers[p.ImageIndex]->SetViewport(0.0f, 0.0f, NumericCast<f32>(width),
														 NumericCast<f32>(height), 0.0f, 1.0f);
	p.GraphicCommandBuffers[p.ImageIndex]->SetScissor(0, 0, width, height);
	if(p.CurrentGraphicsPipeline)
		p.GraphicCommandBuffers[p.ImageIndex]->BindPipeline(*(p.CurrentGraphicsPipeline));

	p.Recording = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::EndGraphicRecording(PerViewportData& p)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(!p.Recording)
		return;

	if(p.CurrentAntiAliasing == RendererAPI::AntiAliasing::MSAA && p.RenderScale == 1.0f) //Inject MSAA resolve pass
	{
		const TRAP::Ref<RenderTarget> presentRenderTarget = GetCurrentSwapchainRenderTarget(p);
		const TRAP::Ref<RenderTarget>& MSAARenderTarget = p.InternalRenderTargets[p.CurrentSwapChainImageIndex];

		Graphics::RendererAPI::GetRenderer()->MSAAResolvePass(*MSAARenderTarget, *presentRenderTarget, p.GraphicCommandBuffers[p.ImageIndex]);
	}

#ifndef TRAP_HEADLESS_MODE
	//Transition RenderTarget to Present
	p.GraphicCommandBuffers[p.ImageIndex]->BindRenderTargets({}, nullptr, nullptr, nullptr, nullptr,
																std::numeric_limits<u32>::max(),
																std::numeric_limits<u32>::max(), nullptr);
	const TRAP::Ref<RenderTarget> presentRenderTarget = GetCurrentSwapchainRenderTarget(p);
	const RenderTargetBarrier barrier{*presentRenderTarget, ResourceState::RenderTarget, ResourceState::Present};
	p.GraphicCommandBuffers[p.ImageIndex]->ResourceBarrier(nullptr, nullptr, &barrier);
#else
	p.GraphicCommandBuffers[p.ImageIndex]->BindRenderTargets({}, nullptr, nullptr, nullptr, nullptr,
																std::numeric_limits<u32>::max(),
																std::numeric_limits<u32>::max(), nullptr);
#endif

	EndGPUFrameProfile(QueueType::Graphics, p);

	//End Recording
	p.GraphicCommandBuffers[p.ImageIndex]->End();

	const QueueSubmitDesc submitDesc
	{
		.Cmds = { *p.GraphicCommandBuffers[p.ImageIndex] },
		.SignalFence = p.RenderCompleteFences[p.ImageIndex],
#ifndef TRAP_HEADLESS_MODE
		.WaitSemaphores = { p.ImageAcquiredSemaphores[p.ImageIndex], p.ComputeCompleteSemaphores[p.ImageIndex] },
		.SignalSemaphores = { p.RenderCompleteSemaphores[p.ImageIndex] }
#else
		.WaitSemaphores = { p.ComputeCompleteSemaphores[p.ImageIndex] },
		.SignalSemaphores = {}
#endif
	};
	s_graphicQueue->Submit(submitDesc);

	p.GraphicsFrameTime = ResolveGPUFrameProfile(QueueType::Graphics, p);

	p.Recording = false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::StartComputeRecording(PerViewportData& p)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	p.State = PerWindowState::PreUpdate;

	if(p.RecordingCompute)
		return;

	//Start Recording
	const TRAP::Ref<Fence>& computeCompleteFence = p.ComputeCompleteFences[p.ImageIndex];

	//Stall if CPU is running "Swap Chain Buffer Count" frames ahead of GPU
	if (computeCompleteFence->GetStatus() == FenceStatus::Incomplete)
		computeCompleteFence->Wait();

	//Reset cmd pool for this frame
	p.ComputeCommandPools[p.ImageIndex]->Reset();

	p.ComputeCommandBuffers[p.ImageIndex]->Begin();

	BeginGPUFrameProfile(QueueType::Compute, p);

	if(p.CurrentComputePipeline)
		p.ComputeCommandBuffers[p.ImageIndex]->BindPipeline(*(p.CurrentComputePipeline));

	p.RecordingCompute = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::EndComputeRecording(PerViewportData& p)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(!p.RecordingCompute)
		return;

	EndGPUFrameProfile(QueueType::Compute, p);

	//End Recording
	p.ComputeCommandBuffers[p.ImageIndex]->End();

	const QueueSubmitDesc submitDesc
	{
		.Cmds = { *p.ComputeCommandBuffers[p.ImageIndex] },
		.SignalFence = {p.ComputeCompleteFences[p.ImageIndex]},
		.WaitSemaphores = { p.GraphicsCompleteSemaphores[p.ImageIndex] },
		.SignalSemaphores = { p.ComputeCompleteSemaphores[p.ImageIndex] }
	};
	s_computeQueue->Submit(submitDesc);

	p.ComputeFrameTime = ResolveGPUFrameProfile(QueueType::Compute, p);

	p.RecordingCompute = false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::Present(PerViewportData& p) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifndef TRAP_HEADLESS_MODE

#ifdef NVIDIA_REFLEX_AVAILABLE
	ReflexMarker(Application::GetGlobalCounter(), PCLSTATS_PRESENT_START);
#endif /*NVIDIA_REFLEX_AVAILABLE*/

	const QueuePresentDesc presentDesc
	{
		.SwapChain = p.SwapChain,
		.WaitSemaphores = { p.RenderCompleteSemaphores[p.ImageIndex] },
		.Index = p.CurrentSwapChainImageIndex
	};
	const PresentStatus presentStatus = s_graphicQueue->Present(presentDesc);

#ifdef NVIDIA_REFLEX_AVAILABLE
	ReflexMarker(Application::GetGlobalCounter(), PCLSTATS_PRESENT_END);
#endif /*NVIDIA_REFLEX_AVAILABLE*/

#endif /*TRAP_HEADLESS_MODE*/

	FrameMark;

	p.ImageIndex = (p.ImageIndex + 1) % RendererAPI::ImageCount;

#ifndef TRAP_HEADLESS_MODE
	if (presentStatus == PresentStatus::OutOfDate || p.ResizeSwapChain)
	{
		p.ResizeSwapChain = false;

		p.SwapChain->UpdateFramebufferSize();
		if(p.DepthStencilTarget)
			CreateOrUpdateDepthStencilTarget(p, *m_device);

		p.CurrentSwapChainImageIndex = 0;
		p.ImageIndex = 0;
	}
	else if (presentStatus == PresentStatus::DeviceReset || presentStatus == PresentStatus::Failed)
	{
		if (presentStatus == PresentStatus::DeviceReset)
			Utils::DisplayError(Utils::ErrorCode::VulkanDeviceReset);
		else if (presentStatus == PresentStatus::Failed)
			Utils::DisplayError(Utils::ErrorCode::VulkanPresentationFailed);
	}
#else /*TRAP_HEADLESS_MODE*/
	if(p.Resize)
	{
		p.Resize = false;

		if(p.NewWidth > 0 && p.NewHeight > 0)
		{
			RendererAPI::RenderTargetDesc rTDesc{};
			rTDesc.Width = p.NewWidth;
			rTDesc.Height = p.NewHeight;
			rTDesc.Depth = 1;
			rTDesc.ArraySize = 1;
			rTDesc.MipLevels = 1;
			rTDesc.Format = GetRecommendedSwapchainFormat(true, false);
			rTDesc.StartState = RendererAPI::ResourceState::RenderTarget;
			rTDesc.SampleCount = SampleCount::One;
			for(u32 i = 0; i < RendererAPI::ImageCount; ++i)
			{
				rTDesc.Name = fmt::format("Color RenderTarget (Index: {})", i);
				p.RenderTargets[i] = RenderTarget::Create(rTDesc);
			}

			p.CurrentSwapChainImageIndex = 0;
			p.ImageIndex = 0;

			if(p.DepthStencilTarget)
				CreateOrUpdateDepthStencilTarget(p, *m_device);
		}
	}
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::InitInternal(const std::string_view gameName)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	m_instance = TRAP::MakeRef<VulkanInstance>(gameName, SetupInstanceLayers(), SetupInstanceExtensions());
#ifdef ENABLE_GRAPHICS_DEBUG
	m_debug = TRAP::MakeScope<VulkanDebug>(m_instance);
#endif /*ENABLE_GRAPHICS_DEBUG*/

	const auto& physicalDevices = VulkanPhysicalDevice::GetAllRatedPhysicalDevices(*m_instance);
	TRAP_ASSERT(!physicalDevices.empty(), "VulkanRenderer::InitInternal(): There are no usable physical devices!");

	TRAP::Scope<VulkanPhysicalDevice> physicalDevice;

	//Get Vulkan GPU UUID
	TRAP::Utils::UUID gpuUUID{};
	const std::optional<std::string> UUIDstr = TRAP::Application::GetConfig().Get<std::string>("VulkanGPU");
	if(UUIDstr)
		gpuUUID = TRAP::Utils::UUIDFromString(*UUIDstr);

	if(gpuUUID == TRAP::Utils::EMPTY_UUID)
	{
		TP_ERROR(Log::RendererVulkanPrefix, "Invalid GPU UUID: \"", Utils::UUIDToString(gpuUUID), "\"!");
		TP_ERROR(Log::RendererVulkanPrefix, "Falling back to score based system");

		const auto& [highscore, ratedDev] = *physicalDevices.rbegin();
		physicalDevice = TRAP::MakeScope<VulkanPhysicalDevice>(m_instance, ratedDev.PhysicalDeviceUUID);
	}
	else
	{
		for (const auto& [score, ratedDev] : physicalDevices)
		{
			if(gpuUUID != ratedDev.PhysicalDeviceUUID)
				continue;

			physicalDevice = TRAP::MakeScope<VulkanPhysicalDevice>(m_instance, ratedDev.PhysicalDeviceUUID);
			break;
		}

		if(!physicalDevice)
		{
			TP_ERROR(Log::RendererVulkanPrefix, "Could not find a GPU with UUID: \"", Utils::UUIDToString(gpuUUID), "\"!");
			TP_ERROR(Log::RendererVulkanPrefix, "Falling back to score based system");

			const auto& [highscore, ratedDev] = *physicalDevices.rbegin();
			physicalDevice = TRAP::MakeScope<VulkanPhysicalDevice>(m_instance, ratedDev.PhysicalDeviceUUID);
		}
	}

	m_device = TRAP::MakeRef<VulkanDevice>(std::move(physicalDevice), SetupDeviceExtensions(*physicalDevice));

	m_vma = TRAP::MakeRef<VulkanMemoryAllocator>(m_device, m_instance);

	s_descriptorPool = TRAP::MakeRef<VulkanDescriptorPool>(8192, "VulkanRenderer Global DescriptorPool");

	AddDefaultResources();

	const VkPhysicalDeviceProperties& devProps = m_device->GetPhysicalDevice().GetVkPhysicalDeviceProperties();
	const VkPhysicalDeviceDriverProperties& devDriverProps = m_device->GetPhysicalDevice().GetVkPhysicalDeviceDriverProperties();
	TP_INFO(Log::RendererVulkanPrefix, "----------------------------------");
	TP_INFO(Log::RendererVulkanPrefix, "Vulkan Instance Version: ",
		VK_VERSION_MAJOR(VulkanInstance::GetInstanceVersion().ValueOr(0)), '.',
		VK_VERSION_MINOR(VulkanInstance::GetInstanceVersion().ValueOr(0)), '.',
		VK_VERSION_PATCH(VulkanInstance::GetInstanceVersion().ValueOr(0)));
	TP_INFO(Log::RendererVulkanPrefix, "Vulkan Device Version: ", VK_VERSION_MAJOR(devProps.apiVersion), '.',
	        VK_VERSION_MINOR(devProps.apiVersion), '.', VK_VERSION_PATCH(devProps.apiVersion));
	TP_INFO(Log::RendererVulkanPrefix, "Driver Version: ", VK_VERSION_MAJOR(devProps.driverVersion), '.',
	        VK_VERSION_MINOR(devProps.driverVersion), '.', VK_VERSION_PATCH(devProps.driverVersion));
	TP_INFO(Log::RendererVulkanPrefix, "Driver: ", devDriverProps.driverName, " (", devDriverProps.driverInfo, ')');
	TP_INFO(Log::RendererVulkanPrefix, "Renderer: ", devProps.deviceName);
	TP_INFO(Log::RendererVulkanPrefix, "----------------------------------");

	m_rendererTitle = fmt::format("[Vulkan {}.{}.{}]",
	                              VK_VERSION_MAJOR(devProps.apiVersion),
	                              VK_VERSION_MINOR(devProps.apiVersion),
								  VK_VERSION_PATCH(devProps.apiVersion));
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::Flush(const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::Flush() const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	PerViewportData& p = *GETPERVIEWPORTDATA;

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	ReflexMarker(Application::GetGlobalCounter(), PCLSTATS_RENDERSUBMIT_START);
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

	EndComputeRecording(p);
	EndGraphicRecording(p);

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	ReflexMarker(Application::GetGlobalCounter(), PCLSTATS_RENDERSUBMIT_END);
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

	Present(p);

	Sampler::UpdateSamplers();

#ifndef TRAP_HEADLESS_MODE
	if (p.CurrentVSync != p.NewVSync) //Change V-Sync state only between frames!
	{
		if(p.SwapChain)
			p.SwapChain->ToggleVSync();
		p.CurrentVSync = p.NewVSync;
	}
#endif /*TRAP_HEADLESS_MODE*/

	if(p.RenderScale != p.NewRenderScale)
		p.RenderScale = p.NewRenderScale;

	UpdateInternalRenderTargets(p);

	StartComputeRecording(p);
	StartGraphicRecording(p);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::Dispatch(std::array<u32, 3> workGroupElements, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::Dispatch(std::array<u32, 3> workGroupElements) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	if(!p.RecordingCompute)
		return;

	//Check if we have an actual shader bound
	if(!p.CurrentComputePipeline)
	{
		//Bind fallback shader
#ifndef TRAP_HEADLESS_MODE
		this->BindShader(*TRAP::Graphics::ShaderManager::Get(RendererAPI::ShaderType::Compute, "FallbackCompute"), *p.Window);
#else
		this->BindShader(*TRAP::Graphics::ShaderManager::Get(RendererAPI::ShaderType::Compute, "FallbackCompute"));
#endif /*TRAP_HEADLESS_MODE*/
	}

	//Calculate used work group sizes
	for(usize i = 0; i < workGroupElements.size(); ++i)
		workGroupElements[i] = NumericCast<u32>(TRAP::Math::Round(NumericCast<f32>(workGroupElements[i]) / NumericCast<f32>(p.CurrentComputeWorkGroupSize[i])));

	p.ComputeCommandBuffers[p.ImageIndex]->Dispatch(std::get<0>(workGroupElements), std::get<1>(workGroupElements), std::get<2>(workGroupElements));
}

//------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetVSync(const bool vsync, const Window& window) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	s_perViewportDataMap.at(&window)->NewVSync = vsync;
}
#endif /*TRAP_HEADLESS_MODE*/

//------------------------------------------------------------------------------------------------------------------//

#if !defined(TRAP_HEADLESS_MODE) && defined(NVIDIA_REFLEX_AVAILABLE)
void TRAP::Graphics::API::VulkanRenderer::SetReflexFPSLimit(const u32 limit)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(!GPUSettings.ReflexSupported)
		return;

	for(auto& [win, viewportData] : s_perViewportDataMap)
	{
		if(limit == 0)
			viewportData->SleepModeParams.minimumIntervalUs = 0;
		else
			viewportData->SleepModeParams.minimumIntervalUs = NumericCast<u32>((1000.0f / NumericCast<f32>(limit)) * 1000.0f);

		if(viewportData->SleepModeParams.bLowLatencyMode && viewportData->SleepModeParams.bLowLatencyBoost)
			SetLatencyMode(LatencyMode::EnabledBoost, *win);
		else if(viewportData->SleepModeParams.bLowLatencyMode)
			SetLatencyMode(LatencyMode::Enabled, *win);
		else
			SetLatencyMode(LatencyMode::Disabled, *win);
	}
}
#endif /*!defined(TRAP_HEADLESS_MODE) && defined(NVIDIA_REFLEX_AVAILABLE)*/

//------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetRenderScale(f32 scale, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetRenderScale(f32 scale) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	scale = TRAP::Math::Clamp(scale, 0.5f, 2.0f);

	GETPERVIEWPORTDATA->NewRenderScale = scale;
}

//------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] f32 TRAP::Graphics::API::VulkanRenderer::GetRenderScale(const Window& window) const
#else
[[nodiscard]] f32 TRAP::Graphics::API::VulkanRenderer::GetRenderScale() const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	return GETPERVIEWPORTDATA->RenderScale;
}

//------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetClearColor(const RendererAPI::Color& color, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetClearColor(const RendererAPI::Color& color) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	GETPERVIEWPORTDATA->ClearColor = color;
}

//------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetClearDepth(const f32 depth, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetClearDepth(const f32 depth) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	GETPERVIEWPORTDATA->ClearDepthStencil.Depth = depth;
}

//------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetClearStencil(const u32 stencil, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetClearStencil(const u32 stencil) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	GETPERVIEWPORTDATA->ClearDepthStencil.Stencil = stencil;
}

//------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetResolution(const u32 width, const u32 height) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(width > 0 && height > 0, "VulkanRenderer::SetResolution(): Invalid render target resolution!");

	if(width > 0 && height > 0)
	{
		s_perViewportData->Resize = true;
		s_perViewportData->NewWidth = width;
		s_perViewportData->NewHeight = height;
	}
}
#endif /*TRAP_HEADLESS_MODE*/

//------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::Math::Vec2ui TRAP::Graphics::API::VulkanRenderer::GetResolution() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	return {s_perViewportData->NewWidth, s_perViewportData->NewHeight};
}
#endif /*TRAP_HEADLESS_MODE*/

//------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetDepthTesting(const bool enabled, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetDepthTesting(const bool enabled) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	PerViewportData& p = *GETPERVIEWPORTDATA;

	std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline).DepthState->DepthTest = enabled;

	if(enabled && !p.DepthStencilTarget)
		CreateOrUpdateDepthStencilTarget(p, *m_device);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetDepthWriting(const bool enabled, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetDepthWriting(const bool enabled) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline).DepthState->DepthWrite = enabled;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetDepthFunction(const CompareMode function, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetDepthFunction(const CompareMode function) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline).DepthState->DepthFunc = function;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetDepthFail(const StencilOp front, const StencilOp back, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetDepthFail(const StencilOp front, const StencilOp back) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	const GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline);

	gpd.DepthState->DepthFrontFail = front;
	gpd.DepthState->DepthBackFail = back;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetDepthBias(const i32 depthBias, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetDepthBias(const i32 depthBias) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline).RasterizerState->DepthBias = depthBias;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetDepthBiasSlopeFactor(const f32 factor, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetDepthBiasSlopeFactor(const f32 factor) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline).RasterizerState->SlopeScaledDepthBias = factor;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetStencilTesting(const bool enabled, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetStencilTesting(const bool enabled) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	PerViewportData& p = *GETPERVIEWPORTDATA;

	std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline).DepthState->StencilTest = enabled;

	if(enabled && !p.DepthStencilTarget)
		CreateOrUpdateDepthStencilTarget(p, *m_device);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetStencilFail(const StencilOp front, const StencilOp back,
                                                         const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetStencilFail(const StencilOp front, const StencilOp back) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	const GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline);

	gpd.DepthState->StencilFrontFail = front;
	gpd.DepthState->StencilBackFail = back;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetStencilPass(const StencilOp front, const StencilOp back, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetStencilPass(const StencilOp front, const StencilOp back) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	const GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline);

	gpd.DepthState->StencilFrontPass = front;
	gpd.DepthState->StencilBackPass = back;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetStencilFunction(const CompareMode front, const CompareMode back,
                                                             const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetStencilFunction(const CompareMode front, const CompareMode back) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	const GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline);

	gpd.DepthState->StencilFrontFunc = front;
	gpd.DepthState->StencilBackFunc = back;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetStencilMask(const u8 read, const u8 write, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetStencilMask(const u8 read, const u8 write) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	const GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline);

	gpd.DepthState->StencilReadMask = read;
	gpd.DepthState->StencilWriteMask = write;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetCullMode(const CullMode mode, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetCullMode(const CullMode mode) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline).RasterizerState->CullMode = mode;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetFillMode(const FillMode mode, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetFillMode(const FillMode mode) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(mode != FillMode::Solid && !GPUSettings.FillModeNonSolid)
		return;

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline).RasterizerState->FillMode = mode;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetPrimitiveTopology(const PrimitiveTopology topology, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetPrimitiveTopology(const PrimitiveTopology topology) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	PerViewportData& p = *GETPERVIEWPORTDATA;

	std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline).PrimitiveTopology = topology;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetFrontFace(const FrontFace face, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetFrontFace(const FrontFace face) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline).RasterizerState->FrontFace = face;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetBlendMode(const BlendMode modeRGB, const BlendMode modeAlpha,
                                                       const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetBlendMode(const BlendMode modeRGB, const BlendMode modeAlpha) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	const GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline);

	gpd.BlendState->BlendModes = { modeRGB };
	gpd.BlendState->BlendAlphaModes = { modeAlpha };
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetBlendConstant(const BlendConstant sourceRGB,
														   const BlendConstant sourceAlpha,
                                                           const BlendConstant destinationRGB,
                                                           const BlendConstant destinationAlpha, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetBlendConstant(const BlendConstant sourceRGB,
														   const BlendConstant sourceAlpha,
                                                           const BlendConstant destinationRGB,
                                                           const BlendConstant destinationAlpha) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	const GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline);

	gpd.BlendState->SrcFactors = {sourceRGB};
	gpd.BlendState->DstFactors = {destinationRGB};
	gpd.BlendState->SrcAlphaFactors = {sourceAlpha};
	gpd.BlendState->DstAlphaFactors = {destinationAlpha};
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetShadingRate(const ShadingRate shadingRate,
		                            					 ShadingRateCombiner postRasterizerRate,
							        					 ShadingRateCombiner finalRate, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetShadingRate(const ShadingRate shadingRate,
		                            					 ShadingRateCombiner postRasterizerRate,
							        					 ShadingRateCombiner finalRate) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(RendererAPI::GPUSettings.ShadingRateCaps == RendererAPI::ShadingRateCaps::NotSupported) //VRS is not supported
	{
		TRAP_ASSERT(false, "VulkanRenderer::SetShadingRate(): Shading rate is not supported by this device!");
		return;
	}

	PerViewportData& p = *GETPERVIEWPORTDATA;

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline);

	if(gpd.DepthState->DepthWrite)
	{
		TRAP_ASSERT(false, "VulkanRenderer::SetShadingRate(): Depth write is enabled, shading rate cannot be set");
		TP_ERROR(Log::RendererVulkanPrefix, "VulkanRenderer::SetShadingRate(): Depth write is enabled, shading rate cannot be set");
		return;
	}

	if(gpd.ShadingRateTexture)
		p.NewShadingRateTexture = nullptr;

	if((RendererAPI::GPUSettings.ShadingRateCaps & RendererAPI::ShadingRateCaps::PerDraw) == RendererAPI::ShadingRateCaps::NotSupported)
		return;

	if(std::to_underlying(RendererAPI::GPUSettings.ShadingRateCombiner & postRasterizerRate) == 0u)
	{
		TP_ERROR(Log::RendererVulkanPrefix, "VulkanRenderer::SetShadingRate(): Shading rate combiner is not supported by this device!");
		return;
	}
	if(std::to_underlying(RendererAPI::GPUSettings.ShadingRateCombiner & finalRate) == 0u)
	{
		TP_ERROR(Log::RendererVulkanPrefix, "VulkanRenderer::SetShadingRate(): Shading rate combiner is not supported by this device!");
		return;
	}

	//VUID-vkCmdSetFragmentShadingRateKHR-primitiveFragmentShadingRate-04510
	if((RendererAPI::GPUSettings.ShadingRateCaps & RendererAPI::ShadingRateCaps::PerPrimitive) == RendererAPI::ShadingRateCaps::NotSupported)
		postRasterizerRate = ShadingRateCombiner::Passthrough;
	//VUID-vkCmdSetFragmentShadingRateKHR-attachmentFragmentShadingRate-04511
	if((RendererAPI::GPUSettings.ShadingRateCaps & RendererAPI::ShadingRateCaps::PerTile) == RendererAPI::ShadingRateCaps::NotSupported)
		finalRate = ShadingRateCombiner::Passthrough;

	gpd.ShadingRate = shadingRate;
	gpd.ShadingRateCombiners = {postRasterizerRate, finalRate};

	p.GraphicCommandBuffers[p.ImageIndex]->SetShadingRate(shadingRate, postRasterizerRate, finalRate);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetShadingRate(TRAP::Ref<TRAP::Graphics::RenderTarget> texture,
                                                         const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetShadingRate(TRAP::Ref<TRAP::Graphics::RenderTarget> texture) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(RendererAPI::GPUSettings.ShadingRateCaps == RendererAPI::ShadingRateCaps::NotSupported) //VRS is not supported
	{
		TRAP_ASSERT(false, "VulkanRenderer::SetShadingRate(): Shading rate is not supported by this device!");
		return;
	}

	PerViewportData& p = *GETPERVIEWPORTDATA;

	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline);

	if(gpd.DepthState->DepthWrite)
	{
		TRAP_ASSERT(false, "VulkanRenderer::SetShadingRate(): Depth write is enabled, shading rate cannot be set");
		TP_ERROR(Log::RendererVulkanPrefix, "VulkanRenderer::SetShadingRate(): Depth write is enabled, shading rate cannot be set");
		return;
	}

	if((RendererAPI::GPUSettings.ShadingRateCaps & RendererAPI::ShadingRateCaps::PerTile) == RendererAPI::ShadingRateCaps::NotSupported)
		return;

	gpd.ShadingRate = ShadingRate::Full;
	gpd.ShadingRateCombiners = {ShadingRateCombiner::Passthrough, ShadingRateCombiner::Override};

	p.GraphicCommandBuffers[p.ImageIndex]->SetShadingRate(gpd.ShadingRate,
															std::get<0>(gpd.ShadingRateCombiners),
															std::get<1>(gpd.ShadingRateCombiners));

	p.NewShadingRateTexture = texture;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::Clear(const ClearBufferType clearType, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::Clear(const ClearBufferType clearType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	TRAP::Ref<RenderTarget> renderTarget;
	if(p.RenderScale != 1.0f || p.CurrentAntiAliasing == RendererAPI::AntiAliasing::MSAA)
		renderTarget = p.InternalRenderTargets[p.CurrentSwapChainImageIndex];
	else
		renderTarget = GetCurrentSwapchainRenderTarget(p);

	const CommandBuffer* const cmdBuffer = p.GraphicCommandBuffers[p.ImageIndex];

	if(std::to_underlying(clearType & ClearBufferType::Color) != 0u)
	{
		cmdBuffer->Clear(p.ClearColor, renderTarget->GetWidth(), renderTarget->GetHeight());
	}

	if(std::to_underlying(clearType & ClearBufferType::Depth_Stencil) != 0u &&
	   TRAP::Graphics::API::ImageFormatIsDepthAndStencil(renderTarget->GetImageFormat()))
	{
		cmdBuffer->Clear(p.ClearDepthStencil.Depth, p.ClearDepthStencil.Stencil,
		                 renderTarget->GetWidth(), renderTarget->GetHeight());
	}
	else if(std::to_underlying(clearType & ClearBufferType::Depth) != 0u &&
	        (TRAP::Graphics::API::ImageFormatIsDepthAndStencil(renderTarget->GetImageFormat()) ||
			 TRAP::Graphics::API::ImageFormatIsDepthOnly(renderTarget->GetImageFormat())))
	{
		cmdBuffer->Clear(p.ClearDepthStencil.Depth, renderTarget->GetWidth(), renderTarget->GetHeight());
	}
	else if(std::to_underlying(clearType & ClearBufferType::Stencil) != 0u &&
	        TRAP::Graphics::API::ImageFormatHasStencil(renderTarget->GetImageFormat()))
	{
		cmdBuffer->Clear(p.ClearDepthStencil.Stencil, renderTarget->GetWidth(), renderTarget->GetHeight());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetViewport(const u32 x, const u32 y, const u32 width,
                                                      const u32 height, const f32 minDepth,
                                                      const f32 maxDepth, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetViewport(const u32 x, const u32 y, const u32 width,
                                                      const u32 height, const f32 minDepth,
                                                      const f32 maxDepth) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if (width == 0 || height == 0)
		return;

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	p.GraphicCommandBuffers[p.ImageIndex]->SetViewport(NumericCast<f32>(x), NumericCast<f32>(y),
	                                                   NumericCast<f32>(width), NumericCast<f32>(height),
	                                                   minDepth, maxDepth);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetScissor(const u32 x, const u32 y, const u32 width,
                                                     const u32 height, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::SetScissor(const u32 x, const u32 y, const u32 width,
                                                     const u32 height) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	p.GraphicCommandBuffers[p.ImageIndex]->SetScissor(x, y, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::Draw(const u32 vertexCount, const u32 firstVertex,
                                               const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::Draw(const u32 vertexCount, const u32 firstVertex) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	p.GraphicCommandBuffers[p.ImageIndex]->Draw(vertexCount, firstVertex);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::DrawIndexed(const u32 indexCount, const u32 firstIndex,
                                                      const i32 firstVertex, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::DrawIndexed(const u32 indexCount, const u32 firstIndex,
                                                      const i32 firstVertex) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	p.GraphicCommandBuffers[p.ImageIndex]->DrawIndexed(indexCount, firstIndex, firstVertex);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::DrawInstanced(const u32 vertexCount, const u32 instanceCount,
                                                        const u32 firstVertex, const u32 firstInstance,
                                                        const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::DrawInstanced(const u32 vertexCount, const u32 instanceCount,
                                                        const u32 firstVertex, const u32 firstInstance) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	p.GraphicCommandBuffers[p.ImageIndex]->DrawInstanced(vertexCount, firstVertex, instanceCount, firstInstance);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::DrawIndexedInstanced(const u32 indexCount, const u32 instanceCount,
                                                               const u32 firstIndex, const u32 firstInstance,
						                                       const i32 firstVertex, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::DrawIndexedInstanced(const u32 indexCount, const u32 instanceCount,
                                                               const u32 firstIndex, const u32 firstInstance,
						                                       const i32 firstVertex) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	p.GraphicCommandBuffers[p.ImageIndex]->DrawIndexedInstanced(indexCount, firstIndex, instanceCount,
	                                                            firstInstance, firstVertex);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::BindShader(Shader& shader, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::BindShader(Shader& shader) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	std::reference_wrapper<Shader> actualShader = shader;
	const ShaderType type = actualShader.get().GetShaderType();

	PerViewportData& p = *GETPERVIEWPORTDATA;
	CommandBuffer* cmdBuffer = nullptr;

	switch(type)
	{
	case ShaderType::Graphics:
	{
		GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline);

		if(!actualShader.get().IsShaderValid())
		{
			//Overwrite invalid shader with fallback
			actualShader = *TRAP::Graphics::ShaderManager::Get(RendererAPI::ShaderType::Graphics, "FallbackGraphics").get();

			gpd.ShaderProgram = &actualShader.get();
			gpd.RootSignature = actualShader.get().GetRootSignature();
		}
		else if(actualShader.get().IsShaderValid() && gpd.ShaderProgram != &actualShader.get())
		{
			gpd.ShaderProgram = &actualShader.get();
			gpd.RootSignature = actualShader.get().GetRootSignature();
		}

		p.CurrentGraphicsPipeline = GetPipeline(p.GraphicsPipelineDesc);

		cmdBuffer = p.GraphicCommandBuffers[p.ImageIndex];
		cmdBuffer->BindPipeline(*(p.CurrentGraphicsPipeline));
		break;
	}

	case ShaderType::Compute:
	{
		ComputePipelineDesc& cpd = std::get<ComputePipelineDesc>(p.ComputePipelineDesc.Pipeline);

		if(!actualShader.get().IsShaderValid())
		{
			//Overwrite invalid shader with fallback
			actualShader = *TRAP::Graphics::ShaderManager::Get(RendererAPI::ShaderType::Compute,"FallbackCompute").get();

			cpd.ShaderProgram = &actualShader.get();
			cpd.RootSignature = actualShader.get().GetRootSignature();
		}
		else if(actualShader.get().IsShaderValid() && cpd.ShaderProgram != &actualShader.get())
		{
			cpd.ShaderProgram = &actualShader.get();
			cpd.RootSignature = actualShader.get().GetRootSignature();
		}

		p.CurrentComputeWorkGroupSize.x() = std::get<0>(actualShader.get().GetNumThreadsPerGroup());
		p.CurrentComputeWorkGroupSize.y() = std::get<1>(actualShader.get().GetNumThreadsPerGroup());
		p.CurrentComputeWorkGroupSize.z() = std::get<2>(actualShader.get().GetNumThreadsPerGroup());

		p.CurrentComputePipeline = GetPipeline(p.ComputePipelineDesc);

		cmdBuffer = p.ComputeCommandBuffers[p.ImageIndex];
		cmdBuffer->BindPipeline(*(p.CurrentComputePipeline));
		break;
	}

	//TODO RayTracing
	}

	if(cmdBuffer != nullptr)
	{
		//Bind Descriptors
		for(u32 i = 0; i < RendererAPI::MaxDescriptorSets; ++i)
		{
			if(actualShader.get().GetDescriptorSets()[i])
			{
				cmdBuffer->BindDescriptorSet(i == 0 ? 0 : p.ImageIndex, *(actualShader.get().GetDescriptorSets()[i]));
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::BindVertexBuffer(const Buffer& vBuffer,
                                                           const VertexBufferLayout& layout, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::BindVertexBuffer(const Buffer& vBuffer,
                                                           const VertexBufferLayout& layout) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	PerViewportData& p = *GETPERVIEWPORTDATA;

	p.GraphicCommandBuffers[p.ImageIndex]->BindVertexBuffer({ vBuffer }, { layout.GetStride() }, {});

	const TRAP::Ref<VertexLayout> lay = TRAP::MakeRef<VertexLayout>();
	const std::vector<VertexBufferElement>& elements = layout.GetElements();
	lay->AttributeCount = NumericCast<u32>(elements.size());
	for(usize i = 0; i < elements.size(); ++i)
	{
		lay->Attributes[i] = VertexAttribute
		{
			.Format = ShaderDataTypeToImageFormat(elements[i].Type),
			.Binding = 0,
			.Location = NumericCast<u32>(i),
			.Offset = elements[i].Offset,
			.Rate = VertexAttributeRate::Vertex
		};
	}
	std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline).VertexLayout = lay;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::BindIndexBuffer(const Buffer& iBuffer,
                                                          const IndexType indexType, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::BindIndexBuffer(const Buffer& iBuffer,
                                                          const IndexType indexType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	p.GraphicCommandBuffers[p.ImageIndex]->BindIndexBuffer(iBuffer, indexType, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::BindDescriptorSet(DescriptorSet& dSet, const u32 index,
															const QueueType queueType, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::BindDescriptorSet(DescriptorSet& dSet, const u32 index,
															const QueueType queueType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "VulkanRenderer::BindDescriptorSet(): Invalid QueueType provided!");

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	if(queueType == QueueType::Graphics)
		p.GraphicCommandBuffers[p.ImageIndex]->BindDescriptorSet(index, dSet);
	else if(queueType == QueueType::Compute)
		p.ComputeCommandBuffers[p.ImageIndex]->BindDescriptorSet(index, dSet);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::BindPushConstants(const std::string_view name, const std::span<const u8> constants,
															const QueueType queueType, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::BindPushConstants(const std::string_view name, const std::span<const u8> constants,
															const QueueType queueType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "VulkanRenderer::BindPushConstants(): Invalid QueueType provided!");

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	if(queueType == QueueType::Graphics)
	{
		p.GraphicCommandBuffers[p.ImageIndex]->BindPushConstants
		(
			*std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline).RootSignature,
			name, constants
		);
	}
	else if(queueType == QueueType::Compute)
	{
		p.ComputeCommandBuffers[p.ImageIndex]->BindPushConstants
		(
			*std::get<ComputePipelineDesc>(p.ComputePipelineDesc.Pipeline).RootSignature,
			name, constants
		);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::BindPushConstantsByIndex(const u32 paramIndex,
                                                                   const std::span<const u8> constants,
																   const QueueType queueType,
																   const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::BindPushConstantsByIndex(const u32 paramIndex,
                                                                   const std::span<const u8> constants,
																   const QueueType queueType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "VulkanRenderer::BindPushConstantsByIndex(): Invalid QueueType provided!");

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	if(queueType == QueueType::Graphics)
	{
		p.GraphicCommandBuffers[p.ImageIndex]->BindPushConstantsByIndex
		(
			*std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline).RootSignature,
			paramIndex, constants
		);
	}
	else if(queueType == QueueType::Compute)
	{
		p.ComputeCommandBuffers[p.ImageIndex]->BindPushConstantsByIndex
		(
			*std::get<ComputePipelineDesc>(p.ComputePipelineDesc.Pipeline).RootSignature,
			paramIndex, constants
		);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::BindRenderTarget(const Graphics::RenderTarget* const colorTarget,
		                                                   const Graphics::RenderTarget* const depthStencil,
							                               RendererAPI::LoadActionsDesc* const loadActions,
							                               std::vector<u32>* const colorArraySlices,
							                               std::vector<u32>* const colorMipSlices,
							                               const u32 depthArraySlice,
														   const u32 depthMipSlice, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::BindRenderTarget(const Graphics::RenderTarget* const colorTarget,
		                                                   const Graphics::RenderTarget* const depthStencil,
							                               RendererAPI::LoadActionsDesc* const loadActions,
							                               std::vector<u32>* const colorArraySlices,
							                               std::vector<u32>* const colorMipSlices,
							                               const u32 depthArraySlice,
														   const u32 depthMipSlice) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	PerViewportData& p = *GETPERVIEWPORTDATA;

	//We may need to change the graphics pipeline
	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline);
	bool rebindShader = false;

	const TRAP::Ref<TRAP::Graphics::RenderTarget>& shadingRateTex = gpd.ShadingRateTexture;

	if(colorTarget != nullptr && (gpd.RenderTargetCount != 1 || gpd.ColorFormats.size() != 1 || gpd.ColorFormats[0] != colorTarget->GetImageFormat() ))
	{
		gpd.RenderTargetCount = 1;
		gpd.ColorFormats = {colorTarget->GetImageFormat()};

		rebindShader = true;
	}
	if(depthStencil != nullptr && gpd.DepthStencilFormat != depthStencil->GetImageFormat())
	{
		gpd.DepthStencilFormat = depthStencil->GetImageFormat();
		rebindShader = true;
	}

	if(rebindShader && gpd.ShaderProgram != nullptr)
	{
#ifndef TRAP_HEADLESS_MODE
		BindShader(*gpd.ShaderProgram, window);
#else
		BindShader(*gpd.ShaderProgram);
#endif /*TRAP_HEADLESS_MODE*/
	}

	std::vector<std::reference_wrapper<const Graphics::RenderTarget>> targets;
	if(colorTarget != nullptr)
		targets.emplace_back(*colorTarget);
	p.GraphicCommandBuffers[p.ImageIndex]->BindRenderTargets
	(
		targets, depthStencil, loadActions, colorArraySlices, colorMipSlices, depthArraySlice, depthMipSlice, shadingRateTex.get()
	);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::BindRenderTargets(const std::vector<std::reference_wrapper<const Graphics::RenderTarget>>& colorTargets,
		                                                    const Graphics::RenderTarget* const depthStencil,
							                                RendererAPI::LoadActionsDesc* const loadActions,
							                                std::vector<u32>* const colorArraySlices,
							                                std::vector<u32>* const colorMipSlices,
							                                const u32 depthArraySlice,
														    const u32 depthMipSlice, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::BindRenderTargets(const std::vector<std::reference_wrapper<const Graphics::RenderTarget>>& colorTargets,
		                                                    const Graphics::RenderTarget* const depthStencil,
							                                RendererAPI::LoadActionsDesc* const loadActions,
							                                std::vector<u32>* const colorArraySlices,
							                                std::vector<u32>* const colorMipSlices,
							                                const u32 depthArraySlice,
														    const u32 depthMipSlice) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	PerViewportData& p = *GETPERVIEWPORTDATA;

	//We may need to change the graphics pipeline
	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p.GraphicsPipelineDesc.Pipeline);
	bool rebindShader = false;

	const TRAP::Ref<TRAP::Graphics::RenderTarget>& shadingRateTex = gpd.ShadingRateTexture;

	if(!colorTargets.empty())
	{
		if(gpd.RenderTargetCount != NumericCast<u32>(colorTargets.size()))
			rebindShader = true;

		gpd.RenderTargetCount = NumericCast<u32>(colorTargets.size());
		gpd.ColorFormats.resize(colorTargets.size());
		for(usize i = 0; i < colorTargets.size(); ++i)
		{
			if(gpd.ColorFormats[i] != colorTargets[i].get().GetImageFormat())
				rebindShader = true;

			gpd.ColorFormats[i] = colorTargets[i].get().GetImageFormat();
		}
	}

	if(depthStencil != nullptr && gpd.DepthStencilFormat != depthStencil->GetImageFormat())
	{
		gpd.DepthStencilFormat = depthStencil->GetImageFormat();
		rebindShader = true;
	}

	if(rebindShader && gpd.ShaderProgram != nullptr)
	{
#ifndef TRAP_HEADLESS_MODE
		BindShader(*gpd.ShaderProgram, window);
#else
		BindShader(*gpd.ShaderProgram);
#endif /*TRAP_HEADLESS_MODE*/
	}

	p.GraphicCommandBuffers[p.ImageIndex]->BindRenderTargets
	(
		colorTargets, depthStencil, loadActions, colorArraySlices, colorMipSlices, depthArraySlice, depthMipSlice, shadingRateTex.get()
	);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::ResourceBufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
															    const QueueType queueType, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::ResourceBufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
															    const QueueType queueType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "VulkanRenderer::ResourceBufferBarrier(): Invalid QueueType provided!");

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	if(queueType == QueueType::Graphics)
		p.GraphicCommandBuffers[p.ImageIndex]->ResourceBarrier(&bufferBarrier, nullptr, nullptr);
	else if(queueType == QueueType::Compute)
		p.ComputeCommandBuffers[p.ImageIndex]->ResourceBarrier(&bufferBarrier, nullptr, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::ResourceBufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
																 const QueueType queueType, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::ResourceBufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
																 const QueueType queueType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "VulkanRenderer::ResourceBufferBarriers(): Invalid QueueType provided!");

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	if(queueType == QueueType::Graphics)
		p.GraphicCommandBuffers[p.ImageIndex]->ResourceBarrier(&bufferBarriers, nullptr, nullptr);
	else if(queueType == QueueType::Compute)
		p.ComputeCommandBuffers[p.ImageIndex]->ResourceBarrier(&bufferBarriers, nullptr, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::ResourceTextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
																 const QueueType queueType, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::ResourceTextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
																 const QueueType queueType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "VulkanRenderer::ResourceTextureBarrier(): Invalid QueueType provided!");

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	if(queueType == QueueType::Graphics)
		p.GraphicCommandBuffers[p.ImageIndex]->ResourceBarrier(nullptr, &textureBarrier, nullptr);
	else if(queueType == QueueType::Compute)
		p.ComputeCommandBuffers[p.ImageIndex]->ResourceBarrier(nullptr, &textureBarrier, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::ResourceTextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
																  const QueueType queueType, const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::ResourceTextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
																  const QueueType queueType) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(queueType == QueueType::Graphics || queueType == QueueType::Compute, "VulkanRenderer::ResourceTextureBarriers(): Invalid QueueType provided!");

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	if(queueType == QueueType::Graphics)
		p.GraphicCommandBuffers[p.ImageIndex]->ResourceBarrier(nullptr, &textureBarriers, nullptr);
	else if(queueType == QueueType::Compute)
		p.ComputeCommandBuffers[p.ImageIndex]->ResourceBarrier(nullptr, &textureBarriers, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::ResourceRenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier,
									                                  const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::ResourceRenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	p.GraphicCommandBuffers[p.ImageIndex]->ResourceBarrier(nullptr, nullptr, &renderTargetBarrier);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::ResourceRenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers,
									                                   const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::ResourceRenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	p.GraphicCommandBuffers[p.ImageIndex]->ResourceBarrier(nullptr, nullptr, &renderTargetBarriers);
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::ReflexSleep() const
{
#ifdef NVIDIA_REFLEX_AVAILABLE
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(!GPUSettings.ReflexSupported)
		return;

	u64 signalValue = 0;
	VkCall(vkGetSemaphoreCounterValueKHR(m_device->GetVkDevice(), m_device->GetReflexSemaphore(), &signalValue));
	++signalValue;

	const VkSemaphoreWaitInfoKHR waitInfo = VulkanInits::SemaphoreWaitInfo(m_device->GetReflexSemaphore(), signalValue);
	VkReflexCall(NvLL_VK_Sleep(m_device->GetVkDevice(), signalValue));
	VkCall(vkWaitSemaphoresKHR(m_device->GetVkDevice(), &waitInfo, std::numeric_limits<u64>::max()));
#endif /*NVIDIA_REFLEX_AVAILABLE*/
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::ReflexMarker([[maybe_unused]] const u32 frame,
                                                       [[maybe_unused]] const u32 marker) const
{
#ifdef NVIDIA_REFLEX_AVAILABLE
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	PCLSTATS_MARKER(marker, frame);

	if(!GPUSettings.ReflexSupported)
		return;

	if(marker == PCLSTATS_PC_LATENCY_PING)
		return;

	if(marker == PCLSTATS_TRIGGER_FLASH) //BUG This gives ERROR_DEVICE_LOST
		return;

	NVLL_VK_LATENCY_MARKER_PARAMS params
	{
		.frameID = frame,
		.markerType = static_cast<NVLL_VK_LATENCY_MARKER_TYPE>(marker)
	};
	VkReflexCall(NvLL_VK_SetLatencyMarker(m_device->GetVkDevice(), &params));
#endif /*NVIDIA_REFLEX_AVAILABLE*/
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
[[nodiscard]] NVLL_VK_LATENCY_RESULT_PARAMS TRAP::Graphics::API::VulkanRenderer::ReflexGetLatency() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	NVLL_VK_LATENCY_RESULT_PARAMS params{};

	VkReflexCall(NvLL_VK_GetLatency(m_device->GetVkDevice(), &params));

	return params;
}
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] bool TRAP::Graphics::API::VulkanRenderer::GetVSync(const Window& window) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	return s_perViewportDataMap.at(&window)->CurrentVSync;
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Utils::UUID TRAP::Graphics::API::VulkanRenderer::GetCurrentGPUUUID() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_device->GetPhysicalDevice().GetUUID();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Graphics::API::VulkanRenderer::GetCurrentGPUName() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_device->GetPhysicalDevice().GetName();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::GPUVendor TRAP::Graphics::API::VulkanRenderer::GetCurrentGPUVendor() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_device->GetPhysicalDevice().GetVendor();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<std::pair<std::string, TRAP::Utils::UUID>> TRAP::Graphics::API::VulkanRenderer::GetAllGPUs() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(!s_usableGPUs.empty())
		return s_usableGPUs;

	for (const auto& [score, ratedDevice] : VulkanPhysicalDevice::GetAllRatedPhysicalDevices(*m_instance))
		s_usableGPUs.emplace_back(ratedDevice.PhysicalDeviceName, ratedDevice.PhysicalDeviceUUID);

	return s_usableGPUs;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Graphics::API::VulkanRenderer::CaptureScreenshot(const Window& window) const
#else
[[nodiscard]] TRAP::Scope<TRAP::Image> TRAP::Graphics::API::VulkanRenderer::CaptureScreenshot() const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const PerViewportData& p = *GETPERVIEWPORTDATA;

	const u32 lastFrame = (p.ImageIndex - 1) % RendererAPI::ImageCount;

	//Wait for queues to finish
	s_computeQueue->WaitQueueIdle();
	s_graphicQueue->WaitQueueIdle();

#ifdef TRAP_HEADLESS_MODE
	const TRAP::Ref<RenderTarget>& rT = p.RenderTargets[lastFrame];
#else
	const TRAP::Ref<RenderTarget>& rT = p.SwapChain->GetRenderTargets()[lastFrame];
#endif

	const u32 channelCount = ImageFormatChannelCount(rT->GetImageFormat());
	const bool hdr = ImageFormatIsFloat(rT->GetImageFormat());
	const bool isUnsigned16 = ImageFormatIsU16(rT->GetImageFormat());
	const bool flipRedBlue = rT->GetImageFormat() != ImageFormat::R8G8B8A8_UNORM;

#ifdef TRAP_HEADLESS_MODE
	static constexpr ResourceState resState = ResourceState::RenderTarget;
#else
	static constexpr ResourceState resState = ResourceState::Present;
#endif

	static constexpr auto FlipRedBlueChannels = []<typename T>(const u32 width, const u32 height, const u32 channels, std::vector<T>& pixels)
	requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
	{
		for(u32 y = 0; y < height; ++y)
		{
			for(u32 x = 0; x < width; ++x)
			{
				const u32 pixelIndex = (y * width + x) * channels;

				std::swap(pixels[pixelIndex], pixels[pixelIndex + 2]);
			}
		}
	};

	//Generate image data buffer

	if(hdr)
	{
		std::vector<f32> pixelDataf32 = MapRenderTarget<f32>(s_graphicQueue, *rT, resState);

		if(flipRedBlue) //Flip the BGRA to RGBA if necessary
			FlipRedBlueChannels(rT->GetWidth(), rT->GetHeight(), channelCount, pixelDataf32);

		return TRAP::Image::LoadFromMemory(rT->GetWidth(), rT->GetHeight(), static_cast<TRAP::Image::ColorFormat>(channelCount), pixelDataf32);
	}
	if(isUnsigned16)
	{
		std::vector<u16> pixelDatau16 = MapRenderTarget<u16>(s_graphicQueue, *rT, resState);

		if(flipRedBlue) //Flip the BGRA to RGBA if necessary
			FlipRedBlueChannels(rT->GetWidth(), rT->GetHeight(), channelCount, pixelDatau16);

		return TRAP::Image::LoadFromMemory(rT->GetWidth(), rT->GetHeight(), static_cast<TRAP::Image::ColorFormat>(channelCount), pixelDatau16);
	}

	std::vector<u8> pixelDatau8 = MapRenderTarget<u8>(s_graphicQueue, *rT, resState);

	if(flipRedBlue) //Flip the BGRA to RGBA if necessary
		FlipRedBlueChannels(rT->GetWidth(), rT->GetHeight(), channelCount, pixelDatau8);

	return TRAP::Image::LoadFromMemory(rT->GetWidth(), rT->GetHeight(), static_cast<TRAP::Image::ColorFormat>(channelCount), pixelDatau8);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::MSAAResolvePass(const RenderTarget& source,
                                                          const RenderTarget& destination,
														  CommandBuffer* const cmd) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(destination.GetSampleCount() == SampleCount::One, "VulkanRenderer::MSAAResolvePass(): destination must not use MSAA!");
	TRAP_ASSERT(cmd, "VulkanRenderer::MSAAResolvePass(): cmd is nullptr!");

	if(source.GetSampleCount() == SampleCount::One)
	{
		TRAP_ASSERT(false, "VulkanRenderer::MSAAResolvePass(): source does not use MSAA!");
		return;
	}

	const VulkanTexture* const dstTex = dynamic_cast<const VulkanTexture*>(destination.GetTexture().get());
	const VulkanTexture* const MSAATex = dynamic_cast<const VulkanTexture*>(source.GetTexture().get());

	//Stop running render pass
	cmd->BindRenderTargets({}, nullptr, nullptr, nullptr, nullptr,
						   std::numeric_limits<u32>::max(),
						   std::numeric_limits<u32>::max(), nullptr);

	//Transition source from RenderTarget to CopySource
	RenderTargetBarrier barrier = {source, ResourceState::RenderTarget, ResourceState::CopySource};
	cmd->ResourceBarrier(nullptr, nullptr, &barrier);
	//Transition destination from RenderTarget to CopyDestination
	barrier = {destination, ResourceState::RenderTarget, ResourceState::CopyDestination};
	cmd->ResourceBarrier(nullptr, nullptr, &barrier);

	const VulkanCommandBuffer* const vkCmdBuf = dynamic_cast<VulkanCommandBuffer*>(cmd);
	vkCmdBuf->ResolveImage(*MSAATex, ResourceState::CopySource, *dstTex, ResourceState::CopyDestination);

	//Transition destination from CopyDestination to RenderTarget
	barrier = {destination, ResourceState::CopyDestination, ResourceState::RenderTarget};
	cmd->ResourceBarrier(nullptr, nullptr, &barrier);
	//Transition source from CopySource to RenderTarget
	barrier = {source, ResourceState::CopySource, ResourceState::RenderTarget};
	cmd->ResourceBarrier(nullptr, nullptr, &barrier);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::UpdateInternalRenderTargets(PerViewportData& viewportData) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	bool rebuildColor = false;
	bool rebuildDepthStencil = false;
#ifndef TRAP_HEADLESS_MODE
	const auto newInternalRes = GetInternalRenderResolution(*viewportData.Window);
#else
	const auto newInternalRes = GetInternalRenderResolution();
#endif /*TRAP_HEADLESS_MODE*/
	if(!viewportData.InternalRenderTargets.empty() &&
	   viewportData.InternalRenderTargets[0] &&
	   (viewportData.InternalRenderTargets[0]->GetWidth() != newInternalRes.x() ||
	    viewportData.InternalRenderTargets[0]->GetHeight() != newInternalRes.y()))
	{
		rebuildColor = true;
	}

	if(viewportData.DepthStencilTarget != nullptr &&
	   (viewportData.DepthStencilTarget->GetWidth() != newInternalRes.x() ||
	    viewportData.DepthStencilTarget->GetHeight() != newInternalRes.y()))
	{
		rebuildDepthStencil = true;
	}

	if (viewportData.CurrentAntiAliasing != s_newAntiAliasing ||
	    viewportData.CurrentSampleCount != s_newSampleCount) //Change sample count only between frames!
	{
		viewportData.CurrentAntiAliasing = s_newAntiAliasing;
		viewportData.CurrentSampleCount = s_newSampleCount;

		if(viewportData.CurrentAntiAliasing == RendererAPI::AntiAliasing::MSAA)
			std::get<GraphicsPipelineDesc>(viewportData.GraphicsPipelineDesc.Pipeline).SampleCount = viewportData.CurrentSampleCount;
		else
			std::get<GraphicsPipelineDesc>(viewportData.GraphicsPipelineDesc.Pipeline).SampleCount = SampleCount::One;

		rebuildColor = true;
	}

	if(rebuildColor || rebuildDepthStencil)
		WaitIdle();

	if(rebuildColor)
	{
		viewportData.InternalRenderTargets.clear();

		RendererAPI::RenderTargetDesc rTDesc
		{
			.Width = newInternalRes.x(),
			.Height = newInternalRes.y(),
			.Depth = 1,
			.ArraySize = 1,
			.MipLevels = 1,
			.SampleCount = (viewportData.CurrentAntiAliasing == AntiAliasing::MSAA) ? viewportData.CurrentSampleCount : SampleCount::One,
			.Format = GetRecommendedSwapchainFormat(true, false),
			.StartState = RendererAPI::ResourceState::RenderTarget
		};

#ifndef TRAP_HEADLESS_MODE
		const u32 imageCount = NumericCast<u32>(viewportData.SwapChain->GetRenderTargets().size());
#else
		static constexpr u32 imageCount = RendererAPI::ImageCount;
#endif /*TRAP_HEADLESS_MODE*/
		viewportData.InternalRenderTargets.resize(imageCount);
		for(u32 i = 0; i < imageCount; ++i)
		{
			rTDesc.Name = fmt::format("Intermediate Color RenderTarget (Index: {})", i);
			viewportData.InternalRenderTargets[i] = RenderTarget::Create(rTDesc);
		}
	}

	if(rebuildDepthStencil)
	{
		const TRAP::Graphics::RendererAPI::RenderTargetDesc depthStencilRTDesc
		{
			.Flags{},
			.Width = newInternalRes.x(),
			.Height = newInternalRes.y(),
			.Depth = 1,
			.ArraySize = 1,
			.MipLevels = 1,
			.SampleCount = TRAP::Graphics::RendererAPI::SampleCount::One,
			.Format = std::get<GraphicsPipelineDesc>(viewportData.GraphicsPipelineDesc.Pipeline).DepthStencilFormat,
			.StartState = TRAP::Graphics::RendererAPI::ResourceState::DepthRead | TRAP::Graphics::RendererAPI::ResourceState::DepthWrite,
			.ClearValue = TRAP::Graphics::RendererAPI::DepthStencil{0.0f, 0},
			.SampleQuality = 0,
			.Descriptors = TRAP::Graphics::RendererAPI::DescriptorType::Texture,
			.Name = "Swapchain Depth/Stencil RenderTarget",
			.NativeHandle = nullptr
		};

		viewportData.DepthStencilTarget = RenderTarget::Create(depthStencilRTDesc);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::RenderScalePass(const RenderTarget& source,
                                                          const RenderTarget& destination,
		                                                  const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::RenderScalePass(const RenderTarget& source,
                                                          const RenderTarget& destination) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	PerViewportData& p = *GETPERVIEWPORTDATA;

	if(p.RenderScale == 1.0f)
	{
		TRAP_ASSERT(false, "VulkanRenderer::RenderScalePass(): RenderScale must not be 1.0f!");
		return;
	}

	VulkanCommandBuffer* const cmd = dynamic_cast<VulkanCommandBuffer*>(p.GraphicCommandBuffers[p.ImageIndex]);
	const RenderTarget* srcPtr = &source;

	//Stop running render pass
	p.GraphicCommandBuffers[p.ImageIndex]->BindRenderTargets({}, nullptr, nullptr, nullptr, nullptr,
															   std::numeric_limits<u32>::max(),
															   std::numeric_limits<u32>::max());

	if(srcPtr->GetSampleCount() != SampleCount::One) //Extra work to resolve MSAA
	{
		TRAP::Ref<RenderTarget>& tempTarget = p.TemporaryResolveRenderTargets[p.ImageIndex];

		bool updateTempTarget = !tempTarget;
		if(tempTarget &&
		   (tempTarget->GetWidth() != srcPtr->GetWidth() ||
	 	    tempTarget->GetHeight() != srcPtr->GetHeight()))
		{
			updateTempTarget = true;
		}

		if(updateTempTarget)
		{
			//Create temporary resolve render target
			const RenderTargetDesc rTDesc
			{
				.Width = srcPtr->GetWidth(),
				.Height = srcPtr->GetHeight(),
				.Depth = 1,
				.ArraySize = 1,
				.MipLevels = 1,
				.SampleCount = SampleCount::One,
				.Format = GetRecommendedSwapchainFormat(true, false),
				.StartState = RendererAPI::ResourceState::RenderTarget,
				.Name = "MSAA Resolve RenderTarget (RenderScalePass)"
			};
			tempTarget = RenderTarget::Create(rTDesc);

			p.TemporaryResolveRenderTargets[p.ImageIndex] = tempTarget;
		}

		//1. Do MSAAResolvePass() to resolve the multi sampled image into a single sampled image with same resolution
		MSAAResolvePass(*srcPtr, *tempTarget, cmd);

		//Overwrite source of RenderScalePass with the resolved RenderTarget
		srcPtr = tempTarget.get();
	}

	//Transition layout for copying
	TextureBarrier barrier
	{
		.Texture = *srcPtr->GetTexture(),
		.CurrentState = ResourceState::RenderTarget,
		.NewState = ResourceState::CopySource
	};
#ifndef TRAP_HEADLESS_MODE
	ResourceTextureBarrier(barrier, QueueType::Graphics, window);
#else
	ResourceTextureBarrier(barrier, QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/
	barrier.Texture = *destination.GetTexture();
	barrier.CurrentState = ResourceState::RenderTarget;
	barrier.NewState = ResourceState::CopyDestination;
#ifndef TRAP_HEADLESS_MODE
	ResourceTextureBarrier(barrier, QueueType::Graphics, window);
#else
	ResourceTextureBarrier(barrier, QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/

	//Do scaling
	TRAP::Ref<VulkanTexture> texInternal = std::dynamic_pointer_cast<VulkanTexture>(srcPtr->GetTexture());
	TRAP::Ref<VulkanTexture> texOutput = std::dynamic_pointer_cast<VulkanTexture>(destination.GetTexture());

	static constexpr VkImageSubresourceLayers subResLayers
	{
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.mipLevel = 0,
		.baseArrayLayer = 0,
		.layerCount = 1
	};

	const VkImageBlit region
	{
		.srcSubresource = subResLayers,
		.srcOffsets = {{0, 0, 0}, {NumericCast<i32>(texInternal->GetWidth()), NumericCast<i32>(texInternal->GetHeight()), 1}},
		.dstSubresource = subResLayers,
		.dstOffsets = {{0, 0, 0}, {NumericCast<i32>(texOutput->GetWidth()), NumericCast<i32>(texOutput->GetHeight()), 1}}
	};

	cmd->BlitImage(*texInternal, *texOutput, region, RendererAPI::FilterType::Linear);

	//Transition layout back
	barrier.Texture = *srcPtr->GetTexture();
	barrier.CurrentState = ResourceState::CopySource;
	barrier.NewState = ResourceState::RenderTarget;
#ifndef TRAP_HEADLESS_MODE
	ResourceTextureBarrier(barrier, QueueType::Graphics, window);
#else
	ResourceTextureBarrier(barrier, QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/
	barrier.Texture = *destination.GetTexture();
	barrier.CurrentState = ResourceState::CopyDestination;
	barrier.NewState = ResourceState::RenderTarget;
#ifndef TRAP_HEADLESS_MODE
	ResourceTextureBarrier(barrier, QueueType::Graphics, window);
#else
	ResourceTextureBarrier(barrier, QueueType::Graphics);
#endif /*TRAP_HEADLESS_MODE*/

	//Start render pass
	cmd->BindRenderTargets({destination}, nullptr, nullptr, nullptr, nullptr,
							std::numeric_limits<u32>::max(),
							std::numeric_limits<u32>::max());

	cmd->SetViewport(0.0f, 0.0f, NumericCast<f32>(destination.GetWidth()), NumericCast<f32>(destination.GetHeight()), 0.0f, 1.0f);
	cmd->SetScissor(0, 0, destination.GetWidth(), destination.GetHeight());
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::SetLatencyMode([[maybe_unused]] const LatencyMode mode,
                                                         [[maybe_unused]] const Window& window)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef NVIDIA_REFLEX_AVAILABLE
	if(!GPUSettings.ReflexSupported)
		return;

	PerViewportData* p = s_perViewportDataMap.at(&window).get();

	p->SleepModeParams.bLowLatencyMode = mode != LatencyMode::Disabled;
	p->SleepModeParams.bLowLatencyBoost = mode == LatencyMode::EnabledBoost;

	VkReflexCall(NvLL_VK_SetSleepMode(m_device->GetVkDevice(), &p->SleepModeParams));
#endif /*NVIDIA_REFLEX_AVAILABLE*/
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::Graphics::RendererAPI::LatencyMode TRAP::Graphics::API::VulkanRenderer::GetLatencyMode([[maybe_unused]] const Window& window) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef NVIDIA_REFLEX_AVAILABLE
	if(!GPUSettings.ReflexSupported)
		return LatencyMode::Disabled;

	PerViewportData* p = s_perViewportDataMap.at(&window).get();

	NVLL_VK_GET_SLEEP_STATUS_PARAMS params{};
	VkReflexCall(NvLL_VK_GetSleepStatus(m_device->GetVkDevice(), &params));

	if(!params.bLowLatencyMode)
	{
		p->SleepModeParams.bLowLatencyMode = false;
		p->SleepModeParams.bLowLatencyBoost = false;
	}

	if(p->SleepModeParams.bLowLatencyMode && p->SleepModeParams.bLowLatencyBoost)
		return LatencyMode::EnabledBoost;
	if(p->SleepModeParams.bLowLatencyMode)
		return LatencyMode::Enabled;
#endif /*NVIDIA_REFLEX_AVAILABLE*/

	return LatencyMode::Disabled;
}

#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::InitPerViewportData(Window& window, const bool VSyncEnabled) const
#else
void TRAP::Graphics::API::VulkanRenderer::InitPerViewportData(const u32 width, const u32 height) const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifndef TRAP_HEADLESS_MODE
	if (s_perViewportDataMap.contains(&window)) //Window is already in map
	{
		TRAP_ASSERT(false, "VulkanRenderer::InitPerViewportData(): Window is already in viewport map!");
		return;
	}
#endif /*TRAP_HEADLESS_MODE*/

	//Add new Window to map
	TRAP::Scope<PerViewportData> p = TRAP::MakeScope<PerViewportData>();

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
	if(Application::GetFPSLimit() == 0)
		p->SleepModeParams.minimumIntervalUs = 0;
	else
		p->SleepModeParams.minimumIntervalUs = NumericCast<u32>(((1000.0f / NumericCast<f32>(Application::GetFPSLimit())) * 1000.0f));
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

#ifndef TRAP_HEADLESS_MODE
	p->Window = &window;
#else
	p->NewWidth = width;
	p->NewHeight = height;
#endif /*TRAP_HEADLESS_MODE*/

	p->GraphicsFrameTime = 0.0f;
	p->ComputeFrameTime = 0.0f;

	RendererAPI::QueryPoolDesc queryPoolDesc{QueryType::Timestamp, 1 * 2};
	RendererAPI::BufferDesc bufferDesc
	{
		.Size = sizeof(u64) * 1 * 2,
		.MemoryUsage = RendererAPI::ResourceMemoryUsage::GPUToCPU,
		.Flags = RendererAPI::BufferCreationFlags::OwnMemory,
		.StartState = RendererAPI::ResourceState::CopyDestination,
		.ElementCount = 2,
		.StructStride = sizeof(u64)
	};

	//For each buffered image
	for (u32 i = 0; i < RendererAPI::ImageCount; ++i)
	{
		//Graphics
		//Create Graphic Command Pool
		const CommandPoolDesc cmdPoolDescGraphics
		{
			.Queue = s_graphicQueue,
			.CreateFlags = {},
#ifndef TRAP_HEADLESS_MODE
			.Name = fmt::format("PerViewportData CommandPool (Window: \"{}\", QueueType: \"Graphics\", Image: {})", window.GetTitle(), i)
#else
			.Name = fmt::format("PerViewportData CommandPool (QueueType: \"Graphics\", Image: {})", i)
#endif
		};
		p->GraphicCommandPools[i] = CommandPool::Create(cmdPoolDescGraphics);
		//Allocate Graphic Command Buffer
#ifndef TRAP_HEADLESS_MODE
		p->GraphicCommandBuffers[i] = &p->GraphicCommandPools[i]->GetCommandBuffer(false, fmt::format("PerViewportData CommandBuffer (Window: \"{}\", QueueType: \"Graphics\", Image: {})", window.GetTitle(), i));
#else
		p->GraphicCommandBuffers[i] = &p->GraphicCommandPools[i]->GetCommandBuffer(false, fmt::format("PerViewportData CommandBuffer (QueueType: \"Graphics\", Image: {})", i));
#endif

		//Create Render Fences/Semaphores
		p->RenderCompleteFences[i] = Fence::Create(false, fmt::format("PerViewportData Fence (RenderComplete, Image: {})", i));
		p->RenderCompleteSemaphores[i] = Semaphore::Create(fmt::format("PerViewportData Semaphore (RenderComplete, Image: {})", i));
		p->GraphicsCompleteSemaphores[i] = Semaphore::Create(fmt::format("PerViewportData Semaphore (GraphicsComplete, Image: {})", i));

		queryPoolDesc.Name = "GPU FrameTime Graphics QueryPool";
		p->GraphicsTimestampQueryPools[i] = QueryPool::Create(queryPoolDesc);
		bufferDesc.Name = "GPU FrameTime Graphics Query Readback Buffer";
		RendererAPI::BufferLoadDesc loadDesc{};
		loadDesc.Desc = bufferDesc;
		GetResourceLoader()->AddResource(loadDesc, nullptr);
		p->GraphicsTimestampReadbackBuffers[i] = loadDesc.Buffer;

		//Compute
		const CommandPoolDesc cmdPoolDescCompute
		{
			.Queue = s_computeQueue,
			.CreateFlags = {},
#ifndef TRAP_HEADLESS_MODE
			.Name = fmt::format("PerViewportData CommandPool (Window: \"{}\", QueueType: \"Compute\", Image: {})", window.GetTitle(), i)
#else
			.Name = fmt::format("PerViewportData CommandPool (QueueType: \"Compute\", Image: {})", i)
#endif
		};
		p->ComputeCommandPools[i] = CommandPool::Create(cmdPoolDescCompute);
#ifndef TRAP_HEADLESS_MODE
		p->ComputeCommandBuffers[i] = &p->ComputeCommandPools[i]->GetCommandBuffer(false, fmt::format("PerViewportData CommandBuffer (Window: \"{}\", QueueType: \"Compute\", Image: {})", window.GetTitle(), i));
#else
		p->ComputeCommandBuffers[i] = &p->ComputeCommandPools[i]->GetCommandBuffer(false, fmt::format("PerViewportData CommandBuffer (QueueType: \"Compute\", Image: {})", i));
#endif

		p->ComputeCompleteFences[i] = Fence::Create(false, fmt::format("PerViewportData Fence (ComputeComplete, Image: {})", i));
		p->ComputeCompleteSemaphores[i] = Semaphore::Create(fmt::format("PerViewportData Semaphore (ComputeComplete, Image: {})", i));

		queryPoolDesc.Name = "GPU FrameTime Compute QueryPool";
		p->ComputeTimestampQueryPools[i] = QueryPool::Create(queryPoolDesc);
		bufferDesc.Name = "GPU FrameTime Compute Query Readback Buffer";
		loadDesc = {};
		loadDesc.Desc = bufferDesc;
		GetResourceLoader()->AddResource(loadDesc, nullptr);
		p->ComputeTimestampReadbackBuffers[i] = loadDesc.Buffer;

#ifndef TRAP_HEADLESS_MODE
		//Image Acquire Semaphore
		p->ImageAcquiredSemaphores[i] = Semaphore::Create(fmt::format("PerViewportData Semaphore (ImageAcquired, Image: {})", i));
#endif /*TRAP_HEADLESS_MODE*/
	}

#ifndef TRAP_HEADLESS_MODE
	//Create Swapchain
	p->CurrentVSync = p->NewVSync = VSyncEnabled;
	SwapChainDesc swapChainDesc
	{
		.Window = &window,
		.PresentQueues = { s_graphicQueue },
		.ImageCount = RendererAPI::ImageCount,
		.Width = window.GetFrameBufferWidth(),
		.Height = window.GetFrameBufferHeight(),
		.ColorFormat = GetRecommendedSwapchainFormat(true, false),
		.ClearValue = p->ClearColor,
		.EnableVSync = p->CurrentVSync,
		.SampleCount = SampleCount::One,
		.OldSwapChain = p->SwapChain.get()
	};
	p->SwapChain = SwapChain::Create(swapChainDesc);

	if (!p->SwapChain)
		Utils::DisplayError(Utils::ErrorCode::VulkanSwapchainCreationFailed);

	//Create MSAA RenderTargets if needed
	if(p->CurrentAntiAliasing == AntiAliasing::MSAA)
	{
		RendererAPI::RenderTargetDesc rTMSAADesc
		{
			.Width = window.GetFrameBufferWidth(),
			.Height = window.GetFrameBufferHeight(),
			.Depth = 1,
			.ArraySize = 1,
			.MipLevels = 1,
			.SampleCount = p->CurrentSampleCount,
			.Format = GetRecommendedSwapchainFormat(true, false),
			.StartState = RendererAPI::ResourceState::RenderTarget
		};
		for(u32 i = 0; i < RendererAPI::ImageCount; ++i)
		{
			rTMSAADesc.Name = fmt::format("MSAA RenderTarget (Index: {})", i);
			p->InternalRenderTargets[i] = RenderTarget::Create(rTMSAADesc);
		}
	}
#else
	RendererAPI::RenderTargetDesc rTDesc
	{
		.Width = p->NewWidth,
		.Height = p->NewHeight,
		.Depth = 1,
		.ArraySize = 1,
		.MipLevels = 1,
		.SampleCount = SampleCount::One,
		.Format = GetRecommendedSwapchainFormat(true, false),
		.StartState = RendererAPI::ResourceState::RenderTarget
	};
	for(u32 i = 0; i < RendererAPI::ImageCount; ++i)
	{
		rTDesc.Name = fmt::format("Color RenderTarget (Index: {})", i);
		p->RenderTargets[i] = RenderTarget::Create(rTDesc);
	}
	if(p->CurrentAntiAliasing == AntiAliasing::MSAA)
	{
		rTDesc.SampleCount = p->CurrentSampleCount;
		for(u32 i = 0; i < RendererAPI::ImageCount; ++i)
		{
			rTDesc.Name = fmt::format("MSAA RenderTarget (Index: {})", i);
			p->InternalRenderTargets[i] = RenderTarget::Create(rTDesc);
		}
	}
#endif

	//Graphics Pipeline
#ifndef TRAP_HEADLESS_MODE
	const TRAP::Ref<RenderTarget>& rT = p->CurrentAntiAliasing == RendererAPI::AntiAliasing::MSAA ?
  	                                    p->InternalRenderTargets[0] :
								        p->SwapChain->GetRenderTargets()[0];
#else
	const auto& rT = p->CurrentAntiAliasing == RendererAPI::AntiAliasing::MSAA ? p->InternalRenderTargets[0] : std::get<0>(p->RenderTargets);
#endif
	p->GraphicsPipelineDesc = {};
	p->GraphicsPipelineDesc.Type = PipelineType::Graphics;
	p->GraphicsPipelineDesc.Pipeline = GraphicsPipelineDesc();
	GraphicsPipelineDesc& gpd = std::get<GraphicsPipelineDesc>(p->GraphicsPipelineDesc.Pipeline);

	gpd.PrimitiveTopology = PrimitiveTopology::TriangleList;
	gpd.RenderTargetCount = 1;
	gpd.ColorFormats = { rT->GetImageFormat() };
	gpd.SampleCount = rT->GetSampleCount();
	gpd.SampleQuality = rT->GetSampleQuality();
	gpd.ShadingRate = RendererAPI::ShadingRate::Full;
	gpd.ShadingRateCombiners = {ShadingRateCombiner::Passthrough, ShadingRateCombiner::Passthrough};

	gpd.DepthState = TRAP::MakeRef<DepthStateDesc>();
	gpd.DepthState->DepthTest = false;
	gpd.DepthState->DepthWrite = false;
	gpd.DepthState->DepthFunc = CompareMode::GreaterOrEqual; //Using GreaterOrEqual instead because of reversed Z depth range
	gpd.DepthState->StencilTest = false;
	gpd.DepthState->StencilReadMask = 0xFF;
	gpd.DepthState->StencilWriteMask = 0xFF;
	gpd.DepthState->StencilFrontFunc = CompareMode::Always;
	gpd.DepthState->StencilFrontFail = {};
	gpd.DepthState->DepthFrontFail = {};
	gpd.DepthState->StencilFrontPass = {};
	gpd.DepthState->StencilBackFunc = CompareMode::Always;
	gpd.DepthState->StencilBackFail = {};
	gpd.DepthState->DepthBackFail = {};
	gpd.DepthState->StencilBackPass = {};

	gpd.RasterizerState = TRAP::MakeRef<RasterizerStateDesc>();
	gpd.RasterizerState->CullMode = CullMode::Back;
	gpd.RasterizerState->DepthBias = 0;
	gpd.RasterizerState->SlopeScaledDepthBias = 0.0f;
	gpd.RasterizerState->FillMode = FillMode::Solid;
	gpd.RasterizerState->FrontFace = FrontFace::CounterClockwise;
	gpd.RasterizerState->DepthClampEnable = false;

	gpd.BlendState = TRAP::MakeRef<BlendStateDesc>();
	gpd.BlendState->SrcFactors = {BlendConstant::One };
	gpd.BlendState->DstFactors = {BlendConstant::Zero };
	gpd.BlendState->SrcAlphaFactors = { BlendConstant::One };
	gpd.BlendState->DstAlphaFactors = { BlendConstant::Zero};
	gpd.BlendState->BlendModes = {};
	gpd.BlendState->BlendAlphaModes = {};
	gpd.BlendState->Masks = {(BIT(0u) | BIT(1u) | BIT(2u) | BIT(3u))};
	gpd.BlendState->RenderTargetMask = BlendStateTargets::BlendStateTargetAll;
	gpd.BlendState->IndependentBlend = false;

	gpd.VertexLayout = TRAP::MakeRef<VertexLayout>();
	const TRAP::Ref<VertexLayout>& vLayout = gpd.VertexLayout;
	vLayout->AttributeCount = 1;
	std::get<0>(vLayout->Attributes).Format = ImageFormat::R32G32B32_SFLOAT;
	std::get<0>(vLayout->Attributes).Binding = 0;
	std::get<0>(vLayout->Attributes).Location = 0;
	std::get<0>(vLayout->Attributes).Offset = 0;

	//Compute Pipeline
	p->ComputePipelineDesc = {};
	p->ComputePipelineDesc.Type = PipelineType::Compute;
	p->ComputePipelineDesc.Pipeline = ComputePipelineDesc();
	p->CurrentComputeWorkGroupSize = {1u, 1u, 1u};

#ifndef TRAP_HEADLESS_MODE
	s_perViewportDataMap[&window] = std::move(p);

	StartComputeRecording(*s_perViewportDataMap.at(&window));
	StartGraphicRecording(*s_perViewportDataMap.at(&window));
#else
	s_perViewportData = std::move(p);

	StartComputeRecording(*s_perViewportData);
	StartGraphicRecording(*s_perViewportData);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Graphics::API::VulkanRenderer::RemovePerViewportData(const Window& window) const
#else
void TRAP::Graphics::API::VulkanRenderer::RemovePerViewportData() const
#endif /*TRAP_HEADLESS_MODE*/
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifndef TRAP_HEADLESS_MODE
	s_perViewportDataMap.erase(&window);
#else
	s_perViewportData.reset();
#endif /*TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::WaitIdle() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	m_device->WaitIdle();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::AddDefaultResources()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPrefix, "Creating DefaultResources");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	s_NullDescriptors = TRAP::MakeScope<NullDescriptors>();

	TRAP::Ref<TRAP::Graphics::API::VulkanTexture> vkTex = nullptr;

	//1D Texture
	TextureDesc textureDesc
	{
		.Width = 1,
		.Height = 1,
		.Depth = 1,
		.ArraySize = 1,
		.MipLevels = 1,
		.SampleCount = SampleCount::One,
		.Format = ImageFormat::R8G8B8A8_UNORM,
		.StartState = ResourceState::Common,
		.Descriptors = DescriptorType::Texture,
	};
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(textureDesc.Name);
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim1D)] = std::move(vkTex);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(textureDesc.Name);
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureUAV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim1D)] = std::move(vkTex);

	//1D Texture Array
	textureDesc.ArraySize = 2;
	textureDesc.Descriptors = DescriptorType::Texture;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(textureDesc.Name);
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim1DArray)] = std::move(vkTex);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(textureDesc.Name);
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureUAV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim1DArray)] = std::move(vkTex);

	//2D Texture
	textureDesc.Width = 2;
	textureDesc.Height = 2;
	textureDesc.ArraySize = 1;
	textureDesc.Descriptors = DescriptorType::Texture;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(textureDesc.Name);
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim2D)] = std::move(vkTex);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(textureDesc.Name);
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureUAV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim2D)] = std::move(vkTex);

	//2D MS Texture
	textureDesc.Descriptors = DescriptorType::Texture;
	textureDesc.SampleCount = SampleCount::Four;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(textureDesc.Name);
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim2DMS)] = std::move(vkTex);
	textureDesc.SampleCount = SampleCount::One;

	//2D Texture Array
	textureDesc.ArraySize = 2;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(textureDesc.Name);
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim2DArray)] = std::move(vkTex);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(textureDesc.Name);
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureUAV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim2DArray)] = std::move(vkTex);

	//2D MS Texture Array
	textureDesc.Descriptors = DescriptorType::Texture;
	textureDesc.SampleCount = SampleCount::Four;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(textureDesc.Name);
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim2DMSArray)] = std::move(vkTex);
	textureDesc.SampleCount = SampleCount::One;

	//3D Texture
	textureDesc.Depth = 2;
	textureDesc.ArraySize = 1;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(textureDesc.Name);
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim3D)] = std::move(vkTex);
	textureDesc.Descriptors = DescriptorType::RWTexture;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(textureDesc.Name);
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureUAV[std::to_underlying(ShaderReflection::TextureDimension::TextureDim3D)] = std::move(vkTex);

	//Cube Texture
	textureDesc.Depth = 1;
	textureDesc.ArraySize = 6;
	textureDesc.Descriptors = DescriptorType::TextureCube;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(textureDesc.Name);
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDimCube)] = std::move(vkTex);
	textureDesc.ArraySize = 6 * 2;
	vkTex = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(textureDesc.Name);
	vkTex->Init(textureDesc);
	s_NullDescriptors->DefaultTextureSRV[std::to_underlying(ShaderReflection::TextureDimension::TextureDimCubeArray)] = std::move(vkTex);

	BufferDesc bufferDesc
	{
		.Size = sizeof(u32),
		.MemoryUsage = ResourceMemoryUsage::GPUOnly,
		.StartState = ResourceState::Common,
		.ElementCount = 1,
		.StructStride = sizeof(u32),
		.Format = ImageFormat::R32_UINT,
		.Descriptors = DescriptorType::Buffer | DescriptorType::UniformBuffer
	};
	s_NullDescriptors->DefaultBufferSRV = TRAP::MakeRef<VulkanBuffer>(bufferDesc);
	bufferDesc.Descriptors = DescriptorType::RWBuffer;
	s_NullDescriptors->DefaultBufferUAV = TRAP::MakeRef<VulkanBuffer>(bufferDesc);

	static const SamplerDesc samplerDesc
	{
		.AddressU = AddressMode::ClampToBorder,
		.AddressV = AddressMode::ClampToBorder,
		.AddressW = AddressMode::ClampToBorder,
		.EnableAnisotropy = false,
		.Name = "Default Sampler"
	};
	s_NullDescriptors->DefaultSampler = TRAP::MakeRef<VulkanSampler>(samplerDesc);

	//Create Command Buffer to transition resources to the correct state
	const QueueDesc queueDesc
	{
		.Type = QueueType::Graphics,
		.Name = "Initial Transition Queue"
	};
	const TRAP::Ref<VulkanQueue> graphicsQueue = TRAP::MakeRef<VulkanQueue>(queueDesc);

	const CommandPoolDesc cmdPoolDesc
	{
		.Queue = graphicsQueue,
		.CreateFlags = CommandPoolCreateFlags::Transient,
		.Name = fmt::format("Initial Transition CommandPool (Transient, QueueType: \"Graphics\")")
	};
	const TRAP::Ref<VulkanCommandPool> cmdPool = TRAP::MakeRef<VulkanCommandPool>(cmdPoolDesc);

	CommandBuffer& cmd = cmdPool->GetCommandBuffer(false, "Initial Transition CommandBuffer");

	const TRAP::Ref<VulkanFence> fence = TRAP::MakeRef<VulkanFence>(false, "Initial Transition Fence");

	auto& NullDescriptorsObjs = s_NullDescriptors->SafeNullDescriptorsObjs.GetUnsafe();
	NullDescriptorsObjs.InitialTransitionQueue = graphicsQueue;
	NullDescriptorsObjs.InitialTransitionCmdPool = cmdPool;
	NullDescriptorsObjs.InitialTransitionCmd = dynamic_cast<VulkanCommandBuffer*>(&cmd);
	NullDescriptorsObjs.InitialTransitionFence = fence;

	//Transition resources
	for (u32 dim = 0; dim < std::to_underlying(ShaderReflection::TextureDimension::TextureDimCount); ++dim)
	{
		if (s_NullDescriptors->DefaultTextureSRV[dim])
			UtilInitialTransition(*s_NullDescriptors->DefaultTextureSRV[dim], ResourceState::ShaderResource);

		if (s_NullDescriptors->DefaultTextureUAV[dim])
			UtilInitialTransition(*s_NullDescriptors->DefaultTextureUAV[dim], ResourceState::UnorderedAccess);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::RemoveDefaultResources()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	if(!s_NullDescriptors)
		return;

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanPrefix, "Destroying DefaultResources");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	for(u32 dim = 0; dim < std::to_underlying(ShaderReflection::TextureDimension::TextureDimCount); ++dim)
	{
		if (s_NullDescriptors->DefaultTextureSRV[dim])
			s_NullDescriptors->DefaultTextureSRV[dim].reset();

		if (s_NullDescriptors->DefaultTextureUAV[dim])
			s_NullDescriptors->DefaultTextureUAV[dim].reset();
	}

	s_NullDescriptors->DefaultBufferSRV.reset();
	s_NullDescriptors->DefaultBufferUAV.reset();

	s_NullDescriptors->DefaultSampler.reset();

	auto& nullDescriptors = s_NullDescriptors->SafeNullDescriptorsObjs.GetUnsafe();
	nullDescriptors.InitialTransitionFence.reset();
	nullDescriptors.InitialTransitionCmdPool.reset();
	nullDescriptors.InitialTransitionQueue.reset();

	s_NullDescriptors.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::UtilInitialTransition(const TRAP::Graphics::Texture& texture,
                                                                const RendererAPI::ResourceState startState)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const auto& nullDescriptorData = s_NullDescriptors->SafeNullDescriptorsObjs.WriteLock();

	VulkanCommandBuffer* const cmd = nullDescriptorData->InitialTransitionCmd;
	nullDescriptorData->InitialTransitionCmdPool->Reset();
	cmd->Begin();
	const TextureBarrier barrier{texture, RendererAPI::ResourceState::Undefined, startState};
	cmd->ResourceBarrier(nullptr, &barrier, nullptr);
	cmd->End();
	const RendererAPI::QueueSubmitDesc submitDesc
	{
		.Cmds = {*cmd},
		.SignalFence = nullDescriptorData->InitialTransitionFence
	};
	nullDescriptorData->InitialTransitionQueue->Submit(submitDesc);
	nullDescriptorData->InitialTransitionFence->Wait();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::VulkanRenderer::RenderPassMap& TRAP::Graphics::API::VulkanRenderer::GetRenderPassMap()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	{
		const auto renderPassData = SafeRenderPassData.ReadLock();
		auto& renderPassMap = SafeRenderPassData.GetUnsafe().ThreadRenderPassMap;
		const auto it = renderPassMap.find(std::this_thread::get_id());
		if(it != renderPassMap.end())
			return it->second;
	}

	//Only need a lock when creating a new RenderPass Map for this thread
	auto renderPassData = SafeRenderPassData.WriteLock();

	return renderPassData->ThreadRenderPassMap[std::this_thread::get_id()];
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::VulkanRenderer::FrameBufferMap& TRAP::Graphics::API::VulkanRenderer::GetFrameBufferMap()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	{
		const auto renderPassData = SafeRenderPassData.ReadLock();
		auto& frameBufferMap = SafeRenderPassData.GetUnsafe().ThreadFrameBufferMap;
		const auto it = frameBufferMap.find(std::this_thread::get_id());
		if(it != frameBufferMap.end())
			return it->second;
	}

	//Only need a lock when creating a new FrameBuffer Map for this thread
	auto renderPassData = SafeRenderPassData.WriteLock();

	return renderPassData->ThreadFrameBufferMap[std::this_thread::get_id()];
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::API::VulkanInstance> TRAP::Graphics::API::VulkanRenderer::GetInstance() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_instance;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::API::VulkanDevice> TRAP::Graphics::API::VulkanRenderer::GetDevice() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_device;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::API::VulkanMemoryAllocator> TRAP::Graphics::API::VulkanRenderer::GetVMA() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_vma;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Pipeline> TRAP::Graphics::API::VulkanRenderer::GetPipeline(PipelineDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const usize hash = std::hash<PipelineDesc>{}(desc);
	const auto pipelineIt = Pipelines.find(hash);

	if(pipelineIt != Pipelines.end())
		return pipelineIt->second;

	const auto tempFolder = TRAP::FileSystem::GetGameTempFolderPath();
	if(tempFolder)
	{
		std::pair<std::unordered_map<u64, TRAP::Ref<PipelineCache>>::iterator, bool> res;
		if (!PipelineCaches.contains(hash))
		{
			const PipelineCacheLoadDesc cacheDesc
			{
				.Path = *tempFolder / fmt::format("{}.cache", hash),
				.Flags = PipelineCacheFlags::None,
				.Name = fmt::format("Pipeline cache (Pipeline: \"{}\")", desc.Name)
			};
			res = PipelineCaches.try_emplace(hash, PipelineCache::Create(cacheDesc));
		}

		if(res.second) //Got inserted
			desc.Cache = res.first->second;
	}

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_TRACE(Log::RendererVulkanPipelinePrefix, "Recreating Graphics Pipeline...");
#endif /*VERBOSE_GRAPHICS_DEBUG*/
	const TRAP::Ref<TRAP::Graphics::Pipeline> pipeline = Pipeline::Create(desc);
	Pipelines[hash] = pipeline;
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_TRACE(Log::RendererVulkanPipelinePrefix, "Cached Graphics Pipeline");
#endif /*VERBOSE_GRAPHICS_DEBUG*/
	return pipeline;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::BeginGPUFrameProfile(const QueueType type, const PerViewportData& p)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const CommandBuffer* cmd = nullptr;
	TRAP::Ref<QueryPool> pool = nullptr;

	switch(type)
	{
	case QueueType::Graphics:
		cmd = p.GraphicCommandBuffers[p.ImageIndex];
		pool = p.GraphicsTimestampQueryPools[p.ImageIndex];
		break;
	case QueueType::Transfer:
		TRAP_ASSERT(false, "VulkanRenderer::BeginGPUFrameProfile(): QueueType must not be QueueType::Transfer!");
		return;
	case QueueType::Compute:
		cmd = p.ComputeCommandBuffers[p.ImageIndex];
		pool = p.ComputeTimestampQueryPools[p.ImageIndex];
		break;
	}

	cmd->ResetQueryPool(*pool, 0, 1 * 2);
	cmd->BeginQuery(*pool, {0});
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderer::EndGPUFrameProfile(const QueueType type, const PerViewportData& p)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(type != QueueType::Transfer, "VulkanRenderer::EndGPUFrameProfile(): QueueType must not be QueueType::Transfer!");

	const CommandBuffer* cmd = nullptr;
	TRAP::Ref<QueryPool> pool = nullptr;
	TRAP::Ref<Buffer> buffer = nullptr;

	switch(type)
	{
	case QueueType::Graphics:
		cmd = p.GraphicCommandBuffers[p.ImageIndex];
		pool = p.GraphicsTimestampQueryPools[p.ImageIndex];
		buffer = p.GraphicsTimestampReadbackBuffers[p.ImageIndex];
		break;
	case QueueType::Transfer:
		TRAP_ASSERT(false, "VulkanRenderer::EndGPUFrameProfile(): QueueType must not be QueueType::Transfer!");
		return;
	case QueueType::Compute:
		cmd = p.ComputeCommandBuffers[p.ImageIndex];
		pool = p.ComputeTimestampQueryPools[p.ImageIndex];
		buffer = p.ComputeTimestampReadbackBuffers[p.ImageIndex];
		break;
	}

	cmd->BeginQuery(*pool, {1});
	cmd->ResolveQuery(*pool, *buffer, 0, 1 * 2);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] f32 TRAP::Graphics::API::VulkanRenderer::ResolveGPUFrameProfile(const QueueType type, const PerViewportData& p)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(type != QueueType::Transfer, "VulkanRenderer::ResolveGPUFrameProfile(): QueueType must not be QueueType::Transfer!");

	f32 time = 0.0f;

	static constexpr RendererAPI::ReadRange readRange{0, sizeof(u64) * 1 * 2};

	TRAP::Ref<Buffer> buffer = nullptr;

	switch(type)
	{
	case QueueType::Graphics:
		buffer = p.GraphicsTimestampReadbackBuffers[p.ImageIndex];
		break;
	case QueueType::Transfer:
		TRAP_ASSERT(false, "VulkanRenderer::ResolveGPUFrameProfile(): QueueType must not be QueueType::Transfer!");
		return 0.0f;
	case QueueType::Compute:
		buffer = p.ComputeTimestampReadbackBuffers[p.ImageIndex];
		break;
	}

	if(buffer->MapBuffer(readRange))
	{
		const std::span<u64> bufferData = buffer->GetCPUMappedAddress<u64>();
		TRAP_ASSERT(bufferData.size() == 2, "VulkanRenderer::ResolveGPUFrameProfile(): Readback buffer size mismatch!");

		const u64 startTime = bufferData[0];
		const u64 endTime = bufferData[1];

		if(endTime > startTime)
		{
			const f64 nsTime = NumericCast<f64>(endTime - startTime);
			if(type == QueueType::Graphics)
			{
				const Ref<VulkanQueue> graphicsQueue = std::dynamic_pointer_cast<VulkanQueue>(s_graphicQueue);
				time = NumericCast<f32>(nsTime / graphicsQueue->GetTimestampFrequency()) * 1000.0f;
			}
			else if(type == QueueType::Compute)
			{
				const Ref<VulkanQueue> computeQueue = std::dynamic_pointer_cast<VulkanQueue>(s_computeQueue);
				time = NumericCast<f32>(nsTime / computeQueue->GetTimestampFrequency()) * 1000.0f;
			}
		}

		buffer->UnMapBuffer();
	}

	return time;
}

#ifdef GETPERVIEWPORTDATA
	#undef GETPERVIEWPORTDATA
#endif /*GETPERVIEWPORTDATA*/

#ifdef HEADLESS_CONSTEXPR
	#undef HEADLESS_CONSTEXPR
#endif /*HEADLESS_CONSTEXPR*/
