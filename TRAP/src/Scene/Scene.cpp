#include "TRAPPCH.h"
#include "Scene.h"

#include "Components.h"
#include "Graphics/Renderer2D.h"
#include "Utils/Time/TimeStep.h"
#include "Entity.h"
#include "ScriptableEntity.h"
#include "Graphics/Cameras/Editor/EditorCamera.h"
#include "Utils/Hash/UID.h"

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

static b2BodyType TRAPRigidbody2DTypeToBox2DBody(TRAP::Rigidbody2DComponent::BodyType bodyType)
{
	switch(bodyType)
	{
	case TRAP::Rigidbody2DComponent::BodyType::Static:
		return b2_staticBody;

	case TRAP::Rigidbody2DComponent::BodyType::Dynamic:
		return b2_dynamicBody;

	case TRAP::Rigidbody2DComponent::BodyType::Kinematic:
		return b2_kinematicBody;

	default:
		TRAP_ASSERT(false, "Unknown body type!");
		return b2_staticBody;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Component>
static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<TRAP::Utils::UID, entt::entity>& enttMap)
{
	([&]()
	{
		const auto view = src.view<Component>();
		for(const auto srcEntity : view)
		{
			const entt::entity dstEntity = enttMap.at(src.get<TRAP::UIDComponent>(srcEntity).UID);

			auto& srcComponent = src.get<Component>(srcEntity);
			dst.emplace_or_replace<Component>(dstEntity, srcComponent);
		}
	}(), ...);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Component>
static void CopyComponent(TRAP::ComponentGroup<Component...>, entt::registry& dst, entt::registry& src,
                          const std::unordered_map<TRAP::Utils::UID, entt::entity>& enttMap)
{
	CopyComponent<Component...>(dst, src, enttMap);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Component>
static void CopyComponentIfExists(TRAP::Entity dst, TRAP::Entity src)
{
	([&]()
	{
		if(src.HasComponent<Component>())
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
	}(), ...);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Component>
static void CopyComponentIfExists(TRAP::ComponentGroup<Component...>, TRAP::Entity dst, TRAP::Entity src)
{
	CopyComponentIfExists<Component...>(dst, src);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Scene> TRAP::Scene::Copy(Ref<Scene> other)
{
	TRAP::Ref<Scene> newScene = TRAP::MakeRef<Scene>();

	newScene->m_viewportWidth = other->m_viewportWidth;
	newScene->m_viewportHeight = other->m_viewportHeight;

	auto& srcSceneRegistry = other->m_registry;
	auto& dstSceneRegistry = newScene->m_registry;
	std::unordered_map<Utils::UID, entt::entity> enttMap{};

	//Create entities with UID and Tag in newScene for each entity with an UID component in other scene.
	auto UIDView = srcSceneRegistry.view<UIDComponent>();
	for(auto e : UIDView)
	{
		Utils::UID uid = srcSceneRegistry.get<UIDComponent>(e).UID;
		const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
		Entity newEntity = newScene->CreateEntityWithUID(uid, name);
		enttMap[uid] = static_cast<entt::entity>(newEntity);
	}

	//Copy components (except UIDComponent and TagComponent)
	CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

	return newScene;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity TRAP::Scene::CreateEntity(const std::string& name)
{
	return CreateEntityWithUID(Utils::UID(), name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity TRAP::Scene::CreateEntityWithUID(Utils::UID uid, const std::string& name)
{
	Entity entity = { m_registry.create(), this };
	entity.AddComponent<UIDComponent>(uid);
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

void TRAP::Scene::OnRuntimeStart()
{
	m_physicsWorld = new b2World({0.0f, -9.8f});

	auto view = m_registry.view<Rigidbody2DComponent>();
	for(auto e : view)
	{
		Entity entity{e, this};
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rigidbody2D = entity.GetComponent<Rigidbody2DComponent>();

		b2BodyDef bodyDef{};
		bodyDef.type = TRAPRigidbody2DTypeToBox2DBody(rigidbody2D.Type);
		bodyDef.position.Set(transform.Position.x, transform.Position.y);
		bodyDef.angle = transform.Rotation.z;

		b2Body* body = m_physicsWorld->CreateBody(&bodyDef);
		body->SetFixedRotation(rigidbody2D.FixedRotation);
		rigidbody2D.RuntimeBody = body;

		if(entity.HasComponent<BoxCollider2DComponent>())
		{
			auto& boxCollider2D = entity.GetComponent<BoxCollider2DComponent>();

			b2PolygonShape boxShape{};
			boxShape.SetAsBox(boxCollider2D.Size.x * transform.Scale.x, boxCollider2D.Size.y * transform.Scale.y);

			b2FixtureDef fixtureDef{};
			fixtureDef.shape = &boxShape;
			fixtureDef.density = boxCollider2D.Density;
			fixtureDef.friction = boxCollider2D.Friction;
			fixtureDef.restitution = boxCollider2D.Restitution;
			fixtureDef.restitutionThreshold = boxCollider2D.RestitutionThreshold;
			boxCollider2D.RuntimeFixture = body->CreateFixture(&fixtureDef);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnRuntimeStop()
{
	delete m_physicsWorld;
	m_physicsWorld = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnTickRuntime(const Utils::TimeStep& deltaTime)
{
	//Physics
	const int32_t velocityIterations = 6;
	const int32_t positionIterations = 2;
	m_physicsWorld->Step(deltaTime, velocityIterations, positionIterations);

	//Retrieve transform from Box2D
	auto view = m_registry.view<Rigidbody2DComponent>();
	for(auto e : view)
	{
		Entity entity{e, this};
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rigidbody2D = entity.GetComponent<Rigidbody2DComponent>();

		b2Body* body = static_cast<b2Body*>(rigidbody2D.RuntimeBody);
		const auto& position = body->GetPosition();
		transform.Position.x = position.x;
		transform.Position.y = position.y;
		transform.Rotation.z = body->GetAngle();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnUpdateRuntime(const Utils::TimeStep deltaTime)
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
	const Graphics::Camera* mainCamera = nullptr;
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

			// Graphics::Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color, nullptr, nullptr);
			Graphics::Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int32_t>(entity));
		}

		Graphics::Renderer2D::EndScene();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnUpdateEditor(const Utils::TimeStep /*deltaTime*/, Graphics::EditorCamera& camera)
{
	Graphics::Renderer2D::BeginScene(camera);

	auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
	for (auto entity : group)
	{
		auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

		// Graphics::Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color, nullptr, nullptr);
		Graphics::Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int32_t>(entity));
	}

	Graphics::Renderer2D::EndScene();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnTick(const TRAP::Utils::TimeStep& deltaTime)
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

			nsc.Instance->OnTick(deltaTime);
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

void TRAP::Scene::DuplicateEntity(Entity entity)
{
	Entity newEntity = CreateEntity(entity.GetName());
	CopyComponentIfExists(AllComponents{}, newEntity, entity);
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
	// static_assert(sizeof(T) == 0, "ComponentAdded is not implemented for this type!");
}

template<>
void TRAP::Scene::OnComponentAdded<TRAP::UIDComponent>(Entity, UIDComponent&)
{}

template<>
void TRAP::Scene::OnComponentAdded<TRAP::TransformComponent>(Entity, TransformComponent&)
{}

template<>
void TRAP::Scene::OnComponentAdded<TRAP::CameraComponent>(Entity, CameraComponent& component)
{
	if(m_viewportWidth > 0 && m_viewportHeight > 0)
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

template<>
void TRAP::Scene::OnComponentAdded<TRAP::Rigidbody2DComponent>(Entity, Rigidbody2DComponent&)
{}

template<>
void TRAP::Scene::OnComponentAdded<TRAP::BoxCollider2DComponent>(Entity, BoxCollider2DComponent&)
{}