function GenerateCatchUserConfigHPP()
    local targetFile = "Catch2/src/catch2/catch_user_config.hpp"

    --Does target file already exist
    if os.isfile(targetFile) then
        return
    end

    local ok, err = os.touchfile(targetFile)

    if not ok then
        term.setTextColor(term.errorColor)
        print("Unable to create " .. targetFile .. ": " .. err)
        term.setTextColor(nil)
        os.exit(-1)
    end

    local content = [[
#ifndef CATCH_USER_CONFIG_HPP_INCLUDED
#define CATCH_USER_CONFIG_HPP_INCLUDED

//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
/**\file
 * **AUTOGENERATED FROM PREMAKE CONFIGURATION**
 *
 * Contains materialized compile-time configuration provided to Catch2's
 * Premake configuration. All compile-time configuration options need to
 * be here, and also documented in `docs/configuration.md`.
 */

#define DO_NOT_USE_WMAIN

#endif /*CATCH_USER_CONFIG_HPP_INCLUDED*/
]]

    io.writefile(targetFile, content)
end

project "Catch2"
    kind "StaticLib"
    language "C++"
    warnings "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    GenerateCatchUserConfigHPP()

    files
    {
        "Catch2/src/**.h",
        "Catch2/src/**.hpp",
        "Catch2/src/**.c",
        "Catch2/src/**.cpp"
    }

    includedirs
    {
        "Catch2/src"
    }
