#ifndef _TRAP_IMGUILAYER_H_
#define _TRAP_IMGUILAYER_H_

#include "Layers/Layer.h"

#include "TRAPPCH.h"

namespace TRAP
{
	namespace Graphics
	{
		class PipelineCache;
	}

	class  ImGuiLayer final : public Layer
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		ImGuiLayer();
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		ImGuiLayer(const ImGuiLayer&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		ImGuiLayer& operator=(const ImGuiLayer&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		ImGuiLayer(ImGuiLayer&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		ImGuiLayer& operator=(ImGuiLayer&&) = default;
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~ImGuiLayer() = default;

		/// <summary>
		/// Called when pushed to a LayerStack.
		/// </summary>
		void OnAttach() override;
		/// <summary>
		/// Called when popped from a LayerStack.
		/// </summary>
		void OnDetach() override;
		/// <summary>
		/// Called when an Event occurs.
		/// </summary>
		/// <param name="event">Event that occurred.</param>
		void OnEvent(Events::Event& event) override;

		/// <summary>
		/// Begin a new ImGui frame.
		/// </summary>
		static void Begin();
		/// <summary>
		/// End an ImGui frame.
		/// </summary>
		static void End();

		/// <summary>
		/// Block ImGui event handling.
		/// </summary>
		/// <param name="block">Whether to block events or not</param>
		void BlockEvents(bool block);

		static void SetDarkThemeColors();
	private:
		bool m_blockEvents;
		float m_time = 0.0f;

		TRAP::Ref<TRAP::Graphics::PipelineCache> m_imguiPipelineCache;
		VkDescriptorPool m_imguiDescriptorPool;

		std::vector<VkDescriptorPoolSize> m_descriptorPoolSizes =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
	};
}

#endif /*_TRAP_IMGUILAYER_H_*/