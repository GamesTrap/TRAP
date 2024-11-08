project "zlib"
    kind "StaticLib"
    language "C"
    warnings "off"

if os.getenv("RUN_CICD_PIPELINE") ~= nil and not table.isempty(os.matchfiles(_MAIN_SCRIPT_DIR .. "/bin/**/*zlib.*")) then
    -- Using CICD Pipeline cache
else
    files
    {
        "zlib/adler32.c",
        "zlib/compress.c",
        "zlib/crc32.c",
        "zlib/deflate.c",
        "zlib/gzclose.c",
        "zlib/gzlib.c",
        "zlib/gzread.c",
        "zlib/gzwrite.c",
        "zlib/inflate.c",
        "zlib/infback.c",
        "zlib/inftrees.c",
        "zlib/inffast.c",
        "zlib/trees.c",
        "zlib/uncompr.c",
        "zlib/zutil.c",
    }
end

    includedirs
    {
        "zlib"
    }

    defines
    {
        "_LARGEFILE64_SOURCE=1"
    }
