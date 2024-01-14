& .\install_powershellmodules.ps1 #Setup Powershell Modules
& .\initialize-image.ps1 #Initialize image
& .\configure-antivirus.ps1 #Configure Antivirus
& .\install_choco.ps1 -Wait #Install Chocolatey
& .\install_software.ps1 #Install Software
& .\disable-jitdebugger.ps1 #Disable JIT Debuggers
& .\finalize-image.ps1 #Finalize image
