import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
"""Source-routing and real CMake INI seed checks; not a Windows key/graphics emulation."""
from pathlib import Path
import re, subprocess, tempfile, unittest
R=Path(__file__).resolve().parents[1]
class Wiring(unittest.TestCase):
 def test_current_shift_f8_route(self):
  s=read_source(R/'src/mod.cpp');a=s.index('auto start_guided_vrik_calibration=');b=s.index('touch_bindings_.SetVrikInputLabStage(0)',a);s=s[a:b]
  self.assertIn('BeginArmLengthCalibration()',s);self.assertIn('f8_down&&!f8_cal_key_prev&&shift_down',s)
  self.assertIn('elapsed>=3000ull',s);self.assertIn('elapsed>=2000ull',s)
  self.assertIn('CompleteArmLengthCalibration()',s);self.assertIn('if(ready){frik_pose_probe::SetCombatCensusMode(3)',s);self.assertNotIn('if(ready){frik_pose_probe::BeginFingerCapture()',s)
  self.assertNotIn('EndGuidedCalibrationPose(',s)
 def test_ini_before_startup_and_worker_service(self):
  s=read_source(R/'src/mod.cpp');self.assertIn('ConfigureCalibrationPersistence(configPathD62.c_str())',s)
  start=s.index('bool Mod::LoadConfig()');load=s[start:s.index('auto settings = ini.section',start)]
  self.assertLess(load.index('Lease iniLeaseD62'),load.index('ConfigureCalibrationPersistence'))
  self.assertLess(load.index('ConfigureCalibrationPersistence'),load.index('ini.load(configPathD62.c_str())'))
  core=read_source(R/'src/frik_pose_probe.cpp');self.assertRegex(core,r'void WeaponControlTick\(\)[^{]*\{[^{}]*ServiceCalibrationIo66100\(\)[^{}]*\}')
  pose=read_source(R/'src/vrik/coherent_pose.inl');self.assertNotIn('ServiceCalibrationIo66100()',pose)
 def test_menu_fields_declared(self):
  h=read_source(R/'src/vr_menu.h');m=read_source(R/'src/mod.cpp');v=read_source(R/'src/vr_menu.cpp')
  for name in ('vrik_finger_capture_state','vrik_finger_capture_active','vrik_calibration_persistence_state'):
   self.assertIn(name,h);self.assertIn('out.'+name,m)
  self.assertIn('s.vrik_calibration_persistence_state<0',v)
 def test_build_marker_and_preclean_backup(self):
  assert_current_identity(R)
  s=(R/"build_release.bat").read_text()
  self.assertLess(s.index('copy /y "build\\release\\RelWithDebInfo\\KCD2VR.ini"'),s.index("if exist build rmdir"))
  self.assertIn('copy /y "%PRESERVED_INI%" "%RESTORE_DIR%\\KCD2VR.ini"',s[s.index(":finish"):])
  self.assertGreaterEqual(s.count("if errorlevel 1 goto finish"),2)
 def test_actual_seed_preserves_existing_ini(self):
  with tempfile.TemporaryDirectory() as root:
   root=Path(root);seed=root/'default.ini';out=root/'game.ini';seed.write_text('[Settings]\nx=default\n')
   cmd=['cmake',f'-DDEFAULT_INI={seed}',f'-DTARGET_INI={out}','-P',str(R/'cmake/seed_ini_if_missing.cmake')]
   subprocess.run(cmd,check=True,capture_output=True);self.assertEqual(out.read_bytes(),seed.read_bytes())
   calibrated=b'[VRIK.ArmCalibration]\nActive=A\nA=keep this existing profile\n';out.write_bytes(calibrated)
   subprocess.run(cmd,check=True,capture_output=True);self.assertEqual(out.read_bytes(),calibrated)
 def test_native_capture_does_not_need_curve_residency(self):
  s=read_source(R/'src/vrik/calibration_runtime.inl');self.assertNotIn('g_finger_acquire6694',s)
  s=read_source(R/'src/vrik/coherent_pose.inl');a=s.index('static bool BeginCoherent6689(');s=s[a:]
  self.assertLess(s.index('FingerCaptureAndLoad66100'),s.index('AcquireCurveReferences6694'))
  self.assertIn('&& !nativeFingerProfile',s.replace('&&!nativeFingerProfile','&& !nativeFingerProfile'))
if __name__=='__main__':unittest.main()
