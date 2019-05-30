#ifndef _TRAP_RENDERER_H_
#define _TRAP_RENDERER_H_

namespace TRAP
{
	class Window;
}

namespace TRAP::Graphics
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

		static void Clear(unsigned int buffer);
		static void Present(Window* window);

		static void SetDepthTesting(bool enabled);
		static void SetBlend(bool enabled);
		static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

		static void SetBlendFunction(RendererBlendFunction source, RendererBlendFunction destination);
		static void SeBlendEquation(RendererBlendEquation blendEquation);

		static std::string_view GetTitle();

		static Renderer* GetRenderer();

	protected:
		virtual void InitInternal() = 0;

		virtual void ClearInternal(unsigned int buffer) = 0;
		virtual void PresentInternal(Window* window) = 0;

		virtual void SetDepthTestingInternal(bool enabled) = 0;
		virtual void SetBlendInternal(bool enabled) = 0;
		virtual void SetViewportInternal(unsigned int x, unsigned int y, unsigned int width, unsigned int height) = 0;

		virtual void SetBlendFunctionInternal(RendererBlendFunction source, RendererBlendFunction destination) = 0;
		virtual void SetBlendEquationInternal(RendererBlendEquation blendEquation) = 0;

		virtual std::string_view GetTitleInternal() const = 0;

		static std::unique_ptr<Renderer> s_Instance;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::Renderer::Clear(const unsigned int buffer)
{
	s_Instance->ClearInternal(buffer);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::Renderer::Present(Window* window)
{
	s_Instance->PresentInternal(window);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::Renderer::SetDepthTesting(const bool enabled)
{
	s_Instance->SetDepthTestingInternal(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::Renderer::SetBlend(const bool enabled)
{
	s_Instance->SetBlendInternal(enabled);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::Renderer::SetViewport(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height)
{
	s_Instance->SetViewportInternal(x, y, width, height);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::Renderer::SetBlendFunction(const RendererBlendFunction source, const RendererBlendFunction destination)
{
	s_Instance->SetBlendFunctionInternal(source, destination);
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::Renderer::SeBlendEquation(const RendererBlendEquation blendEquation)
{
	s_Instance->SetBlendEquationInternal(blendEquation);
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string_view TRAP::Graphics::Renderer::GetTitle()
{
	return s_Instance->GetTitleInternal();
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::Renderer* TRAP::Graphics::Renderer::GetRenderer()
{
	return s_Instance.get();
}

#endif /*_TRAP_RENDERER_H_*/