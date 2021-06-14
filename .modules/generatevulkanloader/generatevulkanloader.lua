premake.modules.generatevulkanloader = {}
local m = premake.modules.generatevulkanloader

local p = premake

local success = true

function CheckLinuxStuff()
    print("Checking Python")
    if(os.outputof("python3 --version > /dev/null 2>&1") == nil) then
        print("Unable to find Python 3")
        return false
    end

    print("Checking pip")
    if(os.outputof("pip --version > /dev/null 2>&1") == nil) then
        print("Unable to find pip")
        return false
    end

    return true
end

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

function LinuxMoveVulkanLoader()
    os.execute("mv ../.modules/generatevulkanloader/VulkanLoader.h ../TRAP/src/Graphics/API/Vulkan/Utils/VulkanLoader.h")
end

function WindowsMoveVulkanLoader()
    os.execute("move ..\\.modules\\generatevulkanloader\\VulkanLoader.h ..\\TRAP\\src\\Graphics\\API\\Vulkan\\Utils\\VulkanLoader.h > NUL")
end

function LinuxCopyTemplate()
    os.execute("cp ../.modules/generatevulkanloader/TVulkanLoader.h ../.modules/generatevulkanloader/VulkanLoader.h")
end

function WindowsCopyTemplate()
    os.execute("copy ..\\.modules\\generatevulkanloader\\TVulkanLoader.h ..\\.modules\\generatevulkanloader\\VulkanLoader.h > NUL")
end

function LinuxExecuteGenerator()
    os.execute("python3 ../.modules/generatevulkanloader/generate.py")
end

function WindowsExecuteGenerator()
    os.execute("python ../.modules/generatevulkanloader/generate.py")
end

newaction
{
    trigger = "generatevulkanloader",
    description = "Generate Vulkan Loader",

    execute = function()
        print("Checking Dependencies")
        if(_TARGET_OS == "linux") then
            if(not CheckLinuxStuff()) then
                success = false
                return
            end
        elseif(_TARGET_OS == "windows") then
            if(not CheckWindowsStuff()) then
                success = false
                return
            end
        else
            print("Unsupported OS: " .. _TARGET_OS)
            success = false
            return
        end

        if(_TARGET_OS == "linux") then
            LinuxCopyTemplate()
        elseif(_TARGET_OS == "windows") then
            WindowsCopyTemplate()
        end

        print("Generating Vulkan Loader...")
        if(_TARGET_OS == "linux") then
            LinuxExecuteGenerator()
        elseif(_TARGET_OS == "windows") then
            WindowsExecuteGenerator()
        end

        if(_TARGET_OS == "linux") then
            LinuxMoveVulkanLoader()
        elseif(_TARGET_OS == "windows") then
            WindowsMoveVulkanLoader()
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