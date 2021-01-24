#ifndef _GAMESTRAP_VULKANTESTS_H_
#define _GAMESTRAP_VULKANTESTS_H_

#include <TRAP.h>

namespace TRAP::Graphics
{
	class Pipeline;
}

class VulkanTests final : public TRAP::Layer
{
public:
	VulkanTests();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnWindowClose(TRAP::Events::WindowCloseEvent& e);
	
	TRAP::Scope<TRAP::Window> m_window;

	TRAP::Utils::Timer m_fpsTimer;

	TRAP::Graphics::Shader* m_defaultShader;
	TRAP::Ref<TRAP::Graphics::RootSignature> m_rootSignature;
	TRAP::Ref<TRAP::Graphics::Pipeline> m_defaultPipeline;

	TRAP::Ref<TRAP::Graphics::PipelineCache> m_defaultPipelineCache;

	TRAP::Ref<TRAP::Graphics::Buffer> m_triangleVertexBuffer;

	std::array<float, 18> m_triangleVertices
	{
		//XYZ RGB
		 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};
};

#endif /*_GAMESTRAP_VULKANTESTS_H_*/