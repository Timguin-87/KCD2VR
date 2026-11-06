// D43 ELBOW / KNEE DISMEMBERMENT - native skin route (Claude, 28 Oct 2026).
// Included by native_gore.inl inside namespace gore32n, after the attachment helpers (AttKind, AttachmentName,
// HeadSkinPath43, HideHeadLease59) and before StartLimb50.
//
// Why native: KCD2 dresses a human as many SKIN attachments (body_arms, body_hands, arms clothing, gloves, legs, boots ...).
// One forearm is part of several of them; the engine can only hide a WHOLE attachment and Lua cannot bind a skin file
// (CreateSkinAttachment ignores the path, v60 finding). So the native side does exactly what CryAnimation does for a
// CA_SKIN line of a .cdf (WHGame 6A350E20, CDF attachment loader around +5AE5EF):
//   1. CAttachmentManager::CreateAttachment(name, CA_SKIN=2, joint=null, project=false)  mgr vt 0x4060548 +0x10 = +20ED500
//   2. g_pCharacterManager (+5189BE0, vt 0x4061730) +0x30 LoadModelSKIN(&smart_ptr, path, flags=0) = +977B64
//   3. new CSKINAttachment: CryModuleMalloc(0x40) +4F75C0, ctor +5ADAC4 (vt 0x3A3F158), m_pIAttachmentSkin = att+8 via
//      the _smart_ptr assign +5ADA88 (AddRef on IAttachmentSkin)
//   4. replacement materials copied from the ORIGINAL layer's CSKINAttachment: GetReplacementMaterial vt+0x80 (+4A34E0),
//      SetReplacementMaterial vt+0x78 (+5AF4EC), lods 0..5 (the 6 smart pointers the ctor builds at +0x10)
//   5. CAttachmentSKIN::AddBinding(obj, skin, 0) vt 0x3A65A08 +0xD0 = +976B4C (it AddRefs the skin, patches the bone
//      remapping for this skeleton and stores obj at +0x28); then our LoadModelSKIN reference is released (CSkin vt
//      0x406C9F0 +0x08 = +9780E8)
//   6. the original layer is hidden through the existing C59 visibility owner (HideHeadLease59: exact entity/character/
//      attachment identity, restored on world load/shutdown edges) and re-hidden for 10 s if an equipment refresh shows it.
// The original attachment and its CSKINAttachment are never modified or released (the clothing system keeps owning them).
// Only on a native-PROVEN dead victim (corpse reported dead by the game, or the native lethal receipt): a living NPC is
// never cut. Runs on the game thread (gore32n::Tick under the native frame), every call SEH guarded, every code address
// byte-pinned below. INI VrDismembermentLevel (read once): 1 = elbow/knee (default), 0 = the old C50 wrist/ankle event only.
namespace d43 {
constexpr std::uintptr_t kSkinAttVt=0x3A65A08,kAddBinding=0x976B4C,kGetObject=0x113CE50,kSetFlags=0x1A72DB0,kGetName=0x9DD2D0,
    kSkinObjVt=0x3A3F158,kSkinObjType=0x1A72CF0,kSkinObjCtor=0x5ADAC4,kSetMtl=0x5AF4EC,kGetMtl=0x4A34E0,kAssignSkinIface=0x5ADA88,
    kCryMalloc=0x4F75C0,kCharMgrGlobal=0x5189BE0,kCharMgrVt=0x4061730,kLoadSkin=0x977B64,kSkinVt=0x406C9F0,kSkinRelease=0x9780E8,
    kMgrVt=0x4060548,kCreateAttachment=0x20ED500;
struct Pin {std::uintptr_t rva;unsigned char n;unsigned char b[16];};
inline constexpr Pin kPins[]{
    {kAddBinding,16,{0x48,0x8b,0xc4,0x48,0x89,0x58,0x08,0x48,0x89,0x68,0x10,0x48,0x89,0x70,0x18,0x48}},
    {kGetObject,5,{0x48,0x8b,0x41,0x28,0xc3}},
    {kSetFlags,4,{0x89,0x51,0x10,0xc3}},
    {kSkinObjType,6,{0xb8,0x03,0x00,0x00,0x00,0xc3}},
    {kSkinObjCtor,16,{0x40,0x53,0x48,0x83,0xec,0x20,0xba,0x08,0x00,0x00,0x00,0x48,0xc7,0x41,0x08,0x00}},
    {kSetMtl,12,{0x41,0x8b,0xc0,0x48,0x8d,0x0c,0xc1,0x48,0x83,0xc1,0x10,0xe9}},
    {kGetMtl,8,{0x8b,0xc2,0x48,0x8b,0x44,0xc1,0x10,0xc3}},
    {kAssignSkinIface,16,{0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x20,0x48,0x8b,0x19,0x48,0x8b,0xf9}},
    {kCryMalloc,16,{0x40,0x53,0x48,0x83,0xec,0x20,0x83,0x3d,0xa3,0x5d,0xfa,0x04,0x00,0x48,0x8b,0xd9}},
    {kLoadSkin,16,{0x40,0x55,0x53,0x56,0x57,0x48,0x8d,0xac,0x24,0x98,0xfe,0xff,0xff,0x48,0x81,0xec}},
    {kSkinRelease,16,{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x20,0x48}},
    {kCreateAttachment,16,{0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x6c,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x57}}};
// vtable slot -> function (all read from the retail image, never written)
struct Slot {std::uintptr_t vt;unsigned off;std::uintptr_t fn;};
inline constexpr Slot kSlots[]{
    {kSkinAttVt,0x10,kGetName},{kSkinAttVt,0x38,kSetFlags},{kSkinAttVt,0xD0,kAddBinding},{kSkinAttVt,0xE0,kGetObject},
    {kSkinObjVt,0x08,kSkinObjType},{kSkinObjVt,0x78,kSetMtl},{kSkinObjVt,0x80,kGetMtl},
    {kCharMgrVt,0x30,kLoadSkin},{kSkinVt,0x08,kSkinRelease},{kMgrVt,0x10,kCreateAttachment}};
enum class R : int {Ok=0,Pins,NotSkinLayer,NoObject,Exists,NoSkinFile,CreateFailed,AllocFailed,BindFailed,Fault};
inline const char* name(R r) noexcept {
    constexpr const char* n[]{"OK","PINS","NOT_SKIN_LAYER","NO_OBJECT","ALREADY_CUT","NO_CUT_SKIN_IN_PAK","CREATE_FAILED","ALLOC_FAILED","BIND_FAILED","FAULT"};
    return n[unsigned(r)<sizeof(n)/sizeof(n[0])?unsigned(r):0];}
} // namespace d43

static bool CutPins43() noexcept {
    static int v=0;if(v)return v>0;const auto b=WhBase();if(!b||!Pins()){return false;}
    bool ok=true;
    for(const auto& p:d43::kPins){unsigned char a[16]{};if(!Read(reinterpret_cast<void*>(b+p.rva),a)||std::memcmp(a,p.b,p.n)){ok=false;
        char m[160]{};std::snprintf(m,sizeof(m),"D43 LIMB_CUT pins: code bytes differ at +%llX - elbow/knee cut OFF",(unsigned long long)p.rva);diagnostic_trace::Write(m);break;}}
    for(const auto& s:d43::kSlots){if(!ok)break;std::uintptr_t fn=0;if(!Read(reinterpret_cast<void*>(b+s.vt+s.off),fn)||fn!=b+s.fn){ok=false;
        char m[160]{};std::snprintf(m,sizeof(m),"D43 LIMB_CUT pins: vtable %llX+0x%X is not +%llX - elbow/knee cut OFF",(unsigned long long)s.vt,s.off,(unsigned long long)s.fn);diagnostic_trace::Write(m);}}
    v=ok?1:-1;if(ok)diagnostic_trace::Write("D43 LIMB_CUT pins OK (CreateAttachment/LoadModelSKIN/CSKINAttachment/AddBinding, WHGame 6A350E20)");
    return ok;
}
// INI VrDismembermentLevel, read once from the KCD2VR.ini the host already loaded (same pattern as C67 ImpaleWaitMs67).
static int LimbLevel43() noexcept {
    static int lv=-1;if(lv>=0)return lv;
    const int raw=g_calibration_ini66100[0]?int(GetPrivateProfileIntA("Settings","VrDismembermentLevel",1,g_calibration_ini66100)):1;
    lv=std::clamp(raw,0,1);
    char m[200]{};std::snprintf(m,sizeof(m),"D43 LIMB_LEVEL %d (%s) source=%s (VrDismemberment must be 1 too)",lv,lv?"ELBOW/KNEE: native cut copies from the limbs41 pak":"WRIST/ANKLE: old C50 event only",g_calibration_ini66100[0]?"INI":"DEFAULT");
    diagnostic_trace::Write(m);return lv;
}
// D43b (Miki: "only on swings/cuts made with high energy, no stabs"): every candidate that reaches StartLimb50 is already a
// CUT (receipt kind 0 - the lethal, corpse and DS-F8 producers all require kind==0, a thrust is kind 1 and never gets here).
// On top of that the elbow/knee cut needs a strong stroke: VrDismembermentMinPower (read once, default 1.0 = a proper full
// stroke on the same strike energy ratio VrDecapitationMinPower uses, 0.3..1.5). Weaker killing cuts stay normal kills.
static float LimbMinPower43() noexcept {
    static float v=-1.f;if(v>=0.f)return v;float raw=1.f;
    if(g_calibration_ini66100[0]){char b[32]{};GetPrivateProfileStringA("Settings","VrDismembermentMinPower","1.0",b,sizeof(b),g_calibration_ini66100);char* end=nullptr;raw=std::strtof(b,&end);if(end==b)raw=1.f;}
    v=std::isfinite(raw)?std::clamp(raw,.3f,1.5f):1.f;
    char m[160]{};std::snprintf(m,sizeof(m),"D43 LIMB_MIN_POWER %.2f (VrDismembermentMinPower, cuts only - stabs never dismember)",double(v));diagnostic_trace::Write(m);return v;
}
// One layer: own CA_SKIN attachment with the cut copy + the original's materials. The original is NOT touched here.
static d43::R BindCutLayer43(void* ci,void* orig,const char* made,const char* cutSkin,
    const visibility59::Key& originalKey,std::uint64_t batch) noexcept {
    using namespace d43;const auto b=WhBase();if(!CutPins43())return R::Pins;
    std::uintptr_t vt=0;if(!orig||!Read(orig,vt)||vt!=b+kSkinAttVt)return R::NotSkinLayer;
    void* mgr=static_cast<unsigned char*>(ci)+8;if(!Read(mgr,vt)||vt!=b+kMgrVt)return R::NotSkinLayer;
    // Never recycle a name-only match: it may belong to Lua or a previous body.
    // The reserved original lease owns cleanup before any copy can be bound.
    auto* lease=g_head_visibility59.find(originalKey,batch);if(!lease||!lease->restorePending)return R::Fault;
    if(CharAttachment59c15(ci,made))return R::Exists;
    void* charMgr=nullptr;if(!Read(reinterpret_cast<void*>(b+kCharMgrGlobal),charMgr)||!charMgr||!Read(charMgr,vt)||vt!=b+kCharMgrVt)return R::Pins;
    void* oldObj=nullptr;void* skin=nullptr;void* att=nullptr;void* obj=nullptr;R r=R::Ok;
    __try{
        oldObj=reinterpret_cast<void*(__fastcall*)(void*)>(b+kGetObject)(orig);
        std::uintptr_t ovt=0;if(!oldObj||!Read(oldObj,ovt)||ovt!=b+kSkinObjVt)return R::NoObject;
        void* sp=nullptr;reinterpret_cast<void**(__fastcall*)(void*,void**,const char*,std::uint32_t)>(b+kLoadSkin)(charMgr,&sp,cutSkin,0u);skin=sp;
        std::uintptr_t svt=0;if(!skin||!Read(skin,svt)||svt!=b+kSkinVt){skin=nullptr;return R::NoSkinFile;} // a missing file returns null (engine logs it)
        att=reinterpret_cast<void*(__fastcall*)(void*,const char*,std::uint32_t,const char*,bool)>(b+kCreateAttachment)(mgr,made,2u,nullptr,false);
        std::uintptr_t avt=0;if(!att||!Read(att,avt)||avt!=b+kSkinAttVt){r=R::CreateFailed;}
        if(r==R::Ok){
            visibility59::Key copy=originalKey;copy.attachment=reinterpret_cast<std::uintptr_t>(att);
            std::snprintf(copy.name,sizeof(copy.name),"%s",made);HeadVisibilityIO59 io{};
            if(!g_head_visibility59.linkCut(io,originalKey,batch,copy))r=R::CreateFailed;
            else reinterpret_cast<void(__fastcall*)(void*,std::uint32_t)>(b+kSetFlags)(att,lease->owned); // bound hidden
        }
        if(r==R::Ok){obj=reinterpret_cast<void*(__fastcall*)(std::size_t)>(b+kCryMalloc)(0x40);if(!obj)r=R::AllocFailed;}
        if(r==R::Ok){
            reinterpret_cast<void*(__fastcall*)(void*)>(b+kSkinObjCtor)(obj);
            reinterpret_cast<void*(__fastcall*)(void**,void*)>(b+kAssignSkinIface)(reinterpret_cast<void**>(static_cast<unsigned char*>(obj)+8),static_cast<unsigned char*>(att)+8);
            for(std::uint32_t lod=0;lod<6;++lod){void* m=reinterpret_cast<void*(__fastcall*)(void*,std::uint32_t)>(b+kGetMtl)(oldObj,lod);
                if(m)reinterpret_cast<void(__fastcall*)(void*,void*,std::uint32_t)>(b+kSetMtl)(obj,m,lod);}
            const std::uint32_t bound=reinterpret_cast<std::uint32_t(__fastcall*)(void*,void*,void*,std::uint32_t)>(b+kAddBinding)(att,obj,skin,0u);
            if(!bound){reinterpret_cast<void*(__fastcall*)(void*,unsigned)>(*reinterpret_cast<void**>(*reinterpret_cast<std::uintptr_t*>(obj)))(obj,1u);obj=nullptr;r=R::BindFailed;}
        }
        reinterpret_cast<void(__fastcall*)(void*)>(b+kSkinRelease)(skin);skin=nullptr; // AddBinding holds its own reference
        // The reserved exact lease retains failed-bind cleanup, including an
        // applied-then-fault binding. Never retire it based on a void Hide call.
    }__except(EXCEPTION_EXECUTE_HANDLER){return R::Fault;}
    return r;
}
#include "cut_lifecycle_d52.inl"
// All layers of the struck limb. Fills the event's sw lists; returns the number of cut layers.
static int CutLayers43(const g::CutCandidate& c,void* ci,gp50::Cut cut,std::uint64_t now,gp50::Event& e) noexcept {
    const char* key=gp50::sideKey43(cut);if(!key||!CutPins43())return 0;
    void* mgr=static_cast<unsigned char*>(ci)+8;const int n=AttachmentCount(mgr);if(n<=0||n>512)return 0;
    if(g_head_visibility_batch61==~std::uint64_t(0)){diagnostic_trace::Write("D43 LIMB_CUT refused reason=VISIBILITY_BATCH_EXHAUSTED");return 0;}
    const std::uint64_t batch=++g_head_visibility_batch61;
    e.nativeBatchD52=batch;
    static char orig[16][64],made[16][64],skins[16][200];
    // pass 1: collect (the attachment list changes while we add our own)
    void* atts[16]{};int m=0;
    for(int i=0;i<n&&m<16;++i){void* att=AttachmentAt(mgr,unsigned(i));char nm[64]{};if(!att||!AttachmentName(att,nm,sizeof(nm)))continue;
        if(g::istarts(nm,"kcd2vr_")||g::istarts(nm,"hand_")||g::istarts(nm,"entity_")||g::istarts(nm,"weapon_")||g::headAttachment(nm))continue;
        bool hid=false;if(!AttSkinClassR12(att)||!AttHidden41(att,hid)||hid)continue; // hidden layers (helmet off etc.) stay as they are
        char raw[260]{};if(!HeadSkinPath43(att,raw,sizeof(raw)))continue;char sk[200]{};gp50::normPath(raw,sk,sizeof(sk));
        if(!gp50::relevantSkin43(cut,sk)||!gp50::safeToken(nm)||!gp50::safeToken(sk))continue;
        std::snprintf(orig[m],64,"%s",nm);std::snprintf(skins[m],200,"%s",sk);atts[m]=att;++m;}
    int done=0;std::string list("D43 LIMB_CUT");char head[160]{};
    std::snprintf(head,sizeof(head)," target=%u side=%s candidates=%d:",c.target,key,m);list+=head;
    for(int i=0;i<m;++i){
        char cutSkin[220]{};char mk[64]{};
        if(!gp50::cutPath43(skins[i],cut,cutSkin,sizeof(cutSkin))||!gp50::cutName43(orig[i],mk,sizeof(mk))){list+=' ';list+=orig[i];list+="=PATH_TOO_LONG";continue;}
        visibility59::Key originalKey{c.target,c.identity,reinterpret_cast<std::uintptr_t>(ci),reinterpret_cast<std::uintptr_t>(atts[i]),{}};
        std::snprintf(originalKey.name,sizeof(originalKey.name),"%s",orig[i]);HeadVisibilityIO59 io{};
        if(!g_head_visibility59.reserveCut(io,originalKey,now,batch)){
            TraceCutLifecycleD52(now,"RESERVE_REFUSED",c.target,batch,orig[i],false);continue;}
        const d43::R r=BindCutLayer43(ci,atts[i],mk,cutSkin,originalKey,batch);
        list+=' ';list+=orig[i];list+='=';list+=d43::name(r);
        if(r!=d43::R::Ok||!g_head_visibility59.activateCut(io,originalKey,batch,now)){
            g_head_visibility59.restoreKey(io,originalKey,batch,now);
            TraceCutLifecycleD52(now,r!=d43::R::Ok?d43::name(r):"ACTIVATE_REFUSED",c.target,batch,orig[i],false);
            list+="(EXACT_CLEANUP_PENDING_OR_DONE)";continue;}
        std::snprintf(made[done],64,"%s",mk);
        e.swOrig[done]=orig[i];e.swMade[done]=made[done];e.swSkin[done]=skins[i];++done;
        for(auto& w:g_cut_watch43)if(!w.key.target){w.key=originalKey;w.until=g_head_visibility59.find(originalKey,batch)->reassertUntil;w.batch=batch;break;}
        TraceCutLifecycleD52(now,"ACTIVE",c.target,batch,orig[i],true);
    }
    e.nSw43=done;if(done)e.level43=1;
    list+=done?" -> CUT":" -> nothing cut (no cut copy loaded: limbs41 pak missing or no such layer)";diagnostic_trace::Write(list.c_str());
    return done;
}
