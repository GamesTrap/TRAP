project "fmt"
    kind "StaticLib"
    language "C++"
    warnings "off"

if os.getenv("RUN_CICD_PIPELINE") ~= nil and not table.isempty(os.matchfiles(_MAIN_SCRIPT_DIR .. "/bin/**/*fmt.*")) then
    -- Using CICD Pipeline cache
else
    files
    {
        "fmt/src/**.cc",
        "fmt/include/**.h",
    }

    removefiles
    {
        "fmt/src/fmt.cc" --We dont support C++ modules
    }
end

    includedirs
    {
        "fmt/include"
    }
