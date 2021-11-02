#ifndef TRAPEDITOR_SCENEGRAPHPANEL_H
#define TRAPEDITOR_SCENEGRAPHPANEL_H

#include <Scene/Scene.h>
#include <Core/Base.h>
#include <Scene/Entity.h>

#include "PropertiesPanel.h"

namespace TRAP
{
	class SceneGraphPanel
	{
	public:
		SceneGraphPanel() = default;
		explicit SceneGraphPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);

		Ref<Scene> m_context;
		Entity m_selectionContext;
		PropertiesPanel m_propertiesPanel;
	};
}

#endif /*TRAPEDITOR_SCENEGRAPHPANEL_H*/