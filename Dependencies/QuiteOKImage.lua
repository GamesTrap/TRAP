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

if os.getenv("RUN_CICD_PIPELINE") ~= nil and not table.isempty(os.matchfiles(_MAIN_SCRIPT_DIR .. "/bin/**/*QuiteOKImage.*")) then
    -- Using CICD Pipeline cache
else
    files
    {
        "QuiteOKImage/qoi.cpp",
    }
end

    includedirs
    {
        "QuiteOKImage"
    }
