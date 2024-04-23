choco install -y gh 7zip jq vcredist-all python ninja nodejs-lts
choco install -y git.install --params "'/GitAndUnixToolsOnPath'"
choco install -y cmake --installargs "ADD_CMAKE_TO_PATH=System"
choco install -y visualstudio2022buildtools --package-parameters "--includeRecommended --add Microsoft.VisualStudio.Workload.VCTools --locale en-US" #Install Visual Studio 2022 Build Tools (C++ Workload)
Import-Module $env:ChocolateyInstall\helpers\chocolateyProfile.psm1
refreshenv
$GitBashPath = ";C:\Program Files\Git\bin;"
$CMakePath = ";C:\Program Files\CMake\bin;"
$7zPath = ";C:\Program Files\7-Zip;"
[Environment]::SetEnvironmentVariable("PATH", $env:PATH.TrimEnd(';') + $GitBashPath, "Machine") #Add bash.exe to PATH
[Environment]::SetEnvironmentVariable("PATH", $env:PATH.TrimEnd(';') + $CMakePath, "Machine") #Add cmake.exe to PATH
[Environment]::SetEnvironmentVariable("PATH", $env:PATH.TrimEnd(';') + $7zPath, "Machine") #Add 7z.exe to PATH

refreshenv
