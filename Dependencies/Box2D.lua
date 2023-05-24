project "Box2D"
    kind "StaticLib"
    language "C++"
    warnings "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

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
