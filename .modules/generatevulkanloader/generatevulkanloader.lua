premake.modules.generatevulkanloader = {}
local m = premake.modules.generatevulkanloader

local p = premake

success = true

function CheckWindowsStuff()
    print("Checking Python")
    if(os.outputof("python --version > NUL") == nil) then
        print("Unable to find Python 3")
        return false
    end

    print("Checking pip")
    if(os.outputof("pip --version > NUL") == nil) then
        print("Unable to find pip")
        return false
    end

    return true
end

newaction
{
    trigger = "generatevulkanloader",
    description = "Generate Vulkan Loader",

    execute = function()
        print("Checking Dependencies")
        local res = true
        if(os.host() == "linux") then
            print("Checking Python")
            local out, errorCode = os.outputof("python3 --version")
            if(errorCode ~= 0) then
                print("Unable to find Python 3")
                res = false
            end

            print("Checking pip")
            local out, errorCode = os.outputof("pip --version")
            if(errorCode ~= 0) then
                print("Unable to find pip")
                res = false
            end
        elseif(os.host() == "windows") then
            print("Checking Python")
            local out, errorCode = os.outputof("python --version")
            if(errorCode ~= 0) then
                print("Unable to find Python 3")
                res = false
            end

            print("Checking pip")
            local out, errorCode = os.outputof("pip --version")
            if(errorCode ~= 0) then
                print("Unable to find pip")
                res = false
            end
        else
            print("Unsupported OS: " .. os.host())
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
        if(os.host() == "linux") then
            os.execute("python3 ../.modules/generatevulkanloader/generate.py")
        elseif(os.host() == "windows") then
            os.execute("python ../.modules/generatevulkanloader/generate.py")
        end

        if(os.host() == "linux") then
            os.execute("mv ../.modules/generatevulkanloader/VulkanLoader.h ../TRAP/src/Graphics/API/Vulkan/Utils/VulkanLoader.h")
        elseif(os.host() == "windows") then
            os.execute("move ..\\.modules\\generatevulkanloader\\VulkanLoader.h ..\\TRAP\\src\\Graphics\\API\\Vulkan\\Utils\\VulkanLoader.h > NUL")
        end
    end,

    onEnd = function()
        if(success) then
            print("Vulkan loader generation successful")
        else
            print("Vulkan loader generation failed")
        end
    end
}

return m