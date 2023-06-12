# GitHub Runner Images

This folder contains the Dockerfiles and scripts used to create the images used by the GitHub Action workflows.

## Linux

### Base Image

OS: Ubuntu 23.04  

Software:

- gnupg
- lsb-release
- curl
- tar
- unzip
- zip
- apt-transport-https
- ca-certificates
- sudo
- gpg-agent
- software-properties-common
- zlib1g-dev
- zstd
- gettext
- libcurl4-openssl-dev
- inetutils-ping
- jq
- wget
- dirmngr
- openssh-client
- locales
- python3-pip
- python3-setuptools
- python3
- dumb-init
- rsync
- libpq-dev
- gosu
- git
- git-man
- git-lfs
- NodeJS 12.22.12 (needed for actions/cache@v2)

### Final Image

OS: Ubuntu 23.04  

Software:

- GitHub Action Runner

Scripts (in execution order):

- Install GitHub Actions Runner and dependencies (install_actions.sh)
- Entrypoint script of container (entrypoint.sh)
- Utility scripts (app_token.sh) (token.sh)

The resulting Image can be run via the provided docker-compose.linux.yml file.

## Windows

OS: Windows Server Core 2022 LTSC  

Software:

- Chocolatey
- Git
- GitHub CLI
- 7-Zip
- Jq
- CMake
- VCRedists
- Visual Studio 2022 Build Tools (C++ workload only)
- NodeJS 12.22.12 (Needed for actions/cache@v2)
- GitHub Action Runner

Scripts (in execution order):

- Setup Image (setup_image.ps1)
- Install Powershell Modules (install_powershellmodules.ps1)
- Initialize Image (initialize-image.ps1)
- Antivirus configuration (configure-antivirus.ps1)
- Install Chocolatey (install_choco.ps1)
- Install Software (install_software.ps1)
- Disable JIT Debugger and Windows error reporting (disable-jitdebugger.ps1)
- Image cleanup (finalize-image.ps1)
- Entrypoint script of container (start.ps1)
- (Unused) Cleanup GitHub Action Runners (cleanup_runners.ps1)

The resulting Image can be run via the provided docker-compose.windows.yml file.
