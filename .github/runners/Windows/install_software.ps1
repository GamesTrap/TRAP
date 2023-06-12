choco install -y git gh 7zip jq vcredist-all
choco install -y nodejs-lts --version=12.22.12
choco install -y cmake.install --installargs "ADD_CMAKE_TO_PATH=System"
choco install -y visualstudio2022buildtools --package-parameters "--includeRecommended --add Microsoft.VisualStudio.Workload.VCTools --locale en-US" #Install Visual Studio 2022 Build Tools (C++ Workload)
Import-Module $env:ChocolateyInstall\helpers\chocolateyProfile.psm1
refreshenv
$GitBashPath = "C:\Program Files\git\bin"
$CMakePath = "C:\Program Files\CMake\bin"
[Environment]::SetEnvironmentVariable("PATH", $env:PATH + $GitBashPath, "Machine") #Add bash.exe to PATH
[Environment]::SetEnvironmentVariable("PATH", $env:PATH + $CMakePath, "Machine") #Add cmake.exe to PATH

#Create alias for node12
if (!(Test-Path $PROFILE))
{
    New-Item -Type File -Path $PROFILE -Force
}
Set-Content -Path $PROFILE -Value "Set-Alias -Name node12 -Value node"
Invoke-Item $PROFILE
Set-Alias -Name node12 -Value node

refreshenv
