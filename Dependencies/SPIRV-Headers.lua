--ODR violation fix
if os.isfile("SPIRV-Headers/include/spirv/unified1/spirv.hpp") then
    os.remove("SPIRV-Headers/include/spirv/unified1/spirv.hpp")
end
if os.isfile("SPIRV-Headers/include/spirv/unified1/spirv.hpp11") then
    os.remove("SPIRV-Headers/include/spirv/unified1/spirv.hpp11")
end