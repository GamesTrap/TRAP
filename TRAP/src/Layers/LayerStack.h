#pragma once

namespace TRAP {

	class LayerStack
	{
	public:
		void PushLayer(std::unique_ptr<Layer> layer);
		void PushOverlay(std::unique_ptr<Layer> overlay);
		void PopLayer(const std::unique_ptr<Layer>& layer);
		void PopOverlay(const std::unique_ptr<Layer>& overlay);

		std::vector<std::unique_ptr<Layer>>::iterator begin() { return m_layers.begin(); }
		std::vector<std::unique_ptr<Layer>>::iterator end() { return m_layers.end(); }

	private:
		std::vector<std::unique_ptr<Layer>> m_layers;
		unsigned int m_layerInsertIndex = 0;
	};

}