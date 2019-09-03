#ifndef _TRAP_RENDERER_H_
#define _TRAP_RENDERER_H_

#include "API/RendererAPI.h"
#include "Buffers/UniformBuffer.h"
#include "Application.h"
#include "RenderCommand.h"

namespace TRAP::Graphics
{
	class Shader;

	class OrthographicCamera;

	class Renderer
	{
	public:
		static std::string_view GetTitle();
		static unsigned int GetDrawCalls();
		static unsigned int GetFPS();
		static float GetFrameTime();
		static unsigned int GetTickRate();

		static void SetTickRate(unsigned int tickRate);

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Shader* shader, const std::unique_ptr<VertexArray>& vertexArray, const Math::Mat4& transform = Math::Mat4::Identity(), RendererPrimitive primitive = RendererPrimitive::Triangle);

		static void Cleanup();

	private:
		struct SceneData
		{
			SceneData()
				: m_projectionMatrix(0.0f), m_viewMatrix(0.0f), m_modelMatrix(0.0f)
			{
			}

			Math::Mat4 m_projectionMatrix;
			Math::Mat4 m_viewMatrix;
			Math::Mat4 m_modelMatrix;
		};

		static std::unique_ptr<SceneData> s_sceneData;
		static std::unique_ptr<UniformBuffer> s_uniformBuffer;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string_view TRAP::Graphics::Renderer::GetTitle()
{
	return API::RendererAPI::GetRenderer()->GetTitle();
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Graphics::Renderer::GetDrawCalls()
{
	return Application::Get().GetDrawCalls();
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Graphics::Renderer::GetFPS()
{
	return Application::Get().GetFPS();
}

//-------------------------------------------------------------------------------------------------------------------//

inline float TRAP::Graphics::Renderer::GetFrameTime()
{
	return Application::Get().GetFrameTime();
}

//-------------------------------------------------------------------------------------------------------------------//

inline unsigned int TRAP::Graphics::Renderer::GetTickRate()
{
	return Application::Get().GetTickRate();
}

//-------------------------------------------------------------------------------------------------------------------//

inline void TRAP::Graphics::Renderer::SetTickRate(const unsigned int tickRate)
{
	Application::Get().SetTickRate(tickRate);
}

#endif /*_TRAP_RENDERER_H_*/