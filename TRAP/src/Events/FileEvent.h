#ifndef TRAP_FILEEVENT_H
#define TRAP_FILEEVENT_H

#include "Event.h"
#include "FS/FileWatcher.h"

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
        /// <param name="oldName">Old name of the file or folder. Only if FileStatus::Renamed.</param>
		FileChangeEvent(TRAP::FS::FileStatus status, std::filesystem::path path,
                        std::filesystem::path oldName = "");
		/// <summary>
		/// Destructor.
		/// </summary>
		~FileChangeEvent() override = default;
		/// <summary>
		/// Copy constructor.
		/// </summary>
		FileChangeEvent(const FileChangeEvent&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		FileChangeEvent& operator=(const FileChangeEvent&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		FileChangeEvent(FileChangeEvent&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		FileChangeEvent& operator=(FileChangeEvent&&) = default;

        /// <summary>
        /// Get the status of the file.
        /// </summary>
        /// <returns>The status of the file.</returns>
        TRAP::FS::FileStatus GetStatus() const;
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
        std::filesystem::path GetOldName() const;

		/// <summary>
		/// Get a string representation of the FileEvent.
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
        static std::string FileStatusToString(TRAP::FS::FileStatus status);

        TRAP::FS::FileStatus m_status;
        std::filesystem::path m_path;
        std::filesystem::path m_oldName;
	};
}

#endif /*TRAP_FILEEVENT_H*/