#ifndef _TRAP_LAYER_H_
#define _TRAP_LAYER_H_

#include "Utils/Time/TimeStep.h"
#include "Events/Event.h"

namespace TRAP {

	class Layer
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="debugName">Layer name for debugging.</param>
		explicit Layer(std::string debugName = "Layer");
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		Layer(const Layer&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		Layer& operator=(const Layer&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		Layer(Layer&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		Layer& operator=(Layer&&) = default;
		/// <summary>
		/// Virtual Default Destructor.
		/// </summary>
		virtual ~Layer() = default;

		/// <summary>
		/// Called when Layer gets pushed to a LayerStack.
		/// </summary>
		virtual void OnAttach();
		/// <summary>
		/// Called when Layer gets popped from a LayerStack.
		/// </summary>
		virtual void OnDetach();
		/// <summary>
		/// Called every frame.
		/// </summary>
		/// <param name="deltaTime">Time difference between current and last frame.</param>
		virtual void OnUpdate(const Utils::TimeStep& deltaTime);
		/// <summary>
		/// Called every tick (default 100 ticks).
		/// </summary>
		virtual void OnTick();
		/// <summary>
		/// Called every frame.<br>
		/// Should be used for debug GUIs.
		/// </summary>
		virtual void OnImGuiRender();
		/// <summary>
		/// Called when an Event occurs.
		/// </summary>
		/// <param name="event">Event that occurred.</param>
		virtual void OnEvent(Events::Event& event);

		/// <summary>
		/// Retrieve the debug name of the Layer.
		/// </summary>
		/// <returns>Name of the Layer.</returns>
		const std::string& GetName() const;
		
	protected:
		std::string m_DebugName;
	};
}

#endif /*_TRAP_LAYER_H_*/