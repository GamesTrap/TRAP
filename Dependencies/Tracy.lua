project "TracyClient"
    kind "StaticLib"
    language "C++"
    warnings "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.group}/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.group}/%{prj.name}")

    files "%{IncludeDir.TRACY}/TracyClient.cpp"

	includedirs "%{IncludeDir.TRACY}"

	filter {"configurations:Profiling"}
	    defines "TRACY_ENABLE"
