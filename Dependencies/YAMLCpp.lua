project "YAMLCpp"
    kind "StaticLib"
    language "C++"
    warnings "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    files
	{
		"YAMLCpp/src/**.h",
		"YAMLCpp/src/**.cpp",

		"YAMLCpp/include/**.h"
	}

	includedirs "%{IncludeDir.YAMLCPP}"

	defines "YAML_CPP_STATIC_DEFINE"
