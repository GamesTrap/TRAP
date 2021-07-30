#ifndef _TRAP_RENDERCOMMAND_H_
#define _TRAP_RENDERCOMMAND_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	using RendererBufferType = RendererAPI::ClearBufferType;
	using CompareMode = RendererAPI::CompareMode;
	using StencilOperation = RendererAPI::StencilOp;
	using CullMode = RendererAPI::CullMode;
	using FillMode = RendererAPI::FillMode;
	using PrimitiveTopology = RendererAPI::PrimitiveTopology;
	using FrontFace = RendererAPI::FrontFace;
	using BlendMode = RendererAPI::BlendMode;
	using BlendConstant = RendererAPI::BlendConstant;

	class RenderCommand //TODO Add Window* to each function
	{
	public:
		static void Present(const Scope<Window>& window); //TODO Use Window* insted?!

		//VSync functions
		static void SetVSync(bool vsync);
		static bool GetVSync();

		//Clear functions
		static void Clear(RendererBufferType buffer);
		static void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f });
		static void SetClearDepth(float depth = 1.0f);
		static void SetClearStencil(uint32_t stencil = 0);

		//Depth functions
		static void SetDepthTesting(bool enabled);
		static void SetDepthWriting(bool enabled);
		static void SetDepthFunction(CompareMode compareMode = CompareMode::Less);
		static void SetDepthFail(StencilOperation front, StencilOperation back);
		static void SetDepthBias(int32_t bias);
		static void SetDepthBiasSlopeFactor(float slopeFactor);

		//Stencil functions
		static void SetStencilTesting(bool enabled);
		static void SetStencilFail(StencilOperation front, StencilOperation back);
		static void SetStencilPass(StencilOperation front, StencilOperation back);
		static void SetStencilFunction(CompareMode front, CompareMode back);
		static void SetStencilMask(uint8_t read, uint8_t write);

		//Miscellaneous functions
		static void SetCullMode(CullMode cullMode);
		static void SetFillMode(FillMode fillMode);
		static void SetPrimitiveTopology(PrimitiveTopology topology);
		static void SetFrontFace(FrontFace face);

		//Blending functions
		static void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha);
		static void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
									 BlendConstant destinationRGB, BlendConstant destinationAlpha);

		//Viewport/Scissor functions
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		//Drawing functions
		static void Draw(uint32_t vertexCount, uint32_t firstVertex = 0);
		static void DrawIndexed(uint32_t indexCount, uint32_t firstIndex = 0, uint32_t firstVertex = 0);

		//Bind functions
		static void SetPushConstants(const char* name, const void* data);
	};
}

#endif /*_TRAP_RENDERCOMMAND_H_*/