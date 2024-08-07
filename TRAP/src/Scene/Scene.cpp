#include "TRAPPCH.h"
#include "Scene.h"

#include "Components.h"
#include "Graphics/Renderer2D.h"
#include "Utils/Time/TimeStep.h"
#include "Entity.h"
#include "Graphics/Cameras/Editor/EditorCamera.h"
#include "Utils/Hash/UID.h"

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif /*_MSC_VER*/
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

#ifdef _MSC_VER
	#pragma warning(pop)
#endif /*_MSC_VER*/

namespace
{
	[[nodiscard]] constexpr b2BodyType TRAPRigidbody2DTypeToBox2DBody(const TRAP::Rigidbody2DComponent::BodyType bodyType)
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
			TRAP_ASSERT(false, "TRAPRigidbody2DTypeToBox2DBody(): Unknown body type!");
			return b2_staticBody;
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	template<typename... Component>
	void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<entt::entity, entt::entity>& enttMap)
	{
		ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

		([&]()
		{
			const auto view = src.view<Component>();
			for(const auto srcEntity : view)
			{
				const entt::entity dstEntity = enttMap.at(srcEntity);

				auto& srcComponent = src.get<Component>(srcEntity);
				dst.emplace_or_replace<Component>(dstEntity, srcComponent);
			}
		}(), ...);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	template<typename... Component>
	void CopyComponent([[maybe_unused]] TRAP::ComponentGroup<Component...> components, entt::registry& dst,
					   entt::registry& src, const std::unordered_map<entt::entity, entt::entity>& enttMap)
	{
		ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None &&
		                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		CopyComponent<Component...>(dst, src, enttMap);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	template<typename... Component>
	void CopyComponentIfExists(TRAP::Entity dst, TRAP::Entity src)
	{
		ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

		([&]()
		{
			if(src.HasComponent<Component>())
				dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}(), ...);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	template<typename... Component>
	void CopyComponentIfExists([[maybe_unused]] TRAP::ComponentGroup<Component...> components, TRAP::Entity dst,
							   TRAP::Entity src)
	{
		ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None &&
		                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		CopyComponentIfExists<Component...>(dst, src);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Scene> TRAP::Scene::Copy(const Ref<Scene>& other)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	TRAP::Ref<Scene> newScene = TRAP::MakeRef<Scene>();

	newScene->m_viewportWidth = other->m_viewportWidth;
	newScene->m_viewportHeight = other->m_viewportHeight;

	auto& srcSceneRegistry = other->m_registry;
	auto& dstSceneRegistry = newScene->m_registry;
	std::unordered_map<entt::entity, entt::entity> enttMap{};

	//Create entities with UID and Tag in newScene for each entity with an UID component in other scene.
	//Reversed so the entities have the same order as in the original scene.
	auto UIDView = srcSceneRegistry.view<UIDComponent>();
	for(auto it : std::ranges::reverse_view(UIDView))
	{
		const auto& name = srcSceneRegistry.get<TagComponent>(it).Tag;
		Entity newEntity = newScene->CreateEntity(name);
		enttMap[it] = static_cast<entt::entity>(newEntity);
	}

	//Copy components (except UIDComponent and TagComponent)
	CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

	return newScene;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Entity TRAP::Scene::CreateEntity(const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	Entity entity = { m_registry.create(), this };
	entity.AddComponent<UIDComponent>();
	entity.AddComponent<TransformComponent>();
	auto& tag = entity.AddComponent<TagComponent>();
	tag.Tag = name.empty() ? "Entity" : name;
	return entity;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::DestroyEntity(const Entity entity)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	m_registry.destroy(entity);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnRuntimeStart()
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	m_physicsWorld = TRAP::MakeScope<b2World>(b2Vec2{0.0f, -9.8f});

	auto view = m_registry.view<Rigidbody2DComponent>();
	for(auto e : view)
	{
		Entity entity{e, this};
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rigidbody2D = entity.GetComponent<Rigidbody2DComponent>();

		b2BodyDef bodyDef{};
		bodyDef.type = TRAPRigidbody2DTypeToBox2DBody(rigidbody2D.Type);
		bodyDef.position.Set(transform.Position.x(), transform.Position.y());
		bodyDef.angle = transform.Rotation.z();

		b2Body* body = m_physicsWorld->CreateBody(&bodyDef);
		body->SetFixedRotation(rigidbody2D.FixedRotation);
		rigidbody2D.RuntimeBody = body;

		if(entity.HasComponent<BoxCollider2DComponent>())
		{
			auto& boxCollider2D = entity.GetComponent<BoxCollider2DComponent>();

			b2PolygonShape boxShape{};
			boxShape.SetAsBox(boxCollider2D.Size.x() * transform.Scale.x(), boxCollider2D.Size.y() * transform.Scale.y());

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
			circleShape.m_p.Set(circleCollider2D.Offset.x(), circleCollider2D.Offset.y());
			circleShape.m_radius = transform.Scale.x() * circleCollider2D.Radius;

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
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	auto view = m_registry.view<Rigidbody2DComponent>();
	for(auto e : view)
	{
		Entity entity{e, this};
		auto& rigidbody2D = entity.GetComponent<Rigidbody2DComponent>();

		if(entity.HasComponent<BoxCollider2DComponent>())
		{
			auto& boxCollider2D = entity.GetComponent<BoxCollider2DComponent>();

			if((rigidbody2D.RuntimeBody != nullptr) && (boxCollider2D.RuntimeFixture != nullptr))
			{
				rigidbody2D.RuntimeBody->DestroyFixture(boxCollider2D.RuntimeFixture);
				boxCollider2D.RuntimeFixture = nullptr;
			}
		}

		if(entity.HasComponent<CircleCollider2DComponent>())
		{
			auto& circleCollider2D = entity.GetComponent<CircleCollider2DComponent>();

			if((rigidbody2D.RuntimeBody != nullptr) && (circleCollider2D.RuntimeFixture != nullptr))
			{
				rigidbody2D.RuntimeBody->DestroyFixture(circleCollider2D.RuntimeFixture);
				circleCollider2D.RuntimeFixture = nullptr;
			}
		}

		if(rigidbody2D.RuntimeBody != nullptr)
		{
			m_physicsWorld->DestroyBody(rigidbody2D.RuntimeBody);
			rigidbody2D.RuntimeBody = nullptr;
		}
	}

	m_physicsWorld.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnTickRuntime(const Utils::TimeStep& deltaTime)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	//Physics
	const i32 velocityIterations = 6;
	const i32 positionIterations = 2;
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
		transform.Position.x() = position.x;
		transform.Position.y() = position.y;
		transform.Rotation.z() = body->GetAngle();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnUpdateRuntime(const Utils::TimeStep deltaTime)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	//Update scripts
	{
		m_registry.view<NativeScriptComponent>().each([this, deltaTime](auto entity, auto& nsc)
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
	if (mainCamera != nullptr)
	{
		Graphics::Renderer2D::BeginScene(*mainCamera, cameraTransform);

		//Render sprites
		auto spriteGroup = m_registry.view<TransformComponent, SpriteRendererComponent>();
		for (auto entity : spriteGroup)
		{
			auto [transform, sprite] = spriteGroup.get<TransformComponent, SpriteRendererComponent>(entity);

			Graphics::Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<i32>(entity));
		}

		//Render circles
		auto circleGroup = m_registry.view<TransformComponent, CircleRendererComponent>();
		for (auto entity : circleGroup)
		{
			auto [transform, circle] = circleGroup.get<TransformComponent, CircleRendererComponent>(entity);

			Graphics::Renderer2D::DrawCircle(transform.GetTransform(), circle.Color,
											circle.Thickness, circle.Fade, static_cast<i32>(entity));
		}

		Graphics::Renderer2D::EndScene();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE
void TRAP::Scene::OnUpdateEditor([[maybe_unused]] const Utils::TimeStep deltaTime, Graphics::EditorCamera& camera)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	Graphics::Renderer2D::BeginScene(camera);

	//Renderer sprites
	auto spriteGroup = m_registry.view<TransformComponent, SpriteRendererComponent>();
	for (auto entity : spriteGroup)
	{
		auto [transform, sprite] = spriteGroup.get<TransformComponent, SpriteRendererComponent>(entity);

		Graphics::Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<i32>(entity));
	}

	//Render circles
	auto circleGroup = m_registry.view<TransformComponent, CircleRendererComponent>();
	for (auto entity : circleGroup)
	{
		auto [transform, circle] = circleGroup.get<TransformComponent, CircleRendererComponent>(entity);

		Graphics::Renderer2D::DrawCircle(transform.GetTransform(), circle.Color,
		                                 circle.Thickness, circle.Fade, static_cast<i32>(entity));
	}

	Graphics::Renderer2D::EndScene();
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnTick(const TRAP::Utils::TimeStep& deltaTime)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	//Update scripts
	{
		m_registry.view<NativeScriptComponent>().each([this, deltaTime](auto entity, auto& nsc)
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

void TRAP::Scene::OnViewportResize(const u32 width, const u32 height)
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	Entity newEntity = CreateEntity(entity.GetName());
	CopyComponentIfExists(AllComponents{}, newEntity, entity);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Entity TRAP::Scene::GetPrimaryCameraEntity()
{
	ZoneNamedC(__tracy, tracy::Color::Turquoise, (GetTRAPProfileSystems() & ProfileSystems::Scene) != ProfileSystems::None);

	auto view = m_registry.view<CameraComponent>();
	for(auto entity : view)
	{
		auto camera = view.get<CameraComponent>(entity);
		if(camera.Primary)
			return Entity{ entity, this };
	}

	return {};
}
