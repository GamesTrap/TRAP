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

os.rename("lib/x86_64/discord_game_sdk.so", "lib/x86_64/libdiscord_game_sdk.so");
