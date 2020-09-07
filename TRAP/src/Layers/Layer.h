#ifndef _TRAP_LAYER_H_
#define _TRAP_LAYER_H_

#include "Utils/Time/TimeStep.h"
#include "Events/Event.h"

namespace TRAP {

	class Layer
	{
	public:
		explicit Layer(std::string debugName = "Layer");
		Layer(const Layer&) = default;
		Layer& operator=(const Layer&) = default;
		Layer(Layer&&) = default;
		Layer& operator=(Layer&&) = default;
		virtual ~Layer() = default;

		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnUpdate(const Utils::TimeStep& deltaTime);
		virtual void OnTick();
		virtual void OnImGuiRender();
		virtual void OnEvent(Events::Event& event);

		const std::string& GetName() const;
		
	protected:
		std::string m_DebugName;
	};
}

#endif /*_TRAP_LAYER_H_*/