premake.modules.generatedocs = {}
local m = premake.modules.generatedocs

local p = premake

success = true
doxygenPath = "Dependencies/Docs/Doxygen"
doxygenBinPath = doxygenPath .. "/bin/"
tempSphinxEmbedPath = "../.modules/generatedocs/sphinx/embed"

newaction
{
    trigger = "gendocs",
    description = "Generate Documentation",

    execute = function()
    local res = true
        print("Checking Dependencies")
        if(os.host() == "linux") then
            print("Checking Doxygen")
            local out, errorCode = os.outputof("doxygen --version")
            if(errorCode ~= 0) then
                print("Unable to find Doxygen")
                res = false
            end

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

            print("Checking Sphinx")
            local out, errorCode = os.outputof("sphinx-build --version")
            if(errorCode ~= 0) then
                print("Sphinx is not installed!")
                print("Installing Sphinx...")
                os.execute("pip install -U sphinx  > /dev/null 2>&1")
            end

            print("Checking Sphinx-Extensions")
            local out, errorCode = os.outputof("pip list | grep -F sphinxcontrib-inlinesyntaxhighlight")
            if(errorCode ~= 0) then
                print("Sphinx-InlineSyntaxHighlight is not installed!")
                print("Installing Sphinx-InlineSyntaxHighlight...")
                os.execute("pip install -U sphinxcontrib-inlinesyntaxhighlight  > /dev/null 2>&1")
            end

            local out, errorCode = os.outputof("pip list | grep -F furo")
            if(errorCode ~= 0) then
                print("Furo theme is not installed!")
                print("Installing Furo theme...")
                os.execute("pip install -U furo  > /dev/null 2>&1")
            end

            local out, errorCode = os.outputof("pip list | grep -F sphinx_inline_tabs")
            local out2, errorCode2 = os.outputof("pip list | grep -F sphinx-inline-tabs")
            if(errorCode ~= 0 and errorCode2 ~= 0) then
                print("Sphinx-Inline-Tabs is not installed!")
                print("Installing Sphinx-Inline-Tabs...")
                os.execute("pip install -U sphinx-inline-tabs  > /dev/null 2>&1")
            end

            local out, errorCode = os.outputof("pip list | grep -F sphinxext-opengraph")
            if(errorCode ~= 0) then
                print("SphinxExt-OpenGraph is not installed!")
                print("Installing SphinxExt-OpenGraph...")
                os.execute("pip install -U sphinxext-opengraph  > /dev/null 2>&1")
            end

            local out, errorCode = os.outputof("pip list | grep -F sphinx-copybutton")
            if(errorCode ~= 0) then
                print("Sphinx-CopyButton is not installed!")
                print("Installing Sphinx-CopyButton...")
                os.execute("pip install -U sphinx-copybutton  > /dev/null 2>&1")
            end

            if(res == false) then
                success = false
                return
            end
        elseif(os.host() == "windows") then
            print("Checking Doxygen")
            local out, errorCode = os.outputof("doxygen --version")
            if(errorCode ~= 0) then
                print("Unable to find Doxygen")
                res = false
            end

            print("Checking Python")
            local out, errorCode = os.outputof("python --version")
            if(errorCode ~= 0) then
                print("Unable to find Python")
                res = false
            end

            print("Checking pip")
            local out, errorCode = os.outputof("pip --version")
            if(errorCode ~= 0) then
                print("Unable to find pip")
                res = false
            end

            print("Checking Sphinx")
            local out, errorCode = os.outputof("sphinx-build --version")
            if(errorCode ~= 0) then
                print("Sphinx is not installed!")
                print("Installing Sphinx...")
                os.execute("pip install -U sphinx  > NUL")
            end

            print("Checking Sphinx-Extensions")
            local out, errorCode = os.outputof("pip list | findstr sphinxcontrib-inlinesyntaxhighlight")
            if(errorCode ~= 0) then
                print("Sphinx-InlineSyntaxHighlight is not installed!")
                print("Installing Sphinx-InlineSyntaxHighlight...")
                os.execute("pip install -U sphinxcontrib-inlinesyntaxhighlight  > NUL")
            end

            local out, errorCode = os.outputof("pip list | findstr furo")
            if(errorCode ~= 0) then
                print("Furo theme is not installed!")
                print("Installing Furo theme...")
                os.execute("pip install -U furo  > NUL")
            end

            local out, errorCode = os.outputof("pip list | findstr sphinx_inline_tabs")
            local out2, errorCode2 = os.outputof("pip list | findstr sphinx-inline-tabs")
            if(errorCode ~= 0 and errorCode2 ~= 0) then
                print("Sphinx-Inline-Tabs is not installed!")
                print("Installing Sphinx-Inline-Tabs...")
                os.execute("pip install -U sphinx-inline-tabs  > NUL")
            end

            local out, errorCode = os.outputof("pip list | findstr sphinxext-opengraph")
            if(errorCode ~= 0) then
                print("SphinxExt-OpenGraph is not installed!")
                print("Installing SphinxExt-OpenGraph...")
                os.execute("pip install -U sphinxext-opengraph  > NUL")
            end

            local out, errorCode = os.outputof("pip list | findstr sphinx-copybutton")
            if(errorCode ~= 0) then
                print("Sphinx-CopyButton is not installed!")
                print("Installing Sphinx-CopyButton...")
                os.execute("pip install -U sphinx-copybutton  > NUL")
            end

            if(res == false) then
                success = false
                return
            end
        else
            print("Unsupported OS: " .. os.host())
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
            os.execute("doxygen ..\\.modules\\generatedocs\\Doxyfile")
        elseif(os.host() == "linux") then
            os.execute("doxygen ../.modules/generatedocs/Doxyfile > /dev/null 2>&1")
        end

        print("Generating Sphinx pages...")
        if(os.host() == "windows") then
            os.execute("sphinx-build ..\\.modules\\generatedocs\\ ..\\.modules\\generatedocs\\sphinx -b html -j 8 -q") --Generate Sphinx pages
        elseif(os.host() == "linux") then
            os.execute("sphinx-build ../.modules/generatedocs/ ../.modules/generatedocs/sphinx -b html -j 8 -q") --Generate Sphinx pages
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
            print("Documentation generation successful")
        else
            print("Documentation generation failed")
        end
    end
}

return m