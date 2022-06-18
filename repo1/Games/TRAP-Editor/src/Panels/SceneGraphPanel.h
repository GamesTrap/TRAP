#ifndef _TRAPEDITOR_SCENEGRAPHPANEL_H_
#define _TRAPEDITOR_SCENEGRAPHPANEL_H_

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

#endif /*_TRAPEDITOR_SCENEGRAPHPANEL_H_*/