#!/usr/bin/env bash
set -euo pipefail

target=${TARGET:?Set TARGET, for example pi@192.168.1.50}
destination=${DEST:-/opt/qt-sysmonitor}
binary=${1:-build-aarch64/QtSysMonitor}

ssh "$target" "sudo mkdir -p '$destination' && sudo chown \$(id -u):\$(id -g) '$destination'"
scp "$binary" "$target:$destination/QtSysMonitor"
echo "Deployed $binary to $target:$destination/QtSysMonitor"
