static void BowCaptureCharacter128(void* ent) noexcept {
    const auto base=WhBase();std::uintptr_t vt=0;void* fn=nullptr;
    if(!ent||!SupportedWhBuild(base)||!Read(ent,vt)||vt!=base+kCEntityVftRva||!Read(reinterpret_cast<unsigned char*>(vt)+0x328,fn)||fn!=reinterpret_cast<void*>(base+kCEntityGetCharacterRva))return;
    void* ci=CharacterForEntity6689(ent);void* anim=nullptr;if(!ci||!ValidateCharInstance(ci,anim))return;
    if(g_bow_char128.exchange(ci,std::memory_order_acq_rel)!=ci){char m[192]{};std::snprintf(m,sizeof(m),"V66.128-BOW CHARACTER bowEntity=%p bowChar=%p source=EQUIPMENT_QUALIFIED_HELD_BOW",ent,ci);diagnostic_trace::Write(m);}
    const auto now=std::uint64_t(GetTickCount64());
    g_bow_identity166.publish(BowCharacterIdentity166{ci,ent,now});
    g_bow_char_tick128.store(now,std::memory_order_release);
}
