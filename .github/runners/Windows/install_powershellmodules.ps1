# Set TLS1.2
[Net.ServicePointManager]::SecurityProtocol = [Net.ServicePointManager]::SecurityProtocol -bor "Tls12"

Write-Host "Setup PowerShellGet"
Install-PackageProvider -Name NuGet -MinimumVersion 2.8.5.201 -Force

# Specifies the installation policy
Set-PSRepository -InstallationPolicy Trusted -Name PSGallery

Write-Host 'Warmup PSModuleAnalysisCachePath (speedup first powershell invocation by 20s)'
$PSModuleAnalysisCachePath = 'C:\PSModuleAnalysisCachePath\ModuleAnalysisCache'

[Environment]::SetEnvironmentVariable('PSModuleAnalysisCachePath', $PSModuleAnalysisCachePath, "Machine")
# make variable to be available in the current session
${env:PSModuleAnalysisCachePath} = $PSModuleAnalysisCachePath

New-Item -Path $PSModuleAnalysisCachePath -ItemType 'File' -Force | Out-Null

# Install PowerShell modules
$modules = @('PowershellGet', 'DockerMsftProvider')

foreach($module in $modules)
{
    $moduleName = $module
    Write-Host "Installing ${moduleName} module"

    if ($module.versions)
    {
        foreach ($version in $module.versions)
        {
            Write-Host " - $version"
            Install-Module -Name $moduleName -RequiredVersion $version -Scope AllUsers -SkipPublisherCheck -Force
        }
        continue
    }

    Install-Module -Name $moduleName -Scope AllUsers -SkipPublisherCheck -Force
}
