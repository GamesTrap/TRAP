#ifndef _TRAP_RENDERCOMMAND_H_
#define _TRAP_RENDERCOMMAND_H_

#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/API/RendererAPI.h"
#include "Maths/Maths.h"

namespace TRAP::Graphics
{
	enum RendererBufferType
	{
		RENDERER_BUFFER_NONE = 0,
		RENDERER_BUFFER_COLOR = BIT(0),
		RENDERER_BUFFER_DEPTH = BIT(1),
		RENDERER_BUFFER_STENCIL = BIT(2)
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
		static void Clear(uint32_t buffer);
		static void Present(Window* window);

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

		static void DrawIndexed(const std::unique_ptr<VertexArray>& vertexArray, RendererPrimitive primitive = RendererPrimitive::Triangle);
		static void Draw(const std::unique_ptr<VertexArray>& vertexArray, RendererPrimitive primitive = RendererPrimitive::Triangle);
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::DrawIndexed(const std::unique_ptr<VertexArray>& vertexArray, const RendererPrimitive primitive)
{
	API::RendererAPI::GetRenderer()->DrawIndexed(vertexArray, primitive);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::Draw(const std::unique_ptr<VertexArray>& vertexArray, const RendererPrimitive primitive)
{
	API::RendererAPI::GetRenderer()->Draw(vertexArray, primitive);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::Clear(const uint32_t buffer)
{
	API::RendererAPI::GetRenderer()->Clear(buffer);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::Present(Window* window)
{
	API::RendererAPI::GetRenderer()->Present(window);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetClearColor(const Math::Vec4& color)
{
	API::RendererAPI::GetRenderer()->SetClearColor(color);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetDepthTesting(const bool enabled)
{
	API::RendererAPI::GetRenderer()->SetDepthTesting(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetDepthMasking(const bool enabled)
{
	API::RendererAPI::GetRenderer()->SetDepthMasking(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetDepthFunction(RendererDepthFunction function)
{
	API::RendererAPI::GetRenderer()->SetDepthFunction(function);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetBlend(const bool enabled)
{
	API::RendererAPI::GetRenderer()->SetBlend(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetCull(const bool enabled)
{
	API::RendererAPI::GetRenderer()->SetCull(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetFrontFace(const RendererFrontFace frontFace)
{
	API::RendererAPI::GetRenderer()->SetFrontFace(frontFace);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetWireFrame(const bool enabled)
{
	API::RendererAPI::GetRenderer()->SetWireFrame(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
{
	API::RendererAPI::GetRenderer()->SetViewport(x, y, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetBlendFunction(const RendererBlendFunction source, const RendererBlendFunction destination)
{
	API::RendererAPI::GetRenderer()->SetBlendFunction(source, destination);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetBlendFunctionSeparate(const RendererBlendFunction sourceRGB,
                                                                    const RendererBlendFunction sourceAlpha,
                                                                    const RendererBlendFunction destinationRGB,
                                                                    const RendererBlendFunction destinationAlpha)
{
	API::RendererAPI::GetRenderer()->SetBlendFunctionSeparate(sourceRGB, sourceAlpha, destinationRGB, destinationAlpha);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetBlendEquation(const RendererBlendEquation blendEquation)
{
	API::RendererAPI::GetRenderer()->SetBlendEquation(blendEquation);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetBlendEquationSeparate(const RendererBlendEquation blendEquationRGB, const RendererBlendEquation blendEquationAlpha)
{
	API::RendererAPI::GetRenderer()->SetBlendEquationSeparate(blendEquationRGB, blendEquationAlpha);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetCullMode(const RendererCullMode cullMode)
{
	API::RendererAPI::GetRenderer()->SetCullMode(cullMode);
}

#endif /*_TRAP_RENDERCOMMAND_H_*/