#ifndef TRAP_VERSION_H

#include <optional>
#include <string_view>
#include <string>

#include <fmt/core.h>

#include "Types.h"

namespace TRAP
{
    /// @brief Semantic version.
    /// @tparam major Major version number.
    /// @tparam minor Minor version number.
    /// @tparam patch Patch version number.
    template<u32 major, u32 minor, u32 patch>
    class SemanticVersion
    {
        static_assert(major < 1024, "Major version number must be less than 1024.");
        static_assert(minor < 1024, "Minor version number must be less than 1024.");
        static_assert(patch < 4096, "Patch version number must be less than 4096.");

    public:
        /// @brief Constructor.
        /// @param preReleaseIdentifier Optional: Pre release identifier.
        /// @param metadataIdentifier Optional: Metadata identifier.
        consteval explicit SemanticVersion(const std::optional<std::string_view> preReleaseIdentifier = std::nullopt,
                                           const std::optional<std::string_view> metadataIdentifier = std::nullopt) noexcept
            : m_preReleaseIdentifier(preReleaseIdentifier), m_metadataIdentifier(metadataIdentifier)
        {
        }

        /// @brief Retrieve packed version.
        /// @return Packed version.
        [[nodiscard]] consteval u32 GetPackedVersion() const noexcept
        {
            return major << 22u | minor << 12u | patch;
        }

        /// @brief Extract major version.
        /// @return Major version number.
        [[nodiscard]] consteval u32 Major() const noexcept
        {
            return major;
        }

        /// @brief Extract minor version.
        /// @return Minor version number.
        [[nodiscard]] consteval u32 Minor() const noexcept
        {
            return minor;
        }

        /// @brief Extract patch version.
        /// @return Patch version number.
        [[nodiscard]] consteval u32 Patch() const noexcept
        {
            return patch;
        }

        /// @brief Extract the pre release identifier.
        /// @return Pre release identifier.
        [[nodiscard]] consteval std::optional<std::string_view> PreReleaseIdentifier() const noexcept
        {
            return m_preReleaseIdentifier;
        }

        /// @brief Extract the metadata identifier.
        /// @return Metadata identifier.
        [[nodiscard]] consteval std::optional<std::string_view> MetadataIdentifier() const noexcept
        {
            return m_metadataIdentifier;
        }

        [[nodiscard]] constexpr std::string ToString() const
        {
            std::string str = fmt::format("{}.{}.{}", major, minor, patch);

            if(m_preReleaseIdentifier)
                str += fmt::format("-{}", *m_preReleaseIdentifier);

            if(m_metadataIdentifier)
                str += fmt::format("+{}", *m_metadataIdentifier);

            return str;
        }

    private:
        std::optional<std::string_view> m_preReleaseIdentifier = std::nullopt;
        std::optional<std::string_view> m_metadataIdentifier = std::nullopt;
    };
}

#endif /*TRAP_VERSION_H*/