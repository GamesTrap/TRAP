#include "TRAPPCH.h"
#include "HotReloadEvent.h"

#include "Graphics/Textures/Texture.h"
#include "Graphics/Shaders/Shader.h"

std::string TRAP::Events::TextureReloadEvent::ToString() const
{
    return "TextureReloadEvent: " + m_texture->GetName();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::TextureReloadEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::TextureReloadEvent::GetName() const
{
	return "TextureReload";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::TextureReloadEvent::GetCategoryFlags() const
{
	return EventCategory::HotReload;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::ShaderReloadEvent::ShaderReloadEvent(TRAP::Ref<TRAP::Graphics::Shader> shader)
    : m_shader(shader)
{}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Shader> TRAP::Events::ShaderReloadEvent::GetShader() const
{
    return m_shader;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::ShaderReloadEvent::ToString() const
{
    return "ShaderReloadEvent: " + m_shader->GetName();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventType TRAP::Events::ShaderReloadEvent::GetEventType() const
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Events::ShaderReloadEvent::GetName() const
{
	return "ShaderReload";
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Events::EventCategory TRAP::Events::ShaderReloadEvent::GetCategoryFlags() const
{
	return EventCategory::HotReload;
}