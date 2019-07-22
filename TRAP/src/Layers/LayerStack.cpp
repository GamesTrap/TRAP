#include "TRAPPCH.h"
#include "LayerStack.h"

#include "Layers/Layer.h"

TRAP::LayerStack::~LayerStack()
{
	for(const auto& layer : m_layers)
	{
		TP_DEBUG("[LayerStack] Destroying Layer: ", layer->GetName());
		layer->OnDetach();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::LayerStack::PushLayer(std::unique_ptr<Layer> layer)
{
	TP_DEBUG("[LayerStack] Pushing Layer: ", layer->GetName());
	layer->OnAttach();
	m_layers.insert(m_layers.begin() + m_layerInsertIndex, std::move(layer));
	m_layerInsertIndex++;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::LayerStack::PushOverlay(std::unique_ptr<Layer> overlay)
{
	TP_DEBUG("[LayerStack] Pushing Overlay: ", overlay->GetName());
	overlay->OnAttach();
	m_layers.push_back(std::move(overlay));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::LayerStack::PopLayer(const std::unique_ptr<Layer>& layer)
{
	const auto it = std::find(m_layers.begin(), m_layers.begin() + m_layerInsertIndex, layer);
	if (it != m_layers.end())
	{
		TP_DEBUG("[LayerStack] Destroying Layer: ", layer->GetName());
		layer->OnDetach();
		m_layers.erase(it);
		m_layerInsertIndex--;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::LayerStack::PopOverlay(const std::unique_ptr<Layer>& overlay)
{
	const auto it = std::find(m_layers.begin() + m_layerInsertIndex, m_layers.end(), overlay);
	if (it != m_layers.end())
	{
		TP_DEBUG("[LayerStack] Destroying Overlay: ", overlay->GetName());
		overlay->OnDetach();
		m_layers.erase(it);
	}
}