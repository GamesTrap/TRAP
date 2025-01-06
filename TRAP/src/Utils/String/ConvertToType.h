#ifndef TRAP_STRING_CONVERTTOTYPE_H
#define TRAP_STRING_CONVERTTOTYPE_H

#include <string>

#include "Core/Types.h"
#include "Utils/String/String.h"

namespace TRAP::Utils::String
{
	/// @brief Convert a string to the given type.
	/// @tparam T Type to get
	/// @param input String to convert.
	/// @return String converted to type T.
	template<typename T>
	[[nodiscard]] constexpr T ConvertToType(const std::string& input);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] constexpr char TRAP::Utils::String::ConvertToType(const std::string& input)
{
    return !input.empty() ? input[0] : '\0';
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] constexpr bool TRAP::Utils::String::ConvertToType(const std::string& input)
{
    return String::CompareAnyCase("TRUE", input);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] constexpr std::string TRAP::Utils::String::ConvertToType(const std::string& input)
{
    return input;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] inline u8 TRAP::Utils::String::ConvertToType(const std::string& input)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    try
    {
        return NumericCast<u8>(std::stoul(input));
    }
    catch(...)
    {
        TP_ERROR(TRAP::Log::UtilsStringPrefix, "Exception while converting string!");
        return 0u;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] inline i8 TRAP::Utils::String::ConvertToType(const std::string& input)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    try
    {
        return NumericCast<i8>(std::stol(input));
    }
    catch(...)
    {
        TP_ERROR(TRAP::Log::UtilsStringPrefix, "Exception while converting string!");
        return 0;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] inline u16 TRAP::Utils::String::ConvertToType(const std::string& input)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    try
    {
        return NumericCast<u16>(std::stoul(input));
    }
    catch(...)
    {
        TP_ERROR(TRAP::Log::UtilsStringPrefix, "Exception while converting string!");
        return 0u;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] inline i16 TRAP::Utils::String::ConvertToType(const std::string& input)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    try
    {
        return NumericCast<i16>(std::stol(input));
    }
    catch(...)
    {
        TP_ERROR(TRAP::Log::UtilsStringPrefix, "Exception while converting string!");
        return 0;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] inline u32 TRAP::Utils::String::ConvertToType(const std::string& input)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    try
    {
        return static_cast<u32>(std::stoul(input));
    }
    catch(...)
    {
        TP_ERROR(TRAP::Log::UtilsStringPrefix, "Exception while converting string!");
        return 0u;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] inline i32 TRAP::Utils::String::ConvertToType(const std::string& input)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    try
    {
        return static_cast<i32>(std::stol(input));
    }
    catch(...)
    {
        TP_ERROR(TRAP::Log::UtilsStringPrefix, "Exception while converting string!");
        return 0;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] inline u64 TRAP::Utils::String::ConvertToType(const std::string& input)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    try
    {
        return std::stoull(input);
    }
    catch(...)
    {
        TP_ERROR(TRAP::Log::UtilsStringPrefix, "Exception while converting string!");
        return 0u;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] inline i64 TRAP::Utils::String::ConvertToType(const std::string& input)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    try
    {
        return std::stoll(input);
    }
    catch(...)
    {
        TP_ERROR(TRAP::Log::UtilsStringPrefix, "Exception while converting string!");
        return 0;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] inline f32 TRAP::Utils::String::ConvertToType(const std::string& input)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    try
    {
        return std::stof(input);
    }
    catch(...)
    {
        TP_ERROR(TRAP::Log::UtilsStringPrefix, "Exception while converting string!");
        return 0.0f;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
[[nodiscard]] inline f64 TRAP::Utils::String::ConvertToType(const std::string& input)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    try
    {
        return std::stod(input);
    }
    catch(...)
    {
        TP_ERROR(TRAP::Log::UtilsStringPrefix, "Exception while converting string!");
        return 0.0;
    }
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] constexpr T TRAP::Utils::String::ConvertToType([[maybe_unused]] const std::string& input)
{
    static_assert(sizeof(T) == 0, "Unconvertable type encountered. For custom types add a specialization for TRAP::Utils::String::ConvertToType()");
}

#endif /*TRAP_STRING_CONVERTTOTYPE_H*/
