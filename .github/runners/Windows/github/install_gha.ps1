Invoke-WebRequest -Uri "https://github.com/actions/runner/releases/download/v$env:GH_RUNNER_VERSION/actions-runner-win-x64-$env:GH_RUNNER_VERSION.zip" -OutFile "actions-runner.zip"
Expand-Archive -Path "actions-runner.zip" -DestinationPath '.'
Remove-Item "actions-runner.zip" -Force
