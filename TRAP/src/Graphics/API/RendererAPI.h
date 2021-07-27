#ifndef _TRAP_RENDERERAPI_H_
#define _TRAP_RENDERERAPI_H_

#include "Maths/Math.h"
#include "Window/Window.h"

#include "Layers/ImGui/ImGuiLayer.h"
#include "ImageFormat.h"

namespace TRAP
{
	class Application;
	class Window;
}

namespace TRAP::Graphics
{
	enum class BufferUsage;
	class VertexBufferLayout;
	class CommandPool;
	class DescriptorPool;
	class DescriptorSet;
	class Shader;
	class Pipeline;
	class PipelineCache;
	class SwapChain;
	class Fence;
	class Semaphore;
	class Queue;
	class Buffer;
	class CommandBuffer;
	class RootSignature;
	class Sampler;
	class RenderTarget;
	class TextureBase;
	enum class RendererCullMode;
	enum class RendererOperation;
	enum class RendererFaceMode;
	enum class RendererBlendEquation;
	enum class RendererBlendFunction;
	enum class RendererFrontFace;
	enum class RendererFunction;
}

namespace TRAP::Graphics::API
{
	class ResourceLoader;

	namespace ShaderReflection
	{
		enum class TextureDimension;
	}
}

namespace TRAP::Graphics
{
	enum class RenderAPI
	{
		NONE = 0,
		Vulkan
	};

	class RendererAPI
	{
	public:
		enum class CompareMode;
		enum class StencilOp;
		enum class CullMode;
		enum class FillMode;
		enum class PrimitiveTopology;
		enum class FrontFace;
		enum class BlendMode;
		enum class BlendConstant;
		enum class ClearFlags;
		enum class IndexType;
		enum class ClearBufferType;
	protected:
		struct PerWindowData;

	public:
		RendererAPI() = default;
		virtual ~RendererAPI() = default;
		RendererAPI(const RendererAPI&) = default;
		RendererAPI& operator=(const RendererAPI&) = default;
		RendererAPI(RendererAPI&&) = default;
		RendererAPI& operator=(RendererAPI&&) = default;

		static void Init(const std::string& gameName);
		static void Shutdown();

		static const TRAP::Scope<RendererAPI>& GetRenderer();
		static const TRAP::Scope<API::ResourceLoader>& GetResourceLoader();

		static void AutoSelectRenderAPI();
		static void SwitchRenderAPI(RenderAPI api);
		static bool IsSupported(RenderAPI api);
		static RenderAPI GetRenderAPI();

		virtual void InitInternal(const std::string& gameName) = 0;

		virtual void Present(const Scope<Window>& window) = 0;

		virtual void SetVSync(bool vsync, Window* window = nullptr) = 0;
		virtual bool GetVSync(Window* window = nullptr) = 0;

		//RenderTarget Stuff
		virtual void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f },
		                           Window* window = nullptr) = 0;
		virtual void SetClearDepth(float depth = 1.0f, Window* window = nullptr) = 0;
		virtual void SetClearStencil(uint32_t stencil = 0, Window* window = nullptr) = 0;

		//Pipeline Stuff
		virtual void SetDepthTesting(bool enabled, Window* window = nullptr) = 0;
		virtual void SetDepthWriting(bool enabled, Window* window = nullptr) = 0;
		virtual void SetDepthFunction(CompareMode function, Window* window = nullptr) = 0;
		virtual void SetDepthFail(StencilOp front, StencilOp back, Window* window = nullptr) = 0;
		virtual void SetDepthBias(int32_t depthBias, Window* window = nullptr) = 0;
		virtual void SetDepthBiasSlopeFactor(float factor, Window* window = nullptr) = 0;
		virtual void SetStencilTesting(bool enabled, Window* window = nullptr) = 0;
		virtual void SetStencilFail(StencilOp front, StencilOp back, Window* window = nullptr) = 0;
		virtual void SetStencilPass(StencilOp front, StencilOp back, Window* window = nullptr) = 0;
		virtual void SetStencilFunction(CompareMode front, CompareMode back, Window* window = nullptr) = 0;
		virtual void SetStencilMask(uint8_t read, uint8_t write, Window* window = nullptr) = 0;
		virtual void SetCullMode(CullMode mode, Window* window = nullptr) = 0;
		virtual void SetFillMode(FillMode mode, Window* window = nullptr) = 0;
		virtual void SetPrimitiveTopology(PrimitiveTopology topology, Window* window = nullptr) = 0;
		virtual void SetFrontFace(FrontFace face, Window* window = nullptr) = 0;
		virtual void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha, Window* window = nullptr) = 0;
		virtual void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
			                          BlendConstant destinationRGB, BlendConstant destinationAlpha,
									  Window* window = nullptr) = 0;

		virtual void Clear(ClearBufferType clearType, Window* window = nullptr) = 0;

		//CommandBuffer Stuff
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height, float minDepth = 0.0f,
		                         float maxDepth = 1.0f, Window* window = nullptr) = 0;
		virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
		                        Window* window = nullptr) = 0;

		virtual void Draw(uint32_t vertexCount, uint32_t firstVertex = 0, Window* window = nullptr) = 0;
		virtual void DrawIndexed(uint32_t indexCount, uint32_t firstIndex = 0, uint32_t firstVertex = 0,
		                         Window* window = nullptr) = 0;

		virtual void BindVertexBuffer(const TRAP::Ref<Buffer>& vBuffer, const VertexBufferLayout& layout,
		                              Window* window = nullptr) = 0;
		virtual void BindIndexBuffer(const TRAP::Ref<Buffer>& iBuffer, IndexType indexType,
		                             Window* window = nullptr) = 0;
		virtual void BindDescriptorSet(DescriptorSet& dSet, uint32_t index, Window* window = nullptr) = 0;
		virtual void BindPushConstants(const char* name, const void* constantsData, Window* window = nullptr) = 0;
		virtual void BindPushConstantsByIndex(uint32_t paramIndex, const void* constantsData,
		                                      Window* window = nullptr) = 0;

		//virtual void DrawIndexed(const Scope<VertexArray>& vertexArray, uint32_t indexCount) = 0;
		//virtual void Draw(const Scope<VertexArray>& vertexArray) = 0;

		virtual const std::string& GetTitle() const = 0;

		virtual std::array<uint8_t, 16> GetCurrentGPUUUID() = 0;
		virtual std::string GetCurrentGPUName() = 0;
		virtual std::vector<std::pair<std::string, std::array<uint8_t, 16>>> GetAllGPUs() = 0;

		static TRAP::Ref<TRAP::Graphics::DescriptorPool> GetDescriptorPool();
		static TRAP::Ref<TRAP::Graphics::Queue> GetGraphicsQueue();
		static TRAP::Ref<TRAP::Graphics::Queue> GetComputeQueue();
		static TRAP::Ref<TRAP::Graphics::RootSignature> GetGraphicsRootSignature(Window* window = nullptr);

	protected:
		static const TRAP::Scope<PerWindowData>& GetMainWindowData();

	public:
		virtual void InitPerWindowData(Window* window) = 0;
		virtual void RemovePerWindowData(Window* window) = 0;

		virtual void WaitIdle() = 0;

		static bool IsVulkanCapable();

		enum class WaveOpsSupportFlags : uint32_t
		{
			None = 0x0,
			Basic = BIT(0),
			Vote = BIT(1),
			Arithmetic = BIT(2),
			Ballot = BIT(3),
			Shuffle = BIT(4),
			ShuffleRelative = BIT(5),
			Clustered = BIT(6),
			Quad = BIT(7),
			PartitionedNV = BIT(8),

			WAVE_OPS_SUPPORT_FLAG_ALL = 0x7FFFFFFF
		};

		enum class QueueType
		{
			Graphics = 0,
			Transfer,
			Compute,

			MAX_QUEUE_TYPE
		};

		enum class QueueFlag : uint32_t
		{
			None = 0x0,
			DisableGPUTimeout = BIT(0),
			InitMicroprofile = BIT(1),

			MAX_QUEUE_FLAG = 0xFFFFFFFF
		};

		enum class QueuePriority
		{
			Normal,
			High,
			GlobalRealtime,

			MAX_QUEUE_PRIORITY
		};

		enum class FenceStatus
		{
			Complete = 0,
			Incomplete,
			NotSubmitted
		};

		enum class LoadActionType
		{
			DontCare,
			Load,
			Clear,

			MAX_LOAD_ACTION_TYPE
		};

		enum class TextureCubeType
		{
			MultiFile,
			Cross,
			//TODO
			//Equirectangular
		};

		enum class SampleCount
		{
			SampleCount1 = BIT(0),
			SampleCount2 = BIT(1),
			SampleCount4 = BIT(3),
			SampleCount8 = BIT(4),
			SampleCount16 = BIT(5)
		};

		enum class TextureCreationFlags
		{
			//Default flag (Texture will use default allocation strategy decided by the API specific allocator)
			None = 0x0,
			//Texture will allocate its own memory (COMMITTED resource)
			OwnMemory = BIT(0),
			//Texture will be allocated in memory which can be shared among multiple GPUs
			ExportAdapter = BIT(2),
			//Use ESRAM to store this texture
			ESRAM = BIT(4),
			//Use on-tile memory to store this texture
			OnTile = BIT(5),
			//Force 2D instead of automatically determining dimension based on width, height, depth
			Force2D = BIT(6),
			//Force 3D instead of automatically determining dimension based on width, height, depth
			Force3D = BIT(7),
			//Display target
			AllowDisplayTarget = BIT(8),
			//Create an sRGB texture
			SRGB = BIT(9),
			//Fast clear
			FastClear = BIT(10),
			//Fragment mask
			FragMask = BIT(11)
		};

		enum class ResourceState
		{
			Undefined = 0x0,
			VertexAndConstantBuffer = BIT(0),
			IndexBuffer = BIT(1),
			RenderTarget = BIT(2),
			UnorderedAccess = BIT(3),
			DepthWrite = BIT(4),
			DepthRead = BIT(5),
			NonPixelShaderResource = BIT(6),
			PixelShaderResource = BIT(7),
			ShaderResource = BIT(6) | BIT(7),
			StreamOut = BIT(8),
			IndirectArgument = BIT(9),
			CopyDestination = BIT(10),
			CopySource = BIT(11),
			GenericRead = (((((BIT(0) | BIT(1)) | BIT(6)) | BIT(7)) | BIT(9)) | BIT(11)),
			Present = BIT(12),
			Common = BIT(13),
			RayTracingAccelerationStructure = BIT(14),
			ShadingRateSource = BIT(15)
		};

		enum class DescriptorType
		{
			Undefined = 0,
			Sampler = BIT(0),
			//SRV read only Texture
			Texture = (Sampler << 1),
			//UAV Texture
			RWTexture = (Texture << 1),
			//SRV read only Buffer
			Buffer = (RWTexture << 1),
			BufferRaw = (Buffer | (Buffer << 1)),
			//UAV Buffer
			RWBuffer = (Buffer << 2),
			RWBufferRaw = (RWBuffer | (RWBuffer << 1)),
			//Uniform buffer
			UniformBuffer = (RWBuffer << 2),
			//Push constant / Root constant
			RootConstant = (UniformBuffer << 1),
			//IA
			VertexBuffer = (RootConstant << 1),
			IndexBuffer = (VertexBuffer << 1),
			IndirectBuffer = (IndexBuffer << 1),
			//Cubemap SRV
			TextureCube = (Texture | (IndirectBuffer << 1)),
			//RTV / DSV per mip slice
			RenderTargetMipSlices = (IndirectBuffer << 2),
			//RTV / DSV per array slice
			RenderTargetArraySlices = (RenderTargetMipSlices << 1),
			//RTV / DSV per depth sice
			RenderTargetDepthSlices = (RenderTargetArraySlices << 1),
			RayTracing = (RenderTargetDepthSlices << 1),

			//Vulkan
			//Subpass input (descriptor type only available in Vulkan)
			InputAttachment = (RayTracing << 1),
			TexelBuffer = (InputAttachment << 1),
			RWTexelBuffer = (TexelBuffer << 1),
			CombinedImageSampler = (RWTexelBuffer << 1)
		};

		//Choosing Memory Type
		enum class ResourceMemoryUsage
		{
			//No intended memory usage specified
			Unknown = 0,
			//Memory will be used on device only, no need to be mapped on host.
			GPUOnly = 1,
			//Memory will be mapped on host.
			//Could be used for transfer to device.
			CPUOnly = 2,
			//Memory will be used for frequent (dynamic) updates from host and reads on device.
			CPUToGPU = 3,
			//Memory will be used for writing on device and readback on host.
			GPUToCPU = 4,

			RESOURCE_MEMORY_USAGE_COUNT,
			RESOURCE_MEMORY_USAGE_MAX_ENUM = 0x7FFFFFFF
		};

		enum class BufferCreationFlags
		{
			//Default flag (Buffer will use aliased memory, buffer will not be CPU accessible until MapBuffer
			//is called)
			None = BIT(0),
			//Buffer will allocate its own memory (COMMITTED resource)
			OwnMemory = BIT(1),
			//Buffer will be persistently mapped
			PersistentMap = BIT(2),
			//Use ESRAM to store this buffer
			ESRAM = BIT(3),
			//Flag to specify not to allocate descriptors for the resource
			NoDescriptorViewCreation = BIT(4)
		};

		enum class ClearBufferType
		{
			NONE = 0,
			Color = BIT(0),
			Depth = BIT(1),
			Stencil = BIT(2),
			Color_Depth = Color | Depth,
			Color_Stencil = Color | Stencil,
			Color_Depth_Stencil = Color | Stencil | Depth,
			Depth_Stencil = Depth | Stencil
		};

		enum class IndirectArgumentType
		{
			IndirectDraw,
			IndirectDrawIndex,
			IndirectDispatch,
			IndirectVertexBuffer,
			IndirectIndexBuffer,
			IndirectConstant,
			IndirectDescriptorTable,
			IndirectPipeline
		};

		enum class DescriptorUpdateFrequency
		{
			None = 0,
			PerFrame,
			PerBatch,
			PerDraw,

			DESCRIPTOR_UPDATE_FREQUENCY_COUNT
		};

		enum class FilterType
		{
			Nearest = 0,
			Linear
		};

		enum class MipMapMode
		{
			Nearest = 0,
			Linear
		};

		enum class AddressMode
		{
			Mirror,
			Repeat,
			ClampToEdge,
			ClampToBorder
		};

		enum class CompareMode
		{
			Never,
			Less,
			Equal,
			LessOrEqual,
			Greater,
			NotEqual,
			GreaterOrEqual,
			Always,

			MAX_COMPARE_MODES
		};

		enum class ShaderStage
		{
			None = 0,
			Vertex = BIT(0),
			TessellationControl = BIT(1),
			TessellationEvaluation = BIT(2),
			Geometry = BIT(3),
			Fragment = BIT(4),
			Compute = BIT(5),
			RayTracing = BIT(6),

			AllGraphics = (static_cast<uint32_t>(Vertex) | static_cast<uint32_t>(TessellationControl) |
			               static_cast<uint32_t>(TessellationEvaluation) | static_cast<uint32_t>(Geometry) |
				           static_cast<uint32_t>(Fragment)),
			Hull = TessellationControl,
			Domain = TessellationEvaluation,

			SHADER_STAGE_COUNT = 7
		};

		enum class RootSignatureFlags
		{
			//Default flag
			None = 0,
			//Local root signature used mainly in raytracing shaders
			Local = BIT(0)
		};

		enum class PipelineType
		{
			Undefined = 0,
			Compute,
			Graphics,
			RayTracing,

			PIPELINE_TYPE_COUNT
		};

		enum class IndexType
		{
			UInt32 = 0,
			UInt16
		};

		enum class BlendConstant
		{
			Zero = 0,
			One,
			SrcColor,
			OneMinusSrcColor,
			DstColor,
			OneMinusDstColor,
			SrcAlpha,
			OneMinusSrcAlpha,
			DstAlpha,
			OneMinusDstAlpha,
			SrcAlphaSaturate,
			BlendFactor,
			OneMinusBlendFactor,

			MAX_BLEND_CONSTANTS
		};

		enum class BlendMode
		{
			Add,
			Subtract,
			ReverseSubtract,
			Min,
			Max,

			MAX_BLEND_MODES
		};

		//Blend states are always attached to one of the eight or more render targets that are in a MRT
		//Mask constants
		enum class BlendStateTargets
		{
			BlendStateTarget0 = BIT(0),
			BlendStateTarget1 = BIT(1),
			BlendStateTarget2 = BIT(2),
			BlendStateTarget3 = BIT(3),
			BlendStateTarget4 = BIT(4),
			BlendStateTarget5 = BIT(5),
			BlendStateTarget6 = BIT(6),
			BlendStateTarget7 = BIT(7),

			BlendStateTargetAll = 0xFF,
		};

		enum class StencilOp
		{
			Keep,
			SetZero,
			Replace,
			Invert,
			Increment,
			Decrement,
			IncrementSaturation,
			DecrementSaturation,

			MAX_STENCIL_OPS
		};

		enum class CullMode
		{
			None = 0,
			Back,
			Front,
			Both,

			MAX_CULL_MODES
		};

		enum class FrontFace
		{
			CounterClockwise = 0,
			Clockwise
		};

		enum class FillMode
		{
			Solid,
			WireFrame,

			MAX_FILL_MODES
		};

		enum class PipelineCacheFlags
		{
			None = 0x0,
			ExternallySynchronized = BIT(0)
		};

		enum class PrimitiveTopology
		{
			PointList = 0,
			LineList,
			LineStrip,
			TriangleList,
			TriangleStrip,
			PatchList,

			PRIMITIVE_TOPOLOGY_COUNT
		};

		enum class VertexAttributeRate
		{
			Vertex = 0,
			Instance = 1,

			VERTEX_ATTRIBUTE_RATE_COUNT
		};

		enum class QueryType
		{
			Timestamp = 0,
			PipelineStatistics,
			Occlusion,

			QUERY_TYPE_COUNT
		};

		enum class PresentStatus
		{
			Success = 0,
			DeviceReset = 1,
			Failed = 2,
			OutOfDate = 3
		};

		enum class SamplerRange
		{
			Full = 0,
			Narrow = 1
		};

		enum class SamplerModelConversion
		{
			RGBIdentity = 0,
			YCBCRIdentity = 1,
			YCBCR709 = 2,
			YCBCR601 = 3,
			YCBCR2020 = 4
		};

		enum class SampleLocation
		{
			Cosited = 0,
			Midpoint = 1
		};

		enum class ClearFlags
		{
			Color = 0x0,
			Depth = BIT(0),
			Stencil = BIT(1)
		};

		enum class ShadingRate
		{
			NotSupported = 0x0,
			Full = BIT(0),
			Half = Full << 1,
			Quarter = Half << 1,
			Eighth = Quarter << 1,
			OneXTwo = Eighth << 1,
			TwoXOne = OneXTwo << 1,
			TwoXFour = TwoXOne << 1,
			FourXTwo = TwoXFour << 1
		};

		enum class ShadingRateCombiner
		{
			Passthrough = 0,
			Override = 1,
			Min = 2,
			Max = 3,
			Sum = 4
		};

		enum class ShadingRateCaps
		{
			NotSupported = 0x0,
			PerDraw = BIT(0),
			PerTile = PerDraw << 1
		};

		struct SubresourceDataDesc
		{
			uint64_t SrcOffset;
			uint32_t MipLevel;
			uint32_t ArrayLayer;

			uint32_t RowPitch;
			uint32_t SlicePitch;
		};

		struct RenderTargetDesc
		{
			//Texture creation flags (decides memory allocation strategy, sharing access, ...)
			TextureCreationFlags Flags{};
			//Width
			uint32_t Width{};
			//Height
			uint32_t Height{};
			//Depth (should be 1 if type is not TextureType::TextureType2D)
			uint32_t Depth{};
			//Texture array size (should be 1 if texture is not a texture array or cubemap)
			uint32_t ArraySize{};
			//Number of mip levels
			uint32_t MipLevels{};
			//Multisample anti-aliasing (MSAA)
			TRAP::Graphics::RendererAPI::SampleCount SampleCount{};
			//Internal image format
			TRAP::Graphics::API::ImageFormat Format{};
			//What state will the texture get created in
			ResourceState StartState{};
			//Optimized clear value (recommended to use the same value when clearing the renderTarget)
			TRAP::Math::Vec4 ClearColor{};
			float ClearDepth = 1.0f;
			uint32_t ClearStencil = 0;
			//The image quality level.
			//The higher the quality, the lower the performance.
			//The valid range is between 0 and the value appropriate for SampleCount
			uint32_t SampleQuality{};
			//Descriptor creation
			DescriptorType Descriptors{};
			//Debug name used in GPU profile
			std::string Name{};

			void* NativeHandle{};
		};

		//Data structure holding necessary info to create a Texture
		struct TextureDesc
		{
			//Texture creation flags (decides memory allocation strategy, sharing access, ...)
			TextureCreationFlags Flags{};
			//Width
			uint32_t Width = 1;
			//Height;
			uint32_t Height = 1;
			//Depth (should be 1 if Type is not TextureType::TextureType3D)
			uint32_t Depth = 1;
			//Texture array size (should be 1 if texture is not a texture array or cubemap)
			uint32_t ArraySize = 1;
			//Number of mip levels
			uint32_t MipLevels = 1;
			//Number of multisamples per pixel (currently Textures created with Usage TextureUsage::SampledImage
			//only support SampleCount1).
			TRAP::Graphics::RendererAPI::SampleCount SampleCount{};
			//The image quality level.
			//The higher the quality, the lower the performance.
			//The valid range is between 0 and the value appropriate for SampleCount.
			uint32_t SampleQuality{};
			//Image format
			TRAP::Graphics::API::ImageFormat Format{};
			//Optimized clear value (recommended to use the same value when clearing the renderTarget)
			TRAP::Math::Vec4 ClearColor{};
			float ClearDepth = 1.0f;
			uint32_t ClearStencil = 0;
			//What state will the texture get created in
			ResourceState StartState{};
			//Descriptor creation
			DescriptorType Descriptors{};
			//Pointer to native texture handle if the texture does not own underlying resource
			void* NativeHandle{};
			//Debug name used in GPU profile
			std::string Name{};

			::VkSamplerYcbcrConversionInfo* VkSamplerYcbcrConversionInfo{};
		};

		struct TextureLoadDesc
		{
			TRAP::Ref<TRAP::Graphics::TextureBase>* Texture;
			//Load empty texture
			TRAP::Ref<TextureDesc> Desc;
			//Filepath with extension.
			std::array<std::string, 6> Filepaths;
			//Following is ignored if Desc != nullptr.
			//Desc->Flags will be considered instead.
			TextureCreationFlags CreationFlag;
			//Is Texture cubemap?
			bool IsCubemap;
			//Type of cubemap texture
			TextureCubeType Type;
		};

		//Data structure holding necessary info to create a Buffer
		struct BufferDesc
		{
			//Size of the buffer (in bytes)
			uint64_t Size{};
			//Alignment
			uint32_t Alignment{};
			//Decides which heap buffer will be used (default, upload, readback)
			ResourceMemoryUsage MemoryUsage{};
			//Creation flags of the buffer
			BufferCreationFlags Flags{};
			//What type of queue the buffer is owned by
			TRAP::Graphics::RendererAPI::QueueType QueueType{};
			//What state will the buffer get created in
			ResourceState StartState{};
			//Index of the first element accessible by the SRV/UAV
			uint64_t FirstElement{};
			//Number of elements in the buffer
			uint64_t ElementCount{};
			//Size of each element (in bytes) in the buffer
			uint64_t StructStride{};
			//ICB draw type
			IndirectArgumentType ICBDrawType{};
			//ICB max vertex buffers slots count
			uint32_t ICBMaxVertexBufferBind{};
			//ICB max fragment buffers slots count
			uint32_t ICBMaxFragmentBufferBind{};
			//Set this to specify a counter buffer for this buffer
			//struct Buffer* CounterBuffer;
			//Format of the buffer
			TRAP::Graphics::API::ImageFormat Format{};
			//Flags specifying the suitable usage of this buffer (Uniform Buffer, Vertex Buffer, Index Buffer, ...)
			DescriptorType Descriptors{};
			//Debug name used in GPU-profile
			const char* Name{};
		};

		struct SamplerDesc
		{
			FilterType MinFilter{};
			FilterType MagFilter{};
			TRAP::Graphics::RendererAPI::MipMapMode MipMapMode{};
			AddressMode AddressU{};
			AddressMode AddressV{};
			AddressMode AddressW{};
			float MipLodBias{};
			float MaxAnisotropy{};
			CompareMode CompareFunc{};

			struct SamplerConversionDesc
			{
				TRAP::Graphics::API::ImageFormat Format;
				SamplerModelConversion Model;
				SamplerRange Range;
				SampleLocation ChromaOffsetX;
				SampleLocation ChromaOffsetY;
				FilterType ChromaFilter;
				bool ForceExplicitReconstruction;

				bool operator==(const SamplerConversionDesc& s) const;
			} SamplerConversionDesc{};

			bool operator==(const SamplerDesc& s) const;
		};

		struct BinaryShaderStageDesc
		{
			std::vector<uint32_t> ByteCode{};
			std::string EntryPoint{};
		};

		struct BinaryShaderDesc
		{
			ShaderStage Stages{};
			BinaryShaderStageDesc Vertex{};
			BinaryShaderStageDesc Fragment{};
			BinaryShaderStageDesc Geometry{};
			BinaryShaderStageDesc TessellationControl{};
			BinaryShaderStageDesc TessellationEvaluation{};
			BinaryShaderStageDesc Compute{};
		};

		struct RootSignatureDesc
		{
			std::vector<Shader*> Shaders{};
			uint32_t MaxBindlessTextures{};
			std::vector<const char*> StaticSamplerNames{};
			std::vector<TRAP::Ref<Sampler>> StaticSamplers{};
			RootSignatureFlags Flags{};
		};

		struct DescriptorInfo
		{
			std::string Name{};
			DescriptorType Type{};
			API::ShaderReflection::TextureDimension Dimension{};
			bool RootDescriptor{};
			DescriptorUpdateFrequency UpdateFrequency{};
			uint32_t Size{};
			//Index in the descriptor set
			uint32_t IndexInParent{};
			uint32_t HandleIndex{};

			VkDescriptorType VkType{};
			uint32_t Reg{};
			uint32_t RootDescriptorIndex{};
			uint32_t VkStages{};
		};

		struct DescriptorSetDesc
		{
			TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
			DescriptorUpdateFrequency UpdateFrequency{};
			uint32_t MaxSets{};
		};

		struct CommandPoolDesc
		{
			TRAP::Ref<TRAP::Graphics::Queue> Queue;
			bool Transient;
		};

		struct QueueDesc
		{
			QueueType Type{};
			QueueFlag Flag{};
			QueuePriority Priority{};
		};

		struct ReadRange
		{
			uint64_t Offset{};
			uint64_t Range{};
		};

		struct QueueSubmitDesc
		{
			std::vector<CommandBuffer*> Cmds{};
			TRAP::Ref<Fence> SignalFence{};
			std::vector<TRAP::Ref<Semaphore>> WaitSemaphores{};
			std::vector<TRAP::Ref<Semaphore>> SignalSemaphores{};
			bool SubmitDone{};
		};

		struct BlendStateDesc
		{
			//Source blend factor per render target.
			std::array<BlendConstant, 8> SrcFactors{};
			//Destination blend factor per render target.
			std::array<BlendConstant, 8> DstFactors{};
			//Source alpha blend factor per render target.
			std::array<BlendConstant, 8> SrcAlphaFactors{};
			//Destination alpha blend factor per render target.
			std::array<BlendConstant, 8> DstAlphaFactors{};
			//Blend mode per render target.
			std::array<BlendMode, 8> BlendModes{};
			//Alpha blend mode per render target.
			std::array<BlendMode, 8> BlendAlphaModes{};
			//Write mask per render target.
			std::array<int32_t, 8> Masks{};
			//Mask that identifies the render targets affected by the blend state.
			BlendStateTargets RenderTargetMask{};
			//Set whether each render target has an unique blend function.
			//When false the blend function in slot 0 will be used for all render targets.
			bool IndependentBlend{};
		};

		struct DepthStateDesc
		{
			bool DepthTest{};
			bool DepthWrite{};
			CompareMode DepthFunc{};
			bool StencilTest{};
			uint8_t StencilReadMask{};
			uint8_t StencilWriteMask{};
			CompareMode StencilFrontFunc{};
			StencilOp StencilFrontFail{};
			StencilOp DepthFrontFail{};
			StencilOp StencilFrontPass{};
			CompareMode StencilBackFunc{};
			StencilOp StencilBackFail{};
			StencilOp DepthBackFail{};
			StencilOp StencilBackPass{};
		};

		struct RasterizerStateDesc
		{
			TRAP::Graphics::RendererAPI::CullMode CullMode{};
			int32_t DepthBias{};
			float SlopeScaledDepthBias{};
			TRAP::Graphics::RendererAPI::FillMode FillMode{};
			TRAP::Graphics::RendererAPI::FrontFace FrontFace{};
			bool DepthClampEnable{};
		};

		struct PipelineCacheDesc
		{
			std::vector<uint8_t> Data{};
			PipelineCacheFlags Flags{};
		};

		struct PipelineCacheLoadDesc
		{
			std::string VirtualOrPhysicalPath{};
			PipelineCacheFlags Flags{};
		};

		struct ComputePipelineDesc
		{
			Shader* ShaderProgram{};
			TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
		};

		struct VertexAttribute
		{
			TRAP::Graphics::API::ImageFormat Format{};
			uint32_t Binding{};
			uint32_t Location{};
			uint32_t Offset{};
			VertexAttributeRate Rate{};
		};

		struct VertexLayout
		{
			uint32_t AttributeCount{};
			std::array<VertexAttribute, 15> Attributes{};
		};

		struct GraphicsPipelineDesc
		{
			Shader* ShaderProgram{};
			TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
			TRAP::Ref<TRAP::Graphics::RendererAPI::VertexLayout> VertexLayout{};
			TRAP::Ref<BlendStateDesc> BlendState{};
			TRAP::Ref<DepthStateDesc> DepthState{};
			TRAP::Ref<RasterizerStateDesc> RasterizerState{};
			std::vector<TRAP::Graphics::API::ImageFormat> ColorFormats{};
			uint32_t RenderTargetCount{};
			TRAP::Graphics::RendererAPI::SampleCount SampleCount{};
			uint32_t SampleQuality{};
			TRAP::Graphics::API::ImageFormat DepthStencilFormat{};
			TRAP::Graphics::RendererAPI::PrimitiveTopology PrimitiveTopology{};
			bool SupportIndirectCommandBuffer{};
		};

		struct RayTracingPipelineDesc
		{
		};

		struct PipelineDesc
		{
			PipelineType Type{};
			std::variant<ComputePipelineDesc,
			             GraphicsPipelineDesc,
						 RayTracingPipelineDesc> Pipeline{GraphicsPipelineDesc()};
			TRAP::Ref<PipelineCache> Cache{};
			void* PipelineExtensions{};
			uint32_t PipelineExtensionCount{};
			const char* Name{};
		};

		struct QueryPoolDesc
		{
			QueryType Type{};
			uint32_t QueryCount{};
		};

		struct QueryDesc
		{
			uint32_t Index{};
		};

		struct IndirectDrawArguments
		{
			uint32_t VertexCount{};
			uint32_t InstanceCount{};
			uint32_t StartVertex{};
			uint32_t StartInstance{};
		};

		struct IndirectDrawIndexArguments
		{
			uint32_t IndexCount{};
			uint32_t InstanceCount{};
			uint32_t StartIndex{};
			uint32_t VertexOffset{};
			uint32_t StartInstance{};
		};

		struct IndirectDispatchArguments
		{
			uint32_t GroupCountX{};
			uint32_t GroupCountY{};
			uint32_t GroupCountZ{};
		};

		struct IndirectArgumentDescriptor
		{
			IndirectArgumentType Type{};
			const char* Name{};
			uint32_t Index{};
		};

		struct CommandSignatureDesc
		{
			TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
			uint32_t IndirectArgCount{};
			std::vector<IndirectArgumentDescriptor> ArgDescs{};
			//Set to true if indirect argument struct should not be aligned to 16 bytes
			bool Packed{};
		};

		struct SwapChainDesc
		{
			//Window handle
			TRAP::Window* Window{};
			//Queues which should be allowed to present
			std::vector<TRAP::Ref<Queue>> PresentQueues{};
			//Number of back buffers in this swapchain
			uint32_t ImageCount{};
			//Width of the swapchain
			uint32_t Width{};
			//Height of the swapchain
			uint32_t Height{};
			//Color format of the swapchain
			TRAP::Graphics::API::ImageFormat ColorFormat{};
			//Clear value
			TRAP::Math::Vec4 ClearColor{};
			float ClearDepth = 1.0f;
			uint32_t ClearStencil = 0;
			//Set whether swapchain will be presented using VSync
			bool EnableVSync{};
			//We can toggle to using FLIP model if application desires
			bool UseFlipSwapEffect{};
		};

		struct RenderTargetBarrier
		{
			TRAP::Ref<TRAP::Graphics::RenderTarget> RenderTarget{};
			ResourceState CurrentState{};
			ResourceState NewState{};
			bool BeginOnly{};
			bool EndOnly{};
			bool Acquire{};
			bool Release{};
			TRAP::Graphics::RendererAPI::QueueType QueueType{};
			//Specify whether following barrier targets particular subresource
			bool SubresourceBarrier{};
			//Following values are ignored if SubresourceBarrier is false
			uint8_t MipLevel{};
			uint16_t ArrayLayer{};
		};

		struct BufferBarrier
		{
			TRAP::Ref<TRAP::Graphics::Buffer> Buffer{};
			ResourceState CurrentState{};
			ResourceState NewState{};
			bool BeginOnly{};
			bool EndOnly{};
			bool Acquire{};
			bool Release{};
			TRAP::Graphics::RendererAPI::QueueType QueueType{};
		};

		struct TextureBarrier
		{
			TRAP::Ref<TRAP::Graphics::TextureBase> Texture{};
			ResourceState CurrentState{};
			ResourceState NewState{};
			bool BeginOnly{};
			bool EndOnly{};
			bool Acquire{};
			bool Release{};
			TRAP::Graphics::RendererAPI::QueueType QueueType{};
			//Specify whether the following barrier targets particular subresource
			bool SubresourceBarrier{};
			//Following values are ignored if SubresourceBarrier is false
			uint8_t MipLevel{};
			uint16_t ArrayLayer{};
		};

		struct DescriptorData
		{
			//User can either set name of descriptor or index (index in RootSignature->Descriptors array)
			//Name of descriptor
			const char* Name{};
			struct BufferOffset
			{
				//Offset to bind the buffer descriptor
				std::vector<uint64_t> Offsets{};
				std::vector<uint64_t> Sizes{};
			};

			//Descriptor set buffer extraction options
			struct DescriptorSetExtraction
			{
				uint32_t DescriptorSetBufferIndex{};
				Shader* DescriptorSetShader{};
				ShaderStage DescriptorSetShaderStage{};
			};

			struct TextureSlice
			{
				uint32_t UAVMipSlice{};
				bool BindMipChain{};
			};
			std::variant<BufferOffset, DescriptorSetExtraction, TextureSlice, bool> Offset{false};
			//Array of resources containing descriptor handles or constant to be used in ring buffer memory
			//DescriptorRange can hold only one resource type array
			//std::vector<TRAP::Ref<API::VulkanAccelerationStructure>> AccelerationStructures; //TODO RT
			std::variant<std::vector<TRAP::Graphics::TextureBase*>, std::vector<Sampler*>,
				std::vector<Buffer*>, std::vector<Pipeline*>,
				std::vector<DescriptorSet*>> Resource{std::vector<TRAP::Graphics::TextureBase*>()};

			//Number of resources in the descriptor(applies to array of textures, buffers, ...)
			uint32_t Count{};
			uint32_t Index = static_cast<uint32_t>(-1);
			bool ExtractBuffer = false;
		};

		struct QueuePresentDesc
		{
			TRAP::Ref<TRAP::Graphics::SwapChain> SwapChain{};
			std::vector<TRAP::Ref<Semaphore>> WaitSemaphores{};
			uint8_t Index{};
			bool SubmitDone{};
		};

		struct LoadActionsDesc
		{
			std::array<TRAP::Math::Vec4, 8> ClearColorValues{};
			std::array<LoadActionType, 8> LoadActionsColor{};
			float ClearDepth = 1.0f;
			uint32_t ClearStencil = 0;
			LoadActionType LoadActionDepth{};
			LoadActionType LoadActionStencil{};
		};

		struct DescriptorIndexMap
		{
			std::unordered_map<std::string, uint32_t> Map;
		};

		struct MappedMemoryRange
		{
			uint8_t* Data = nullptr;
			TRAP::Ref<TRAP::Graphics::Buffer> Buffer = nullptr;
			uint64_t Offset = 0;
			uint64_t Size = 0;
			uint32_t Flags = 0;
		};

		//Note: Only use for procedural textures which are created on CPU (noise textures, font texture, ...)
		struct TextureUpdateDesc
		{
			TRAP::Ref<TRAP::Graphics::TextureBase> Texture = nullptr;
			uint32_t MipLevel = 0;
			uint32_t ArrayLayer = 0;

			//To be filled by the caller
			//Example
			//TRAP::Graphics::RendererAPI::TextureUpdateDesc update = {Texture};
			//TRAP::Graphics::RendererAPI::GetResourceLoader()->BeginUpdateResource(update);
			//Row by row copy is required if DstRowStride > SrcRowStride.
			//Single memcpy will work if DstRowStride == SrcRowStride.
			//2D:
			//for (uint32_t r = 0; r < update.RowCount; ++r)
			//    memcpy(update.MappedData + r * update.DstRowStride, srcPixels + r * update.SrcRowStride, update.SrcRowStride);
			//3D:
			//for (uint32_t z = 0; z < depth; ++z)
			//{
			//    uint8_t* dstData = update.MappedData + update.DstSliceStride * z;
			//    uint8_t* srcData = srcPixels + update.SrcSliceStride * z;
			//    for (uint32_t r = 0; r < update.RowCount; ++r)
			//        memcpy(dstData + r * update.DstRowStride, srcData + r * update.SrcRowStride, update.SrcRowStride);
			//}
			//TRAP::Graphics::RendererAPI::GetResourceLoader()->EndUpdateResource(update, &token);
			uint8_t* MappedData;
			//Size of each row in destination including padding - Needs to be respected
			//otherwise texture data will be corrupted if dst row stride is not the same as src row stride
			uint32_t DstRowStride;
			//Number of rows in this slice of the texture
			uint32_t RowCount;
			//Src row stride for convenience (RowCount * width * texture format size)
			uint32_t SrcRowStride;
			//Size of each slice in destination including padding - Use for offsetting dst data updating 3D textures
			uint32_t DstSliceStride;
			//Size of each slice in src - Use for offsetting src data when updating 3D textures
			uint32_t SrcSliceStride;

			//Internal
			struct
			{
				MappedMemoryRange MappedRange;
			} Internal;
		};
		struct ResourceLoaderDesc
		{
			uint64_t BufferSize;
			uint32_t BufferCount;
		};

		struct BufferLoadDesc
		{
			TRAP::Ref<TRAP::Graphics::Buffer> Buffer;
			const void* Data;
			BufferDesc Desc;

			//Force Reset Buffer to nullptr
			bool ForceReset;
		};

		struct BufferUpdateDesc
		{
			TRAP::Ref<TRAP::Graphics::Buffer> Buffer;
			uint64_t DstOffset;
			uint64_t Size;

			//To be filled by the caller
			void* MappedData;

			//Internal
			struct
			{
				MappedMemoryRange MappedRange;
			} Internal;
		};

		inline static struct GPUSettings
		{
			uint32_t UniformBufferAlignment;
			uint32_t UploadBufferTextureAlignment;
			uint32_t UploadBufferTextureRowAlignment;
			uint32_t MaxVertexInputBindings;
			uint32_t MaxRootSignatureDWORDS;
			uint32_t WaveLaneCount;
			TRAP::Graphics::RendererAPI::WaveOpsSupportFlags WaveOpsSupportFlags;
			bool MultiDrawIndirect;
			uint32_t ROVsSupported;
			bool TessellationSupported;
			bool GeometryShaderSupported;
			float MaxAnisotropy;
			uint32_t MaxImageDimension2D;
			uint32_t MaxImageDimensionCube;
		} GPUSettings{};

		inline static constexpr uint32_t ImageCount = 3; //Triple Buffered

	protected:
		static TRAP::Scope<RendererAPI> s_Renderer;
		static RenderAPI s_RenderAPI;
		static TRAP::Scope<API::ResourceLoader> s_ResourceLoader;

		static TRAP::Ref<DescriptorPool> s_descriptorPool;
		static TRAP::Ref<Queue> s_graphicQueue;
		static TRAP::Ref<Queue> s_computeQueue;
		static std::array<TRAP::Ref<CommandPool>, ImageCount> s_computeCommandPools;
		static std::array<CommandBuffer*, ImageCount> s_computeCommandBuffers;

		friend class TRAP::ImGuiLayer;

		struct PerWindowData
		{
			friend class TRAP::ImGuiLayer;

			~PerWindowData();

			TRAP::Window* Window;

			uint32_t ImageIndex = 0;
			std::array<TRAP::Ref<CommandPool>, ImageCount> GraphicCommandPools;
			std::array<CommandBuffer*, ImageCount> GraphicCommandBuffers;
			std::array<TRAP::Ref<Fence>, ImageCount> RenderCompleteFences;
			TRAP::Ref<Semaphore> ImageAcquiredSemaphore;
			std::array<TRAP::Ref<Semaphore>, ImageCount> RenderCompleteSemaphores;

			TRAP::Ref<TRAP::Graphics::SwapChain> SwapChain;
			uint32_t CurrentSwapChainImageIndex;

			TRAP::Math::Vec4 ClearColor{0.1f, 0.1f, 0.1f, 1.0f};
			float ClearDepth = 1.0f;
			uint32_t ClearStencil = 0;

			bool CurrentVSync;
			bool NewVSync;

			PipelineDesc GraphicsPipelineDesc;
			TRAP::Ref<Pipeline> CurrentGraphicsPipeline;

			bool Recording;
		};

	protected:
		static std::unordered_map<Window*, TRAP::Scope<PerWindowData>> s_perWindowDataMap;
		static std::mutex s_perWindowDataMutex;

	private:
		static bool s_isVulkanCapable;
		static bool s_isVulkanCapableFirstTest;
	};
}

MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::WaveOpsSupportFlags)
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::TextureCreationFlags);
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::ResourceState);
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::DescriptorType);
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::QueueFlag);
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::BufferCreationFlags);
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::ShaderStage);
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::RootSignatureFlags);
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::BlendStateTargets);
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::PipelineCacheFlags);
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::ClearFlags);
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::ShadingRate);
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::ShadingRateCaps);
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::ClearBufferType);

#endif /*_TRAP_RENDERERAPI_H_*/