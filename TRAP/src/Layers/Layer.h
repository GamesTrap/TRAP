#ifndef _TRAP_LAYER_H_
#define _TRAP_LAYER_H_

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
		virtual void OnUpdate(Utils::TimeStep deltaTime) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
		virtual void OnResize(unsigned int width, unsigned int height) {}

		std::string_view GetName() const;
	protected:
		std::string m_DebugName;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Layer::Layer(std::string debugName)
	: m_DebugName(std::move(debugName))
{
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string_view TRAP::Layer::GetName() const
{
	return m_DebugName;
}

#endif /*_TRAP_LAYER_H_*/