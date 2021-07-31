#include "PropertiesPanel.h"

#include <Scene/Components.h>
#include <Core/PlatformDetection.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <Maths/Math.h>

static void DrawVec3Control(const std::string& label,
                            TRAP::Math::Vec3& values,
                            const float resetValues = 0.0f,
                            const float columnWidth = 100.0f)
{
	ImGuiIO& io = ImGui::GetIO();
	const auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text("%s", label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	const ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize))
		values.x = resetValues;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize))
		values.y = resetValues;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.18f, 0.25f, 0.8f, 1.0f));
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize))
		values.z = resetValues;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::PropertiesPanel::PropertiesPanel(const Entity& entity, const Ref<Scene>& context)
{
	SetEntity(entity);

	m_context = context;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::PropertiesPanel::SetEntity(const Entity& entity)
{
	m_entity = entity;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::PropertiesPanel::SetContext(const Ref<Scene>& context)
{
	m_context = context;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::PropertiesPanel::OnImGuiRender()
{
	ImGui::Begin("Properties");
	if(m_entity)
		DrawComponents(m_entity);
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::PropertiesPanel::DrawComponents(Entity entity)
{
	if (entity.HasComponent<TagComponent>())
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		//TODO Replace array with real string?!
		std::array<char, 256> buffer{};
#ifdef TRAP_PLATFORM_WINDOWS
		strcpy_s(buffer.data(), buffer.size() * sizeof(char), tag.c_str());
#else
		strcpy(buffer.data(), tag.c_str());
#endif
		if(ImGui::InputText("##Tag", buffer.data(), buffer.size() * sizeof(char)))
		{
			tag = std::string(buffer.data());
		}
	}

	ImGui::SameLine();
	ImGui::PushItemWidth(-1);

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");

	if (ImGui::BeginPopup("AddComponent"))
	{
		if (!m_entity.HasComponent<CameraComponent>())
		{
			if (ImGui::MenuItem("Camera"))
			{
				m_entity.AddComponent<CameraComponent>();
				ImGui::CloseCurrentPopup();
			}
		}

		if (!m_entity.HasComponent<SpriteRendererComponent>())
		{
			if (ImGui::MenuItem("Sprite Renderer"))
			{
				m_entity.AddComponent<SpriteRendererComponent>();
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::EndPopup();
	}

	ImGui::PopItemWidth();

	if (entity.HasComponent<TransformComponent>())
	{
		if (ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(TransformComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding, "Transform"))
		{
			auto& tc = entity.GetComponent<TransformComponent>();
			DrawVec3Control("Position", tc.Position);
			Math::Vec3 rotation = Math::Degrees(tc.Rotation);
			DrawVec3Control("Rotation", rotation);
			tc.Rotation = Math::Radians(rotation);
			DrawVec3Control("Scale", tc.Scale, 1.0f);
			ImGui::TreePop();
		}
	}

	DrawComponent<CameraComponent>("Camera", entity, [&](auto& component)
	{
		auto& camera = component.Camera;

		if (ImGui::Checkbox("Primary", &component.Primary))
		{
			auto view = m_context->Reg().view<CameraComponent>();
			for(auto ent : view)
			{
				if(entity != ent)
					view.get<CameraComponent>(ent).Primary = false;
			}
		}

		constexpr std::array<const char*, 2> projectionTypeStrings = { "Perspective", "Orthographic" };
		const char* currentProjectionTypeString = projectionTypeStrings[static_cast<uint32_t>(camera.GetProjectionType())];
		if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
		{
			for (uint32_t i = 0; i < projectionTypeStrings.size(); i++)
			{
				const bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
				if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
				{
					currentProjectionTypeString = projectionTypeStrings[i];
					camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(i));
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
		{
			float perspectiveVerticalFOV = Math::Degrees(camera.GetPerspectiveVerticalFOV());
			if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFOV, 0.25f, 0.0f, 120.0f))
				camera.SetPerspectiveVerticalFOV(Math::Radians(perspectiveVerticalFOV));

			float perspectiveNear = camera.GetPerspectiveNearClip();
			if (ImGui::DragFloat("Near", &perspectiveNear))
				camera.SetPerspectiveNearClip(perspectiveNear);

			float perspectiveFar = camera.GetPerspectiveFarClip();
			if (ImGui::DragFloat("Far", &perspectiveFar))
				camera.SetPerspectiveFarClip(perspectiveFar);
		}

		if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
		{
			float orthoSize = camera.GetOrthographicSize();
			if (ImGui::DragFloat("Size", &orthoSize))
				camera.SetOrthographicSize(orthoSize);

			float orthoNear = camera.GetOrthographicNearClip();
			if (ImGui::DragFloat("Near", &orthoNear))
				camera.SetOrthographicNearClip(orthoNear);

			float orthoFar = camera.GetOrthographicFarClip();
			if (ImGui::DragFloat("Far", &orthoFar))
				camera.SetOrthographicFarClip(orthoFar);

			ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
		}
	});

	DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
	{
		ImGui::ColorEdit4("Color", &component.Color[0]);
	});
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T, typename UIFunction>
void TRAP::PropertiesPanel::DrawComponent(const std::string& name, Entity& entity, UIFunction func)
{
	constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap |
		ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;

	if (entity.HasComponent<T>())
	{
		auto& component = entity.GetComponent<T>();
		const ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4.0f, 4.0f });
		const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::Separator();
		const bool open = ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(T).hash_code()), treeNodeFlags, "%s", name.c_str());
		ImGui::PopStyleVar();
		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
		if (ImGui::Button(":", ImVec2{ lineHeight, lineHeight }))
			ImGui::OpenPopup("ComponentSettings");

		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Remove Component"))
				removeComponent = true;

			ImGui::EndPopup();
		}

		if (open)
		{
			func(component);
			ImGui::TreePop();
		}

		if (removeComponent)
			entity.RemoveComponent<T>();
	}
}