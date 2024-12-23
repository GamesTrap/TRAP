#include <catch2/catch_test_macros.hpp>

#include "Utils/String/String.h"
#include "Utils/String/ConvertToType.h"

TEST_CASE("TRAP::Utils::String", "[utils][string]")
{
    SECTION("SplitStringView(string_view, string_view)")
    {
        static constexpr std::string_view a = "Lorem,ipsum,hello,world!";
        static constexpr std::string_view b = "Lorem,ipsum,hello,world !";
        static constexpr std::string_view c = "Lorem,";
        static constexpr std::string_view d = "Lorem,ipsum";
        static constexpr std::string_view e = ",ipsum";

        STATIC_REQUIRE(TRAP::Utils::String::SplitStringView(a, ",") == std::vector<std::string_view>{"Lorem", "ipsum", "hello", "world!"});
        STATIC_REQUIRE(TRAP::Utils::String::SplitStringView(a, "") == std::vector<std::string_view>{a});
        STATIC_REQUIRE(TRAP::Utils::String::SplitStringView(b, ", ") == std::vector<std::string_view>{"Lorem", "ipsum", "hello", "world", "!"});
        STATIC_REQUIRE(TRAP::Utils::String::SplitStringView(c, ",") == std::vector<std::string_view>{"Lorem"});
        STATIC_REQUIRE(TRAP::Utils::String::SplitStringView(d, ",") == std::vector<std::string_view>{"Lorem", "ipsum"});
        STATIC_REQUIRE(TRAP::Utils::String::SplitStringView(e, ",") == std::vector<std::string_view>{"ipsum"});
        STATIC_REQUIRE(TRAP::Utils::String::SplitStringView("", ",") == std::vector<std::string_view>{});
    }

    SECTION("SplitStringView(string_view, char)")
    {
        static constexpr std::string_view a = "Lorem,ipsum,hello,world!";
        static constexpr std::string_view b = "Lorem,";
        static constexpr std::string_view c = "Lorem,ipsum";
        static constexpr std::string_view d = ",ipsum";

        STATIC_REQUIRE(TRAP::Utils::String::SplitStringView(a, ',') == std::vector<std::string_view>{"Lorem", "ipsum", "hello", "world!"});
        STATIC_REQUIRE(TRAP::Utils::String::SplitStringView(b, ",") == std::vector<std::string_view>{"Lorem"});
        STATIC_REQUIRE(TRAP::Utils::String::SplitStringView(c, ",") == std::vector<std::string_view>{"Lorem", "ipsum"});
        STATIC_REQUIRE(TRAP::Utils::String::SplitStringView(d, ",") == std::vector<std::string_view>{"ipsum"});
        STATIC_REQUIRE(TRAP::Utils::String::SplitStringView("", ",") == std::vector<std::string_view>{});
    }

    SECTION("SplitString(string, string_view)")
    {
        static constexpr std::string_view a = "Lorem,ipsum,hello,world!";
        static constexpr std::string_view b = "Lorem,ipsum,hello,world !";
        static constexpr std::string_view c = "Lorem,";
        static constexpr std::string_view d = "Lorem,ipsum";
        static constexpr std::string_view e = ",ipsum";

        REQUIRE(TRAP::Utils::String::SplitString(std::string(a), ",") == std::vector<std::string>{"Lorem", "ipsum", "hello", "world!"});
        REQUIRE(TRAP::Utils::String::SplitString(std::string(a), "") == std::vector<std::string>{std::string(a)});
        REQUIRE(TRAP::Utils::String::SplitString(std::string(b), ", ") == std::vector<std::string>{"Lorem", "ipsum", "hello", "world", "!"});
        REQUIRE(TRAP::Utils::String::SplitString(std::string(c), ",") == std::vector<std::string>{"Lorem"});
        REQUIRE(TRAP::Utils::String::SplitString(std::string(d), ",") == std::vector<std::string>{"Lorem", "ipsum"});
        REQUIRE(TRAP::Utils::String::SplitString(std::string(e), ",") == std::vector<std::string>{"ipsum"});
        REQUIRE(TRAP::Utils::String::SplitString("", ",") == std::vector<std::string>{});
    }

    SECTION("SplitString(string, char)")
    {
        static constexpr std::string_view a = "Lorem,ipsum,hello,world!";
        static constexpr std::string_view b = "Lorem,";
        static constexpr std::string_view c = "Lorem,ipsum";
        static constexpr std::string_view d = ",ipsum";

        REQUIRE(TRAP::Utils::String::SplitString(std::string(a), ',') == std::vector<std::string>{"Lorem", "ipsum", "hello", "world!"});
        REQUIRE(TRAP::Utils::String::SplitString(std::string(b), ",") == std::vector<std::string>{"Lorem"});
        REQUIRE(TRAP::Utils::String::SplitString(std::string(c), ",") == std::vector<std::string>{"Lorem", "ipsum"});
        REQUIRE(TRAP::Utils::String::SplitString(std::string(d), ",") == std::vector<std::string>{"ipsum"});
        REQUIRE(TRAP::Utils::String::SplitString("", ",") == std::vector<std::string>{});
    }

    SECTION("GetLinesStringView()")
    {
        static constexpr std::string_view a = "First line...";
        static constexpr std::string_view b = "First line...\nSecond long line\nLast line.";
        static constexpr std::string_view c = "First line...\r\nSecond long line\r\nLast line.";
        static constexpr std::string_view d = "First line...\r\nSecond long line\nLast line.";

        STATIC_REQUIRE(TRAP::Utils::String::GetLinesStringView(a) == std::vector<std::string_view>{a});
        STATIC_REQUIRE(TRAP::Utils::String::GetLinesStringView(b) == std::vector<std::string_view>{"First line...", "Second long line", "Last line."});
        STATIC_REQUIRE(TRAP::Utils::String::GetLinesStringView(c) == std::vector<std::string_view>{"First line...", "Second long line", "Last line."});
        STATIC_REQUIRE(TRAP::Utils::String::GetLinesStringView(d) == std::vector<std::string_view>{"First line...", "Second long line", "Last line."});
        STATIC_REQUIRE(TRAP::Utils::String::GetLinesStringView("") == std::vector<std::string_view>{});
    }

    SECTION("GetLines()")
    {
        static constexpr std::string_view a = "First line...";
        static constexpr std::string_view b = "First line...\nSecond long line\nLast line.";
        static constexpr std::string_view c = "First line...\r\nSecond long line\r\nLast line.";
        static constexpr std::string_view d = "First line...\r\nSecond long line\nLast line.";

        REQUIRE(TRAP::Utils::String::GetLines(std::string(a)) == std::vector<std::string>{std::string(a)});
        REQUIRE(TRAP::Utils::String::GetLines(std::string(b)) == std::vector<std::string>{"First line...", "Second long line", "Last line."});
        REQUIRE(TRAP::Utils::String::GetLines(std::string(c)) == std::vector<std::string>{"First line...", "Second long line", "Last line."});
        REQUIRE(TRAP::Utils::String::GetLines(std::string(d)) == std::vector<std::string>{"First line...", "Second long line", "Last line."});
        REQUIRE(TRAP::Utils::String::GetLines("") == std::vector<std::string>{});
    }

    SECTION("Contains(string_view, string_view)")
    {
        STATIC_REQUIRE(TRAP::Utils::String::Contains("abc", "a"));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::Contains("abc", "d"));
        STATIC_REQUIRE(TRAP::Utils::String::Contains("abc", "ab"));
        STATIC_REQUIRE(TRAP::Utils::String::Contains("", ""));
        STATIC_REQUIRE(TRAP::Utils::String::Contains("abc", ""));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::Contains("", "a"));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::Contains("", "ab"));
    }

    SECTION("Contains(string_view, char)")
    {
        STATIC_REQUIRE(TRAP::Utils::String::Contains("abc", 'a'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::Contains("abc", 'd'));
        STATIC_REQUIRE(TRAP::Utils::String::Contains("abc", "ab"));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::Contains("", 'a'));
    }

    SECTION("Contains(string_view, const char*)")
    {
        static constexpr std::string_view a = "a";
        static constexpr std::string_view d = "d";
        static constexpr std::string_view ab = "ab";

        STATIC_REQUIRE(TRAP::Utils::String::Contains("abc", a.data()));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::Contains("abc", d.data()));
        STATIC_REQUIRE(TRAP::Utils::String::Contains("abc", ab.data()));
        STATIC_REQUIRE(TRAP::Utils::String::Contains("", ""));
        STATIC_REQUIRE(TRAP::Utils::String::Contains("abc", ""));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::Contains("", a.data()));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::Contains("", ab.data()));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::Contains("", nullptr));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::Contains("abc", nullptr));
    }

    SECTION("GetSuffixStringView()")
    {
        STATIC_REQUIRE(TRAP::Utils::String::GetSuffixStringView("").empty());
        STATIC_REQUIRE(TRAP::Utils::String::GetSuffixStringView("abc").empty());
        STATIC_REQUIRE(TRAP::Utils::String::GetSuffixStringView("abc.de") == "de");
        STATIC_REQUIRE(TRAP::Utils::String::GetSuffixStringView("abc.").empty());
        STATIC_REQUIRE(TRAP::Utils::String::GetSuffixStringView("abc.def.gh") == "gh");
        STATIC_REQUIRE(TRAP::Utils::String::GetSuffixStringView("abc.def.").empty());
    }

    SECTION("GetSuffix()")
    {
        STATIC_REQUIRE(TRAP::Utils::String::GetSuffix("").empty());
        STATIC_REQUIRE(TRAP::Utils::String::GetSuffix("abc").empty());
        STATIC_REQUIRE(TRAP::Utils::String::GetSuffix("abc.de") == "de");
        STATIC_REQUIRE(TRAP::Utils::String::GetSuffix("abc.").empty());
        STATIC_REQUIRE(TRAP::Utils::String::GetSuffix("abc.def.gh") == "gh");
        STATIC_REQUIRE(TRAP::Utils::String::GetSuffix("abc.def.").empty());
    }

    SECTION("ToLower(string)")
    {
        STATIC_REQUIRE(TRAP::Utils::String::ToLower("").empty());
        STATIC_REQUIRE(TRAP::Utils::String::ToLower("Hello World!") == "hello world!");
        STATIC_REQUIRE(TRAP::Utils::String::ToLower("0123456789") == "0123456789");
    }

    SECTION("ToLower(char)")
    {
        STATIC_REQUIRE(TRAP::Utils::String::ToLower('a') == 'a');
        STATIC_REQUIRE(TRAP::Utils::String::ToLower('A') == 'a');
        STATIC_REQUIRE(TRAP::Utils::String::ToLower('1') == '1');
    }

    SECTION("ToUpper(string)")
    {
        STATIC_REQUIRE(TRAP::Utils::String::ToUpper("").empty());
        STATIC_REQUIRE(TRAP::Utils::String::ToUpper("Hello World!") == "HELLO WORLD!");
        STATIC_REQUIRE(TRAP::Utils::String::ToUpper("0123456789") == "0123456789");
    }

    SECTION("ToUpper(char)")
    {
        STATIC_REQUIRE(TRAP::Utils::String::ToUpper('a') == 'A');
        STATIC_REQUIRE(TRAP::Utils::String::ToUpper('A') == 'A');
        STATIC_REQUIRE(TRAP::Utils::String::ToUpper('1') == '1');
    }

    SECTION("IsDigit()")
    {
        STATIC_REQUIRE(TRAP::Utils::String::IsDigit('0'));
        STATIC_REQUIRE(TRAP::Utils::String::IsDigit('1'));
        STATIC_REQUIRE(TRAP::Utils::String::IsDigit('2'));
        STATIC_REQUIRE(TRAP::Utils::String::IsDigit('3'));
        STATIC_REQUIRE(TRAP::Utils::String::IsDigit('4'));
        STATIC_REQUIRE(TRAP::Utils::String::IsDigit('5'));
        STATIC_REQUIRE(TRAP::Utils::String::IsDigit('6'));
        STATIC_REQUIRE(TRAP::Utils::String::IsDigit('7'));
        STATIC_REQUIRE(TRAP::Utils::String::IsDigit('8'));
        STATIC_REQUIRE(TRAP::Utils::String::IsDigit('9'));
        STATIC_REQUIRE(TRAP::Utils::String::IsDigit('0'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsDigit('a'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsDigit('!'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsDigit('Z'));
    }

    SECTION("IsHexDigit()")
    {
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('0'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('1'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('2'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('3'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('4'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('5'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('6'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('7'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('8'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('9'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('0'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('a'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('b'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('c'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('d'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('e'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('f'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('A'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('B'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('C'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('D'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('E'));
        STATIC_REQUIRE(TRAP::Utils::String::IsHexDigit('F'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsHexDigit('g'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsHexDigit('G'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsHexDigit('!'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsHexDigit('Z'));
    }

    SECTION("IsSpace()")
    {
        STATIC_REQUIRE(TRAP::Utils::String::IsSpace(' '));
        STATIC_REQUIRE(TRAP::Utils::String::IsSpace('\f'));
        STATIC_REQUIRE(TRAP::Utils::String::IsSpace('\n'));
        STATIC_REQUIRE(TRAP::Utils::String::IsSpace('\r'));
        STATIC_REQUIRE(TRAP::Utils::String::IsSpace('\t'));
        STATIC_REQUIRE(TRAP::Utils::String::IsSpace('\v'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsSpace('a'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsSpace('1'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsSpace('!'));
    }

    SECTION("IsAlpha()")
    {
        STATIC_REQUIRE(TRAP::Utils::String::IsAlpha('a'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlpha('A'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlpha('b'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlpha('B'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlpha('c'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlpha('C'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlpha('x'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlpha('X'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlpha('y'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlpha('Y'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlpha('z'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlpha('Z'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsAlpha('1'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsAlpha('2'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsAlpha('3'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsAlpha('4'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsAlpha('5'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsAlpha('6'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsAlpha('7'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsAlpha('8'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsAlpha('9'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsAlpha('0'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsAlpha('!'));
    }

    SECTION("IsAlphaNumeric()")
    {
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('a'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('A'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('b'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('B'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('c'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('C'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('x'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('X'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('y'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('Y'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('z'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('Z'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('1'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('2'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('3'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('4'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('5'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('6'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('7'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('8'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('9'));
        STATIC_REQUIRE(TRAP::Utils::String::IsAlphaNumeric('0'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsAlphaNumeric('!'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsAlphaNumeric('#'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsAlphaNumeric('?'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsAlphaNumeric('-'));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::IsAlphaNumeric('+'));
    }

    SECTION("GetCount()")
    {
        STATIC_REQUIRE(TRAP::Utils::String::GetCount("", 'a') == 0);
        STATIC_REQUIRE(TRAP::Utils::String::GetCount("abc", 'a') == 1);
        STATIC_REQUIRE(TRAP::Utils::String::GetCount("Hello World!", 'l') == 3);
    }

    SECTION("CompareAnyCase()")
    {
        STATIC_REQUIRE(TRAP::Utils::String::CompareAnyCase("", ""));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::CompareAnyCase("abc", ""));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::CompareAnyCase("", "abc"));
        STATIC_REQUIRE(TRAP::Utils::String::CompareAnyCase("abc", "abc"));
        STATIC_REQUIRE(TRAP::Utils::String::CompareAnyCase("ABC", "ABC"));
        STATIC_REQUIRE(TRAP::Utils::String::CompareAnyCase("ABC", "abc"));
        STATIC_REQUIRE(TRAP::Utils::String::CompareAnyCase("abc", "ABC"));
        STATIC_REQUIRE(TRAP::Utils::String::CompareAnyCase("abc123", "ABC123"));
        STATIC_REQUIRE(TRAP::Utils::String::CompareAnyCase("ABC123", "abc123"));
    }

    SECTION("EncodeUTF8()")
    {
        //ASCII
        STATIC_REQUIRE(TRAP::Utils::String::EncodeUTF8('a') == "a");
        STATIC_REQUIRE(TRAP::Utils::String::EncodeUTF8('A') == "A");
        STATIC_REQUIRE(TRAP::Utils::String::EncodeUTF8('1') == "1");
        STATIC_REQUIRE(TRAP::Utils::String::EncodeUTF8('$') == "$");

        //2-Byte UTF-8
        STATIC_REQUIRE(TRAP::Utils::String::EncodeUTF8(0x000000A9) == "©");
        //3-Byte UTF-8
        STATIC_REQUIRE(TRAP::Utils::String::EncodeUTF8(0x000020AC) == "€");
        //4-Byte UTF-8
        STATIC_REQUIRE(TRAP::Utils::String::EncodeUTF8(0x0001F340) == "🍀");
    }

    SECTION("GetTimeStamp()")
    {
        REQUIRE_FALSE(TRAP::Utils::String::GetTimeStamp(std::chrono::system_clock::now()).empty());
    }

    SECTION("GetDateTimeStamp()")
    {
        REQUIRE_FALSE(TRAP::Utils::String::GetDateTimeStamp(std::chrono::system_clock::now()).empty());
    }

#ifdef TRAP_PLATFORM_WINDOWS
    SECTION("CreateUTF8StringFromWideStringWin32()")
    {
        REQUIRE(TRAP::Utils::String::CreateUTF8StringFromWideStringWin32(L"Hello World!") == "Hello World!");
        REQUIRE(TRAP::Utils::String::CreateUTF8StringFromWideStringWin32(L"Hello World! ™©") == "Hello World! ™©");
    }
#endif /*TRAP_PLATFORM_WINDOWS*/

#ifdef TRAP_PLATFORM_WINDOWS
    SECTION("CreateWideStringFromUTF8StringWin32()")
    {
        REQUIRE(TRAP::Utils::String::CreateWideStringFromUTF8StringWin32("Hello World!") == L"Hello World!");
        REQUIRE(TRAP::Utils::String::CreateWideStringFromUTF8StringWin32("Hello World! ™©") == L"Hello World! ™©");
    }
#endif /*TRAP_PLATFORM_WINDOWS*/

    SECTION("ConvertToType<char>()")
    {
        STATIC_REQUIRE(TRAP::Utils::String::ConvertToType<char>("") == '\0');
        STATIC_REQUIRE(TRAP::Utils::String::ConvertToType<char>("a") == 'a');
        STATIC_REQUIRE(TRAP::Utils::String::ConvertToType<char>("ab") == 'a');
    }

    SECTION("ConvertToType<bool>()")
    {
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::ConvertToType<bool>(""));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::ConvertToType<bool>("a"));
        STATIC_REQUIRE(TRAP::Utils::String::ConvertToType<bool>("true"));
        STATIC_REQUIRE(TRAP::Utils::String::ConvertToType<bool>("TRUE"));
        STATIC_REQUIRE(TRAP::Utils::String::ConvertToType<bool>("tRuE"));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::ConvertToType<bool>("false"));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::ConvertToType<bool>("FALSE"));
        STATIC_REQUIRE_FALSE(TRAP::Utils::String::ConvertToType<bool>("fAlSe"));
    }

    SECTION("ConvertToType<std::string>()")
    {
        STATIC_REQUIRE(TRAP::Utils::String::ConvertToType<std::string>("") == "");
        STATIC_REQUIRE(TRAP::Utils::String::ConvertToType<std::string>("Hello World!") == "Hello World!");
    }

    SECTION("ConvertToType<u8>()")
    {
        REQUIRE(TRAP::Utils::String::ConvertToType<u8>("") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<u8>("Hello World!") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<u8>("0") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<u8>("120") == 120);
        REQUIRE(TRAP::Utils::String::ConvertToType<u8>("1024") == 0);
    }

    SECTION("ConvertToType<i8>()")
    {
        REQUIRE(TRAP::Utils::String::ConvertToType<i8>("") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<i8>("Hello World!") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<i8>("0") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<i8>("120") == 120);
        REQUIRE(TRAP::Utils::String::ConvertToType<i8>("-120") == -120);
        REQUIRE(TRAP::Utils::String::ConvertToType<i8>("1024") == 0);
    }

    SECTION("ConvertToType<u16>()")
    {
        REQUIRE(TRAP::Utils::String::ConvertToType<u16>("") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<u16>("Hello World!") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<u16>("0") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<u16>("120") == 120);
        REQUIRE(TRAP::Utils::String::ConvertToType<u16>("1024") == 1024);
    }

    SECTION("ConvertToType<i16>()")
    {
        REQUIRE(TRAP::Utils::String::ConvertToType<i16>("") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<i16>("Hello World!") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<i16>("0") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<i16>("120") == 120);
        REQUIRE(TRAP::Utils::String::ConvertToType<i16>("-120") == -120);
        REQUIRE(TRAP::Utils::String::ConvertToType<i16>("1024") == 1024);
        REQUIRE(TRAP::Utils::String::ConvertToType<i16>("-1024") == -1024);
    }

    SECTION("ConvertToType<u32>()")
    {
        REQUIRE(TRAP::Utils::String::ConvertToType<u32>("") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<u32>("Hello World!") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<u32>("0") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<u32>("120") == 120);
        REQUIRE(TRAP::Utils::String::ConvertToType<u32>("70000") == 70000);
    }

    SECTION("ConvertToType<i32>()")
    {
        REQUIRE(TRAP::Utils::String::ConvertToType<i32>("") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<i32>("Hello World!") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<i32>("0") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<i32>("120") == 120);
        REQUIRE(TRAP::Utils::String::ConvertToType<i32>("-120") == -120);
        REQUIRE(TRAP::Utils::String::ConvertToType<i32>("70000") == 70000);
        REQUIRE(TRAP::Utils::String::ConvertToType<i32>("-70000") == -70000);
    }

    SECTION("ConvertToType<u64>()")
    {
        REQUIRE(TRAP::Utils::String::ConvertToType<u64>("") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<u64>("Hello World!") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<u64>("0") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<u64>("120") == 120);
        REQUIRE(TRAP::Utils::String::ConvertToType<u64>("70000") == 70000);
    }

    SECTION("ConvertToType<i64>()")
    {
        REQUIRE(TRAP::Utils::String::ConvertToType<i64>("") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<i64>("Hello World!") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<i64>("0") == 0);
        REQUIRE(TRAP::Utils::String::ConvertToType<i64>("120") == 120);
        REQUIRE(TRAP::Utils::String::ConvertToType<i64>("-120") == -120);
        REQUIRE(TRAP::Utils::String::ConvertToType<i64>("70000") == 70000);
        REQUIRE(TRAP::Utils::String::ConvertToType<i64>("-70000") == -70000);
    }

    SECTION("ConvertToType<f32>()")
    {
        REQUIRE(TRAP::Utils::String::ConvertToType<f32>("") == 0.0f);
        REQUIRE(TRAP::Utils::String::ConvertToType<f32>("Hello World!") == 0.0f);
        REQUIRE(TRAP::Utils::String::ConvertToType<f32>("0") == 0.0f);
        REQUIRE(TRAP::Utils::String::ConvertToType<f32>("120.5") == 120.5f);
        REQUIRE(TRAP::Utils::String::ConvertToType<f32>("-120.5") == -120.5f);
        REQUIRE(TRAP::Utils::String::ConvertToType<f32>("70000") == 70000.0f);
        REQUIRE(TRAP::Utils::String::ConvertToType<f32>("-70000") == -70000.0f);
    }

    SECTION("ConvertToType<f32>()")
    {
        REQUIRE(TRAP::Utils::String::ConvertToType<f64>("") == 0.0);
        REQUIRE(TRAP::Utils::String::ConvertToType<f64>("Hello World!") == 0.0);
        REQUIRE(TRAP::Utils::String::ConvertToType<f64>("0") == 0.0);
        REQUIRE(TRAP::Utils::String::ConvertToType<f64>("120.5") == 120.5);
        REQUIRE(TRAP::Utils::String::ConvertToType<f64>("-120.5") == -120.5);
        REQUIRE(TRAP::Utils::String::ConvertToType<f64>("70000") == 70000.0);
        REQUIRE(TRAP::Utils::String::ConvertToType<f64>("-70000") == -70000.0);
    }
}
