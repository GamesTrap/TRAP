#include "TRAPPCH.h"
#include "LayerStack.h"

namespace TRAP {
	void LayerStack::PushLayer(std::unique_ptr<Layer> layer)
	{
		m_layers.insert(m_layers.begin() + m_layerInsertIndex, std::move(layer));
		m_layerInsertIndex++;
	}

	void LayerStack::PushOverlay(std::unique_ptr<Layer> overlay)
	{
		m_layers.push_back(std::move(overlay));
	}

	void LayerStack::PopLayer(const std::unique_ptr<Layer>& layer)
	{
		const auto it = std::find(m_layers.begin(), m_layers.end(), layer);
		if(it != m_layers.end())
		{
			m_layers.erase(it);
			m_layerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(const std::unique_ptr<Layer>& overlay)
	{
		const auto it = std::find(m_layers.begin(), m_layers.end(), overlay);
		if (it != m_layers.end())
			m_layers.erase(it);
	}
}