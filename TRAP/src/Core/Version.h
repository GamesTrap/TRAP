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

        /// @brief Copy constructor.
        consteval SemanticVersion(const SemanticVersion&) = delete;
        /// @brief Move constructor.
        consteval SemanticVersion(SemanticVersion&&) noexcept = default;

        /// @brief Copy assignment operator.
        consteval SemanticVersion& operator=(const SemanticVersion&) = delete;
        /// @brief Move assignment operator.
        consteval SemanticVersion& operator=(SemanticVersion&&) noexcept = default;

        /// @brief Destructor.
        constexpr ~SemanticVersion() = default;

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
        [[nodiscard]] constexpr std::optional<std::string_view> PreReleaseIdentifier() const noexcept
        {
            return m_preReleaseIdentifier;
        }

        /// @brief Extract the metadata identifier.
        /// @return Metadata identifier.
        [[nodiscard]] constexpr std::optional<std::string_view> MetadataIdentifier() const noexcept
        {
            return m_metadataIdentifier;
        }

    private:
        std::optional<std::string_view> m_preReleaseIdentifier = std::nullopt;
        std::optional<std::string_view> m_metadataIdentifier = std::nullopt;
    };
}

template<u32 major, u32 minor, u32 patch>
struct fmt::formatter<TRAP::SemanticVersion<major, minor, patch>>
{
    constexpr auto parse(const fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    fmt::format_context::iterator format(const TRAP::SemanticVersion<major, minor, patch>& semVer, fmt::format_context& ctx) const
    {
        auto out = ctx.out();
        out = fmt::format_to(out, "{}.{}.{}", major, minor, patch);

        if(semVer.PreReleaseIdentifier())
            out = fmt::format_to(out, "-{}", *semVer.PreReleaseIdentifier());

        if(semVer.MetadataIdentifier())
            out = fmt::format_to(out, "+{}", *semVer.MetadataIdentifier());

        return out;
    }
};

#endif /*TRAP_VERSION_H*/
