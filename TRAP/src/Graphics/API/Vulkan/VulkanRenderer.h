#ifndef TRAP_VULKANRENDERER_H
#define TRAP_VULKANRENDERER_H

#include "Graphics/API/RendererAPI.h"
#include "Maths/Math.h"
#include "Window/WindowingAPI.h"
#include "Graphics/API/ShaderReflection.h"

namespace TRAP::Graphics
{
	class TextureBase;
}

namespace TRAP::Graphics::API
{
	class VulkanSampler;
	class VulkanRenderTarget;
	class VulkanBuffer;
	class VulkanMemoryAllocator;
	class VulkanFrameBuffer;
	class VulkanRenderPass;
	class VulkanDescriptorPool;
	class VulkanPhysicalDevice;
	class VulkanDevice;
	class VulkanInstance;
	class VulkanDebug;
	class VulkanCommandPool;
	class VulkanCommandBuffer;
	class VulkanFence;
	class VulkanQueue;

	class VulkanRenderer final : public RendererAPI
	{
	public:
		explicit VulkanRenderer();
		~VulkanRenderer() override;

		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;
		VulkanRenderer(VulkanRenderer&&) = delete;
		VulkanRenderer& operator=(VulkanRenderer&&) = delete;

		void InitInternal(const std::string_view gameName) override;

		void Present(Window* window) override;

		void SetVSync(bool vsync, Window* window = nullptr) override;

		void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f }, Window* window = nullptr) override;
		void SetClearDepth(float depth = 1.0f, Window* window = nullptr) override;
		void SetClearStencil(uint32_t stencil = 0, Window* window = nullptr) override;
		void SetDepthTesting(bool enabled, Window* window = nullptr) override;
		void SetDepthWriting(bool enabled, Window* window = nullptr) override;
		void SetDepthFunction(CompareMode function, Window* window = nullptr) override;
		void SetDepthFail(StencilOp front, StencilOp back, Window* window) override;
		void SetDepthBias(int32_t depthBias, Window* window = nullptr) override;
		void SetDepthBiasSlopeFactor(float factor, Window* window = nullptr) override;
		void SetStencilTesting(bool enabled, Window* window = nullptr) override;
		void SetStencilFail(StencilOp front, StencilOp back, Window* window = nullptr) override;
		void SetStencilPass(StencilOp front, StencilOp back, Window* window = nullptr) override;
		void SetStencilFunction(CompareMode front, CompareMode back, Window* window = nullptr) override;
		void SetStencilMask(uint8_t read, uint8_t write, Window* window = nullptr) override;
		void SetCullMode(CullMode mode, Window* window = nullptr) override;
		void SetFillMode(FillMode mode, Window* window = nullptr) override;
		void SetPrimitiveTopology(PrimitiveTopology topology, Window* window = nullptr) override;
		void SetFrontFace(FrontFace face, Window* window = nullptr) override;
		void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha, Window* window = nullptr) override;
		void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
							  BlendConstant destinationRGB, BlendConstant destinationAlpha,
							  Window* window = nullptr) override;

		void Clear(ClearBufferType clearType, Window* window = nullptr) override;

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height, float minDepth = 0.0f,
		                 float maxDepth = 1.0f, Window* window = nullptr) override;
		void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Window* window = nullptr) override;

		void Draw(uint32_t vertexCount, uint32_t firstVertex, Window* window = nullptr) override;
		void DrawIndexed(uint32_t indexCount, uint32_t firstIndex, uint32_t firstVertex,
		                 Window* window = nullptr) override;
		void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex = 0,
		                   uint32_t firstInstance = 0, Window* window = nullptr) override;
		void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount,
		                          uint32_t firstIndex = 0, uint32_t firstInstance = 0,
							      uint32_t firstVertex = 0, Window* window = nullptr) override;

		void BindShader(Shader* shader, Window* window = nullptr) const;
		void BindVertexBuffer(const TRAP::Ref<Buffer>& vBuffer, const VertexBufferLayout& layout,
		                      Window* window = nullptr) override;
		void BindIndexBuffer(const TRAP::Ref<Buffer>& iBuffer, IndexType indexType,
		                     Window* window = nullptr) override;
		void BindDescriptorSet(DescriptorSet& dSet, uint32_t index, Window* window = nullptr) override;
		void BindPushConstants(const char* name, const void* constantsData, Window* window = nullptr) override;
		void BindPushConstantsByIndex(uint32_t paramIndex, const void* constantsData,
		                              Window* window = nullptr) override;

		const std::string& GetTitle() const override;
		bool GetVSync(Window* window = nullptr) override;

		std::array<uint8_t, 16> GetCurrentGPUUUID() override;
		std::string GetCurrentGPUName() override;
		std::vector<std::pair<std::string, std::array<uint8_t, 16>>> GetAllGPUs() override;

		void InitPerWindowData(Window* window) override;
		void RemovePerWindowData(Window* window) override;

		void WaitIdle() override;

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
		static bool s_samplerYcbcrConversionExtension;

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

		union DescriptorUpdateData
		{
			VkDescriptorImageInfo ImageInfo;
			VkDescriptorBufferInfo BufferInfo;
			VkBufferView BufferView;
		};

		struct FrameBufferDesc
		{
			TRAP::Ref<VulkanRenderPass> RenderPass;
			std::vector<TRAP::Ref<RenderTarget>> RenderTargets;
			TRAP::Ref<RenderTarget> DepthStencil;
			std::vector<uint32_t> ColorArraySlices;
			std::vector<uint32_t> ColorMipSlices;
			uint32_t DepthArraySlice;
			uint32_t DepthMipSlice;
		};

		struct RenderPassDesc
		{
			std::vector<ImageFormat> ColorFormats;
			std::vector<LoadActionType> LoadActionsColor;
			std::vector<bool> SRGBValues;
			uint32_t RenderTargetCount;
			TRAP::Graphics::RendererAPI::SampleCount SampleCount;
			ImageFormat DepthStencilFormat;
			LoadActionType LoadActionDepth;
			LoadActionType LoadActionStencil;
		};

		struct UpdateFrequencyLayoutInfo
		{
			//Array of all bindings in the descriptor set
			std::vector<VkDescriptorSetLayoutBinding> Bindings{};
			//Array of all descriptors in this descriptor set
			std::vector<DescriptorInfo*> Descriptors{};
			//Array of all descriptors marked as dynamic in this descriptor set
			//(applicable to DescriptorType::UniformBuffer)
			std::vector<DescriptorInfo*> DynamicDescriptors{};
			//Hash map to get index of the descriptor in the root signature
			std::unordered_map<DescriptorInfo*, uint64_t> DescriptorIndexMap{};
		};

		//Create default resources to be used as null descriptors in case user does not specify some descriptors
		struct NullDescriptors
		{
			std::array<TRAP::Ref<TRAP::Graphics::TextureBase>,
			           static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCount)> DefaultTextureSRV;
			std::array<TRAP::Ref<TRAP::Graphics::TextureBase>,
			           static_cast<uint32_t>(ShaderReflection::TextureDimension::TextureDimCount)> DefaultTextureUAV;
			TRAP::Ref<VulkanBuffer> DefaultBufferSRV;
			TRAP::Ref<VulkanBuffer> DefaultBufferUAV;
			TRAP::Ref<VulkanSampler> DefaultSampler;
			std::mutex SubmitMutex;

			//Unlike DirectX 12, Vulkan textures start in undefined layout.
			//With this, we transition them to the specified layout so app codee doesn't
			//have to worry about this
			std::mutex InitialTransitionMutex;
			TRAP::Ref<VulkanQueue> InitialTransitionQueue;
			TRAP::Ref<VulkanCommandPool> InitialTransitionCmdPool;
			VulkanCommandBuffer* InitialTransitionCmd;
			TRAP::Ref<VulkanFence> InitialTransitionFence;
		};
		static TRAP::Scope<NullDescriptors> s_NullDescriptors;
		static std::vector<VkPipelineColorBlendAttachmentState> DefaultBlendAttachments;
		static VkPipelineRasterizationStateCreateInfo DefaultRasterizerDesc;
		static VkPipelineDepthStencilStateCreateInfo DefaultDepthDesc;
		static VkPipelineColorBlendStateCreateInfo DefaultBlendDesc;

		static void UtilInitialTransition(TRAP::Ref<TRAP::Graphics::TextureBase> texture, RendererAPI::ResourceState startState);

		//Per Thread Render Pass synchronization logic
		//Render-passes are not exposed to the engine code since they are not available on all APIs
		//This map takes care of hashing a render pass based on the render targets passed to
		//CommandBuffer::BeginRender()
		using RenderPassMap = std::unordered_map<uint64_t, TRAP::Ref<VulkanRenderPass>>;
		using RenderPassMapNode = RenderPassMap::value_type;
		using RenderPassMapIt = RenderPassMap::iterator;
		using FrameBufferMap = std::unordered_map<uint64_t, TRAP::Ref<VulkanFrameBuffer>>;
		using FrameBufferMapNode = FrameBufferMap::value_type;
		using FrameBufferMapIt = FrameBufferMap::iterator;

		static RenderPassMap& GetRenderPassMap();
		static FrameBufferMap& GetFrameBufferMap();

		TRAP::Ref<VulkanInstance> GetInstance() const;
		TRAP::Ref<VulkanDevice> GetDevice() const;
		TRAP::Ref<VulkanMemoryAllocator> GetVMA() const;

		static const TRAP::Ref<Pipeline>& GetPipeline(PipelineDesc& desc);

	private:
		static std::vector<std::string> SetupInstanceLayers();
		static std::vector<std::string> SetupInstanceExtensions();
		static std::vector<std::string> SetupDeviceExtensions(const TRAP::Scope<VulkanPhysicalDevice>& physicalDevice);

		void AddDefaultResources() const;
		static void RemoveDefaultResources();

		static void StartGraphicRecording(const TRAP::Scope<PerWindowData>& p);
		static void EndGraphicRecording(const TRAP::Scope<PerWindowData>& p);

		std::string m_rendererTitle;

		TRAP::Ref<VulkanInstance> m_instance;
		TRAP::Scope<VulkanDebug> m_debug;
		TRAP::Ref<VulkanDevice> m_device;
		TRAP::Ref<VulkanMemoryAllocator> m_vma;

		//RenderPass map per thread (this will make lookups lock free and we only need a lock when inserting
		//a RenderPass Map for the first time)
		static std::unordered_map<std::thread::id, RenderPassMap> s_renderPassMap;
		//FrameBuffer map per thread (this will make lookups lock free and we only need a lock when inserting
		//a FrameBuffer Map for the first time)
		static std::unordered_map<std::thread::id, FrameBufferMap> s_frameBufferMap;
		static std::mutex s_renderPassMutex;

		static std::vector<std::pair<std::string, std::array<uint8_t, 16>>> s_usableGPUs;
		static std::unordered_map<uint64_t, TRAP::Ref<Pipeline>> s_pipelines;
		static std::unordered_map<uint64_t, TRAP::Ref<PipelineCache>> s_pipelineCaches;
		static std::mutex s_pipelineMutex;

		static VulkanRenderer* s_renderer;
	};
}

#endif /*TRAP_VULKANRENDERER_H*/