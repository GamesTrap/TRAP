project "ImGuizmo"
    kind "StaticLib"
    language "C++"
    warnings "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    files
    {
        "ImGuizmo/ImGuizmo.cpp",
        "ImGuizmo/ImGuizmo.h"
    }

    includedirs "%{IncludeDir.IMGUI}"

    defines "IMGUI_DISABLE_OBSOLETE_FUNCTIONS"
