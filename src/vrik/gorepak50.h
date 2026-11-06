#pragma once
// C50 (Miki 18 Oct): decapitation and dismemberment move into a SEPARATE optional pak (Mods/kcd2vr_gore), so the VR mod
// itself is not an adult mod. Without that pak the VR mod never hides, cuts or spawns any body part: a kill move is a
// normal native kill (blood stays the game's own). With the pak installed the VR mod only DECIDES (which kill move hit
// which part, where, how hard, from which side) and hands one event to the pak's Lua (KCD2VR_Gore.Cut). The pak owns
// what it looks like: hidden parts, cut body meshes, stumps, the flying head/hand/foot.
// Pure logic (host tested). The native side lives in vrik/gore32.inl.
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "gore_material_d54.h"

namespace kcd2vr::gorepak50 {
inline constexpr const char* kModFolder="kcd2vr_gore";          // <game>/Mods/kcd2vr_gore/mod.manifest
inline constexpr int kProtocol=1;                               // KCD2VR_Gore.protocol the pak must report
// C75 sends up to 24 attachment names/skin paths, rather than the old "hidden"
// token. A complete event must fit before it is queued; callers restore their
// own presentation leases if building or queueing fails.
inline constexpr std::size_t kEventLuaCapacity61=32768;
// 0 = not checked yet, 1 = pak installed, 2 = no pak (the VR mod does no gore at all)
inline std::atomic<int> state{0};
inline std::atomic<bool> dismember{false};                      // VrDismemberment (needs the matching pak): qualified corpse elbow/knee layer cuts

enum class Cut : int {None=0,Head,LeftHand,RightHand,LeftFoot,RightFoot};
inline const char* luaName(Cut c) noexcept {
    switch(c){case Cut::Head:return "head";case Cut::LeftHand:return "lh";case Cut::RightHand:return "rh";
              case Cut::LeftFoot:return "lf";case Cut::RightFoot:return "rf";default:return "none";}}

// The victim's limb joints on HIS skeleton (KCD2 human skeleton names, read from the game's body skins:
// LeftHand / LeftForeArm / LeftFoot / LeftLeg = calf, LeftUpLeg = thigh). -1 = not resolved.
struct LimbIds {int lHand=-1,lFore=-1,rHand=-1,rFore=-1,lFoot=-1,lCalf=-1,rFoot=-1,rCalf=-1;};
inline constexpr const char* kLimbJointNames[8]{"LeftHand","LeftForeArm","RightHand","RightForeArm","LeftFoot","LeftLeg","RightFoot","RightLeg"};
inline int* limbSlot(LimbIds& l,int i) noexcept {int* s[8]{&l.lHand,&l.lFore,&l.rHand,&l.rFore,&l.lFoot,&l.lCalf,&l.rFoot,&l.rCalf};return (i>=0&&i<8)?s[i]:nullptr;}
// part = struck physics part (= joint id), chain = its parents (nearest first). A hit on the hand, a finger or the forearm
// takes the hand off at the wrist; a hit on the foot or the calf takes the foot off at the ankle (phase 1: the game has
// separate hand/foot body pieces; elbow / knee cuts need own meshes, later).
inline Cut classifyLimb(const LimbIds& l,int part,const int* chain,int chainLen) noexcept {
    if(part<0)return Cut::None;
    auto is=[&](int id)->bool{if(id<0)return false;if(part==id)return true;for(int i=0;i<chainLen&&i<8;++i)if(chain[i]==id)return true;return false;};
    if(is(l.lHand)||is(l.lFore))return Cut::LeftHand;
    if(is(l.rHand)||is(l.rFore))return Cut::RightHand;
    if(is(l.lFoot)||is(l.lCalf))return Cut::LeftFoot;
    if(is(l.rFoot)||is(l.rCalf))return Cut::RightFoot;
    return Cut::None;
}
// Which of the victim's skin attachments the pak may need for this cut (it keeps cut copies of the game's own body,
// hand, foot and leg meshes). Paths come from the game's attachment objects.
inline bool relevantSkin(Cut c,const char* path) noexcept {
    if(!path||!*path)return false;
    auto has=[&](const char* s){return std::strstr(path,s)!=nullptr;};
    if(c==Cut::LeftHand||c==Cut::RightHand)return has("/body/")||has("/hands/");
    if(c==Cut::LeftFoot||c==Cut::RightFoot)return has("/body/")||has("/feet/")||has("/legs/");
    return false;
}
// D43 (Claude, 28 Oct 2026, Miki: "finally working dismemberment of one chosen arm or leg"): the cut is at the ELBOW / KNEE.
// The pak (kcd2vr_gore_limbs41_*.pak, made by tools/gore_limb_d41.py) ships for every body/clothing skin that has forearm or
// shin vertices a copy <stem>_kcdg<side>.skin where that limb is folded into a closed cap at the joint, and a flying piece
// <stem>_kcdg<side>p.skin. The native side (native_gore limb_cut_d43.inl) binds the copy as an OWN skin attachment
// (kcd2vr_cut_<layer>) on the dead victim and hides the original layer, so the arm/leg really ends at the elbow/knee and
// rides the ragdoll. side: la/ra = left/right forearm (hand or forearm struck), ll/rl = left/right shin (foot or calf struck).
inline const char* sideKey43(Cut c) noexcept {
    switch(c){case Cut::LeftHand:return "la";case Cut::RightHand:return "ra";case Cut::LeftFoot:return "ll";case Cut::RightFoot:return "rl";default:return nullptr;}}
// Which layers may carry the struck limb (normalized path). Everything else (head, hair, scabbards, jewellery) is never touched.
// A layer without a cut copy in the pak simply fails to load and stays as it is.
inline bool relevantSkin43(Cut c,const char* path) noexcept {
    if(!path||!*path||!std::strstr(path,"objects/characters/humans/"))return false;
    auto has=[&](const char* s){return std::strstr(path,s)!=nullptr;};
    if(has("/head/")||has("/hair/")||has("/beard")||has("/scabbard/")||has("/decoration/")||has("/f_head/")||has("_kcdg"))return false;
    const std::size_t n=std::strlen(path);if(n<6||std::strcmp(path+n-5,".skin"))return false;
    if(c==Cut::LeftHand||c==Cut::RightHand)return has("/body/")||has("/arms/")||has("/hands/")||has("/arm_armour/")||has("/torso/")||has("/f_torso/");
    if(c==Cut::LeftFoot||c==Cut::RightFoot)return has("/body/")||has("/legs/")||has("/feet/")||has("/f_legs/")||has("/torso/")||has("/f_torso/");
    return false;
}
// <stem>_kcdg<side>.skin from a normalized .skin path. false when it does not fit or the input is not a .skin path.
inline bool cutPath43(const char* skin,Cut c,char* out,std::size_t cap) noexcept {
    const char* key=sideKey43(c);if(!skin||!key||!out||!cap)return false;
    const std::size_t n=std::strlen(skin);if(n<6||std::strcmp(skin+n-5,".skin"))return false;
    const int k=std::snprintf(out,cap,"%.*s_kcdg%s.skin",int(n-5),skin,key);return k>0&&std::size_t(k)<cap;
}
// the own attachment name for a cut layer: kcd2vr_cut_<layer> (the C50 limb scan already skips every kcd2vr_ name)
inline bool cutName43(const char* layer,char* out,std::size_t cap) noexcept {
    if(!layer||!*layer||!out||!cap)return false;const int k=std::snprintf(out,cap,"kcd2vr_cut_%s",layer);return k>0&&std::size_t(k)<cap;}
// Only plain path/name characters go into the Lua text (no quotes, no backslashes, no control characters).
inline bool safeToken(const char* s) noexcept {
    if(!s||!*s)return false;
    for(const char* p=s;*p;++p){const unsigned char ch=static_cast<unsigned char>(*p);
        if(!((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||(ch>='0'&&ch<='9')||ch=='_'||ch=='/'||ch=='.'||ch=='-'))return false;}
    return true;
}
// lower case, forward slashes (the game mixes both in its tables)
inline void normPath(const char* in,char* out,std::size_t cap) noexcept {
    std::size_t i=0;if(!cap)return;
    for(;in&&in[i]&&i+1<cap;++i){char ch=in[i];if(ch=='\\')ch='/';if(ch>='A'&&ch<='Z')ch=char(ch-'A'+'a');out[i]=ch;}
    out[i]=0;
}
// H24: the primary flying-head model is the face skin, never an optional
// beard/hair/helmet attachment. A /head/ substring alone also matches clothing.
inline int primaryHeadSkinRankH24(const char* attachment,const char* raw) noexcept {
    if(!attachment||!raw||!raw[0]||std::strlen(raw)>=260)return 0;
    char name[64]{},path[260]{};
    if(std::strlen(attachment)>=sizeof(name))return 0;
    normPath(attachment,name,sizeof(name));normPath(raw,path,sizeof(path));
    const int rank=std::strcmp(name,"head_face")==0?2:std::strcmp(name,"head")==0?1:0;
    if(!rank||!safeToken(path)||!std::strstr(path,"/head/"))return 0;
    const char* forbidden[]{"/clothing/","/beards/","/beard/","/hair/","/hairs/","/helmet/","/helmets/","/coif/"};
    for(const char* segment:forbidden)if(std::strstr(path,segment))return 0;
    const std::size_t n=std::strlen(path);
    return n>=5&&std::strcmp(path+n-5,".skin")==0?rank:0;
}
inline bool offerPrimaryHeadSkinH24(const char* attachment,const char* raw,
                                  char* selected,std::size_t cap,int& selectedRank) noexcept {
    const int rank=primaryHeadSkinRankH24(attachment,raw);
    if(!selected||!cap||!rank||rank<=selectedRank||std::strlen(raw)>=cap)return false;
    normPath(raw,selected,cap);selectedRank=rank;return true;
}
struct Event {std::uint32_t target=0;Cut cut=Cut::None;double p[3]{},d[3]{};double energy=0;bool corpse=false;bool female=false;
    const char* headSkin=nullptr;int nAtt=0;const char* attName[24]{};const char* attSkin[24]{};
    // D54 presentation only: copied names choose wrappers; live native material
    // instances are transferred after synchronous Lua creation, never serialized.
    const char* headMtl=nullptr;const char* attMtlD54[24]{};
    std::uint64_t materialTokenD54=0;
    std::uintptr_t nativeVictimD54=0,nativeCharacterD54=0;
    // D43: level43=1 = elbow/knee cut already done natively; sw* = original layer, own cut attachment, original skin path
    int level43=0;int nSw43=0;const char* swOrig[16]{};const char* swMade[16]{};const char* swSkin[16]{};
    std::uint64_t nativeBatchD52=0; // native rollback only, never serialized as an engine pointer
};
// One Lua chunk: the call only happens if the pak defined KCD2VR_Gore with the same protocol; results go to kcd.log.
// returns the length written, or 0 if it did not fit / bad input (never a cut-off chunk).
inline int buildLua(const Event& e,char* out,std::size_t cap) noexcept {
    if(!out||cap<64||e.cut==Cut::None)return 0;
    for(int i=0;i<3;++i)if(!std::isfinite(e.p[i])||!std::isfinite(e.d[i]))return 0;
    const bool nativeLimbR93=e.level43==1&&sideKey43(e.cut);
    if(nativeLimbR93){
        if(e.nSw43<1||e.nSw43>16)return 0;
        for(int i=0;i<e.nSw43;++i)if(!safeToken(e.swOrig[i])||!safeToken(e.swMade[i])||!safeToken(e.swSkin[i]))return 0;
    }
    int n=std::snprintf(out,cap,
        "if KCD2VR_Gore and KCD2VR_Gore.protocol==%d and KCD2VR_Gore.Cut%s then local ok,err=pcall(KCD2VR_Gore.Cut,{target=%u,cut='%s',"
        "p={x=%.3f,y=%.3f,z=%.3f},d={x=%.3f,y=%.3f,z=%.3f},energy=%.2f,corpse=%s,female=%s,",
        kProtocol,nativeLimbR93?" and KCD2VR_Gore.nativeLimbHandoverR93==1":"",e.target,luaName(e.cut),e.p[0],e.p[1],e.p[2],e.d[0],e.d[1],e.d[2],e.energy,e.corpse?"true":"false",e.female?"true":"false");
    if(n<0||std::size_t(n)>=cap)return 0;
    auto put=[&](const char* s)->bool{const int k=std::snprintf(out+n,cap-std::size_t(n),"%s",s);if(k<0||std::size_t(n+k)>=cap)return false;n+=k;return true;};
    if(e.headSkin&&safeToken(e.headSkin)){if(!put("headSkin='")||!put(e.headSkin)||!put("',"))return 0;}
    if(e.headMtl&&safeToken(e.headMtl)){if(!put("headMtl=KCD2VR_Gore.nativeMaterialD54 and '" )||!put(e.headMtl)||!put("' or nil,"))return 0;}
    if(e.materialTokenD54){char token[128]{};std::snprintf(token,sizeof(token),"materialTokenD54=KCD2VR_Gore.nativeMaterialD54 and '%016llx' or nil,",(unsigned long long)e.materialTokenD54);if(!put(token))return 0;}
    if(!put("att={"))return 0;
    for(int i=0;i<e.nAtt&&i<24;++i){
        if(!safeToken(e.attName[i])||!safeToken(e.attSkin[i]))continue;
        if(!put("{n='")||!put(e.attName[i])||!put("',s='")||!put(e.attSkin[i])||!put("'"))return 0;
        if(e.attMtlD54[i]&&safeToken(e.attMtlD54[i])){if(!put(",m=KCD2VR_Gore.nativeMaterialD54 and '")||!put(e.attMtlD54[i])||!put("' or nil"))return 0;}
        if(e.materialTokenD54){char index[96]{};std::snprintf(index,sizeof(index),",iD54=KCD2VR_Gore.nativeMaterialD54 and %d or nil",i+1);if(!put(index))return 0;}
        if(!put("},"))return 0;
    }
    if(e.level43>0){ // D43: closes att={...} and opens sw={...}; the common tail closes whichever list is open
        char lv[32]{};std::snprintf(lv,sizeof(lv),"},lvl=%d,sw={",e.level43);if(!put(lv))return 0;
        for(int i=0;i<e.nSw43&&i<16;++i){
            if(!safeToken(e.swOrig[i])||!safeToken(e.swMade[i])||!safeToken(e.swSkin[i]))continue;
            if(!put("{n='")||!put(e.swOrig[i])||!put("',c='")||!put(e.swMade[i])||!put("',s='")||!put(e.swSkin[i])||!put("'},"))return 0;
        }
    }
    if(nativeLimbR93){
        // ExecuteBuffer must fail for a missing capability, rejected handover or
        // pre-acceptance exception. The native owner then restores its exact batch.
        if(!put("}}) if not ok or err~=true then error('KCD2VR_GORE native limb handover not accepted') end else error('KCD2VR_GORE native limb handover capability missing') end"))return 0;
    }else{
        char tail[200]{};std::snprintf(tail,sizeof(tail),"}}) if not ok then System.LogAlways('KCD2VR_GORE error target=%u cut=%s '..tostring(err)) end "
            "else System.LogAlways('KCD2VR_GORE pak script not loaded (protocol %d)') end",e.target,luaName(e.cut),kProtocol);
        if(!put(tail))return 0;
    }
    return n;
}
} // namespace kcd2vr::gorepak50
