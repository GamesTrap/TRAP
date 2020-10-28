#include "SceneGraphPanel.h"

#include <Scene/Components.h>

#include <imgui.h>

TRAP::SceneGraphPanel::SceneGraphPanel(const Ref<Scene>& context)
{
	SetContext(context);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneGraphPanel::SetContext(const Ref<Scene>& context)
{
	m_context = context;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneGraphPanel::OnImGuiRender()
{
	ImGui::Begin("Scene Graph");

	m_context->m_registry.each([&](auto entityID)
	{
		const Entity entity{ entityID, m_context.get() };
		DrawEntityNode(entity);
	});

	if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
	{
		m_selectionContext = {};
		m_propertiesPanel.SetEntity({});
	}
	
	ImGui::End();

	m_propertiesPanel.OnImGuiRender();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneGraphPanel::DrawEntityNode(Entity entity)
{
	auto& tag = entity.GetComponent<TagComponent>().Tag;

	const ImGuiTreeNodeFlags flags = ((m_selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	const bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uint64_t>(static_cast<uint32_t>(entity))), flags, "%s", tag.c_str());
	if(ImGui::IsItemClicked())
	{
		m_selectionContext = entity;
		m_propertiesPanel.SetEntity(m_selectionContext);
	}

	if(opened)
		ImGui::TreePop();
}