#ifndef _TRAP_RENDERERAPI_H_
#define _TRAP_RENDERERAPI_H_

#include "Maths/Math.h"

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
	enum class RendererDepthFunction;
	enum class RendererBufferType;
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

		virtual void Clear(RendererBufferType buffer) = 0;
		virtual void Present(const std::unique_ptr<Window>& window) = 0;

		virtual void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f }) = 0;
		virtual void SetDepthTesting(bool enabled) = 0;
		virtual void SetDepthMasking(bool enabled) = 0;
		virtual void SetDepthFunction(RendererDepthFunction function) = 0;
		virtual void SetBlend(bool enabled) = 0;
		virtual void SetCull(bool enabled) = 0;
		virtual void SetFrontFace(RendererFrontFace frontFace) = 0;
		virtual void SetWireFrame(bool enabled) = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void SetBlendFunction(RendererBlendFunction source, RendererBlendFunction destination) = 0;
		virtual void SetBlendFunctionSeparate(RendererBlendFunction sourceRGB,
			                                  RendererBlendFunction sourceAlpha,
			                                  RendererBlendFunction destinationRGB,
			                                  RendererBlendFunction destinationAlpha
		) = 0;
		virtual void SetBlendEquation(RendererBlendEquation blendEquation) = 0;
		virtual void SetBlendEquationSeparate(RendererBlendEquation blendEquationRGB, RendererBlendEquation blendEquationAlpha) = 0;

		virtual void SetCullMode(RendererCullMode cullMode) = 0;

		virtual void DrawIndexed(const Scope<VertexArray>& vertexArray, RendererPrimitive primitive) = 0;
		virtual void Draw(const Scope<VertexArray>& vertexArray, RendererPrimitive primitive) = 0;

		virtual std::string_view GetTitle() const = 0;

	protected:
		static Scope<RendererAPI> s_Renderer;
	};
}

#endif /*_TRAP_RENDERERAPI_H_*/