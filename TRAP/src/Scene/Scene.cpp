#include "TRAPPCH.h"
#include "Scene.h"

#include "Components.h"
#include "Graphics/Renderer2D.h"
#include "Utils/Time/TimeStep.h"
#include "Entity.h"

TRAP::Scene::Scene() = default;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scene::~Scene() = default;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity TRAP::Scene::CreateEntity(const std::string& name)
{
	Entity entity = { m_registry.create(), this };
	entity.AddComponent<TransformComponent>();
	auto& tag = entity.AddComponent<TagComponent>();
	tag.Tag = name.empty() ? tag.Tag = "Entity" : tag.Tag = name;
	return entity;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnUpdate(const Utils::TimeStep deltaTime)
{
	//Update scripts
	{
		m_registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
		{
			//TODO For now create and instantiate every native script.
			if(!nsc.Instance)
			{
				nsc.InstantiateFunction();
				nsc.Instance->m_entity = Entity{ entity, this };

				if(nsc.OnCreateFunction)
					nsc.OnCreateFunction(nsc.Instance);
			}

			if(nsc.OnUpdateFunction)
				nsc.OnUpdateFunction(nsc.Instance, deltaTime);
		});
	}
	
	//Render 2D
	//Find Main Camera
	Graphics::Camera* mainCamera = nullptr;
	Math::Mat4* cameraTransform = nullptr;
	{
		auto view = m_registry.view<TransformComponent, CameraComponent>();
		for (auto entity : view)
		{
			auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

			if(camera.Primary)
			{
				mainCamera = &camera.Camera;
				cameraTransform = &transform.Transform;
				break;
			}
		}
	}

	//Go Render something
	if (mainCamera)
	{
		Graphics::Renderer2D::BeginScene(mainCamera->GetProjectionMatrix(), *cameraTransform);

		auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Graphics::Renderer2D::DrawQuad(transform, sprite.Color, nullptr);
		}

		Graphics::Renderer2D::EndScene();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnTick()
{
	//Update scripts
	{
		m_registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
		{
			//TODO For now create and instantiate every native script.
			if (!nsc.Instance)
			{
				nsc.InstantiateFunction();
				nsc.Instance->m_entity = Entity{ entity, this };

				if(nsc.OnCreateFunction)
					nsc.OnCreateFunction(nsc.Instance);
			}

			if(nsc.OnTickFunction)
				nsc.OnTickFunction(nsc.Instance);
		});
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnViewportResize(const uint32_t width, const uint32_t height)
{
	m_viewportWidth = width;
	m_viewportHeight = height;

	//Resize non-FixedAspectRatio Cameras
	auto view = m_registry.view<CameraComponent>();
	for(auto entity : view)
	{
		auto& cameraComponent = view.get<CameraComponent>(entity);
		if(!cameraComponent.FixedAspectRatio)
			cameraComponent.Camera.SetViewportSize(width, height);
	}
}