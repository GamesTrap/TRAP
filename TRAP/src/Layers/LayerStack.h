#ifndef TRAP_LAYERSTACK_H
#define TRAP_LAYERSTACK_H

#include "Layer.h"

namespace TRAP
{
	class LayerStack
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		LayerStack() = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		LayerStack(const LayerStack&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		LayerStack& operator=(const LayerStack&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		LayerStack(LayerStack&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		LayerStack& operator=(LayerStack&&) = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~LayerStack();

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
		void PopLayer(const std::unique_ptr<Layer>& layer);
		/// <summary>
		/// Pop an overlay from the layer stack.
		/// </summary>
		/// <param name="overlay">Overlay to pop.</param>
		void PopOverlay(const std::unique_ptr<Layer>& overlay);
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
		std::vector<std::unique_ptr<Layer>>::iterator begin();
		/// <summary>
		/// Layer stack end iterator.
		/// </summary>
		/// <returns>Layer stack iterator.</returns>
		std::vector<std::unique_ptr<Layer>>::iterator end();
		/// <summary>
		/// Layers tack reverse begin iterator.
		/// </summary>
		/// <returns>Layer stack reverse iterator.</returns>
		std::vector<std::unique_ptr<Layer>>::reverse_iterator rbegin();
		/// <summary>
		/// Layer stack reverse end iterator.
		/// </summary>
		/// <returns>Layer stack reverse iterator.</returns>
		std::vector<std::unique_ptr<Layer>>::reverse_iterator rend();

		/// <summary>
		/// Layer stack const begin iterator.
		/// </summary>
		/// <returns>Layer stack const iterator.</returns>
		std::vector<std::unique_ptr<Layer>>::const_iterator begin() const;
		/// <summary>
		/// Layer stack const end iterator.
		/// </summary>
		/// <returns>Layer stack const iterator.</returns>
		std::vector<std::unique_ptr<Layer>>::const_iterator end() const;
		/// <summary>
		/// Layer stack const reverse begin iterator.
		/// </summary>
		/// <returns>Layer stack reverse iterator.</returns>
		std::vector<std::unique_ptr<Layer>>::const_reverse_iterator rbegin() const;
		/// <summary>
		/// Layer stack const reverse end iterator.
		/// </summary>
		/// <returns>Layer stack reverse iterator.</returns>
		std::vector<std::unique_ptr<Layer>>::const_reverse_iterator rend() const;

	private:
		std::vector<std::unique_ptr<Layer>> m_layers;
		uint32_t m_layerInsertIndex = 0;
	};
}

#endif /*TRAP_LAYERSTACK_H*/