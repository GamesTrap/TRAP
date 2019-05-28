#pragma once

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

		static void Clear(const unsigned int buffer) { s_Instance->ClearInternal(buffer); }
		static void Present(Window* window) { s_Instance->PresentInternal(window); }

		static void SetDepthTesting(const bool enabled) { s_Instance->SetDepthTestingInternal(enabled); }
		static void SetBlend(const bool enabled) { s_Instance->SetBlendInternal(enabled); }
		static void SetViewport(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height) { s_Instance->SetViewportInternal(x, y, width, height); }

		static void SetBlendFunction(const RendererBlendFunction source, const RendererBlendFunction destination) { s_Instance->SetBlendFunctionInternal(source, destination); }
		static void SeBlendEquation(const RendererBlendEquation blendEquation) { s_Instance->SetBlendEquationInternal(blendEquation); }

		static std::string_view GetTitle() { return s_Instance->GetTitleInternal(); }

		static Renderer* GetRenderer() { return s_Instance.get(); }

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
	private:
	};
}
