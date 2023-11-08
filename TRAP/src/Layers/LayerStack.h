#ifndef TRAP_LAYERSTACK_H
#define TRAP_LAYERSTACK_H

#include <memory>
#include <vector>

#include "Layers/Layer.h"

namespace TRAP
{
	class LayerStack
	{
	public:
		/// @brief Constructor.
		constexpr LayerStack() = default;

		/// @brief Copy constructor.
		consteval LayerStack(const LayerStack &) noexcept = delete;
		/// @brief Move constructor.
		constexpr LayerStack(LayerStack &&) noexcept = default;
		/// @brief Copy assignment operator.
		consteval LayerStack &operator=(const LayerStack &) noexcept = delete;
		/// @brief Move assignment operator.
		constexpr LayerStack &operator=(LayerStack &&) noexcept = default;

		/// @brief Destructor.
		constexpr ~LayerStack() = default;

		/// @brief Shut down the LayerStack.
		/// This detaches and destroys all Layers.
		void Shutdown();

		/// @brief Push a layer to the layer stack.
		/// @param layer Layer to push.
		void PushLayer(std::unique_ptr<Layer> layer);
		/// @brief Push an overlay to the layer stack.
		/// @param overlay Overlay to push.
		void PushOverlay(std::unique_ptr<Layer> overlay);
		/// @brief Pop a layer from the layer stack.
		/// @param layer Layer to pop.
		void PopLayer(Layer* layer);
		/// @brief Pop an overlay from the layer stack.
		/// @param overlay Overlay to pop.
		void PopOverlay(Layer* overlay);

		/// @brief Layer stack begin iterator.
		/// @return Layer stack iterator.
		[[nodiscard]] constexpr std::vector<std::unique_ptr<Layer>>::iterator begin() noexcept;
		/// @brief Layer stack end iterator.
		/// @return Layer stack iterator.
		[[nodiscard]] constexpr std::vector<std::unique_ptr<Layer>>::iterator end() noexcept;
		/// @brief Layer stack reverse begin iterator.
		/// @return Layer stack reverse iterator.
		[[nodiscard]] constexpr std::vector<std::unique_ptr<Layer>>::reverse_iterator rbegin() noexcept;
		/// @brief Layer stack reverse end iterator.
		/// @return Layer stack reverse iterator.
		[[nodiscard]] constexpr std::vector<std::unique_ptr<Layer>>::reverse_iterator rend() noexcept;

		/// @brief Layer stack const begin iterator.
		/// @return Layer stack const iterator.
		[[nodiscard]] constexpr std::vector<std::unique_ptr<Layer>>::const_iterator begin() const noexcept;
		/// @brief Layer stack const end iterator.
		/// @return Layer stack const iterator.
		[[nodiscard]] constexpr std::vector<std::unique_ptr<Layer>>::const_iterator end() const noexcept;
		/// @brief Layer stack const reverse begin iterator.
		/// @return Layer stack reverse iterator.
		[[nodiscard]] constexpr std::vector<std::unique_ptr<Layer>>::const_reverse_iterator rbegin() const noexcept;
		/// @brief Layer stack const reverse end iterator.
		/// @return Layer stack reverse iterator.
		[[nodiscard]] constexpr std::vector<std::unique_ptr<Layer>>::const_reverse_iterator rend() const noexcept;

	private:
		std::vector<std::unique_ptr<Layer>> m_layers;
		u32 m_layerInsertIndex = 0;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::unique_ptr<TRAP::Layer>>::iterator TRAP::LayerStack::begin() noexcept
{
	return m_layers.begin();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::unique_ptr<TRAP::Layer>>::iterator TRAP::LayerStack::end() noexcept
{
	return m_layers.end();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::unique_ptr<TRAP::Layer>>::reverse_iterator TRAP::LayerStack::rbegin() noexcept
{
	return m_layers.rbegin();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::unique_ptr<TRAP::Layer>>::reverse_iterator TRAP::LayerStack::rend() noexcept
{
	return m_layers.rend();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::unique_ptr<TRAP::Layer>>::const_iterator TRAP::LayerStack::begin() const noexcept
{
	return m_layers.begin();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::unique_ptr<TRAP::Layer>>::const_iterator TRAP::LayerStack::end() const noexcept
{
	return m_layers.end();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::unique_ptr<TRAP::Layer>>::const_reverse_iterator TRAP::LayerStack::rbegin() const noexcept
{
	return m_layers.rbegin();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::unique_ptr<TRAP::Layer>>::const_reverse_iterator TRAP::LayerStack::rend() const noexcept
{
	return m_layers.rend();
}

#endif /*TRAP_LAYERSTACK_H*/