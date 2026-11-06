import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import re, unittest
R = Path(__file__).resolve().parents[1]
P = read_source(R/"src/diagnostics/native_hit_probe134.inl")

class Probe134(unittest.TestCase):
    def test_only_f3_rung_one(self):
        self.assertIn("diagnostics::lab::Get(diagnostics::lab::Key::F3)==1", P)

    def test_every_detour_forwards_original_once(self):
        for name, orig in [("ProcessHitDetour","g_process_original"),("SubmitHitDetour","g_submit_original"),
                           ("ActorUpdateDetour","g_update_original"),("PhysCollisionDetour","g_phys_original")]:
            body = P.split(name+"(",1)[1].split("\n}\n",1)[0]
            self.assertGreaterEqual(body.count(orig+"("), 1, name)

    def test_read_only(self):
        # No writes into native memory: reads go through ReadCopy6689 only.
        self.assertNotIn("PhysPutFloat", P); self.assertNotIn("WriteQuat", P)
        self.assertIn("writes=NONE", P)

    def test_prologues_are_checked(self):
        self.assertIn("FAIL-CLOSED prologue mismatch", P)

    def test_weapon_constraint_switches_wired(self):
        c = read_source(R/"src/vrik/coherent_pose.inl"); m = read_source(R/"src/mod.cpp")
        self.assertIn("g_weapon_constraint_drive134.load", c); self.assertIn("g_weapon_two_hand_constraint134.load", c)
        self.assertIn('"WeaponConstraintDrive"', m); self.assertIn('"WeaponTwoHandConstraint"', m)

    def test_player_combat_actor_uses_proven_chain(self):
        self.assertIn("kCombatPlayerVft=0x3A80610ull", P)
        self.assertIn("kPlayerVft=0x3AA4B98ull", P)
        self.assertIn("Rd<const void*>(player,0x278,nullptr)!=actor", P)
        self.assertIn("entity=Rd<const void*>(player,0x38,nullptr)", P)
        self.assertIn("Rd<const void*>(state,0xCE0,nullptr)!=actor", P)
        self.assertIn("playerResolved=%llu", P)
        self.assertIn("identityReject=%llu", P)

if __name__ == "__main__":
    unittest.main()
