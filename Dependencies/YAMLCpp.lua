project "YAMLCpp"
    kind "StaticLib"
    language "C++"
    warnings "off"

    files
	{
		"YAMLCpp/src/**.h",
		"YAMLCpp/src/**.cpp",

		"YAMLCpp/include/**.h"
	}

	includedirs "%{IncludeDir.YAMLCPP}"
