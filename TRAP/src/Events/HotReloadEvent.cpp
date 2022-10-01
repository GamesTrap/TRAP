#include "TRAPPCH.h"
#include "HotReloadEvent.h"

#include "Graphics/Textures/Texture.h"
#include "Graphics/Shaders/Shader.h"

TRAP::Events::TextureReloadEvent::TextureReloadEvent(TRAP::Ref<TRAP::Graphics::Texture> texture)
	: m_texture(texture)
{
	ZoneScopedC(tracy::Color::Purple);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Events::TextureReloadEvent::GetTexture() const
{
	ZoneScopedC(tracy::Color::Purple);

	return m_texture;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::TextureReloadEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

    return "TextureReloadEvent: " + m_texture->GetName();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::TextureReloadEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::TextureReloadEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "TextureReload";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::TextureReloadEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::HotReload;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::ShaderReloadEvent::ShaderReloadEvent(TRAP::Ref<TRAP::Graphics::Shader> shader)
    : m_shader(shader)
{
	ZoneScopedC(tracy::Color::Purple);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Shader> TRAP::Events::ShaderReloadEvent::GetShader() const
{
	ZoneScopedC(tracy::Color::Purple);

    return m_shader;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::ShaderReloadEvent::ToString() const
{
	ZoneScopedC(tracy::Color::Purple);

    return "ShaderReloadEvent: " + m_shader->GetName();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::ShaderReloadEvent::GetEventType() const
{
	ZoneScopedC(tracy::Color::Purple);

	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::ShaderReloadEvent::GetName() const
{
	ZoneScopedC(tracy::Color::Purple);

	return "ShaderReload";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::ShaderReloadEvent::GetCategoryFlags() const
{
	ZoneScopedC(tracy::Color::Purple);

	return EventCategory::HotReload;
}