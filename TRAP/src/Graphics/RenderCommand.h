#ifndef _TRAP_RENDERCOMMAND_H_
#define _TRAP_RENDERCOMMAND_H_

#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	enum class RendererBufferType
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

	enum class RendererBlendFunction
	{
		Zero,
		One,
		Source_Alpha,
		Destination_Alpha,
		One_Minus_Source_Alpha
	};

	enum class RendererBlendEquation
	{
		Add,
		Subtract
	};

	enum class RendererCullMode
	{
		Front,
		Back,
		Front_And_Back
	};

	enum class RendererFrontFace
	{
		Clockwise,
		Counter_Clockwise
	};

	enum class RendererPrimitive
	{
		Point,
		Line,
		Triangle,
		Patch
	};

	enum class RendererDepthFunction
	{
		Always,
		Never,
		Less,
		Equal,
		Less_Equal,
		Greater,
		Not_Equal,
		Greater_Equal
	};

	class RenderCommand
	{
	public:
		static void Clear(RendererBufferType buffer);
		static void Present(const std::unique_ptr<Window>& window);

		static void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f });
		static void SetDepthTesting(bool enabled);
		static void SetDepthMasking(bool enabled);
		static void SetDepthFunction(RendererDepthFunction function = RendererDepthFunction::Less);
		static void SetBlend(bool enabled);
		static void SetCull(bool enabled);
		static void SetFrontFace(RendererFrontFace frontFace = RendererFrontFace::Counter_Clockwise);
		static void SetWireFrame(bool enabled);
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		static void SetBlendFunction(RendererBlendFunction source = RendererBlendFunction::One, RendererBlendFunction destination = RendererBlendFunction::Zero);
		static void SetBlendFunctionSeparate(RendererBlendFunction sourceRGB = RendererBlendFunction::One,
		                                     RendererBlendFunction sourceAlpha = RendererBlendFunction::One,
		                                     RendererBlendFunction destinationRGB = RendererBlendFunction::Zero,
		                                     RendererBlendFunction destinationAlpha = RendererBlendFunction::Zero
		);
		static void SetBlendEquation(RendererBlendEquation blendEquation = RendererBlendEquation::Add);
		static void SetBlendEquationSeparate(RendererBlendEquation blendEquationRGB = RendererBlendEquation::Add, RendererBlendEquation blendEquationAlpha = RendererBlendEquation::Add);

		static void SetCullMode(RendererCullMode cullMode = RendererCullMode::Back);

		static void DrawIndexed(const Scope<VertexArray>& vertexArray, uint32_t indexCount = 0, RendererPrimitive primitive = RendererPrimitive::Triangle);
		static void Draw(const Scope<VertexArray>& vertexArray, RendererPrimitive primitive = RendererPrimitive::Triangle);
	};
}

#endif /*_TRAP_RENDERCOMMAND_H_*/