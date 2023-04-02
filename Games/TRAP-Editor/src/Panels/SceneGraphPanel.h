#ifndef TRAPEDITOR_SCENEGRAPHPANEL_H
#define TRAPEDITOR_SCENEGRAPHPANEL_H

#include <Scene/Scene.h>
#include <Core/Base.h>
#include <Scene/Entity.h>

namespace TRAP
{
	class SceneGraphPanel
	{
	public:
		SceneGraphPanel() = default;
		explicit SceneGraphPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

		void SetSelectedEntity(const Entity& entity);
		[[nodiscard]] Entity GetSelectedEntity() const;

	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

		Ref<Scene> m_context;
		Entity m_selectionContext;
	};
}

#endif /*TRAPEDITOR_SCENEGRAPHPANEL_H*/