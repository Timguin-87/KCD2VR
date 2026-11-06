// C49 BLOODY FIGHTS native side. Included in frik_pose_probe's private namespace right after gore32.inl (needs gore32n::Blood /
// ToModel / ToWorld / Rot / g_lua_exec43, FlinchEntityById178r11, CharacterForEntity6689, ReadModelWorld6692). Native game thread
// only, from the native frame (Tick right after gore32n::Tick). Idle unless a qualified hit by Henry or a kill move just happened:
// two mailbox reads per frame, nothing else. Pure rules: vrik/blood49.h.
namespace blood49n {
namespace b=kcd2vr::blood49;namespace g=kcd2vr::gore32;namespace d44=kcd2vr::decap44;
static b::Once<16> g_once;static b::Gushes<3> g_gushes;
static std::atomic<bool> g_no_lua{false},g_no_fx{false};
// fail closed, said once per session: which route is missing
static void SayOnce(std::atomic<bool>& flag,const char* line) noexcept {if(!flag.exchange(true))diagnostic_trace::Write(line);}
static bool F3(std::uint64_t now) noexcept {return diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now);}
// the game's MaterialEffects service as gore32n::Blood uses it (pinned there again on every call)
static bool FxReady() noexcept {
    const auto base=WhBase();const bool ok=retail_build112::Ready(base)&&g_material_contract116.load()==base&&!g_contact_fx_fault178r12.load();
    if(!ok)SayOnce(g_no_fx,"C49 BLOOD route=NO_MATERIAL_EFFECTS (material service not armed, pin mismatch or an earlier fault): no extra blood particles this session");
    return ok;}
// the struck NPC, still the same entity as in the receipt, never Henry
static void* Victim(std::uint32_t target,std::uintptr_t identity) noexcept {
    void* e=FlinchEntityById178r11(target);
    if(!e||e==g_last_player_entity.load(std::memory_order_acquire)||reinterpret_cast<std::uintptr_t>(e)!=identity)return nullptr;return e;}
// out of the wound towards Henry and a bit up: the side the blood comes out of
static g::V3 TowardHenry(g::V3 p) noexcept {
    void* henry=g_last_player_entity.load(std::memory_order_acquire);void* hci=henry?CharacterForEntity6689(henry):nullptr;ModelWorld6692 hw{};
    g::V3 d{0,0,1};
    if(hci&&ReadModelWorld6692(hci,hw)){d={hw.t.x-p.x,hw.t.y-p.y,0};const double l=g::len(d);d=l>1e-3?d*(1/l):g::V3{};d.z=.35;}
    const double l=g::len(d);return l>1e-6&&g::finite(d)?d*(1/l):g::V3{0,0,1};}
// the game's own clothes/skin blood on one zone (entity.actor:AddBlood, exactly as the game's DeadBody scripts call it)
static bool LuaAddBlood(std::uint32_t target,const char* zone,float amount) noexcept {
    auto exec=gore32n::g_lua_exec43.load(std::memory_order_acquire);
    if(!exec){SayOnce(g_no_lua,"C49 BLOOD route=NO_LUA (C47 ExecuteBuffer pins not OK): no extra clothes blood / neck gush attachment this session");return false;}
    if(!zone||!std::isfinite(amount)||amount<=0||!gore32n::EligibleTargetD56(target,std::uint64_t(GetTickCount64())))return false;
    char lua[300]{};std::snprintf(lua,sizeof(lua),"local v=System.GetEntity(%u) if v and v.actor then pcall(function() v.actor:AddBlood(%.2f,'%s') end) end",target,double(amount),zone);
    return exec(lua);}
// ------------------------------------------------------------ 2x blood on a hit
static void OnHit(const b::Hit& h,std::uint64_t now) noexcept {
    const char* row=b::row(h.weaponClass,h.kind==1);const char* z=b::zone(h.bodyMaterial);const float amount=b::extraAmount(h.energy);
    bool fx=false,cloth=false;const char* why="OK";
    if(!Victim(h.target,h.identity))why="VICTIM_CHANGED_OR_HENRY";
    else if(!row)why="NO_BLADE_ROW";
    else{
        const g::V3 p{h.point[0],h.point[1],h.point[2]};const g::V3 n=TowardHenry(p);
        const float nrm[3]{float(n.x),float(n.y),float(n.z)};
        if(FxReady())fx=gore32n::Blood(h.point,nrm,h.target,row,1.0f);
        if(z)cloth=LuaAddBlood(h.target,z,amount);
    }
    if(h.death==1&&row&&Victim(h.target,h.identity)){ // a killing hit by Henry with a blade/axe/mace/polearm: the gush (an impale latch / a head coming off upgrades it)
        const double p[3]{h.point[0],h.point[1],h.point[2]};const g::V3 n=TowardHenry({p[0],p[1],p[2]});const double d[3]{n.x,n.y,n.z};
        (void)b::offerKill(b::Kill::Lethal,h.target,h.identity,p,d,h.bodyMaterial,now);}
    if(!F3(now))return;
    char m[360]{};std::snprintf(m,sizeof(m),"C49 BLOOD hit=%llu target=%u kind=%s weaponClass=%d dmg=%.1f death=%d extra=%s%s%s row=%s zone=%s amount=%.2f route=MATERIAL_EFFECTS%s reason=%s",
        (unsigned long long)h.serial,h.target,h.kind==1?"THRUST":"CUT",h.weaponClass,double(h.damage),h.death,fx?"PARTICLE":"",fx&&cloth?"+":"",cloth?"CLOTH":(fx?"":"NONE"),
        row?row:"NONE",z?z:"UNKNOWN",double(amount),gore32n::g_lua_exec43.load()?"+LUA_ADDBLOOD":"(no lua)",why);
    diagnostic_trace::Write(m);
}
// ------------------------------------------------------------ kill-move gush
// C59: a successful queued Lua call / SetAttachmentEffect is not a renderer
// acknowledgment for a corpse-neck attachment. The headset showed floating
// caps/effects. Keep the current exact named-neck pulse route below; do not
// report an attachment or create a new unproven bone effect as a substitute.
static bool NeckAttachment(std::uint32_t) noexcept {return false;}
static void StartGush(const b::KillEvent& k,std::uint64_t now) noexcept {
    void* e=Victim(k.target,k.identity);void* ci=e?CharacterForEntity6689(e):nullptr;
    b::Gush in{};in.target=k.target;in.identity=k.identity;in.kill=k.kill;in.bodyMaterial=k.bodyMaterial;
    bool anchored=false;const char* frame="NONE(EXACT_ANCHOR_REQUIRED)";
    if(e&&ci&&k.kill==b::Kill::Decap){
        gore32n::NeckFrame55 neck{};
        if(gore32n::ReadNeckFrame55(ci,neck)){
            in.character55=reinterpret_cast<std::uintptr_t>(ci);in.skeleton55=reinterpret_cast<std::uintptr_t>(neck.skeleton);in.neckJoint55=neck.joint;
            anchored=true;frame="CURRENT_NAMED_NECK";
        }
    }else if(e&&k.kill==b::Kill::Impale){
        b::PartWound55 wound{};
        if(b::partWound55.read(wound)&&b::freshPart55(wound,k.target,k.identity,0,now,g_pose_epoch6689.load(),g_weapon_request_epoch6694.load())){
            in.anchorSerial55=wound.serial;anchored=true;frame="EXACT_STRUCK_PART";
        }
    }
    // MODEL root is not a ragdoll wound. Ordinary lethal hits keep their exact
    // contact burst; repeated fountains need a current named bone or the already
    // accepted original collider/raw-part anchor. No stationary WORLD fallback.
    b::Gush* slot=nullptr;const auto st=anchored?g_gushes.start(in,now,slot):b::Start::Refused;
    bool neck=false,cloth=false;
    if(e&&st!=b::Start::Duplicate){
        if(k.kill==b::Kill::Decap)neck=NeckAttachment(k.target);
        const char* z=k.kill==b::Kill::Decap?"head":b::zone(k.bodyMaterial);if(!z&&k.kill==b::Kill::Impale)z="torso";if(z)cloth=LuaAddBlood(k.target,z,.6f);
    }
    if(!F3(now)||st==b::Start::Duplicate)return;
    char m[360]{};std::snprintf(m,sizeof(m),"C55 BLOOD_GUSH kill=%s target=%u ok=%d start=%s frame=%s pulses=%d overMs=%llu neckAttachment=%d cloth=%d route=MATERIAL_EFFECTS%s",
        b::name(k.kill),k.target,(st==b::Start::Started||st==b::Start::Upgraded)?1:0,b::name(st),frame,b::kPulses,
        (unsigned long long)b::kGushMs,neck?1:0,cloth?1:0,gore32n::g_lua_exec43.load()?"+LUA":"(no lua)");
    diagnostic_trace::Write(m);
}
static void TickGushes(std::uint64_t now) noexcept {
    if(!g_gushes.any())return;
    for(auto& s:g_gushes.slot){
        if(!s.active)continue;
        if(b::Gushes<3>::done(s,now)){
            if(F3(now)){char m[200]{};std::snprintf(m,sizeof(m),"C49 BLOOD_GUSH_DONE kill=%s target=%u pulses=%d okPulses=%d",b::name(s.kill),s.target,s.pulses,s.okPulses);diagnostic_trace::Write(m);}
            s={};continue;}
        if(!b::Gushes<3>::due(s,now))continue;
        void* e=Victim(s.target,s.identity);void* ci=e?CharacterForEntity6689(e):nullptr;
        if(!e){s={};continue;} // gone / changed: stop quietly
        g::V3 p{},d{};bool current55=false;
        if(s.kill==b::Kill::Decap&&ci&&reinterpret_cast<std::uintptr_t>(ci)==s.character55){
            gore32n::NeckFrame55 neck{};
            if(gore32n::ReadNeckFrame55(ci,neck)&&reinterpret_cast<std::uintptr_t>(neck.skeleton)==s.skeleton55&&neck.joint==s.neckJoint55){
                p=neck.point;d=neck.up;current55=true;
            }
        }else if(s.kill==b::Kill::Impale&&s.anchorSerial55){
            b::PartWound55 wound{};
            if(b::partWound55.read(wound)&&b::freshPart55(wound,s.target,s.identity,s.anchorSerial55,now,g_pose_epoch6689.load(),g_weapon_request_epoch6694.load())){
                p={wound.point[0],wound.point[1],wound.point[2]};d={wound.dir[0],wound.dir[1],wound.dir[2]};current55=true;
            }
        }
        if(!current55){if(F3(now))diagnostic_trace::Write("C55 BLOOD_GUSH stopped reason=EXACT_ANCHOR_LOST worldRootFallback=REFUSED");s={};continue;}
        const double dl=g::len(d);d=dl>1e-6?d*(1/dl):g::V3{0,0,1};
        const float pos[3]{float(p.x),float(p.y),float(p.z)},nrm[3]{float(d.x),float(d.y),float(d.z)};
        const int i=s.pulses++;
        if(FxReady()&&gore32n::Blood(pos,nrm,s.target,i&1?"hit_passed_sword_stab_player":"hit_passed_sword_slash_player",b::kPulseStrength[i]))++s.okPulses;
    }
}
static void Tick(std::uint64_t now) noexcept {
    b::Hit h{};
    if(b::hits.read(h)&&g_once.take(h.serial)&&now>=h.tick&&now-h.tick<=500&&b::on.load(std::memory_order_relaxed))OnHit(h,now);
    b::KillEvent k{};
    while(b::kills.pop(k))if(now>=k.tick&&now-k.tick<=2000&&b::on.load(std::memory_order_relaxed))StartGush(k,now); // the LETHAL one above first, then impale/decap upgrades
    if(!b::on.load(std::memory_order_relaxed)){for(auto& s:g_gushes.slot)s={};return;}
    TickGushes(now);
}
} // namespace blood49n
