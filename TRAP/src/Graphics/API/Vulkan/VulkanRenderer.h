#ifndef _TRAP_VULKANRENDERER_H_
#define _TRAP_VULKANRENDERER_H_

#include "Graphics/API/RendererAPI.h"
#include "Maths/Math.h"
#include "Window/WindowingAPI.h"
#include "Graphics/API/ShaderReflection.h"

namespace TRAP::Graphics::API
{
	class VulkanRenderTarget;
	class VulkanBuffer;
	class VulkanTexture;
	class VulkanMemoryAllocator;
	class VulkanFrameBuffer;
	class VulkanRenderPass;
	class VulkanDescriptorPool;
	class VulkanPhysicalDevice;
	class VulkanDevice;
	class VulkanInstance;
	class VulkanDebug;

	class VulkanRenderer final : public RendererAPI
	{
	public:
		explicit VulkanRenderer();
		~VulkanRenderer();
		
		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;
		VulkanRenderer(VulkanRenderer&&) = delete;
		VulkanRenderer& operator=(VulkanRenderer&&) = delete;

		void InitInternal() override;

		void Clear(RendererBufferType buffer) override;
		void Present(const Scope<Window>& window) override;

		void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f }) override;
		void SetDepthTesting(bool enabled) override;
		void SetDepthMasking(bool enabled) override;
		void SetDepthFunction(RendererFunction function) override;
		void SetStencilTesting(bool enabled) override;
		void SetStencilMasking(uint32_t mask) override;
		void SetStencilMaskingSeparate(RendererFaceMode face, uint32_t mask) override;
		void SetStencilFunction(RendererFunction function, int32_t reference, uint32_t mask) override;
		void SetStencilFunctionSeparate(RendererFaceMode face, RendererFunction function, int32_t reference, uint32_t mask) override;
		void SetStencilOperation(RendererOperation stencilFail, RendererOperation depthFail, RendererOperation pass) override;
		void SetStencilOperationSeparate(RendererFaceMode face, RendererOperation stencilFail, RendererOperation depthFail, RendererOperation pass) override;
		void SetBlend(bool enabled) override;
		void SetCull(bool enabled) override;
		void SetFrontFace(RendererFrontFace frontFace) override;
		void SetWireFrame(bool enabled) override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		void SetBlendFunction(RendererBlendFunction source, RendererBlendFunction destination) override;
		void SetBlendFunctionSeparate(RendererBlendFunction sourceRGB,
		                              RendererBlendFunction sourceAlpha,
		                              RendererBlendFunction destinationRGB,
		                              RendererBlendFunction destinationAlpha) override;
		void SetBlendEquation(RendererBlendEquation blendEquation) override;
		void SetBlendEquationSeparate(RendererBlendEquation blendEquationRGB, RendererBlendEquation blendEquationAlpha) override;

		void SetCullMode(RendererCullMode cullMode) override;

		void DrawIndexed(const Scope<VertexArray>& vertexArray, uint32_t indexCount) override;
		void Draw(const Scope<VertexArray>& vertexArray) override;

		const std::string& GetTitle() const override;

		std::array<uint8_t, 16> GetCurrentGPUUUID() override;
		std::string GetCurrentGPUName() override;
		std::vector<std::pair<std::string, std::array<uint8_t, 16>>> GetAllGPUs() override;

		//Instance Extensions
		static bool s_debugUtilsExtension;
		static bool s_swapchainColorSpaceExtension;
		static bool s_VRExtensions;

		//Device Extensions
		static bool s_shaderDrawParameters;
		static bool s_subgroupBroadcastDynamicID;
		static bool s_fragmentShaderInterlockExtension;
		static bool s_drawIndirectCountExtension;
		static bool s_descriptorIndexingExtension;
		static bool s_raytracingExtension;

		static bool s_renderdocCapture;
		static bool s_debugMarkerSupport;

		static struct GPUCapBits
		{
			std::array<bool, static_cast<uint32_t>(ImageFormat::IMAGE_FORMAT_COUNT)> CanShaderReadFrom{};
			std::array<bool, static_cast<uint32_t>(ImageFormat::IMAGE_FORMAT_COUNT)> CanShaderWriteTo{};
			std::array<bool, static_cast<uint32_t>(ImageFormat::IMAGE_FORMAT_COUNT)> CanRenderTargetWriteTo{};
		} s_GPUCapBits;

		struct SizeOffset
		{
			uint32_t Size;
			uint32_t Offset;
		};

		struct DescriptorIndexMap
		{
			std::unordered_map<std::string, uint32_t> Map;
		};
		
		union DescriptorUpdateData
		{
			VkDescriptorImageInfo ImageInfo;
			VkDescriptorBufferInfo BufferInfo;
			VkBufferView BufferView;
		};

		struct FrameBufferDesc
		{
			TRAP::Ref<VulkanRenderPass> RenderPass;
			std::vector<TRAP::Ref<VulkanRenderTarget>> RenderTargets;
			TRAP::Ref<VulkanRenderTarget> DepthStencil;
			std::vector<uint32_t> ColorArraySlices;
			std::vector<uint32_t> ColorMipSlices;
			uint32_t DepthArraySlice;
			uint32_t DepthMipSlice;
		};

		struct RenderPassDesc
		{
			std::vector<RendererAPI::ImageFormat> ColorFormats;
			std::vector<RendererAPI::LoadActionType> LoadActionsColor;
			std::vector<bool> SRGBValues;
			uint32_t RenderTargetCount;
			RendererAPI::SampleCount SampleCount;
			RendererAPI::ImageFormat DepthStencilFormat;
			RendererAPI::LoadActionType LoadActionDepth;
			RendererAPI::LoadActionType LoadActionStencil;
		};

		struct UpdateFrequencyLayoutInfo
		{
			//Array of all bindings in the descriptor set
			std::vector<VkDescriptorSetLayoutBinding> Bindings{};
			//Array of all descriptors in this descriptor set
			std::vector<RendererAPI::DescriptorInfo*> Descriptors{};
			//Array of all descriptors marked as dynamic in this descriptor set (applicable to DescriptorType::UniformBuffer)
			std::vector<RendererAPI::DescriptorInfo*> DynamicDescriptors{};
			//Hash map to get index of the descriptor in the root signature
			std::unordered_map<RendererAPI::DescriptorInfo*, uint32_t> DescriptorIndexMap{};
		};

		struct SubresourceDesc
		{
			uint64_t SrcOffset;
			uint32_t MipLevel;
			uint32_t ArrayLayer;
			uint32_t RowPitch;
			uint32_t SlicePitch;
		};

		struct NullDescriptors
		{
			std::array<TRAP::Ref<VulkanTexture>, static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCount)> DefaultTextureSRV;
			std::array<TRAP::Ref<VulkanTexture>, static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCount)> DefaultTextureUAV;
			TRAP::Ref<VulkanBuffer> DefaultBufferSRV;
			TRAP::Ref<VulkanBuffer> DefaultBufferUAV;
			TRAP::Ref<VulkanSampler> DefaultSampler;
			std::mutex SubmitMutex;
		};
		static TRAP::Scope<NullDescriptors> s_NullDescriptors;
		
	private:		
		static std::vector<std::string> SetupInstanceLayers();
		static std::vector<std::string> SetupInstanceExtensions();
		static std::vector<std::string> SetupDeviceExtensions(const TRAP::Scope<VulkanPhysicalDevice>& physicalDevice);

		void AddDefaultResources();
		void RemoveDefaultResources();
		
		std::string m_rendererTitle;

		TRAP::Ref<VulkanInstance> m_instance;
		TRAP::Scope<VulkanDebug> m_debug;
		TRAP::Ref<VulkanDevice> m_device;
		TRAP::Ref<VulkanMemoryAllocator> m_vma;
		TRAP::Ref<VulkanDescriptorPool> m_descriptorPool;

		std::mutex m_renderPassMutex;
		using RenderPassMap = std::unordered_map<uint64_t, VulkanRenderPass*>;
		TRAP::Scope<std::unordered_map<std::thread::id, RenderPassMap>> m_renderPassMap;
		using FrameBufferMap = std::unordered_map<uint64_t, VulkanFrameBuffer*>;
		TRAP::Scope<std::unordered_map<std::thread::id, FrameBufferMap>> m_frameBufferMap;

		static std::vector<std::pair<std::string, std::array<uint8_t, 16>>> s_usableGPUs;
		
		static VulkanRenderer* s_renderer;
	};
}

#endif /*_TRAP_VULKANRENDERER_H_*/