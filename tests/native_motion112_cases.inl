// Included in the full production-TU bridge fixture. Synthetic memory is not
// evidence of retail timing; it exercises the actual predicates and adapters.
diagnostics::lab::Set(diagnostics::lab::Key::F3,3);
using kcd2vr::retail_build112::hostRetailVerified112;
hostRetailVerified112=true;g_motion_contract112=base;
g_phys_armed6698=true;g_phys_fault6698=false;g_native_fault104=false;
g_gameplay_allowed=true;g_head_valid=true;g_right_valid=true;g_left_valid=true;
animation_filter::nativeBodyOwned112=false;g_census_mode=3;g_weapon_control_mode=6;
put(state.data()+0x218,1);put(state.data()+0x2D8,std::uint8_t(1));
put(state.data()+0x2A8,std::uint8_t(0));put(player.data()+0x30,std::uint32_t(123));
std::array<unsigned char,0x80> alignment{},record{},clip{},producer{};
std::array<unsigned char,0xB00> character{};
std::array<unsigned char,0x420> motionAction{};std::array<unsigned char,0x20> collision{};
put(alignment.data(),base+0x3A633B0);put(alignment.data()+8,actor.data());
put(actor.data()+0x250,alignment.data());put(actor.data()+0x218,collision.data());
put(collision.data()+8,actor.data());
put(alignment.data()+0x58,reinterpret_cast<std::uintptr_t>(record.data()));
put(alignment.data()+0x60,reinterpret_cast<std::uintptr_t>(record.data()+0x20));
put(alignment.data()+0x68,reinterpret_cast<std::uintptr_t>(record.data()+record.size()));
put(record.data()+0xC,std::uint32_t(123));put(record.data()+0x10,std::uint8_t(2));
put(record.data()+0x18,clip.data());put(clip.data(),base+0x3BAD328);
frame.input.tick=hostTickCount64;frame.input.sample=200;
frame.output.active[0]=frame.output.owned[0]=frame.output.contactKnown[0]=true;
frame.profiles[0].primary=frame.profiles[0].nativeSeed=true;
frame.profiles[0].nativeWeapon={{0,0,0,1},{0,1,0}};
frame.output.weapons[0]={{0,0,0,1},{.1,1,0}};
frame.world[3]=frame.world[7]=1;
g_phys_frame6698.publish(frame);animation_filter::combatPoseTick112=hostTickCount64;
ck(NativeAlignmentOwned112(alignment.data()),"exact Henry attacker-position record permits late alignment gate");
{ // R112: real logs (R106/R111) answer true with NO records yet; an empty vector no longer blocks the veto
    namespace ar=kcd2vr::align24;std::uint8_t why=ar::None;
    put(alignment.data()+0x60,reinterpret_cast<std::uintptr_t>(record.data()));
    ck(NativeAlignmentOwned112(alignment.data(),&why)&&why==ar::None,"R112 empty record vector (begin==end) permits the ordinary no-step veto");
    put(alignment.data()+0x58,std::uintptr_t(0));put(alignment.data()+0x60,std::uintptr_t(0));put(alignment.data()+0x68,std::uintptr_t(0));
    ck(NativeAlignmentOwned112(alignment.data()),"R112 never-allocated vector (all null) permits the ordinary no-step veto");
    put(alignment.data()+0x58,reinterpret_cast<std::uintptr_t>(record.data()+0x20));put(alignment.data()+0x60,reinterpret_cast<std::uintptr_t>(record.data()));
    put(alignment.data()+0x68,reinterpret_cast<std::uintptr_t>(record.data()+record.size()));why=ar::None;
    ck(!NativeAlignmentOwned112(alignment.data(),&why)&&why==ar::RecordVector,"R112 end<begin still fails closed");
    put(alignment.data()+0x58,reinterpret_cast<std::uintptr_t>(record.data()));put(alignment.data()+0x60,reinterpret_cast<std::uintptr_t>(record.data()+0x10));why=ar::None;
    ck(!NativeAlignmentOwned112(alignment.data(),&why)&&why==ar::RecordVector,"R112 partial record still fails closed");
    put(alignment.data()+0x60,reinterpret_cast<std::uintptr_t>(record.data())); put(state.data()+0x218,5);
    ck(!NativeAlignmentOwned112(alignment.data()),"R112 empty vector: special action (kick/masterstrike) keeps native alignment");put(state.data()+0x218,1);
    animation_filter::nativeBodyOwned112=true;ck(!NativeAlignmentOwned112(alignment.data()),"R112 empty vector: paired/native body keeps native alignment");animation_filter::nativeBodyOwned112=false;
    put(alignment.data()+0x60,reinterpret_cast<std::uintptr_t>(record.data()+0x20));
}
ck(!NativeAlignmentResult112(alignment.data(),base+0x9E2D55,true),"qualified late query overrides true while native caller still owns release");
ck(NativeAlignmentResult112(alignment.data(),base+0x9E2D56,true),"unknown query caller remains native");
ck(!NativeAlignmentResult112(alignment.data(),base+0x9E2D55,false),"native false is preserved");
for(auto pair:std::array<std::pair<unsigned char,std::uintptr_t>,2>{{{1,0x3A62098},{3,0x3A622C8}}}){
    put(record.data()+0x10,pair.first);put(clip.data(),base+pair.second);
    ck(NativeAlignmentOwned112(alignment.data()),"combined-parent and rotation types require their own exact clip identity");
}
put(record.data()+0x10,std::uint8_t(2));put(clip.data(),base+0x3BAD328);
put(record.data()+0xC,std::uint32_t(456));ck(!NativeAlignmentOwned112(alignment.data()),"NPC-target registration keeps complete native alignment");put(record.data()+0xC,std::uint32_t(123));
put(record.data()+0x10,std::uint8_t(4));ck(!NativeAlignmentOwned112(alignment.data()),"unrelated alignment type is never suppressed");put(record.data()+0x10,std::uint8_t(2));
put(clip.data(),base+0x3A622C8);ck(!NativeAlignmentOwned112(alignment.data()),"type/vtable disagreement fails closed");put(clip.data(),base+0x3BAD328);
put(actor.data()+0x250,static_cast<void*>(nullptr));ck(!NativeAlignmentOwned112(alignment.data()),"manager actor backlink is mandatory");put(actor.data()+0x250,alignment.data());
put(state.data()+0x218,5);ck(!NativeAlignmentOwned112(alignment.data()),"special/fist action keeps native alignment");put(state.data()+0x218,1);
animation_filter::nativeBodyOwned112=true;ck(!NativeAlignmentOwned112(alignment.data()),"hit reaction or paired native body revokes alignment override");animation_filter::nativeBodyOwned112=false;
g_ctx_cutscene=true;ck(!NativeAlignmentOwned112(alignment.data()),"cutscene never acquires alignment");g_ctx_cutscene=false;
hostRetailVerified112=false;ck(!NativeAlignmentOwned112(alignment.data()),"matching timestamp without verified SHA cannot activate new native writes");hostRetailVerified112=true;
diagnostics::lab::Set(diagnostics::lab::Key::F3,2);ck(!NativeAlignmentOwned112(alignment.data()),"F3=2 remains native at alignment boundary");diagnostics::lab::Set(diagnostics::lab::Key::F3,3);
{ // C25 receipts: refusal names report the first failing condition; decisions are unchanged.
    namespace ar=kcd2vr::align24;std::uint8_t why=ar::None;
    ck(NativeAlignmentOwned112(alignment.data(),&why)&&why==ar::None,"C25 owned query leaves no refusal reason");
    put(record.data()+0xC,std::uint32_t(456));why=ar::None;
    ck(!NativeAlignmentOwned112(alignment.data(),&why)&&why==ar::RecordTarget,"C25 NPC-target record names RECORD_TARGET");put(record.data()+0xC,std::uint32_t(123));
    put(record.data()+0x10,std::uint8_t(4));why=ar::None;
    ck(!NativeAlignmentOwned112(alignment.data(),&why)&&why==ar::RecordType,"C25 unrelated type names RECORD_TYPE");put(record.data()+0x10,std::uint8_t(2));
    put(clip.data(),base+0x3A622C8);why=ar::None;
    ck(!NativeAlignmentOwned112(alignment.data(),&why)&&why==ar::RecordVtable,"C25 type/vtable disagreement names RECORD_VTABLE");put(clip.data(),base+0x3BAD328);
    put(state.data()+0x218,5);why=ar::None;
    ck(!NativeAlignmentOwned112(alignment.data(),&why)&&why==ar::ActionType,"C25 special action names ACTION_TYPE");put(state.data()+0x218,1);
    g_ctx_cutscene=true;why=ar::None;
    ck(!NativeAlignmentOwned112(alignment.data(),&why)&&why==ar::GameContext,"C25 cutscene names GAME_CONTEXT");g_ctx_cutscene=false;
    diagnostics::lab::Set(diagnostics::lab::Key::F3,2);why=ar::None;
    ck(!NativeAlignmentOwned112(alignment.data(),&why)&&why==ar::Mode,"C25 F3=2 names MODE");diagnostics::lab::Set(diagnostics::lab::Key::F3,3);
    const auto skips=g_alignment_skips112.load();
    ck(!NativeAlignmentResult112(alignment.data(),base+0x9E2D55,true)&&g_alignment_skips112.load()==skips+1,"C25 veto still answers false and counts once");
    ck(NativeAlignmentResult112(alignment.data(),base+0x9E2D56,true)&&g_alignment_skips112.load()==skips+1,"C25 foreign caller stays native and uncounted");
    // F3 combat capture: receipts record Henry's answers per attack, edge-only, without changing them.
    namespace cap=diagnostics::capture168;
    cap::gate.select(cap::Domain::Combat,true,hostTickCount64);alignTraceLines25.clear();g_align_ledger25.Discard();
    const auto t0=g_align_ledger25.totals();const auto consumed=g_attack_timing_consumed22.load();
    g_attack_timing_consumed22=consumed+1000;
    ck(!NativeAlignmentResult112(alignment.data(),base+0x9E2D55,true),"C25 receipts on: veto answer unchanged");
    put(record.data()+0x10,std::uint8_t(4));
    ck(NativeAlignmentResult112(alignment.data(),base+0x9E2D55,true),"C25 receipts on: refusal answer unchanged");
    put(record.data()+0x10,std::uint8_t(2));
    ck(!NativeAlignmentResult112(alignment.data(),base+0x9E2D55,false),"C25 receipts on: native false unchanged");
    ServiceAlignVetoReceipts25();ck(alignTraceLines25.empty(),"C25 no line while the attack is still live");
    g_attack_timing_consumed22=consumed+1001; // the next hidden attack closes the previous one
    (void)NativeAlignmentResult112(alignment.data(),base+0x9E2D55,true);
    ServiceAlignVetoReceipts25();
    const auto t1=g_align_ledger25.totals();
    ck(alignTraceLines25.size()==1&&alignTraceLines25[0].find("calls=3 trueAnswers=2 vetoed=1 refused=1 firstRefusal=RECORD_TYPE")!=std::string::npos&&
       alignTraceLines25[0].find("unexpectedRecords=1 unexpectedType=4")!=std::string::npos&&alignTraceLines25[0].find("decision=UNCHANGED")!=std::string::npos,
       "C25 one edge line per ended attack with first refusal and unexpected record");
    ck(t1.calls==t0.calls+4&&t1.vetoed==t0.vetoed+2&&t1.refused==t0.refused+1,"C25 totals follow Henry's queries");
    put(alignment.data()+8,entity.data());const auto t2=g_align_ledger25.totals().calls;
    (void)NativeAlignmentResult112(alignment.data(),base+0x9E2D55,true);
    ck(g_align_ledger25.totals().calls==t2,"C25 non-Henry managers are not receipts");put(alignment.data()+8,actor.data());
    cap::gate.select(cap::Domain::Combat,false,hostTickCount64);ServiceAlignVetoReceipts25();
    ck(!g_align_ledger25.pending()&&!g_align_ledger25.current().open,"C25 capture end discards pending edges");
    (void)NativeAlignmentResult112(alignment.data(),base+0x9E2D55,true);
    ck(g_align_ledger25.totals().calls==t2,"C25 receipts are off without the F3 capture lease");
    g_attack_timing_consumed22=consumed;alignTraceLines25.clear();
}

put(producer.data()+8,actor.data());put(producer.data()+0x18,motionAction.data());put(producer.data()+0x4C,std::uint8_t(1));
put(motionAction.data()+0xC,std::int16_t(17));put(motionAction.data()+0x40C,7); // no guessed 0/1 hand mapping
QuatT28 nativePrevious{{0,0,0,1},{0,0,0}},nativeCurrent{{0,0,0,1},{0,1,0}},vp{},vc{};
put(producer.data()+0x30,nativePrevious);
const auto previousPointer=reinterpret_cast<const QuatT28*>(producer.data()+0x30);
PhysSocketBank101 sockets{};auto& socket=sockets.sockets[0];
socket.valid=true;socket.key=frame.profiles[0].key;socket.generation=frame.profiles[0].generation;
socket.character=character.data();socket.joint=17;socket.callback=77;socket.relative={{0,0,0,1},{0,0,0}};
PhysGripBank6698 grips{};auto& captured=grips.grip[0];captured.key=socket.key;captured.generation=socket.generation;
CommittedFrame6695 body{};body.valid[0]=true;
auto motionFrame=[&](bool advance){
    if(advance){hostTickCount64+=11;++frame.input.sample;frame.output.weapons[0].p.x+=.02;}
    frame.input.tick=hostTickCount64;socket.tick=hostTickCount64;
    body.stamp={frame.input.bodyEpoch,frame.input.sample,frame.input.tick,reinterpret_cast<std::uintptr_t>(character.data())};
    captured.nativeStamp=body.stamp;frame.correlation126[0]={body.stamp,socket};g_phys_grips6698.publish(grips);g_phys_sockets101.publish(sockets);
    g_phys_frame6698.publish(frame);g_committed_frame6695.publish(body);animation_filter::combatPoseTick112=hostTickCount64;
};
auto replace=[&](std::uintptr_t caller=0){return NativeTrajectoryPrepare112(caller?caller:base+0xA7EB58,collision.data(),7,17,previousPointer,&nativeCurrent,.011f,vp,vc);};
motionFrame(false);ck(!replace(),"first pivot match is observation only");
motionFrame(true);ck(!replace(),"second distinct pivot match is still observation only");
motionFrame(true);ck(replace()&&std::abs(vc.t[0]-vp.t[0]-.02f)<1.e-5,"third coherent pivot match submits consecutive allowed VR motion");
ck(replace()&&!std::memcmp(&vp,&vc,sizeof(vp)),"duplicate native callback cannot replay the last VR segment");
ck(!replace(base+0xA7C9C1)&&!replace(base+0x27C1695),"authored prediction subdivisions remain native");
ck(!std::memcmp(previousPointer,&nativePrevious,sizeof(nativePrevious)),"borrowed native previous-pose memory is never modified");
++captured.nativeStamp.sample;g_phys_grips6698.publish(grips);
ck(replace(),"F3 live-bone path also uses committed capture rather than a newer independent grip mailbox");
socket.relative.t[0]=.02f;g_phys_sockets101.publish(sockets);
ck(!replace(),"F3 live-bone path rejects changed live socket topology from the same attachment callback");
socket.relative.t[0]=0;socket.callback=77;g_phys_sockets101.publish(sockets);
motionFrame(true);replace();motionFrame(true);replace();motionFrame(true);ck(replace(),"F3 path reacquires after genuine topology rejection");
// Do not publish a new committed frame: only the asynchronous attachment mailbox
// advances. Its newer callback must not invalidate the already coherent capture.
socket.relative.t[0]=.02f;socket.callback=78;g_phys_sockets101.publish(sockets);
ck(replace(),"F3 live-bone path ignores topology from a newer attachment callback");
socket.callback=77;socket.relative.t[0]=0;g_phys_sockets101.publish(sockets);
nativeCurrent.t[0]=.1f;ck(!replace(),"uncorrelated native collision pivot resets trajectory lease");nativeCurrent.t[0]=0;
motionFrame(true);ck(!replace(),"pivot mismatch requires complete reacquisition");
motionFrame(true);ck(!replace(),"reacquisition cannot skip warmup");motionFrame(true);ck(replace(),"reacquisition succeeds only with three fresh matches");
socket.joint=18;motionFrame(true);ck(!replace(),"different collision bone cannot borrow weapon socket transform");socket.joint=17;
motionFrame(true);replace();motionFrame(true);replace();motionFrame(true);replace();
g_right_valid=false;ck(!replace(),"tracking loss immediately clears trajectory lease");g_right_valid=true;
motionFrame(true);ck(!replace(),"tracking recovery cannot resume stale trajectory");
motionFrame(true);replace();motionFrame(true);replace();
diagnostics::lab::Set(diagnostics::lab::Key::F3,2);diagnostics::lab::Set(diagnostics::lab::Key::F3,3);
motionFrame(true);ck(!replace(),"mode cycle between native callbacks revokes prior trajectory via lab epoch");
put(motionAction.data()+0x308,std::uint8_t(1));ck(!replace(),"native already-hit flag cannot be bypassed");put(motionAction.data()+0x308,std::uint8_t(0));
put(producer.data()+8,entity.data());ck(!replace(),"foreign producer cannot substitute Henry collision motion");put(producer.data()+8,actor.data());
motionFrame(true);replace();motionFrame(true);replace();motionFrame(true);replace();
hostTickCount64+=81;ck(!replace(),"stale render packet cannot feed native hit sweep");
hostRetailVerified112=false;g_motion_contract112=0;
