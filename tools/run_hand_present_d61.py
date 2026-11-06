"""Compatibility entry point: D62 replaced the unsafe D61 early display bridge.
Historical D61 receipts remain under repair_evidence/d61.
"""
from run_hand_present_d62 import main
if __name__ == '__main__':
    raise SystemExit(main())
