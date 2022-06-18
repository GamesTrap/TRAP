premake.modules.generatevulkanloader = {}
local m = premake.modules.generatevulkanloader

local p = premake

success = true

newaction
{
    trigger = "generatevulkanloader",
    description = "Generate Vulkan Loader",

    execute = function()
        print("Checking Dependencies")
        local res = true
        if(os.host() == "linux") then
            print("Checking Python")
            local out, errorCode = os.outputof("python --version")
            if(errorCode ~= 0) then
                term.setTextColor(term.errorColor)
                print("Unable to find Python 3")
                term.setTextColor(nil)
                res = false
            end

            print("Checking pip")
            local out, errorCode = os.outputof("pip --version")
            if(errorCode ~= 0) then
                term.setTextColor(term.errorColor)
                print("Unable to find pip")
                term.setTextColor(nil)
                res = false
            end
        elseif(os.host() == "windows") then
            print("Checking Python")
            local out, errorCode = os.outputof("python --version")
            if(errorCode ~= 0) then
                term.setTextColor(term.errorColor)
                print("Unable to find Python 3")
                term.setTextColor(nil)
                res = false
            end

            print("Checking pip")
            local out, errorCode = os.outputof("pip --version")
            if(errorCode ~= 0) then
                term.setTextColor(term.errorColor)
                print("Unable to find pip")
                term.setTextColor(nil)
                res = false
            end
        else
            term.setTextColor(term.errorColor)
            print("Unsupported OS: " .. os.host())
            term.setTextColor(nil)
            success = false
            return
        end

        if(res == false) then
            success = false
            return
        end

        if(os.host() == "linux") then
            os.execute("cp ../.modules/generatevulkanloader/TVulkanLoader.h ../.modules/generatevulkanloader/VulkanLoader.h")
        elseif(os.host() == "windows") then
            os.execute("copy ..\\.modules\\generatevulkanloader\\TVulkanLoader.h ..\\.modules\\generatevulkanloader\\VulkanLoader.h > NUL")
        end

        print("Generating Vulkan Loader...")
        os.execute("python ../.modules/generatevulkanloader/generate.py")

        if(os.host() == "linux") then
            os.execute("mv ../.modules/generatevulkanloader/VulkanLoader.h ../TRAP/src/Graphics/API/Vulkan/Utils/VulkanLoader.h")
        elseif(os.host() == "windows") then
            os.execute("move ..\\.modules\\generatevulkanloader\\VulkanLoader.h ..\\TRAP\\src\\Graphics\\API\\Vulkan\\Utils\\VulkanLoader.h > NUL")
        end
    end,

    onEnd = function()
        if(success) then
            term.setTextColor(term.infoColor)
            print("Vulkan loader generated successfully")
            term.setTextColor(nil)
        else
            term.setTextColor(term.errorColor)
            print("Vulkan loader generation failed")
            term.setTextColor(nil)
        end
    end
}

return m