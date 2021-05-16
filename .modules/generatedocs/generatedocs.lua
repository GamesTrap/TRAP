premake.modules.generatedocs = {}
local m = premake.modules.generatedocs

local p = premake

local success = true
local doxygenPath = "Dependencies/Docs/Doxygen"
local doxygenBinPath = doxygenPath .. "/bin/"
local tempSphinxEmbedPath = "../.modules/generatedocs/sphinx/embed"

function InstallSphinx()
    print("Installing Sphinx...")
    os.execute("pip install -U sphinx")
end

function InstallSphinxInlineSyntaxHighlight()
    print("Installing Sphinx-InlineSyntaxHighlight...")
    os.execute("pip install -U sphinxcontrib-inlinesyntaxhighlight")
end

function CheckLinuxStuff()
    print("Checking Doxygen")
    if(os.outputof("doxygen --version > /dev/null 2>&1") == nil) then
        print("Unable to find Doxygen")
        return false
    end

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

    print("Checking Sphinx")
    if(os.outputof("sphinx-build --version > /dev/null 2>&1") == nil) then
        print("Sphinx is not installed!")
        InstallSphinx()
    end

    print("Checking Sphinx-Extensions")
    if(os.outputof("pip list | grep -F sphinxcontrib-inlinesyntaxhighlight > /dev/null 2>&1") == nil) then
        print("Sphinx-InlineSyntaxHighlight is not installed!")
        InstallSphinxInlineSyntaxHighlight()
    end

    return true
end

function CheckWindowsStuff()
    print("Checking Doxygen")
    if(os.outputof("doxygen --version > NUL") == nil) then
        print("Unable to find Doxygen")
        return false
    end

    print("Checking Python")
    if(os.outputof("python --version >NUL") == nil) then
        print("Unable to find Python 3")
        return false
    end

    print("Checking pip")
    if(os.outputof("pip --version > NUL") == nil) then
        print("Unable to find pip")
        return false
    end

    print("Checking Sphinx")
    if(os.outputof("sphinx-build --version > NUL") == nil) then
        print("Sphinx is not installed!")
        InstallSphinx()
    end

    print("Checking Sphinx-Extensions")
    if(os.outputof("pip list | findstr sphinxcontrib-inlinesyntaxhighlight > NUL") == nil) then
        print("Sphinx-InlineSyntaxHighlight is not installed!")
        InstallSphinxInlineSyntaxHighlight()
    end

    return true
end

function LinuxCopyDocs()
    os.mkdir("../docs") --Recreate final docs folder
    os.execute("mv ../.modules/generatedocs/sphinx/** ../docs/")
    os.execute("mv ../.modules/generatedocs/sphinx/.** ../docs/ > /dev/null 2>&1")
end

function WindowsCopyDocs()
    os.execute("move ../.modules/generatedocs/sphinx ../ > NUL")
    os.execute("rename ..\\sphinx docs > NUL")
end

function LinuxCopyFonts()
    os.execute("cp -r ../.modules/generatedocs/fonts ../.modules/generatedocs/sphinx/fonts")
end

function WindowsCopyFonts()
    os.execute("xcopy ../.modules/generatedocs/fonts ../.modules/generatedocs/sphinx/fonts > NUL")
end

newaction
{
    trigger = "gendocs",
    description = "Generate Documentation",

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

        print("Generating Documentation...")
        print("Generating Doxygen pages...")
        if(not os.isdir(tempSphinxEmbedPath)) then
            os.mkdir(tempSphinxEmbedPath)
        end
        local doxygenCmd = "doxygen ../.modules/generatedocs/Doxyfile"
        if(_TARGET_OS == "linux") then
            doxygenCmd = "doxygen ../.modules/generatedocs/Doxyfile > /dev/null 2>&1"
        end
        os.execute(doxygenCmd) --Generate Doxygen pages

        print("Generating Sphinx pages...")
        os.execute("sphinx-build ../.modules/generatedocs/ ../.modules/generatedocs/sphinx -b html -j 8 -q") --Generate Sphinx pages

        print("Copying .nojekyll")
        os.copyfile("../.modules/generatedocs/.nojekyll", "../.modules/generatedocs/sphinx") --Needed for GitHub Pages to display correctly as a static page

        print("Copying fonts")
        if(_TARGET_OS == "linux") then
            LinuxCopyFonts()
        elseif(_TARGET_OS == "windows") then
            WindowsCopyFonts()
        end

        os.rmdir("../docs") --Delete old pages

        print("Copying files to docs folder...")
        if(_TARGET_OS == "linux") then
            LinuxCopyDocs()
        elseif(_TARGET_OS == "windows") then
            WindowsCopyDocs()
        end
    end,

    onEnd = function()
        if(success) then
            print("Documentation generation successful")
        else
            print("Documentation generation failed")
        end
    end
}

return m