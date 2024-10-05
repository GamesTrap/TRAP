#ifndef TRAP_UID_H
#define TRAP_UID_H

#include <filesystem>

#include "Core/Types.h"
#include "Scene/SceneSerializer.h"

#include "Utils/Random/Random.h"

namespace TRAP::Utils
{
    /// @brief Unique identifier, basically a randomly generated 64-bit number
    class UID
    {
    public:
        /// @brief Constructor. Generated a new unique identifier.
        UID() = default;

        /// @brief Destructor.
        constexpr ~UID() = default;

        /// @brief Copy constructor.
        consteval UID(const UID& other) noexcept = delete;
        /// @brief Move constructor.
        constexpr UID(UID&& other) noexcept = default;

        /// @brief Copy assignment operator.
        consteval UID& operator=(const UID& other) noexcept = delete;
        /// @brief Move assignment operator.
        constexpr UID& operator=(UID&& other) noexcept = default;

        /// @brief Retrieve the unique identifier as a u64.
        /// @return Unique identifier as u64.
        [[nodiscard]] constexpr operator u64() const noexcept;

    private:
        /// @brief Constructor.
        constexpr explicit UID(u64 uid) noexcept;

        u64 m_uid = TRAP::Utils::RandomThreadLocal::Get();

        friend bool TRAP::SceneSerializer::Deserialize(const std::filesystem::path& filepath);
    };
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Utils::UID::UID(u64 uid) noexcept
    : m_uid(uid)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Utils::UID::operator u64() const noexcept
{
    return m_uid;
}

//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
    template<>
    struct hash<TRAP::Utils::UID>
    {
        [[nodiscard]] constexpr usize operator()(const TRAP::Utils::UID& uid) const noexcept
        {
            return uid;
        }
    };
}

#endif /*TRAP_UID_H*/
