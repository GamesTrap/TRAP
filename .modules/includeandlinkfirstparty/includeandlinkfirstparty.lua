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
		"SPIRV-Cross"
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
            -- Needed for Networking
            "ws2_32",
            "wsock32"
        }

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
		"%{IncludeDir.GCEM}"
	}

    filter "system:linux"
        local wayland = require "generatewayland"
        wayland.GenerateWayland()
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
	if(thirdparty.IncludeDiscordGameSDK()) then
		dofileopt(path.join(_MAIN_SCRIPT_DIR, "Dependencies/DiscordGameSDK/Compatibility"))
	end
	-- NVIDIA Reflex SDK stuff
	thirdparty.IncludeNVIDIAReflexSDK()

    filter {}
end
function m.LinkTRAP()
    LinkTRAPShared()

    filter {}

    links
	{
		"TRAP",

		"ImGui",
		"ImGuizmo"
	}

    filter {}
end

function m.IncludeTRAPHeadless()
    IncludeTRAPShared()

    filter {}

    defines "TRAP_HEADLESS_MODE"

    filter{}
end
function m.LinkTRAPHeadless()
    LinkTRAPShared()

    filter {}

	links "TRAP-Headless"

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

    -- Discord Game SDK stuff
	if(thirdparty.IncludeDiscordGameSDK()) then
		dofileopt(path.join(_MAIN_SCRIPT_DIR .. "Dependencies/DiscordGameSDK/Compatibility"))
	end
	-- NVIDIA Reflex SDK stuff
	thirdparty.IncludeNVIDIAReflexSDK()

    filter {}
end
function m.LinkTRAPUnitTests()
    LinkTRAPShared()

    filter {}

    links
    {
        "TRAP-UnitTests",
        "Catch2",
		"ImGui",
		"ImGuizmo"
    }

    filter {}
end

return m