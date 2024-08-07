#ifndef TRAP_LAYER_H
#define TRAP_LAYER_H

#include <string>

#include "Utils/Time/TimeStep.h"

namespace TRAP::Events
{
	class Event;
}

namespace TRAP
{
	class Layer
	{
	public:
		/// @brief Constructor.
		/// @param debugName Layer name for debugging.
		constexpr explicit Layer(std::string debugName = "Layer") noexcept;
		/// @brief Copy constructor.
		consteval Layer(const Layer&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval Layer& operator=(const Layer&) noexcept = delete;
		/// @brief Move constructor.
		constexpr Layer(Layer&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr Layer& operator=(Layer&&) noexcept = default;
		/// @brief Destructor.
		constexpr virtual ~Layer() = default;

		/// @brief Called when Layer gets pushed to a layer stack.
		virtual void OnAttach();
		/// @brief Called when Layer gets popped from a layer stack.
		virtual void OnDetach();
		/// @brief Called every frame.
		/// @param deltaTime Time difference between current and last frame.
		virtual void OnUpdate(const Utils::TimeStep& deltaTime);
		/// @brief Called every tick (default 100 ticks).
		/// @param deltaTime Time difference between current and last tick.
		virtual void OnTick(const Utils::TimeStep& deltaTime);
		/// @brief Called every frame.
		/// Should be used for debug GUIs.
		virtual void OnImGuiRender();
		/// @brief Called when an event occurs.
		/// @param event Event that occurred.
		virtual void OnEvent(Events::Event& event);

		/// @brief Retrieve the debug name of the layer.
		/// @return Name of the layer.
		[[nodiscard]] constexpr std::string GetName() const noexcept;

	protected:
		std::string m_DebugName;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Layer::Layer(std::string debugName) noexcept
	: m_DebugName(std::move(debugName))
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Layer::GetName() const noexcept
{
	return m_DebugName;
}

#endif /*TRAP_LAYER_H*/