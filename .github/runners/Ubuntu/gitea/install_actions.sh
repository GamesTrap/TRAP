#!/bin/bash -ex
GT_RUNNER_VERSION=$1
TARGETPLATFORM=$2

export TARGET_ARCH="amd64"
if [[ $TARGETPLATFORM == "linux/arm64" ]]; then
  export TARGET_ARCH="arm64"
fi

curl -L "https://gitea.com/gitea/act_runner/releases/download/v${GT_RUNNER_VERSION}/act_runner-${GT_RUNNER_VERSION}-linux-${TARGET_ARCH}.xz" > act_runner.xz
unxz act_runner.xz
chmod +x "act_runner"
mkdir /_work
