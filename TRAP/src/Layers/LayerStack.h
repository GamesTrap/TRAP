#ifndef _TRAP_LAYERSTACK_H_
#define _TRAP_LAYERSTACK_H_

namespace TRAP {

	class LayerStack
	{
	public:
		void PushLayer(std::unique_ptr<Layer> layer);
		void PushOverlay(std::unique_ptr<Layer> overlay);
		void PopLayer(const std::unique_ptr<Layer>& layer);
		void PopOverlay(const std::unique_ptr<Layer>& overlay);

		std::vector<std::unique_ptr<Layer>>::iterator begin();
		std::vector<std::unique_ptr<Layer>>::iterator end();

	private:
		std::vector<std::unique_ptr<Layer>> m_layers;
		unsigned int m_layerInsertIndex = 0;
	};

}

//-------------------------------------------------------------------------------------------------------------------//

inline std::vector<std::unique_ptr<TRAP::Layer>>::iterator TRAP::LayerStack::begin()
{
	return m_layers.begin();
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::vector<std::unique_ptr<TRAP::Layer>>::iterator TRAP::LayerStack::end()
{
	return m_layers.end();
}

#endif /*_TRAP_LAYERSTACK_H_*/