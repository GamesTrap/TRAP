-- Make ffi.h compatible
ffi = ""
for line in io.lines("cpp/ffi.h") do
    if (string.contains(line, "#include <stdint.h>")) then
        ffi = ffi .. "#include <cstdint>" .. "\n"
    else
        ffi = ffi .. line .. "\n"
    end
end
ffiFile = io.open("cpp/ffi.h", "r+")
if ffiFile then
    ffiFile:write(ffi)
    io.close(ffiFile)
end

-- Make every other .cpp file compatible
cppFiles = {"achievement_manager.cpp", "activity_manager.cpp",
            "application_manager.cpp", "core.cpp",
            "image_manager.cpp", "lobby_manager.cpp",
            "network_manager.cpp", "overlay_manager.cpp",
            "relationship_manager.cpp", "storage_manager.cpp",
            "store_manager.cpp", "types.cpp",
            "user_manager.cpp", "voice_manager.cpp"}
incompatibleFiles = {}
for key, value in ipairs(cppFiles) do
    f = io.open("cpp/" .. value, "r")
    if f then
        firstLine = f:read()
        if (not string.match(firstLine, '#include "TRAPPCH.h')) then
            incompatibleFiles[#incompatibleFiles + 1] = value
        end
        io.close(f);
    end
end

for key, value in ipairs(incompatibleFiles) do
    content = '#include "TRAPPCH.h"\n'
    for line in io.lines("cpp/" .. value) do
        content = content .. line .. "\n"
    end

    f = io.open("cpp/" .. value, "w")
    if f then
        f:write(content)
        io.close(f);
    end
end