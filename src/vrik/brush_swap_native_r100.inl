// R100 loose scenery props (swap): native leaves. Included by frik_pose_probe.cpp right
// before vrik/static_grab_r89.inl (game thread only, the same native rhythm tick).
// Every address below was read from WHGame.dll timestamp 6A350E20 (sha256 bdf8f9e4...).
// Code bytes and vtable slots are compared before the first use; a different game build
// switches only this feature off with one KCD2VR.log line. Proofs: REVIEW100_swap.md.
//  CBrush vtable +401FE70 (RTTI .?AVCBrush@@)
//   +0x0B0 GetEntityStatObj  +433F44: matrix copy from +0x50 (Matrix34, row major), IStatObj* at +0x98
//   +0x0F0 GetPhysics        +1A73360: IPhysicalEntity* at +0x80 (Physicalize +530C81 stores it there)
//   +0x220 Hide(bool)        +5DF758: toggles ERF_HIDDEN = bit 8 of the 64-bit flags at +0x28 and calls
//                            +0x228 (+400584) which tells the render-node container; shared by all 20
//                            render-node vtables (IRenderNode base implementation)
//  CStatObj vtable +3A2F888 (same pin as native_ammo_service_t22)
//   +0x148 GetFilePath +1A81E20 (char* at +0xA0)   +0x1C0 GetSubObjectCount +78F7A8 (+0x200..+0x208 / 0xB0)
//   +0x1D8 IsSubObject +1AAE310 (+0x17C bit 4)     +0x1E0 GetParentObject   +1AAE2B0 (+0x218)
//  CPhysicalEntity vtable +3A578C0: +0x50 GetForeignData +5D6F44 (short id +0x28, data +0x20);
//   sim class byte +0x47 (0 static, 0xFF while suspended - DestroyPhysicalEntity +8478F0/+8474EF)
//  CPhysicalWorld vtable +407FC80 +0x98 DestroyPhysicalEntity(pent, mode, bThreadSafe) +846E10:
//   mode 1 suspends into the world's hidden list, mode 2 restores; CBrush::Dephysicalize +53107E
//   calls the same slot.
namespace brush_swap_r100n {
namespace B=kcd2vr::brush_swap_r100;
namespace A=kcd2vr::subsystems::smithing::anvil_physics_a3;
constexpr std::uintptr_t kBrushVft=0x401FE70,kStatVft=0x3A2F888,kStaticBodyVft=0x3A578C0;
constexpr std::uintptr_t kHideRva=0x5DF758,kDestroyRva=0x846E10;
struct CodePin {std::uintptr_t rva;unsigned char n;unsigned char b[48];};
static constexpr CodePin kCode[]{
    {0x433F44,13,{0x4D,0x85,0xC0,0x75,0x08,0x48,0x8B,0x81,0x98,0x00,0x00,0x00,0xC3}}, // GetEntityStatObj
    {0x1A73360,8,{0x48,0x8B,0x81,0x80,0x00,0x00,0x00,0xC3}},                          // GetPhysics
    {kHideRva,44,{0x48,0x83,0xEC,0x28,0x4C,0x8B,0x41,0x28,0x49,0x8B,0xC0,0x48,0xC1,0xE8,0x08,0x24,0x01,0x3A,0xD0,0x74,0x12,0x48,0x8B,0x01,
        0x49,0x0F,0xBA,0xF8,0x08,0x4C,0x89,0x41,0x28,0xFF,0x90,0x28,0x02,0x00,0x00,0x48,0x83,0xC4,0x28,0xC3}},       // Hide
    {0x400584,20,{0x48,0x83,0xEC,0x28,0x4C,0x8B,0x49,0x20,0x4D,0x85,0xC9,0x74,0x1E,0x49,0x8B,0x01,0x48,0xBA,0x00,0x01}}, // hidden-changed
    {0x1A81E20,8,{0x48,0x8B,0x81,0xA0,0x00,0x00,0x00,0xC3}},                          // GetFilePath
    {0x78F7A8,14,{0x48,0x8B,0x81,0x08,0x02,0x00,0x00,0x48,0x2B,0x81,0x00,0x02,0x00,0x00}}, // GetSubObjectCount
    {0x1AAE310,12,{0x8B,0x81,0x7C,0x01,0x00,0x00,0xC1,0xE8,0x04,0x24,0x01,0xC3}},     // IsSubObject
    {0x1AAE2B0,8,{0x48,0x8B,0x81,0x18,0x02,0x00,0x00,0xC3}},                          // GetParentObject
    {0x5D6F44,13,{0x0F,0xBF,0x41,0x28,0x3B,0xC2,0x75,0x05,0x48,0x8B,0x41,0x20,0xC3}}, // GetForeignData
    {kDestroyRva,24,{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x18,0x48,0x89,0x78,0x20,0x55,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57}}, // DestroyPhysicalEntity
};
struct SlotPin {std::uintptr_t vft,off,target;};
static constexpr SlotPin kSlots[]{
    {kBrushVft,0xB0,0x433F44},{kBrushVft,0xF0,0x1A73360},{kBrushVft,0x220,kHideRva},{kBrushVft,0x228,0x400584},
    {kStatVft,0x148,0x1A81E20},{kStatVft,0x1C0,0x78F7A8},{kStatVft,0x1D8,0x1AAE310},{kStatVft,0x1E0,0x1AAE2B0},
    {kStaticBodyVft,0x50,0x5D6F44},{A::kWorldVtableRva,0x98,kDestroyRva},
};
static std::atomic<int> g_pinsR100{0}; // 0 not read yet, 1 exact, -1 different game code (one log line)
static bool SwapPinsR100() noexcept {
    const int s=g_pinsR100.load(std::memory_order_acquire);if(s)return s>0;
    const auto base=WhBase();if(!base)return false; // module not readable yet: ask again next grab
    bool ok=true;
    for(const auto& p:kCode){unsigned char got[48]{};
        if(!A::readRaw(base+p.rva,got,p.n)||std::memcmp(got,p.b,p.n)){ok=false;break;}}
    for(const auto& s2:kSlots){std::uintptr_t v=0;
        if(!ok||!A::readRaw(base+s2.vft+s2.off,&v,sizeof(v))||v!=base+s2.target){ok=false;break;}}
    g_pinsR100.store(ok?1:-1,std::memory_order_release);
    if(!ok)diagnostic_trace::Write("R100 BRUSH_SWAP game code differs from WHGame 6A350E20 (brush/statobj/physics pins): loose scenery swap is off for this game build");
    return ok;
}
template<class T> static bool RawR100(std::uintptr_t at,T& v) noexcept {return at&&A::readRaw(at,&v,sizeof(v));}
static bool TextR100(std::uintptr_t at,char* out,std::size_t n) noexcept {
    if(!out||n<2)return false;out[0]=0;if(!at)return false;
    for(std::size_t i=0;i+1<n;++i){char c=0;if(!A::readRaw(at+i,&c,1))return false;out[i]=c;if(!c)return true;}
    out[n-1]=0;return false; // longer than the buffer: refuse instead of cutting a path
}
// body = static physical entity from the palm query. Fills the copied brush facts.
static bool SwapReadBrushR100(std::uintptr_t body,B::Brush& out,const char*& why) noexcept {
    out=B::Brush{};why="GAME_CODE_MISMATCH";if(!SwapPinsR100())return false;
    const auto base=WhBase();std::uintptr_t vf=0,brush=0,link=0,stat=0;short id=-1;unsigned char sim=0xEE;std::uint64_t flags=0;
    why="BODY_NOT_A_STATIC_ENTITY";
    if(!RawR100(body,vf)||vf!=base+kStaticBodyVft||!RawR100(body+0x28,id)||id!=B::kForeignStatic||
       !RawR100(body+0x20,brush)||!brush||!RawR100(body+0x47,sim))return false;
    why="BODY_NOT_STATIC_SIMCLASS";if(sim!=0)return false;
    why="OWNER_NOT_A_BRUSH";if(!RawR100(brush,vf)||vf!=base+kBrushVft)return false;
    why="BRUSH_PHYSICS_LINK_MISMATCH";if(!RawR100(brush+0x80,link)||link!=body)return false;
    why="BRUSH_ALREADY_HIDDEN";if(!RawR100(brush+0x28,flags)||(flags&0x100ull))return false;
    why="BRUSH_HAS_NO_STATOBJ";if(!RawR100(brush+0x98,stat)||!stat||!RawR100(stat,vf)||vf!=base+kStatVft)return false;
    why="BRUSH_READ";
    if(!A::readRaw(brush+0x50,out.tm,sizeof(out.tm))||!A::readRaw(body+0x08,out.bb,sizeof(out.bb))||
       !RawR100(brush+0x88,out.material))return false;
    std::uint32_t bits=0;std::uintptr_t first=0,last=0,file=0,geo=0;
    if(!RawR100(stat+0x17C,bits)||!RawR100(stat+0x218,out.parent)||!RawR100(stat+0x200,first)||!RawR100(stat+0x208,last)||
       !RawR100(stat+0xA0,file)||!RawR100(stat+0xA8,geo))return false;
    out.isSub=(bits>>4)&1u;out.subCount=last>=first&&last-first<0xB0ull*4096?int((last-first)/0xB0):-1;
    why="MODEL_PATH_UNREADABLE";if(!TextR100(file,out.path,sizeof(out.path)))return false;
    if(geo)(void)TextR100(geo,out.geo,sizeof(out.geo));
    out.body=body;out.brush=brush;out.stat=stat;why="OK";return true;
}
static bool HiddenR100(std::uintptr_t brush,bool& hidden) noexcept {
    std::uint64_t flags=0;if(!RawR100(brush+0x28,flags))return false;hidden=(flags&0x100ull)!=0;return true;
}
static bool CallHideR100(void* brush,bool hide) noexcept {
    __try{reinterpret_cast<void(__fastcall*)(void*,bool)>(WhBase()+kHideRva)(brush,hide);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// IRenderNode::Hide through the exact CBrush slot; confirmed by ERF_HIDDEN afterwards.
static bool SwapHideR100(std::uintptr_t brush,bool hide) noexcept {
    std::uintptr_t vf=0;bool now=false;
    if(!SwapPinsR100()||!RawR100(brush,vf)||vf!=WhBase()+kBrushVft||!HiddenR100(brush,now))return false;
    if(now!=hide&&!CallHideR100(reinterpret_cast<void*>(brush),hide))return false;
    return HiddenR100(brush,now)&&now==hide;
}
static int CallDestroyR100(void* world,void* body,int mode) noexcept {
    __try{return reinterpret_cast<int(__fastcall*)(void*,void*,int,int)>(WhBase()+kDestroyRva)(world,body,mode,0);}
    __except(EXCEPTION_EXECUTE_HANDLER){return -1;}
}
// mode 1 = suspend (body leaves the world grid, stays alive and owned by the brush),
// mode 2 = restore. bThreadSafe 0: the world queues it itself while physics steps.
static bool SwapPhysicsR100(std::uintptr_t body,int mode) noexcept {
    if(!SwapPinsR100()||(mode!=1&&mode!=2))return false;
    const auto base=WhBase();void* world=nullptr;std::uintptr_t vft=0,slot=0,vf=0;
    if(!A::readRaw(base+A::kWorldSlotRva,&world,sizeof(world))||!world||!RawR100(reinterpret_cast<std::uintptr_t>(world),vft)||
       vft!=base+A::kWorldVtableRva||!RawR100(vft+0x98,slot)||slot!=base+kDestroyRva||!RawR100(body,vf)||vf!=base+kStaticBodyVft)return false;
    return CallDestroyR100(world,reinterpret_cast<void*>(body),mode)==1;
}
// Exact CEntity::SetPosRotScale (vft+0x168 = +509810, same leaf as PropPlace596).
static bool CallPlaceR100(std::uintptr_t fn,void* e,const QuatT28& q,const float* scale) noexcept {
    __try{reinterpret_cast<EntitySetPosRotScaleFn>(fn)(e,q.t,q.q,scale,0x10u);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool SwapPlaceR100(void* entity,const ph::Pose& pose,const float (&scale)[3]) noexcept {
    std::uintptr_t vf=0,fn=0;
    if(!entity||!Read(entity,vf)||vf!=WhBase()+kCEntityVftRva||!Read(reinterpret_cast<void*>(vf+0x168),fn)||fn!=WhBase()+0x509810||!ph::finite(pose))return false;
    return CallPlaceR100(fn,entity,PhysQuat6698(pose),scale);
}
// After a load the old brush may be gone. Only an object that still is exactly our
// suspended brush (vtables, two-way physics link, sim class 0xFF, hidden, same statobj)
// is restored; a new level's brush is never suspended, so it can never match.
static bool SwapRestoreEntryR100(const B::Entry& e) noexcept {
    if(!SwapPinsR100())return false;
    const auto base=WhBase();std::uintptr_t vf=0,link=0,owner=0,stat=0;short id=-1;unsigned char sim=0;bool hidden=false;
    if(!RawR100(e.brush,vf)||vf!=base+kBrushVft||!RawR100(e.brush+0x80,link)||link!=e.body||
       !RawR100(e.body,vf)||vf!=base+kStaticBodyVft||!RawR100(e.body+0x20,owner)||owner!=e.brush||
       !RawR100(e.body+0x28,id)||id!=B::kForeignStatic||!RawR100(e.brush+0x98,stat)||stat!=e.stat||
       !HiddenR100(e.brush,hidden))return false;
    bool ok=true;
    if(RawR100(e.body+0x47,sim)&&sim==0xFF)ok=SwapPhysicsR100(e.body,2)&&ok;
    if(hidden)ok=SwapHideR100(e.brush,false)&&ok;
    return ok;
}
static std::uint64_t SwapLoadEpisodeR100(bool& loading) noexcept {
    const auto w=kcd2vr::observed_load_recovery_r96::observe();loading=w.loading;return w.episode;
}
static bool SwapNoteReadyR100() noexcept {return kcd2vr::door_signal19c4r4::ArmSwapNoteR100();}
} // namespace brush_swap_r100n
