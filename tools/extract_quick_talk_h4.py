from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
h=(r/'src/subsystems/hud_ui_controls/host.inl').read_text();start=h.index('struct GestureFrame59c5');frame=h[start:h.index('static void ApplyUtilityGestures54',start)];start=h.index('static kcd2vr::gesture_talk59c5::Output PrepareGestureTalkH4');body=h[start:h.index('// M2_SLOT_END 5016',start)]
(r/'tests/generated_quick_talk_h4.inc').write_text(frame+body)
m=(r/'src/mod.cpp').read_text();a=m.index('const auto gestureTalkH4=PrepareGestureTalkH4');b=m.index('auto pad=native_vr_pad_premap::Build',a);c=m.index('kcd2vr::gesture_talk59c5::apply54(pad,gestureTalkH4)',b);d=m.index('native_input_look_lab::PublishGamepadState',c);assert a<b<c<d
n=(r/'src/vrik/physical_native.inl').read_text();p=(r/'src/subsystems/throw_drop/holster_gestures.inl').read_text()
assert 'talkH4::equipment(' not in n
assert 'talkOwnsH4' in p and 'equipment114::update(pending,observation)' in p
(r/'tests/quick_talk_h4_provenance.json').write_text(json.dumps({'source':'src/subsystems/hud_ui_controls/host.inl','sha256':hashlib.sha256(h.encode()).hexdigest(),'generatedSHA256':hashlib.sha256((frame+body).encode()).hexdigest(),'actualModCallerOrdering':'PrepareGestureTalkH4 -> Build -> apply54 -> PublishGamepadState','modSHA256':hashlib.sha256(m.encode()).hexdigest(),'equipmentOwnership':'Earlier Talk arbitrates before producer acquisition; actual ring-enqueued equipment is not re-arbitrated by later Talk. Issued pending FSM is observation-only.','nativeConsumerSHA256':hashlib.sha256(n.encode()).hexdigest(),'holsterProducerSHA256':hashlib.sha256(p.encode()).hexdigest()},indent=2))
