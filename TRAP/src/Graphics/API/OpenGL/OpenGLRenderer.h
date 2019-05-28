#pragma once

namespace TRAP::Graphics
{
	class OpenGLRenderer final : public Renderer
	{
	public:
		OpenGLRenderer();
		~OpenGLRenderer();
		OpenGLRenderer(const OpenGLRenderer&) = default;
		OpenGLRenderer& operator=(const OpenGLRenderer&) = default;
		OpenGLRenderer(OpenGLRenderer&&) = default;
		OpenGLRenderer& operator=(OpenGLRenderer&&) = default;

		void InitInternal() override;

		void ClearInternal(unsigned int buffer) override;
		void PresentInternal(Window* window) override;

		void SetDepthTestingInternal(bool enabled) override;
		void SetBlendInternal(bool enabled) override;
		void SetViewportInternal(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;

		void SetBlendFunctionInternal(RendererBlendFunction source, RendererBlendFunction destination) override;
		void SetBlendEquationInternal(RendererBlendEquation blendEquation) override;

		std::string_view GetTitleInternal() const override;

		static OpenGLRenderer* Get() { return reinterpret_cast<OpenGLRenderer*>(s_Instance.get()); }

		static unsigned int TRAPRendererBufferToGL(unsigned int buffer);
		static unsigned int TRAPRendererBlendFunctionToGL(RendererBlendFunction function);
		static unsigned int TRAPRendererBlendEquationToGL(RendererBlendEquation blendEquation);

	private:
		static void GLAPIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
		static void InitDebug();

		API::OpenGLContext* m_context;
		std::string m_rendererTitle;
	};
}
