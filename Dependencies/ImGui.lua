project "ImGui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    systemversion "latest"
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

    defines "IMGUI_DISABLE_OBSOLETE_FUNCTIONS"

    filter "configurations:Debug"
	    runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
        optimize "Full"

	filter "configurations:RelWithDebInfo"
		runtime "Release"
        optimize "Debug"
		symbols "On"