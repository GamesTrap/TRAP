#include "TRAPPCH.h"
#include "Scene.h"

#include "Components.h"
#include "Graphics/Renderer2D.h"
#include "Utils/Time/TimeStep.h"

TRAP::Scene::Scene()
{
	
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scene::~Scene() = default;

//-------------------------------------------------------------------------------------------------------------------//

entt::entity TRAP::Scene::CreateEntity()
{
	return m_registry.create();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Scene::OnUpdate(Utils::TimeStep deltaTime)
{
	auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
	for(auto entity : group)
	{
		auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

		Graphics::Renderer2D::DrawQuad(transform, sprite.Color, nullptr);
	}
}