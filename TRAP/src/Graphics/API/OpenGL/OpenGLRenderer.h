#ifndef _TRAP_OPENGLRENDERER_H_
#define _TRAP_OPENGLRENDERER_H_

#include "Graphics/API/RendererAPI.h"
#include "Maths/Math.h"
#include "OpenGLContext.h"

namespace TRAP::Graphics
{
	class Texture;
}

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

		void Clear(RendererBufferType buffer) override;
		void Present(const Scope<Window>& window) override;

		void SetClearColor(const Math::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f }) override;
		void SetDepthTesting(bool enabled) override;
		void SetDepthMasking(bool enabled) override;
		void SetDepthFunction(RendererFunction function) override;
		void SetStencilTesting(bool enabled) override;
		void SetStencilMasking(uint32_t mask) override;
		void SetStencilMaskingSeparate(RendererFaceMode face, uint32_t mask) override;
		void SetStencilFunction(RendererFunction function, int32_t reference, uint32_t mask) override;
		void SetStencilFunctionSeparate(RendererFaceMode face, RendererFunction function, int32_t reference, uint32_t mask) override;
		void SetStencilOperation(RendererOperation stencilFail, RendererOperation depthFail, RendererOperation pass) override;
		void SetStencilOperationSeparate(RendererFaceMode face, RendererOperation stencilFail, RendererOperation depthFail, RendererOperation pass) override;
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

		void SetCullMode(RendererFaceMode cullMode) override;

		void SetWireFrame(bool enabled) override;

		void DrawIndexed(const Scope<VertexArray>& vertexArray, uint32_t indexCount, RendererPrimitive primitive) override;
		void Draw(const Scope<VertexArray>& vertexArray, RendererPrimitive primitive) override;

		const std::string& GetTitle() const override;

		std::vector<uint8_t> GetCurrentGPUUUID() override;
		std::string GetCurrentGPUName() override;
		std::vector<std::pair<std::string, std::vector<uint8_t>>> GetAllGPUs() override;
		
		static uint32_t TRAPRendererBufferToOpenGL(RendererBufferType buffer);
		static uint32_t TRAPRendererBlendFunctionToOpenGL(RendererBlendFunction function);
		static uint32_t TRAPRendererBlendEquationToOpenGL(RendererBlendEquation blendEquation);
		static uint32_t TRAPRendererFaceModeToOpenGL(RendererFaceMode faceMode);
		static uint32_t TRAPRendererFrontFaceToOpenGL(RendererFrontFace frontFace);
		static uint32_t TRAPRendererPrimitiveToOpenGL(RendererPrimitive primitive);
		static uint32_t TRAPRendererFunctionToOpenGL(RendererFunction function);
		static uint32_t TRAPRendererOperationToOpenGL(RendererOperation operation);

		static uint32_t GetMaxTextureUnits();
		static std::unordered_map<uint32_t, const Texture*>& GetBoundTextures();
		
	private:
		static OpenGLRenderer* Get();
		static void GLAPIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
		static void InitDebug();

		API::OpenGLContext* m_context;
		std::string m_rendererTitle;

		static uint32_t s_maxCombinedTextureUnits;
		static std::unordered_map<uint32_t, const Texture*> s_boundTextures;
	};
}

#endif /*_TRAP_OPENGLRENDERER_H_*/