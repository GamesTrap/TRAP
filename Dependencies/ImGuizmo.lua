project "ImGuizmo"
    kind "StaticLib"
    language "C++"
    warnings "off"

    files
    {
        "ImGuizmo/ImGuizmo.cpp",
        "ImGuizmo/ImGuizmo.h"
    }

    includedirs "%{IncludeDir.IMGUI}"
