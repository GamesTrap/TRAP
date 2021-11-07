#ifndef TRAP_HOTRELOADEVENT_H
#define TRAP_HOTRELOADEVENT_H

#include "Event.h"

namespace TRAP::Graphics
{
    class Texture;
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
		/// <param name="texture">Pointer to the affected Texture.</param>
		TextureReloadEvent(TRAP::Graphics::Texture* texture);
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~TextureReloadEvent() override = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		TextureReloadEvent(const TextureReloadEvent&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		TextureReloadEvent& operator=(const TextureReloadEvent&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		TextureReloadEvent(TextureReloadEvent&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		TextureReloadEvent& operator=(TextureReloadEvent&&) = default;

		/// <summary>
		/// Retrieve a pointer to the affected Texture.
		/// </summary>
		/// <returns>Texture pointer.</returns>
		TRAP::Graphics::Texture* GetTexture() const;

		/// <summary>
		/// Get a string representation of the TextureReloadEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		static EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the Event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the Event.
		/// </summary>
		/// <returns>Name.</returns>
		const char* GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the Event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	private:
        TRAP::Graphics::Texture* m_texture;
	};
}

#endif /*TRAP_HOTRELOADEVENT_H*/
