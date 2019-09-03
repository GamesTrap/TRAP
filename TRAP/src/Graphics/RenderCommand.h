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
		NONE = 0,
		Zero,
		One,
		Source_Alpha,
		Destination_Alpha,
		One_Minus_Source_Alpha
	};

	enum class RendererBlendEquation
	{
		NONE = 0,
		Add,
		Subtract
	};

	enum class RendererCullMode
	{
		NONE = 0,
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

	class RenderCommand
	{
	public:
		static void Clear(unsigned int buffer);
		static void Present(Window* window);

		static void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f });
		static void SetDepthTesting(bool enabled);
		static void SetBlend(bool enabled);
		static void SetCull(bool enabled);
		static void SetFrontFace(RendererFrontFace frontFace);
		static void SetWireFrame(bool enabled);
		static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

		static void SetBlendFunction(RendererBlendFunction source, RendererBlendFunction destination);
		static void SetBlendEquation(RendererBlendEquation blendEquation);

		static void SetCullMode(RendererCullMode cullMode);

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

inline void TRAP::Graphics::RenderCommand::SetBlendEquation(const RendererBlendEquation blendEquation)
{
	API::RendererAPI::GetRenderer()->SetBlendEquation(blendEquation);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::RenderCommand::SetCullMode(const RendererCullMode cullMode)
{
	API::RendererAPI::GetRenderer()->SetCullMode(cullMode);
}

#endif /*_TRAP_RENDERCOMMAND_H_*/