#!/usr/bin/dumb-init /bin/bash
# shellcheck shell=bash

export RUNNER_ALLOW_RUNASROOT=1
export PATH=${PATH}:/actions-runner

_RANDOM_RUNNER_SUFFIX=${RANDOM_RUNNER_SUFFIX:="true"}

_RUNNER_NAME=${RUNNER_NAME:-${RUNNER_NAME_PREFIX:-gitea-runner}-$(head /dev/urandom | tr -dc A-Za-z0-9 | head -c 13 ; echo '')}
if [[ ${RANDOM_RUNNER_SUFFIX} != "true" ]]; then
  # In some cases this file does not exist
  if [[ -f "/etc/hostname" ]]; then
    # in some cases it can also be empty
    if [[ $(stat --printf="%s" /etc/hostname) -ne 0 ]]; then
      _RUNNER_NAME=${RUNNER_NAME:-${RUNNER_NAME_PREFIX:-gitea-runner}-$(cat /etc/hostname)}
      echo "RANDOM_RUNNER_SUFFIX is ${RANDOM_RUNNER_SUFFIX}. /etc/hostname exists and has content. Setting runner name to ${_RUNNER_NAME}"
    else
      echo "RANDOM_RUNNER_SUFFIX is ${RANDOM_RUNNER_SUFFIX} ./etc/hostname exists but is empty. Not using /etc/hostname."
    fi
  else
    echo "RANDOM_RUNNER_SUFFIX is ${RANDOM_RUNNER_SUFFIX} but /etc/hostname does not exist. Not using /etc/hostname."
  fi
fi

_LABELS=${LABELS:-default}
_GITEA_HOST=${GITEA_HOST}
_RUN_AS_ROOT=${RUN_AS_ROOT:="true"}

configure_runner() {
  ARGS=()

  echo "Configuring"
  ./act_runner \
      register \
      --no-interactive \
      --instance "${_GITEA_HOST}" \
      --token "${RUNNER_TOKEN}" \
      --name "${_RUNNER_NAME}" \
      --labels "${_LABELS}"
}

configure_runner

# Container's command (CMD) execution as runner user

if [[ ${_RUN_AS_ROOT} == "true" ]]; then
  if [[ $(id -u) -eq 0 ]]; then
    "$@"
  else
    echo "ERROR: RUN_AS_ROOT env var is set to true but the user has been overridden and is not running as root, but UID '$(id -u)'"
    exit 1
  fi
else
  if [[ $(id -u) -eq 0 ]]; then
    /usr/bin/chown -R runner "${_RUNNER_WORKDIR}" /actions-runner
    # The toolcache is not recursively chowned to avoid recursing over prepulated tooling in derived docker images
    /usr/bin/chown runner /opt/hostedtoolcache/
    /usr/sbin/gosu runner "$@"
  else
    "$@"
  fi
fi
