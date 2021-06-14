#ifndef _TRAP_LAYERSTACK_H_
#define _TRAP_LAYERSTACK_H_

#include "Layer.h"

namespace TRAP 
{
	class LayerStack
	{
	public:
		/// <summary>
		/// Default Constructor.
		/// </summary>
		LayerStack() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		LayerStack(const LayerStack&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		LayerStack& operator=(const LayerStack&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		LayerStack(LayerStack&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		LayerStack& operator=(LayerStack&&) = default;
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~LayerStack();

		/// <summary>
		/// Push a Layer to the LayerStack.
		/// </summary>
		/// <param name="layer">Layer to push.</param>
		void PushLayer(std::unique_ptr<Layer> layer);
		/// <summary>
		/// Push an Overlay to the LayerStack.
		/// </summary>
		/// <param name="overlay">Overlay to push.</param>
		void PushOverlay(std::unique_ptr<Layer> overlay);
		/// <summary>
		/// Pop a Layer from the LayerStack.
		/// </summary>
		/// <param name="layer">Layer to pop.</param>
		void PopLayer(const std::unique_ptr<Layer>& layer);
		/// <summary>
		/// Pop an Overlay from the LayerStack.
		/// </summary>
		/// <param name="overlay">Overlay to pop.</param>
		void PopOverlay(const std::unique_ptr<Layer>& overlay);
		/// <summary>
		/// Pop a Layer from the LayerStack.
		/// </summary>
		/// <param name="layer">Layer to pop.</param>
		void PopLayer(Layer* layer);
		/// <summary>
		/// Pop an Overlay from the LayerStack.
		/// </summary>
		/// <param name="overlay">Overlay to pop.</param>
		void PopOverlay(Layer* overlay);

		/// <summary>
		/// LayerStack begin iterator.
		/// </summary>
		/// <returns>LayerStack iterator.</returns>
		std::vector<std::unique_ptr<Layer>>::iterator begin();
		/// <summary>
		/// LayerStack end iterator.
		/// </summary>
		/// <returns>LayerStack iterator.</returns>
		std::vector<std::unique_ptr<Layer>>::iterator end();
		/// <summary>
		/// LayerStack reverse begin iterator.
		/// </summary>
		/// <returns>LayerStack reverse iterator.</returns>
		std::vector<std::unique_ptr<Layer>>::reverse_iterator rbegin();
		/// <summary>
		/// LayerStack reverse end iterator.
		/// </summary>
		/// <returns>LayerStack reverse iterator.</returns>
		std::vector<std::unique_ptr<Layer>>::reverse_iterator rend();

		/// <summary>
		/// LayerStack const begin iterator.
		/// </summary>
		/// <returns>LayerStack const iterator.</returns>
		std::vector<std::unique_ptr<Layer>>::const_iterator begin() const;
		/// <summary>
		/// LayerStack const end iterator.
		/// </summary>
		/// <returns>LayerStack const iterator.</returns>
		std::vector<std::unique_ptr<Layer>>::const_iterator end() const;
		/// <summary>
		/// LayerStack const reverse begin iterator.
		/// </summary>
		/// <returns>LayerStack reverse iterator.</returns>
		std::vector<std::unique_ptr<Layer>>::const_reverse_iterator rbegin() const;
		/// <summary>
		/// LayerStack const reverse end iterator.
		/// </summary>
		/// <returns>LayerStack reverse iterator.</returns>
		std::vector<std::unique_ptr<Layer>>::const_reverse_iterator rend() const;

	private:
		std::vector<std::unique_ptr<Layer>> m_layers;
		uint32_t m_layerInsertIndex = 0;
	};
}

#endif /*_TRAP_LAYERSTACK_H_*/