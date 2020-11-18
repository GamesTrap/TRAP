#ifndef _TRAPEDITORPROPERTIESPANEL_H_
#define _TRAPEDITORPROPERTIESPANEL_H_

#include <TRAP_Assert.h>
#include <Scene/Entity.h>

namespace TRAP
{
	class PropertiesPanel
	{
	public:
		PropertiesPanel() = default;
		explicit PropertiesPanel(const Entity& entity, const Ref<Scene>& context);

		void SetEntity(const Entity& entity);
		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();
	private:
		void DrawComponents(Entity entity);
		template<typename T, typename UIFunction>
		void DrawComponent(const std::string& name, Entity& entity, UIFunction func);

		Entity m_entity;
		Ref<Scene> m_context;
	};
}

#endif /*_TRAPEDITORPROPERTIESPANEL_H_*/