project "TracyClient"
    kind "StaticLib"
    language "C++"
    warnings "off"

if os.getenv("RUN_CICD_PIPELINE") ~= nil and not table.isempty(os.matchfiles(_MAIN_SCRIPT_DIR .. "/bin/**/*TracyClient.*")) then
    -- Using CICD Pipeline cache
else
    files "%{IncludeDir.TRACY}/TracyClient.cpp"
end

	includedirs "%{IncludeDir.TRACY}"
