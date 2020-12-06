#ifndef _TRAP_RENDERERAPI_H_
#define _TRAP_RENDERERAPI_H_

#include "Maths/Math.h"
#include "Window/Window.h"

namespace TRAP
{
	class Application;
	class Window;
}

namespace TRAP::Graphics 
{
	enum class RendererCullMode;
	enum class RendererOperation;
	class VertexArray;
	enum class RendererFaceMode;
	enum class RendererBlendEquation;
	enum class RendererBlendFunction;
	enum class RendererFrontFace;
	enum class RendererFunction;
	enum class RendererBufferType;
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
		RendererAPI() = default;
		virtual ~RendererAPI() = default;
		RendererAPI(const RendererAPI&) = default;
		RendererAPI& operator=(const RendererAPI&) = default;
		RendererAPI(RendererAPI&&) = default;
		RendererAPI& operator=(RendererAPI&&) = default;

		static void Init();
		static void Shutdown();

		static const TRAP::Scope<RendererAPI>& GetRenderer();
		
		static void AutoSelectRenderAPI();
		static void SwitchRenderAPI(RenderAPI api);
		static bool IsSupported(RenderAPI api);
		static RenderAPI GetRenderAPI();
		static void SetVSync(bool enabled);
		static bool GetVSync();
		static void Use(Window* window);

		virtual void InitInternal() = 0;
		
		virtual void Clear(RendererBufferType buffer) = 0;
		virtual void Present(const Scope<Window>& window) = 0;

		virtual void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f }) = 0;
		virtual void SetDepthTesting(bool enabled) = 0;
		virtual void SetDepthMasking(bool enabled) = 0;
		virtual void SetDepthFunction(RendererFunction function) = 0;
		virtual void SetStencilTesting(bool enabled) = 0;
		virtual void SetStencilMasking(uint32_t mask) = 0;
		virtual void SetStencilMaskingSeparate(RendererFaceMode face, uint32_t mask) = 0;
		virtual void SetStencilFunction(RendererFunction function, int32_t reference, uint32_t mask) = 0;
		virtual void SetStencilFunctionSeparate(RendererFaceMode face, RendererFunction function, int32_t reference, uint32_t mask) = 0;
		virtual void SetStencilOperation(RendererOperation stencilFail, RendererOperation depthFail, RendererOperation pass) = 0;
		virtual void SetStencilOperationSeparate(RendererFaceMode face, RendererOperation stencilFail, RendererOperation depthFail, RendererOperation pass) = 0;
		virtual void SetBlend(bool enabled) = 0;
		virtual void SetCull(bool enabled) = 0;
		virtual void SetFrontFace(RendererFrontFace frontFace) = 0;
		virtual void SetWireFrame(bool enabled) = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void SetBlendFunction(RendererBlendFunction source, RendererBlendFunction destination) = 0;
		virtual void SetBlendFunctionSeparate(RendererBlendFunction sourceRGB,
			                                  RendererBlendFunction sourceAlpha,
			                                  RendererBlendFunction destinationRGB,
			                                  RendererBlendFunction destinationAlpha
		) = 0;
		virtual void SetBlendEquation(RendererBlendEquation blendEquation) = 0;
		virtual void SetBlendEquationSeparate(RendererBlendEquation blendEquationRGB, RendererBlendEquation blendEquationAlpha) = 0;

		virtual void SetCullMode(RendererCullMode cullMode) = 0;

		virtual void DrawIndexed(const Scope<VertexArray>& vertexArray, uint32_t indexCount) = 0;
		virtual void Draw(const Scope<VertexArray>& vertexArray) = 0;

		virtual const std::string& GetTitle() const = 0;

		virtual std::array<uint8_t, 16> GetCurrentGPUUUID() = 0;
		virtual std::string GetCurrentGPUName() = 0;
		virtual std::vector<std::pair<std::string, std::array<uint8_t, 16>>> GetAllGPUs() = 0;

		static bool IsVulkanCapable();

		enum class ImageFormat;
		
		static constexpr bool ImageFormatIsDepthOnly(const ImageFormat fmt)
		{
			switch (fmt)
			{
			case ImageFormat::D16_UNORM:
				return true;

			case ImageFormat::X8_D24_UNORM:
				return true;

			case ImageFormat::D32_SFLOAT:
				return true;

			default:
				return false;
			}
		}
		static constexpr bool ImageFormatIsDepthAndStencil(const ImageFormat fmt)
		{
			switch (fmt)
			{
			case ImageFormat::D16_UNORM_S8_UINT:
				return true;

			case ImageFormat::D24_UNORM_S8_UINT:
				return true;

			case ImageFormat::D32_SFLOAT_S8_UINT:
				return true;

			default:
				return false;
			}
		}
		static constexpr bool ImageFormatIsStencilOnly(const ImageFormat fmt)
		{
			switch(fmt)
			{
			case ImageFormat::S8_UINT:
				return true;
				
			default:
				return false;
			}
		}
		static constexpr bool ImageFormatHasStencil(const ImageFormat fmt)
		{
			return ImageFormatIsStencilOnly(fmt) || ImageFormatIsDepthAndStencil(fmt);
		}

		enum class WaveOpsSupportFlags : uint32_t
		{
			None = 0x0,
			Basic = 0x00000001,
			Vote = 0x00000002,
			Arithmetic = 0x00000004,
			Ballot = 0x00000008,
			Shuffle = 0x00000010,
			ShuffleRelative = 0x00000020,
			Clustered = 0x00000040,
			Quad = 0x00000080,
			PartitionedNV = 0x00000100,

			ALL = 0x7FFFFFFF
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
			DisableGPUTimeout = 0x1,
			InitMicroprofile = 0x2,

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

		enum class SampleCount
		{
			SampleCount1 = 1,
			SampleCount2 = 2,
			SampleCount4 = 4,
			SampleCount8 = 8,
			SampleCount16 = 16
		};

		enum class ImageFormat
		{
			Undefined = 0,
			R1_UNORM = 1,
			R2_UNORM = 2,
			R4_UNORM = 3,
			R4G4_UNORM = 4,
			G4R4_UNORM = 5,
			A8_UNORM = 6,
			R8_UNORM = 7,
			R8_SNORM = 8,
			R8_UINT = 9,
			R8_SINT = 10,
			R8_SRGB = 11,
			B2G3R3_UNORM = 12,
			R4G4B4A4_UNORM = 13,
			R4G4B4X4_UNORM = 14,
			B4G4R4A4_UNORM = 15,
			B4G4R4X4_UNORM = 16,
			A4R4G4B4_UNORM = 17,
			X4R4G4B4_UNORM = 18,
			A4B4G4R4_UNORM = 19,
			X4B4G4R4_UNORM = 20,
			R5G6B5_UNORM = 21,
			B5G6R5_UNORM = 22,
			R5G5B5A1_UNORM = 23,
			B5G5R5A1_UNORM = 24,
			A1B5G5R5_UNORM = 25,
			A1R5G5B5_UNORM = 26,
			R5G5B5X1_UNORM = 27,
			B5G5R5X1_UNORM = 28,
			X1R5G5B5_UNORM = 29,
			X1B5G5R5_UNORM = 30,
			B2G3R3A8_UNORM = 31,
			R8G8_UNORM = 32,
			R8G8_SNORM = 33,
			G8R8_UNORM = 34,
			G8R8_SNORM = 35,
			R8G8_UINT = 36,
			R8G8_SINT = 37,
			R8G8_SRGB = 38,
			R16_UNORM = 39,
			R16_SNORM = 40,
			R16_UINT = 41,
			R16_SINT = 42,
			R16_SFLOAT = 43,
			R16_SBFLOAT = 44,
			R8G8B8_UNORM = 45,
			R8G8B8_SNORM = 46,
			R8G8B8_UINT = 47,
			R8G8B8_SINT = 48,
			R8G8B8_SRGB = 49,
			B8G8R8_UNORM = 50,
			B8G8R8_SNORM = 51,
			B8G8R8_UINT = 52,
			B8G8R8_SINT = 53,
			B8G8R8_SRGB = 54,
			R8G8B8A8_UNORM = 55,
			R8G8B8A8_SNORM = 56,
			R8G8B8A8_UINT = 57,
			R8G8B8A8_SINT = 58,
			R8G8B8A8_SRGB = 59,
			B8G8R8A8_UNORM = 60,
			B8G8R8A8_SNORM = 61,
			B8G8R8A8_UINT = 62,
			B8G8R8A8_SINT = 63,
			B8G8R8A8_SRGB = 64,
			R8G8B8X8_UNORM = 65,
			B8G8R8X8_UNORM = 66,
			R16G16_UNORM = 67,
			G16R16_UNORM = 68,
			R16G16_SNORM = 69,
			G16R16_SNORM = 70,
			R16G16_UINT = 71,
			R16G16_SINT = 72,
			R16G16_SFLOAT = 73,
			R16G16_SBFLOAT = 74,
			R32_UINT = 75,
			R32_SINT = 76,
			R32_SFLOAT = 77,
			A2R10G10B10_UNORM = 78,
			A2R10G10B10_UINT = 79,
			A2R10G10B10_SNORM = 80,
			A2R10G10B10_SINT = 81,
			A2B10G10R10_UNORM = 82,
			A2B10G10R10_UINT = 83,
			A2B10G10R10_SNORM = 84,
			A2B10G10R10_SINT = 85,
			R10G10B10A2_UNORM = 86,
			R10G10B10A2_UINT = 87,
			R10G10B10A2_SNORM = 88,
			R10G10B10A2_SINT = 89,
			B10G10R10A2_UNORM = 90,
			B10G10R10A2_UINT = 91,
			B10G10R10A2_SNORM = 92,
			B10G10R10A2_SINT = 93,
			B10G11R11_UFLOAT = 94,
			E5B9G9R9_UFLOAT = 95,
			R16G16B16_UNORM = 96,
			R16G16B16_SNORM = 97,
			R16G16B16_UINT = 98,
			R16G16B16_SINT = 99,
			R16G16B16_SFLOAT = 100,
			R16G16B16_SBFLOAT = 101,
			R16G16B16A16_UNORM = 102,
			R16G16B16A16_SNORM = 103,
			R16G16B16A16_UINT = 104,
			R16G16B16A16_SINT = 105,
			R16G16B16A16_SFLOAT = 106,
			R16G16B16A16_SBFLOAT = 107,
			R32G32_UINT = 108,
			R32G32_SINT = 109,
			R32G32_SFLOAT = 110,
			R32G32B32_UINT = 111,
			R32G32B32_SINT = 112,
			R32G32B32_SFLOAT = 113,
			R32G32B32A32_UINT = 114,
			R32G32B32A32_SINT = 115,
			R32G32B32A32_SFLOAT = 116,
			R64_UINT = 117,
			R64_SINT = 118,
			R64_SFLOAT = 119,
			R64G64_UINT = 120,
			R64G64_SINT = 121,
			R64G64_SFLOAT = 122,
			R64G64B64_UINT = 123,
			R64G64B64_SINT = 124,
			R64G64B64_SFLOAT = 125,
			R64G64B64A64_UINT = 126,
			R64G64B64A64_SINT = 127,
			R64G64B64A64_SFLOAT = 128,
			D16_UNORM = 129,
			X8_D24_UNORM = 130,
			D32_SFLOAT = 131,
			S8_UINT = 132,
			D16_UNORM_S8_UINT = 133,
			D24_UNORM_S8_UINT = 134,
			D32_SFLOAT_S8_UINT = 135,
			DXBC1_RGB_UNORM = 136,
			DXBC1_RGB_SRGB = 137,
			DXBC1_RGBA_UNORM = 138,
			DXBC1_RGBA_SRGB = 139,
			DXBC2_UNORM = 140,
			DXBC2_SRGB = 141,
			DXBC3_UNORM = 142,
			DXBC3_SRGB = 143,
			DXBC4_UNORM = 144,
			DXBC4_SNORM = 145,
			DXBC5_UNORM = 146,
			DXBC5_SNORM = 147,
			DXBC6H_UFLOAT = 148,
			DXBC6H_SFLOAT = 149,
			DXBC7_UNORM = 150,
			DXBC7_SRGB = 151,
			PVRTC1_2BPP_UNORM = 152,
			PVRTC1_4BPP_UNORM = 153,
			PVRTC2_2BPP_UNORM = 154,
			PVRTC2_4BPP_UNORM = 155,
			PVRTC1_2BPP_SRGB = 156,
			PVRTC1_4BPP_SRGB = 157,
			PVRTC2_2BPP_SRGB = 158,
			PVRTC2_4BPP_SRGB = 159,
			ETC2_R8G8B8_UNORM = 160,
			ETC2_R8G8B8_SRGB = 161,
			ETC2_R8G8B8A1_UNORM = 162,
			ETC2_R8G8B8A1_SRGB = 163,
			ETC2_R8G8B8A8_UNORM = 164,
			ETC2_R8G8B8A8_SRGB = 165,
			ETC2_EAC_R11_UNORM = 166,
			ETC2_EAC_R11_SNORM = 167,
			ETC2_EAC_R11G11_UNORM = 168,
			ETC2_EAC_R11G11_SNORM = 169,
			ASTC_4x4_UNORM = 170,
			ASTC_4x4_SRGB = 171,
			ASTC_5x4_UNORM = 172,
			ASTC_5x4_SRGB = 173,
			ASTC_5x5_UNORM = 174,
			ASTC_5x5_SRGB = 175,
			ASTC_6x5_UNORM = 176,
			ASTC_6x5_SRGB = 177,
			ASTC_6x6_UNORM = 178,
			ASTC_6x6_SRGB = 179,
			ASTC_8x5_UNORM = 180,
			ASTC_8x5_SRGB = 181,
			ASTC_8x6_UNORM = 182,
			ASTC_8x6_SRGB = 183,
			ASTC_8x8_UNORM = 184,
			ASTC_8x8_SRGB = 185,
			ASTC_10x5_UNORM = 186,
			ASTC_10x5_SRGB = 187,
			ASTC_10x6_UNORM = 188,
			ASTC_10x6_SRGB = 189,
			ASTC_10x8_UNORM = 190,
			ASTC_10x8_SRGB = 191,
			ASTC_10x10_UNORM = 192,
			ASTC_10x10_SRGB = 193,
			ASTC_12x10_UNORM = 194,
			ASTC_12x10_SRGB = 195,
			ASTC_12x12_UNORM = 196,
			ASTC_12x12_SRGB = 197,
			CLUT_P4 = 198,
			CLUT_P4A4 = 199,
			CLUT_P8 = 200,
			CLUT_P8A8 = 201
		};

		enum class TextureCreationFlags
		{
			//Default flag (Texture will use default allocation strategy decided by the API specific allocator)
			None = 0,
			//Texture will allocate its own memory (COMMITTED resource)
			OwnMemory = 0x01,
			//Texture will be allocated in memory which can be shared among multiple GPUs
			ExportAdapter = 0x04,
			//Use ESRAM to store this texture
			ESRAM = 0x10,
			//Use on-tile memory to store this texture
			OnTile = 0x20,
			//Force 2D instead of automatically determining dimension based on width, height, depth
			Force2D = 0x40,
			//Force 3D instead of automatically determining dimension based on width, height, depth
			Force3D = 0x80,
			//Display target
			AllowDisplayTarget = 0x100,
			//Create an sRGB texture
			SRGB = 0x200
		};

		enum class ResourceState
		{
			Undefined = 0,
			VertexAndConstantBuffer = 0x1,
			IndexBuffer = 0x2,
			RenderTarget = 0x4,
			UnorderedAccess = 0x8,
			DepthWrite = 0x10,
			DepthRead = 0x20,
			NonPixelShaderResource = 0x40,
			PixelShaderResource = 0x80,
			ShaderResource = 0x40 | 0x80,
			StreamOut = 0x100,
			IndirectArgument = 0x200,
			CopyDestination = 0x400,
			CopySource = 0x800,
			GenericRead = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
			Present = 0x1000,
			Common = 0x2000,
			RayTracingAccelerationStructure = 0x4000
		};

		enum class DescriptorType
		{
			Undefined = 0,
			Sampler = 0x01,
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
			RWTexelBuffer = (TexelBuffer << 1)
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
			//Default flag (Buffer will use aliased memory, buffer will not be CPU accessible until MapBuffer is called)
			None = 0x01,
			//Buffer will allocate its own memory (COMMITTED resource)
			OnwMemory = 0x02,
			//Buffer will be persistently mapped
			PersistentMap = 0x04,
			//Use ESRAM to store this buffer
			ESRAM = 0x08,
			//Flag to specify not to allocate descriptors for the resoruce
			NoDescriptorViewCreation = 0x10
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
			Vertex = 0x00000001,
			TessellationControl = 0x00000002,
			TessellationEvaluation = 0x00000004,
			Geometry = 0x00000008,
			Fragment = 0x00000010,
			Compute = 0x00000020,
			RayTracing = 0x00000040,
			
			AllGraphics = (static_cast<uint32_t>(Vertex) | static_cast<uint32_t>(TessellationControl) |
			               static_cast<uint32_t>(TessellationEvaluation) | static_cast<uint32_t>(Geometry) |
				           static_cast<uint32_t>(Fragment)),
			Hull = TessellationControl,
			Domain = TessellationEvaluation,

			SHADER_STAGE_COUNT = 7
		};
		
		union ClearValue
		{
			struct
			{
				float R;
				float G;
				float B;
				float A;
			};
			struct
			{
				float Depth;
				uint32_t Stencil;
			};
		};
		
		struct RenderTargetDesc
		{
			//Texture creation flags (decides memory allocation strategy, sharing access, ...)
			TextureCreationFlags Flags;
			//Width
			uint32_t Width;
			//Height
			uint32_t Height;
			//Depth (should be 1 if type is not TextureType::TextureType2D)
			uint32_t Depth;
			//Texture array size (should be 1 if texture is not a texture array or cubemap)
			uint32_t ArraySize;
			//Number of mip levels
			uint32_t MipLevels;
			//Multisample anti-aliasing (MSAA)
			SampleCount SampleCount;
			//Internal image format
			ImageFormat Format;
			//What state will the texture get created in
			ResourceState StartState;
			//Optimized clear value (recommended to use the same value when clearing the renderTarget)
			ClearValue ClearValue;
			//The image quality level.
			//The higher the quality, the lower the performance.
			//The valid range is between 0 and the value appropriate for SampleCount
			uint32_t SampleQuality;
			//Descriptor creation
			DescriptorType Descriptors;
			//Debug name used in GPU profile
			const char* Name;
		};

		//Data structure holding necessary info to create a Texture
		struct TextureDesc
		{
			//Texture creation flags (decides memory allocation strategy, sharing access, ...)
			TextureCreationFlags Flags;
			//Width
			uint32_t Width;
			//Height;
			uint32_t Height;
			//Depth (should be 1 if Type is not TextureType::TextureType2D)
			uint32_t Depth;
			//Texture array size (should be 1 if texture is not a texture array or cubemap)
			uint32_t ArraySize;
			//Number of mip levels
			uint32_t MipLevels;
			//Number of multisamples per pixel (currently Textures created with Usage TextureUsage::SampledImage only support SampleCount1).
			SampleCount SampleCount;
			//The image quality level.
			//The higher the quality, the lower the performance.
			//The valid range is between 0 and the value appropriate for SampleCount.
			uint32_t SampleQuality;
			//Image format
			ImageFormat Format;
			//Optimized clear value (recommended to use the same value when clearing the renderTarget)
			ClearValue ClearValue;
			//What state will the texture get created in
			ResourceState StartState;
			//Descriptor creation
			DescriptorType Descriptors;
			//Debug name used in GPU profile
			const char* Name;
			//Is the texture CPU accessible (applicable on hardware supporting CPU mapped texture (UMA))
			bool HostVisible;
		};

		//Data structure holding necessary info to create a Buffer
		struct BufferDesc
		{
			//Size of the buffer (in bytes)
			uint64_t Size;
			//Alignment
			uint32_t Alignment;
			//Decides which heap buffer will be used (default, upload, readback)
			ResourceMemoryUsage MemoryUsage;
			//Creation flags of the buffer
			BufferCreationFlags Flags;
			//What type of queue the buffer is owned by
			QueueType QueueType;
			//What state will the buffer get created in
			ResourceState StartState;
			//Index of the first element accessible by the SRV/UAV
			uint64_t FirstElement;
			//Number of elements in the buffer
			uint64_t ElementCount;
			//Size of each element (in bytes) in the buffer
			uint64_t StructStride;
			//ICB draw type
			IndirectArgumentType ICBDrawType;
			//ICB max vertex buffers slots count
			uint32_t ICBMaxVertexBufferBind;
			//ICB max fragment buffers slots count
			uint32_t ICBMaxFragmentBufferBind;
			//Set this to specify a counter buffer for this buffer
			//struct Buffer* CounterBuffer;
			//Format of the buffer
			ImageFormat Format;
			//Flags specifying the suitable usage of this buffer (Uniform Buffer, Vertex Buffer, Index Buffer, ...)
			DescriptorType Descriptors;
			//Debug name used in GPU-profile
			const char* Name;
		};

		struct SamplerDesc
		{
			FilterType MinFilter;
			FilterType MagFilter;
			MipMapMode MipMapMode;
			AddressMode AddressU;
			AddressMode AddressV;
			AddressMode AddressW;
			float MipLodBias;
			float MaxAnisotropy;
			CompareMode CompareFunc;
		};

		struct BinaryShaderStageDesc
		{
			std::vector<uint8_t> ByteCode;
			std::string EntryPoint;
		};

		struct BinaryShaderDesc
		{
			ShaderStage Stages;
			//Specify whether shader will own byte code memory
			bool OwnByteCode;
			BinaryShaderStageDesc Vertex;
			BinaryShaderStageDesc Fragment;
			BinaryShaderStageDesc Geometry;
			BinaryShaderStageDesc TessellationControl;
			BinaryShaderStageDesc TessellationEvaluation;
			BinaryShaderStageDesc Compute;
		};
		
		struct QueueDesc
		{
			QueueType Type;
			QueueFlag Flag;
			QueuePriority Priority;
		};

		struct ReadRange
		{
			uint64_t Offset;
			uint64_t Range;
		};

		/*inline static struct alignsas(64) Renderer
		{
			std::vector<std::pair<std::string, std::string>> BuiltinShaderDefines;
		} Renderer{};*/
		
		inline static struct GPUSettings
		{
			uint32_t UniformBufferAlignment;
			uint32_t UploadBufferTextureAlignment;
			uint32_t UploadBufferTextureRowAlignment;
			uint32_t MaxVertexInputBindings;
			uint32_t MaxRootSignatureDWORDS;
			uint32_t WaveLaneCount;
			WaveOpsSupportFlags WaveOpsSupportFlags;
			uint32_t MultiDrawIndirect;
			uint32_t ROVsSupported;
			uint32_t TessellationSupported;
			uint32_t GeometryShaderSupported;
		} GPUSettings{};

	protected:
		static TRAP::Scope<RendererAPI> s_Renderer;
		static RenderAPI s_RenderAPI;

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

#endif /*_TRAP_RENDERERAPI_H_*/