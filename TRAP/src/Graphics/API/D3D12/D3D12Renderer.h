#ifndef _TRAP_D3D12RENDERER_H_
#define _TRAP_D3D12RENDERER_H_

#ifdef TRAP_PLATFORM_WINDOWS

#include "Graphics/API/RendererAPI.h"
#include "D3D12Context.h"
#include "Maths/Math.h"

namespace TRAP::Graphics::API
{
	class D3D12Renderer final : public RendererAPI
	{
	public:
		D3D12Renderer();
		~D3D12Renderer();
		D3D12Renderer(const D3D12Renderer&) = default;
		D3D12Renderer& operator=(const D3D12Renderer&) = default;
		D3D12Renderer(D3D12Renderer&&) = default;
		D3D12Renderer& operator=(D3D12Renderer&&) = default;

		void InitInternal() override;

		void Clear(RendererBufferType buffer) override;
		void Present(const std::unique_ptr<Window>& window) override;

		void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f }) override;
		void SetDepthTesting(bool enabled) override;
		void SetDepthMasking(bool enabled) override;
		void SetDepthFunction(RendererDepthFunction function) override;
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

		void DrawIndexed(const Scope<VertexArray>& vertexArray, RendererPrimitive primitive) override;
		void Draw(const Scope<VertexArray>& vertexArray, RendererPrimitive primitive) override;

		std::string_view GetTitle() const override;

		static D3D12Renderer* Get();

	private:
		D3D12Context* m_context;
		std::string m_rendererTitle;
	};
}

#endif

#endif /*_TRAP_D3D12RENDERER_H_*/