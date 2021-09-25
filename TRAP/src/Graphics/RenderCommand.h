#ifndef _TRAP_RENDERCOMMAND_H_
#define _TRAP_RENDERCOMMAND_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	using ClearBuffer = RendererAPI::ClearBufferType;
	using CompareMode = RendererAPI::CompareMode;
	using StencilOperation = RendererAPI::StencilOp;
	using CullMode = RendererAPI::CullMode;
	using FillMode = RendererAPI::FillMode;
	using PrimitiveTopology = RendererAPI::PrimitiveTopology;
	using FrontFace = RendererAPI::FrontFace;
	using BlendMode = RendererAPI::BlendMode;
	using BlendConstant = RendererAPI::BlendConstant;

	class RenderCommand
	{
	public:
		static void Present(Window* window);

		//VSync functions
		static void SetVSync(bool vsync, Window* window = nullptr);
		static bool GetVSync(Window* window = nullptr);

		//Clear functions
		static void Clear(ClearBuffer buffer, Window* window = nullptr);
		static void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f }, Window* window = nullptr);
		static void SetClearDepth(float depth = 1.0f, Window* window = nullptr);
		static void SetClearStencil(uint32_t stencil = 0, Window* window = nullptr);

		//Depth functions
		static void SetDepthTesting(bool enabled, Window* window = nullptr);
		static void SetDepthWriting(bool enabled, Window* window = nullptr);
		static void SetDepthFunction(CompareMode compareMode = CompareMode::Less, Window* window = nullptr);
		static void SetDepthFail(StencilOperation front, StencilOperation back, Window* window = nullptr);
		static void SetDepthBias(int32_t bias, Window* window = nullptr);
		static void SetDepthBiasSlopeFactor(float slopeFactor, Window* window = nullptr);

		//Stencil functions
		static void SetStencilTesting(bool enabled, Window* window = nullptr);
		static void SetStencilFail(StencilOperation front, StencilOperation back, Window* window = nullptr);
		static void SetStencilPass(StencilOperation front, StencilOperation back, Window* window = nullptr);
		static void SetStencilFunction(CompareMode front, CompareMode back, Window* window = nullptr);
		static void SetStencilMask(uint8_t read, uint8_t write, Window* window = nullptr);

		//Miscellaneous functions
		static void SetCullMode(CullMode cullMode, Window* window = nullptr);
		static void SetFillMode(FillMode fillMode, Window* window = nullptr);
		static void SetPrimitiveTopology(PrimitiveTopology topology, Window* window = nullptr);
		static void SetFrontFace(FrontFace face, Window* window = nullptr);

		//Blending functions
		static void SetBlendMode(BlendMode modeRGB, BlendMode modeAlpha, Window* window = nullptr);
		static void SetBlendConstant(BlendConstant sourceRGBA, BlendConstant destinationRGBA,
									 Window* window = nullptr);
		static void SetBlendConstant(BlendConstant sourceRGB, BlendConstant sourceAlpha,
									 BlendConstant destinationRGB, BlendConstant destinationAlpha,
									 Window* window = nullptr);

		//Viewport/Scissor functions
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Window* window = nullptr);
		static void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Window* window = nullptr);

		//Drawing functions
		static void Draw(uint32_t vertexCount, uint32_t firstVertex = 0, Window* window = nullptr);
		static void DrawIndexed(uint32_t indexCount, uint32_t firstIndex = 0, uint32_t firstVertex = 0,
		                        Window* window = nullptr);
		static void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex = 0,
		                          uint32_t firstInstance = 0, Window* window = nullptr);
		static void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex = 0,
		                                 uint32_t firstInstance = 0, uint32_t firstVertex = 0,
										 Window* window = nullptr);

		//Bind functions
		static void SetPushConstants(const char* name, const void* data, Window* window = nullptr);
	};
}

#endif /*_TRAP_RENDERCOMMAND_H_*/