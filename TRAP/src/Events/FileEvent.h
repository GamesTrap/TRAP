#ifndef TRAP_FILEEVENT_H
#define TRAP_FILEEVENT_H

#include <optional>

#include "Event.h"

namespace TRAP::FileSystem
{
	enum class FileStatus;
}

namespace TRAP::Events
{
    /// <summary>
	/// File change event.
	/// </summary>
	class FileChangeEvent final : public Event
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
        /// <param name="status">Status of the provided file or folder.</param>
        /// <param name="path">Path to a file or folder.</param>
        /// <param name="oldName">Optional: Old name of the file or folder. Only set if status is FileStatus::Renamed.</param>
		FileChangeEvent(TRAP::FileSystem::FileStatus status, std::filesystem::path path,
                        std::optional<std::filesystem::path> oldName = std::nullopt) noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		~FileChangeEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		FileChangeEvent(const FileChangeEvent&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		FileChangeEvent& operator=(const FileChangeEvent&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		FileChangeEvent(FileChangeEvent&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		FileChangeEvent& operator=(FileChangeEvent&&) noexcept = default;

        /// <summary>
        /// Get the status of the file.
        /// </summary>
        /// <returns>The status of the file.</returns>
        constexpr TRAP::FileSystem::FileStatus GetStatus() const;
        /// <summary>
        /// Get the path of the file.
        /// </summary>
        /// <returns>The path of the file.</returns>
        std::filesystem::path GetPath() const;
        /// <summary>
        /// Get the old name of the file.
        /// Only set when FileStatus::Renamed.
        /// </summary>
        /// <returns>The old name of the file.</returns>
        std::optional<std::filesystem::path> GetOldName() const;

		/// <summary>
		/// Get a string representation of the FileEvent.
		/// </summary>
		/// <returns>String representation.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		static constexpr EventType GetStaticType();
		/// <summary>
		/// Retrieve the EventType of the event.
		/// </summary>
		/// <returns>EventType.</returns>
		EventType GetEventType() const override;
		/// <summary>
		/// Retrieve the name of the event.
		/// </summary>
		/// <returns>Name.</returns>
		std::string GetName() const override;
		/// <summary>
		/// Retrieve the category flags of the event.
		/// </summary>
		/// <returns>Combination of one or more EventCategory's.</returns>
		EventCategory GetCategoryFlags() const override;

	private:
        TRAP::FileSystem::FileStatus m_status;
        std::filesystem::path m_path;
        std::optional<std::filesystem::path> m_oldName;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::FileSystem::FileStatus TRAP::Events::FileChangeEvent::GetStatus() const
{
	return m_status;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Events::EventType TRAP::Events::FileChangeEvent::GetStaticType()
{
	return EventType::FileChange;
}

#endif /*TRAP_FILEEVENT_H*/