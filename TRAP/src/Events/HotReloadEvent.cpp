#include "TRAPPCH.h"
#include "HotReloadEvent.h"

#include "Graphics/Textures/Texture.h"
#include "Graphics/Shaders/Shader.h"

TRAP::Events::TextureReloadEvent::TextureReloadEvent(TRAP::Ref<TRAP::Graphics::Texture> texture)
	: m_texture(std::move(texture))
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(m_texture, "TextureReloadEvent(): Texture is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Events::TextureReloadEvent::GetTexture() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_texture;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::TextureReloadEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    return fmt::format("TextureReloadEvent: {}", m_texture->GetName());
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::ShaderReloadEvent::ShaderReloadEvent(TRAP::Ref<TRAP::Graphics::Shader> shader)
    : m_shader(std::move(shader))
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	TRAP_ASSERT(m_shader, "ShaderReloadEvent(): Shader is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Shader> TRAP::Events::ShaderReloadEvent::GetShader() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    return m_shader;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::ShaderReloadEvent::ToString() const
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Events) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    return fmt::format("ShaderReloadEvent: {}", m_shader->GetName());
}
