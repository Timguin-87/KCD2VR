struct InjectResult35 {std::uint64_t tick=0;bool seen=false,ok=false,post=false,fields=false;float dmg=0,stam=0;int death=-1;
    std::int32_t blocked=-1,perfect=-1,parry=-1,master=-1;std::uint32_t target=0;
    int nativePart=-1,material=-1,stabbed=-1,combo=-1,comboEnd=-1;int process=-1,reactionCreated=-1,reactionKind=-1,dispatch=-1;};
void DecodeNativeHitResult59c7g(InjectResult35& r,void* hitInfo,void* details,bool ok,bool post) noexcept {
    r.tick=std::uint64_t(GetTickCount64());r.seen=true;r.ok=ok;r.post=post;
    // False native results need not initialize output structures. The compact
    // R35 logger also mislabeled blocked/perfect/master as parry/blocked/perfect.
    r.fields=false;r.blocked=r.perfect=r.parry=r.master=r.death=-1;r.dmg=r.stam=0;r.target=0;
    r.nativePart=r.material=r.stabbed=r.combo=r.comboEnd=-1;
    if(!ok||!hitInfo||!details)return;r.fields=true;
    r.nativePart=Rd<int>(hitInfo,0x3C,-1);r.material=Rd<int>(hitInfo,0x38,-1);r.stabbed=Rd<std::uint8_t>(hitInfo,0x74,0);
    r.combo=Rd<std::uint8_t>(details,4,0);r.comboEnd=Rd<std::uint8_t>(details,5,0);
    r.dmg=Rd<float>(hitInfo,0x30,0.f);r.stam=Rd<float>(hitInfo,0x2C,0.f);r.death=Rd<std::uint8_t>(hitInfo,0x40,0);r.target=Rd<std::uint32_t>(hitInfo,0x28,0u);
    r.parry=Rd<std::uint8_t>(details,0,0);r.blocked=Rd<std::uint8_t>(details,1,0);r.perfect=Rd<std::uint8_t>(details,3,0);r.master=Rd<std::uint8_t>(details,7,0);}
