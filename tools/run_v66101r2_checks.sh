#!/usr/bin/env bash
# Host-only V66.101R2 suite; no Windows SDK/link or native-game execution.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
exec bash "$ROOT/tools/run_v66101r1_checks.sh" "${1:-$ROOT/delivery/v66101r2/host}" "${2:-}"
