static bool ConsumeNativeAnswer19c4r2(const Pending& p,const void* henryActor,const void* target,std::uint64_t now) noexcept {
        const auto answer=g_answers19c4r2.query(reinterpret_cast<std::uintptr_t>(henryActor),reinterpret_cast<std::uintptr_t>(target),p.release51,now);
        if(answer.match==kcd2vr::answer59c4r2::Match::Same||answer.match==kcd2vr::answer59c4r2::Match::Unknown){
            kcd2vr::hybrid_serial59::Reservation answerReservation59;
            if(!answerReservation59.reserve(p.release51,now)){
                const auto claimed=kcd2vr::contact176::claimedSerial.load(std::memory_order_acquire);
                // C56 review (Claude): this terminal exit (contact dropped as "answered") used to be silent.
                g_drop_serial_claimed56.fetch_add(1,std::memory_order_relaxed);
                if(Allow(4,4)){char m[300]{};std::snprintf(m,sizeof(m),"C56 VR_HIT_ANSWERED_BY_NPC serial=%u target=%u release=%llu answer=%s claimed=%llu match=%s gesture=%llu ageMs=%llu delivered=0",
                    p.serial,p.entityId,(unsigned long long)p.lease47c2.serial,kcd2vr::hybrid_serial59::name(answerReservation59.result()),(unsigned long long)claimed,answer.match==kcd2vr::answer59c4r2::Match::Same?"SAME":"UNKNOWN",
                    (unsigned long long)p.gesture59c3,(unsigned long long)(now-p.tick));diagnostic_trace::Write(m);}
                return true;}
            if(!answerReservation59.tryEnter())return true; // already applied: conservative no-redelivery, no new accepted feedback
            g_answered_skips59c17.fetch_add(1,std::memory_order_relaxed);
            const int answered=answer.kind;const bool known=answer.match==kcd2vr::answer59c4r2::Match::Same;
            if(known&&answered!=5)frik_pose_probe::NotifyNativeWeaponImpact136R2(0.f,0.f,std::clamp(p.energy,.05f,1.f),answered!=4,answered==3,answered==4,p.entityId);
            if(Allow(4,8)){char m[420]{};std::snprintf(m,sizeof(m),"R59.16C10R17 VR_HIT_ANSWERED_BY_NPC serial=%u target=%u release=%llu answer=%s lineage=%s ageMs=%llu delivered=0 nativePart=%d nativeMaterial=%d nativeProducer=%s answerAgeMs=%llu ourPart=%d ourMaterial=%d (receipt only: material/part never decide)",
                p.serial,p.entityId,(unsigned long long)p.release51.serial,!known?"UNRESOLVED_APPLICATION_NO_REDELIVERY":answered==5?"NATIVE_APPLICATION":answered==4?"HENRY_NATIVE_MASTER_STRIKE":answered==3?"PERFECT_BLOCK":answered==2?"PARRY_IN_PLACE":"BLOCK",known?"EXACT_ADMITTED_RELEASE":"UNKNOWN_CONSERVATIVE",(unsigned long long)(now-p.tick),
                answer.part,answer.material,answer.callerRva==0xD536BCull?"COLLISION":answer.callerRva==0x173BF58ull?"FOLLOWUP_C":answer.callerRva?"OTHER":"UNKNOWN",(unsigned long long)(answer.tick&&now>=answer.tick?now-answer.tick:0),p.part,p.bodyMaterial);diagnostic_trace::Write(m);}
            return true;}
    return false;
}
