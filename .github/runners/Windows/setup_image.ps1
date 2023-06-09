& .\install_powershellmodules.ps1 #Setup Powershell Modules
& .\initialize-image.ps1 #Initialize image
& .\configure-antivirus.ps1 #Configure Antivirus
& .\install_choco.ps1 -Wait #Install Chocolatey
& .\install_software.ps1 #Install Software
& .\disable-jitdebugger.ps1 #Disable JIT Debuggers
Invoke-WebRequest -Uri "https://github.com/actions/runner/releases/download/v$env:GH_RUNNER_VERSION/actions-runner-win-x64-$env:GH_RUNNER_VERSION.zip" -OutFile "actions-runner.zip"; Expand-Archive -Path "actions-runner.zip" -DestinationPath '.'; Remove-Item "actions-runner.zip" -Force #Download GitHub Action Runner
& .\finalize-image.ps1 #Finalize image
