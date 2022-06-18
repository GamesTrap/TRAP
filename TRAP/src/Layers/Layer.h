#ifndef TRAP_LAYER_H
#define TRAP_LAYER_H

#include "Utils/Time/TimeStep.h"
#include "Events/Event.h"

namespace TRAP
{
	class Layer
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="debugName">Layer name for debugging.</param>
		explicit Layer(std::string debugName = "Layer");
		/// <summary>
		/// Copy constructor.
		/// </summary>
		Layer(const Layer&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Layer& operator=(const Layer&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Layer(Layer&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Layer& operator=(Layer&&) = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Layer() = default;

		/// <summary>
		/// Called when Layer gets pushed to a layer stack.
		/// </summary>
		virtual void OnAttach();
		/// <summary>
		/// Called when Layer gets popped from a layer stack.
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
		/// Called every frame.
		/// Should be used for debug GUIs.
		/// </summary>
		virtual void OnImGuiRender();
		/// <summary>
		/// Called when an event occurs.
		/// </summary>
		/// <param name="event">Event that occurred.</param>
		virtual void OnEvent(Events::Event& event);

		/// <summary>
		/// Retrieve the debug name of the layer.
		/// </summary>
		/// <returns>Name of the layer.</returns>
		const std::string& GetName() const;

	protected:
		std::string m_DebugName;
	};
}

#endif /*TRAP_LAYER_H*/