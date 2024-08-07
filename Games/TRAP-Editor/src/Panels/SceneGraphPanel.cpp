#include "SceneGraphPanel.h"

#include <TRAP.h>

#include <imgui_internal.h>

namespace
{
	void DrawVec3Control(const std::string& label, //TODO can be replaced with std::string_view?!
						 TRAP::Math::Vec3& values,
						 const f32 resetValues = 0.0f,
						 const f32 columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto* const boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		const f32 lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		const ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x() = resetValues;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x(), 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y() = resetValues;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y(), 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.18f, 0.25f, 0.8f, 1.0f));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z() = resetValues;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z(), 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::SceneGraphPanel::SceneGraphPanel(const Ref<Scene>& context)
{
	SetContext(context);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneGraphPanel::SetContext(const Ref<Scene>& context)
{
	m_context = context;
	m_selectionContext = {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneGraphPanel::OnImGuiRender()
{
	ImGui::Begin("Scene Graph");

	if(m_context)
	{
		for(const auto entityID : m_context->m_registry.storage<entt::entity>())
		{
			const Entity entity{ entityID, m_context.get() };
			DrawEntityNode(entity);
		};

		if(ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
			m_selectionContext = {};

		//Right-click on blank space
		if(ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Entity"))
				m_context->CreateEntity("Entity");

			ImGui::EndPopup();
		}
	}

	ImGui::End();

	ImGui::Begin("Properties");
	if(m_selectionContext)
		DrawComponents(m_selectionContext);
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneGraphPanel::DrawEntityNode(Entity entity)
{
	auto& tag = entity.GetComponent<TagComponent>().Tag;

	const ImGuiTreeNodeFlags flags = ((m_selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	const bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<u64>(entity.GetUID())), flags, "%s", tag.c_str());
	if(ImGui::IsItemClicked())
		m_selectionContext = entity;

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
			m_selectionContext = {};
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T, typename UIFunction>
requires TRAP::IsComponent<T> && std::is_invocable_r_v<void, UIFunction, T&>
void DrawComponent(const std::string& name, TRAP::Entity& entity, UIFunction func) //TODO name can be replaced by std::string_view
{
	static constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap |
		ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;

	if (entity.HasComponent<T>())
	{
		auto& component = entity.GetComponent<T>();
		const ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4.0f, 4.0f });
		const f32 lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::Separator();
		const bool open = ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(T).hash_code()), treeNodeFlags, "%s", name.c_str());
		ImGui::PopStyleVar();
		bool removeComponent = false;
		if(!std::same_as<T, TRAP::TransformComponent>)
		{
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button(":", ImVec2{ lineHeight, lineHeight }))
				ImGui::OpenPopup("ComponentSettings");

			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

		}

		if (open)
		{
			func(component);
			ImGui::TreePop();
		}
		if(!std::same_as<T, TRAP::TransformComponent>)
		{
			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::SceneGraphPanel::DrawComponents(Entity entity)
{
	if (entity.HasComponent<TagComponent>() && entity.HasComponent<UIDComponent>())
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		const Utils::UID& uid = entity.GetUID();

		std::string buffer = tag;
		if(ImGui::InputText("##Tag", &buffer))
			tag = buffer;
		if(ImGui::IsItemHovered(/*ImGuiHoveredFlags_DelayShort*/)) //TODO
		{
			ImGui::BeginTooltip();
			ImGui::Text("UID: %lu", static_cast<u64>(uid));
			ImGui::EndTooltip();
		}
	}

	ImGui::SameLine();
	ImGui::PushItemWidth(-1);

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");

	if (ImGui::BeginPopup("AddComponent"))
	{
		DisplayAddComponentEntry<CameraComponent>("Camera");
		DisplayAddComponentEntry<SpriteRendererComponent>("Sprite Renderer");
		DisplayAddComponentEntry<CircleRendererComponent>("Circle Renderer");
		DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
		DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
		DisplayAddComponentEntry<CircleCollider2DComponent>("Circle Collider 2D");

		ImGui::EndPopup();
	}

	ImGui::PopItemWidth();

	DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
	{
			DrawVec3Control("Position", component.Position);
			Math::Vec3 rotation = Math::Degrees(component.Rotation);
			DrawVec3Control("Rotation", rotation);
			component.Rotation = Math::Radians(rotation);
			DrawVec3Control("Scale", component.Scale, 1.0f);
	});

	DrawComponent<CameraComponent>("Camera", entity, [&](auto& component)
	{
		auto& camera = component.Camera;

		ImGui::Checkbox("Primary", &component.Primary);

		static constexpr std::array<std::string_view, 2> projectionTypeStrings = { "Perspective", "Orthographic" };
		const char* currentProjectionTypeString = projectionTypeStrings[std::to_underlying(camera.GetProjectionType())].data();
		if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
		{
			for (u32 i = 0; i < projectionTypeStrings.size(); i++)
			{
				const bool isSelected = std::string_view(currentProjectionTypeString) == projectionTypeStrings[i];
				if (ImGui::Selectable(projectionTypeStrings[i].data(), isSelected))
				{
					currentProjectionTypeString = projectionTypeStrings[i].data();
					camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(i));
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
		{
			f32 perspectiveVerticalFOV = Math::Degrees(camera.GetPerspectiveVerticalFOV());
			if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFOV, 0.25f, 0.0f, 120.0f))
				camera.SetPerspectiveVerticalFOV(Math::Radians(perspectiveVerticalFOV));

			f32 perspectiveNear = camera.GetPerspectiveNearClip();
			if (ImGui::DragFloat("Near", &perspectiveNear))
				camera.SetPerspectiveNearClip(perspectiveNear);
		}

		if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
		{
			f32 orthoSize = camera.GetOrthographicSize();
			if (ImGui::DragFloat("Size", &orthoSize))
				camera.SetOrthographicSize(orthoSize);

			f32 orthoNear = camera.GetOrthographicNearClip();
			if (ImGui::DragFloat("Near", &orthoNear))
				camera.SetOrthographicNearClip(orthoNear);

			f32 orthoFar = camera.GetOrthographicFarClip();
			if (ImGui::DragFloat("Far", &orthoFar))
				camera.SetOrthographicFarClip(orthoFar);

			ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
		}
	});

	DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
	{
		ImGui::ColorEdit4("Color", &std::get<0>(component.Color));
	});

	DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
	{
		ImGui::ColorEdit4("Color", &std::get<0>(component.Color));
		ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
		ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
	});

	DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
	{
		static constexpr std::array<std::string_view, 3> bodyTypeStrings{"Static", "Dynamic", "Kinematic"};
		const char* currentBodyTypeString = bodyTypeStrings[std::to_underlying(component.Type)].data();
		if(ImGui::BeginCombo("Body Type", currentBodyTypeString))
		{
			for(u32 i = 0; i < bodyTypeStrings.size(); ++i)
			{
				const bool isSelected = std::string_view(currentBodyTypeString) == bodyTypeStrings[i];
				if(ImGui::Selectable(bodyTypeStrings[i].data(), isSelected))
				{
					currentBodyTypeString = bodyTypeStrings[i].data();
					component.Type = static_cast<Rigidbody2DComponent::BodyType>(i);
				}

				if(isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
	});

	DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
	{
		ImGui::DragFloat2("Offset", &std::get<0>(component.Offset), 0.1f);
		ImGui::DragFloat2("Size", &std::get<0>(component.Size), 0.1f);
		ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("RestitutionThreshold", &component.RestitutionThreshold, 0.01f, 0.0f);
	});

	DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component)
	{
		ImGui::DragFloat2("Offset", &std::get<0>(component.Offset), 0.1f);
		ImGui::DragFloat("Radius", &component.Radius, 0.1f);
		ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("RestitutionThreshold", &component.RestitutionThreshold, 0.01f, 0.0f);
	});
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires TRAP::IsComponent<T>
void TRAP::SceneGraphPanel::DisplayAddComponentEntry(const std::string& entryName)
{
	if(!m_selectionContext.HasComponent<T>())
	{
		if(ImGui::MenuItem(entryName.c_str()))
		{
			m_selectionContext.AddComponent<T>();
			ImGui::CloseCurrentPopup();
		}
	}
}
