#ifndef _TRAP_IMGUILAYER_H_
#define _TRAP_IMGUILAYER_H_

#include "Layers/Layer.h"

namespace TRAP
{
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
	private:
		float m_time = 0.0f;
	};
}

#endif /*_TRAP_IMGUILAYER_H_*/