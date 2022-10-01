VersionMajor = 0
VersionMinor = 0
VersionPatch = 0

function RetrieveVersion()
    local pattern = "^#* +(%d+)%.(%d+)%.(%d+)( +)(%d%d%d%d%-%d%d%-%d%d) *$"
    local changeFile = "GLSLang/CHANGES.md"
    local changeFileContent = io.readfile(changeFile)

    if changeFileContent then
        --For each line in changeFileContent
        for line in changeFileContent:gmatch("[^\r\n]+") do
            --If the line matches the pattern
            if line:match(pattern) then
                --Get the version
                local major, minor, patch, _, date = line:match(pattern)
                VersionMajor = major
                VersionMinor = minor
                VersionPatch = patch
                -- term.setTextColor(term.infoColor)
                -- print("Detected GLSLang version " .. VersionMajor .. "." .. VersionMinor .. "." .. VersionPatch)
                -- term.setTextColor(nil)
                return
            end
        end
    end

    term.setTextColor(term.errorColor)
    print("Unable to retrieve version from Dependencies/GLSLang/CHANGES.md")
    term.setTextColor(nil)
    os.exit(-1)
end

function GenerateBuildInfoH()
    if (VersionMajor == 0 and VersionMinor == 0 and VersionPatch == 0) then
        RetrieveVersion()
    end

    local templateFile = "GLSLang/build_info.h.tmpl"
    local targetFile = "GLSLang/glslang/build_info.h"

    --Does target file already exist
    if os.isfile(targetFile) then
        return
    end

    local ok, err = os.touchfile(templateFile, targetFile)

    if not ok then
        term.setTextColor(term.errorColor)
        print("Unable to copy " .. templateFile .. " to " .. targetFile .. ": " .. err)
        term.setTextColor(nil)
        os.exit(-1)
    end

    --Replace @major@ @minor@ @patch@ and @flavor@
    local content = io.readfile(templateFile)
    content = content:gsub("@major@", VersionMajor)
    content = content:gsub("@minor@", VersionMinor)
    content = content:gsub("@patch@", VersionPatch)
    content = content:gsub("@flavor@", "")
    io.writefile(targetFile, content)
end

project "GLSLang"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    systemversion "latest"
    warnings "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    GenerateBuildInfoH() --Generate build_info.h

    files
    {
        "GLSLang/glslang/GenericCodeGen/*.cpp",
        "GLSLang/glslang/HLSL/*.cpp",
        "GLSLang/glslang/MachineIndependent/*.cpp",
        "GLSLang/glslang/MachineIndependent/preprocessor/*.cpp",

        "GLSLang/OGLCompilersDLL/InitializeDll.cpp",

        "GLSLang/glslang/HLSL/*.h",
        "GLSLang/glslang/Include/*.h",
        "GLSLang/glslang/MachineIndependent/*.h",
        "GLSLang/glslang/MachineIndependent/preprocessor/*.h",

        "GLSLang/OGLCompilersDLL/InitializeDll.h",
        "GLSLang/StandAlone/DirStackFileIncluder.h",
        "GLSLang/glslang/OSDependent/osinclude.h",
        "GLSLang/glslang/Public/ShaderLang.h"
    }

    removefiles
    {
        "GLSLang/glslang/HLSL/pch.h",
        "GLSLang/glslang/MachineIndependent/pch.h"
    }

    includedirs "%{IncludeDir.GLSLANG}"

    defines
    {
        -- "AMD_EXTENSIONS",
        -- "NV_EXTENSIONS",
        "ENABLE_OPT=0",
        "ENABLE_HLSL=0",
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "system:windows"
        files "GLSLang/glslang/OSDependent/Windows/ossource.cpp"

        defines
        {
            "GLSLANG_OSINCLUDE_WIN32=true",
            "GLSLANG_OSINCLUDE_UNIX=false"
        }

    filter "system:linux"
        files "GLSLang/glslang/OSDependent/Unix/ossource.cpp"

        defines
        {
            "GLSLANG_OSINCLUDE_WIN32=false",
            "GLSLANG_OSINCLUDE_UNIX=true"
        }

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

    filter "configurations:Profiling"
        editandcontinue "Off"
        runtime "Release"
        optimize "Full"
        symbols "On"

project "SPIRV"
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
        "GLSLang/SPIRV/*.cpp",

        "GLSLang/SPIRV/GlslangToSpv.h",
        "GLSLang/SPIRV/Logger.h",
        "GLSLang/SPIRV/SPVRemapper.h",
        "GLSLang/SPIRV/SpvBuilder.h",
        "GLSLang/SPIRV/SpvTools.h",
        "GLSLang/SPIRV/bitutils.h",
        "GLSLang/SPIRV/disassemble.h",
        "GLSLang/SPIRV/doc.h",
        "GLSLang/SPIRV/hex_float.h",
        "GLSLang/SPIRV/spvIR.h",

        "GLSLang/SPIRV/GLSL.ext.AMD.h",
        "GLSLang/SPIRV/GLSL.ext.EXT.h",
        "GLSLang/SPIRV/GLSL.ext.KHR.h",
        "GLSLang/SPIRV/GLSL.ext.NV.h",
        "GLSLang/SPIRV/GLSL.ext.450.h",
        "GLSLang/SPIRV/NonSemanticDebugPrintf.h",
        "GLSLang/SPIRV/spirv.hpp"
    }

    removefiles "GLSLang/SPIRV/SpvTools.cpp"

    includedirs
    {
        "%{IncludeDir.GLSLANG}",
        "%{IncludeDir.GLSLANG}/SPIRV",
    }

    links "GLSLang"

    filter "system:windows"
        defines
        {
            "GLSLANG_OSINCLUDE_WIN32=true",
            "GLSLANG_OSINCLUDE_UNIX=false"
        }

    filter "system:linux"
        defines
        {
            "GLSLANG_OSINCLUDE_WIN32=false",
            "GLSLANG_OSINCLUDE_UNIX=true"
        }

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

    filter "configurations:Profiling"
        editandcontinue "Off"
        runtime "Release"
        optimize "Full"
        symbols "On"

project "GLSLang-Default-Resource-Limits"
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
        "GLSLang/StandAlone/ResourceLimits.cpp",
        "GLSLang/StandAlone/ResourceLimits.h"
    }

    links "GLSLang"

    filter "system:windows"
        defines
        {
            "GLSLANG_OSINCLUDE_WIN32=true",
            "GLSLANG_OSINCLUDE_UNIX=false"
        }

    filter "system:linux"
        defines
        {
            "GLSLANG_OSINCLUDE_WIN32=false",
            "GLSLANG_OSINCLUDE_UNIX=true"
        }

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

    filter "configurations:Profiling"
        editandcontinue "Off"
        runtime "Release"
        optimize "Full"
        symbols "On"