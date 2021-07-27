#ifndef _TRAP_RENDERCOMMAND_H_
#define _TRAP_RENDERCOMMAND_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	using RendererBufferType = RendererAPI::ClearBufferType;

	enum class RendererBlendFunction
	{
		Zero,
		One,
		Source_Color,
		One_Minus_Source_Color,
		Destination_Color,
		One_Minus_Destination_Color,
		Source_Alpha,
		One_Minus_Source_Alpha,
		Destination_Alpha,
		One_Minus_Destination_Alpha,
		Source_Alpha_Saturate,
		Constant_Color,
		One_Minus_Constant_Color
	};

	enum class RendererBlendEquation
	{
		Add,
		Subtract,
		Reverse_Subtract,
		Min,
		Max
	};

	enum class RendererFaceMode
	{
		Front,
		Back,
		Front_And_Back
	};

	enum class RendererCullMode
	{
		Back,
		Front
	};

	enum class RendererFrontFace
	{
		Clockwise,
		Counter_Clockwise
	};

	enum class RendererFunction
	{
		Never,
		Less,
		Equal,
		Less_Or_Equal,
		Greater,
		Not_Equal,
		Greater_Or_Equal,
		Always
	};

	enum class RendererOperation
	{
		Keep,
		Zero,
		Replace,
		Invert,
		Increment_And_Wrap,
		Decrement_And_Wrap,
		Increment_And_Clamp,
		Decrement_And_Clamp,
	};

	class RenderCommand
	{
	public:
		static void Clear(RendererBufferType buffer);
		static void Present(const Scope<Window>& window);

		static void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f });
		static void SetClearDepthStencil(float depth = 1.0f, uint32_t stencil = 0); //TODO Split up
		static void SetDepthTesting(bool enabled);
		static void SetDepthMasking(bool enabled);
		static void SetDepthFunction(RendererFunction function = RendererFunction::Less);
		static void SetStencilTesting(bool enabled);
		static void SetStencilMasking(uint32_t mask);
		static void SetStencilMaskingSeparate(RendererFaceMode face = RendererFaceMode::Front_And_Back,
		                                      uint32_t mask = 0xFF);
		static void SetStencilFunction(RendererFunction function, int32_t reference, uint32_t mask);
		static void SetStencilFunctionSeparate(RendererFaceMode face, RendererFunction function, int32_t reference,
		                                       uint32_t mask);
		static void SetStencilOperation(RendererOperation stencilFail, RendererOperation depthFail,
		                                RendererOperation pass);
		static void SetStencilOperationSeparate(RendererFaceMode face, RendererOperation stencilFail,
		                                        RendererOperation depthFail, RendererOperation pass);
		static void SetBlend(bool enabled);
		static void SetCull(bool enabled);
		static void SetFrontFace(RendererFrontFace frontFace = RendererFrontFace::Counter_Clockwise);
		static void SetWireFrame(bool enabled);
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		static void SetBlendFunction(RendererBlendFunction source = RendererBlendFunction::One,
		                             RendererBlendFunction destination = RendererBlendFunction::Zero);
		static void SetBlendFunctionSeparate(RendererBlendFunction sourceRGB = RendererBlendFunction::One,
		                                     RendererBlendFunction sourceAlpha = RendererBlendFunction::One,
		                                     RendererBlendFunction destinationRGB = RendererBlendFunction::Zero,
		                                     RendererBlendFunction destinationAlpha = RendererBlendFunction::Zero
		);
		static void SetBlendEquation(RendererBlendEquation blendEquation = RendererBlendEquation::Add);
		static void SetBlendEquationSeparate(RendererBlendEquation blendEquationRGB = RendererBlendEquation::Add,
		                                     RendererBlendEquation blendEquationAlpha = RendererBlendEquation::Add);

		static void SetCullMode(RendererCullMode cullMode = RendererCullMode::Back);

		//static void DrawIndexed(const Scope<VertexArray>& vertexArray, uint32_t indexCount = 0);
		//static void Draw(const Scope<VertexArray>& vertexArray);
	};
}

#endif /*_TRAP_RENDERCOMMAND_H_*/