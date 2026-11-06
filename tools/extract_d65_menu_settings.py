"""Extract the actual D65 menu settings contributions for a bounded IO fixture."""
from pathlib import Path
import argparse,hashlib,json,os
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args()
r=Path(__file__).resolve().parents[1];o=a.out;o.mkdir(parents=True,exist_ok=True)
def atomic_text(path,text):
 tmp=path.with_name(path.name+'.d69.tmp');tmp.write_bytes(text.encode());os.replace(tmp,path)

live=(r/'src/subsystems/experimental/live_runtime_d40.inl').read_text()
mod=(r/'src/mod.cpp').read_text();ranged=(r/'src/subsystems/ranged_reload/host.inl').read_text()
config=(r/'src/subsystems/ranged_reload/config.inl').read_text();owner=(r/'src/action_time_owner59c11.cpp').read_text()
sections={}
for mode in ['READ','APPLY','SAVE','LOAD']:
 start=live.index('defined(KCD2VR_D40_LIVE_'+mode+')')
 end=live.find('#elif',start)
 if end<0:end=live.index('#else',start)
 body=live[start:end]
 names=('npcLifeD73','lively_npc_d73::','mapNativePaperM43','held_map_m1::NativePaperSelectedM43','held_map_m1::SetNativePaperM43','knifeRightHipD66','knife_pouch_d66::','crankTravelD66','twoHandBiasD65','twoHandPrimaryBiasD65','bowAimD65','crossbowReloadD65','alchemySpeedD65','pickupClockD65','pickupCameraBridgeD55',
        'two_hand_d65::','ranged_options_d65c::','ranged_options_d66c::','SetAlchemyTransferSpeedD65','AlchemyTransferSpeedD65()',
        'SetUnownedPickupClockD65','UnownedPickupClockD65()','camera_hold_e1::pickupBridgeD55','VrAlchemyTransferTuningD67Adopted','VrAlchemyTransferSpeedD65',
        'motion_trials_d67::','held_motion_d67::','crafting_d67::','claude_motion_d69::','weapon_trials_d69::','claude_d70::','contact_handover_d71::','roomscale58::','dodge_window_d70::')
 lines=[x for x in body.splitlines() if any(n in x for n in names)]
 if mode=='READ': lines.insert(0,'auto& d40=out.live_d39;')
 if mode=='APPLY':lines.insert(0,'const auto& d40=state.live_d39;')
 # The punch base is in the existing combat settings owner; ranged persistence
 # stays in its existing module. Extract those exact lines too, not replicas.
 lines += [x for x in mod.splitlines() if
     ('out.punch_minimum_m_d65=' in x if mode=='READ' else
      'punch_travel_d65::minimumMeters.store' in x and 'state.' in x if mode=='APPLY' else
      'ok=wf("VrPunchMinimumTravelMetersD65"' in x if mode=='SAVE' else
      'punch_travel_d65::minimumMeters.store' in x and 'ReadIni' in x)]
 if mode in ('SAVE','LOAD'):lines += [x for x in mod.splitlines() if '"VrMotionTrialD67"' in x and ('ok=wi' in x if mode=='SAVE' else 'ReadIniIntExplicit' in x)]
 if mode=='SAVE':lines += [x for x in ranged.splitlines() if 'ok' in x and ('"VrBowPhysicalAimD65C"' in x or '"VrCrossbowReloadD65C"' in x or '"VrCrankTravelD66C"' in x)]
 if mode=='LOAD':lines += [x for x in config.splitlines() if 'ReadIni' in x and ('"VrBowPhysicalAimD65C"' in x or '"VrCrossbowReloadD65C"' in x or '"VrCrankTravelD66C"' in x)]
 assert len(lines)>=7,(mode,lines)
 sections[mode]=lines
 atomic_text(o/('d65_settings_'+mode.lower()+'.inc'),'\n'.join(lines)+'\n')
apis=[x for x in owner.splitlines() if x.startswith(('void SetAlchemyTransferSpeedD65(','float AlchemyTransferSpeedD65(','void SetUnownedPickupClockD65(','bool UnownedPickupClockD65('))]
assert len(apis)==4
atomic_text(o/'d65_action_options.inc','\n'.join(apis)+'\n')
files=['src/mod.cpp','src/subsystems/experimental/live_runtime_d40.inl','src/subsystems/experimental/live_settings_d39.h',
       'src/subsystems/ranged_reload/host.inl','src/subsystems/ranged_reload/config.inl','src/action_time_owner59c11.cpp',
       'src/vrik/claude_motion_d69.h','src/vrik/weapon_trials_d69.h','src/experiments/claude_bundle_d70.h','src/experiments/contact_handover_d71.h']
atomic_text(o/'SETTINGS_INPUTS.json',json.dumps({'source':str(r),'files':{n:hashlib.sha256((r/n).read_bytes()).hexdigest() for n in files},'extracted':sections},indent=2)+'\n')
print('Extracted current D69 read/apply/save/load and action option APIs')
