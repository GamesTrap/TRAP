#include "TRAPPCH.h"
#include "HotReloadEvent.h"

#include "Graphics/Textures/Texture.h"
#include "Graphics/Shaders/Shader.h"

TRAP::Events::TextureReloadEvent::TextureReloadEvent(TRAP::Ref<TRAP::Graphics::Texture> texture)
	: m_texture(texture)
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(m_texture, "TextureReloadEvent(): Texture is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Events::TextureReloadEvent::GetTexture() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_texture;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::TextureReloadEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(m_texture, "TextureReloadEvent::ToString(): Texture is nullptr!");

    return "TextureReloadEvent: " + m_texture->GetName();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::TextureReloadEvent::GetEventType() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::TextureReloadEvent::GetName() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "TextureReload";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::TextureReloadEvent::GetCategoryFlags() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return EventCategory::HotReload;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::ShaderReloadEvent::ShaderReloadEvent(TRAP::Ref<TRAP::Graphics::Shader> shader)
    : m_shader(shader)
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(m_shader, "ShaderReloadEvent(): Shader is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Shader> TRAP::Events::ShaderReloadEvent::GetShader() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    return m_shader;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::ShaderReloadEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(m_shader, "ShaderReloadEvent::ToString(): Shader is nullptr!");

    return "ShaderReloadEvent: " + m_shader->GetName();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::ShaderReloadEvent::GetEventType() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::ShaderReloadEvent::GetName() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return "ShaderReload";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::ShaderReloadEvent::GetCategoryFlags() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return EventCategory::HotReload;
}