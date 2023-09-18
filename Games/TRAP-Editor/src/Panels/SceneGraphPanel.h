#ifndef TRAPEDITOR_SCENEGRAPHPANEL_H
#define TRAPEDITOR_SCENEGRAPHPANEL_H

#include <Scene/Components.h>
#include <Scene/Scene.h>
#include <Core/Base.h>
#include <Scene/Entity.h>

namespace TRAP
{
	class SceneGraphPanel
	{
	public:
		constexpr SceneGraphPanel() = default;
		explicit SceneGraphPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

		constexpr void SetSelectedEntity(const Entity& entity);
		[[nodiscard]] constexpr Entity GetSelectedEntity() const;

	private:
		template<typename T>
		requires TRAP::IsComponent<T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

		Ref<Scene> m_context = nullptr;
		Entity m_selectionContext{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::SceneGraphPanel::SetSelectedEntity(const Entity& entity)
{
	m_selectionContext = entity;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Entity TRAP::SceneGraphPanel::GetSelectedEntity() const
{
	return m_selectionContext;
}

#endif /*TRAPEDITOR_SCENEGRAPHPANEL_H*/