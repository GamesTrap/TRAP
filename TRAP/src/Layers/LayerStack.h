#ifndef _TRAP_LAYERSTACK_H_
#define _TRAP_LAYERSTACK_H_

#include "Layer.h"

namespace TRAP 
{
	class LayerStack
	{
	public:
		LayerStack() = default;
		LayerStack(const LayerStack&) = default;
		LayerStack& operator=(const LayerStack&) = default;
		LayerStack(LayerStack&&) = default;
		LayerStack& operator=(LayerStack&&) = default;
		~LayerStack();

		void PushLayer(std::unique_ptr<Layer> layer);
		void PushOverlay(std::unique_ptr<Layer> overlay);
		void PopLayer(const std::unique_ptr<Layer>& layer);
		void PopOverlay(const std::unique_ptr<Layer>& overlay);

		std::vector<std::unique_ptr<Layer>>::iterator begin();
		std::vector<std::unique_ptr<Layer>>::iterator end();
		std::vector<std::unique_ptr<Layer>>::reverse_iterator rbegin();
		std::vector<std::unique_ptr<Layer>>::reverse_iterator rend();

		std::vector<std::unique_ptr<Layer>>::const_iterator begin() const;
		std::vector<std::unique_ptr<Layer>>::const_iterator end()	const;
		std::vector<std::unique_ptr<Layer>>::const_reverse_iterator rbegin() const;
		std::vector<std::unique_ptr<Layer>>::const_reverse_iterator rend() const;

	private:
		std::vector<std::unique_ptr<Layer>> m_layers;
		uint32_t m_layerInsertIndex = 0;
	};

}

#endif /*_TRAP_LAYERSTACK_H_*/