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

		void Clear(unsigned int buffer) override;
		void Present(Window* window) override;

		void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f }) override;
		void SetDepthTesting(bool enabled) override;
		void SetDepthMasking(bool enabled) override;
		void SetDepthFunction(RendererDepthFunction function) override;
		void SetBlend(bool enabled) override;
		void SetCull(bool enabled) override;
		void SetFrontFace(RendererFrontFace frontFace) override;
		void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;

		void SetBlendFunction(RendererBlendFunction source, RendererBlendFunction destination) override;
		void SetBlendEquation(RendererBlendEquation blendEquation) override;

		void SetCullMode(RendererCullMode cullMode) override;

		void SetWireFrame(bool enabled) override;

		void DrawIndexed(const std::unique_ptr<VertexArray>& vertexArray, RendererPrimitive primitive) override;
		void Draw(const std::unique_ptr<VertexArray>& vertexArray, RendererPrimitive primitive) override;

		std::string_view GetTitle() const override;

		static OpenGLRenderer* Get();

		static unsigned int TRAPRendererBufferToOpenGL(unsigned int buffer);
		static unsigned int TRAPRendererBlendFunctionToOpenGL(RendererBlendFunction function);
		static unsigned int TRAPRendererBlendEquationToOpenGL(RendererBlendEquation blendEquation);
		static unsigned int TRAPRendererCullModeToOpenGL(RendererCullMode cullMode);
		static unsigned int TRAPRendererFrontFaceToOpenGL(RendererFrontFace frontFace);
		static unsigned int TRAPRendererPrimitiveToOpenGL(RendererPrimitive primitive);
		static unsigned int TRAPRendererDepthFunctionToOpenGL(RendererDepthFunction function);

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