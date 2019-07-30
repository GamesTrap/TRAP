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
		NONE,
		ZERO,
		ONE,
		SOURCE_ALPHA,
		DESTINATION_ALPHA,
		ONE_MINUS_SOURCE_ALPHA
	};

	enum class RendererBlendEquation
	{
		NONE,
		ADD,
		SUBTRACT
	};

	enum class RendererCullMode
	{
		NONE,
		FRONT,
		BACK,
		FRONT_AND_BACK
	};

	enum class RendererFrontFace
	{
		CLOCKWISE,
		COUNTER_CLOCKWISE
	};

	enum class RendererPrimitive
	{
		POINT,
		LINE,
		TRIANGLE
	};

	class RenderCommand
	{
	public:
		static void Clear(unsigned int buffer);
		static void Present(Window* window);

		static void SetClearColor(const Maths::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f });
		static void SetDepthTesting(bool enabled);
		static void SetBlend(bool enabled);
		static void SetCull(bool enabled);
		static void SetFrontFace(RendererFrontFace frontFace);
		static void SetWireFrame(bool enabled);
		static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

		static void SetBlendFunction(RendererBlendFunction source, RendererBlendFunction destination);
		static void SeBlendEquation(RendererBlendEquation blendEquation);

		static void SetCullMode(RendererCullMode cullMode);

		static void DrawIndexed(const std::unique_ptr<VertexArray>& vertexArray, RendererPrimitive primitive = RendererPrimitive::TRIANGLE);
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::DrawIndexed(const std::unique_ptr<VertexArray>& vertexArray, const RendererPrimitive primitive)
{
	API::RendererAPI::GetRenderer()->DrawIndexed(vertexArray, primitive);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::Clear(const unsigned int buffer)
{
	API::RendererAPI::GetRenderer()->Clear(buffer);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::Present(Window* window)
{
	API::RendererAPI::GetRenderer()->Present(window);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetClearColor(const Maths::Vec4& color)
{
	API::RendererAPI::GetRenderer()->SetClearColor(color);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetDepthTesting(const bool enabled)
{
	API::RendererAPI::GetRenderer()->SetDepthTesting(enabled);
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

inline void TRAP::Graphics::RenderCommand::SetViewport(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height)
{
	API::RendererAPI::GetRenderer()->SetViewport(x, y, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetBlendFunction(const RendererBlendFunction source, const RendererBlendFunction destination)
{
	API::RendererAPI::GetRenderer()->SetBlendFunction(source, destination);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SeBlendEquation(const RendererBlendEquation blendEquation)
{
	API::RendererAPI::GetRenderer()->SetBlendEquation(blendEquation);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetCullMode(const RendererCullMode cullMode)
{
	API::RendererAPI::GetRenderer()->SetCullMode(cullMode);
}

#endif /*_TRAP_RENDERCOMMAND_H_*/