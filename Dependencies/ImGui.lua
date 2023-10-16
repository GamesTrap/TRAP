project "ImGui"
    kind "StaticLib"
    language "C++"
    warnings "off"

    files
    {
        "ImGui/imconfig.h",
        "ImGui/imgui.h",
        "ImGui/imgui.cpp",
        "ImGui/imgui_draw.cpp",
        "ImGui/imgui_internal.h",
        "ImGui/imgui_widgets.cpp",
        "ImGui/imgui_tables.cpp",
        "ImGui/imstb_rectpack.h",
        "ImGui/imstb_textedit.h",
        "ImGui/imstb_truetype.h",
        "ImGui/imgui_demo.cpp"
    }
