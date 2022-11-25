#include "TRAPPCH.h"
#include "LayerStack.h"

#include "Layers/Layer.h"

void TRAP::LayerStack::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

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
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	TP_DEBUG(Log::LayerStackPrefix, "Pushing layer: ", layer->GetName());
	layer->OnAttach();
	m_layers.insert(m_layers.begin() + m_layerInsertIndex, std::move(layer));
	m_layerInsertIndex++;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::LayerStack::PushOverlay(std::unique_ptr<Layer> overlay)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	TP_DEBUG(Log::LayerStackPrefix, "Pushing overlay: ", overlay->GetName());
	overlay->OnAttach();
	m_layers.emplace_back(std::move(overlay));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::LayerStack::PopLayer(Layer* const layer)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

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
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

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

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Layer>>::iterator TRAP::LayerStack::begin()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_layers.begin();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Layer>>::iterator TRAP::LayerStack::end()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_layers.end();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Layer>>::reverse_iterator TRAP::LayerStack::rbegin()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_layers.rbegin();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Layer>>::reverse_iterator TRAP::LayerStack::rend()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_layers.rend();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Layer>>::const_iterator TRAP::LayerStack::begin() const
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_layers.begin();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Layer>>::const_iterator TRAP::LayerStack::end() const
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_layers.end();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Layer>>::const_reverse_iterator TRAP::LayerStack::rbegin() const
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_layers.rbegin();
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::unique_ptr<TRAP::Layer>>::const_reverse_iterator TRAP::LayerStack::rend() const
{
	ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_layers.rend();
}