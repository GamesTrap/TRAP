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
	class CommandPool;
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

namespace TRAP::Graphics::API
{
	class ResourceLoader;
	class VulkanTexture;

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
		enum class FrontFace;
		enum class BlendMode;
		enum class BlendConstant;
		enum class ClearFlags;
		union ClearValue;
	
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
		virtual void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f }, Window* window = nullptr) = 0;

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
		virtual void SetFrontFace(FrontFace face, Window* window = nullptr) = 0;
		virtual void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha, Window* window = nullptr) = 0;
		virtual void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
			                          BlendConstant destinationRGB, BlendConstant destinationAlpha,
									  Window* window = nullptr) = 0;

		virtual void Clear(ClearFlags clear, ClearValue value, Window* window = nullptr) = 0;
		
		//CommandBuffer Stuff
		virtual void SetViewport(uint32_t x,
		                         uint32_t y,
		                         uint32_t width,
		                         uint32_t height,
		                         float minDepth = 0.0f,
		                         float maxDepth = 1.0f,
		                         Window* window = nullptr) = 0;
		virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Window* window = nullptr) = 0;
		
		//TODO More CommandBuffer Stuff
		virtual void Draw(uint32_t vertexCount, uint32_t firstVertex = 0, Window* window = nullptr) = 0;
		
		virtual void DrawIndexed(const Scope<VertexArray>& vertexArray, uint32_t indexCount) = 0;
		virtual void Draw(const Scope<VertexArray>& vertexArray) = 0;

		virtual const std::string& GetTitle() const = 0;

		virtual std::array<uint8_t, 16> GetCurrentGPUUUID() = 0;
		virtual std::string GetCurrentGPUName() = 0;
		virtual std::vector<std::pair<std::string, std::array<uint8_t, 16>>> GetAllGPUs() = 0;

		virtual void InitPerWindowData(Window* window) = 0;
		virtual void RemovePerWindowData(Window* window) = 0;

		virtual CommandBuffer* GetCurrentGraphicCommandBuffer(Window* window) = 0; //TODO Remove
		virtual TRAP::Ref<TRAP::Graphics::SwapChain> GetCurrentSwapChain(Window* window) = 0; //TODO Remove

		virtual void WaitIdle() = 0;

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
		static constexpr uint32_t ImageFormatBitSizeOfBlock(const ImageFormat fmt)
		{
			switch(fmt)
			{
			case ImageFormat::Undefined: return 0;
			case ImageFormat::R1_UNORM: return 8;
			case ImageFormat::R2_UNORM: return 8;
			case ImageFormat::R4_UNORM: return 8;
			case ImageFormat::R4G4_UNORM: return 8;
			case ImageFormat::G4R4_UNORM: return 8;
			case ImageFormat::A8_UNORM: return 8;
			case ImageFormat::R8_UNORM: return 8;
			case ImageFormat::R8_SNORM: return 8;
			case ImageFormat::R8_UINT: return 8;
			case ImageFormat::R8_SINT: return 8;
			case ImageFormat::R8_SRGB: return 8;
			case ImageFormat::B2G3R3_UNORM: return 8;
			case ImageFormat::R4G4B4A4_UNORM: return 16;
			case ImageFormat::R4G4B4X4_UNORM: return 16;
			case ImageFormat::B4G4R4A4_UNORM: return 16;
			case ImageFormat::B4G4R4X4_UNORM: return 16;
			case ImageFormat::A4R4G4B4_UNORM: return 16;
			case ImageFormat::X4R4G4B4_UNORM: return 16;
			case ImageFormat::A4B4G4R4_UNORM: return 16;
			case ImageFormat::X4B4G4R4_UNORM: return 16;
			case ImageFormat::R5G6B5_UNORM: return 16;
			case ImageFormat::B5G6R5_UNORM: return 16;
			case ImageFormat::R5G5B5A1_UNORM: return 16;
			case ImageFormat::B5G5R5A1_UNORM: return 16;
			case ImageFormat::A1B5G5R5_UNORM: return 16;
			case ImageFormat::A1R5G5B5_UNORM: return 16;
			case ImageFormat::R5G5B5X1_UNORM: return 16;
			case ImageFormat::B5G5R5X1_UNORM: return 16;
			case ImageFormat::X1R5G5B5_UNORM: return 16;
			case ImageFormat::X1B5G5R5_UNORM: return 16;
			case ImageFormat::B2G3R3A8_UNORM: return 16;
			case ImageFormat::R8G8_UNORM: return 16;
			case ImageFormat::R8G8_SNORM: return 16;
			case ImageFormat::G8R8_UNORM: return 16;
			case ImageFormat::G8R8_SNORM: return 16;
			case ImageFormat::R8G8_UINT: return 16;
			case ImageFormat::R8G8_SINT: return 16;
			case ImageFormat::R8G8_SRGB: return 16;
			case ImageFormat::R16_UNORM: return 16;
			case ImageFormat::R16_SNORM: return 16;
			case ImageFormat::R16_UINT: return 16;
			case ImageFormat::R16_SINT: return 16;
			case ImageFormat::R16_SFLOAT: return 16;
			case ImageFormat::R16_SBFLOAT: return 16;
			case ImageFormat::R8G8B8_UNORM: return 24;
			case ImageFormat::R8G8B8_SNORM: return 24;
			case ImageFormat::R8G8B8_UINT: return 24;
			case ImageFormat::R8G8B8_SINT: return 24;
			case ImageFormat::R8G8B8_SRGB: return 24;
			case ImageFormat::B8G8R8_UNORM: return 24;
			case ImageFormat::B8G8R8_SNORM: return 24;
			case ImageFormat::B8G8R8_UINT: return 24;
			case ImageFormat::B8G8R8_SINT: return 24;
			case ImageFormat::B8G8R8_SRGB: return 24;
			case ImageFormat::R16G16B16_UNORM: return 48;
			case ImageFormat::R16G16B16_SNORM: return 48;
			case ImageFormat::R16G16B16_UINT: return 48;
			case ImageFormat::R16G16B16_SINT: return 48;
			case ImageFormat::R16G16B16_SFLOAT: return 48;
			case ImageFormat::R16G16B16_SBFLOAT: return 48;
			case ImageFormat::R16G16B16A16_UNORM: return 64;
			case ImageFormat::R16G16B16A16_SNORM: return 64;
			case ImageFormat::R16G16B16A16_UINT: return 64;
			case ImageFormat::R16G16B16A16_SINT: return 64;
			case ImageFormat::R16G16B16A16_SFLOAT: return 64;
			case ImageFormat::R16G16B16A16_SBFLOAT: return 64;
			case ImageFormat::R32G32_UINT: return 64;
			case ImageFormat::R32G32_SINT: return 64;
			case ImageFormat::R32G32_SFLOAT: return 64;
			case ImageFormat::R32G32B32_UINT: return 96;
			case ImageFormat::R32G32B32_SINT: return 96;
			case ImageFormat::R32G32B32_SFLOAT: return 96;
			case ImageFormat::R32G32B32A32_UINT: return 128;
			case ImageFormat::R32G32B32A32_SINT: return 128;
			case ImageFormat::R32G32B32A32_SFLOAT: return 128;
			case ImageFormat::R64_UINT: return 64;
			case ImageFormat::R64_SINT: return 64;
			case ImageFormat::R64_SFLOAT: return 64;
			case ImageFormat::R64G64_UINT: return 128;
			case ImageFormat::R64G64_SINT: return 128;
			case ImageFormat::R64G64_SFLOAT: return 128;
			case ImageFormat::R64G64B64_UINT: return 192;
			case ImageFormat::R64G64B64_SINT: return 192;
			case ImageFormat::R64G64B64_SFLOAT: return 192;
			case ImageFormat::R64G64B64A64_UINT: return 256;
			case ImageFormat::R64G64B64A64_SINT: return 256;
			case ImageFormat::R64G64B64A64_SFLOAT: return 256;
			case ImageFormat::D16_UNORM: return 16;
			case ImageFormat::S8_UINT: return 8;
			case ImageFormat::D32_SFLOAT_S8_UINT: return 64;
			case ImageFormat::DXBC1_RGB_UNORM: return 64;
			case ImageFormat::DXBC1_RGB_SRGB: return 64;
			case ImageFormat::DXBC1_RGBA_UNORM: return 64;
			case ImageFormat::DXBC1_RGBA_SRGB: return 64;
			case ImageFormat::DXBC2_UNORM: return 128;
			case ImageFormat::DXBC2_SRGB: return 128;
			case ImageFormat::DXBC3_UNORM: return 128;
			case ImageFormat::DXBC3_SRGB: return 128;
			case ImageFormat::DXBC4_UNORM: return 64;
			case ImageFormat::DXBC4_SNORM: return 64;
			case ImageFormat::DXBC5_UNORM: return 128;
			case ImageFormat::DXBC5_SNORM: return 128;
			case ImageFormat::DXBC6H_UFLOAT: return 128;
			case ImageFormat::DXBC6H_SFLOAT: return 128;
			case ImageFormat::DXBC7_UNORM: return 128;
			case ImageFormat::DXBC7_SRGB: return 128;
			case ImageFormat::PVRTC1_2BPP_UNORM: return 64;
			case ImageFormat::PVRTC1_4BPP_UNORM: return 64;
			case ImageFormat::PVRTC2_2BPP_UNORM: return 64;
			case ImageFormat::PVRTC2_4BPP_UNORM: return 64;
			case ImageFormat::PVRTC1_2BPP_SRGB: return 64;
			case ImageFormat::PVRTC1_4BPP_SRGB: return 64;
			case ImageFormat::PVRTC2_2BPP_SRGB: return 64;
			case ImageFormat::PVRTC2_4BPP_SRGB: return 64;
			case ImageFormat::ETC2_R8G8B8_UNORM: return 64;
			case ImageFormat::ETC2_R8G8B8_SRGB: return 64;
			case ImageFormat::ETC2_R8G8B8A1_UNORM: return 64;
			case ImageFormat::ETC2_R8G8B8A1_SRGB: return 64;
			case ImageFormat::ETC2_R8G8B8A8_UNORM: return 64;
			case ImageFormat::ETC2_R8G8B8A8_SRGB: return 64;
			case ImageFormat::ETC2_EAC_R11_UNORM: return 64;
			case ImageFormat::ETC2_EAC_R11_SNORM: return 64;
			case ImageFormat::ETC2_EAC_R11G11_UNORM: return 64;
			case ImageFormat::ETC2_EAC_R11G11_SNORM: return 64;
			case ImageFormat::ASTC_4x4_UNORM: return 128;
			case ImageFormat::ASTC_4x4_SRGB: return 128;
			case ImageFormat::ASTC_5x4_UNORM: return 128;
			case ImageFormat::ASTC_5x4_SRGB: return 128;
			case ImageFormat::ASTC_5x5_UNORM: return 128;
			case ImageFormat::ASTC_5x5_SRGB: return 128;
			case ImageFormat::ASTC_6x5_UNORM: return 128;
			case ImageFormat::ASTC_6x5_SRGB: return 128;
			case ImageFormat::ASTC_6x6_UNORM: return 128;
			case ImageFormat::ASTC_6x6_SRGB: return 128;
			case ImageFormat::ASTC_8x5_UNORM: return 128;
			case ImageFormat::ASTC_8x5_SRGB: return 128;
			case ImageFormat::ASTC_8x6_UNORM: return 128;
			case ImageFormat::ASTC_8x6_SRGB: return 128;
			case ImageFormat::ASTC_8x8_UNORM: return 128;
			case ImageFormat::ASTC_8x8_SRGB: return 128;
			case ImageFormat::ASTC_10x5_UNORM: return 128;
			case ImageFormat::ASTC_10x5_SRGB: return 128;
			case ImageFormat::ASTC_10x6_UNORM: return 128;
			case ImageFormat::ASTC_10x6_SRGB: return 128;
			case ImageFormat::ASTC_10x8_UNORM: return 128;
			case ImageFormat::ASTC_10x8_SRGB: return 128;
			case ImageFormat::ASTC_10x10_UNORM: return 128;
			case ImageFormat::ASTC_10x10_SRGB: return 128;
			case ImageFormat::ASTC_12x10_UNORM: return 128;
			case ImageFormat::ASTC_12x10_SRGB: return 128;
			case ImageFormat::ASTC_12x12_UNORM: return 128;
			case ImageFormat::ASTC_12x12_SRGB: return 128;
			case ImageFormat::CLUT_P4: return 8;
			case ImageFormat::CLUT_P4A4: return 8;
			case ImageFormat::CLUT_P8: return 8;
			case ImageFormat::CLUT_P8A8: return 16;
			case ImageFormat::G16B16G16R16_422_UNORM: return 8;
			case ImageFormat::B16G16R16G16_422_UNORM: return 8;
			case ImageFormat::R12X4G12X4B12X4A12X4_UNORM_4PACK16: return 8;
			case ImageFormat::G12X4B12X4G12X4R12X4_422_UNORM_4PACK16: return 8;
			case ImageFormat::B12X4G12X4R12X4G12X4_422_UNORM_4PACK16: return 8;
			case ImageFormat::R10X6G10X6B10X6A10X6_UNORM_4PACK16: return 8;
			case ImageFormat::G10X6B10X6G10X6R10X6_422_UNORM_4PACK16: return 8;
			case ImageFormat::B10X6G10X6R10X6G10X6_422_UNORM_4PACK16: return 8;
			case ImageFormat::G8B8G8R8_422_UNORM: return 4;
			case ImageFormat::B8G8R8G8_422_UNORM: return 4;
			default: return 32;
			}
		}
		static constexpr uint32_t ImageFormatWidthOfBlock(const ImageFormat fmt)
		{
			switch(fmt)
			{
			case ImageFormat::Undefined: return 1;
			case ImageFormat::R1_UNORM: return 8;
			case ImageFormat::R2_UNORM: return 4;
			case ImageFormat::R4_UNORM: return 2;
			case ImageFormat::DXBC1_RGB_UNORM: return 4;
			case ImageFormat::DXBC1_RGB_SRGB: return 4;
			case ImageFormat::DXBC1_RGBA_UNORM: return 4;
			case ImageFormat::DXBC1_RGBA_SRGB: return 4;
			case ImageFormat::DXBC2_UNORM: return 4;
			case ImageFormat::DXBC2_SRGB: return 4;
			case ImageFormat::DXBC3_UNORM: return 4;
			case ImageFormat::DXBC3_SRGB: return 4;
			case ImageFormat::DXBC4_UNORM: return 4;
			case ImageFormat::DXBC4_SNORM: return 4;
			case ImageFormat::DXBC5_UNORM: return 4;
			case ImageFormat::DXBC5_SNORM: return 4;
			case ImageFormat::DXBC6H_UFLOAT: return 4;
			case ImageFormat::DXBC6H_SFLOAT: return 4;
			case ImageFormat::DXBC7_UNORM: return 4;
			case ImageFormat::DXBC7_SRGB: return 4;
			case ImageFormat::PVRTC1_2BPP_UNORM: return 8;
			case ImageFormat::PVRTC1_4BPP_UNORM: return 4;
			case ImageFormat::PVRTC2_2BPP_UNORM: return 8;
			case ImageFormat::PVRTC2_4BPP_UNORM: return 4;
			case ImageFormat::PVRTC1_2BPP_SRGB: return 8;
			case ImageFormat::PVRTC1_4BPP_SRGB: return 4;
			case ImageFormat::PVRTC2_2BPP_SRGB: return 8;
			case ImageFormat::PVRTC2_4BPP_SRGB: return 4;
			case ImageFormat::ETC2_R8G8B8_UNORM: return 4;
			case ImageFormat::ETC2_R8G8B8_SRGB: return 4;
			case ImageFormat::ETC2_R8G8B8A1_UNORM: return 4;
			case ImageFormat::ETC2_R8G8B8A1_SRGB: return 4;
			case ImageFormat::ETC2_R8G8B8A8_UNORM: return 4;
			case ImageFormat::ETC2_R8G8B8A8_SRGB: return 4;
			case ImageFormat::ETC2_EAC_R11_UNORM: return 4;
			case ImageFormat::ETC2_EAC_R11_SNORM: return 4;
			case ImageFormat::ETC2_EAC_R11G11_UNORM: return 4;
			case ImageFormat::ETC2_EAC_R11G11_SNORM: return 4;
			case ImageFormat::ASTC_4x4_UNORM: return 4;
			case ImageFormat::ASTC_4x4_SRGB: return 4;
			case ImageFormat::ASTC_5x4_UNORM: return 5;
			case ImageFormat::ASTC_5x4_SRGB: return 5;
			case ImageFormat::ASTC_5x5_UNORM: return 5;
			case ImageFormat::ASTC_5x5_SRGB: return 5;
			case ImageFormat::ASTC_6x5_UNORM: return 6;
			case ImageFormat::ASTC_6x5_SRGB: return 6;
			case ImageFormat::ASTC_6x6_UNORM: return 6;
			case ImageFormat::ASTC_6x6_SRGB: return 6;
			case ImageFormat::ASTC_8x5_UNORM: return 8;
			case ImageFormat::ASTC_8x5_SRGB: return 8;
			case ImageFormat::ASTC_8x6_UNORM: return 8;
			case ImageFormat::ASTC_8x6_SRGB: return 8;
			case ImageFormat::ASTC_8x8_UNORM: return 8;
			case ImageFormat::ASTC_8x8_SRGB: return 8;
			case ImageFormat::ASTC_10x5_UNORM: return 10;
			case ImageFormat::ASTC_10x5_SRGB: return 10;
			case ImageFormat::ASTC_10x6_UNORM: return 10;
			case ImageFormat::ASTC_10x6_SRGB: return 10;
			case ImageFormat::ASTC_10x8_UNORM: return 10;
			case ImageFormat::ASTC_10x8_SRGB: return 10;
			case ImageFormat::ASTC_10x10_UNORM: return 10;
			case ImageFormat::ASTC_10x10_SRGB: return 10;
			case ImageFormat::ASTC_12x10_UNORM: return 12;
			case ImageFormat::ASTC_12x10_SRGB: return 12;
			case ImageFormat::ASTC_12x12_UNORM: return 12;
			case ImageFormat::ASTC_12x12_SRGB: return 12;
			case ImageFormat::CLUT_P4: return 2;
			default: return 1;
			}
		}
		static constexpr uint32_t ImageFormatHeightOfBlock(const ImageFormat fmt)
		{
			switch(fmt)
			{
			case ImageFormat::Undefined: return 1;
			case ImageFormat::DXBC1_RGB_UNORM: return 4;
			case ImageFormat::DXBC1_RGB_SRGB: return 4;
			case ImageFormat::DXBC1_RGBA_UNORM: return 4;
			case ImageFormat::DXBC1_RGBA_SRGB: return 4;
			case ImageFormat::DXBC2_UNORM: return 4;
			case ImageFormat::DXBC2_SRGB: return 4;
			case ImageFormat::DXBC3_UNORM: return 4;
			case ImageFormat::DXBC3_SRGB: return 4;
			case ImageFormat::DXBC4_UNORM: return 4;
			case ImageFormat::DXBC4_SNORM: return 4;
			case ImageFormat::DXBC5_UNORM: return 4;
			case ImageFormat::DXBC5_SNORM: return 4;
			case ImageFormat::DXBC6H_UFLOAT: return 4;
			case ImageFormat::DXBC6H_SFLOAT: return 4;
			case ImageFormat::DXBC7_UNORM: return 4;
			case ImageFormat::DXBC7_SRGB: return 4;
			case ImageFormat::PVRTC1_2BPP_UNORM: return 4;
			case ImageFormat::PVRTC1_4BPP_UNORM: return 4;
			case ImageFormat::PVRTC2_2BPP_UNORM: return 4;
			case ImageFormat::PVRTC2_4BPP_UNORM: return 4;
			case ImageFormat::PVRTC1_2BPP_SRGB: return 4;
			case ImageFormat::PVRTC1_4BPP_SRGB: return 4;
			case ImageFormat::PVRTC2_2BPP_SRGB: return 4;
			case ImageFormat::PVRTC2_4BPP_SRGB: return 4;
			case ImageFormat::ETC2_R8G8B8_UNORM: return 4;
			case ImageFormat::ETC2_R8G8B8_SRGB: return 4;
			case ImageFormat::ETC2_R8G8B8A1_UNORM: return 4;
			case ImageFormat::ETC2_R8G8B8A1_SRGB: return 4;
			case ImageFormat::ETC2_R8G8B8A8_UNORM: return 4;
			case ImageFormat::ETC2_R8G8B8A8_SRGB: return 4;
			case ImageFormat::ETC2_EAC_R11_UNORM: return 4;
			case ImageFormat::ETC2_EAC_R11_SNORM: return 4;
			case ImageFormat::ETC2_EAC_R11G11_UNORM: return 4;
			case ImageFormat::ETC2_EAC_R11G11_SNORM: return 4;
			case ImageFormat::ASTC_4x4_UNORM: return 4;
			case ImageFormat::ASTC_4x4_SRGB: return 4;
			case ImageFormat::ASTC_5x4_UNORM: return 4;
			case ImageFormat::ASTC_5x4_SRGB: return 4;
			case ImageFormat::ASTC_5x5_UNORM: return 5;
			case ImageFormat::ASTC_5x5_SRGB: return 5;
			case ImageFormat::ASTC_6x5_UNORM: return 5;
			case ImageFormat::ASTC_6x5_SRGB: return 5;
			case ImageFormat::ASTC_6x6_UNORM: return 6;
			case ImageFormat::ASTC_6x6_SRGB: return 6;
			case ImageFormat::ASTC_8x5_UNORM: return 5;
			case ImageFormat::ASTC_8x5_SRGB: return 5;
			case ImageFormat::ASTC_8x6_UNORM: return 6;
			case ImageFormat::ASTC_8x6_SRGB: return 6;
			case ImageFormat::ASTC_8x8_UNORM: return 8;
			case ImageFormat::ASTC_8x8_SRGB: return 8;
			case ImageFormat::ASTC_10x5_UNORM: return 5;
			case ImageFormat::ASTC_10x5_SRGB: return 5;
			case ImageFormat::ASTC_10x6_UNORM: return 6;
			case ImageFormat::ASTC_10x6_SRGB: return 6;
			case ImageFormat::ASTC_10x8_UNORM: return 8;
			case ImageFormat::ASTC_10x8_SRGB: return 8;
			case ImageFormat::ASTC_10x10_UNORM: return 10;
			case ImageFormat::ASTC_10x10_SRGB: return 10;
			case ImageFormat::ASTC_12x10_UNORM: return 10;
			case ImageFormat::ASTC_12x10_SRGB: return 10;
			case ImageFormat::ASTC_12x12_UNORM: return 12;
			case ImageFormat::ASTC_12x12_SRGB: return 12;
			default: return 1;
			}
		}
		static constexpr bool ImageFormatIsPlanar(const ImageFormat fmt)
		{
			switch(fmt)
			{
			case ImageFormat::G8_B8R8_2PLANE_420_UNORM:
			case ImageFormat::G8_B8R8_2PLANE_422_UNORM:
			case ImageFormat::G8_B8_R8_3PLANE_420_UNORM:
			case ImageFormat::G8_B8_R8_3PLANE_422_UNORM:
			case ImageFormat::G8_B8_R8_3PLANE_444_UNORM:
			case ImageFormat::G16_B16R16_2PLANE_420_UNORM:
			case ImageFormat::G16_B16R16_2PLANE_422_UNORM:
			case ImageFormat::G16_B16_R16_3PLANE_420_UNORM:
			case ImageFormat::G16_B16_R16_3PLANE_422_UNORM:
			case ImageFormat::G16_B16_R16_3PLANE_444_UNORM:
			case ImageFormat::G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
			case ImageFormat::G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
			case ImageFormat::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
			case ImageFormat::G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
			case ImageFormat::G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
				return true;
				
			default:
				return false;
			}
		}
		static constexpr uint32_t ImageFormatNumOfPlanes(const ImageFormat fmt)
		{
			switch(fmt)
			{
			case ImageFormat::G8_B8_R8_3PLANE_420_UNORM:
			case ImageFormat::G8_B8_R8_3PLANE_422_UNORM:
			case ImageFormat::G8_B8_R8_3PLANE_444_UNORM:
			case ImageFormat::G16_B16_R16_3PLANE_420_UNORM:
			case ImageFormat::G16_B16_R16_3PLANE_422_UNORM:
			case ImageFormat::G16_B16_R16_3PLANE_444_UNORM:
			case ImageFormat::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
			case ImageFormat::G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
			case ImageFormat::G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
				return 3;

			case ImageFormat::G8_B8R8_2PLANE_420_UNORM:
			case ImageFormat::G8_B8R8_2PLANE_422_UNORM:
			case ImageFormat::G16_B16R16_2PLANE_420_UNORM:
			case ImageFormat::G16_B16R16_2PLANE_422_UNORM:
			case ImageFormat::G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
			case ImageFormat::G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
				return 2;
				
			default:
				return 1;
			}
		}
		static constexpr uint32_t ImageFormatIsSinglePlane(const ImageFormat fmt)
		{
			return !ImageFormatIsPlanar(fmt) || ImageFormatNumOfPlanes(fmt) < 2;
		}
		static constexpr uint32_t ImageFormatPlaneWidth(const ImageFormat fmt, const uint32_t plane, const uint32_t width)
		{
			if (plane == 0)
				return width;

			switch(fmt)
			{
			case ImageFormat::G8_B8_R8_3PLANE_420_UNORM:
			case ImageFormat::G8_B8R8_2PLANE_420_UNORM:
			case ImageFormat::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
			case ImageFormat::G8_B8_R8_3PLANE_422_UNORM:
			case ImageFormat::G8_B8R8_2PLANE_422_UNORM:
			case ImageFormat::G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
			case ImageFormat::G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
			case ImageFormat::G16_B16_R16_3PLANE_420_UNORM:
			case ImageFormat::G16_B16_R16_3PLANE_422_UNORM:
			case ImageFormat::G16_B16R16_2PLANE_420_UNORM:
			case ImageFormat::G16_B16R16_2PLANE_422_UNORM:
				return width >> 1;
				
			default:
				return width;
			}
		}
		static constexpr uint32_t ImageFormatPlaneHeight(const ImageFormat fmt, const uint32_t plane, const uint32_t height)
		{
			if (plane == 0)
				return height;

			switch(fmt)
			{
			case ImageFormat::G8_B8_R8_3PLANE_420_UNORM:
			case ImageFormat::G8_B8R8_2PLANE_420_UNORM:
			case ImageFormat::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
			case ImageFormat::G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
			case ImageFormat::G16_B16_R16_3PLANE_420_UNORM:
			case ImageFormat::G16_B16R16_2PLANE_420_UNORM:
				return height >> 1;
				
			default:
				return height;
			}
		}
		static constexpr uint32_t ImageFormatPlaneSizeOfBlock(const ImageFormat fmt, const uint32_t plane)
		{
			switch(fmt)
			{
			case ImageFormat::G8_B8_R8_3PLANE_420_UNORM:
			case ImageFormat::G8_B8_R8_3PLANE_422_UNORM:
			case ImageFormat::G8_B8_R8_3PLANE_444_UNORM:
				return 1;

			case ImageFormat::G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
			case ImageFormat::G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
			case ImageFormat::G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
			case ImageFormat::G16_B16_R16_3PLANE_420_UNORM:
			case ImageFormat::G16_B16_R16_3PLANE_422_UNORM:
			case ImageFormat::G16_B16_R16_3PLANE_444_UNORM:
			case ImageFormat::G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
				return 2;

			case ImageFormat::G8_B8R8_2PLANE_420_UNORM:
			case ImageFormat::G8_B8R8_2PLANE_422_UNORM:
				return 0 == plane ? 1 : 2;

			case ImageFormat::G16_B16R16_2PLANE_420_UNORM:
			case ImageFormat::G16_B16R16_2PLANE_422_UNORM:
			case ImageFormat::G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
			case ImageFormat::G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
			case ImageFormat::G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
				return 0 == plane ? 2 : 4;
				
			default:
				return 1;
			}
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
			CLUT_P8A8 = 201,
			R4G4B4A4_UNORM_PACK16 = 202,
			B4G4R4A4_UNORM_PACK16 = 203,
			R5G6B5_UNORM_PACK16 = 204,
			B5G6R5_UNORM_PACK16 = 205,
			R5G5B5A1_UNORM_PACK16 = 206,
			B5G5R5A1_UNORM_PACK16 = 207,
			A1R5G5B5_UNORM_PACK16 = 208,
			G16B16G16R16_422_UNORM = 209,
			B16G16R16G16_422_UNORM = 210,
			R12X4G12X4B12X4A12X4_UNORM_4PACK16 = 211,
			G12X4B12X4G12X4R12X4_422_UNORM_4PACK16 = 212,
			B12X4G12X4R12X4G12X4_422_UNORM_4PACK16 = 213,
			R10X6G10X6B10X6A10X6_UNORM_4PACK16 = 214,
			G10X6B10X6G10X6R10X6_422_UNORM_4PACK16 = 215,
			B10X6G10X6R10X6G10X6_422_UNORM_4PACK16 = 216,
			G8B8G8R8_422_UNORM = 217,
			B8G8R8G8_422_UNORM = 218,
			G8_B8_R8_3PLANE_420_UNORM = 219,
			G8_B8R8_2PLANE_420_UNORM = 220,
			G8_B8_R8_3PLANE_422_UNORM = 221,
			G8_B8R8_2PLANE_422_UNORM = 222,
			G8_B8_R8_3PLANE_444_UNORM = 223,
			G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16 = 224,
			G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16 = 225,
			G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16 = 226,
			G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 = 227,
			G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16 = 228,
			G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16 = 229,
			G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16 = 230,
			G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16 = 231,
			G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16 = 232,
			G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16 = 233,
			G16_B16_R16_3PLANE_420_UNORM = 234,
			G16_B16_R16_3PLANE_422_UNORM = 235,
			G16_B16_R16_3PLANE_444_UNORM = 236,
			G16_B16R16_2PLANE_420_UNORM = 237,
			G16_B16R16_2PLANE_422_UNORM = 238,
			
			IMAGE_FORMAT_COUNT = 239
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
			//Default flag (Buffer will use aliased memory, buffer will not be CPU accessible until MapBuffer is called)
			None = 0x01,
			//Buffer will allocate its own memory (COMMITTED resource)
			OnwMemory = 0x02,
			//Buffer will be persistently mapped
			PersistentMap = 0x04,
			//Use ESRAM to store this buffer
			ESRAM = 0x08,
			//Flag to specify not to allocate descriptors for the resource
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

		enum class RootSignatureFlags
		{
			//Default flag
			None = 0,
			//Local root signature used mainly in raytracing shaders
			Local = 0x1
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
			BlendStateTarget0 = 0x1,
			BlendStateTarget1 = 0x2,
			BlendStateTarget2 = 0x4,
			BlendStateTarget3 = 0x8,
			BlendStateTarget4 = 0x10,
			BlendStateTarget5 = 0x20,
			BlendStateTarget6 = 0x40,
			BlendStateTarget7 = 0x80,
			
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
			ExternallySynchronized = 0x1
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
			Depth = 0x1,
			Stencil = 0x2
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
			SampleCount SampleCount{};
			//Internal image format
			ImageFormat Format{};
			//What state will the texture get created in
			ResourceState StartState{};
			//Optimized clear value (recommended to use the same value when clearing the renderTarget)
			ClearValue ClearValue{};
			//The image quality level.
			//The higher the quality, the lower the performance.
			//The valid range is between 0 and the value appropriate for SampleCount
			uint32_t SampleQuality{};
			//Descriptor creation
			DescriptorType Descriptors{};
			//Debug name used in GPU profile
			const char* Name{};

			void* NativeHandle{};
		};

		//Data structure holding necessary info to create a Texture
		struct TextureDesc
		{
			//Texture creation flags (decides memory allocation strategy, sharing access, ...)
			TextureCreationFlags Flags{};
			//Width
			uint32_t Width{};
			//Height;
			uint32_t Height{};
			//Depth (should be 1 if Type is not TextureType::TextureType3D)
			uint32_t Depth{};
			//Texture array size (should be 1 if texture is not a texture array or cubemap)
			uint32_t ArraySize{};
			//Number of mip levels
			uint32_t MipLevels{};
			//Number of multisamples per pixel (currently Textures created with Usage TextureUsage::SampledImage only support SampleCount1).
			SampleCount SampleCount{};
			//The image quality level.
			//The higher the quality, the lower the performance.
			//The valid range is between 0 and the value appropriate for SampleCount.
			uint32_t SampleQuality{};
			//Image format
			ImageFormat Format{};
			//Optimized clear value (recommended to use the same value when clearing the renderTarget)
			ClearValue ClearValue{};
			//What state will the texture get created in
			ResourceState StartState{};
			//Descriptor creation
			DescriptorType Descriptors{};
			//Pointer to native texture handle if the texture does not own underlying resource
			void* NativeHandle{};
			//Debug name used in GPU profile
			const char* Name{};

			VkSamplerYcbcrConversionInfo* VkSamplerYcbcrConversionInfo{};
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
			QueueType QueueType{};
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
			ImageFormat Format{};
			//Flags specifying the suitable usage of this buffer (Uniform Buffer, Vertex Buffer, Index Buffer, ...)
			DescriptorType Descriptors{};
			//Debug name used in GPU-profile
			const char* Name{};
		};

		struct SamplerDesc
		{
			FilterType MinFilter{};
			FilterType MagFilter{};
			MipMapMode MipMapMode{};
			AddressMode AddressU{};
			AddressMode AddressV{};
			AddressMode AddressW{};
			float MipLodBias{};
			float MaxAnisotropy{};
			CompareMode CompareFunc{};

			struct SamplerConversionDesc
			{
				ImageFormat Format;
				SamplerModelConversion Model;
				SamplerRange Range;
				SampleLocation ChromaOffsetX;
				SampleLocation ChromaOffsetY;
				FilterType ChromaFilter;
				bool ForceExplicitReconstruction;
			} SamplerConversionDesc{};
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
			const char* Name{};
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
			TRAP::Ref<RootSignature> RootSignature{};
			DescriptorUpdateFrequency UpdateFrequency{};
			uint32_t MaxSets{};
		};

		struct CommandPoolDesc
		{
			TRAP::Ref<Queue> Queue;
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
			CullMode CullMode{};
			int32_t DepthBias{};
			float SlopeScaledDepthBias{};
			FillMode FillMode{};
			FrontFace FrontFace{};
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
			TRAP::Ref<RootSignature> RootSignature{};
		};

		struct VertexAttribute
		{
			ImageFormat Format{};
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
			TRAP::Ref<RootSignature> RootSignature{};
			TRAP::Ref<VertexLayout> VertexLayout{};
			TRAP::Ref<BlendStateDesc> BlendState{};
			TRAP::Ref<DepthStateDesc> DepthState{};
			TRAP::Ref<RasterizerStateDesc> RasterizerState{};
			std::vector<ImageFormat> ColorFormats{};
			uint32_t RenderTargetCount{};
			SampleCount SampleCount{};
			uint32_t SampleQuality{};
			ImageFormat DepthStencilFormat{};
			PrimitiveTopology PrimitiveTopology{};
			bool SupportIndirectCommandBuffer{};
		};

		struct RayTracingPipelineDesc
		{
		};

		struct PipelineDesc
		{			
			PipelineType Type{};
			std::variant<ComputePipelineDesc, GraphicsPipelineDesc, RayTracingPipelineDesc> Pipeline{};
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
			TRAP::Ref<RootSignature> RootSignature{};
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
			ImageFormat ColorFormat{};
			//Clear value
			ClearValue ColorClearValue{};
			//Set whether swapchain will be presented using VSync
			bool EnableVSync{};
			//We can toggle to using FLIP model if application desires
			bool UseFlipSwapEffect{};
		};

		struct RenderTargetBarrier
		{
			TRAP::Ref<RenderTarget> RenderTarget{};
			ResourceState CurrentState{};
			ResourceState NewState{};
			bool BeginOnly{};
			bool EndOnly{};
			bool Acquire{};
			bool Release{};
			QueueType QueueType{};
			//Specify whether following barrier targets particular subresource
			bool SubresourceBarrier{};
			//Following values are ignored if SubresourceBarrier is false
			uint8_t MipLevel{};
			uint16_t ArrayLayer{};
		};

		struct BufferBarrier
		{
			TRAP::Ref<Buffer> Buffer{};
			ResourceState CurrentState{};
			ResourceState NewState{};
			bool BeginOnly{};
			bool EndOnly{};
			bool Acquire{};
			bool Release{};
			QueueType QueueType{};
		};

		struct TextureBarrier
		{
			TRAP::Ref<API::VulkanTexture> Texture{};
			ResourceState CurrentState{};
			ResourceState NewState{};
			bool BeginOnly{};
			bool EndOnly{};
			bool Acquire{};
			bool Release{};
			QueueType QueueType{};
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
			std::variant<BufferOffset, DescriptorSetExtraction, TextureSlice, bool> Offset{};
			//Array of resources containing descriptor handles or constant to be used in ring buffer memory
			//DescriptorRange can hold only one resource type array
			//std::vector<TRAP::Ref<API::VulkanAccelerationStructure>> AccelerationStructures; //TODO RT
			std::variant<std::vector<TRAP::Ref<API::VulkanTexture>>, std::vector<TRAP::Ref<Sampler>>,
				std::vector<TRAP::Ref<Buffer>>, std::vector<TRAP::Ref<Pipeline>>,
				std::vector<TRAP::Ref<DescriptorSet>>> Resource{};

			//Number of resources in the descriptor(applies to array of textures, buffers, ...)
			uint32_t Count{};
			uint32_t Index = static_cast<uint32_t>(-1);
			bool ExtractBuffer = false;
		};

		struct QueuePresentDesc
		{
			TRAP::Ref<SwapChain> SwapChain{};
			std::vector<TRAP::Ref<Semaphore>> WaitSemaphores{};
			uint8_t Index{};
			bool SubmitDone{};
		};

		struct LoadActionsDesc
		{
			std::array<ClearValue, 8> ClearColorValues{};
			std::array<LoadActionType, 8> LoadActionsColor{};
			ClearValue ClearDepth{};
			LoadActionType LoadActionDepth{};
			LoadActionType LoadActionStencil{};
		};

		struct VirtualTexture
		{
			//Sparse queue binding information
			VkBindSparseInfo BindSparseInfo{};
			//Sparse image memory bindings of all memory-backed virtual tables
			TRAP::Ref<void> SparseImageMemoryBinds{};
			//Sparse queue memory bindings for the mip tail (if present)
			TRAP::Ref<void> OpaqueMemoryBinds{};
			//First mip level in mip tail
			uint32_t MipTailStart{};
			//Lastly filled mip level in mip tail
			uint32_t LastFilledMip{};
			//Memory type for Sparse texture's memory
			uint32_t SparseMemoryTypeIndex{};
			//Sparse image memory bind info
			VkSparseImageMemoryBindInfo ImageMemoryBindInfo{};
			//Sparse image opaque memory bind info (mip tail)
			VkSparseImageOpaqueMemoryBindInfo OpaqueMemoryBindInfo{};

			//Virtual Texture members
			//Contains all virtual pages of the texture
			TRAP::Ref<void> Pages{};
			//Visibility data
			TRAP::Ref<Buffer> Visibility{};
			//PrevVisibility data
			TRAP::Ref<Buffer> PrevVisibility{};
			//Alive Page's Index
			TRAP::Ref<Buffer> AlivePage{};
			//Page's Index which should be removed
			TRAP::Ref<Buffer> RemovePage{};
			//A { uint alive; uint remove; } count of pages which are alive or should be remove
			TRAP::Ref<Buffer> PageCounts{};
			//Original Pixel image data
			std::vector<uint8_t> VirtualImageData{};
			//Total pages count
			uint32_t VirtualPageTotalCount{};
			//Sparse Virtual Texture width
			uint64_t SparseVirtualTexturePageWidth{};
			//Sparse Virtual Texture height
			uint64_t SparseVirtualTexturePageHeight{};
		};

		//Virtual Texture Page as a part of the partially resident Texture
		//Contains memory bindings, offsets and status information
		struct VirtualTexturePage
		{
			//Buffer which contains the image data and be used for copying it to Virtual Texture
			TRAP::Ref<Buffer> IntermediateBuffer{};
			//Mip level for this page
			uint32_t MipLevel{};
			//Array layer for this page
			uint32_t Layer{};
			//Index for this page
			uint32_t Index{};

			//Offset for this page
			VkOffset3D Offset{};
			//Size for this page
			VkExtent3D Extent{};
			//Sparse image memory bind for this page
			VkSparseImageMemoryBind ImageMemoryBind{};
			//Byte size for this page
			VkDeviceSize Size{};
		};

		struct DescriptorIndexMap
		{
			std::unordered_map<std::string, uint32_t> Map;
		};

		struct MappedMemoryRange
		{
			uint8_t* Data;
			TRAP::Ref<Buffer> Buffer;
			uint64_t Offset;
			uint64_t Size;
			uint32_t Flags;
		};

		struct ResourceLoaderDesc
		{
			uint64_t BufferSize;
			uint32_t BufferCount;
		};

		struct BufferLoadDesc
		{
			TRAP::Ref<Buffer> Buffer;
			const void* Data;
			BufferDesc Desc;

			//Force Reset Buffer to nullptr
			bool ForceReset;
		};
		
		struct BufferUpdateDesc
		{
			TRAP::Ref<Buffer> Buffer;
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
			uint32_t UniformBufferAlignment{};
			uint32_t UploadBufferTextureAlignment{};
			uint32_t UploadBufferTextureRowAlignment{};
			uint32_t MaxVertexInputBindings{};
			uint32_t MaxRootSignatureDWORDS{};
			uint32_t WaveLaneCount{};
			WaveOpsSupportFlags WaveOpsSupportFlags{};
			bool MultiDrawIndirect{};
			uint32_t ROVsSupported{};
			uint32_t TessellationSupported{};
			uint32_t GeometryShaderSupported{};
		} GPUSettings{};

	protected:
		static TRAP::Scope<RendererAPI> s_Renderer;
		static RenderAPI s_RenderAPI;
		static TRAP::Scope<API::ResourceLoader> s_ResourceLoader;

		struct PerWindowData
		{
			~PerWindowData();
			
			inline static constexpr uint32_t ImageCount = 3; //Triple Buffered

			Window* Window;
			
			uint32_t ImageIndex = 0;
			TRAP::Ref<Queue> GraphicQueue;
			TRAP::Ref<Queue> ComputeQueue;
			std::array<TRAP::Ref<CommandPool>, ImageCount> GraphicCommandPools;
			std::array<CommandBuffer*, ImageCount> GraphicCommandBuffers;
			std::array<TRAP::Ref<CommandPool>, ImageCount> ComputeCommandPools;
			std::array<CommandBuffer*, ImageCount> ComputeCommandBuffers;
			std::array<TRAP::Ref<Fence>, ImageCount> RenderCompleteFences;
			TRAP::Ref<Semaphore> ImageAcquiredSemaphore;
			std::array<TRAP::Ref<Semaphore>, ImageCount> RenderCompleteSemaphores;
			
			TRAP::Ref<SwapChain> SwapChain;
			uint32_t CurrentSwapChainImageIndex;

			ClearValue ClearColor{0.1f, 0.1f, 0.1f, 1.0f};
			
			bool CurrentVSync;
			bool NewVSync;

			PipelineDesc GraphicsPipelineDesc;
			RootSignatureDesc RootSignatureDesc;
			TRAP::Ref<Pipeline> CurrentGraphicPipeline;

			bool Recording;
		};
		static TRAP::Scope<std::unordered_map<Window*, TRAP::Scope<PerWindowData>>> s_perWindowDataMap;
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

#endif /*_TRAP_RENDERERAPI_H_*/