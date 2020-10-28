#ifndef _TRAPEDITORPROPERTIESPANEL_H_
#define _TRAPEDITORPROPERTIESPANEL_H_

#include <Scene/Entity.h>
#include <TRAP_Assert.h>

namespace TRAP
{
	class PropertiesPanel
	{
	public:
		PropertiesPanel() = default;
		explicit PropertiesPanel(const Entity& entity);

		void SetEntity(const Entity& entity);

		void OnImGuiRender() const;
	private:
		void DrawComponents(Entity entity) const;
		template<typename T, typename F>
		void DrawComponent(const std::string& name, F func) const;

		Entity m_entity;
	};
}

#endif /*_TRAPEDITORPROPERTIESPANEL_H_*/