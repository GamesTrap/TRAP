Param (
    [Parameter(Mandatory = $false)]
    [string]$gthost = $env:GT_HOST,
    [Parameter(Mandatory = $false)]
    [string]$regToken = $env:GT_TOKEN,
    [Parameter(Mandatory = $false)]
    [string]$name = $env:RUNNER_NAME,
    [Parameter(Mandatory = $false)]
    [string]$runnerLabels = $env:RUNNER_LABELS
)

$runnerBaseName = $name
$runnerName = $runnerBaseName + '-' + (((New-Guid).Guid).replace("-", "")).substring(0, 5)

try {
    #Register new runner instance
    write-host "Registering Gitea Self Hosted Runner on: $gthost"
    ./act_runner.exe register --no-interactive --instance $gthost --token $regToken --name $runnerName --labels $runnerLabels

    #Start runner listener for jobs
    ./act_runner.exe daemon
}
catch {
    Write-Error $_.Exception.Message
}
