#ifndef _TRAP_OPENGLRENDERER_H_
#define _TRAP_OPENGLRENDERER_H_

namespace TRAP::Graphics::API
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
		void SetCullInternal(bool enabled) override;
		void SetFrontFaceInternal(RendererFrontFace frontFace) override;
		void SetViewportInternal(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;

		void SetBlendFunctionInternal(RendererBlendFunction source, RendererBlendFunction destination) override;
		void SetBlendEquationInternal(RendererBlendEquation blendEquation) override;

		void SetCullModeInternal(RendererCullMode cullMode) override;

		void SetWireFrameInternal(bool enabled) override;

		std::string_view GetTitleInternal() const override;

		static OpenGLRenderer* Get();

		static unsigned int TRAPRendererBufferToOpenGL(unsigned int buffer);
		static unsigned int TRAPRendererBlendFunctionToOpenGL(RendererBlendFunction function);
		static unsigned int TRAPRendererBlendEquationToOpenGL(RendererBlendEquation blendEquation);
		static unsigned int TRAPRendererCullModeToOpenGL(RendererCullMode cullMode);
		static unsigned int TRAPRendererFrontFaceToOpenGL(RendererFrontFace frontFace);

	private:
		static void GLAPIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
		static void InitDebug();

		API::OpenGLContext* m_context;
		std::string m_rendererTitle;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string_view TRAP::Graphics::API::OpenGLRenderer::GetTitleInternal() const
{
	return m_rendererTitle;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::OpenGLRenderer* TRAP::Graphics::API::OpenGLRenderer::Get()
{
	return reinterpret_cast<OpenGLRenderer*>(s_Renderer.get());
}

#endif /*_TRAP_OPENGLRENDERER_H_*/