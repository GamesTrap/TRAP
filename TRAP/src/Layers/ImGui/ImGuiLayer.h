#ifndef _TRAP_IMGUILAYER_H_
#define _TRAP_IMGUILAYER_H_

#include "Layers/Layer.h"

namespace TRAP
{
	class  ImGuiLayer final : public Layer
	{
	public:
		ImGuiLayer();
		ImGuiLayer(const ImGuiLayer&) = default;
		ImGuiLayer& operator=(const ImGuiLayer&) = default;
		ImGuiLayer(ImGuiLayer&&) = default;
		ImGuiLayer& operator=(ImGuiLayer&&) = default;
		~ImGuiLayer() = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Events::Event& event) override;

		static void Begin();
		static void End();
	private:
		float m_time = 0.0f;
	};
}

#endif /*_TRAP_IMGUILAYER_H_*/