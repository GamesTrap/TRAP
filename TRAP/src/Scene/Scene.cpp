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
#include <box2d/b2_circle_shape.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

[[nodiscard]] static b2BodyType TRAPRigidbody2DTypeToBox2DBody(TRAP::Rigidbody2DComponent::BodyType bodyType)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	switch(bodyType)
	{
	case TRAP::Rigidbody2DComponent::BodyType::Static:
		return b2_staticBody;

	case TRAP::Rigidbody2DComponent::BodyType::Dynamic:
		return b2_dynamicBody;

	case TRAP::Rigidbody2DComponent::BodyType::Kinematic:
		return b2_kinematicBody;

	default:
		TRAP_ASSERT(false, "TRAPRigidbody2DTypeToBox2DBody(): Unknown body type!");
		return b2_staticBody;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Component>
static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<TRAP::Utils::UID, entt::entity>& enttMap)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

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
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	CopyComponent<Component...>(dst, src, enttMap);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename... Component>
static void CopyComponentIfExists(TRAP::Entity dst, TRAP::Entity src)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

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
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	CopyComponentIfExists<Component...>(dst, src);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Scene> TRAP::Scene::Copy(Ref<Scene> other)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	TRAP::Ref<Scene> newScene = TRAP::MakeRef<Scene>();

	newScene->m_viewportWidth = other->m_viewportWidth;
	newScene->m_viewportHeight = other->m_viewportHeight;

	auto& srcSceneRegistry = other->m_registry;
	auto& dstSceneRegistry = newScene->m_registry;
	std::unordered_map<Utils::UID, entt::entity> enttMap{};

	//Create entities with UID and Tag in newScene for each entity with an UID component in other scene.
	//Reversed so the entities have the same order as in the original scene.
	auto UIDView = srcSceneRegistry.view<UIDComponent>();
	for(auto it = UIDView.rbegin(); it != UIDView.rend(); ++it)
	{
		Utils::UID uid = srcSceneRegistry.get<UIDComponent>(*it).UID;
		const auto& name = srcSceneRegistry.get<TagComponent>(*it).Tag;
		Entity newEntity = newScene->CreateEntityWithUID(uid, name);
		enttMap[uid] = static_cast<entt::entity>(newEntity);
	}

	//Copy components (except UIDComponent and TagComponent)
	CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

	return newScene;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Entity TRAP::Scene::CreateEntity(const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	return CreateEntityWithUID(Utils::UID(), name);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Entity TRAP::Scene::CreateEntityWithUID(Utils::UID uid, const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

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
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	m_registry.destroy(entity);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnRuntimeStart()
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	m_physicsWorld = TRAP::MakeScope<b2World>(b2Vec2{0.0f, -9.8f});

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

		if(entity.HasComponent<CircleCollider2DComponent>())
		{
			auto& circleCollider2D = entity.GetComponent<CircleCollider2DComponent>();

			b2CircleShape circleShape{};
			circleShape.m_p.Set(circleCollider2D.Offset.x, circleCollider2D.Offset.y);
			circleShape.m_radius = transform.Scale.x * circleCollider2D.Radius;

			b2FixtureDef fixtureDef{};
			fixtureDef.shape = &circleShape;
			fixtureDef.density = circleCollider2D.Density;
			fixtureDef.friction = circleCollider2D.Friction;
			fixtureDef.restitution = circleCollider2D.Restitution;
			fixtureDef.restitutionThreshold = circleCollider2D.RestitutionThreshold;
			circleCollider2D.RuntimeFixture = body->CreateFixture(&fixtureDef);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnRuntimeStop()
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	auto view = m_registry.view<Rigidbody2DComponent>();
	for(auto e : view)
	{
		Entity entity{e, this};
		auto& rigidbody2D = entity.GetComponent<Rigidbody2DComponent>();

		if(entity.HasComponent<BoxCollider2DComponent>())
		{
			auto& boxCollider2D = entity.GetComponent<BoxCollider2DComponent>();

			if(rigidbody2D.RuntimeBody && boxCollider2D.RuntimeFixture)
			{
				rigidbody2D.RuntimeBody->DestroyFixture(boxCollider2D.RuntimeFixture);
				boxCollider2D.RuntimeFixture = nullptr;
			}
		}

		if(entity.HasComponent<CircleCollider2DComponent>())
		{
			auto& circleCollider2D = entity.GetComponent<CircleCollider2DComponent>();

			if(rigidbody2D.RuntimeBody && circleCollider2D.RuntimeFixture)
			{
				rigidbody2D.RuntimeBody->DestroyFixture(circleCollider2D.RuntimeFixture);
				circleCollider2D.RuntimeFixture = nullptr;
			}
		}

		if(rigidbody2D.RuntimeBody)
		{
			m_physicsWorld->DestroyBody(rigidbody2D.RuntimeBody);
			rigidbody2D.RuntimeBody = nullptr;
		}
	}

	m_physicsWorld.reset();
	m_physicsWorld = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnTickRuntime(const Utils::TimeStep& deltaTime)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

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

		b2Body* body = rigidbody2D.RuntimeBody;
		const auto& position = body->GetPosition();
		transform.Position.x = position.x;
		transform.Position.y = position.y;
		transform.Rotation.z = body->GetAngle();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnUpdateRuntime(const Utils::TimeStep deltaTime)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

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

		//Render sprites
		auto spriteGroup = m_registry.view<TransformComponent, SpriteRendererComponent>();
		for (auto entity : spriteGroup)
		{
			auto [transform, sprite] = spriteGroup.get<TransformComponent, SpriteRendererComponent>(entity);

			Graphics::Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int32_t>(entity));
		}

		//Render circles
		auto circleGroup = m_registry.view<TransformComponent, CircleRendererComponent>();
		for (auto entity : circleGroup)
		{
			auto [transform, circle] = circleGroup.get<TransformComponent, CircleRendererComponent>(entity);

			Graphics::Renderer2D::DrawCircle(transform.GetTransform(), circle.Color,
											circle.Thickness, circle.Fade, static_cast<int32_t>(entity));
		}

		Graphics::Renderer2D::EndScene();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnUpdateEditor(const Utils::TimeStep /*deltaTime*/, Graphics::EditorCamera& camera)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	Graphics::Renderer2D::BeginScene(camera);

	//Renderer sprites
	auto spriteGroup = m_registry.view<TransformComponent, SpriteRendererComponent>();
	for (auto entity : spriteGroup)
	{
		auto [transform, sprite] = spriteGroup.get<TransformComponent, SpriteRendererComponent>(entity);

		Graphics::Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int32_t>(entity));
	}

	//Render circles
	auto circleGroup = m_registry.view<TransformComponent, CircleRendererComponent>();
	for (auto entity : circleGroup)
	{
		auto [transform, circle] = circleGroup.get<TransformComponent, CircleRendererComponent>(entity);

		Graphics::Renderer2D::DrawCircle(transform.GetTransform(), circle.Color,
		                                 circle.Thickness, circle.Fade, static_cast<int32_t>(entity));
	}

	Graphics::Renderer2D::EndScene();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnTick(const TRAP::Utils::TimeStep& deltaTime)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

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
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

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
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	Entity newEntity = CreateEntity(entity.GetName());
	CopyComponentIfExists(AllComponents{}, newEntity, entity);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Entity TRAP::Scene::GetPrimaryCameraEntity()
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Scene);

	auto view = m_registry.view<CameraComponent>();
	for(auto entity : view)
	{
		auto camera = view.get<CameraComponent>(entity);
		if(camera.Primary)
			return Entity{ entity, this };
	}

	return {};
}
