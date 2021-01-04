#ifndef _GAMESTRAP_VULKANTESTS_H_
#define _GAMESTRAP_VULKANTESTS_H_

#include <TRAP.h>

namespace TRAP::Graphics::API
{
	class VulkanCommandPool;
}

class VulkanTests final : public TRAP::Layer
{
public:
	VulkanTests();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;

	//void OnEvent(TRAP::Events::Event& event) override;

private:
	//bool OnWindowClose(TRAP::Events::WindowCloseEvent& e);
	
	//TRAP::Scope<TRAP::Window> m_window;

	TRAP::Utils::Timer m_fpsTimer;

	inline static constexpr uint32_t ImageCount = 3; //Triple Buffered
	TRAP::Ref<TRAP::Graphics::API::VulkanQueue> m_graphicsQueue;
	std::array<TRAP::Ref<TRAP::Graphics::API::VulkanCommandPool>, ImageCount> m_cmdPools;
	std::array<TRAP::Graphics::CommandBuffer*, ImageCount> m_cmds;

	TRAP::Ref<TRAP::Graphics::API::VulkanSwapChain> m_swapChain;
	std::array<TRAP::Ref<TRAP::Graphics::API::VulkanFence>, ImageCount> m_renderCompleteFences;
	TRAP::Ref<TRAP::Graphics::API::VulkanSemaphore> m_imageAcquiredSemaphore;
	std::array<TRAP::Ref<TRAP::Graphics::API::VulkanSemaphore>, ImageCount> m_renderCompleteSemaphores;

	TRAP::Ref<TRAP::Graphics::API::VulkanShader> m_defaultShader;
	TRAP::Ref<TRAP::Graphics::RootSignature> m_rootSignature;
	TRAP::Ref<TRAP::Graphics::API::VulkanPipeline> m_defaultPipeline;

	uint32_t m_frameIndex;
};

#endif /*_GAMESTRAP_VULKANTESTS_H_*/