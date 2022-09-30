#include "TRAPPCH.h"
#include "LayerStack.h"

#include "Layers/Layer.h"

void TRAP::LayerStack::Shutdown()
{
	for(auto& layer : m_layers)
	{
		TP_DEBUG(Log::LayerStackPrefix, "Destroying layer: ", layer->GetName());
		layer->OnDetach();
		layer.reset();
	}

	m_layers.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::LayerStack::PushLayer(std::unique_ptr<Layer> layer)
{
	TP_DEBUG(Log::LayerStackPrefix, "Pushing layer: ", layer->GetName());
	layer->OnAttach();
	m_layers.insert(m_layers.begin() + m_layerInsertIndex, std::move(layer));
	m_layerInsertIndex++;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::LayerStack::PushOverlay(std::unique_ptr<Layer> overlay)
{
	TP_DEBUG(Log::LayerStackPrefix, "Pushing overlay: ", overlay->GetName());
	overlay->OnAttach();
	m_layers.emplace_back(std::move(overlay));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::LayerStack::PopLayer(Layer* const layer)
{
	TRAP_ASSERT(layer, "Layer is nullptr!");

	const auto it = std::find_if(m_layers.begin(), m_layers.begin() + m_layerInsertIndex,
		                         [&layer](const std::unique_ptr<Layer>& l){return l.get() == layer;});
	if (it != m_layers.end())
	{
		TP_DEBUG(Log::LayerStackPrefix, "Destroying layer: ", layer->GetName());
		layer->OnDetach();
		m_layers.erase(it);
		m_layerInsertIndex--;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::LayerStack::PopOverlay(Layer* const overlay)
{
	TRAP_ASSERT(overlay, "Overlay is nullptr!");

	const auto it = std::find_if(m_layers.begin() + m_layerInsertIndex, m_layers.end(),
		                         [&overlay](const std::unique_ptr<Layer>& l){return l.get() == overlay;});
	if (it != m_layers.end())
	{
		TP_DEBUG(Log::LayerStackPrefix, "Destroying overlay: ", overlay->GetName());
		overlay->OnDetach();
		m_layers.erase(it);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Layer>>::iterator TRAP::LayerStack::begin()
{
	return m_layers.begin();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Layer>>::iterator TRAP::LayerStack::end()
{
	return m_layers.end();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Layer>>::reverse_iterator TRAP::LayerStack::rbegin()
{
	return m_layers.rbegin();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Layer>>::reverse_iterator TRAP::LayerStack::rend()
{
	return m_layers.rend();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Layer>>::const_iterator TRAP::LayerStack::begin() const
{
	return m_layers.begin();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Layer>>::const_iterator TRAP::LayerStack::end() const
{
	return m_layers.end();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Layer>>::const_reverse_iterator TRAP::LayerStack::rbegin() const
{

	return m_layers.rbegin();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Layer>>::const_reverse_iterator TRAP::LayerStack::rend() const
{
	return m_layers.rend();
}