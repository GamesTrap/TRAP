#ifndef TRAP_FILEEVENT_H
#define TRAP_FILEEVENT_H

#include "Event.h"
#include "FileSystem/FileSystemWatcher.h"
#include "Utils/Optional.h"

namespace TRAP::FileSystem
{
	enum class FileSystemStatus;
}

namespace TRAP::Events
{
	/// @brief Filesystem change event.
	class FileSystemChangeEvent final : public Event
	{
	public:
		/// @brief Constructor.
		/// @param status Status of the provided file or folder.
		/// @param path Path to a file or folder.
		/// @param oldName Optional: Old path of the file or folder. Only set if status is FileSystemStatus::Renamed.
		FileSystemChangeEvent(TRAP::FileSystem::FileSystemStatus status, std::filesystem::path path,
                              const TRAP::Optional<std::filesystem::path>& oldPath = TRAP::NullOpt) noexcept;
		/// @brief Destructor.
		~FileSystemChangeEvent() override = default;
		/// @brief Copy constructor.
		consteval FileSystemChangeEvent(const FileSystemChangeEvent&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval FileSystemChangeEvent& operator=(const FileSystemChangeEvent&) noexcept = delete;
		/// @brief Move constructor.
		FileSystemChangeEvent(FileSystemChangeEvent&&) noexcept = default;
		/// @brief Move assignment operator.
		FileSystemChangeEvent& operator=(FileSystemChangeEvent&&) noexcept = default;

		/// @brief Get the status of the file or folder.
		/// @return The status of the file or folder.
        [[nodiscard]] constexpr TRAP::FileSystem::FileSystemStatus GetStatus() const noexcept;
		/// @brief Get the path of the file or folder.
		/// @return The path of the file or folder.
        [[nodiscard]] std::filesystem::path GetPath() const noexcept;
		/// @brief Get the old path of the file or folder.
		/// @return The old path of the file or folder if any, empty optional otherwise.
		/// @note Only set when FileSystemStatus is FileSystemStatus::Renamed.
        [[nodiscard]] TRAP::Optional<std::filesystem::path> GetOldPath() const noexcept;

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
        TRAP::FileSystem::FileSystemStatus m_status;
        std::filesystem::path m_path;
        TRAP::Optional<std::filesystem::path> m_oldPath;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::FileSystem::FileSystemStatus TRAP::Events::FileSystemChangeEvent::GetStatus() const noexcept
{
	return m_status;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::FileSystemChangeEvent::GetStaticType() noexcept
{
	return EventType::FileChange;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventType TRAP::Events::FileSystemChangeEvent::GetEventType() const noexcept
{
	return GetStaticType();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Events::FileSystemChangeEvent::GetName() const
{
	return "FileSystemChange";
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Events::EventCategory TRAP::Events::FileSystemChangeEvent::GetCategoryFlags() const noexcept
{
	return EventCategory::FileSystemChange;
}

#endif /*TRAP_FILEEVENT_H*/