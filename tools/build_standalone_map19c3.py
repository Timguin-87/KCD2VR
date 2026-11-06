"""Historical entry point: C32 requires the complete native artwork payload.

This invokes the pinned map builder; use --data, --output and --report. Merge its
8-member overlay into the sole current companion. Do not install a second map
mod or the retired minimap-dependent KCD2VRHeldMap addon.
"""
from pathlib import Path
import runpy

if __name__ == "__main__":
    runpy.run_path(str(Path(__file__).with_name("build_native_held_map32.py")),run_name="__main__")
