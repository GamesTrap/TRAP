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
	if (ImGui::Button("X", buttonSize))
		values.x = resetValues;
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.3f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
	if (ImGui::Button("Y", buttonSize))
		values.y = resetValues;
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.3f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.18f, 0.25f, 0.8f, 1.0f));
	if (ImGui::Button("Z", buttonSize))
		values.z = resetValues;
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.3f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();
	
	ImGui::Columns(1);

	ImGui::PopID();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::PropertiesPanel::PropertiesPanel(const Entity& entity)
{
	SetEntity(entity);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::PropertiesPanel::SetEntity(const Entity& entity)
{
	m_entity = entity;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::PropertiesPanel::OnImGuiRender() const
{
	ImGui::Begin("Properties");
	if(m_entity)
		DrawComponents(m_entity);
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::PropertiesPanel::DrawComponents(Entity entity) const
{
	if (entity.HasComponent<TagComponent>())
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		std::array<char, 256> buffer{};
#ifdef TRAP_PLATFORM_WINDOWS
		strcpy_s(buffer.data(), buffer.size() * sizeof(char), tag.c_str());
#else
		strcpy(buffer.data(), tag.c_str());
#endif
		if(ImGui::InputText("Tag", buffer.data(), buffer.size() * sizeof(char)))
		{
			tag = std::string(buffer.data());
		}
	}

	if (entity.HasComponent<TransformComponent>())
	{
		DrawComponent<TransformComponent>("Transform", [&]()
		{
			auto& tc = entity.GetComponent<TransformComponent>();
			DrawVec3Control("Position", tc.Position);
			Math::Vec3 rotation = Math::Degrees(tc.Rotation);
			DrawVec3Control("Rotation", rotation);
			tc.Rotation = Math::Radians(rotation);
			DrawVec3Control("Scale", tc.Scale, 1.0f);
		});
	}

	if (entity.HasComponent<CameraComponent>())
	{
		DrawComponent<CameraComponent>("Camera", [&]()
		{
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			ImGui::Checkbox("Primary", &cameraComponent.Primary); //BUG Doesn't uncheck other cameras

			std::array<const char*, 2> projectionTypeStrings = { "Perspective", "Orthographic" };
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

				ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);
			}
		});
	}

	if (entity.HasComponent<SpriteRendererComponent>())
	{
		DrawComponent<SpriteRendererComponent>("Sprite Renderer", [&]()
		{
			auto& src = entity.GetComponent<SpriteRendererComponent>();
			ImGui::ColorEdit4("Color", &src.Color[0]);
		});
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T, typename F>
void TRAP::PropertiesPanel::DrawComponent(const std::string& name, F func) const
{
	if (ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(T).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "%s", name.c_str()))
	{
		func();
		ImGui::TreePop();
	}
}