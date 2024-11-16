#include "TRAPPCH.h"
#include "HotReloadEvent.h"

#include "Graphics/Textures/Texture.h"
#include "Graphics/Shaders/Shader.h"

TRAP::Events::TextureReloadEvent::TextureReloadEvent(TRAP::Ref<TRAP::Graphics::Texture> texture)
	: m_texture(std::move(texture))
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (GetTRAPProfileSystems() & ProfileSystems::Events) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	TRAP_ASSERT(m_texture, "TextureReloadEvent(): Texture is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Events::TextureReloadEvent::GetTexture() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (GetTRAPProfileSystems() & ProfileSystems::Events) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_texture;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::TextureReloadEvent::ToString() const
{
	return fmt::format("TextureReloadEvent: {}", m_texture ? m_texture->GetName() : "nullptr");
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::ShaderReloadEvent::ShaderReloadEvent(TRAP::Ref<TRAP::Graphics::Shader> shader)
    : m_shader(std::move(shader))
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (GetTRAPProfileSystems() & ProfileSystems::Events) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	TRAP_ASSERT(m_shader, "ShaderReloadEvent(): Shader is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Shader> TRAP::Events::ShaderReloadEvent::GetShader() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Purple, (GetTRAPProfileSystems() & ProfileSystems::Events) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    return m_shader;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Events::ShaderReloadEvent::ToString() const
{
	return fmt::format("ShaderReloadEvent: {}", m_shader ? m_shader->GetName() : "nullptr");
}
