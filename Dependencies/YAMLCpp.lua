project "YAMLCpp"
    kind "StaticLib"
    language "C++"
    warnings "off"

if os.getenv("RUN_CICD_PIPELINE") ~= nil and not table.isempty(os.matchfiles(_MAIN_SCRIPT_DIR .. "/bin/**/*YAMLCpp.*")) then
	-- Using CICD Pipeline cache
else
    files
	{
		"YAMLCpp/src/**.h",
		"YAMLCpp/src/**.cpp",

		"YAMLCpp/include/**.h"
	}
end

	includedirs "%{IncludeDir.YAMLCPP}"
