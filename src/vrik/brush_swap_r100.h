#pragma once
// R100 loose scenery props (swap), INI VrSceneryPropSwapR100, Experiments > Hands page 2.
// Most small table props (tankards, plates, cups, spoons, candles, bowls) are plain level
// BRUSHES, not entities (Data/IPL_GameData.pak prefabs: 923 Type="Brush" vs 47 GeomEntity).
// A brush has a static physical entity whose foreign data is the CBrush render node
// (iForeignData 1, CBrush::Physicalize WHGame+530C69). When an empty hand closes Grip on
// such a mesh, the swap hides the brush (IRenderNode::Hide), suspends its physics
// (IPhysicalWorld::DestroyPhysicalEntity mode 1, reversible with mode 2), spawns a NOT SAVED
// BasicEntity rigid body with the same .cgf at the same transform and grabs it on the normal
// small-prop carry path. Anything failing after the hide is rolled back right away.
// This header is pure (selection, path rules, swap order, table, Lua note); the native
// leaves live in brush_swap_native_r100.inl, proofs in REVIEW100_swap.md.
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace kcd2vr::brush_swap_r100 {
inline std::atomic<bool> enabled{true}; // ON = the swap runs (independent of VrStaticPropGrabR89)

inline constexpr double kMaxDiameter=.65;   // world AABB diagonal of the brush body
inline constexpr double kInflate=.04;       // palm may be 4 cm outside the box
inline constexpr double kDensity=350,kMinMass=.2,kMaxMass=1.5;
inline constexpr unsigned kTableMax=256;
// CBrush::Physicalize: CreatePhysicalEntity(PE_STATIC, 0, this, iForeignData=1, -1, host)
inline constexpr short kForeignStatic=1,kForeignEntity=2;

// one physical entity returned by the palm box query (read only, copied)
struct Candidate {
    std::uintptr_t body=0,owner=0;float bb[6]{};short foreign=-1;
    std::uint32_t entity=0;   // CEntity id when foreign==2
    bool swapped=false;       // entity is one of our loose copies
};
enum class Pick {None,Entity,Brush};
struct Choice {Pick kind=Pick::None;unsigned index=0;double volume=0,diameter=0;const char* why="NO_BODY_AROUND_PALM";};

inline bool boxOk(const float (&bb)[6]) noexcept {
    for(int k=0;k<3;++k)if(!std::isfinite(bb[k])||!std::isfinite(bb[k+3])||bb[k+3]<bb[k]||std::fabs(bb[k])>1e6f||std::fabs(bb[k+3])>1e6f)return false;
    return true;
}
inline double diameterOf(const float (&bb)[6]) noexcept {
    double d=0;for(int k=0;k<3;++k){const double e=double(bb[k+3])-bb[k];d+=e*e;}return std::sqrt(d);
}
inline double volumeOf(const float (&bb)[6]) noexcept {
    double v=1;for(int k=0;k<3;++k)v*=double(bb[k+3])-bb[k]+.001;return v;
}
inline bool contains(const float (&bb)[6],const double (&p)[3],double inflate=kInflate) noexcept {
    for(int k=0;k<3;++k)if(p[k]<bb[k]-inflate||p[k]>bb[k+3]+inflate)return false;
    return true;
}
// loose copy mass from the brush box (hollow mugs/plates are light; never heavier than 1.5 kg)
inline double massFor(const float (&bb)[6]) noexcept {
    double v=1;for(int k=0;k<3;++k)v*=std::max(0.,double(bb[k+3])-bb[k]);
    if(!std::isfinite(v))return kMinMass;
    return std::clamp(v*kDensity,kMinMass,kMaxMass);
}
// Pick the smallest body whose world box holds the palm. Entities need allowEntities (the
// old "Small props in the hand" switch) unless they are our own loose copies; brushes need
// allowBrushes (this switch), the static foreign id, an owner and <= 65 cm. Terrain (foreign
// 0), furniture and the table under the hand never win against a smaller fitting box.
// valid(i,kind) is the native check (class/body/brush readback) and may refuse.
template<class Valid>
inline Choice choose(const double (&palm)[3],const Candidate* c,unsigned n,bool allowEntities,bool allowBrushes,Valid&& valid) noexcept {
    Choice best{};best.volume=1e18;bool found=false,sawInside=false;
    for(unsigned i=0;i<n;++i){
        const auto& x=c[i];if(!x.body||!boxOk(x.bb))continue;
        Pick kind=Pick::None;
        if(x.foreign==kForeignEntity&&x.owner&&x.entity&&(allowEntities||(allowBrushes&&x.swapped)))kind=Pick::Entity;
        else if(x.foreign==kForeignStatic&&x.owner&&allowBrushes)kind=Pick::Brush;
        if(kind==Pick::None)continue;
        if(!contains(x.bb,palm)){if(!found&&!sawInside)best.why=kind==Pick::Brush?"MESH_NEAR_BUT_PALM_NOT_ON_IT":"ENTITY_NEAR_BUT_PALM_NOT_ON_IT";continue;}
        sawInside=true;
        const double diameter=diameterOf(x.bb),volume=volumeOf(x.bb);
        if(kind==Pick::Brush&&!(diameter<=kMaxDiameter)){if(!found)best.why="MESH_BIGGER_THAN_65CM_OR_FURNITURE";continue;}
        if(volume>=best.volume)continue;
        if(!valid(i,kind)){if(!found)best.why=kind==Pick::Brush?"MESH_NOT_A_SINGLE_BRUSH":"ENTITY_CLASS_SIZE_OR_BODY_NOT_GRABBABLE";continue;}
        best.kind=kind;best.index=i;best.volume=volume;best.diameter=diameter;best.why="FOUND";found=true;
    }
    if(!found){best.kind=Pick::None;best.volume=0;}
    return best;
}

// Model path for Properties.object_Model: one plain .cgf, no merged/combined/HLOD geometry.
inline bool cleanPath(const char* in,char* out,std::size_t n,const char*& why) noexcept {
    why="NO_MODEL_PATH";if(!out||n<8)return false;out[0]=0;if(!in||!*in)return false;
    std::size_t k=0;
    for(std::size_t i=0;in[i];++i){
        if(k+1>=n){why="MODEL_PATH_TOO_LONG";out[0]=0;return false;}
        char c=in[i];if(c=='\\')c='/';
        const bool ok=(c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='-'||c=='.'||c=='/';
        if(!ok){why="MODEL_PATH_CHARACTERS";out[0]=0;return false;}
        out[k++]=c;
    }
    out[k]=0;
    char low[256]{};const std::size_t m=std::min(k,sizeof(low)-1);
    for(std::size_t i=0;i<m;++i){const char c=out[i];low[i]=c>='A'&&c<='Z'?char(c-'A'+'a'):c;}
    if(k<5||std::strcmp(low+m-4,".cgf")){why="MODEL_NOT_A_CGF";out[0]=0;return false;}
    if(std::strstr(low,"..")||low[0]=='/'){why="MODEL_PATH_CHARACTERS";out[0]=0;return false;}
    static const char* merged[]{"hlod","cestool","merged","combined"};
    for(const char* w:merged)if(std::strstr(low,w)){why="MERGED_OR_COMBINED_MESH";out[0]=0;return false;}
    // small pieces of houses/nature must stay level geometry even when <= 65 cm (a window part,
    // a nail, a stone). Folders from the game's own prefab brushes (IPL_GameData.pak Prefabs):
    // props live in objects/manmade/task_specific_props/household/cooking_eating/{plates,jugs,
    // tankards,cups,bowls,eating_tools}, common_illumination, common_furniture/baskets, food;
    // buildings in objects/manmade/structures, wall/window parts in objects/intermediates,
    // stones/plants in objects/natural. The log line names the model of every refusal.
    static const char* fixed[]{"/manmade/structures/","/intermediates/","objects/natural/","/nature/","objects/ui/","objects/panorama/","objects/special/"};
    for(const char* w:fixed)if(std::strstr(low,w)){why="BUILDING_OR_TERRAIN_PART";out[0]=0;return false;}
    why="OK";return true;
}

// What the native readback found about the brush (copied, borrowed pointers only as ids).
struct Brush {
    std::uintptr_t body=0,brush=0,stat=0,material=0;
    float tm[12]{};float bb[6]{};
    bool isSub=false;std::uintptr_t parent=0;int subCount=0;
    char path[200]{};char geo[48]{};
};
// Native readback refusals that are not geometry: the brush shows only a part of a file.
inline const char* brushRule(const Brush& b) noexcept {
    if(b.isSub||b.parent)return "PART_OF_ANOTHER_MESH_FILE";
    if(!boxOk(b.bb))return "BAD_BOX";
    if(!(diameterOf(b.bb)<=kMaxDiameter))return "MESH_BIGGER_THAN_65CM_OR_FURNITURE";
    for(float v:b.tm)if(!std::isfinite(v))return "BAD_TRANSFORM";
    return nullptr;
}

// swapped brushes for this level/session. Cleared on every load (the level brings its brushes
// back); never a permanent block: a full table only refuses until the next load.
struct Entry {std::uintptr_t brush=0,body=0,stat=0;std::uint32_t entity=0,serial=0;unsigned char soundKind=0;}; // R124: prop_impact_sound_r124::Kind of the .cgf path
struct Table {
    Entry e[kTableMax]{};unsigned n=0;std::uint64_t episode=0;bool known=false;
    const Entry* byBrush(std::uintptr_t b) const noexcept {for(unsigned i=0;i<n;++i)if(b&&e[i].brush==b)return &e[i];return nullptr;}
    const Entry* byBody(std::uintptr_t b) const noexcept {for(unsigned i=0;i<n;++i)if(b&&e[i].body==b)return &e[i];return nullptr;}
    bool hasEntity(std::uint32_t id) const noexcept {for(unsigned i=0;i<n;++i)if(id&&e[i].entity==id)return true;return false;}
    const Entry* byEntity(std::uint32_t id) const noexcept {for(unsigned i=0;i<n;++i)if(id&&e[i].entity==id)return &e[i];return nullptr;} // R124
    bool add(const Entry& x) noexcept {if(n>=kTableMax||!x.brush||byBrush(x.brush))return false;e[n++]=x;return true;}
    void clear() noexcept {n=0;}
};

// Lua -> native note: the companion reports the spawned entity id through the private
// System.GetCVar name "kcd2vr_swap_note_r100:<serial>:<id>" (same native getter hook as
// kcd2vr_small_pickup_v1). Only the one armed serial is accepted; it answers 1.
inline constexpr char notePrefix[]="kcd2vr_swap_note_r100:";
inline std::atomic<std::uint32_t> notePending{0},noteEntity{0};
inline bool parseNote(const char* text,std::uint32_t& serial,std::uint32_t& id) noexcept {
    serial=id=0;if(!text)return false;const char* p=text;
    for(char c:notePrefix){if(!c)break;if(*p++!=c)return false;}
    auto number=[&](std::uint32_t& v)->bool{unsigned digits=0;std::uint64_t x=0;
        while(*p>='0'&&*p<='9'){if(++digits>10)return false;x=x*10+unsigned(*p++-'0');}
        if(!digits||x>0xFFFFFFFFull)return false;
        v=std::uint32_t(x);return true;};
    if(!number(serial)||*p++!=':'||!number(id)||*p)return false;
    return serial&&id;
}
inline std::uint32_t acceptNote(const char* text) noexcept {
    std::uint32_t serial=0,id=0;if(!parseNote(text,serial,id))return 0;
    std::uint32_t want=notePending.load(std::memory_order_acquire);
    if(!want||want!=serial||!notePending.compare_exchange_strong(want,0,std::memory_order_acq_rel))return 0;
    noteEntity.store(id,std::memory_order_release);return 1;
}

// Swap order. Each leaf proves its own effect. Rollback is the exact reverse:
// remove the loose copy, restore physics (mode 2), un-hide.
// L: bool hide(bool), bool suspend(), bool restore(), bool spawn(uint32&), bool place(uint32),
//    bool grab(uint32), bool remove(uint32)
struct Outcome {bool ok=false,keptLoose=false,rollbackClean=true;const char* why="";std::uint32_t entity=0;};
template<class L> inline Outcome swap(L& l) noexcept {
    Outcome o{};
    if(!l.hide(true)){o.why="HIDE_NOT_CONFIRMED";o.rollbackClean=l.hide(false);return o;}
    if(!l.suspend()){o.why="PHYSICS_SUSPEND_REFUSED";o.rollbackClean=l.hide(false);return o;}
    std::uint32_t id=0;
    const bool spawned=l.spawn(id)&&id;
    const char* why=!spawned?"SPAWN_REFUSED":!l.place(id)?"PLACE_NOT_CONFIRMED":!l.grab(id)?"GRAB_NOT_CONFIRMED":nullptr;
    if(!why){o.ok=true;o.entity=id;o.why="SWAPPED";return o;}
    o.why=why;
    if(spawned&&!l.remove(id)){
        // the loose copy could not be removed: it stays as the prop (brush stays hidden,
        // physics stays suspended), so there is never a doubled mesh.
        o.keptLoose=true;o.entity=id;return o;
    }
    const bool restored=l.restore();
    const bool shown=l.hide(false);
    o.rollbackClean=restored&&shown;return o;
}
}
