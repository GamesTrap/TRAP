#ifndef CONVERTTOSPIRV_TYPES_H
#define CONVERTTOSPIRV_TYPES_H

#include <cstdint>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

#if __STDCPP_FLOAT32_T__ == 1
    using f32 = std::float32_t;
#else
    static_assert(sizeof(float) >= 4, "Float must be at least 32-bits!");
    using f32 = float;
#endif
#if __STDCPP_FLOAT64_T__ == 1
    using f64 = std::float64_t;
#else
    static_assert(sizeof(double) >= 8, "Double must be at least 64-bits!");
    using f64 = double;
#endif

using usize = std::size_t;
using isize = std::ptrdiff_t;

using iptr = std::intptr_t;
using uptr = std::uintptr_t;

#endif /*CONVERTTOSPIRV_TYPES_H*/
