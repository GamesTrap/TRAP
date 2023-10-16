project "TracyClient"
    kind "StaticLib"
    language "C++"
    warnings "off"

    files "%{IncludeDir.TRACY}/TracyClient.cpp"

	includedirs "%{IncludeDir.TRACY}"
