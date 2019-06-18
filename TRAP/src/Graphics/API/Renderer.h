#ifndef _TRAP_RENDERER_H_
#define _TRAP_RENDERER_H_

namespace TRAP
{
	class Window;
}

namespace TRAP::Graphics::API
{
	enum class RendererBufferType
	{
		RENDERER_BUFFER_NONE    = 0,
		RENDERER_BUFFER_COLOR   = BIT(0),
		RENDERER_BUFFER_DEPTH   = BIT(1),
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

	class Renderer
	{
	public:
		Renderer() = default;
		virtual ~Renderer() = default;
		Renderer(const Renderer&) = default;
		Renderer& operator=(const Renderer&) = default;
		Renderer(Renderer&&) = default;
		Renderer& operator=(Renderer&&) = default;

		static void Init();
		static void Shutdown();

		static void Clear(unsigned int buffer);
		static void Present(Window* window);

		static void SetDepthTesting(bool enabled);
		static void SetBlend(bool enabled);
		static void SetCull(bool enabled);
		static void SetFrontFace(RendererFrontFace frontFace);
		static void SetWireFrame(bool enabled);
		static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

		static void SetBlendFunction(RendererBlendFunction source, RendererBlendFunction destination);
		static void SeBlendEquation(RendererBlendEquation blendEquation);

		static void SetCullMode(RendererCullMode cullMode);


		static std::string_view GetTitle();

		static Renderer* GetRenderer();

	protected:
		virtual void InitInternal() = 0;

		virtual void ClearInternal(unsigned int buffer) = 0;
		virtual void PresentInternal(Window* window) = 0;

		virtual void SetDepthTestingInternal(bool enabled) = 0;
		virtual void SetBlendInternal(bool enabled) = 0;
		virtual void SetCullInternal(bool enabled) = 0;
		virtual void SetFrontFaceInternal(RendererFrontFace frontFace) = 0;
		virtual void SetWireFrameInternal(bool enabled) = 0;
		virtual void SetViewportInternal(unsigned int x, unsigned int y, unsigned int width, unsigned int height) = 0;

		virtual void SetBlendFunctionInternal(RendererBlendFunction source, RendererBlendFunction destination) = 0;
		virtual void SetBlendEquationInternal(RendererBlendEquation blendEquation) = 0;

		virtual void SetCullModeInternal(RendererCullMode cullMode) = 0;


		virtual std::string_view GetTitleInternal() const = 0;

		static std::unique_ptr<Renderer> s_Renderer;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::Renderer::Clear(const unsigned int buffer)
{
	s_Renderer->ClearInternal(buffer);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::Renderer::Present(Window* window)
{
	s_Renderer->PresentInternal(window);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::Renderer::SetDepthTesting(const bool enabled)
{
	s_Renderer->SetDepthTestingInternal(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::Renderer::SetBlend(const bool enabled)
{
	s_Renderer->SetBlendInternal(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::Renderer::SetCull(const bool enabled)
{
	s_Renderer->SetCullInternal(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::Renderer::SetFrontFace(const RendererFrontFace frontFace)
{
	s_Renderer->SetFrontFaceInternal(frontFace);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::Renderer::SetWireFrame(const bool enabled)
{
	s_Renderer->SetWireFrameInternal(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::Renderer::SetViewport(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height)
{
	s_Renderer->SetViewportInternal(x, y, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::Renderer::SetBlendFunction(const RendererBlendFunction source, const RendererBlendFunction destination)
{
	s_Renderer->SetBlendFunctionInternal(source, destination);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::Renderer::SeBlendEquation(const RendererBlendEquation blendEquation)
{
	s_Renderer->SetBlendEquationInternal(blendEquation);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::API::Renderer::SetCullMode(const RendererCullMode cullMode)
{
	s_Renderer->SetCullModeInternal(cullMode);
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string_view TRAP::Graphics::API::Renderer::GetTitle()
{
	return s_Renderer->GetTitleInternal();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::Renderer* TRAP::Graphics::API::Renderer::GetRenderer()
{
	return s_Renderer.get();
}

#endif /*_TRAP_RENDERER_H_*/