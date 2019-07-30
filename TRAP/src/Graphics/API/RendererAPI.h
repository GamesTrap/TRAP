#ifndef _TRAP_RENDERERAPI_H_
#define _TRAP_RENDERERAPI_H_

#include "Maths/Maths.h"

namespace TRAP
{
	class Window;
}

namespace TRAP::Graphics 
{
	class VertexArray;
	enum class RendererPrimitive;
	enum class RendererCullMode;
	enum class RendererBlendEquation;
	enum class RendererBlendFunction;
	enum class RendererFrontFace;
}

namespace TRAP::Graphics::API
{
	class RendererAPI
	{
	public:
		RendererAPI() = default;
		virtual ~RendererAPI() = default;
		RendererAPI(const RendererAPI&) = default;
		RendererAPI& operator=(const RendererAPI&) = default;
		RendererAPI(RendererAPI&&) = default;
		RendererAPI& operator=(RendererAPI&&) = default;

		static void Init();
		static void Shutdown();

		static RendererAPI* GetRenderer();

		virtual void InitInternal() = 0;

		virtual void Clear(unsigned int buffer) = 0;
		virtual void Present(Window* window) = 0;

		virtual void SetClearColor(const Maths::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f }) = 0;
		virtual void SetDepthTesting(bool enabled) = 0;
		virtual void SetBlend(bool enabled) = 0;
		virtual void SetCull(bool enabled) = 0;
		virtual void SetFrontFace(RendererFrontFace frontFace) = 0;
		virtual void SetWireFrame(bool enabled) = 0;
		virtual void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) = 0;

		virtual void SetBlendFunction(RendererBlendFunction source, RendererBlendFunction destination) = 0;
		virtual void SetBlendEquation(RendererBlendEquation blendEquation) = 0;

		virtual void SetCullMode(RendererCullMode cullMode) = 0;

		virtual void DrawIndexed(const std::unique_ptr<VertexArray>& vertexArray, RendererPrimitive primitive) = 0;

		virtual std::string_view GetTitle() const = 0;

	protected:
		static std::unique_ptr<RendererAPI> s_Renderer;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::RendererAPI* TRAP::Graphics::API::RendererAPI::GetRenderer()
{
	return s_Renderer.get();
}

#endif /*_TRAP_RENDERERAPI_H_*/