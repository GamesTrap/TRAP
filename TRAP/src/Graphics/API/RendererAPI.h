#ifndef TRAP_RENDERERAPI_H
#define TRAP_RENDERERAPI_H

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
	class Texture;
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
	/// <summary>
	/// Supported render APIs.
	/// </summary>
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
		enum class IndexType;
		enum class ClearBufferType;
		enum class ShadingRate;
		enum class ShadingRateCombiner;
		enum class ResourceState;
		enum class QueueType;
		struct LoadActionsDesc;
		struct BufferBarrier;
		struct TextureBarrier;
		struct RenderTargetBarrier;
	//protected:
		struct PerWindowData;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		RendererAPI() = default;
	public:
		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~RendererAPI() = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		RendererAPI(const RendererAPI&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		RendererAPI& operator=(const RendererAPI&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		RendererAPI(RendererAPI&&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		RendererAPI& operator=(RendererAPI&&) = delete;

		/// <summary>
		/// Initialize the Renderer.
		/// </summary>
		/// <param name="gameName">Name of the game.</param>
		/// <param name="renderAPI">Render API to use.</param>
		static void Init(const std::string_view gameName, RenderAPI renderAPI);
		/// <summary>
		/// Shutdown the Renderer.
		/// </summary>
		static void Shutdown();

		/// <summary>
		/// Retrieve the Renderer singleton.
		/// </summary>
		/// <returns>Renderer.</returns>
		static RendererAPI* GetRenderer();
		/// <summary>
		/// Retrieve the resource loader singleton.
		/// </summary>
		/// <returns>Resource loader.</returns>
		static API::ResourceLoader* GetResourceLoader();

		/// <summary>
		/// Auto select a supported render API.
		/// </summary>
		/// <returns>Auto selected render API.</returns>
		static RenderAPI AutoSelectRenderAPI();
		/// <summary>
		/// Check whether a render API is supported by the system.
		/// </summary>
		/// <returns>True if supported, false otherwise.</returns>
		static bool IsSupported(RenderAPI api);
		/// <summary>
		/// Retrieve the currently used render API.
		/// </summary>
		/// <returns>Currently used render API.</returns>
		static RenderAPI GetRenderAPI();

		/// <summary>
		/// Initialize the internal renderer.
		/// </summary>
		/// <param name="gameName">Name of the game.</param>
		virtual void InitInternal(const std::string_view gameName) = 0;

		/// <summary>
		/// Present to the given window.
		/// </summary>
		/// <param name="window">Window to present.</param>
		virtual void Present(Window* window) = 0;

		/// <summary>
		/// Dispatch to the given window.
		/// </summary>
		/// <param name="workGroupElements">
		/// Number of elements to dispatch for each dimension.
		/// The elements are automatically divided by the number of threads in the work group and rounded up.
		/// </param>
		/// <param name="window">Window to Dispatch.</param>
		virtual void Dispatch(std::array<uint32_t, 3> workGroupElements, Window* window) = 0;

		/// <summary>
		/// Set the VSync state for the given window.
		/// </summary>
		/// <param name="vsync">Enable or disable VSync.</param>
		/// <param name="window">Window to set VSync for. Default: Main Window.</param>
		virtual void SetVSync(bool vsync, Window* window = nullptr) = 0;
		/// <summary>
		/// Retrieve whether VSync is enabled or not for the given window.
		/// </summary>
		/// <param name="window">Window to retrieve VSync for. Default: Main Window.</param>
		/// <returns>True if VSync is enabled, false otherwise.</returns>
		virtual bool GetVSync(Window* window = nullptr) = 0;

		//RenderTarget Stuff

		/// <summary>
		/// Set the clear color to be used by the given window.
		/// </summary>
		/// <param name="color">New clear color.</param>
		/// <param name="window">Window to set clear color for. Default: Main Window.</param>
		virtual void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f },
		                           Window* window = nullptr) = 0;
		/// <summary>
		/// Set the clear depth value to be used by the given window.
		/// </summary>
		/// <param name="depth">New clear depth value. Must be between 0.0f and 1.0f</param>
		/// <param name="window">Window to set clear depth value for. Default: Main Window.</param>
		virtual void SetClearDepth(float depth = 1.0f, Window* window = nullptr) = 0;
		/// <summary>
		/// Set the clear stencil value to be used by the given window.
		/// </summary>
		/// <param name="stencil">New clear stencil value.</param>
		/// <param name="window">Window to set clear stencil value for. Default: Main Window.</param>
		virtual void SetClearStencil(uint32_t stencil = 0, Window* window = nullptr) = 0;
#ifdef TRAP_HEADLESS_MODE
		/// <summary>
		/// Set the resolution of the render targets used by the given window.
		///
		/// Note: This function is only available in Headless mode.
		/// </summary>
		/// <param name="width">New width.</param>
		/// <param name="height">New height.</param>
		/// <param name="window">Window to set resolution for. Default: Main Window.</param>
		virtual void SetResolution(uint32_t width, uint32_t height, Window* window = nullptr) = 0;
#endif

		//Pipeline Stuff

		/// <summary>
		/// Enable or disable depth testing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable depth testing.</param>
		/// <param name="window">Window to set depth testing for. Default: Main Window.</param>
		virtual void SetDepthTesting(bool enabled, Window* window = nullptr) = 0;
		/// <summary>
		/// Enable or disable depth writing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable depth writing.</param>
		/// <param name="window">Window to set depth writing for. Default: Main Window.</param>
		virtual void SetDepthWriting(bool enabled, Window* window = nullptr) = 0;
		/// <summary>
		/// Set the depth function for the given window.
		/// </summary>
		/// <param name="function">Function to use for depth testing.</param>
		/// <param name="window">Window to set depth function for. Default: Main Window.</param>
		virtual void SetDepthFunction(CompareMode function, Window* window = nullptr) = 0;
		/// <summary>
		/// Set the depth action to perform when depth testing fails for the given window.
		/// </summary>
		/// <param name="front">Depth action to perform when depth testing fails.</param>
		/// <param name="back">Depth action to perform when depth testing fails.</param>
		/// <param name="window">Window to set the depth fail action for. Default: Main Window.</param>
		virtual void SetDepthFail(StencilOp front, StencilOp back, Window* window = nullptr) = 0;
		/// <summary>
		/// Set the depth bias (scalar factor to add to each fragments depth value) for the given window.
		/// </summary>
		/// <param name="depthBias">Depth bias.</param>
		/// <param name="window">Window to set the depth bias for. Default: Main Window.</param>
		virtual void SetDepthBias(int32_t depthBias, Window* window = nullptr) = 0;
		/// <summary>
		/// Set the depth bias slope factor (scalar factor applied to fragment's slope in depth bias calculation) for the given window.
		/// </summary>
		/// <param name="factor">Depth bias slope factor.</param>
		/// <param name="window">Window to set the depth bias slope factor for. Default: Main Window.</param>
		virtual void SetDepthBiasSlopeFactor(float factor, Window* window = nullptr) = 0;
		/// <summary>
		/// Enable or disable stencil testing for the given window.
		/// </summary>
		/// <param name="enabled">Enable or disable stencil testing.</param>
		/// <param name="window">Window to set stencil testing for. Default: Main Window.</param>
		virtual void SetStencilTesting(bool enabled, Window* window = nullptr) = 0;
		/// <summary>
		/// Set the stencil action to perform when stencil testing fails for the given window.
		/// </summary>
		/// <param name="front">Stencil action to perform when stencil testing fails.</param>
		/// <param name="back">Stencil action to perform when stencil testing fails.</param>
		/// <param name="window">Window to set the stencil fail action for. Default: Main Window.</param>
		virtual void SetStencilFail(StencilOp front, StencilOp back, Window* window = nullptr) = 0;
		/// <summary>
		/// Set the stencil action to perform when stencil testing and depth testing passes for the given window.
		/// </summary>
		/// <param name="front">Stencil action to perform when passed.</param>
		/// <param name="back">Stencil action to perform when passed.</param>
		/// <param name="window">Window to set the stencil pass action for. Default: Main Window.</param>
		virtual void SetStencilPass(StencilOp front, StencilOp back, Window* window = nullptr) = 0;
		/// <summary>
		/// Set the stencil functions for the given window.
		/// </summary>
		/// <param name="front">Function to use on the front for stencil testing.</param>
		/// <param name="back">Function to use on the back for stencil testing.</param>
		/// <param name="window">Window to set stencil functions for. Default: Main Window.</param>
		virtual void SetStencilFunction(CompareMode front, CompareMode back, Window* window = nullptr) = 0;
		/// <summary>
		/// Set the stencil mask for the given window.
		/// </summary>
		/// <param name="read">Select the bits of the stencil values to test.</param>
		/// <param name="write">Select the bits of the stencil values updated by the stencil test.</param>
		/// <param name="window">Window to set stencil mask for. Default: Main Window.</param>
		virtual void SetStencilMask(uint8_t read, uint8_t write, Window* window = nullptr) = 0;
		/// <summary>
		/// Set the cull mode for the given window.
		/// </summary>
		/// <param name="mode">Cull mode to use.</param>
		/// <param name="window">Window to set cull mode for. Default: Main Window.</param>
		virtual void SetCullMode(CullMode mode, Window* window = nullptr) = 0;
		/// <summary>
		/// Set the fill mode for the given window.
		/// </summary>
		/// <param name="mode">Fill mode to use.</param>
		/// <param name="window">Window to set fill mode for. Default: Main Window.</param>
		virtual void SetFillMode(FillMode mode, Window* window = nullptr) = 0;
		/// <summary>
		/// Set the primitive topology for the given window.
		/// </summary>
		/// <param name="topology">Primitive topology to use.</param>
		/// <param name="window">Window to set primitive topology for. Default: Main Window.</param>
		virtual void SetPrimitiveTopology(PrimitiveTopology topology, Window* window = nullptr) = 0;
		/// <summary>
		/// Set the front face winding order for the given window.
		/// </summary>
		/// <param name="face">Front face winding order to use.</param>
		/// <param name="window">Window to set front face winding order for. Default: Main Window.</param>
		virtual void SetFrontFace(FrontFace face, Window* window = nullptr) = 0;
		/// <summary>
		/// Set the blend mode for the given window.
		/// </summary>
		/// <param name="modeRGB">Blend mode to use for the RGB channels.</param>
		/// <param name="modeAlpha">Blend mode to use for the alpha channel.</param>
		/// <param name="window">Window to set the blend mode for. Default: Main Window.</param>
		virtual void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha, Window* window = nullptr) = 0;
		/// <summary>
		/// Set the blend constants/factors for the given window.
		/// </summary>
		/// <param name="sourceRGB">Specifies how the red, green, and blue blending factors are computed.</param>
		/// <param name="sourceAlpha">Specifies how the alpha source blending factor is computed.</param>
		/// <param name="destinationRGB">Specifies how the red, green, and blue destination blending factors are computed.</param>
		/// <param name="destinationAlpha">Specified how the alpha destination blending factor is computed.</param>
		/// <param name="window">Window to set the blend constants for. Default: Main Window.</param>
		virtual void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
			                          BlendConstant destinationRGB, BlendConstant destinationAlpha,
									  Window* window = nullptr) = 0;
		/// <summary>
		/// Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// </summary>
		/// <param name="shadingRate">Shading rate to use.</param>
		/// <param name="texture">Unused by Vulkan.</param>
		/// <param name="postRasterizerRate">Shading rate combiner to use.</param>
		/// <param name="finalRate">Shading rate combiner to use.</param>
		/// <param name="window">Window to set the shading rate for. Default: Main Window.</param>
		virtual void SetShadingRate(ShadingRate shadingRate,
						            TRAP::Graphics::Texture* texture,
		                            ShadingRateCombiner postRasterizerRate,
							        ShadingRateCombiner finalRate, Window* window = nullptr) = 0;

		/// <summary>
		/// Clear the given window's render target.
		/// </summary>
		/// <param name="clearType">Type of buffer to clear.</param>
		/// <param name="window">Window to clear. Default: Main Window.</param>
		virtual void Clear(ClearBufferType clearType, Window* window = nullptr) = 0;

		//CommandBuffer Stuff

		/// <summary>
		/// Set viewport size for the given window.
		/// </summary>
		/// <param name="x">X coordinate of the top left corner of the viewport.</param>
		/// <param name="y">Y coordinate of the top left corner of the viewport.</param>
		/// <param name="width">New viewport width.</param>
		/// <param name="height">New viewport height.</param>
		/// <param name="minDepth">New min depth value. Default: 0.0f.</param>
		/// <param name="maxDepth">New max depth value. Default: 1.0f.</param>
		/// <param name="window">Window to set viewport for. Default: Main Window.</param>
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height, float minDepth = 0.0f,
		                         float maxDepth = 1.0f, Window* window = nullptr) = 0;
		/// <summary>
		/// Set scissor size for the given window.
		/// </summary>
		/// <param name="x">Upper left corner.</param>
		/// <param name="y">Upper left corner.</param>
		/// <param name="width">New scissor width.</param>
		/// <param name="height">New scissor height.</param>
		/// <param name="window">Window to set scissor size for. Default: Main Window.</param>
		virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
		                        Window* window = nullptr) = 0;

		/// <summary>
		/// Draw non-indexed, non-instanced geometry for the given window.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		virtual void Draw(uint32_t vertexCount, uint32_t firstVertex = 0, Window* window = nullptr) = 0;
		/// <summary>
		/// Draw indexed, non-instanced geometry for the given window.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw. Default: 0.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		virtual void DrawIndexed(uint32_t indexCount, uint32_t firstIndex = 0, uint32_t firstVertex = 0,
		                         Window* window = nullptr) = 0;
		/// <summary>
		/// Draw non-indexed, instanced geometry for the given window.
		/// </summary>
		/// <param name="vertexCount">Number of vertices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="firstInstance">Index of the first instance to draw. Default: 0.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex = 0,
		                           uint32_t firstInstance = 0, Window* window = nullptr) = 0;
		/// <summary>
		/// Draw indexed, instanced geometry for the given window.
		/// </summary>
		/// <param name="indexCount">Number of indices to draw.</param>
		/// <param name="instanceCount">Number of instances to draw.</param>
		/// <param name="firstIndex">Index of the first indice to draw. Default: 0.</param>
		/// <param name="firstInstance">Index of the first instance to draw. Default: 0.</param>
		/// <param name="firstVertex">Index of the first vertex to draw. Default: 0.</param>
		/// <param name="window">Window to draw for. Default: Main Window.</param>
		virtual void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount,
		                                  uint32_t firstIndex = 0, uint32_t firstInstance = 0,
										  uint32_t firstVertex = 0, Window* window = nullptr) = 0;

		/// <summary>
		/// Bind vertex buffer on the given window.
		/// </summary>
		/// <param name="vBuffer">Vertex buffer to bind.</param>
		/// <param name="layout">Layout of the vertex buffer.</param>
		/// <param name="window">Window to bind the vertex buffer for. Default: Main Window.</param>
		virtual void BindVertexBuffer(const TRAP::Ref<Buffer>& vBuffer, const VertexBufferLayout& layout,
		                              Window* window = nullptr) = 0;
		/// <summary>
		/// Bind an index buffer on the given window.
		/// </summary>
		/// <param name="iBuffer">Index buffer to bind.</param>
		/// <param name="indexType">Data type used by the index buffer.</param>
		/// <param name="window">Window to bind the vertex buffer for. Default: Main Window.</param>
		virtual void BindIndexBuffer(const TRAP::Ref<Buffer>& iBuffer, IndexType indexType,
		                             Window* window = nullptr) = 0;
		/// <summary>
		/// Bind a descriptor set on the given window.
		/// </summary>
		/// <param name="dSet">Descriptor set to bind.</param>
		/// <param name="index">Index for which descriptor set to bind.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		/// <param name="window">Window to bind the descriptor set for. Default: Main Window.</param>
		virtual void BindDescriptorSet(DescriptorSet& dSet, uint32_t index,
		                               QueueType queueType = QueueType::Graphics,
									   Window* window = nullptr) = 0;
		/// <summary>
		/// Bind push constant buffer data on the given window.
		/// Note: There is an optimized function which uses the index into the RootSignature
		///       instead of the name of the push constant block.
		/// </summary>
		/// <param name="name">Name of the push constant block.</param>
		/// <param name="constantsData">Pointer to the constant buffer data.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		/// <param name="window">Window to bind the push constants for. Default: Main Window.</param>
		virtual void BindPushConstants(const char* name, const void* constantsData,
		                               QueueType queueType = QueueType::Graphics,
									   Window* window = nullptr) = 0;
		/// <summary>
		/// Bind push constant buffer data on the given window.
		/// </summary>
		/// <param name="paramIndex">Index of the push constant block in the RootSignatures descriptors array.</param>
		/// <param name="constantsData">Pointer to the constant buffer data.</param>
		/// <param name="queueType">Queue type on which to perform the bind operation. Default: Graphics.</param>
		/// <param name="window">Window to bind the push constants for. Default: Main Window.</param>
		virtual void BindPushConstantsByIndex(uint32_t paramIndex, const void* constantsData,
											  QueueType queueType = QueueType::Graphics,
											  Window* window = nullptr) = 0;
		/// <summary>
		/// Bind render target(s) on the given window.
		///
		/// Note: This functions ends the currently running render pass and starts a new one.
		/// </summary>
		/// <param name="colorTarget">Color render target to bind.</param>
		/// <param name="depthStencil">Optional depth stencil target to bind. Default: nullptr.</param>
		/// <param name="loadActions">Optional load actions for each render target. Default: nullptr.</param>
		/// <param name="colorArraySlices">Optional color array slices for each render target. Default: nullptr.</param>
		/// <param name="colorMipSlices">Optional color mip slices for each render target. Default: nullptr.</param>
		/// <param name="depthArraySlice">Optional depth array slice for the depth stencil target. Default: -1.</param>
		/// <param name="depthMipSlice">Optional depth mip slice for the depth stencil target. Default: -1.</param>
		/// <param name="window">Window to bind the render target(s) for. Default: Main Window.</param>
		virtual void BindRenderTarget(const TRAP::Ref<Graphics::RenderTarget>& colorTarget,
		                              const TRAP::Ref<Graphics::RenderTarget>& depthStencil = nullptr,
									  const RendererAPI::LoadActionsDesc* loadActions = nullptr,
									  std::vector<uint32_t>* colorArraySlices = nullptr,
									  std::vector<uint32_t>* colorMipSlices = nullptr,
									  uint32_t depthArraySlice = -1, uint32_t depthMipSlice = -1,
									  Window* window = nullptr) = 0;
		/// <summary>
		/// Bind render target(s) on the given window.
		///
		/// Note: This functions ends the currently running render pass and starts a new one.
		/// </summary>
		/// <param name="colorTargets">Color render target(s) to bind.</param>
		/// <param name="depthStencil">Optional depth stencil target to bind. Default: nullptr.</param>
		/// <param name="loadActions">Optional load actions for each render target. Default: nullptr.</param>
		/// <param name="colorArraySlices">Optional color array slices for each render target. Default: nullptr.</param>
		/// <param name="colorMipSlices">Optional color mip slices for each render target. Default: nullptr.</param>
		/// <param name="depthArraySlice">Optional depth array slice for the depth stencil target. Default: -1.</param>
		/// <param name="depthMipSlice">Optional depth mip slice for the depth stencil target. Default: -1.</param>
		/// <param name="window">Window to bind the render target(s) for. Default: Main Window.</param>
		virtual void BindRenderTargets(const std::vector<TRAP::Ref<Graphics::RenderTarget>>& colorTargets,
		                               const TRAP::Ref<Graphics::RenderTarget>& depthStencil = nullptr,
									   const RendererAPI::LoadActionsDesc* loadActions = nullptr,
									   std::vector<uint32_t>* colorArraySlices = nullptr,
									   std::vector<uint32_t>* colorMipSlices = nullptr,
									   uint32_t depthArraySlice = -1, uint32_t depthMipSlice = -1,
									   Window* window = nullptr) = 0;

		/// <summary>
		/// Add a resource barrier (memory dependency) for the given window.
		/// </summary>
		/// <param name="bufferBarrier">Buffer barrier.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to add the barrier for. Default: Main Window.</param>
		virtual void ResourceBufferBarrier(const RendererAPI::BufferBarrier& bufferBarrier,
										   QueueType queueType = QueueType::Graphics,
								           Window* window = nullptr) = 0;
		/// <summary>
		/// Add resource barriers (memory dependencies) for the given window.
		/// </summary>
		/// <param name="bufferBarriers">Buffer barriers.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to add the barriers for. Default: Main Window.</param>
		virtual void ResourceBufferBarriers(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
											QueueType queueType = QueueType::Graphics,
									        Window* window = nullptr) = 0;
		/// <summary>
		/// Add a resource barrier (memory dependency) for the given window.
		/// </summary>
		/// <param name="textureBarrier">Texture barrier.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to add the barrier for. Default: Main Window.</param>
		virtual void ResourceTextureBarrier(const RendererAPI::TextureBarrier& textureBarrier,
											QueueType queueType = QueueType::Graphics,
									        Window* window = nullptr) = 0;
		/// <summary>
		/// Add resource barriers (memory dependencies) for the given window.
		/// </summary>
		/// <param name="textureBarriers">Texture barriers.</param>
		/// <param name="queueType">Queue type on which to perform the barrier operation. Default: Graphics.</param>
		/// <param name="window">Window to add the barriers for. Default: Main Window.</param>
		virtual void ResourceTextureBarriers(const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
											 QueueType queueType = QueueType::Graphics,
									         Window* window = nullptr) = 0;
		/// <summary>
		/// Add a resource barrier (memory dependency) for the given window.
		/// </summary>
		/// <param name="renderTargetBarrier">Render target barrier.</param>
		/// <param name="window">Window to add the barrier for. Default: Main Window.</param>
		virtual void ResourceRenderTargetBarrier(const RendererAPI::RenderTargetBarrier& renderTargetBarrier,
									             Window* window = nullptr) = 0;
		/// <summary>
		/// Add resource barriers (memory dependencies) for the given window.
		/// </summary>
		/// <param name="renderTargetBarriers">Render target barriers.</param>
		/// <param name="window">Window to add the barriers for. Default: Main Window.</param>
		virtual void ResourceRenderTargetBarriers(const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers,
									              Window* window = nullptr) = 0;

		/// <summary>
		/// Retrieve the renderer title.
		/// Example title: "[Vulkan 1.3.0]".
		/// </summary>
		/// <returns>Renderer title.</returns>
		virtual const std::string& GetTitle() const = 0;

		/// <summary>
		/// Retrieve the currently used GPUs UUID.
		/// </summary>
		/// <returns>GPU's UUID.</returns>
		virtual std::array<uint8_t, 16> GetCurrentGPUUUID() = 0;
		/// <summary>
		/// Retrieve the name of the currently used GPU.
		/// </summary>
		/// <returns>GPU's name.</returns>
		virtual std::string GetCurrentGPUName() = 0;
		/// <summary>
		/// Retrieve a list of all supported GPUs.
		/// The list contains the GPUs name and UUID.
		/// </summary>
		/// <returns>List of all supported GPUs.</returns>
		virtual std::vector<std::pair<std::string, std::array<uint8_t, 16>>> GetAllGPUs() = 0;

		/// <summary>
		/// Capture a screenshot of the last presented frame.
		/// </summary>
		/// <param name="window">Window to capture screenshot on. Default: Main Window.</param>
		/// <returns>Captured screenshot as TRAP::Image on success, Black 1x1 TRAP::Image otherwise.</returns>
		virtual TRAP::Scope<TRAP::Image> CaptureScreenshot(Window* window = nullptr) = 0;

		/// <summary>
		/// Retrieve the used descriptor pool.
		/// </summary>
		/// <returns>Descriptor pool.</returns>
		static TRAP::Ref<TRAP::Graphics::DescriptorPool> GetDescriptorPool();
		/// <summary>
		/// Retrieve the used graphics queue.
		/// </summary>
		/// <returns>Graphics queue.</returns>
		static TRAP::Ref<TRAP::Graphics::Queue> GetGraphicsQueue();
		/// <summary>
		/// Retrieve the used compute queue.
		/// </summary>
		/// <returns>Compute queue.</returns>
		static TRAP::Ref<TRAP::Graphics::Queue> GetComputeQueue();
		/// <summary>
		/// Retrieve the used transfer queue.
		/// </summary>
		/// <returns>Transfer queue.</returns>
		static TRAP::Ref<TRAP::Graphics::Queue> GetTransferQueue();
		/// <summary>
		/// Retrieve the currently used graphics root signature of the given window.
		/// </summary>
		/// <param name="window">Window to retrieve the graphics root signature from. Default: Main Window.</param>
		/// <returns>Graphics root signature.</returns>
		static TRAP::Ref<TRAP::Graphics::RootSignature> GetGraphicsRootSignature(Window* window = nullptr);

		/// <summary>
		/// Start a render pass for the given window.
		///
		/// Note: This will bind the render target for the current frame again.
		/// </summary>
		/// <param name="window">Window to start render pass for. Default: Main Window.</param>
		static void StartRenderPass(Window* window = nullptr);
		/// <summary>
		/// Stop running render pass of the given window.
		/// </summary>
		/// <param name="window">Window to stop render pass on. Default: Main Window.</param>
		static void StopRenderPass(Window* window = nullptr);

		/// <summary>
		/// Transition a texture from old layout to the new layout.
		/// The transition happens immediately and is guaranteed to be complete when the function returns.
		/// </summary>
		/// <param name="texture">Texture to transition layout.</param>
		/// <param name="oldLayout">Current resource state of the given texture.</param>
		/// <param name="newLayout">New resource state for the given texture.</param>
		/// <param name="queueType">Queue type on which to perform the transition. Default: Graphics.</param>
		static void Transition(TRAP::Graphics::Texture* texture,
							   TRAP::Graphics::RendererAPI::ResourceState oldLayout,
							   TRAP::Graphics::RendererAPI::ResourceState newLayout,
							   TRAP::Graphics::RendererAPI::QueueType queueType = QueueType::Graphics);

	//protected:
		/// <summary>
		/// Retrieve the main windows internal rendering data.
		/// </summary>
		/// <returns>Main windows internal rendering data.</returns>
		static PerWindowData& GetMainWindowData();

	public:
		/// <summary>
		/// Initialize the internal rendering data of the given window.
		/// </summary>
		/// <param name="window">Window to initialize the internal rendering data for.</param>
		virtual void InitPerWindowData(Window* window) = 0;
		/// <summary>
		/// Remove the internal rendering data of the given window.
		/// </summary>
		/// <param name="window">Window to remove the internal rendering data from.</param>
		virtual void RemovePerWindowData(Window* window) = 0;

		/// <summary>
		/// Wait for the GPU to idle.
		/// </summary>
		virtual void WaitIdle() = 0;

		/// <summary>
		/// Check if the system is Vulkan API capable.
		///
		/// Note: The first call to this function will run the Vulkan capability tester.
		/// </summary>
		/// <returns>True if the system is Vulkan API capable, false otherwise.</returns>
		static bool IsVulkanCapable();

		/// <summary>
		/// Enum bit flag for the different wave operations.
		/// </summary>
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

		/// <summary>
		/// Enum describing the different types of queues.
		/// </summary>
		enum class QueueType
		{
			Graphics = 0,
			Transfer,
			Compute,

			MAX_QUEUE_TYPE
		};

		/// <summary>
		/// Enum describing queue flags.
		/// </summary>
		enum class QueueFlag : uint32_t
		{
			None = 0x0,
			DisableGPUTimeout = BIT(0), //Disable the GPU timeout for this command queue (DirectX 12)

			MAX_QUEUE_FLAG = 0xFFFFFFFF
		};

		/// <summary>
		/// Enum describing the different priorities for a queue (DirectX 12).
		/// </summary>
		enum class QueuePriority
		{
			Normal,
			High,
			GlobalRealtime,

			MAX_QUEUE_PRIORITY
		};

		/// <summary>
		/// Enum describing the status of a fence.
		/// </summary>
		enum class FenceStatus
		{
			Complete = 0,
			Incomplete,
			NotSubmitted
		};

		/// <summary>
		/// Enum describing the different types of load actions.
		/// </summary>
		enum class LoadActionType
		{
			DontCare,
			Load,
			Clear,

			MAX_LOAD_ACTION_TYPE
		};

		/// <summary>
		/// Enum describing the different types of store actions.
		/// </summary>
		enum class StoreActionType
		{
			//Store is the most common use case so keep that as default
			Store,
			DontCare,
			None,

			MAX_STORE_ACTION_TYPE
		};

		/// <summary>
		/// Enum describing the different types of cube textures.
		/// </summary>
		enum class TextureCubeType
		{
			NONE = 0,

			MultiFile,
			Cross,
			//TODO
			//Equirectangular
		};

		/// <summary>
		/// Enum describing the different sample counts.
		/// </summary>
		enum class SampleCount
		{
			SampleCount1 = BIT(0),
			SampleCount2 = BIT(1),
			SampleCount4 = BIT(2),
			SampleCount8 = BIT(3),
			SampleCount16 = BIT(4)
		};

		/// <summary>
		/// Enum describing the different anti aliasing methods.
		/// </summary>
		enum class AntiAliasing
		{
			Off,
			MSAA,
			// SSAA
		};

		/// <summary>
		/// Enum bit flags used by texture creation.
		/// </summary>
		enum class TextureCreationFlags
		{
			//Default flag (Texture will use default allocation strategy decided by the API specific allocator)
			None = 0x0,
			//Texture will allocate its own memory (COMMITTED resource)
			OwnMemory = BIT(0),
			//Texture will be allocated in memory which can be shared among multiple processes
			Export = BIT(1),
			//Texture will be allocated in memory which can be shared among multiple GPUs
			ExportAdapter = BIT(2),
			//Texture will be imported from a handle created in another process
			Import = BIT(3),
			//Use ESRAM to store this texture
			ESRAM = BIT(4),
			//Use on-tile memory to store this texture
			OnTile = BIT(5),
			//Force 2D instead of automatically determining dimension based on width, height, depth
			Force2D = BIT(7),
			//Force 3D instead of automatically determining dimension based on width, height, depth
			Force3D = BIT(8),
			//Display target
			AllowDisplayTarget = BIT(9),
			//Create an sRGB texture
			SRGB = BIT(10),
			//Create a normal map texture
			NormalMap = BIT(11),
			//Fast clear
			FastClear = BIT(12),
			//Fragment mask
			FragMask = BIT(13),
			//Create a storage texture
			Storage = BIT(14)
		};

		/// <summary>
		/// Enum describing the state of a resource.
		/// </summary>
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

		/// <summary>
		/// Enum describing the type of a descriptor.
		/// </summary>
		enum class DescriptorType
		{
			Undefined = 0,
			Sampler = BIT(0),
			//SRV read only Texture
			Texture = BIT(1),
			//UAV Texture
			RWTexture = BIT(2),
			//SRV read only Buffer
			Buffer = BIT(3),
			BufferRaw = (Buffer | (Buffer << 1)),
			//UAV Buffer
			RWBuffer = BIT(5),
			RWBufferRaw = (RWBuffer | (RWBuffer << 1)),
			//Uniform buffer
			UniformBuffer = BIT(7),
			//Push constant / Root constant
			RootConstant = BIT(8),
			//IA
			VertexBuffer = BIT(9),
			IndexBuffer = BIT(10),
			IndirectBuffer = BIT(11),
			//Cubemap SRV
			TextureCube = (Texture | (IndirectBuffer << 1)),
			//RTV / DSV per mip slice
			RenderTargetMipSlices = BIT(13),
			//RTV / DSV per array slice
			RenderTargetArraySlices = BIT(14),
			//RTV / DSV per depth sice
			RenderTargetDepthSlices = BIT(15),
			RayTracing = BIT(16),

			//Vulkan
			//Subpass input (descriptor type only available in Vulkan)
			InputAttachment = BIT(17),
			TexelBuffer = BIT(18),
			RWTexelBuffer = BIT(19),
			CombinedImageSampler = BIT(20),

			//Khronos ray tracing extension
			AccelerationStructure = BIT(21),
			AccelerationStructureBuildInput = BIT(22),
			ShaderDeviceAddress  = BIT(23),
			ShaderBindingTable = BIT(24)
		};

		//Choosing Memory Type

		/// <summary>
		/// Enum describing the different memory usage types for a resource.
		/// </summary>
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

		/// <summary>
		/// Enum describing flags for the buffer creation.
		/// </summary>
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
			NoDescriptorViewCreation = BIT(4),

			//Vulkan
			HostVisible = 0x100,
			HostCoherent = 0x200
		};

		/// <summary>
		/// Enum describing the different types of buffers for clearing.
		/// </summary>
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

		/// <summary>
		/// Enum describing the type of an indirect argument.
		/// </summary>
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

		/// <summary>
		/// Enum describing the update frequency of a descriptor.
		/// Note: The same update frequency order should be used by shaders.
		/// </summary>
		enum class DescriptorUpdateFrequency
		{
			Static = 0,
			Dynamic
		};
		/// <summary>
		/// The maximum amount of descriptor sets that can be used.
		/// </summary>
		inline static constexpr uint32_t MaxDescriptorSets = 4;

		/// <summary>
		/// Enum describing the different types of filtering used by samplers.
		/// </summary>
		enum class FilterType
		{
			Nearest = 0,
			Linear
		};

		/// <summary>
		/// Enum describing the different modes of mip mapping used by samplers.
		/// </summary>
		enum class MipMapMode
		{
			Nearest = 0,
			Linear
		};

		/// <summary>
		/// Enum describing the different address modes used by samplers.
		/// </summary>
		enum class AddressMode
		{
			Mirror,
			Repeat,
			ClampToEdge,
			ClampToBorder
		};

		/// <summary>
		/// Enum describing the different compare modes used by depth/stencil testing and samplers.
		/// </summary>
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

		/// <summary>
		/// Enum describing the different shader stages making up a shader.
		/// </summary>
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

		/// <summary>
		/// Enum describing the different flags for root signature creation.
		/// </summary>
		enum class RootSignatureFlags
		{
			//Default flag
			None = 0,
			//Local root signature used mainly in raytracing shaders
			Local = BIT(0)
		};

		/// <summary>
		/// Enum describing the different pipeline types.
		/// </summary>
		enum class PipelineType
		{
			Undefined = 0,
			Compute,
			Graphics,
			RayTracing,

			PIPELINE_TYPE_COUNT
		};

		/// <summary>
		/// Enum describing the different data types used by index buffers.
		/// </summary>
		enum class IndexType
		{
			UInt32 = 0,
			UInt16
		};

		/// <summary>
		/// Enum describing the different blend constants used for blending.
		/// </summary>
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

		/// <summary>
		/// Enum describing the different blend modes/functions used for blending.
		/// </summary>
		enum class BlendMode
		{
			Add,
			Subtract,
			ReverseSubtract,
			Min,
			Max,

			MAX_BLEND_MODES
		};

		/// <summary>
		/// Enum describing which render target to affect with the blend state.
		/// </summary>
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

		/// <summary>
		/// Enum describing the different stencil/depth operations.
		/// </summary>
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

		/// <summary>
		/// Enum describing the different cull modes.
		/// </summary>
		enum class CullMode
		{
			None = 0,
			Back,
			Front,
			Both,

			MAX_CULL_MODES
		};

		/// <summary>
		/// Enum describing the different front face winding orders.
		/// </summary>
		enum class FrontFace
		{
			CounterClockwise = 0,
			Clockwise
		};

		/// <summary>
		/// Enum describing the different fill modes.
		/// </summary>
		enum class FillMode
		{
			Solid,
			Line,
			Point,

			MAX_FILL_MODES
		};

		/// <summary>
		/// Enum describing flags for pipeline cache creation.
		/// </summary>
		enum class PipelineCacheFlags
		{
			None = 0x0,
			ExternallySynchronized = BIT(0)
		};

		/// <summary>
		/// Enum describing the different primitive topologies.
		/// </summary>
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

		/// <summary>
		/// Enum describing the different vertex attribute rates.
		/// </summary>
		enum class VertexAttributeRate
		{
			Vertex = 0,
			Instance = 1,

			VERTEX_ATTRIBUTE_RATE_COUNT
		};

		/// <summary>
		/// Enum describing the different types of queries.
		/// </summary>
		enum class QueryType
		{
			Timestamp = 0,
			PipelineStatistics,
			Occlusion,

			QUERY_TYPE_COUNT
		};

		/// <summary>
		/// Enum describing the different presentation statuses.
		/// </summary>
		enum class PresentStatus
		{
			Success = 0,
			DeviceReset = 1,
			Failed = 2,
			OutOfDate = 3
		};

		/// <summary>
		/// Enum describing the different sampler ranges used by YUV conversion samplers.
		/// </summary>
		enum class SamplerRange
		{
			Full = 0,
			Narrow = 1
		};

		/// <summary>
		/// Enum describing the different sampler models used by YUV conversion samplers.
		/// </summary>
		enum class SamplerModelConversion
		{
			RGBIdentity = 0,
			YCBCRIdentity = 1,
			YCBCR709 = 2,
			YCBCR601 = 3,
			YCBCR2020 = 4
		};

		/// <summary>
		/// Enum describing the different sample locations used by YUV conversion samplers.
		/// </summary>
		enum class SampleLocation
		{
			Cosited = 0,
			Midpoint = 1
		};

		/// <summary>
		/// Enum describing the shading rates used by fragment/pixel shaders.
		/// </summary>
		enum class ShadingRate
		{
			NotSupported = 0x0,
			Full = BIT(0),
			Half = BIT(1),
			Quarter = BIT(2),
			Eighth = BIT(3),
			OneXTwo = BIT(4),
			TwoXOne = BIT(5),
			TwoXFour = BIT(6),
			FourXTwo = BIT(7)
		};

		/// <summary>
		/// Enum describing how to combine different shading rates.
		/// </summary>
		enum class ShadingRateCombiner
		{
			Passthrough = 0x0,
			Override = BIT(0),
			Min = BIT(1),
			Max = BIT(2),
			Sum = BIT(3)
		};

		/// <summary>
		/// Enum describing the shading rate capabilities supported by the GPU.
		/// </summary>
		enum class ShadingRateCaps
		{
			NotSupported = 0x0,
			PerDraw = BIT(0),
			PerTile = BIT(1)
		};

		/// <summary>
		/// Description of a subresource.
		/// Used to update a existing resource.
		/// </summary>
		struct SubresourceDataDesc
		{
			uint64_t SrcOffset;
			uint32_t MipLevel;
			uint32_t ArrayLayer;

			uint32_t RowPitch;
			uint32_t SlicePitch;
		};

		/// <summary>
		/// Description of a render target.
		/// </summary>
		struct RenderTargetDesc
		{
			//Texture creation flags (decides memory allocation strategy, sharing access, ...)
			TextureCreationFlags Flags{};
			//Width
			uint32_t Width{};
			//Height
			uint32_t Height{};
			//Depth (should be 1 if type is not TextureType::TextureCube)
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

		/// <summary>
		/// Description of a texture.
		/// </summary>
		struct TextureDesc
		{
			//Texture creation flags (decides memory allocation strategy, sharing access, ...)
			TextureCreationFlags Flags{};
			//Width
			uint32_t Width = 1;
			//Height;
			uint32_t Height = 1;
			//Depth (should be 1 if Type is not TextureType::TextureCube)
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

			//Optional Vulkan YCbCr conversion info
			::VkSamplerYcbcrConversionInfo* VkSamplerYcbcrConversionInfo{};
		};

		/// <summary>
		/// Description for a texture load.
		/// </summary>
		struct TextureLoadDesc
		{
			//Target to load texture info into.
			TRAP::Graphics::Texture* Texture;
			//Load empty texture
			TextureDesc* Desc;
			//Filepath with extension.
			std::array<std::filesystem::path, 6> Filepaths;
			//Following is ignored if Desc != nullptr.
			//Desc->Flags will be considered instead.
			TextureCreationFlags CreationFlag;
			//Is Texture cubemap?
			bool IsCubemap;
			//Type of cubemap texture
			TextureCubeType Type;
		};

		/// <summary>
		/// Description of a buffer.
		/// </summary>
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
			//Buffer* CounterBuffer;
			//Format of the buffer
			TRAP::Graphics::API::ImageFormat Format{};
			//Flags specifying the suitable usage of this buffer (Uniform Buffer, Vertex Buffer, Index Buffer, ...)
			DescriptorType Descriptors{};
			//Debug name used in GPU-profile
			const std::string_view Name{};
		};

		/// <summary>
		/// Description of a sampler.
		/// </summary>
		struct SamplerDesc
		{
			//Minification filter
			FilterType MinFilter{};
			//Magnification filter
			FilterType MagFilter{};
			//Mip mapping mode.
			TRAP::Graphics::RendererAPI::MipMapMode MipMapMode{};
			//Address mode for U coordinate
			AddressMode AddressU{};
			//Address mode for V coordinate
			AddressMode AddressV{};
			//Address mode for W coordinate
			AddressMode AddressW{};
			//Mip lod bias (offset from the calculated mip map level)
			float MipLodBias{};
			//Whether to use the given lod range or not
			bool SetLodRange{};
			//Minimum lod value
			float MinLod{};
			//Maximum lod value
			float MaxLod{};
			//Max anisotropy
			float MaxAnisotropy{};
			//Comparison function compares sampled data against existing sampled data
			CompareMode CompareFunc{};

			/// <summary>
			/// Description of YCbCr(YUV) conversion sampler.
			/// </summary>
			struct SamplerConversionDesc
			{
				//YCbCr(YUV) image format
				TRAP::Graphics::API::ImageFormat Format;
				//Conversion model
				SamplerModelConversion Model;
				//Range
				SamplerRange Range;
				//X offset for chroma
				SampleLocation ChromaOffsetX;
				//Y offset for chroma
				SampleLocation ChromaOffsetY;
				//Chroma filter
				FilterType ChromaFilter;
				//Explicitly force the reconstruction
				bool ForceExplicitReconstruction;

				bool operator==(const SamplerConversionDesc& s) const;
			} SamplerConversionDesc{};

			bool operator==(const SamplerDesc& s) const;
		};

		/// <summary>
		/// Description of a binary shader stage.
		/// </summary>
		struct BinaryShaderStageDesc
		{
			//Shader data
			std::vector<uint32_t> ByteCode{};
			//Entry point of shader
			std::string EntryPoint{};
		};

		/// <summary>
		/// Description of a binary shader.
		/// </summary>
		struct BinaryShaderDesc
		{
			//Shader stages contained in the binary shader
			ShaderStage Stages{};
			//Vertex shader stage data
			BinaryShaderStageDesc Vertex{};
			//Fragment shader stage data
			BinaryShaderStageDesc Fragment{};
			//Geometry shader stage data
			BinaryShaderStageDesc Geometry{};
			//TessellationContol shader stage data
			BinaryShaderStageDesc TessellationControl{};
			//TessellationEvaluation shader stage data
			BinaryShaderStageDesc TessellationEvaluation{};
			//Compute shader stage data
			BinaryShaderStageDesc Compute{};
		};

		/// <summary>
		/// Description of a root signature.
		/// </summary>
		struct RootSignatureDesc
		{
			//Shaders to manage
			std::vector<Shader*> Shaders{};
			//Max number of bindless texture (DirectX 12)
			uint32_t MaxBindlessTextures{};
			//Static sampler names
			std::vector<const char*> StaticSamplerNames{};
			//Static samplers
			std::vector<TRAP::Ref<Sampler>> StaticSamplers{};
			//Root signature creation flags
			RootSignatureFlags Flags{};
		};

		/// <summary>
		/// Struct containing information about a descriptor.
		/// </summary>
		struct DescriptorInfo
		{
			//Name of descriptor
			std::string Name{};
			//Type of descriptor
			DescriptorType Type{};
			//Dimension of texture
			API::ShaderReflection::TextureDimension Dimension{};
			//Is root descriptor?
			bool RootDescriptor{};
			//Descriptor set
			uint32_t Set{};
			//Size of descriptor
			uint32_t Size{};
			//Index in the descriptor set
			uint32_t IndexInParent{};
			//Index of the handle in the root signature
			uint32_t HandleIndex{};

			//Descriptor type (Vulkan)
			VkDescriptorType VkType{};
			//Resource binding location
			uint32_t Reg{};
			//Index into the root descriptor.
			uint32_t RootDescriptorIndex{};
			//Shader stages using the descriptor (Vulkan)
			uint32_t VkStages{};
		};

		/// <summary>
		/// Description of a descriptor set.
		/// </summary>
		struct DescriptorSetDesc
		{
			//Root signature to use
			TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
			//Descriptor set index
			uint32_t Set;
			//Max amount of sets to allocate
			uint32_t MaxSets{};
		};

		/// <summary>
		/// Description of a command pool.
		/// </summary>
		struct CommandPoolDesc
		{
			//Queue to be used by the command pool
			TRAP::Ref<TRAP::Graphics::Queue> Queue;
			//Indicate that the allocated command buffers will be short lived
			bool Transient;
		};

		/// <summary>
		/// Description of a queue.
		/// </summary>
		struct QueueDesc
		{
			//Type of queue
			QueueType Type{};
			//Queue creation flags
			QueueFlag Flag{};
			//Priority for the queue (DirectX 12)
			QueuePriority Priority{};
		};

		/// <summary>
		/// Read range used for buffer mapping.
		/// </summary>
		struct ReadRange
		{
			//Offset from the start of the buffer
			uint64_t Offset{};
			//Range to read (DirectX 12)
			uint64_t Range{};
		};

		/// <summary>
		/// Description for a queue submission.
		/// </summary>
		struct QueueSubmitDesc
		{
			///Command buffers to submit
			std::vector<CommandBuffer*> Cmds{};
			//Fence to signal
			TRAP::Ref<Fence> SignalFence{};
			//Semaphores to wait on
			std::vector<TRAP::Ref<Semaphore>> WaitSemaphores{};
			//Semaphores to signal
			std::vector<TRAP::Ref<Semaphore>> SignalSemaphores{};
		};

		/// <summary>
		/// Description of a blend state.
		/// </summary>
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

		/// <summary>
		/// Description of a depth state.
		/// </summary>
		struct DepthStateDesc
		{
			//Enable depth testing?
			bool DepthTest{};
			//Enable depth writing?
			bool DepthWrite{};
			//Depth comparison function
			CompareMode DepthFunc{};
			//Enable stencil testing?
			bool StencilTest{};
			//Stencil read mask
			uint8_t StencilReadMask{};
			//Stencil write mask
			uint8_t StencilWriteMask{};
			//Function to use on the front for stencil testing
			CompareMode StencilFrontFunc{};
			//Stencil action to perform when stencil testing fails
			StencilOp StencilFrontFail{};
			//Depth action to perform when depth testing fails
			StencilOp DepthFrontFail{};
			//Stencil action to perform when passed
			StencilOp StencilFrontPass{};
			//Function to use on the back for stencil testing
			CompareMode StencilBackFunc{};
			//Stencil action to perform when stencil testing fails
			StencilOp StencilBackFail{};
			//Depth action to perform when depth testing fails
			StencilOp DepthBackFail{};
			//Stencil action to perform when passed
			StencilOp StencilBackPass{};
		};

		/// <summary>
		/// Description of a rasterizer state.
		/// </summary>
		struct RasterizerStateDesc
		{
			//Cull mode
			TRAP::Graphics::RendererAPI::CullMode CullMode{};
			//Depth bias
			int32_t DepthBias{};
			//Depth bias slope factor.
			float SlopeScaledDepthBias{};
			//Fill mode
			TRAP::Graphics::RendererAPI::FillMode FillMode{};
			//Front face winding order
			TRAP::Graphics::RendererAPI::FrontFace FrontFace{};
			//Clamp depth to min/max depth
			bool DepthClampEnable{};
		};

		/// <summary>
		/// Description of a pipeline cache.
		/// </summary>
		struct PipelineCacheDesc
		{
			//Pipeline data
			std::vector<uint8_t> Data{};
			//Pipeline cache creation flags
			PipelineCacheFlags Flags{};
		};

		/// <summary>
		/// Description of a pipeline cache to load.
		/// </summary>
		struct PipelineCacheLoadDesc
		{
			//Path to pipeline cache
			std::filesystem::path Path{};
			//Pipeline cache creation flags
			PipelineCacheFlags Flags{};
		};

		/// <summary>
		/// Description of a compute pipeline.
		/// </summary>
		struct ComputePipelineDesc
		{
			//Shader to use
			Shader* ShaderProgram{};
			//Root signature to use
			TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
		};

		/// <summary>
		/// Description of a single vertex attribute.
		/// </summary>
		struct VertexAttribute
		{
			//Attribute format
			TRAP::Graphics::API::ImageFormat Format{};
			//Attribute binding
			uint32_t Binding{};
			//Attribute location
			uint32_t Location{};
			//Attribute offset
			uint32_t Offset{};
			//Attribute rate
			VertexAttributeRate Rate{};
		};

		/// <summary>
		/// Description of a vertex layout.
		/// </summary>
		struct VertexLayout
		{
			//Amount of attributes in the layout
			uint32_t AttributeCount{};
			//Attributes
			std::array<VertexAttribute, 15> Attributes{};
		};

		/// <summary>
		/// Description of a graphics pipeline.
		/// </summary>
		struct GraphicsPipelineDesc
		{
			//Shader to use
			Shader* ShaderProgram{};
			//Root signature to use
			TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
			//Vertex layou to use
			TRAP::Ref<TRAP::Graphics::RendererAPI::VertexLayout> VertexLayout{};
			//Blend state to use
			TRAP::Ref<BlendStateDesc> BlendState{};
			//Depth state to use
			TRAP::Ref<DepthStateDesc> DepthState{};
			//Rasterizer state to use
			TRAP::Ref<RasterizerStateDesc> RasterizerState{};
			//Color formats for the render targets
			std::vector<TRAP::Graphics::API::ImageFormat> ColorFormats{};
			//How many render targets to use
			uint32_t RenderTargetCount{};
			//Sample count to use by render targets
			TRAP::Graphics::RendererAPI::SampleCount SampleCount{};
			//Sample quality to use by render targets (DirectX 12)
			uint32_t SampleQuality{};
			//Depth/Stencil image format to use
			TRAP::Graphics::API::ImageFormat DepthStencilFormat{};
			//Primitive topology to use
			TRAP::Graphics::RendererAPI::PrimitiveTopology PrimitiveTopology{};
		};

		/// <summary>
		/// Description of a RayTracing pipeline.
		/// </summary>
		struct RayTracingPipelineDesc
		{
		};

		/// <summary>
		/// Description of a pipeline.
		/// </summary>
		struct PipelineDesc
		{
			//Type of pipeline
			PipelineType Type{};
			//Pipeline
			std::variant<ComputePipelineDesc,
			             GraphicsPipelineDesc,
						 RayTracingPipelineDesc> Pipeline{GraphicsPipelineDesc()};
			//Pipeline cache
			TRAP::Ref<PipelineCache> Cache{};
			//Pipeline extensions (DirectX 12)
			void* PipelineExtensions{};
			//Pipeline extensions count (DirectX 12)
			uint32_t PipelineExtensionCount{};

			//Name for the pipeline
			const char* Name{};
		};

		/// <summary>
		/// Description of a query pool.
		/// </summary>
		struct QueryPoolDesc
		{
			//Type of query to hold
			QueryType Type{};
			//Max amount of queries to manage
			uint32_t QueryCount{};
		};

		/// <summary>
		/// Description of a query.
		/// </summary>
		struct QueryDesc
		{
			//Index of the query into the query pool
			uint32_t Index{};
		};

		/// <summary>
		/// Struct holding indirect draw arguments.
		/// </summary>
		struct IndirectDrawArguments
		{
			//How many vertices to draw
			uint32_t VertexCount{};
			//How many instances to draw
			uint32_t InstanceCount{};
			//Vertex offset to start at
			uint32_t StartVertex{};
			//Instance offset to start at
			uint32_t StartInstance{};
		};

		/// <summary>
		/// Struct holding indirect indexed draw arguments.
		/// </summary>
		struct IndirectDrawIndexArguments
		{
			//How many indices to draw
			uint32_t IndexCount{};
			//How many instances to draw
			uint32_t InstanceCount{};
			//Index buffer offset to start at
			uint32_t StartIndex{};
			//Vertex offset to start at
			uint32_t VertexOffset{};
			//Instance offset to start at
			uint32_t StartInstance{};
		};

		/// <summary>
		/// Struct holding indirect dispatch arguments.
		/// </summary>
		struct IndirectDispatchArguments
		{
			//X work group size
			uint32_t GroupCountX{};
			//Y work group size
			uint32_t GroupCountY{};
			//Z work group size
			uint32_t GroupCountZ{};
		};

		/// <summary>
		/// Struct holding indirect descriptor argument.
		/// </summary>
		struct IndirectArgumentDescriptor
		{
			//Type of indirect argument
			IndirectArgumentType Type{};
			//Name of descriptor
			const char* Name{};
			//Index of descriptor
			uint32_t Index{};
		};

		/// <summary>
		/// Description of a command signature.
		/// </summary>
		struct CommandSignatureDesc
		{
			//Root signature.
			TRAP::Ref<TRAP::Graphics::RootSignature> RootSignature{};
			//Count of indirect arguments
			uint32_t IndirectArgCount{};
			//Indirect argument descriptions
			std::vector<IndirectArgumentDescriptor> ArgDescs{};
			//Set to true if indirect argument struct should not be aligned to 16 bytes
			bool Packed{};
		};

		/// <summary>
		/// Description of a swapchain.
		/// </summary>
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
			//Anti aliasing sample count (1 = no AA)
			RendererAPI::SampleCount SampleCount = SampleCount::SampleCount1;
			//Anti aliasing method
			RendererAPI::AntiAliasing AntiAliasing = AntiAliasing::Off;
		};

		/// <summary>
		/// Struct holding a render target barrier.
		/// </summary>
		struct RenderTargetBarrier
		{
			//Render target
			TRAP::Ref<TRAP::Graphics::RenderTarget> RenderTarget{};
			//Current resource state of the render target
			ResourceState CurrentState{};
			//Target resource state of the render target
			ResourceState NewState{};
			//Starts a barrier transition in a new state, putting a resource in a temporary no-access condition (DirectX 12)
			bool BeginOnly{};
			//This barrier completes a transition, setting a new state and restoring active access to a resource (DirectX 12)
			bool EndOnly{};
			//Acquire ownershop of the queue family
			bool Acquire{};
			//Release ownership of the queue family
			bool Release{};
			//Type of queue to acquire or release
			TRAP::Graphics::RendererAPI::QueueType QueueType{};
			//Specify whether following barrier targets particular subresource
			bool SubresourceBarrier{};

			//Following values are ignored if SubresourceBarrier is false

			//Mip level
			uint8_t MipLevel{};
			//Array layer
			uint16_t ArrayLayer{};
		};

		/// <summary>
		/// Struct holding a buffer barrier.
		/// </summary>
		struct BufferBarrier
		{
			//Buffer
			TRAP::Ref<TRAP::Graphics::Buffer> Buffer{};
			//Current resource state of the buffer
			ResourceState CurrentState{};
			//Target resource state of the buffer
			ResourceState NewState{};
			//Starts a barrier transition in a new state, putting a resource in a temporary no-access condition (DirectX 12)
			bool BeginOnly{};
			//This barrier completes a transition, setting a new state and restoring active access to a resource (DirectX 12)
			bool EndOnly{};
			//Acquire ownership of the queue family
			bool Acquire{};
			//Release ownership of the queue family
			bool Release{};
			//Type of queue to acquire or release
			TRAP::Graphics::RendererAPI::QueueType QueueType{};
		};

		/// <summary>
		/// Struct holding a texture barrier.
		/// </summary>
		struct TextureBarrier
		{
			//Texture
			TRAP::Graphics::Texture* Texture{};
			//Current resource state of the texture
			ResourceState CurrentState{};
			//Target resource state of the texture
			ResourceState NewState{};
			//Starts a barrier transition in a new state, putting a resource in a temporary no-access condition (DirectX 12)
			bool BeginOnly{};
			//This barrier completes a transition, setting a new state and restoring active access to a resource (DirectX 12)
			bool EndOnly{};
			//Acquire ownership of the queue family
			bool Acquire{};
			//Release ownership of the queue family
			bool Release{};
			//Type of queue to acquire or release
			TRAP::Graphics::RendererAPI::QueueType QueueType{};
			//Specify whether the following barrier targets particular subresource
			bool SubresourceBarrier{};

			//Following values are ignored if SubresourceBarrier is false

			//Mip level
			uint8_t MipLevel{};
			//Array layer
			uint16_t ArrayLayer{};
		};

		/// <summary>
		/// Struct holding a data of a descriptor.
		/// </summary>
		struct DescriptorData
		{
			//User can either set name of descriptor or index (index in RootSignature->Descriptors array)
			//Name of descriptor
			const char* Name{};
			/// <summary>
			/// Range(s) to bind (buffer, offset, size)
			/// </summary>
			struct BufferOffset
			{
				//Offset to bind the buffer descriptor
				std::vector<uint64_t> Offsets{};
				//Size of the buffer to bind
				std::vector<uint64_t> Sizes{};
			};

			/// <summary>
			/// Descriptor set buffer extraction options
			/// </summary>
			struct DescriptorSetExtraction
			{
				//Index of the descriptor set to extract
				uint32_t DescriptorSetBufferIndex{};
				Shader* DescriptorSetShader{};
				ShaderStage DescriptorSetShaderStage{};
			};

			struct TextureSlice
			{
				//When binding UAV, control the mip slice to bind for UAV (example - generating mipmaps in a compute shader)
				uint32_t UAVMipSlice{};
				//Binds entire mip chain as array of UAV
				bool BindMipChain{};
			};
			std::variant<BufferOffset, DescriptorSetExtraction, TextureSlice, bool> Offset{false};
			//Array of resources containing descriptor handles or constant to be used in ring buffer memory
			//DescriptorRange can hold only one resource type array
			//Can be one of the following:
			//Array of texture descriptors (SRV and UAV textures)
			//Array of sampler descriptors
			//Array of buffer descriptors (SRV, UAV and CBV buffers)
			//Array of pipeline descriptors
			//DescriptorSet buffer extraction
			//Custom binding (RayTracing acceleration structure ...)
			std::variant<std::vector<TRAP::Graphics::Texture*>, std::vector<Sampler*>,
				std::vector<Buffer*>, std::vector<Pipeline*>,
				std::vector<DescriptorSet*>> Resource{std::vector<TRAP::Graphics::Texture*>()}; //TODO RayTracing acceleration structure

			//Number of resources in the descriptor(applies to array of textures, buffers, ...)
			uint32_t Count{};
			//Index into RootSignature->Descriptors array
			uint32_t Index = static_cast<uint32_t>(-1);
		};

		/// <summary>
		/// Description of a queue presentation.
		/// </summary>
		struct QueuePresentDesc
		{
			//Swapchain to preesent
			TRAP::Ref<TRAP::Graphics::SwapChain> SwapChain{};
			//Semaphores to wait on before presenting
			std::vector<TRAP::Ref<Semaphore>> WaitSemaphores{};
			//Render target of the swapchain to present
			uint8_t Index{};
		};

		/// <summary>
		/// Description of actions to perform on load.
		/// </summary>
		struct LoadActionsDesc
		{
			//Action to perform on the color attachment(s) on load.
			std::array<LoadActionType, 8> LoadActionsColor{};
			//Action to perform on the depth attachment on load.
			LoadActionType LoadActionDepth{};
			//Action to perform on the stencil attachment on load.
			LoadActionType LoadActionStencil{};
			//Clear color(s)
			std::array<TRAP::Math::Vec4, 8> ClearColorValues{};
			//Clear depth value
			float ClearDepth = 1.0f;
			//Clear stencil value
			uint32_t ClearStencil = 0;
			//Action to perform on the color attachment(s) on store.
			std::array<StoreActionType, 8> StoreActionsColor{};
			//Action to perform on the depth attachment on store.
			StoreActionType StoreActionDepth{};
			//Action to perform on the stencil attachment on store.
			StoreActionType StoreActionStencil{};
		};

		/// <summary>
		/// Map resolving a name to its descriptor index in a root signature.
		/// </summary>
		using DescriptorIndexMap = std::unordered_map<std::string, uint32_t>;

		/// <summary>
		/// Struct holding data about a mapped memory range.
		/// </summary>
		struct MappedMemoryRange
		{
			//Pointer to mapped buffer data with offset already applied to
			uint8_t* Data = nullptr;
			//Mapped buffer
			TRAP::Ref<TRAP::Graphics::Buffer> Buffer = nullptr;
			//Offset in the buffer to start at
			uint64_t Offset = 0;
			//Mapped size
			uint64_t Size = 0;
			uint32_t Flags = 0;
		};

		/// <summary>
		/// Description for a texture update.
		/// Note: Only use for procedural textures which are created on CPU (noise textures, font texture, ...)
		/// </summary>
		struct TextureUpdateDesc
		{
			//Texture to update
			TRAP::Graphics::Texture* Texture = nullptr;
			//Mip level to update
			uint32_t MipLevel = 0;
			//Array layer to update
			uint32_t ArrayLayer = 0;

			//To be filled by the caller
			//Example:
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
			uint64_t DstRowStride;
			//Number of rows in this slice of the texture
			uint64_t RowCount;
			//Src row stride for convenience (RowCount * width * texture format size)
			uint64_t SrcRowStride;
			//Size of each slice in destination including padding - Use for offsetting dst data updating 3D textures
			uint64_t DstSliceStride;
			//Size of each slice in src - Use for offsetting src data when updating 3D textures
			uint64_t SrcSliceStride;

			//Internal
			struct
			{
				MappedMemoryRange MappedRange;
			} Internal;
		};

		/// <summary>
		/// Description for the resource loader.
		/// </summary>
		struct ResourceLoaderDesc
		{
			//Size for each staging buffer
			uint64_t BufferSize;
			//Amount of staging buffers to create
			uint32_t BufferCount;
		};

		/// <summary>
		/// Description for a buffer load.
		/// </summary>
		struct BufferLoadDesc
		{
			//Output buffer with uploaded data.
			TRAP::Ref<TRAP::Graphics::Buffer> Buffer;
			//Data to upload
			const void* Data;
			//Description for the buffer
			BufferDesc Desc;

			//Force Reset Buffer to 0
			bool ForceReset;
		};

		/// <summary>
		/// Description for a buffer update.
		/// </summary>
		struct BufferUpdateDesc
		{
			//Buffer to update
			TRAP::Ref<TRAP::Graphics::Buffer> Buffer;
			//Offset into buffer to update data at
			uint64_t DstOffset;
			//Size of data to update
			uint64_t Size;

			//To be filled by the caller
			void* MappedData;

			//Internal
			struct
			{
				MappedMemoryRange MappedRange;
			} Internal;
		};

		/// <summary>
		/// Struct holding data about a GPUs features, limits and other properties.
		/// </summary>
		inline static struct GPUSettings
		{
			uint64_t UniformBufferAlignment;
			uint32_t MaxUniformBufferRange;
			uint64_t StorageBufferAlignment;
			uint64_t MaxStorageBufferRange;
			uint64_t UploadBufferTextureAlignment;
			uint64_t UploadBufferTextureRowAlignment;
			uint32_t MaxVertexInputBindings;
			uint32_t MaxVertexInputAttributes;
			uint32_t MaxRootSignatureDWORDS;
			uint32_t MaxImageDimension2D;
			uint32_t MaxImageDimensionCube;
			uint32_t MaxPushConstantSize;
			uint32_t MaxSamplerAllocationCount;
			uint32_t MaxTessellationControlPoints;
			uint32_t MaxMSAASampleCount;
			float MaxAnisotropy;

			uint32_t WaveLaneCount;
			TRAP::Graphics::RendererAPI::WaveOpsSupportFlags WaveOpsSupportFlags;

			uint32_t ROVsSupported;
			bool MultiDrawIndirectSupported;
			bool TessellationSupported;
			bool GeometryShaderSupported;
			bool FillModeNonSolid;
			bool SurfaceSupported;
			bool PresentSupported;

			//Variable rate shading capabilities
			TRAP::Graphics::RendererAPI::ShadingRate ShadingRates;
			TRAP::Graphics::RendererAPI::ShadingRateCaps ShadingRateCaps;
			TRAP::Graphics::RendererAPI::ShadingRateCombiner ShadingRateCombiner;
			uint32_t ShadingRateTexelWidth;
			uint32_t ShadingRateTexelHeight;
		} GPUSettings{};

		inline static constexpr uint32_t ImageCount = 3; //Triple Buffered

		/// <summary>
		/// Retrieve the image index currently used for rendering from the given window.
		/// </summary>
		/// <param name="window">Window to retrieve image index from.</param>
		/// <returns>Image index.</returns>
		static uint32_t GetCurrentImageIndex(TRAP::Window* window);

#ifdef ENABLE_NSIGHT_AFTERMATH
		//GPU crash dump tracker using Nsight Aftermath instrumentation
		static bool s_aftermathSupport;
		static bool s_diagnosticsConfigSupport;
		static bool s_diagnosticCheckPointsSupport;
#endif

	protected:
		static TRAP::Scope<RendererAPI> s_Renderer;
		static RenderAPI s_RenderAPI;
		static TRAP::Scope<API::ResourceLoader> s_ResourceLoader;

		static TRAP::Ref<DescriptorPool> s_descriptorPool;
		static TRAP::Ref<Queue> s_graphicQueue;
		static TRAP::Ref<Queue> s_computeQueue;
		static TRAP::Ref<Queue> s_transferQueue;

	public:
		/// <summary>
		/// Per window data used for rendering.
		/// </summary>
		struct PerWindowData
		{
			/// <summary>
			/// Destructor.
			/// </summary>
			~PerWindowData();

			TRAP::Window* Window;

			//Swapchain/Graphics stuff
			uint32_t ImageIndex = 0;
			std::array<TRAP::Ref<CommandPool>, ImageCount> GraphicCommandPools;
			std::array<CommandBuffer*, ImageCount> GraphicCommandBuffers;
			std::array<TRAP::Ref<Fence>, ImageCount> RenderCompleteFences;
			TRAP::Ref<Semaphore> ImageAcquiredSemaphore;
			std::array<TRAP::Ref<Semaphore>, ImageCount> RenderCompleteSemaphores;
			std::array<TRAP::Ref<Semaphore>, ImageCount> GraphicsCompleteSemaphores;
			PipelineDesc GraphicsPipelineDesc;
			TRAP::Ref<Pipeline> CurrentGraphicsPipeline;
			RendererAPI::SampleCount SampleCount = RendererAPI::SampleCount::SampleCount8; //TODO Set via RenderCommand instead
			RendererAPI::AntiAliasing AntiAliasing = RendererAPI::AntiAliasing::MSAA; //TODO Set via RenderCommand instead
			bool Recording;

			TRAP::Ref<TRAP::Graphics::SwapChain> SwapChain;
#ifdef TRAP_HEADLESS_MODE
			std::array<TRAP::Ref<RenderTarget>, ImageCount> RenderTargets;
			bool Resize = false;
			uint32_t NewWidth = 1920, NewHeight = 1080; //Default RenderTargets to use Full HD
#endif
			uint32_t CurrentSwapChainImageIndex;

			TRAP::Math::Vec4 ClearColor{0.1f, 0.1f, 0.1f, 1.0f};
			float ClearDepth = 1.0f;
			uint32_t ClearStencil = 0;

			bool CurrentVSync;
			bool NewVSync;

			//Compute stuff
			std::array<TRAP::Ref<CommandPool>, ImageCount> ComputeCommandPools;
			std::array<CommandBuffer*, ImageCount>  ComputeCommandBuffers;
			std::array<TRAP::Ref<Fence>, ImageCount> ComputeCompleteFences;
			std::array<TRAP::Ref<Semaphore>, ImageCount> ComputeCompleteSemaphores;
			TRAP::Math::Vec3 CurrentComputeWorkGroupSize;
			PipelineDesc ComputePipelineDesc;
			TRAP::Ref<Pipeline> CurrentComputePipeline;
			bool RecordingCompute;
		};

	protected:
		static std::unordered_map<Window*, TRAP::Scope<PerWindowData>> s_perWindowDataMap;

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
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::ShadingRate);
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::ShadingRateCaps);
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::ShadingRateCombiner);
MAKE_ENUM_FLAG(TRAP::Graphics::RendererAPI::ClearBufferType);

#endif /*TRAP_RENDERERAPI_H*/