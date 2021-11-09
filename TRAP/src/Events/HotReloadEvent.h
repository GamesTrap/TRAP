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
		TextureReloadEvent(TRAP::Graphics::Texture* texture);
		/// <summary>
		/// Destructor.
		/// </summary>
		~TextureReloadEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		TextureReloadEvent(const TextureReloadEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		TextureReloadEvent& operator=(const TextureReloadEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		TextureReloadEvent(TextureReloadEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		TextureReloadEvent& operator=(TextureReloadEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected texture.
		/// </summary>
		/// <returns>Texture pointer.</returns>
		TRAP::Graphics::Texture* GetTexture() const;

		/// <summary>
		/// Get a string representation of the TextureReloadEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	private:
        TRAP::Graphics::Texture* m_texture;
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
		/// <param name="shader">Pointer to the affected shader.</param>
		ShaderReloadEvent(TRAP::Graphics::Shader* shader);
		/// <summary>
		/// Destructor.
		/// </summary>
		~ShaderReloadEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		ShaderReloadEvent(const ShaderReloadEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		ShaderReloadEvent& operator=(const ShaderReloadEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		ShaderReloadEvent(ShaderReloadEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		ShaderReloadEvent& operator=(ShaderReloadEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected shader.
		/// </summary>
		/// <returns>Shader pointer.</returns>
		TRAP::Graphics::Shader* GetShader() const;

		/// <summary>
		/// Get a string representation of the ShaderReloadEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	private:
        TRAP::Graphics::Shader* m_shader;
	};
}

#endif /*TRAP_HOTRELOADEVENT_H*/