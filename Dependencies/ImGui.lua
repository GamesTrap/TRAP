project "ImGui"
    kind "StaticLib"
    language "C++"
    warnings "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

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

    defines
    {
        "IMGUI_DISABLE_OBSOLETE_FUNCTIONS",
		"IMGUI_DISABLE_OBSOLETE_KEYIO",
    }
