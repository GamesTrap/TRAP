#pragma once

namespace TRAP {

	class Layer
	{
	public:
		explicit Layer(std::string name = "Layer");
		Layer(const Layer&) = default;
		Layer& operator=(const Layer&) = default;
		Layer(Layer&&) = default;
		Layer& operator=(Layer&&) = default;
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
		virtual void OnResize(unsigned int width, unsigned int height) {}

		std::string_view GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}