#ifndef _TRAP_VULKANRENDERER_H_
#define _TRAP_VULKANRENDERER_H_

#include "Graphics/API/RendererAPI.h"
#include "Maths/Math.h"
#include "Window/WindowingAPI.h"

namespace TRAP::Graphics::API
{
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

		void SetCullMode(RendererFaceMode cullMode) override;

		void DrawIndexed(const Scope<VertexArray>& vertexArray, uint32_t indexCount, RendererPrimitive primitive) override;
		void Draw(const Scope<VertexArray>& vertexArray, RendererPrimitive primitive) override;

		const std::string& GetTitle() const override;

		std::array<uint8_t, 16> GetCurrentGPUUUID() override;
		std::string GetCurrentGPUName() override;
		std::vector<std::pair<std::string, std::array<uint8_t, 16>>> GetAllGPUs() override;
		
	private:		
		std::string m_rendererTitle;

		static VulkanRenderer* s_renderer;
	};
}

#endif /*_TRAP_VULKANRENDERER_H_*/