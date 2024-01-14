Invoke-WebRequest -Uri "https://gitea.com/gitea/act_runner/releases/download/v$env:GT_RUNNER_VERSION/act_runner-$env:GT_RUNNER_VERSION-windows-amd64.exe" -OutFile "act_runner.exe"
