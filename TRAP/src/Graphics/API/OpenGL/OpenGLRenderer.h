#ifndef _TRAP_OPENGLRENDERER_H_
#define _TRAP_OPENGLRENDERER_H_

#include "Graphics/API/RendererAPI.h"
#include "Maths/Math.h"
#include "OpenGLContext.h"

namespace TRAP::Graphics::API
{
	class OpenGLRenderer final : public RendererAPI
	{
	public:
		OpenGLRenderer();
		~OpenGLRenderer();
		OpenGLRenderer(const OpenGLRenderer&) = default;
		OpenGLRenderer& operator=(const OpenGLRenderer&) = default;
		OpenGLRenderer(OpenGLRenderer&&) = default;
		OpenGLRenderer& operator=(OpenGLRenderer&&) = default;

		void InitInternal() override;

		void Clear(uint32_t buffer) override;
		void Present(Window* window) override;

		void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f }) override;
		void SetDepthTesting(bool enabled) override;
		void SetDepthMasking(bool enabled) override;
		void SetDepthFunction(RendererDepthFunction function) override;
		void SetBlend(bool enabled) override;
		void SetCull(bool enabled) override;
		void SetFrontFace(RendererFrontFace frontFace) override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		void SetBlendFunction(RendererBlendFunction source, RendererBlendFunction destination) override;
		void SetBlendFunctionSeparate(RendererBlendFunction sourceRGB,
		                              RendererBlendFunction sourceAlpha,
		                              RendererBlendFunction destinationRGB,
		                              RendererBlendFunction destinationAlpha) override;
		void SetBlendEquation(RendererBlendEquation blendEquation) override;
		void SetBlendEquationSeparate(RendererBlendEquation blendEquationRGB, RendererBlendEquation blendEquationAlpha) override;

		void SetCullMode(RendererCullMode cullMode) override;

		void SetWireFrame(bool enabled) override;

		void DrawIndexed(const std::unique_ptr<VertexArray>& vertexArray, RendererPrimitive primitive) override;
		void Draw(const std::unique_ptr<VertexArray>& vertexArray, RendererPrimitive primitive) override;

		std::string_view GetTitle() const override;

		static OpenGLRenderer* Get();

		static uint32_t TRAPRendererBufferToOpenGL(uint32_t buffer);
		static uint32_t TRAPRendererBlendFunctionToOpenGL(RendererBlendFunction function);
		static uint32_t TRAPRendererBlendEquationToOpenGL(RendererBlendEquation blendEquation);
		static uint32_t TRAPRendererCullModeToOpenGL(RendererCullMode cullMode);
		static uint32_t TRAPRendererFrontFaceToOpenGL(RendererFrontFace frontFace);
		static uint32_t TRAPRendererPrimitiveToOpenGL(RendererPrimitive primitive);
		static uint32_t TRAPRendererDepthFunctionToOpenGL(RendererDepthFunction function);

	private:
		static void GLAPIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
		static void InitDebug();

		API::OpenGLContext* m_context;
		std::string m_rendererTitle;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string_view TRAP::Graphics::API::OpenGLRenderer::GetTitle() const
{
	return m_rendererTitle;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::OpenGLRenderer* TRAP::Graphics::API::OpenGLRenderer::Get()
{
	return reinterpret_cast<OpenGLRenderer*>(s_Renderer.get());
}

#endif /*_TRAP_OPENGLRENDERER_H_*/