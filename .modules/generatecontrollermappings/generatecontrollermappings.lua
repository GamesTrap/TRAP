--For license see Licenses/SDL_GameControllerDB.LICENSE

premake.modules.generatecontrollermappings = {}
local m = premake.modules.generatecontrollermappings

local p = premake
local success = true

function DownloadProgress(total, current)
    local ratio = current / total;
    ratio = math.min(math.max(ratio, 0), 1);
    local percent = math.floor(ratio * 100);

    local downloadedStr = ("%d/%d"):format(current, total)
    local progressBar = ("[" .. ("="):rep(math.floor(percent / 2)) .. (" "):rep(50 - math.floor(percent / 2)) .. "]")

    io.write(("\rProgress: %s %s %.2f%%"):format(progressBar, downloadedStr, percent))
    io.flush()
end

newaction
{
    trigger = "genconmappings",
    description = "Generate Controller Mappings from SDL upstream",

    execute = function()
        print("Generating Controller Mappings...")

        -- Variables & Paths
        local sourceURL = "https://raw.githubusercontent.com/gabomdq/SDL_GameControllerDB/master/gamecontrollerdb.txt"
        local sourcePath = "../.modules/generatecontrollermappings/gamecontrollerdb.txt"
        local templatePath = "../.modules/generatecontrollermappings/TControllerMappings.h"
        local targetPath = "../TRAP/src/Input/ControllerMappings.h"

        -- Check if template File can be opened
        local templateFile = io.open(templatePath, "r")
        if templateFile ~= nil then
            templateFile:close()
        else
            success = false
            term.setTextColor(term.errorColor)
            print("Failed to find template file " .. templatePath)
            term.setTextColor(nil)
            return
        end

        -- Download mappings from SDL
        http.download(sourceURL, sourcePath, {progress = DownloadProgress})
        print()

        -- Check if mapping File can be opened
        local sourceFile = io.open(sourcePath, "r")
        if sourceFile ~= nil then
            sourceFile:close()
        else
            success = false
            term.setTextColor(term.errorColor)
            print("Failed to find source file " .. sourcePath)
            term.setTextColor(nil)
            return
        end

        -- Copy everything from template
        infile = io.open(templatePath, "r")
        local instr = infile:read("*a")
        infile:close()

        -- Loop for each line in mapping File
        for line in io.lines(sourcePath) do
            local ioscheck = string.contains(line, "platform:iOS")
            local androidcheck = string.contains(line, "platform:Android")
            local macosxcheck = string.contains(line, "platform:Mac OS X")
            local matchesRegex = string.match(line, "^[0-9a-fA-F].*$");

            if(not(ioscheck) and not(androidcheck) and not(macosxcheck) and matchesRegex) then
                -- instr = instr:gsub("@TRAP_CONTROLLER_MAPPINGS@", "\t\t\"" .. line .. "\",\n@TRAP_CONTROLLER_MAPPINGS@")
                if (string.contains(line, "platform:Windows")) then
                    instr = instr:gsub("@TRAP_WIN32_MAPPINGS@", "\t\t\"" .. line .. "\",\n@TRAP_WIN32_MAPPINGS@")
                elseif (string.contains(line, "platform:Linux")) then
                    instr = instr:gsub("@TRAP_LINUX_MAPPINGS@", "\t\t\"" .. line .. "\",\n@TRAP_LINUX_MAPPINGS@")
                end
            end
        end

        -- Remove SDL mapping file
        os.remove(sourcePath)

        -- Remove last @TRAP_WIN32_MAPPINGS@ & @TRAP_LINUX_MAPPINGS@
        instr = instr:gsub("@TRAP_WIN32_MAPPINGS@", "")
        instr = instr:gsub("@TRAP_LINUX_MAPPINGS@", "")

        -- Set timestamp
        -- local timestamp = os.date("%Y-%m-%d");
        -- instr = instr:gsub("@MODIFICATION_DATE@", timestamp)

        -- Write final mapping file
        outfile = io.open(targetPath, "w")
        outfile:write(instr)
        outfile:close()
    end,

    onEnd = function()
        if(success) then
            term.setTextColor(term.infoColor)
            print("Controller Mappings generation finished successfully")
            term.setTextColor(nil)
        else
            term.setTextColor(term.errorColor)
            print("Controller Mappings generation failed!")
            term.setTextColor(nil)
        end
    end
}

return m