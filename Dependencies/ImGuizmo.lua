project "ImGuizmo"
    kind "StaticLib"
    language "C++"
    warnings "off"

if os.getenv("RUN_CICD_PIPELINE") ~= nil and not table.isempty(os.matchfiles(_MAIN_SCRIPT_DIR .. "/bin/**/*ImGuizmo.*")) then
    -- Using CICD Pipeline cache
else
    files
    {
        "ImGuizmo/ImGuizmo.cpp",
        "ImGuizmo/ImGuizmo.h"
    }
end

    includedirs "%{IncludeDir.IMGUI}"
