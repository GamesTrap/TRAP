premake.modules.generatedocs = {}
local m = premake.modules.generatedocs

local p = premake

mainRepoPath = "../"
generateDocsPath = mainRepoPath .. ".modules/generatedocs/"
tempSphinxEmbedPath = generateDocsPath .. "sphinx/embed"
pythonRequirementsPath = generateDocsPath .. "requirements.txt"
pythonEnvPath = mainRepoPath .. ".penv/"
pipPath = pythonEnvPath .. "bin/pip"
sphinxBuildPath = pythonEnvPath .. "bin/sphinx-build"

function SetupPaths()
    if(os.host() == "windows") then
        mainRepoPath = path.translate(mainRepoPath)
        generateDocsPath = path.translate(generateDocsPath)
        tempSphinxEmbedPath = path.translate(tempSphinxEmbedPath)
        pythonRequirementsPath = path.translate(pythonRequirementsPath)
        pythonEnvPath = path.translate(pythonEnvPath)
        pipPath = path.translate(pythonEnvPath .. "Scripts/./pip.exe")
        sphinxBuildPath = path.translate(pythonEnvPath .. "Scripts/./sphinx-build.exe")
    end
end

function GenerateChangelog()
    print("Generating Changelog...")

    -- Check if SITREPS.txt exists
    if not os.isfile(mainRepoPath .. "SITREPS.txt") then
        term.setTextColor(term.errorColor)
        print("Unable to find SITREPS.txt in root folder!")
        term.setTextColor(nil)
        return false
    end

    -- Delete existing changelog.md if it exists
    if os.isfile(generateDocsPath .. "pages/changelog.md") then
        os.remove(generateDocsPath .. "pages/changelog.md")
    end

    -- Open SITREPS.txt
    local sitrep = io.readfile(mainRepoPath .. "SITREPS.txt")
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
    io.writefile(generateDocsPath .. "pages/changelog.md", changelog)

    return true
end

newaction
{
    trigger = "gendocs",
    description = "Generate Documentation",

    execute = function()
        SetupPaths()

        print("Checking Dependencies")
        if(os.host() == "linux" or os.host() == "windows") then
            print("Checking Doxygen")
            local out, errorCode = os.outputof("doxygen --version")
            if(errorCode ~= 0) then
                term.setTextColor(term.errorColor)
                print("Unable to find Doxygen")
                term.setTextColor(nil)
                return
            end

            print("Checking Python")
            local out, errorCode = os.outputof("python --version")
            if(errorCode ~= 0) then
                term.setTextColor(term.errorColor)
                print("Unable to find Python 3")
                term.setTextColor(nil)
                return
            end

            print("Checking pip")
            local out, errorCode = os.outputof("pip --version")
            if(errorCode ~= 0) then
                term.setTextColor(term.errorColor)
                print("Unable to find pip")
                term.setTextColor(nil)
                return
            end
        else
            term.setTextColor(term.errorColor)
            print("Unsupported OS: " .. os.host())
            term.setTextColor(nil)
            return
        end

        -- Create new python evironment
        result, errorCode = os.outputof("python -m venv " .. pythonEnvPath)
        if(errorCode ~= 0) then
            term.setTextColor(term.errorColor)
            print("Failed to create new python enviroment (" .. pythonEnvPath .. ")")
            term.setTextColor(nil)
            return
        end

        -- Install all python requirements
        result, errorCode = os.outputof(pipPath .. " install -r " .. pythonRequirementsPath)
        if(errorCode ~= 0 and not os.host() == "windows") then --On Windows I get return code 1 instead of expected 0 although all packages were successfully installed :/
            term.setTextColor(term.errorColor)
            print("Failed to install requirements from " .. pythonRequirementsPath)
            term.setTextColor(nil)
            return
        end

        if not GenerateChangelog() then
            return
        end

        print("Generating Documentation...")
        print("Generating Doxygen pages...")
        if(not os.isdir(tempSphinxEmbedPath)) then
            os.mkdir(tempSphinxEmbedPath)
        end

        --Generate Doxygen pages
        os.execute("doxygen " .. generateDocsPath .. "Doxyfile-prj.cfg")

        print("Generating Sphinx pages...")
        os.execute(sphinxBuildPath .. " " .. generateDocsPath .. " " .. generateDocsPath .. "sphinx -b html -j auto -q")

        --Delete .doctrees folder
        os.rmdir(generateDocsPath .. "sphinx/.doctrees")

        print("Copying .nojekyll")
        os.copyfile(generateDocsPath .. ".nojekyll", generateDocsPath .. "sphinx") --Needed for GitHub Pages to display correctly as a static page

        print("Copying fonts")
        if(os.host() == "linux") then
            os.execute("cp -r " .. generateDocsPath .. "fonts " .. generateDocsPath .. "sphinx/fonts")
        elseif(os.host() == "windows") then
            os.execute("xcopy " .. generateDocsPath .. "fonts " .. generateDocsPath .. "sphinx\\fonts /E /I")
        end

        --Delete old pages
        os.rmdir(mainRepoPath .. "docs")

        print("Copying files to docs folder...")
        if(os.host() == "linux") then
            os.mkdir(mainRepoPath .. "docs") --Recreate final docs folder
            os.execute("mv " .. generateDocsPath .. "sphinx/** " .. mainRepoPath .. "docs/")
            os.execute("mv " .. generateDocsPath .. "sphinx/.** " .. mainRepoPath .. "docs/")
        elseif(os.host() == "windows") then
            os.execute("move " .. generateDocsPath .. "sphinx " .. mainRepoPath)
            os.execute("rename " .. mainRepoPath .. "sphinx docs")
        end
    end,
}

return m
