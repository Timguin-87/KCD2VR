"""Reuse the established PE/import/build-log checks with the V166 marker.

Reads the linked file only; never loads the ASI or changes the game directory.
"""
import verify_release147 as verifier

verifier.MARKER = 'KCD2VR-V66.166-HUD-HILT-BOW-CONTRACT-REPAIRS'

if __name__ == '__main__':
    verifier.main()
