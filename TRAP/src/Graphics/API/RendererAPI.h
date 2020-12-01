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

		enum class WaveOpsSupportFlags : uint32_t
		{
			None = 0x0,
			Basic_Bit = 0x00000001,
			Vote_Bit = 0x00000002,
			Arithmetic_Bit = 0x00000004,
			Ballot_Bit = 0x00000008,
			Shuffle_Bit = 0x00000010,
			Shuffle_Relative_Bit = 0x00000020,
			Clustered_Bit = 0x00000040,
			Quad_Bit = 0x00000080,
			Partitioned_Bit_NV = 0x00000100,

			ALL = 0x7FFFFFFF
		};

		enum class QueueType
		{
			Graphics = 0,
			Transfer,
			Compute,

			MAX_QUEUE_TYPE
		};

		enum class FenceStatus
		{
			Complete = 0,
			Incomplete,
			NotSubmitted,
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

#endif /*_TRAP_RENDERERAPI_H_*/