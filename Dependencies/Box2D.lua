project "Box2D"
    kind "StaticLib"
    language "C++"
    warnings "off"

if os.getenv("RUN_CICD_PIPELINE") ~= nil and not table.isempty(os.matchfiles(_MAIN_SCRIPT_DIR .. "/bin/**/*Box2D.*")) then
    -- Using CICD Pipeline cache
else
    files
    {
        "Box2D/src/**.h",
        "Box2D/src/**.cpp",
        "Box2D/include/**.h"
    }
end

    includedirs
    {
        "Box2D/include",
        "Box2D/src"
    }
