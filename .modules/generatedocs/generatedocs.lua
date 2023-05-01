premake.modules.generatedocs = {}
local m = premake.modules.generatedocs

local p = premake

success = true
doxygenPath = "Dependencies/Docs/Doxygen"
doxygenBinPath = doxygenPath .. "/bin/"
tempSphinxEmbedPath = "../.modules/generatedocs/sphinx/embed"

function GenerateChangelog()
    print("Generating Changelog...")

    -- Check if SITREPS.txt exists
    if not os.isfile("../SITREPS.txt") then
        term.setTextColor(term.errorColor)
        print("Unable to find SITREPS.txt in root folder!")
        term.setTextColor(nil)
        return false
    end

    -- Delete existing changelog.md if it exists
    if os.isfile("../.modules/generatedocs/pages/changelog.md") then
        os.remove("../.modules/generatedocs/pages/changelog.md")
    end

    -- Open SITREPS.txt
    local sitrep = io.readfile("../SITREPS.txt")
    -- Explode SITREPS.txt into lines
    local sitrepLines = string.explode(sitrep, "\n")

    local changelog = "(changelog)=\n\n# Changelog\n\nGenerated on " .. os.date("%m/%d/%Y") .. "\n\n"

    -- Reverse loop through each line in sitrepLines (so we get newest -> oldest in the end)
    local changes = {}
    local currentYear = 0
    for i = #sitrepLines, 1, -1 do
        if sitrepLines[i] == "" then -- If line is blank, reset changes
            changes = {}
        elseif string.find(sitrepLines[i], "SITREP", 1, true) then -- If line contains 'SITREP'
            -- Reverse changes table (so it has the right order)
            for j = 1, math.floor(#changes / 2) do
                local tmp = changes[j]
                changes[j] = changes[#changes - j + 1]
                changes[#changes - j + 1] = tmp
            end

            -- Create title for changelog entry ("TRAP™ Engine x.x.x - MM/DD/YYYY|YYwWWXX" or "TRAP™ Engine MM/DD/YYYY|YYwWWXX")
            local title = "TRAP™ Engine "
            if string.find(sitrepLines[i + 1], "Changed TRAP Engine version to", 1, true) then
                -- Get version number
                local version = string.sub(sitrepLines[i + 1], string.find(sitrepLines[i + 1], "to", 1, true) + 3)
                -- Remove everything after we hit ')'
                version = string.sub(version, 1, string.find(version, ")", 1, true) - 1)
                -- Remove everything that comes before '('
                version = string.sub(version, string.find(version, "(", 1, true) + 1)

                title = title .. version .. " - "
            end
            -- Add date and calendar week
            title = title .. string.sub(sitrepLines[i], 8)

            -- Categorize SITREPs by year
            if string.find(sitrepLines[i], "|") and string.find(sitrepLines[i], "/") then
                local newYear = string.sub(sitrepLines[i], 14, 14 + 3)
                if newYear ~= currentYear and currentYear == 0 then -- First year
                    currentYear = newYear
                    changelog = changelog .. "::::{dropdown} " .. currentYear .. "\n:animate: fade-in-slide-down\n\n"
                elseif newYear ~= currentYear then -- New year (not first year change)
                    currentYear = newYear
                    --End current year
                    changelog = changelog .. "::::\n\n"
                    -- Write new year
                    changelog = changelog .. "::::{dropdown} " .. currentYear .. "\n:animate: fade-in-slide-down\n\n"
                end
            end

            -- Add title for this version
            changelog = changelog .. ":::{dropdown} " .. title .. "\n:animate: fade-in\n\n"

            -- Add all changes to the version
            for j, change in ipairs(changes) do
                    changelog = changelog .. change .. "\n"
            end

            if i == #sitrepLines then -- If this is the last line, add a newline
                changelog = changelog .. "\n:::\n"
            else
                changelog = changelog .. "\n:::\n\n"
            end
        elseif string.find(sitrepLines[i], " -", 1, true) then -- If line contains ' -'
            if not string.find(sitrepLines[i], "Changed TRAP Engine version to", 1, true) then
                local temp = sitrepLines[i]

                -- Remove everything after last "~" if exists
                if string.find(temp, " ~") then
                    temp = string.sub(temp, 1, string.find(temp, " ~") - 1)
                end

                -- Escape HTML chars (specifically '<' and '>')
                temp = string.gsub(temp, "<", "&lt;")
                temp = string.gsub(temp, ">", "&gt;")

                -- Convert indentation (4 spaces -> 2 spaces)

                local spaces = 0
                for j = 1, #temp do
                    -- If char is a space, remove it
                    if temp:sub(j, j) == " " then
                        spaces = spaces + 1
                    elseif temp:sub(j, j) == "\t" then
                        spaces = spaces + 4
                    else
                        break
                    end
                end

                -- Remove spaces from temp
                temp = string.sub(temp, spaces + 1)

                local newIndent = spaces / 4 - 1
                -- Add newIndent count ' ' to temp
                for j = 1, newIndent do
                    temp = "  " .. temp
                end

                changes[#changes + 1] = temp
            end
        else
            print("Error: " .. sitrepLines[i])
        end
    end

    changelog = changelog .. "::::\n"

    -- Write changelog
    io.writefile("../.modules/generatedocs/pages/changelog.md", changelog)

    return true
end

newaction
{
    trigger = "gendocs",
    description = "Generate Documentation",

    execute = function()
        print("Checking Dependencies")
        if(os.host() == "linux") then
            print("Checking Doxygen")
            local out, errorCode = os.outputof("doxygen --version")
            if(errorCode ~= 0) then
                term.setTextColor(term.errorColor)
                print("Unable to find Doxygen")
                term.setTextColor(nil)
                success = false
                return

            end

            print("Checking Python")
            local out, errorCode = os.outputof("python --version")
            if(errorCode ~= 0) then
                term.setTextColor(term.errorColor)
                print("Unable to find Python 3")
                term.setTextColor(nil)
                success = false
                return
            end

            print("Checking pip")
            local out, errorCode = os.outputof("pip --version")
            if(errorCode ~= 0) then
                term.setTextColor(term.errorColor)
                print("Unable to find pip")
                term.setTextColor(nil)
                success = false
                return
            end

            print("Checking Sphinx")
            local out, errorCode = os.outputof("sphinx-build --version")
            if(errorCode ~= 0) then
                term.setTextColor(term.warningColor)
                print("Unable to find sphinx-build!")
                term.setTextColor(nil)
                success = false
                return
            end

            print("Checking Sphinx-Extensions")
            -- local out, errorCode = os.outputof("pip list | grep -F sphinxcontrib-inlinesyntaxhighlight")
            -- if(errorCode ~= 0) then
            --     term.setTextColor(term.warningColor)
            --     print("Sphinx-InlineSyntaxHighlight is not installed!")
            --     term.setTextColor(nil)
            --     print("Installing Sphinx-InlineSyntaxHighlight...")
            --     os.execute("pip install -U sphinxcontrib-inlinesyntaxhighlight")
            -- end

            local out, errorCode = os.outputof("pip list | grep -F exhale")
            if(errorCode ~= 0) then
                term.setTextColor(term.warningColor)
                print("Exhale is not installed!")
                term.setTextColor(nil)
                print("Installing Exhale...")
                os.execute("pip install -U exhale")
            end

            local out, errorCode = os.outputof("pip list | grep -F breathe")
            if(errorCode ~= 0) then
                term.setTextColor(term.warningColor)
                print("Breathe is not installed!")
                term.setTextColor(nil)
                print("Installing Breathe...")
                os.execute("pip install -U breathe")
            end

            local out, errorCode = os.outputof("pip list | grep -F furo")
            if(errorCode ~= 0) then
                term.setTextColor(term.warningColor)
                print("Furo theme is not installed!")
                term.setTextColor(nil)
                print("Installing Furo theme...")
                os.execute("pip install -U furo")
            end

            local out, errorCode = os.outputof("pip list | grep -F sphinx_inline_tabs")
            local out2, errorCode2 = os.outputof("pip list | grep -F sphinx-inline-tabs")
            if(errorCode ~= 0 and errorCode2 ~= 0) then
                term.setTextColor(term.warningColor)
                print("Sphinx-Inline-Tabs is not installed!")
                term.setTextColor(nil)
                print("Installing Sphinx-Inline-Tabs...")
                os.execute("pip install -U sphinx-inline-tabs")
            end

            local out, errorCode = os.outputof("pip list | grep -F sphinxext-opengraph")
            if(errorCode ~= 0) then
                term.setTextColor(term.warningColor)
                print("SphinxExt-OpenGraph is not installed!")
                term.setTextColor(nil)
                print("Installing SphinxExt-OpenGraph...")
                os.execute("pip install -U sphinxext-opengraph")
            end

            local out, errorCode = os.outputof("pip list | grep -F sphinx-copybutton")
            if(errorCode ~= 0) then
                term.setTextColor(term.warningColor)
                print("Sphinx-CopyButton is not installed!")
                term.setTextColor(nil)
                print("Installing Sphinx-CopyButton...")
                os.execute("pip install -U sphinx-copybutton")
            end

            local out, errorCode = os.outputof("pip list | grep -F myst-parser")
            if(errorCode ~= 0) then
                term.setTextColor(term.warningColor)
                print("MyST-Parser is not installed!")
                term.setTextColor(nil)
                print("Installing MyST-Parser...")
                os.execute("pip install -U myst-parser")
            end

            local out, errorCode = os.outputof("pip list | grep -F Pygments")
            if(errorCode ~= 0) then
                term.setTextColor(term.warningColor)
                print("Pygments is not installed!")
                term.setTextColor(nil)
                print("Installing Pygments...")
                os.execute("pip install -U Pygments")
            end

            local out, errorCode = os.outputof("pip list | grep -F sphinx-design")
            local out2, errorCode2 = os.outputof("pip list | grep -F sphinx_design")
            if(errorCode ~= 0 and errorCode2 ~= 0) then
                term.setTextColor(term.warningColor)
                print("Sphinx-Design is not installed!")
                term.setTextColor(nil)
                print("Installing Sphinx-Design...")
                os.execute("pip install -U sphinx-design")
            end

        elseif(os.host() == "windows") then
            print("Checking Doxygen")
            local out, errorCode = os.outputof("doxygen --version")
            if(errorCode ~= 0) then
                term.setTextColor(term.errorColor)
                print("Unable to find Doxygen")
                term.setTextColor(nil)
                success = false
                return
            end

            print("Checking Python")
            local out, errorCode = os.outputof("python --version")
            if(errorCode ~= 0) then
                term.setTextColor(term.errorColor)
                print("Unable to find Python")
                term.setTextColor(nil)
                success = false
                return
            end

            print("Checking pip")
            local out, errorCode = os.outputof("pip --version")
            if(errorCode ~= 0) then
                term.setTextColor(term.errorColor)
                print("Unable to find pip")
                term.setTextColor(nil)
                success = false
                return
            end

            print("Checking Sphinx")
            local out, errorCode = os.outputof("sphinx-build --version")
            if(errorCode ~= 0) then
                term.setTextColor(term.warningColor)
                print("Unable to find sphinx-build!")
                term.setTextColor(nil)
                success = false
                return
            end

            print("Checking Sphinx-Extensions")
            -- local out, errorCode = os.outputof("pip list | findstr sphinxcontrib-inlinesyntaxhighlight")
            -- if(errorCode ~= 0) then
            --     term.setTextColor(term.warningColor)
            --     print("Sphinx-InlineSyntaxHighlight is not installed!")
            --     term.setTextColor(nil)
            --     print("Installing Sphinx-InlineSyntaxHighlight...")
            --     os.execute("pip install -U sphinxcontrib-inlinesyntaxhighlight")
            -- end

            local out, errorCode = os.outputof("pip list | findstr exhale")
            if(errorCode ~= 0) then
                term.setTextColor(term.warningColor)
                print("Exhale is not installed!")
                term.setTextColor(nil)
                print("Installing Exhale...")
                os.execute("pip install -U exhale")
            end

            local out, errorCode = os.outputof("pip list | findstr breathe")
            if(errorCode ~= 0) then
                term.setTextColor(term.warningColor)
                print("Breathe is not installed!")
                term.setTextColor(nil)
                print("Installing Breathe...")
                os.execute("pip install -U breathe")
            end

            local out, errorCode = os.outputof("pip list | findstr furo")
            if(errorCode ~= 0) then
                term.setTextColor(term.warningColor)
                print("Furo theme is not installed!")
                term.setTextColor(nil)
                print("Installing Furo theme...")
                os.execute("pip install -U furo")
            end

            local out, errorCode = os.outputof("pip list | findstr sphinx_inline_tabs")
            local out2, errorCode2 = os.outputof("pip list | findstr sphinx-inline-tabs")
            if(errorCode ~= 0 and errorCode2 ~= 0) then
                term.setTextColor(term.warningColor)
                print("Sphinx-Inline-Tabs is not installed!")
                term.setTextColor(nil)
                print("Installing Sphinx-Inline-Tabs...")
                os.execute("pip install -U sphinx-inline-tabs")
            end

            local out, errorCode = os.outputof("pip list | findstr sphinxext-opengraph")
            if(errorCode ~= 0) then
                term.setTextColor(term.warningColor)
                print("SphinxExt-OpenGraph is not installed!")
                term.setTextColor(nil)
                print("Installing SphinxExt-OpenGraph...")
                os.execute("pip install -U sphinxext-opengraph")
            end

            local out, errorCode = os.outputof("pip list | findstr sphinx-copybutton")
            if(errorCode ~= 0) then
                term.setTextColor(term.warningColor)
                print("Sphinx-CopyButton is not installed!")
                term.setTextColor(nil)
                print("Installing Sphinx-CopyButton...")
                os.execute("pip install -U sphinx-copybutton")
            end

            local out, errorCode = os.outputof("pip list | findstr myst-parser")
            if(errorCode ~= 0) then
                term.setTextColor(term.warningColor)
                print("MyST-Parser is not installed!")
                term.setTextColor(nil)
                print("Installing MyST-Parser...")
                os.execute("pip install -U myst-parser")
            end

            local out, errorCode = os.outputof("pip list | findstr Pygments")
            if(errorCode ~= 0) then
                term.setTextColor(term.warningColor)
                print("Pygments is not installed!")
                term.setTextColor(nil)
                print("Installing Pygments...")
                os.execute("pip install -U Pygments")
            end

            local out, errorCode = os.outputof("pip list | findstr sphinx-design")
            local out2, errorCode2 = os.outputof("pip list | findstr sphinx_design")
            if(errorCode ~= 0 and errorCode2 ~= 0) then
                term.setTextColor(term.warningColor)
                print("Sphinx-Design is not installed!")
                term.setTextColor(nil)
                print("Installing Sphinx-Design...")
                os.execute("pip install -U sphinx-design")
            end
        else
            term.setTextColor(term.errorColor)
            print("Unsupported OS: " .. os.host())
            term.setTextColor(nil)
            success = false
            return
        end

        if not GenerateChangelog() then
            success = false
            return
        end

        print("Generating Documentation...")
        print("Generating Doxygen pages...")
        if(not os.isdir(tempSphinxEmbedPath)) then
            os.mkdir(tempSphinxEmbedPath)
        end

        --Generate Doxygen pages
        if(os.host() == "windows") then
            os.execute("doxygen ..\\.modules\\generatedocs\\Doxyfile-prj.cfg")
        elseif(os.host() == "linux") then
            os.execute("doxygen ../.modules/generatedocs/Doxyfile-prj.cfg > /dev/null 2>&1")
        end

        print("Generating Sphinx pages...")
        if(os.host() == "windows") then
            os.execute("sphinx-build ..\\.modules\\generatedocs\\ ..\\.modules\\generatedocs\\sphinx -b html -j 16 -q") --Generate Sphinx pages
        elseif(os.host() == "linux") then
            os.execute("sphinx-build ../.modules/generatedocs/ ../.modules/generatedocs/sphinx -b html -j 16 -q") --Generate Sphinx pages
        end

        print("Copying .nojekyll")
        os.copyfile("../.modules/generatedocs/.nojekyll", "../.modules/generatedocs/sphinx") --Needed for GitHub Pages to display correctly as a static page

        print("Copying fonts")
        if(os.host() == "linux") then
            os.execute("cp -r ../.modules/generatedocs/fonts ../.modules/generatedocs/sphinx/fonts")
        elseif(os.host() == "windows") then
            os.execute("xcopy ..\\.modules\\generatedocs\\fonts ..\\.modules\\generatedocs\\sphinx\\fonts > NUL")
        end

        if(os.host() == "linux") then
            os.rmdir("../docs") --Delete old pages
        elseif(os.host() == "windows") then
            os.rmdir("..\\docs") --Delete old pages
        end

        print("Copying files to docs folder...")
        if(os.host() == "linux") then
            os.mkdir("../docs") --Recreate final docs folder
            os.execute("mv ../.modules/generatedocs/sphinx/** ../docs/")
            os.execute("mv ../.modules/generatedocs/sphinx/.** ../docs/ > /dev/null 2>&1")
        elseif(os.host() == "windows") then
            os.execute("move ..\\.modules\\generatedocs\\sphinx ..\\ > NUL")
            os.execute("rename ..\\sphinx docs > NUL")
        end
    end,

    onEnd = function()
        if(success == true) then
            term.setTextColor(term.infoColor)
            print("Documentation generated successfully")
            term.setTextColor(nil)
        else
            term.setTextColor(term.errorColor)
            print("Documentation generation failed")
            term.setTextColor(nil)
        end
    end
}

return m
