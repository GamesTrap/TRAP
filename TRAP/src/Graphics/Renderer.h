#ifndef _TRAP_RENDERER_H_
#define _TRAP_RENDERER_H_

namespace TRAP::Graphics
{
	class Renderer
	{
	public:
		static std::string_view GetTitle();

		static void BeginScene();
		static void EndScene();

		static void Submit(const std::unique_ptr<VertexArray>& vertexArray);
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string_view TRAP::Graphics::Renderer::GetTitle()
{
	return API::RendererAPI::GetRenderer()->GetTitle();
}

#endif /*_TRAP_RENDERER_H_*/