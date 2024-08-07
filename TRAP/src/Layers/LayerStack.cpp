#include "TRAPPCH.h"
#include "LayerStack.h"

#include "Layers/Layer.h"

void TRAP::LayerStack::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None);

	for(auto& layer : m_layers)
	{
		TP_DEBUG(Log::LayerStackPrefix, "Destroying layer: ", layer->GetName());
		layer->OnDetach();
	}

	m_layers.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::LayerStack::PushLayer(std::unique_ptr<Layer> layer)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None);

	TP_DEBUG(Log::LayerStackPrefix, "Pushing layer: ", layer->GetName());
	layer->OnAttach();
	m_layers.insert(m_layers.begin() + m_layerInsertIndex, std::move(layer));
	m_layerInsertIndex++;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::LayerStack::PushOverlay(std::unique_ptr<Layer> overlay)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None);

	TP_DEBUG(Log::LayerStackPrefix, "Pushing overlay: ", overlay->GetName());
	overlay->OnAttach();
	m_layers.emplace_back(std::move(overlay));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::LayerStack::PopLayer(Layer* const layer)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None);

	TRAP_ASSERT(layer, "LayerStack::PopLayer(): Layer is nullptr!");

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
	ZoneNamedC(__tracy, tracy::Color::Brown, (GetTRAPProfileSystems() & ProfileSystems::Layers) != ProfileSystems::None);

	TRAP_ASSERT(overlay, "LayerStack::PopOverlay(): Overlay is nullptr!");

	const auto it = std::find_if(m_layers.begin() + m_layerInsertIndex, m_layers.end(),
		                         [&overlay](const std::unique_ptr<Layer>& l){return l.get() == overlay;});
	if (it != m_layers.end())
	{
		TP_DEBUG(Log::LayerStackPrefix, "Destroying overlay: ", overlay->GetName());
		overlay->OnDetach();
		m_layers.erase(it);
	}
}
