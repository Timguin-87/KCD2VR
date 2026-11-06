#pragma once
// W1 (Claude, 25 Oct 2026) - VRIK on every Henry skeleton + the captured native fist as default.
// Miki: "load from another save has the same hands - misaligned and not working fingers" and "fists from the
// codex fixes as the default fists". 25 Oct H50 capture: rig wrist 163/198 -> fingerMap=0 (no fingers, raw
// controller wrist), later rig 408/443 -> 22 fingers. the old finder only accepted joint ids 408/443/472.
// now the 5 finger chains are found by the joint NAME crc of each finger joint (skeleton record +8, the same crc
// the native finger profile already reads) and their parent links, on any joint numbering. fallback: topology.
// the H45 captured punch endpoint (tick 935961531) is keyed by those crcs too, so it applies on every rig.
#include <array>
#include <atomic>
#include <cstdint>
namespace kcd2vr::finger_rig_w1 {
inline std::atomic<bool> capturedFistDefault{true}; // INI VrCapturedFistDefault: the F7 rung 2 fist is the normal fist
inline std::atomic<std::uint32_t> lastMethod[2]{}; // per side: 1 name crc, 2 proven fixed ids, 3 topology
struct RefJoint {int id,parent;std::uint32_t crc;float q[4];};
// thumb 4, index 4, middle 4, ring 5, pinky 5 joints; parent-local closed fist (H45 capture); [0]=right [1]=left.
inline constexpr int kChainLength[5]{4,4,4,5,5};
inline constexpr RefJoint kPunch[2][22]{
  {
    {416,408,0x9829BECFu,{0.639356494f,-0.341034532f,-0.0556316972f,0.686894417f}},
    {421,416,0x0120EF75u,{0.0399066806f,-0.0381337628f,-0.43914026f,0.896721303f}},
    {422,421,0x7627DFE3u,{-0.0253575146f,-0.0166355371f,-0.619723141f,0.784234285f}},
    {423,422,0xE8434A40u,{-0.000220291317f,0.00606703758f,-0.0186245516f,0.999808133f}},
    {415,408,0xFEBAC1B3u,{-0.0169665199f,-0.0423644334f,-0.503853619f,0.862582803f}},
    {424,415,0x67B39009u,{-0.0125406682f,0.00872209668f,-0.802230477f,0.596818984f}},
    {425,424,0x10B4A09Fu,{0.00455552386f,-0.00148120907f,-0.640724123f,0.767756283f}},
    {426,425,0x8ED0353Cu,{0.000558450818f,-0.00493966974f,-0.0320944488f,0.999472499f}},
    {414,408,0x45445772u,{-0.0868088081f,-0.0425890088f,-0.595007658f,0.797882438f}},
    {427,414,0xDC4D06C8u,{0.00413706899f,0.00147172809f,-0.822452068f,0.568817556f}},
    {428,427,0xAB4A365Eu,{-0.0013026594f,0.00550485635f,-0.59964329f,0.800247371f}},
    {429,428,0x352EA3FDu,{-0.000671997666f,0.0061951899f,-0.0374119133f,0.999280572f}},
    {413,408,0xB133130Au,{0.201066405f,-0.126078248f,-0.0280686021f,-0.971024632f}},
    {430,413,0xDBF635C5u,{-0.0955015421f,-0.0247885734f,0.632730961f,-0.768060327f}},
    {431,430,0x42FF647Fu,{0.0162482262f,0.0335493982f,-0.739558816f,0.67205894f}},
    {432,431,0x35F854E9u,{0.00453007221f,0.0277165622f,-0.506151795f,0.861986995f}},
    {433,432,0xAB9CC14Au,{-0.000320404768f,0.00568824261f,-0.0132732689f,0.999895692f}},
    {412,408,0xCDCAE074u,{0.255747259f,-0.192159474f,0.0153933754f,-0.947328448f}},
    {434,412,0x147BB2DFu,{-0.10384392f,0.0270544458f,0.530673683f,-0.840755641f}},
    {435,434,0x8D72E365u,{0.0292319674f,0.0353847481f,-0.789445341f,0.612102568f}},
    {436,435,0xFA75D3F3u,{-0.0235468745f,0.0148991272f,-0.569390416f,0.821594834f}},
    {437,436,0x64114650u,{-0.00529788435f,0.0331332684f,-0.0597496293f,0.997649312f}},
  },
  {
    {451,443,0x78F4E1ACu,{0.536258638f,-0.335402489f,0.0443435609f,0.773282349f}},
    {454,451,0xE1FDB016u,{0.0577163696f,-0.0219000578f,-0.359364539f,0.931153297f}},
    {455,454,0x96FA8080u,{-0.0130648725f,0.0128187332f,-0.388458729f,0.921284378f}},
    {456,455,0x089E1523u,{-0.000479295792f,0.00668665674f,-0.0175539907f,0.999823451f}},
    {450,443,0x1E679ED0u,{-0.0118482849f,-0.0577552877f,-0.628569067f,0.775515914f}},
    {457,450,0x876ECF6Au,{-0.0120077515f,0.0154343266f,-0.81908989f,0.573331773f}},
    {458,457,0xF069FFFCu,{0.00211170316f,-0.00382880867f,-0.598007679f,0.801478446f}},
    {459,458,0x6E0D6A5Fu,{0.000460244657f,-0.00479488028f,-0.0257877521f,0.999655843f}},
    {449,443,0x911FBACFu,{-0.105216704f,-0.067530565f,-0.656772673f,0.743652344f}},
    {460,449,0x0816EB75u,{-0.0068290839f,-0.00192598416f,-0.858360529f,0.512998044f}},
    {461,460,0x7F11DBE3u,{0.0024946332f,0.0117462799f,-0.579716265f,0.814729989f}},
    {462,461,0xE1754E40u,{-0.000699952245f,0.00604684651f,-0.029933095f,0.999533415f}},
    {448,443,0x51EE4C69u,{-0.201025605f,0.12599881f,0.027992785f,0.971045554f}},
    {463,448,0xD34048CFu,{0.0720222443f,0.00740161538f,-0.709346652f,0.701131463f}},
    {464,463,0x4A491975u,{8.09455523e-05f,0.0414735451f,-0.836242974f,0.546788514f}},
    {465,464,0x3D4E29E3u,{0.0117681036f,0.0406249501f,-0.523368359f,0.851056278f}},
    {466,465,0xA32ABC40u,{-0.00028784576f,0.00555051165f,-0.0106339911f,0.999928057f}},
    {447,443,0x19910DC9u,{-0.255902231f,0.19613263f,-0.0167957246f,0.946448088f}},
    {467,447,0xF4A6EDBCu,{0.120230347f,-0.0388442278f,-0.610756099f,0.781673133f}},
    {468,467,0x6DAFBC06u,{-0.0104746791f,0.0371478721f,-0.875625312f,0.481446415f}},
    {469,468,0x1AA88C90u,{-0.0312431604f,0.00596047193f,-0.587531865f,0.808575749f}},
    {470,469,0x84CC1933u,{-0.00223408639f,0.0270837694f,-0.0475282222f,0.998500228f}},
  },
};
struct Chain {std::array<int,6> id{};int count=0;};
enum Method : std::uint32_t {None=0,Crc=1,Fixed=2,Topology=3};
// crcOf(j) -> name crc of joint j (0 = unreadable), parentOf(j) -> parent id (-2 = unreadable).
template<class CrcFn,class ParentFn>
inline bool DiscoverByCrc(int count,int hand,bool left,CrcFn crcOf,ParentFn parentOf,std::array<Chain,5>& out) noexcept {
    if(count<=0||hand<0||hand>=count)return false;
    const auto& ref=kPunch[left?1:0];int n=0;
    for(int f=0;f<5;++f){Chain c{};int expectedParent=hand;
        for(int k=0;k<kChainLength[f];++k,++n){const std::uint32_t want=ref[n].crc;int found=-1;
            for(int j=0;j<count;++j){if(crcOf(j)!=want||parentOf(j)!=expectedParent)continue;
                if(found>=0)return false; // two joints with one name under one parent: not a hand we know
                found=j;}
            if(found<0)return false;c.id[c.count++]=found;expectedParent=found;
        }
        out[f]=c;
    }
    return true;
}
// fallback: 5 single-child chains under the hand (item branch excluded), ordered like the proven Henry rig
// (higher joint id = thumb ... lower = pinky).
template<class ParentFn>
inline bool DiscoverByTopology(int count,int hand,int item,ParentFn parentOf,std::array<Chain,5>& out) noexcept {
    if(count<=0||hand<0||hand>=count)return false;
    std::array<Chain,8> found{};int nf=0;
    for(int root=count-1;root>=0;--root){
        if(root==item||parentOf(root)!=hand)continue;
        Chain c{};c.id[c.count++]=root;int cur=root;bool branch=false;
        for(int d=0;d<5&&c.count<int(c.id.size());++d){int child=-1,nc=0;
            for(int j=0;j<count;++j)if(j!=item&&parentOf(j)==cur){child=j;if(++nc>1)break;}
            if(nc==0)break;if(nc>1){branch=true;break;}c.id[c.count++]=child;cur=child;}
        if(branch||c.count<3||c.count>5)continue;
        if(nf>=int(found.size()))return false;found[nf++]=c;
    }
    if(nf!=5)return false;
    for(int f=0;f<5;++f)out[f]=found[f];
    return true;
}
// the captured fist is keyed by name crc + chain position (not joint numbers): every joint must be the hand's or
// the previous chain joint's child and carry the captured joint's name crc.
template<class ParentFn,class CrcFn>
inline bool PunchMatches(const std::array<Chain,5>& chains,int hand,bool left,ParentFn parentOf,CrcFn crcAt) noexcept {
    const auto& ref=kPunch[left?1:0];int n=0;
    for(int f=0;f<5;++f){if(chains[f].count!=kChainLength[f])return false;
        for(int k=0;k<chains[f].count;++k,++n){const int id=chains[f].id[k];
            const int wantParent=k?chains[f].id[k-1]:hand;
            if(id<0||parentOf(id)!=wantParent||crcAt(n)!=ref[n].crc)return false;}}
    return n==22;
}
} // namespace kcd2vr::finger_rig_w1
