project "ModernDialogs"
    kind "StaticLib"
    language "C++"
    warnings "off"

if os.getenv("RUN_CICD_PIPELINE") ~= nil and not table.isempty(os.matchfiles(_MAIN_SCRIPT_DIR .. "/bin/**/*ModernDialogs.*")) then
    -- Using CICD Pipeline cache
else
    files
    {
        "ModernDialogs/ModernDialogs/**.h",
        "ModernDialogs/ModernDialogs/**.cpp"
    }
end
