#pragma once

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

		static void Begin();
		static void End();
	private:
		float m_time = 0.0f;
	};
}
