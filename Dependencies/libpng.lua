function GeneratePNGLibConfig()
    local templateFile = "libpng/scripts/pnglibconf.h.prebuilt"
    local targetFile = "libpng/pnglibconf.h"

    --Does target file already exist
    if os.isfile(targetFile) then
        return
    end

    local ok, err = os.copyfile(templateFile, targetFile)

    if not ok then
        term.setTextColor(term.errorColor)
        print("Unable to copy " .. templateFile .. " to " .. targetFile .. ": " .. err)
        term.setTextColor(nil)
        os.exit(-1)
    end
end

project "libpng"
    kind "StaticLib"
    language "C"
    warnings "off"

    GeneratePNGLibConfig()

    files
    {
        "libpng/png.c",
        "libpng/pngerror.c",
        "libpng/pngget.c",
        "libpng/pngmem.c",
        "libpng/pngpread.c",
        "libpng/pngread.c",
        "libpng/pngrio.c",
        "libpng/pngrtran.c",
        "libpng/pngrutil.c",
        "libpng/pngset.c",
        "libpng/pngtrans.c",
        "libpng/pngwio.c",
        "libpng/pngwrite.c",
        "libpng/pngwtran.c",
        "libpng/pngwutil.c",
    }

    includedirs
    {
        "libpng",
		"%{IncludeDir.ZLIB}"
    }
