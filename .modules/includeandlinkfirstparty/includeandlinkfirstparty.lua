premake.modules.includeandlinkfirstparty = {}
local m = premake.modules.includeandlinkfirstparty

local p = premake

local thirdparty = require "includethirdparty"

function LinkTRAPShared()
    filter {}

	links
	{
		"YAMLCpp",
		"ModernDialogs",
		"GLSLang",
		"Box2D",
		"TracyClient",
		"fmt",
		"SPIRV-Cross",
        "libpng",
        "zlib",
        "QuiteOKImage",
	}

    filter "system:linux"
        links
        {
            "dl",
            "pthread",

            "wayland-client",
            "wayland-cursor",
            "xkbcommon"
        }

    filter "system:windows"
        links
        {
            "ws2_32",
            "wsock32",
            "ole32",
            "shell32",
            "gdi32",
            "comdlg32",
            "advapi32"
        }

    -- Nsight Aftermath stuff
    thirdparty.LinkNsightAftermathSDK()
	-- Steamworks SDK stuff
    thirdparty.LinkSteamworksSDK()

    filter {}
end

function IncludeTRAPShared()
    filter {}

    -- Nsight Aftermath stuff
	thirdparty.IncludeNsightAftermathSDK()
	-- Steamworks SDK stuff
	thirdparty.IncludeSteamworksSDK()

    includedirs "%{wks.location}/TRAP/src"

	externalincludedirs
	{
		"%{IncludeDir.GLSLANG}",
		"%{IncludeDir.SPIRV}",
		"%{IncludeDir.SPIRVCROSS}",
		"%{IncludeDir.VULKAN}/include/",
		"%{IncludeDir.ENTT}",
		"%{IncludeDir.YAMLCPP}",
		"%{IncludeDir.MODERNDIALOGS}",
		"%{IncludeDir.VMA}",
		"%{IncludeDir.BOX2D}",
		"%{IncludeDir.TRACY}",
		"%{IncludeDir.FMT}",
		"%{IncludeDir.GCEM}",
		"%{IncludeDir.ZLIB}",
		"%{IncludeDir.LIBPNG}",
		"%{IncludeDir.QUITEOKIMAGE}",
	}

    filter "system:linux"
        if m.AlreadyExecutedWaylandGenerator == nil or m.AlreadyExecutedWaylandGenerator == false then
            local wayland = require "generatewayland"
            wayland.GenerateWayland()
            m.AlreadyExecutedWaylandGenerator = true
        end
        externalincludedirs "%{IncludeDir.WAYLAND}"

    filter {}
end

function m.IncludeTRAP()
    IncludeTRAPShared()

    filter {}

	externalincludedirs
	{
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.IMGUIZMO}"
	}

    -- Discord Game SDK stuff
	thirdparty.IncludeDiscordGameSDK()

	-- NVIDIA Reflex SDK stuff
	thirdparty.IncludeNVIDIAReflexSDK()

    filter {}
end
function m.LinkTRAP()

    filter {}

    links
	{
		"TRAP",

		"ImGui",
		"ImGuizmo"
	}

    LinkTRAPShared()

    -- Discord Game SDK stuff
    thirdparty.LinkDiscordGameSDK()
	-- NVIDIA Reflex SDK stuff
    thirdparty.LinkNVIDIAReflexSDK()

    filter {}
end

function m.IncludeTRAPHeadless()
    IncludeTRAPShared()

    filter {}

    defines "TRAP_HEADLESS_MODE"

    filter{}
end
function m.LinkTRAPHeadless()

    filter {}

	links "TRAP-Headless"

    LinkTRAPShared()

    filter {}

    return true
end

function m.IncludeTRAPUnitTests()
    IncludeTRAPShared()

    filter {}

    externalincludedirs
	{
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.IMGUIZMO}",
        "%{IncludeDir.CATCH2}"
	}
    defines "TRAP_UNITTESTS"

    filter {}
end
function m.LinkTRAPUnitTests()

    filter {}

    links
    {
        "TRAP-UnitTests",
        "Catch2",
		"ImGui",
		"ImGuizmo"
    }

    LinkTRAPShared()

    filter {}
end

return m
