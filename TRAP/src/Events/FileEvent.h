#ifndef TRAP_FILEEVENT_H
#define TRAP_FILEEVENT_H

#include <optional>

#include "Event.h"
#include "FileSystem/FileWatcher.h"

namespace TRAP::FileSystem
{
	enum class FileStatus;
}

namespace TRAP::Events
{
	/// @brief File change event.
	class FileChangeEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param status Status of the provided file or folder.
		/// @param path Path to a file or folder.
		/// @param oldName Optional: Old name of the file or folder. Only set if status is FileStatus::Renamed.
		FileChangeEvent(TRAP::FileSystem::FileStatus status, std::filesystem::path path,
                        const std::optional<std::filesystem::path>& oldName = std::nullopt) noexcept;
		/// @brief Destructor.
		~FileChangeEvent() override = default;
		/// @brief Copy constructor.
		consteval FileChangeEvent(const FileChangeEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval FileChangeEvent& operator=(const FileChangeEvent&) noexcept = delete;
		/// @brief Move constructor.
		FileChangeEvent(FileChangeEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		FileChangeEvent& operator=(FileChangeEvent&&) noexcept = default;

		/// @brief Get the status of the file.
		/// @return The status of the file.
        [[nodiscard]] constexpr TRAP::FileSystem::FileStatus GetStatus() const noexcept;
		/// @brief Get the path of the file.
		/// @return The path of the file.
        [[nodiscard]] std::filesystem::path GetPath() const noexcept;
		/// @brief Get the old name of the file.
		/// @return The old name of the file.
		/// @note Only set when FileStatus is FileStatus::Renamed.
        [[nodiscard]] std::optional<std::filesystem::path> GetOldName() const noexcept;

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
		/// @brief Get a string representation of the event.
		/// @return String representation.
		[[nodiscard]] std::string ToString() const override;

	private:
        TRAP::FileSystem::FileStatus m_status;
        std::filesystem::path m_path;
        std::optional<std::filesystem::path> m_oldName;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::FileSystem::FileStatus TRAP::Events::FileChangeEvent::GetStatus() const noexcept
{
	return m_status;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::FileChangeEvent::GetStaticType() noexcept
{
	return EventType::FileChange;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::FileChangeEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::FileChangeEvent::GetName() const
{
	return "FileChange";
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::FileChangeEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::FileChange;
}

#endif /*TRAP_FILEEVENT_H*/