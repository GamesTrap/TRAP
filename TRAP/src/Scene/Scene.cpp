#include "TRAPPCH.h"
#include "Scene.h"

#include "Components.h"
#include "Graphics/Renderer2D.h"
#include "Utils/Time/TimeStep.h"
#include "Entity.h"

TRAP::Entity TRAP::Scene::CreateEntity(const std::string& name)
{
	Entity entity = { m_registry.create(), this };
	entity.AddComponent<TransformComponent>();
	auto& tag = entity.AddComponent<TagComponent>();
	tag.Tag = name.empty() ? tag.Tag = "Entity" : tag.Tag = name;
	return entity;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::DestroyEntity(const Entity entity)
{
	m_registry.destroy(entity);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnUpdate(const Utils::TimeStep deltaTime)
{
	//Update scripts
	{
		m_registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
		{
			//TODO For now create and instantiate every native script.
			//Bug Memory leak OnDestroy is never called!
			if(!nsc.Instance)
			{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_entity = Entity{ entity, this };
				nsc.Instance->OnCreate();
			}

			nsc.Instance->OnUpdate(deltaTime);
		});
	}

	//Render 2D
	//Find Main Camera
	Graphics::Camera* mainCamera = nullptr;
	Math::Mat4 cameraTransform;
	{
		auto view = m_registry.view<TransformComponent, CameraComponent>();
		for (auto entity : view)
		{
			auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

			if(camera.Primary)
			{
				mainCamera = &camera.Camera;
				cameraTransform = transform.GetTransform();
				break;
			}
		}
	}

	//Go Render something
	if (mainCamera)
	{
		Graphics::Renderer2D::BeginScene(*mainCamera, cameraTransform);

		auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Graphics::Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color, nullptr, nullptr);
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
			//Bug Memory leak OnDestroy is never called!
			if (!nsc.Instance)
			{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_entity = Entity{ entity, this };
				nsc.Instance->OnCreate();
			}

			nsc.Instance->OnTick();
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

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity TRAP::Scene::GetPrimaryCameraEntity()
{
	auto view = m_registry.view<CameraComponent>();
	for(auto entity : view)
	{
		auto camera = view.get<CameraComponent>(entity);
		if(camera.Primary)
			return Entity{ entity, this };
	}

	return {};
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
void TRAP::Scene::OnComponentAdded(Entity, T&)
{
	static_assert(sizeof(T) == 0, "ComponentAdded is not implemented for this type!");
}

template<>
void TRAP::Scene::OnComponentAdded<TRAP::TransformComponent>(Entity, TransformComponent&)
{}

template<>
void TRAP::Scene::OnComponentAdded<TRAP::CameraComponent>(Entity, CameraComponent& component)
{
	component.Camera.SetViewportSize(m_viewportWidth, m_viewportHeight);
}

template<>
void TRAP::Scene::OnComponentAdded<TRAP::SpriteRendererComponent>(Entity, SpriteRendererComponent&)
{}

template<>
void TRAP::Scene::OnComponentAdded<TRAP::TagComponent>(Entity, TagComponent&)
{}

template<>
void TRAP::Scene::OnComponentAdded<TRAP::NativeScriptComponent>(Entity, NativeScriptComponent&)
{}