project "fmt"
    kind "StaticLib"
    language "C++"
    warnings "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    files
    {
        "fmt/src/**.cc",
        "fmt/include/**.h",
    }

    removefiles
    {
        "fmt/src/fmt.cc"
    }

    includedirs
    {
        "fmt/include"
    }

    defines
    {
        "FMT_OS"
    }
