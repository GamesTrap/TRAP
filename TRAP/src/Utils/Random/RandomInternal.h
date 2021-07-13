#ifndef _TRAP_RANDOMINTERNAL_H_
#define _TRAP_RANDOMINTERNAL_H_

namespace TRAP::Utils::INTERNAL
{
    /// <summary>
    /// Key type for getting common type numbers or objects.
    /// </summary>
    struct Common { };

    /// <summary>
    /// True if type T is applicable by a std::uniform_int_distribution.
    /// </summary>
    /// <typeparam name="T">Type applicable by std::uniform_int_distribution.</typeparam>
    template<typename T>
    struct IsUniformInt
    {
        static constexpr bool value = std::is_same<T,  int16_t>::value ||
                                      std::is_same<T,  int32_t>::value ||
                                      std::is_same<T,  int64_t>::value ||
                                      std::is_same<T, uint16_t>::value ||
                                      std::is_same<T, uint32_t>::value ||
                                      std::is_same<T, uint64_t>::value;
    };

    /// <summary>
    /// True if type T is applicable by a std::uniform_real_distribution.
    /// </summary>
    /// <typeparam name="T">Type applicable by std::uniform_real_distribution.</typeparam>
    template<typename T>
    struct IsUniformReal
    {
        static constexpr bool value = std::is_same<T,       float>::value ||
                                      std::is_same<T,      double>::value ||
                                      std::is_same<T, long double>::value;
    };

    /// <summary>
    /// True if type T is plain byte.
    /// </summary>
    /// <typeparam name="T">Byte type.</typeparam>
    template<typename T>
    struct IsByte
    {
        static constexpr bool value = std::is_same<T,  int8_t>::value ||
                                      std::is_same<T, uint8_t>::value;
    };

    /// <summary>
    /// True if type T is plain number type.
    /// </summary>
    /// <typeparam name="T">Number type.</typeparam>
    template<typename T>
    struct IsSupportedNumber
    {
        static constexpr bool value = IsByte<T>::value ||
                                      IsUniformReal<T>::value ||
                                      IsUniformInt <T>::value;
    };

    /// <summary>
    /// True if type T is character type.
    /// </summary>
    /// <typeparam name="T">Character type.</typeparam>
    template<typename T>
    struct IsSupportedCharacter
    {
        static constexpr bool value = std::is_same<T,     char>::value ||
                                      std::is_same<T,  wchar_t>::value ||
                                      std::is_same<T, char16_t>::value ||
                                      std::is_same<T, char32_t>::value;
    };

    /// <summary>
    /// True if type T is iterator
    /// </summary>
    /// <typeparam name="T">Iterator type.</typeparam>
    template<typename T>
    struct IsIterator
    {
    private:
        static char Test(...) { return 0; }

        template <typename U, typename = typename std::iterator_traits<U>::difference_type,
                              typename = typename std::iterator_traits<U>::pointer,
                              typename = typename std::iterator_traits<U>::reference,
                              typename = typename std::iterator_traits<U>::value_type,
                              typename = typename std::iterator_traits<U>::iterator_category>
        static long Test(U&&) { return 0; }
    public:
        static constexpr bool value = std::is_same<decltype(Test(std::declval<T>())), long>::value;
    };
}

#endif /*_TRAP_RANDOMINTERNAL_H_*/