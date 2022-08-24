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
	m_selectionContext = {};

	m_propertiesPanel.SetContext(m_context);
	m_propertiesPanel.SetEntity(m_selectionContext);
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

	if(ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
	{
		m_selectionContext = {};
		m_propertiesPanel.SetEntity({});
	}

	//Right-click on blank space
	if(ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
	{
		if (ImGui::MenuItem("Create Entity"))
			m_context->CreateEntity("Entity");

		ImGui::EndPopup();
	}

	ImGui::End();

	m_propertiesPanel.OnImGuiRender();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneGraphPanel::SetSelectedEntity(const Entity& entity)
{
	m_selectionContext = entity;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity TRAP::SceneGraphPanel::GetSelectedEntity() const
{
	return m_selectionContext;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneGraphPanel::DrawEntityNode(Entity entity)
{
	auto& tag = entity.GetComponent<TagComponent>().Tag;

	const ImGuiTreeNodeFlags flags = ((m_selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	const bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uint64_t>(static_cast<uint32_t>(entity))), flags, "%s", tag.c_str());
	if(ImGui::IsItemClicked())
	{
		m_selectionContext = entity;
		m_propertiesPanel.SetEntity(m_selectionContext);
	}

	bool entityDeleted = false;
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Delete Entity"))
			entityDeleted = true;

		ImGui::EndPopup();
	}

	if(opened)
		ImGui::TreePop();

	if(entityDeleted)
	{
		m_context->DestroyEntity(entity);
		if (m_selectionContext == entity)
		{
			m_selectionContext = {};
			m_propertiesPanel.SetEntity({});
		}
	}
}