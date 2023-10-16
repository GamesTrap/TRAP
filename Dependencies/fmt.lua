project "fmt"
    kind "StaticLib"
    language "C++"
    warnings "off"

    files
    {
        "fmt/src/**.cc",
        "fmt/include/**.h",
    }

    removefiles
    {
        "fmt/src/fmt.cc" --We dont support C++ modules
    }

    includedirs
    {
        "fmt/include"
    }
