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
	/// @brief Texture reloaded event.
	class TextureReloadEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param texture Pointer to the affected texture.
		explicit TextureReloadEvent(TRAP::Ref<TRAP::Graphics::Texture> texture);
		/// @brief Destructor.
		~TextureReloadEvent() override = default;
		/// @brief Copy constructor.
		TextureReloadEvent(const TextureReloadEvent&) noexcept = default;
		/// @brief Copy assignment operator.
		TextureReloadEvent& operator=(const TextureReloadEvent&) noexcept = default;
		/// @brief Move constructor.
		TextureReloadEvent(TextureReloadEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		TextureReloadEvent& operator=(TextureReloadEvent&&) noexcept = default;

		/// @brief Retrieve a pointer to the affected texture.
		/// @return Texture pointer.
		[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> GetTexture() const noexcept;

		/// @brief Get a string representation of the TextureReloadEvent.
		/// @return String representation.
		[[nodiscard]] std::string ToString() const override;

		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// @brief Retrieve the name of the event.
		/// @return Name.
		[[nodiscard]] constexpr std::string GetName() const override;
		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

	private:
        TRAP::Ref<TRAP::Graphics::Texture> m_texture;
	};

	/// @brief Shader reloaded event.
	class ShaderReloadEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param shader Affected shader.
		explicit ShaderReloadEvent(TRAP::Ref<TRAP::Graphics::Shader> shader);
		/// @brief Destructor.
		~ShaderReloadEvent() override = default;
		/// @brief Copy constructor.
		ShaderReloadEvent(const ShaderReloadEvent&) noexcept = default;
		/// @brief Copy assignment operator.
		ShaderReloadEvent& operator=(const ShaderReloadEvent&) noexcept = default;
		/// @brief Move constructor.
		ShaderReloadEvent(ShaderReloadEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		ShaderReloadEvent& operator=(ShaderReloadEvent&&) noexcept = default;

		/// @brief Retrieve the affected shader.
		/// @return Shader.
		[[nodiscard]] TRAP::Ref<TRAP::Graphics::Shader> GetShader() const noexcept;

		/// @brief Get a string representation of the ShaderReloadEvent.
		/// @return String representation.
		[[nodiscard]] std::string ToString() const override;

		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] static constexpr EventType GetStaticType() noexcept;
		/// @brief Retrieve the EventType of the event.
		/// @return EventType.
		[[nodiscard]] constexpr EventType GetEventType() const noexcept override;
		/// @brief Retrieve the name of the event.
		/// @return Name.
		[[nodiscard]] constexpr std::string GetName() const override;
		/// @brief Retrieve the category flags of the event.
		/// @return Combination of one or more EventCategory's.
		[[nodiscard]] constexpr EventCategory GetCategoryFlags() const noexcept override;

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

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::TextureReloadEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::TextureReloadEvent::GetName() const
{
	return "TextureReload";
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::TextureReloadEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::HotReload;
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::ShaderReloadEvent::GetStaticType() noexcept
{
	return EventType::ShaderReload;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::ShaderReloadEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::ShaderReloadEvent::GetName() const
{
	return "ShaderReload";
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::ShaderReloadEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::HotReload;
}

#endif /*TRAP_HOTRELOADEVENT_H*/