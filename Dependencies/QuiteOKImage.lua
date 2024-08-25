function GenerateQOICpp()
    local targetFile = "QuiteOKImage/qoi.cpp"

    --Does target file already exist
    if os.isfile(targetFile) then
        return
    end

    io.writefile(targetFile, "#define QOI_IMPLEMENTATION\n#include \"qoi.h\"")
end

project "QuiteOKImage"
    kind "StaticLib"
    language "C++"
    warnings "off"

    GenerateQOICpp()

    files
    {
        "QuiteOKImage/qoi.cpp",
    }

    includedirs
    {
        "QuiteOKImage"
    }
