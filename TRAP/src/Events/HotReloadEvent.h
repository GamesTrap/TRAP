#ifndef TRAP_HOTRELOADEVENT_H
#define TRAP_HOTRELOADEVENT_H

#include "Event.h"

namespace TRAP::Graphics
{
    class Texture;
    class Shader;
}

namespace TRAP::Events
{
	/// <summary>
	/// Texture reloaded.
	/// </summary>
	class TextureReloadEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="texture">Pointer to the affected texture.</param>
		TextureReloadEvent(TRAP::Ref<TRAP::Graphics::Texture> texture);
		/// <summary>
		/// Destructor.
		/// </summary>
		~TextureReloadEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		TextureReloadEvent(const TextureReloadEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		TextureReloadEvent& operator=(const TextureReloadEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		TextureReloadEvent(TextureReloadEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		TextureReloadEvent& operator=(TextureReloadEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve a pointer to the affected texture.
		/// </summary>
		/// <returns>Texture pointer.</returns>
		[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> GetTexture() const noexcept;

		/// <summary>
		/// Get a string representation of the TextureReloadEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] EventCategory GetCategoryFlags() const noexcept override;

	private:
        TRAP::Ref<TRAP::Graphics::Texture> m_texture;
	};

    /// <summary>
	/// Shader reloaded.
	/// </summary>
	class ShaderReloadEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="shader">Affected shader.</param>
		ShaderReloadEvent(TRAP::Ref<TRAP::Graphics::Shader> shader);
		/// <summary>
		/// Destructor.
		/// </summary>
		~ShaderReloadEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		ShaderReloadEvent(const ShaderReloadEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		ShaderReloadEvent& operator=(const ShaderReloadEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		ShaderReloadEvent(ShaderReloadEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		ShaderReloadEvent& operator=(ShaderReloadEvent&&) noexcept = default;

		/// <summary>
		/// Retrieve the affected shader.
		/// </summary>
		/// <returns>Shader.</returns>
		[[nodiscard]] TRAP::Ref<TRAP::Graphics::Shader> GetShader() const noexcept;

		/// <summary>
		/// Get a string representation of the ShaderReloadEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		[[nodiscard]] std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		[[nodiscard]] EventType GetEventType() const noexcept override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		[[nodiscard]] std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		[[nodiscard]] EventCategory GetCategoryFlags() const noexcept override;

	private:
        TRAP::Ref<TRAP::Graphics::Shader> m_shader;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::TextureReloadEvent::GetStaticType() noexcept
{
	return EventType::TextureReload;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::ShaderReloadEvent::GetStaticType() noexcept
{
	return EventType::ShaderReload;
}

#endif /*TRAP_HOTRELOADEVENT_H*/