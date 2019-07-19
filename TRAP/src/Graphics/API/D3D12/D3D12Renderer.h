#ifndef _TRAP_D3D12RENDERER_H_
#define _TRAP_D3D12RENDERER_H_

#ifdef TRAP_PLATFORM_WINDOWS

namespace TRAP::Graphics::API
{
	class D3D12Renderer final : public RendererAPI
	{
	public:
		D3D12Renderer();
		~D3D12Renderer();
		D3D12Renderer(const D3D12Renderer&) = default;
		D3D12Renderer& operator=(const D3D12Renderer&) = default;
		D3D12Renderer(D3D12Renderer&&) = default;
		D3D12Renderer& operator=(D3D12Renderer&&) = default;

		void InitInternal() override;

		void Clear(unsigned int buffer) override;
		void Present(Window* window) override;

		void SetClearColor(const Maths::Vec4& color = { 0.1f, 0.1f, 0.1f, 1.0f }) override;
		void SetDepthTesting(bool enabled) override;
		void SetBlend(bool enabled) override;
		void SetCull(bool enabled) override;
		void SetFrontFace(RendererFrontFace frontFace) override;
		void SetWireFrame(bool enabled) override;
		void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;

		void SetBlendFunction(RendererBlendFunction source, RendererBlendFunction destination) override;
		void SetBlendEquation(RendererBlendEquation blendEquation) override;

		void SetCullMode(RendererCullMode cullMode) override;

		void DrawIndexed(const std::unique_ptr<VertexArray>& vertexArray) override;

		std::string_view GetTitle() const override;

		static D3D12Renderer* Get();

	private:
		D3D12Context* m_context;
		std::string m_rendererTitle;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string_view TRAP::Graphics::API::D3D12Renderer::GetTitle() const
{	
	return m_rendererTitle;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::API::D3D12Renderer* TRAP::Graphics::API::D3D12Renderer::Get()
{
	return reinterpret_cast<D3D12Renderer*>(s_Renderer.get());
}

#endif

#endif /*_TRAP_D3D12RENDERER_H_*/