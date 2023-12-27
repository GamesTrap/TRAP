project "DiscordGameSDK"
    kind "StaticLib"
    language "C++"
    warnings "off"

    files
    {
        "%{IncludeDir.DISCORDGAMESDK}/**.h",
        "%{IncludeDir.DISCORDGAMESDK}/**.cpp"
    }
    externalincludedirs "%{IncludeDir.DISCORDGAMESDK}"
