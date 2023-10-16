project "Box2D"
    kind "StaticLib"
    language "C++"
    warnings "off"

    files
    {
        "Box2D/src/**.h",
        "Box2D/src/**.cpp",
        "Box2D/include/**.h"
    }

    includedirs
    {
        "Box2D/include",
        "Box2D/src"
    }
