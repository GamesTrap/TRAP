#ifndef TRAP_LAYERSTACK_H
#define TRAP_LAYERSTACK_H

#include "Core/Base.h"

namespace TRAP
{
	class Layer;

	class LayerStack
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		LayerStack() = default;

		/// <summary>
		/// Shut down the LayerStack.
		/// This detaches and destroys all Layers.
		/// </summary>
		void Shutdown();

		/// <summary>
		/// Push a layer to the layer stack.
		/// </summary>
		/// <param name="layer">Layer to push.</param>
		void PushLayer(std::unique_ptr<Layer> layer);
		/// <summary>
		/// Push an overlay to the layer stack.
		/// </summary>
		/// <param name="overlay">Overlay to push.</param>
		void PushOverlay(std::unique_ptr<Layer> overlay);
		/// <summary>
		/// Pop a layer from the layer stack.
		/// </summary>
		/// <param name="layer">Layer to pop.</param>
		void PopLayer(Layer* layer);
		/// <summary>
		/// Pop an overlay from the layer stack.
		/// </summary>
		/// <param name="overlay">Overlay to pop.</param>
		void PopOverlay(Layer* overlay);

		/// <summary>
		/// Layer stack begin iterator.
		/// </summary>
		/// <returns>Layer stack iterator.</returns>
		[[nodiscard]] std::vector<std::unique_ptr<Layer>>::iterator begin() noexcept;
		/// <summary>
		/// Layer stack end iterator.
		/// </summary>
		/// <returns>Layer stack iterator.</returns>
		[[nodiscard]] std::vector<std::unique_ptr<Layer>>::iterator end() noexcept;
		/// <summary>
		/// Layer stack reverse begin iterator.
		/// </summary>
		/// <returns>Layer stack reverse iterator.</returns>
		[[nodiscard]] std::vector<std::unique_ptr<Layer>>::reverse_iterator rbegin() noexcept;
		/// <summary>
		/// Layer stack reverse end iterator.
		/// </summary>
		/// <returns>Layer stack reverse iterator.</returns>
		[[nodiscard]] std::vector<std::unique_ptr<Layer>>::reverse_iterator rend() noexcept;

		/// <summary>
		/// Layer stack const begin iterator.
		/// </summary>
		/// <returns>Layer stack const iterator.</returns>
		[[nodiscard]] std::vector<std::unique_ptr<Layer>>::const_iterator begin() const noexcept;
		/// <summary>
		/// Layer stack const end iterator.
		/// </summary>
		/// <returns>Layer stack const iterator.</returns>
		[[nodiscard]] std::vector<std::unique_ptr<Layer>>::const_iterator end() const noexcept;
		/// <summary>
		/// Layer stack const reverse begin iterator.
		/// </summary>
		/// <returns>Layer stack reverse iterator.</returns>
		[[nodiscard]] std::vector<std::unique_ptr<Layer>>::const_reverse_iterator rbegin() const noexcept;
		/// <summary>
		/// Layer stack const reverse end iterator.
		/// </summary>
		/// <returns>Layer stack reverse iterator.</returns>
		[[nodiscard]] std::vector<std::unique_ptr<Layer>>::const_reverse_iterator rend() const noexcept;

	private:
		std::vector<std::unique_ptr<Layer>> m_layers;
		uint32_t m_layerInsertIndex = 0;
	};
}

#endif /*TRAP_LAYERSTACK_H*/