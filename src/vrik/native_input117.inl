// Retail upstream input-class selection. These hooks do NOT call RequestAttack,
// create actions, set attack result bits, or replace native eligibility checks.
namespace msH14=kcd2vr::master_gate47;
using NativeInputLeaf117=void(__fastcall*)(void*,int);
static NativeInputLeaf117 g_input_begin117=nullptr,g_input_release117=nullptr;
static std::optional<DetourModKit::hook::Hook> g_input_begin_hook117,g_input_release_hook117;
static std::atomic<std::uintptr_t> g_input_contract117{0};
static std::mutex g_input_arm_mutex117;
struct NativeInputScope117 {
    NativeOwner104 owner{};ph::CommandKind stroke=ph::CommandKind::None;
    bool prepare=false,visited=false,fistPunchR19=false;int passed=-1;
    bool kickSpecialD72=false;std::uint64_t kickSerialD72=0;
    kcd2vr::final_combo_d72::Permit finalD72{};
    bool releaseStableR24=false;std::uint64_t releaseSourceR24=0;
    std::uint64_t prepareSourceR18=0;bool prepareStableR18=false;
    kcd2vr::swing_effort59c14::Input54 costInput54{};
    kcd2vr::solver_command_h28::Stamp solverH28{};std::uint64_t sourceSampleH28=0;
};
static thread_local NativeInputScope117* g_input_scope117=nullptr;
// Exact semantic edge stack. A nested foreign native edge masks the outer R3
// permission; after it returns the changed source generation revokes the old
// edge. Deferred selectors use the separately retained native input source.
struct NativeInputEdgeD72 {
    NativeInputScope117* scope=nullptr;void* actor=nullptr;std::uint64_t source=0;bool owned=false;
};
static thread_local const NativeInputEdgeD72* g_native_input_edge_d72=nullptr;
struct NativeInputEdgeScopeD72 {
    NativeInputEdgeD72 edge{};const NativeInputEdgeD72* prior=nullptr;
    NativeInputEdgeScopeD72(bool owned,NativeInputScope117* scope,std::uint64_t source):
        edge{scope,owned&&scope?scope->owner.actor:nullptr,source,owned},prior(g_native_input_edge_d72){g_native_input_edge_d72=&edge;}
    ~NativeInputEdgeScopeD72(){g_native_input_edge_d72=prior;}
};
static std::atomic<std::uint64_t> g_input_begin51{0},g_input_requested51{0},g_input_release51{0},g_input_empty_release51{0},g_input_stab_begin51{0},g_input_stab_release51{0};
static NativeOwner104 g_input_prepared_owner117; // native-frame thread only
struct NativeReleasedInputH14 {
    NativeOwner104 owner{};kcd2vr::motion59c4::Key key{};std::uint64_t tick=0;int input=-1;
};
static NativeReleasedInputH14 g_released_input_h14;
static std::uint64_t g_input_source_generation_h14=1; // native input thread; copied across original callbacks
static bool NativeFreeKickInputOwnerR24(void*) noexcept;
static std::uint64_t NativePreparedSourceGenerationH26() noexcept {return g_input_source_generation_h14;}
struct NativePreparedInputR18 {
    NativeOwner104 owner{};std::uint64_t gesture=0,key=0,generation=0,request=0,tick=0,source=0;
    int input=-1;bool controlledHoldH26=false;
};
static NativePreparedInputR18 g_prepared_input_r18;
static void NativeInvalidateReleasedInputH14() noexcept {g_released_input_h14={};g_prepared_input_r18={};++g_input_source_generation_h14;}
static void NativeForgetSolverLineageH28(void* actor) noexcept;
static void NativeForeignInputH14(void* self,bool owned) noexcept {
    if(owned)return;void* actor=nullptr;
    if(self&&Read(static_cast<unsigned char*>(self)+0x18,actor)&&actor&&
       (actor==g_released_input_h14.owner.actor||actor==g_input_prepared_owner117.actor||
        (g_input_scope117&&actor==g_input_scope117->owner.actor)||NativeFreeKickInputOwnerR24(actor))){kcd2vr::final_combo_d72::revoke();NativeForgetSolverLineageH28(actor);NativeInvalidateReleasedInputH14();}
}
static bool NativeInputOwnedLeaf51(void* self,std::uintptr_t caller,bool prepare) noexcept {
    const auto* s=g_input_scope117;void* actor=nullptr;
    if(!s||!s->visited||s->prepare!=prepare||!self)return false;
    const bool special=s->stroke==ph::CommandKind::Pommel&&!s->finalD72.serial;
    return caller==WhBase()+(special?(prepare?0x27C0998:0x27C0952):(prepare?0x27C08E6:0x27C0916))&&
        Read(static_cast<unsigned char*>(self)+0x18,actor)&&actor==s->owner.actor;
}
static bool NativePreparedOwnerMatches117(void* player) noexcept {
    NativeOwner104 now{};const auto& old=g_input_prepared_owner117;
    return old.player==player&&NativeResolve104(player,now)&&now.actor==old.actor&&now.state==old.state&&now.epoch==old.epoch;
}
static int NativeInputClass117(void* self,int input,std::uintptr_t caller,bool prepare) noexcept {
    auto* s=g_input_scope117;
    // Normal input, AI and nested unrelated calls take the original fast path.
    if(!s||!self||s->prepare!=prepare||g_input_contract117.load()!=WhBase()||!retail_build112::Ready(WhBase()))return input;
    const bool special=s->stroke==ph::CommandKind::Pommel&&!s->finalD72.serial;
    const auto expected=WhBase()+(special?(prepare?0x27C0998:0x27C0952):(prepare?0x27C08E6:0x27C0916));
    if(caller!=expected)return input;
    void* actor=nullptr;NativeOwner104 now{};int heavy=-1,light=-1,specialId=-1;
    if(!Read(static_cast<unsigned char*>(self)+0x18,actor)||actor!=s->owner.actor||
       !NativeResolve104(s->owner.player,now)||now.actor!=s->owner.actor||now.state!=s->owner.state||now.epoch!=s->owner.epoch||
       !Read(reinterpret_cast<void*>(WhBase()+0x5321014),heavy)||
       !Read(reinterpret_cast<void*>(WhBase()+0x5321010),light)||
       !Read(reinterpret_cast<void*>(WhBase()+0x5321018),specialId)||
       heavy<0||heavy>128||light<0||light>128||specialId<0||specialId>128||heavy==light||specialId==light||specialId==heavy||
       input!=(special?specialId:heavy))return input;
    // Controller primary handlers hard-code heavy. A physical thrust supplies
    // the live light-input ID at BOTH edges, inside the same upstream lifecycle.
    // The native D4AAC4/D4A99C bodies still run all their original tests.
    // V66.178R55: with VrThrustAsSwing (default) a thrust is the ORDINARY attack from its origin zone, not the light/stab input.
    if(s->fistPunchR19){
        FistOwner101 fists{};
        if(!FistReadOwner101(fists)||!fists.empty||fists.player!=now.player||fists.epoch!=now.epoch||
           fists.request!=g_fist_native101.request)return input;
    }
    if(s->finalD72.serial){
        if(kcd2vr::final_combo_d72::live.load(std::memory_order_acquire)!=s->finalD72.serial||
           !kcd2vr::final_combo_d72::fresh(s->finalD72,GetTickCount64())||
           s->finalD72.player!=reinterpret_cast<std::uintptr_t>(now.player)||s->finalD72.actor!=reinterpret_cast<std::uintptr_t>(now.actor)||
           s->finalD72.state!=reinterpret_cast<std::uintptr_t>(now.state)||s->finalD72.body!=now.epoch||
           (s->finalD72.input!=heavy&&s->finalD72.input!=light))return input;
        s->visited=true;s->passed=s->finalD72.input;return s->passed;
    }
    if(s->kickSpecialD72&&s->kickSerialD72){
        // D72 additions use the existing SPECIAL family at BOTH original
        // primary-controller edges. HEAVY ranged fire cannot select them.
        s->visited=true;s->passed=specialId;return specialId;
    }
    s->visited=true;s->passed=s->fistPunchR19?heavy:
        (s->stroke==ph::CommandKind::Thrust&&!g_thrust_as_swing55.load(std::memory_order_relaxed))?light:input;
    return s->passed;
}
// The ordinary controller handlers supply attack_heavy at both native edges.
// H19: authored unarmed punch/hook rows use HEAVY, as do the native controller
// and the recorded H8 type6 hit. R19's LIGHT guess has no authored unarmed row.
// Keep exact empty-hand scope/edge proof; weapon thrust, reference, cleanup and
// native action/outcome selection remain unchanged.
static bool NativeInputArm117() noexcept;
static bool FistDispatchPunchR19(void* player,const std::uint64_t* action,int mode,bool prepare) noexcept {
    NativeOwner104 owner{};FistOwner101 fists{};void* input=nullptr;
    if(!action||mode!=(prepare?1:2)||!g_combat_input_original101||!NativeInputArm117()||
       !NativeResolve104(player,owner)||!FistReadOwner101(fists)||!fists.empty||
       fists.player!=player||fists.epoch!=owner.epoch||!g_fist_native101.owned||
       g_fist_native101.player!=player||g_fist_native101.epoch!=owner.epoch||
       g_fist_native101.request!=fists.request||
       !Read(static_cast<unsigned char*>(player)+0xB88,input))return false;
    NativeInputScope117 scope{};scope.owner=owner;scope.prepare=prepare;
    scope.stroke=ph::CommandKind::Slash;scope.fistPunchR19=true;
    auto* prior=g_input_scope117;g_input_scope117=&scope;
    __try{g_combat_input_original101(input,action,mode,mode==2?0.f:1.f);}
    __except(EXCEPTION_EXECUTE_HANDLER){g_input_scope117=prior;g_fist_fault101=true;return false;}
    g_input_scope117=prior;
    // Both exact native input edges must be observed. A returned handler that
    // never visited this semantic boundary is not a completed punch transport.
    int heavy=-1;
    const bool observed=scope.visited&&Read(reinterpret_cast<void*>(WhBase()+0x5321014),heavy)&&scope.passed==heavy;
    if(cc::active(GetTickCount64())){char line[240]{};std::snprintf(line,sizeof(line),
        "H19 FIST_INPUT edge=%s gesture=%llu boundaryObserved=%d liveHeavy=%d passed=%d transport=%d selection=NATIVE damage=NOT_INFERRED",
        prepare?"PREPARE":"RELEASE",(unsigned long long)(prepare?g_fist_native101.lastSample:g_fist_native101.pendingSample),
        scope.visited?1:0,heavy,scope.passed,observed?1:0);diagnostic_trace::Write(line);}
    return observed;
}
// C23b OUTCOME_CHAIN (receipts only): the owned prepare/commit edge opens/joins the gesture's chain with the
// live +7D8 count already read for MELEE_INPUT; the XI handler return adds the count D4AD94 saw. No native effect.
static void OutcomeEdge23(const ph::Command& c,bool prepare,int ms7D8) noexcept {
    if(!c.gesture59c3||c.kick59)return;
    const auto now=std::uint64_t(GetTickCount64());
    const bool factory=kcd2vr::outcome23::factoryArmed.load(std::memory_order_relaxed)&&CombatExperiment175Enabled();
    const bool slot=kcd2vr::outcome23::slotArmed.load(std::memory_order_relaxed);
    kcd2vr::outcome23::with([&](kcd2vr::outcome23::Core& k){k.edge(c.gesture59c3,int(c.stroke117),prepare,ms7D8,factory,slot,now,kcd2vr::outcome23::counters);});
}
static void OutcomeXi23(bool prepare,int ms7D8) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    kcd2vr::outcome23::with([&](kcd2vr::outcome23::Core& k){k.xi(prepare,ms7D8,now,kcd2vr::outcome23::counters);});
}
// V66.178R13: the game-side dispatch of OUR XI prepare/release runs the retail handler with the
// same input-class scope the direct path used, so a physical thrust selects the light input id.
static void XiOwnedCall178r13(void* input,const std::uint64_t* action,int mode,float value,void* player,unsigned group,const char* name,std::uint64_t now) noexcept {
    NativeInputScope117 scope{};bool scoped=false;
    const int pending=kcd2vr::attack_xi178r8::dispatchedStroke51>=0?kcd2vr::attack_xi178r8::dispatchedStroke51:
        kcd2vr::attack_xi178r8::pendingStroke.load(std::memory_order_relaxed);
    if(group==1&&pending&&(mode==1||mode==2)&&NativeResolve104(player,scope.owner)){
        scope.stroke=pending==2?ph::CommandKind::Thrust:pending==3?ph::CommandKind::Pommel:ph::CommandKind::Slash;scope.prepare=mode==1;scoped=true;}
    if(scoped){namespace se=kcd2vr::swing_effort59c14;se::Input54 input54{};
        const bool found54=scope.prepare?se::inputRegistry54.read(kcd2vr::attack_xi178r8::dispatchedCost54,now,input54):
            se::inputRegistry54.readHeld54(kcd2vr::attack_xi178r8::dispatchedCost54,now,input54);
        if(found54&&
           input54.packetSerial54==kcd2vr::attack_xi178r8::dispatchedSerial54&&input54.packetStroke54==unsigned(pending)&&
           input54.key.player==reinterpret_cast<std::uintptr_t>(player)&&input54.actor==reinterpret_cast<std::uintptr_t>(scope.owner.actor)&&
           input54.state==reinterpret_cast<std::uintptr_t>(scope.owner.state)&&input54.key.body==scope.owner.epoch&&
           input54.key.recovery==kcd2vr::contact176::recoveryEpoch55c1.load()&&input54.key.request==g_weapon_request_epoch6694.load()&&
           kcd2vr::solver_rpg_h29::costOwned(input54,frik_pose_probe::GeometryIntentOwnerH30,
               frik_pose_probe::GeometrySolverCostOwnerValidH28)){
            scope.costInput54=input54;scope.solverH28=input54.solverH28;scope.sourceSampleH28=input54.solverH28.sourceSample;}

    }
    if(scoped){kcd2vr::final_combo_d72::Permit final{};
        if(kcd2vr::final_combo_d72::mailbox.read(final)&&final.player==reinterpret_cast<std::uintptr_t>(player)&&
           final.actor==reinterpret_cast<std::uintptr_t>(scope.owner.actor)&&final.state==reinterpret_cast<std::uintptr_t>(scope.owner.state)&&
           final.body==scope.owner.epoch&&final.physicalKind==pending&&final.xiSerial&&
           final.xiSerial==kcd2vr::attack_xi178r8::prepareSerial.load(std::memory_order_acquire)&&
           (final.xiSerial&255u)==kcd2vr::attack_xi178r8::dispatchedSerial54)scope.finalD72=final;
    }
    kcd2vr::final_combo_d72::DispatchScope dispatchedFinalD72(&scope.finalD72);
    kcd2vr::finisher_cb1::ReleaseScope finalScopeD72(scope.finalD72.serial?
        (pending==3?kcd2vr::finisher_cb1::Kind::Pommel:pending==2?kcd2vr::finisher_cb1::Kind::Thrust:kcd2vr::finisher_cb1::Kind::Cut):
        kcd2vr::finisher_cb1::Kind::None,scope.finalD72.gesture);
    int ms7D8xi23=-2; // C23: -2 not an owned attack edge, -1 unreadable, else the live count D4AD94 is about to test
    if(scoped&&!NativeMasterCount23(scope.owner,ms7D8xi23))ms7D8xi23=-1;
    struct Saved{NativeInputScope117* prev;~Saved(){g_input_scope117=prev;}} saved{g_input_scope117};
    g_input_scope117=scoped?&scope:nullptr; // an unrelated nested dispatch cannot borrow the outer stab class
    InvalidateReadCache101R2();if(g_combat_input_original101){kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::CombatInput};g_combat_input_original101(input,action,mode,value);}InvalidateReadCache101R2();
    g_input_scope117=saved.prev;
    if(scoped)kcd2vr::attack_xi178r8::receipt49(scope.prepare,scope.visited);
    if(scoped)OutcomeXi23(scope.prepare,ms7D8xi23); // C23b receipt (after the handler returned)
    const bool ours=group==1||!std::strcmp(name,"prepare_to_attack")||!std::strcmp(name,"attack_primary")||!std::strcmp(name,"attack_abort")||!std::strcmp(name,"block")||!std::strcmp(name,"attack_special"); // V66.178R20: the game flushes unrelated queued actions inside our dispatch; log only the combat edges
    if(ours&&(cc::active(now)||(scoped&&ms7D8xi23>0))){char m[300]{};std::snprintf(m,sizeof(m),"V66.178R13 XI_RECEIPT action=%s mode=%d stage=HANDLER_RETURN stroke=%d inputClassVisited=%d passed=%d ms7D8=%d nativeAdmission=UNVERIFIED damage=NOT_INFERRED",name,mode,pending,scope.visited?1:0,scope.passed,ms7D8xi23);diagnostic_trace::Write(m);}
}
static void __fastcall NativeInputBegin117(void* self,int input) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::InputBegin};
    kcd2vr::attack_xi178r8::HookTimer178r22 ht22;
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const int cls=NativeInputClass117(self,input,caller,true);
    const bool ownedH14=NativeInputOwnedLeaf51(self,caller,true);
    NativeForeignInputH14(self,ownedH14);
    if(ownedH14&&(!g_input_scope117||!g_input_scope117->finalD72.serial))kcd2vr::final_combo_d72::revoke();
    if(ownedH14)NativeInvalidateReleasedInputH14(); // a new preparation cannot reuse the old release's pending input
    auto* prepareScopeR18=ownedH14?g_input_scope117:nullptr;
    const auto prepareSourceR18=ownedH14?g_input_source_generation_h14:0;
    if(prepareScopeR18){prepareScopeR18->prepareSourceR18=prepareSourceR18;prepareScopeR18->prepareStableR18=false;}
    namespace se54=kcd2vr::swing_effort59c14;se54::NativeCost54 prep54{};
    if(auto* s=g_input_scope117;NativeInputOwnedLeaf51(self,caller,true)&&se54::validInput54(s->costInput54)){
        prep54.key=s->costInput54.key;prep54.actor=reinterpret_cast<std::uintptr_t>(s->owner.actor);prep54.state=reinterpret_cast<std::uintptr_t>(s->owner.state);
        prep54.preparationOnly54=true;prep54.observedTick54=GetTickCount64();
        if(Read(static_cast<unsigned char*>(s->owner.state)+0x218,prep54.type)&&Read(static_cast<unsigned char*>(s->owner.state)+0x1B8,prep54.hand)&&prep54.hand>=0&&prep54.hand<=1&&
           Read(static_cast<unsigned char*>(s->owner.state)+0x8C8+8*unsigned(prep54.hand),prep54.nativeWeapon54))prep54.nativeFields54=true;
    }
    se54::NativeCostScope54 prepScope54(prep54);
    NativeInputEdgeScopeD72 edgeD72(ownedH14,g_input_scope117,g_input_source_generation_h14);
    if(g_input_begin117){kcd2vr::attack_xi178r8::OriginalTimer178r22 ot22;kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::InputBegin};g_input_begin117(self,cls);}
    const bool prepProof54=prepScope54.finish(g_input_begin117&&NativeInputOwnedLeaf51(self,caller,true),GetTickCount64());
    if(prepareScopeR18&&g_input_scope117==prepareScopeR18)
        prepareScopeR18->prepareStableR18=g_input_begin117&&prepareSourceR18==g_input_source_generation_h14&&
            NativeInputOwnedLeaf51(self,caller,true);
    if(prep54.valid()&&diagnostics::combat_capture::active(GetTickCount64())){char cost54[310]{};std::snprintf(cost54,sizeof(cost54),
        "C54 NATIVE_INPUT_COST gesture=%llu nonce=%u actualType=%d actualHand=%d nativeWeapon=%llu raised=%u invalid=%d proof=%d credit=NONE deltaOverride=NONE",
        (unsigned long long)prep54.key.gesture,kcd2vr::attack_xi178r8::dispatchedCost54,prep54.type,prep54.hand,
        (unsigned long long)prep54.nativeWeapon54,prepScope54.value.raised,int(prepScope54.value.invalid),int(prepProof54));diagnostic_trace::Write(cost54);}
    if(auto* s=g_input_scope117;NativeInputOwnedLeaf51(self,caller,true)){
        ++g_input_begin51;if(s->stroke==ph::CommandKind::Thrust)++g_input_stab_begin51;
        unsigned char requested=0;if(Read(static_cast<unsigned char*>(s->owner.state)+0x2A8,requested)&&requested==1)++g_input_requested51;
    }
}
static void __fastcall NativeInputRelease117(void* self,int input) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::InputRelease};
    kcd2vr::attack_xi178r8::HookTimer178r22 ht22;
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const int cls=NativeInputClass117(self,input,caller,false);
    const bool ownedH14=NativeInputOwnedLeaf51(self,caller,false);
    NativeForeignInputH14(self,ownedH14);
    if(ownedH14)NativeInvalidateReleasedInputH14(); // no older source proof survives an interrupted/reentrant original release
    const auto sourceGenerationH14=g_input_source_generation_h14;
    const auto* releaseScopeH14=g_input_scope117;
    namespace se54=kcd2vr::swing_effort59c14;const se54::Input54* released54=nullptr;
    if(auto* s=g_input_scope117;NativeInputOwnedLeaf51(self,caller,false)&&se54::validInput54(s->costInput54))released54=&s->costInput54;
    se54::ReleaseScope54 release54(released54); // foreign/nested calls explicitly mask outer ancestry
    if(auto* s=g_input_scope117;NativeInputOwnedLeaf51(self,caller,false)){
        ++g_input_release51;if(s->stroke==ph::CommandKind::Thrust)++g_input_stab_release51;
        unsigned char requested=255;if(Read(static_cast<unsigned char*>(s->owner.state)+0x2A8,requested)&&requested==0)++g_input_empty_release51;
    }
    NativeInputEdgeScopeD72 edgeD72(ownedH14,g_input_scope117,g_input_source_generation_h14);
    if(g_input_release117){kcd2vr::attack_xi178r8::OriginalTimer178r22 ot22;kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::InputRelease};g_input_release117(self,cls);}
    if(ownedH14){
        // A native request remaining set AFTER our observed original release
        // is distinct from a later manual/foreign press with the same input ID.
        // Nested unowned input revokes the transaction; it cannot be restored
        // when this outer release returns. No native lifetime is extended.
        NativeReleasedInputH14 released{};NativeOwner104 current{};unsigned char pending=0;int actual=-1;
        auto* scope=g_input_scope117;
        if(g_input_release117&&scope&&scope==releaseScopeH14&&released54&&sourceGenerationH14==g_input_source_generation_h14&&
           scope->stroke!=ph::CommandKind::Pommel&&NativeResolve104(scope->owner.player,current)&&
           current.actor==scope->owner.actor&&current.state==scope->owner.state&&current.epoch==scope->owner.epoch&&
           Read(static_cast<unsigned char*>(current.state)+0x2A8,pending)&&pending==1&&
           Read(static_cast<unsigned char*>(current.state)+0x248,actual)&&actual==cls){
            released.owner=current;released.key=released54->key;released.tick=GetTickCount64();released.input=cls;
        }
        if(sourceGenerationH14==g_input_source_generation_h14){
            NativeInvalidateReleasedInputH14();g_released_input_h14=released;
            if(scope&&scope==releaseScopeH14&&NativeInputOwnedLeaf51(self,caller,false)){
                scope->releaseStableR24=g_input_release117!=nullptr;scope->releaseSourceR24=g_input_source_generation_h14;
            }
        }
    }
}
static bool NativeInputContract117(std::uintptr_t b) noexcept {
    if(!SupportedWhBuild(b)||!retail_build112::Ready(b))return false;
    struct Pin{std::uintptr_t rva;unsigned char bytes[16];};
    const Pin pins[]{
        {0xD4AAC4,{0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x74,0x24,0x20,0x57,0x48,0x83,0xec,0x30,0x48}},
        {0xD4A99C,{0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x83,0xec,0x20,0x48}},
        {0x27C08C8,{0x40,0x53,0x48,0x83,0xec,0x20,0x83,0x79,0x38,0,0x48,0x8b,0xd9,0x75,0x16,0x8b}},
        {0x27C08F8,{0x40,0x53,0x48,0x83,0xec,0x20,0x83,0x79,0x38,1,0x48,0x8b,0xd9,0x75,0x0f,0x8b}},
        {0xD4A7EC,{0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b,0xd9,0x41,0x83,0xf9,1,0x75,0x17,0x48}},
        {0x27C0928,{0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b,0xd9,0x41,0x83,0xe9,1,0x74,0x24,0x41}}
    };
    for(const auto& p:pins){unsigned char actual[16]{};
        if(!Read(reinterpret_cast<void*>(b+p.rva),actual)||std::memcmp(actual,p.bytes,16))return false;}
    return true;
}
#include "native_admission51.inl"
static bool NativeInputArm117() noexcept {
    const auto b=WhBase();std::lock_guard<std::mutex> lock(g_input_arm_mutex117);
    if(b&&g_input_contract117.load()==b)return true;
    if(!NativeInputContract117(b))return false;
    DetourModKit::hook::InlineRequest a{"kcd2vr_v66117_input_begin",DetourModKit::hook::Target{DetourModKit::Address{b+0xD4AAC4}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(a),&NativeInputBegin117);if(!h)return false;
    g_input_begin_hook117=std::move(*h);g_input_begin117=g_input_begin_hook117->original<NativeInputLeaf117>();
    DetourModKit::hook::InlineRequest z{"kcd2vr_v66117_input_release",DetourModKit::hook::Target{DetourModKit::Address{b+0xD4A99C}},DetourModKit::hook::Options{}};
    auto j=DetourModKit::hook::inline_at(std::move(z),&NativeInputRelease117);
    if(j){g_input_release_hook117=std::move(*j);g_input_release117=g_input_release_hook117->original<NativeInputLeaf117>();}
    if(!g_input_begin117||!g_input_release117||!g_input_begin_hook117->enable()||!g_input_release_hook117->enable()){
        g_input_begin_hook117.reset();g_input_release_hook117.reset();g_input_begin117=g_input_release117=nullptr;return false;
    }
    g_input_contract117.store(b,std::memory_order_release);
    diagnostic_trace::Write(ArmAdmission51()?"V66.178R51C1 ADMISSION_HOOK ready=1":"V66.178R51C1 ADMISSION_HOOK ready=0 nativeRules=UNCHANGED");return true;
}
static void ShutdownNativeInput117() noexcept {
    ShutdownAdmission51();
    std::lock_guard<std::mutex> lock(g_input_arm_mutex117);g_input_contract117=0;
    if(g_input_begin_hook117){(void)g_input_begin_hook117->disable();g_input_begin_hook117.reset();}
    if(g_input_release_hook117){(void)g_input_release_hook117->disable();g_input_release_hook117.reset();}
    g_input_begin117=g_input_release117=nullptr;
}
static thread_local const NativeOwner104* g_reject_owner23=nullptr; // C23: exact owner already resolved for this edge, if any
static bool NativeInputReject117(const char* reason,const ph::Command& c,const nc104::ReadReceipt59* read59=nullptr,const nc104::Observed* partial59=nullptr) noexcept {
    ++g_rung1_reject30c1;g_rung1_last_reject30c1=reason;
    int ms7D8=-1;if(g_reject_owner23)(void)NativeMasterCount23(*g_reject_owner23,ms7D8); // -1: owner unresolved or unreadable
    char msg[680]{};std::snprintf(msg,sizeof(msg),"V66.117 MELEE_INPUT rejected=%s stroke=%u zone=%d sample=%llu kickRequest=%llu ms7D8=%d msHandoff=%llu",
        reason,unsigned(c.stroke117),c.zone117,(unsigned long long)c.sample,(unsigned long long)(c.kick59?c.kickTicket59c3.id:0),ms7D8,(unsigned long long)c.masterHandoff23);
    if(read59&&partial59&&cc::active(c.tick)){
        const auto used=std::strlen(msg);std::snprintf(msg+used,sizeof(msg)-used,
            " observe59{why=%s offset=0x%X readMask=0x%X phase=%d type=%d input=%d zone=%d hand=%d reqRaw=%u candidateRaw=%u blockingRaw=%u validity=UNTRUSTED_PARTIAL extraReads=0}",
            read59->why,read59->offset,read59->readMask,partial59->phase,partial59->type,partial59->input,partial59->zone,partial59->hand,
            read59->requested,read59->candidate,read59->blocking);
    }
    diagnostic_trace::Write(msg);return false;
}
// R57C1: feint changes only the zone of OUR still-held ordinary input.
// H20: the native redirect must see the OLD desired zone. A preceding bare
// 90F824 setter changes +188 and suppresses 936A28's phase4 feint notification.
// This is a replacement for that setter on one qualified cut refinement only.
// Native code retains its own action, checks ability/rows, and notifies the NPC.
using NativeCutFeintFnH20=bool(__fastcall*)(void*,std::int32_t,std::int32_t);
struct NativeCutFeintBindingH20 {std::uintptr_t base=0;NativeCutFeintFnH20 call=nullptr;};
static NativeCutFeintBindingH20 g_cut_feint_h20;
struct NativeCutFeintPinH20 {std::uintptr_t rva;unsigned size;unsigned char bytes[16];};
static constexpr NativeCutFeintPinH20 kCutFeintPinsH20[]{
    {0x936A28,16,{0x48,0x89,0x5C,0x24,0x20,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57}},
    {0x936C79,16,{0x45,0x3B,0xEF,0x0F,0x84,0x81,0x00,0x00,0x00,0xBA,0x04,0x00,0x00,0x00,0x48,0x8B}},
    {0x936C93,16,{0x48,0x8B,0xCF,0xE8,0xD9,0x9B,0x31,0x00,0x84,0xC0,0x75,0x64,0x48,0x8D,0x8F,0x48}},
};
static bool NativeCutFeintContractH20(std::uintptr_t base) noexcept {
    if(!base||!retail_build112::Ready(base)||g_native_fault104.load())return false;
    for(const auto& pin:kCutFeintPinsH20){unsigned char bytes[16]{};
        for(unsigned i=0;i<pin.size;++i)if(!Read(reinterpret_cast<void*>(base+pin.rva+i),bytes[i]))return false;
        if(std::memcmp(bytes,pin.bytes,pin.size))return false;
    }
    return true;
}
static bool NativeCutFeintCallH20(void* actor,int zone,int guard) noexcept {
    __try {const bool changed=g_cut_feint_h20.call(actor,zone,guard);InvalidateReadCache101R2();return changed;}
    __except(EXCEPTION_EXECUTE_HANDLER){g_native_fault104.store(true);InvalidateReadCache101R2();return false;}
}
static bool NativeCutFeintH20(const NativeOwner104& owner,const ph::Command& c) noexcept {
    const auto base=WhBase();int guard=-1,oldZone=-1;
    if(!NativeCutFeintContractH20(base))return NativeInputReject117("CUT_FEINT_RETAIL_CONTRACT",c);
    if(!owner.actor||!owner.state||!Read(static_cast<unsigned char*>(owner.state)+0xC8,guard)||
       !Read(static_cast<unsigned char*>(owner.state)+0x188,oldZone))
        return NativeInputReject117("CUT_FEINT_GUARD_OR_OLD_ZONE_UNREADABLE",c);
    if(oldZone==c.zone117)return NativeInputReject117("CUT_FEINT_NO_ZONE_CHANGE",c);
    if(g_cut_feint_h20.base!=base||!g_cut_feint_h20.call)
        g_cut_feint_h20={base,reinterpret_cast<NativeCutFeintFnH20>(base+0x936A28)};
    // A refusal/substitution can still have native side effects. Spend the
    // original one-change permission BEFORE crossing callbacks; never retry it
    // or fall back to a bare setter. No new input, cost or contact is published.
    g_phys_native6698.cutFeintUsed62=true;
    return NativeCutFeintCallH20(owner.actor,c.zone117,guard)||
        NativeInputReject117("CUT_FEINT_NATIVE_REFUSED_OR_UNCHANGED",c);
}
namespace solverH28=kcd2vr::solver_command_h28;
// A typed solver proof cannot downgrade to the legacy tracked-ray path.
static bool NativeSolverOwnerCurrentH28(const solverH28::Stamp& proof,const PhysFrame6698& f,
 std::uint64_t now,bool originalClock=true) noexcept {
 if(solverH28::empty(proof)||!f.output.solverOwnedT4[0]||solverH28::empty(f.output.solverH28)||
    !f.input.allowed||!f.input.physicalCombat177||!f.input.coherentMelee119||!f.input.tracked[0]||
    !kcd2vr::contact176::fresh(f.input.tick,now,80)||proof.sourceSample>f.input.sample||
    proof.owner.player!=reinterpret_cast<std::uintptr_t>(f.player)||proof.owner.manager!=reinterpret_cast<std::uintptr_t>(f.manager)||
    proof.owner.key!=f.profiles[0].key||proof.owner.generation!=f.profiles[0].generation||
    proof.owner.epoch!=f.input.bodyEpoch||proof.owner.request!=f.request||
    f.output.solverLeaseT4!=proof.colliderLease||f.output.solverWorldGenT4!=proof.worldGeneration||
    f.output.solverBodyGenT4!=proof.bodyGeneration||f.output.solverTrackingEpochH24!=proof.trackingEpoch||
    f.output.solverCancelGenerationH24!=proof.cancelGeneration||
    !ph::attack_authority55c1(f.input,f.output,0))return false;
 NativeHeldSourceT4 source{};heldT4::Achieved achieved{};heldT4::ContactOwnerH24 contact{};
 const auto admission=heldT4::AdmissionSerial();
 if(!heldT4::HasUnretiredCollider()||!g_native_held_source_t4.read(source)||!source.valid||
    source.input.owner!=proof.owner||source.input.colliderLease!=proof.colliderLease||
    source.input.trackingEpoch!=proof.trackingEpoch||source.input.cancelGeneration!=proof.cancelGeneration||
    !NativeHeldSourceCurrentT4(source.input)||!heldT4::Read(achieved)||!heldT4::ReadContactOwnerH24(contact))return false;
 const bool current=originalClock?solverH28::current(proof,achieved,contact,admission,now):
    solverH28::currentIncarnation(proof,achieved,contact,admission,now);
 return current&&solverH28::current(f.output.solverH28,achieved,contact,admission,now)&&
    admission==heldT4::AdmissionSerial();
}
static bool NativeMeleeCommandCurrentH28(const ph::Command& c,const PhysFrame6698& f) noexcept {
 const auto recoveryH30=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
 if((c.recoveryEpochH30&&c.recoveryEpochH30!=recoveryH30)||
    (!c.recoveryEpochH30&&c.side>=0&&c.side<2&&f.output.normalIntentH30[c.side].current))return false;
 if(solverH28::empty(c.solverH28)&&!f.output.solverOwnedT4[0]&&solverH28::empty(f.output.solverH28))return NativeHeldContactsCurrentT4();
 const auto& p=c.solverH28;
 return c.side==0&&!c.kick59&&c.gesture59c3&&p.owner.key==c.key&&p.owner.generation==c.generation&&
    p.owner.epoch==c.bodyEpoch&&p.owner.request==c.epoch&&p.sourceSample<=c.sample&&p.sourceTick<=c.tick&&
    NativeSolverOwnerCurrentH28(p,f,GetTickCount64());
}
struct NativeSolverLineageH28 {solverH28::Stamp proof{};kcd2vr::motion59c4::Key key{};NativeOwner104 owner{};std::uint64_t sample=0;};
static std::array<NativeSolverLineageH28,16> g_solver_lineage_h28{};static unsigned g_solver_lineage_next_h28=0;
static std::mutex g_solver_lineage_mutex_h28;
static void NativeForgetSolverLineageH28(void* actor) noexcept {
 const std::lock_guard<std::mutex> lock(g_solver_lineage_mutex_h28);
 for(auto& l:g_solver_lineage_h28)if(!actor||l.owner.actor==actor)l={};
}
static void NativeForgetSolverGestureH28(void* player,std::uint64_t gesture) noexcept {
 if(!gesture)return;const std::lock_guard<std::mutex> lock(g_solver_lineage_mutex_h28);
 for(auto& l:g_solver_lineage_h28)if(l.owner.player==player&&l.key.gesture==gesture)l={};
}
static void NativeRememberSolverLineageH28(const ph::Command& c,const NativeInputScope117& scope) noexcept {
 if(solverH28::empty(c.solverH28)||!kcd2vr::motion59c4::valid(scope.costInput54.key))return;
 const std::lock_guard<std::mutex> lock(g_solver_lineage_mutex_h28);
 for(auto& l:g_solver_lineage_h28)if(kcd2vr::motion59c4::same(l.key,scope.costInput54.key)){
    l={c.solverH28,scope.costInput54.key,scope.owner,c.sample};return;}
 auto& l=g_solver_lineage_h28[g_solver_lineage_next_h28++%g_solver_lineage_h28.size()];
 l={c.solverH28,scope.costInput54.key,scope.owner,c.sample};
}
static bool NativeSolverLineageForKeyH28(std::uint64_t gesture,std::uint64_t key,std::uint64_t generation,
 std::uint64_t epoch,std::uint64_t request,NativeSolverLineageH28& out) noexcept {
 const auto matches=[&](const kcd2vr::motion59c4::Key& k){return k.gesture==gesture&&k.weapon==key&&k.generation==generation&&
    k.body==epoch&&k.request==request&&k.recovery==kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);};
 if(const auto* scope=g_input_scope117;scope&&matches(scope->costInput54.key)&&!solverH28::empty(scope->solverH28)){
    out={scope->solverH28,scope->costInput54.key,scope->owner,scope->sourceSampleH28};return true;}
 const std::lock_guard<std::mutex> lock(g_solver_lineage_mutex_h28);
 for(const auto& l:g_solver_lineage_h28)if(matches(l.key)&&!solverH28::empty(l.proof)){out=l;return true;}
 return false;
}
static bool NativeSolverLineageOwnerCurrentH28(const NativeSolverLineageH28& l,const PhysFrame6698& f,std::uint64_t now) noexcept {
 NativeOwner104 current{};
 return NativeResolve104(l.owner.player,current)&&current.player==l.owner.player&&current.actor==l.owner.actor&&
    current.state==l.owner.state&&current.entity==l.owner.entity&&current.epoch==l.owner.epoch&&
    l.key.player==reinterpret_cast<std::uintptr_t>(current.player)&&NativeSolverOwnerCurrentH28(l.proof,f,now,false);
}
// D34 FE VrFeintHybrid (proof and rules: feint_hybrid_fe.h). One native feint of the COMMITTED HYBRID cut per
// stroke, through the same native function the player's own direction input reaches (C5D728 -> 936A28), only while
// Henry is still in phase 4 and the proven opponent's block helper has not taken its row (+5D0). Every refusal leaves
// the committed native attack untouched (no cancel, no other write).
static void NativeFeintTraceFE(const ph::Command& c,const kcd2vr::feint_fe::GameState& g,kcd2vr::feint_fe::Gate why,
    bool returned,int observed,std::uint64_t now) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
    char line[420]{};std::snprintf(line,sizeof(line),
        "FE FEINT stroke=%llu from=%d to=%d route=%s npcBlockStarted=%d returned=%d observedF8=%d henryPhase=%d npcPhase=%d npc5D0=%d sinceCommitMs=%lld asked=%llu feints=%llu refused=%llu",
        (unsigned long long)c.gesture59c3,g.oldZone,c.zone117,kcd2vr::feint_fe::name(why),
        kcd2vr::feint_fe::npcBlockStarted(g.npcPhase,g.npc5D0)?1:0,returned?1:0,observed,g.henryPhase,g.npcPhase,g.npc5D0,
        g.sinceCommitMs==~0ull?-1ll:(long long)g.sinceCommitMs,(unsigned long long)kcd2vr::feint_fe::asked.load(),
        (unsigned long long)kcd2vr::feint_fe::feints.load(),(unsigned long long)kcd2vr::feint_fe::refused.load());
    diagnostic_trace::Write(line);
}
static bool NativeHybridFeintFE(void* player,const ph::Command& c,const PhysFrame6698& f,PhysNativeOwner6698& n,std::uint64_t now) noexcept {
    namespace fe=kcd2vr::feint_fe;
    fe::GameState g{};g.on=fe::enabled.load(std::memory_order_relaxed);g.target=c.zone117;
    g.hybrid=CombatSynchronized59c10r9()&&!CombatIntent59c16()&&!kcd2vr::motion59c4::selected.load(std::memory_order_acquire);
    NativeOwner104 owner{};std::uint64_t key=0;
    g.owner=c.feintFE&&c.kind==ph::CommandKind::Zone57&&c.side==0&&!c.kick59&&!c.held57&&c.gesture59c3&&
        c.stroke117==ph::CommandKind::Slash&&!n.attackPrepared&&n.committedGestureFE==c.gesture59c3&&
        n.prepareGestureC10R18==c.gesture59c3&&n.stroke117==ph::CommandKind::Slash&&n.attackKey==c.key&&
        n.attackGeneration117==c.generation&&n.player==f.player&&f.player==player&&n.epoch==c.bodyEpoch&&n.request==c.epoch&&
        c.epoch==f.request&&c.epoch==g_weapon_request_epoch6694.load()&&c.bodyEpoch==f.input.bodyEpoch&&c.bodyEpoch==g_pose_epoch6689.load()&&
        now>=c.tick&&now-c.tick<=100&&g_gameplay_allowed.load()&&g_head_valid.load()&&g_right_valid.load()&&
        retail_build112::Ready(WhBase())&&f.manager==g_henry_item_attach_manager.load()&&
        NativeResolve104(player,owner)&&NativeHandKey6697(f.manager,0,key)&&key==c.key;
    g.used=n.cutFeintUsed62;
    if(n.committedTickFE&&now>=n.committedTickFE)g.sinceCommitMs=now-n.committedTickFE;
    int guard=-1;nc104::Observed henry{};NativeParryOpponent176 opponent{};
    if(g.owner){
        g.readable=NativeObserve104(owner,henry)&&Read(static_cast<unsigned char*>(owner.state)+0xC8,guard)&&
            Read(static_cast<unsigned char*>(owner.state)+0x188,g.oldZone);
        if(g.readable)g.henryPhase=henry.phase;
        g.opponent=NativeParryOpponentRead176(owner,opponent)&&opponent.state;
        if(g.opponent){g.npcPhase=opponent.observedPhase19c4r1;
            if(!Read(static_cast<unsigned char*>(opponent.state)+0x5D8,g.npc5D0))g.opponent=false;}
        unsigned char slot=0;g.comboSlot=!Read(static_cast<unsigned char*>(owner.state)+0x868,slot)||slot==1;
    }
    auto why=fe::gate(g);
    const auto base=WhBase();
    if(why==fe::Gate::Pass&&!NativeCutFeintContractH20(base))why=fe::Gate::Unreadable;
    if(why!=fe::Gate::Pass){fe::refused.fetch_add(1,std::memory_order_relaxed);NativeFeintTraceFE(c,g,why,false,-1,now);return false;}
    if(g_cut_feint_h20.base!=base||!g_cut_feint_h20.call)
        g_cut_feint_h20={base,reinterpret_cast<NativeCutFeintFnH20>(base+0x936A28)};
    n.cutFeintUsed62=true; // one native feint per stroke, spent at the call (a refused native check is not retried)
    const bool returned=NativeCutFeintCallH20(owner.actor,c.zone117,guard);
    int observed=-1;(void)Read(static_cast<unsigned char*>(owner.state)+0xF8,observed);
    const bool ok=returned&&observed==c.zone117;
    if(ok){n.zone117=c.zone117;fe::feints.fetch_add(1,std::memory_order_relaxed);}else fe::refused.fetch_add(1,std::memory_order_relaxed);
    NativeFeintTraceFE(c,g,ok?fe::Gate::Pass:fe::Gate::NativeRefused,returned,observed,now);
    return ok;
}
#include "../subsystems/masterstrike_combos/combo_zone_owner_r94.inl"
static bool NativeZoneOnly57(void* player,const ph::Command& c,const PhysFrame6698& f) noexcept {
    if(!NativeMeleeCommandCurrentH28(c,f))return NativeInputReject117("T4_SOLVER_OR_UNOBSERVED_ZONE",c);
    const auto now=std::uint64_t(GetTickCount64());const auto& n=g_phys_native6698;
    NativeOwner104 owner{},again{};nc104::Observed state{};int observed=-1;std::uint64_t key=0;
    if(!g_gameplay_allowed.load()||!g_head_valid.load()||!g_right_valid.load()||g_phys_fault6698.load()||
       !g_phys_armed6698.load()||animation_filter::nativeBodyOwned112.load()||diagnostics::lab::NativeReference()||
       !CombatExperiment175Enabled()||!FullWeaponMode120(g_weapon_control_mode.load())||
       !retail_build112::Ready(WhBase())||g_input_contract117.load()!=WhBase()||
       !n.attackPrepared||(!n.heldPrepare57&&!NativeForwardRefinement19c4r2(n,c,f,now))||
       (c.cutFeint62&&!NativeForwardRefinement19c4r2(n,c,f,now))||
       n.attackKey!=c.key||n.attackGeneration117!=c.generation||
       c.side!=0||f.player!=player||f.manager!=g_henry_item_attach_manager.load()||
       c.epoch!=f.request||c.epoch!=g_weapon_request_epoch6694.load()||
       c.bodyEpoch!=f.input.bodyEpoch||c.bodyEpoch!=g_pose_epoch6689.load()||
       now<c.tick||now-c.tick>100||now<f.input.tick||now-f.input.tick>80||
       !ph::command_authority_h30(f.input,f.output,f.profiles[0],0)||!f.profiles[0].melee()||
       f.profiles[0].key!=c.key||f.profiles[0].generation!=c.generation||
       (c.zone117!=0&&c.zone117!=1&&c.zone117!=2&&c.zone117!=5)||
       !NativePreparedOwnerMatches117(player)||!NativeResolve104(player,owner)||
       !NativeObserve104(owner,state)||!native_admission147::guardInput(state)||
       !NativeHandKey6697(f.manager,0,key)||key!=c.key)return false;
    if(NativePreserveComboZoneR94(owner,c))return true; // exact observed recipe zone, no second native write
    const auto heldSourceH26=g_input_source_generation_h14;
    if(c.controlledHoldH26){
        const auto& proof=g_prepared_input_r18;int heavy=-1;
        if(!c.held57||!c.cutFeint62||!proof.controlledHoldH26||!proof.source||proof.source!=g_input_source_generation_h14||
           proof.owner.player!=owner.player||proof.owner.actor!=owner.actor||proof.owner.state!=owner.state||proof.owner.epoch!=owner.epoch||
           proof.gesture!=c.gesture59c3||proof.key!=c.key||proof.generation!=c.generation||proof.request!=c.epoch||
           !Read(reinterpret_cast<void*>(WhBase()+0x5321014),heavy)||proof.input!=heavy)
            return NativeInputReject117("HELD_FEINT_ORIGINAL_PREPARE_UNOBSERVED_OR_REPLACED",c);
    }
    if(!NativeMeleeCommandCurrentH28(c,f)||
       !(c.cutFeint62?NativeCutFeintH20(owner,c):NativeZone104(owner.actor,c.zone117))||!NativeResolve104(player,again)||
       again.actor!=owner.actor||again.state!=owner.state||again.epoch!=owner.epoch||
       !NativeHandKey6697(f.manager,0,key)||key!=c.key||
       !Read(static_cast<unsigned char*>(owner.state)+0xF8,observed)||observed!=c.zone117||
       (c.cutFeint62&&(!NativePreparedOwnerMatches117(player)||!g_phys_native6698.attackPrepared||
        g_phys_native6698.prepareGestureC10R18!=c.gesture59c3||
        c.epoch!=g_weapon_request_epoch6694.load()||c.bodyEpoch!=g_pose_epoch6689.load()||
        !g_gameplay_allowed.load()||!g_head_valid.load()||!g_right_valid.load()||!NativeMeleeCommandCurrentH28(c,f))))return false;
    if(c.controlledHoldH26&&(g_input_source_generation_h14!=heldSourceH26||
       g_prepared_input_r18.source!=heldSourceH26))return NativeInputReject117("HELD_FEINT_CALLBACK_REPLACED_SOURCE",c);
    if(cc::active(now)&&c.gesture59c3)kcd2vr::outcome23::with([&](kcd2vr::outcome23::Core& k){
        const kcd2vr::outcome23::Owner59 id{reinterpret_cast<std::uintptr_t>(player),c.key,c.generation,c.bodyEpoch,c.epoch};
        k.announce59(c.gesture59c3,id,0,0,c.held57?3:c.cutFeint62?5:4,c.zone117,c.zone117,observed,c.sample,now);
        k.announcedDispatch59(c.gesture59c3,id,c.sample,true,observed,now);});
    return true;
}
// R59.16C10R4F (Miki, 14 Oct: "every movement towards the enemy he sees as an attack... only for strikes pointing toward the enemy"):
// the 00:30 log had 121 MELEE_INPUT PREPARE / 82 COMMIT edges against 14 live contacts - 68 native swings into the air (the hidden
// native attack plays its sound, the NPC reacts to a phase, nothing is hit). A VR stroke is only forwarded to the native attack
// input when Henry has a mutual combat opponent (state +CF8 back-linked), that opponent is within VrAttackReachMeters (2.6 m,
// centre to centre) and lies within 70 degrees of Henry's body forward. Outside that, the blade still moves and the geometry
// route still adjudicates a real contact; only the native request (the swoosh) is withheld. VrAttackRequireOpponent=false restores R4.
std::atomic<bool> g_attack_require_opponent59c10{true};std::atomic<float> g_attack_reach_m59c10{2.6f};
static std::atomic<unsigned> g_attack_reach_rejects59c10{0};
static bool NativeMeasureTowardOpponent50(const NativeOwner104& owner,const char*& why,float& distance,float& cosine,NativeParryOpponent176* observed59) noexcept {
    why=nullptr;distance=-1.f;cosine=-2.f;
    NativeParryOpponent176 opp{};
    if(!NativeParryOpponentRead176(owner,opp)){why="NO_OPPONENT_LINK";return false;}
    if(observed59)*observed59=opp;
    std::uint32_t henryId=0;
    if(!owner.entity||!Read(static_cast<unsigned char*>(owner.entity)+0xC,henryId)||!henryId){why="HENRY_ENTITY";return false;}
    Matrix34V6550 h{},o{};
    if(!GetEntityWorldTmV6552(owner.entity,henryId,h)||!GetEntityWorldTmV6552(opp.entity,opp.id,o)){why="ENTITY_TM";return false;}
    const float dx=o.m[3]-h.m[3],dy=o.m[7]-h.m[7],dz=o.m[11]-h.m[11];
    distance=std::sqrt(dx*dx+dy*dy+dz*dz);
    if(!std::isfinite(distance)){why="ENTITY_TM";return false;}
    const float reach=g_attack_reach_m59c10.load(std::memory_order_relaxed);
    if(distance>(reach<1.f?1.f:reach)){why="OUT_OF_REACH";return false;}
    // CryEngine: column 1 of the world matrix is the entity's forward (Y).
    const float fx=h.m[1],fy=h.m[5],fz=h.m[9],fl=std::sqrt(fx*fx+fy*fy+fz*fz),hl=std::sqrt(dx*dx+dy*dy);
    if(fl>1e-4f&&hl>.05f){cosine=(fx*dx+fy*dy)/(std::sqrt(fx*fx+fy*fy)*hl+1e-6f);if(cosine<.342f){why="NOT_TOWARD_OPPONENT";return false;}} // 70 deg
    return true;
}
// C54 restores the proven C50 split lost from this independent C51R1 line:
// optional ordinary-input admission must not disable actual counter geometry.
static bool NativeStrokeTowardOpponent117(const NativeOwner104& owner,const char*& why,float& distance,float& cosine,NativeParryOpponent176* observed59) noexcept {
    why=nullptr;distance=-1.f;cosine=-2.f;
    if(kcd2vr::melee_tuning_c54::bypassExtraAttackVetoesH41.load()||!g_attack_require_opponent59c10.load(std::memory_order_relaxed))return true;
    if(NativeMeasureTowardOpponent50(owner,why,distance,cosine,observed59))return true;
    if(why&&std::strcmp(why,"NO_OPPONENT_LINK")==0){why=nullptr;return true;} // retained first-strike admission
    return false;
}
// R59.16C10R17 MASTER STRIKE (Miki: "working master strikes"; R14's game-decides policy). The master-strike trigger counts on Henry's
// state +0x7D8 only while his STANCE (+0xF8) equals the attacker's +0x158 master_strike zone (C566D8); the guard tick keeps the
// stance on the blade's side. The prepare used to REWRITE the stance to the stroke's origin zone (a thrust is a side zone with
// VrThrustAsSwing) - on the very frame of the press - so D4AD94 found the window closed: 0 master-strike factory queries since R14.
// While the window is open the stroke now keeps the stance that opened it; the game then turns the attack press into its master
// strike (RequestDefenseAction kind 5) or, if its own checks refuse, into the ordinary attack.
// g_ms_keep_stance59c17 lives with the guard owner so both paths retain the same live window stance.
static std::atomic<std::uint64_t> g_ms_window_presses59c17{0},g_ms_window_seen59c17{0};
static int NativeMasterWindow59c17(const NativeOwner104& owner) noexcept {
    int count=0;return owner.state&&Read(static_cast<unsigned char*>(owner.state)+0x7D8,count)&&count>0&&count<64?count:0;
}
static int MasterPerk47(const NativeOwner104&) noexcept;
// Stack-only permission to RETAIN a measured stance while our existing block
// input has metadata7 in the observed action sector. It cannot write a sector,
// press LT, survive another held-input acquisition or extend any input deadline.
struct NativePreopenGuardH14 {
    NativeOwner104 owner{};std::uint64_t lease=0,key=0,generation=0,sample=0,frame=0;
    int side=-1;
};
static bool NativePreopenMetadataCurrentH14(const NativePreopenGuardH14& token,const NativeOwner104& owner,
    const ph::Command& c,const PhysFrame6698& f,std::uint64_t now) noexcept {
    if(!token.lease||token.lease!=g_guard_lease_h14.load(std::memory_order_relaxed)||
       token.owner.player!=owner.player||token.owner.actor!=owner.actor||token.owner.state!=owner.state||token.owner.epoch!=owner.epoch||
       token.side<0||token.side>1||token.key!=f.profiles[token.side].key||token.generation!=f.profiles[token.side].generation||
       token.sample!=c.sample||token.frame!=f.input.sample||!f.output.active[token.side]||!f.output.owned[token.side]||
       (g_guard_manual_owner112.load()==owner.player&&g_guard_manual112.load()))return false;
    return g_native_guard112.held?
        g_native_guard112.leaseH14==token.lease&&NativeBlockMetadataInputH14(owner,c,f,now):
        !NativePhysicalGuardHeld117(owner.player)&&NativeBlockMetadataReleasedH14(owner,c,f,now);
}
struct NativePreopenStanceH14 {
    kcd2vr::preopen_stance_h14::In physical{};
    NativeParryOpponent176 opponent{};bool ownerCurrent=false;
};
// Native 90A663..90A6A3 compares current stance with the offered combat_zone's
// master_strike_zone BEFORE opening +7D8. Keep only a matching measured stance;
// never choose the opposite side from the enemy to manufacture eligibility.
static NativePreopenStanceH14 NativeReadPreopenStanceH14(const NativeOwner104& owner,const ph::Command& c,
    const PhysFrame6698& f,std::uint64_t now,const NativePreopenGuardH14& metadata={}) noexcept {
    NativePreopenStanceH14 out{};auto& in=out.physical;NativeOwner104 current{};nc104::Observed state{};
    void* manager=nullptr;void* back=nullptr;std::uint64_t key=0;
    out.ownerCurrent=NativeResolve104(owner.player,current)&&current.actor==owner.actor&&current.state==owner.state&&
        current.epoch==owner.epoch&&current.epoch==c.bodyEpoch&&current.epoch==g_pose_epoch6689.load()&&
        c.epoch==f.request&&c.epoch==g_weapon_request_epoch6694.load()&&
        Read(static_cast<unsigned char*>(owner.player)+0x9D0,manager)&&manager==f.manager&&manager==g_henry_item_attach_manager.load()&&
        PhysVft6698(manager,0x3EC9958)&&Read(static_cast<unsigned char*>(manager)+0x38,back)&&back==owner.player&&
        NativeHandKey6697(manager,0,key)&&key==c.key&&NativeObserve104(current,state);
    if(!out.ownerCurrent)return out;
    in.keepStance=g_ms_keep_stance59c17.load(std::memory_order_relaxed);
    in.mode=msH14::clampMode(msH14::mode.load(std::memory_order_relaxed));
    in.held=c.held57;in.kick=c.kick59;in.writable=native_admission147::guardWritable(state);
    in.ownedMetadata=nc104::blockMetadataH14(state)&&NativePreopenMetadataCurrentH14(metadata,owner,c,f,now);
    in.stroke=c.stroke117==ph::CommandKind::Thrust?msH14::Stroke::Thrust:
        c.stroke117==ph::CommandKind::Slash?msH14::Stroke::Cut:msH14::Stroke::Unknown;
    in.origin=c.originZone167;in.energy=msH14::prepareEnergy(c.direction.x,c.direction.y,c.direction.z);
    in.minEnergy=msH14::clampEnergy(msH14::minEnergy.load(std::memory_order_relaxed));
    in.current=c.kind==ph::CommandKind::Prepare&&c.side==0&&c.gesture59c3&&c.sample&&c.sample<=f.input.sample&&
        f.input.sample==g_phys_ready_sample6698.load(std::memory_order_acquire)&&c.tick&&now>=c.tick&&now-c.tick<=100&&
        f.input.tick&&now>=f.input.tick&&now-f.input.tick<=80&&f.player==owner.player&&f.input.bodyEpoch==owner.epoch&&
        c.key==f.profiles[0].key&&c.generation==f.profiles[0].generation&&f.profiles[0].primary&&f.profiles[0].melee()&&
        ph::command_authority_h30(f.input,f.output,f.profiles[0],0);
    (void)NativeMasterCount23(current,in.count);(void)Read(static_cast<unsigned char*>(current.state)+0xF8,in.stance);
    if(NativeParryOpponentRead176(current,out.opponent)&&out.opponent.incoming&&out.opponent.attacking)in.offered=out.opponent.sector;
    // Read the ability table only at an otherwise matching physical edge.
    if(in.mode==1&&in.current&&(in.writable||in.ownedMetadata)&&in.count>=0&&in.count<64&&in.stance==in.origin&&
       msH14::masterZone(in.offered)==in.stance)
        in.perk=MasterPerk47(current);
    return out;
}
// C48: share DIRECT's measured incoming-sector selection with HYBRID at its
// existing native prepare edge. This only chooses Henry's requested stance;
// it never starts/awards an NPC block or publishes a second attack phase.
static int NativeHybridApproachZone48(const NativeOwner104& owner,const ph::Command& c,std::uint64_t now,NativeParryOpponent176* observed59=nullptr) noexcept {
    namespace db=kcd2vr::dirblock39;
    if(!CombatSynchronized59c10r9()||!db::enabled.load(std::memory_order_relaxed)||
       c.stroke117!=ph::CommandKind::Slash||c.kick59||c.masterHandoff23||c.held57||!c.gesture59c3)return c.zone117;
    NativeParryOpponent176 opponent{};
    if(!NativeParryOpponentRead176(owner,opponent))return c.zone117;
    if(observed59)*observed59=opponent;
    if(opponent.observedPhase19c4r1!=1&&opponent.observedPhase19c4r1!=2)return c.zone117;
    const auto sample=db::unpack(db::mailbox.load(std::memory_order_acquire),c.gesture59c3,now,db::kFreshMs);
    db::Choice why{};const int zone=db::chooseInitial(true,true,0,c.zone117,sample,why);
    if(zone!=c.zone117&&cc::active(now)){char line[240]{};std::snprintf(line,sizeof(line),
        "C48 HYBRID_APPROACH gesture=%llu origin=%d announced=%d npc=%u phase=%d source=DIRECT_SECTOR nativeBlockAward=NONE",
        (unsigned long long)c.gesture59c3,c.zone117,zone,opponent.id,opponent.observedPhase19c4r1);diagnostic_trace::Write(line);}
    return zone;
}
// I12 VrComboZoneAssist: inside Henry's OPEN native combo slot (+0x868 payload 1) a press is announced with the
// zone of the next step of the most progressed native candidate (combo_zone_assist_cz.h). Only the zone we were
// about to hand to NativeZone104 changes; outside a slot, without a recipe in progress, kicks/held/master handoff,
// or with the toggle off this returns the zone unchanged. The native matcher decides the step.
static int NativeComboZoneAssistCZ(const NativeOwner104& owner,const ph::Command& c,int zone,std::uint64_t now,bool& pickedR94,NativeParryOpponent176& opponentR94) noexcept {
    pickedR94=false;
    namespace cz=kcd2vr::combo_zone_assist_cz;
    if(!cz::enabled.load(std::memory_order_relaxed)||!MotionClockSelected50()||!CombatSynchronized59c10r9())return zone;
    if(c.kick59||c.held57||c.masterHandoff23||c.cutFeint62||
       (c.stroke117!=ph::CommandKind::Slash&&c.stroke117!=ph::CommandKind::Thrust))return zone;
    unsigned char slot=0;
    const bool slotOpen=owner.state&&Read(static_cast<unsigned char*>(owner.state)+0x868,slot)&&slot==1;
    if(!slotOpen||!NativeParryOpponentRead176(owner,opponentR94))return zone; // no recipe lease without the current native target
    cz::NativeCandidate list[cz::kMaxCandidates]{};
    const unsigned n=slotOpen?kcd2vr::combat_diag_probe::ComboCandidatesCZ(owner.actor,list,cz::kMaxCandidates):0u;
    const auto kind=c.stroke117==ph::CommandKind::Thrust?cz::Kind::Stab:cz::Kind::Cut;
    const auto pick=cz::choose(true,slotOpen,list,n,zone,kind,g_thrust_as_swing55.load(std::memory_order_relaxed));
    if(pick.result==cz::Result::NoSlot)return zone; // the ordinary case: no line, no change
    if(pick.result==cz::Result::Assisted)cz::assisted.fetch_add(1,std::memory_order_relaxed);
    else if(pick.result==cz::Result::SameZone)cz::sameZone.fetch_add(1,std::memory_order_relaxed);
    else cz::refused.fetch_add(1,std::memory_order_relaxed);
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
    char line[300]{};std::snprintf(line,sizeof(line),
        "CZ COMBO_ZONE_ASSIST recipe=%s group=%s step=%u/%u swingZone=%d announcedZone=%d kind=%s candidates=%u result=%s assisted=%llu same=%llu refused=%llu native=DECIDES",
        pick.recipe?pick.recipe->id:"-",pick.recipe?pick.recipe->group:"-",pick.step+1,pick.required,zone,
        pick.recipe?pick.zone:zone,kind==cz::Kind::Stab?"STAB":"CUT",n,cz::name(pick.result),
        (unsigned long long)cz::assisted.load(),(unsigned long long)cz::sameZone.load(),(unsigned long long)cz::refused.load());
    diagnostic_trace::Write(line);}
    if(!pick.recipe)return zone;
    pickedR94=true; // candidate only; publication waits for native zone ACK and observed prepare
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Bug,now)){
        char text[96]{};std::snprintf(text,sizeof(text),"combo %s %u/%u: %s%s",pick.recipe->id,pick.step+1,pick.required,
            cz::zoneText(pick.zone),cz::kindText(pick.recipe->steps[pick.step]));
        (void)kcd2vr::visible_failure::publishDiagnosticD74(text,now,diagnostics::capture168::Domain::Bug);
    }
    return pick.zone;
}
struct NativeOwnedComboInputH14 {
    kcd2vr::subsystems::masterstrike_combos::NativeSlotF1 native{};
    kcd2vr::combo_lineage19::SlotLeaseF1 lease{};
    int input=-1;std::uint64_t sourceGeneration=0;
};
// A released VR input is not an enduring held-button owner. D4AAC4 calls
// D4AD94 -> D4AE70/C5AC2C before ordinary attack selection; native +2A8 can
// still be set while that SAME admitted action has its real combo slot open.
// Only this exact action/release/target may reach the ORIGINAL input arbiter.
// This reads an existing lease; it never opens, consumes or refreshes one.
static bool NativeReadOwnedComboInputH14(const NativeOwner104& owner,const ph::Command& c,
    const PhysFrame6698& f,std::uint64_t now,NativeOwnedComboInputH14& out) noexcept {
    out={};out.sourceGeneration=g_input_source_generation_h14;namespace cg=kcd2vr::combo_lineage19;
    const auto& prior=g_phys_native6698;const auto& source=g_released_input_h14;
    if(c.kind!=ph::CommandKind::Prepare||c.side!=0||c.kick59||c.held57||c.masterHandoff23||
       (c.stroke117!=ph::CommandKind::Slash&&c.stroke117!=ph::CommandKind::Thrust&&c.stroke117!=ph::CommandKind::Pommel)||
       !c.gesture59c3||!c.sample||c.sample>f.input.sample||!kcd2vr::contact176::fresh(c.tick,now,100)||
       prior.attackPrepared||!prior.natural117||prior.player!=owner.player||prior.epoch!=owner.epoch||
       prior.request!=c.epoch||prior.attackKey!=c.key||prior.attackGeneration117!=c.generation||
       !prior.prepareGestureC10R18||!NativePreparedOwnerMatches117(owner.player)||
       source.owner.player!=owner.player||source.owner.entity!=owner.entity||source.owner.actor!=owner.actor||source.owner.state!=owner.state||source.owner.epoch!=owner.epoch||
       (!source.tick||source.tick>now))return false;
    std::uint64_t consumed=0;if(!cg::nativeSlot.read(out.lease,consumed))return false;
    const auto& old=out.lease.owner;const auto& k=old.key;
    const unsigned mode=(combat_diag_probe::Synchronized59c10r9()?1u:0u)|
        (combat_diag_probe::Intent59c16()?2u:0u)|(kcd2vr::motion59c4::selected.load()?4u:0u)|
        (combat_diag_probe::GeometryLiveAction178r35()?8u:0u);
    if(!cg::windowAllowedF6(out.lease.window,kcd2vr::combo_test_ds::force.load(std::memory_order_relaxed))||old.mode!=mode||
       !kcd2vr::contact176::fresh(old.tick,now,out.lease.window)||
       // The original release may open the slot synchronously, then return
       // and publish this source timestamp a little AFTER immutable old.tick.
       // Bound only this exact source's proximity on either side of opening;
       // the existing lease deadline and binding/ticket opening checks stay fixed.
       (source.tick>old.tick?source.tick-old.tick:old.tick-source.tick)>1500||
       !frik_pose_probe::ReadNativeComboContinuationH19(owner.actor,out.native,out.lease))return false;
    const auto& n=out.native;const auto& t=n.release;
    if(old.release!=t.serial||old.release!=consumed||old.action!=n.action||
       old.actor!=reinterpret_cast<std::uintptr_t>(owner.actor)||old.state!=reinterpret_cast<std::uintptr_t>(owner.state)||
       n.actor!=old.actor||n.state!=old.state||n.entity!=reinterpret_cast<std::uintptr_t>(owner.entity)||
       k.player!=reinterpret_cast<std::uintptr_t>(owner.player)||k.player!=t.player||
       k.weapon!=c.key||k.weapon!=t.key||k.generation!=c.generation||k.generation!=t.generation||
       k.body!=owner.epoch||k.body!=c.bodyEpoch||k.body!=t.epoch||k.request!=c.epoch||k.request!=t.request||
       k.recovery!=kcd2vr::contact176::recoveryEpoch55c1.load()||k.recovery!=t.recovery59c3||
       k.gesture!=t.gesture59c3||k.gesture!=prior.prepareGestureC10R18||c.gesture59c3==k.gesture||
       c.sample<=t.sample||c.tick<t.tick||!kcd2vr::motion59c4::same(k,source.key))return false;
    void* manager=nullptr;void* back=nullptr;NativeParryOpponent176 target{};nc104::Observed state{};
    unsigned char slot=0;int heavy=-1,light=-1;
    if(!Read(static_cast<unsigned char*>(owner.actor)+0x3B0,manager)||reinterpret_cast<std::uintptr_t>(manager)!=old.manager||
       !Read(static_cast<unsigned char*>(manager)+8,back)||back!=owner.actor||
       !NativeParryOpponentRead176(owner,target)||reinterpret_cast<std::uintptr_t>(target.actor)!=old.target||
       reinterpret_cast<std::uintptr_t>(target.entity)!=old.targetEntity||target.id!=old.targetId||
       !NativeObserve104(owner,state)||(state.phase!=16&&state.phase!=32)||(state.type!=0&&state.type!=1)||state.hand!=0||!state.requested||
       !Read(static_cast<unsigned char*>(owner.state)+0x868,slot)||slot!=1||
       !Read(reinterpret_cast<void*>(WhBase()+0x5321014),heavy)||!Read(reinterpret_cast<void*>(WhBase()+0x5321010),light)||
       heavy<0||heavy>128||light<0||light>128||heavy==light)return false;
    // Match the previous physical preparation's actual ordinary input family.
    const int expected=prior.stroke117==ph::CommandKind::Slash?heavy:
        prior.stroke117==ph::CommandKind::Thrust?(g_thrust_as_swing55.load()?heavy:light):-1;
    if(expected<0||state.input!=expected||source.input!=state.input)return false;
    if(out.sourceGeneration!=g_input_source_generation_h14)return false;
    out.input=state.input;return true;
}
static bool NativeSameOwnedComboInputH14(const NativeOwnedComboInputH14& a,const NativeOwnedComboInputH14& b) noexcept {
    const auto& x=a.native;const auto& y=b.native;
    return x.action&&x.action==y.action&&x.helper==y.helper&&x.clock==y.clock&&x.manager==y.manager&&x.row==y.row&&
        x.admission==y.admission&&x.policy==y.policy&&x.release.serial==y.release.serial&&
        a.input==b.input&&a.sourceGeneration==b.sourceGeneration&&a.lease.owner.release==b.lease.owner.release&&a.lease.owner.tick==b.lease.owner.tick&&
        a.lease.window==b.lease.window&&a.lease.owner.action==b.lease.owner.action&&
        kcd2vr::combo_lineage19::sameLineage(a.lease.owner,b.lease.owner);
}
// D72/D82: ordinary inputs keep their original 100 ms deadline. Only a final
// intent already captured from a live native slot may wait for its own measured
// contact; that contact can create one new input transaction, never a new hit.
namespace finalD72=kcd2vr::final_combo_d72;
struct NativeFinalPendingD72 {
    finalD72::Permit permit{};
    bool captured=false,waiting=false;
    struct OfferD82 {std::uintptr_t recipe=0,step=0;unsigned cursor=0,required=0;int input=-1;std::uint8_t technique[16]{};};
    std::array<OfferD82,kcd2vr::diagnostics::combo::MaxCandidates> offersD82{};unsigned countD82=0;
    NativeOwnedComboInputH14 continuationD82{};
    finalD72::Permit contactD82{};
    std::uint64_t recoveryD82=0;
    bool continuationKnownD82=false,retiredD82=false,prepareEnteredD82=false,releaseEnteredD82=false;
};
static NativeFinalPendingD72 g_final_pending_d72;
static finalD72::Permit g_final_prepared_d72;
// Read the ORIGINAL native action/slot lease without publishing a new input or
// changing its deadline. H19 rechecks the current action, release, tracking and
// held item. H14 still separately admits the eventual fresh input transaction.
static bool NativeFinalContinuationD82(const NativeOwner104& owner,const finalD72::Permit& p,
    std::uint64_t recovery,std::uint64_t now,NativeOwnedComboInputH14& out) noexcept {
    namespace cg=kcd2vr::combo_lineage19;out={};std::uint64_t consumed=0;
    out.sourceGeneration=g_input_source_generation_h14;
    if(!cg::nativeSlot.read(out.lease,consumed))return false;
    const auto& old=out.lease.owner;const auto& k=old.key;
    const unsigned mode=(combat_diag_probe::Synchronized59c10r9()?1u:0u)|
        (combat_diag_probe::Intent59c16()?2u:0u)|(kcd2vr::motion59c4::selected.load()?4u:0u)|
        (combat_diag_probe::GeometryLiveAction178r35()?8u:0u);
    nc104::Observed state{};
    if(old.release!=consumed||old.mode!=mode||old.actor!=p.actor||old.state!=p.state||old.manager!=p.manager||
       old.target!=p.target||old.targetEntity!=p.targetEntity||old.targetId!=p.targetId||
       k.player!=p.player||k.weapon!=p.key||k.generation!=p.generation||k.body!=p.body||k.request!=p.request||
       !k.gesture||k.gesture==p.gesture||k.recovery!=recovery||recovery!=kcd2vr::contact176::recoveryEpoch55c1.load()||
       !kcd2vr::contact176::fresh(old.tick,now,out.lease.window)||
       !frik_pose_probe::ReadNativeComboContinuationH19(owner.actor,out.native,out.lease)||
       !NativeObserve104(owner,state)||(state.phase!=16&&state.phase!=32)||(state.type!=0&&state.type!=1)||state.hand!=0||
       out.sourceGeneration!=g_input_source_generation_h14)return false;
    const auto& n=out.native;const auto& t=n.release;
    if(n.action!=old.action||n.actor!=p.actor||n.state!=p.state||n.entity!=p.entity||t.serial!=old.release||
       t.gesture59c3!=k.gesture||t.player!=p.player||t.key!=p.key||t.generation!=p.generation||
       t.epoch!=p.body||t.request!=p.request||t.recovery59c3!=recovery)return false;
    out.input=state.input;return true;
}
static bool NativeFinalContactCurrentD82(const finalD72::Permit& p,const PhysFrame6698& f,std::uint64_t now) noexcept {
    const auto& r=f.output.impactRecord178r30[0];
    // Later samples of the SAME body contact do not refresh or replace the
    // captured occurrence. Another target/collider/gesture invalidates it.
    return finalD72::fresh(p,now)&&r.valid&&r.gesture59c3==p.gesture&&r.key==p.key&&r.generation==p.generation&&
        r.epoch==p.body&&r.request==p.request&&r.entityId==p.targetId&&r.collider==p.contact.collider&&
        r.tick>=p.contact.tick&&r.tick<=now&&r.sample>=p.contact.sample&&r.sample<=f.input.sample;
}
static bool NativeFinalComboDeferredD72(const ph::Command& c,const PhysFrame6698& f,std::uint64_t now) noexcept {
    const auto& pending=g_final_pending_d72;const auto& p=pending.permit;
    return pending.captured&&pending.waiting&&p.started&&now>=p.started&&now-p.started<=100&&
        finalD72::sameGesture(p,reinterpret_cast<std::uintptr_t>(f.player),c.gesture59c3,c.key,c.generation,c.bodyEpoch,c.epoch)&&
        c.sample>=p.sourceSample&&c.tick>=p.started&&c.tick<=now&&
        f.request==p.request&&f.input.bodyEpoch==p.body&&f.input.allowed&&f.input.coherentMelee119&&
        f.output.active[0]&&f.output.owned[0]&&f.profiles[0].key==p.key&&f.profiles[0].generation==p.generation;
}
static bool NativeFinalComboPreparedD72(const ph::Command& c,const PhysFrame6698& f,std::uint64_t now) noexcept {
    const auto& p=g_final_prepared_d72;const auto& r=f.output.impactRecord178r30[0];
    const int kind=c.stroke117==ph::CommandKind::Pommel?3:c.stroke117==ph::CommandKind::Thrust?2:c.stroke117==ph::CommandKind::Slash?1:0;
    return finalD72::fresh(p,now)&&finalD72::sameGesture(p,reinterpret_cast<std::uintptr_t>(f.player),
        c.gesture59c3,c.key,c.generation,c.bodyEpoch,c.epoch)&&c.side==0&&!c.kick59&&!c.held57&&
        kind&&((p.physicalKind<=2&&kind<=2)||(p.physicalKind==3&&kind==3))&&
        f.request==p.request&&f.input.bodyEpoch==p.body&&f.input.allowed&&f.input.coherentMelee119&&
        f.output.active[0]&&f.output.owned[0]&&f.profiles[0].key==p.key&&f.profiles[0].generation==p.generation&&
        r.valid&&r.gesture59c3==p.gesture&&r.key==p.key&&r.generation==p.generation&&r.epoch==p.body&&r.request==p.request&&
        r.entityId==p.targetId&&r.collider==p.contact.collider&&
        (p.contactClockD82?NativeFinalContactCurrentD82(p,f,now):r.tick==p.contact.tick&&r.sample==p.contact.sample);
}
enum class NativeFinalResultD72 {Ordinary,Deferred,Ready,Refused};
static bool NativeFinalStepD72(const kcd2vr::diagnostics::combo::Candidate& candidate,unsigned cursor,
    int heavy,int light,std::uintptr_t& step,int& input) noexcept {
    step=0;input=-1;std::uintptr_t begin=0,end=0;
    if(!candidate.recipe||!Read(reinterpret_cast<void*>(candidate.recipe+0x40),begin)||
       !Read(reinterpret_cast<void*>(candidate.recipe+0x48),end)||!begin||end<begin||
       (end-begin)%32||(end-begin)/32>64)return false;
    for(auto at=begin;at<end;at+=32){int type=-1,index=-1,family=-2;
        if(!Read(reinterpret_cast<void*>(at+8),type)||!Read(reinterpret_cast<void*>(at+0x18),index)||
           !Read(reinterpret_cast<void*>(at+0x10),family))return false;
        if(type==-1||index!=int(cursor))continue;
        // This is the ORIGINAL selector's input-family field, not an inferred
        // Slash/Stab enum. The original also supplies its charged/solver bits.
        if(family!=-1&&family!=heavy&&family!=light)return false;
        step=at;input=family==-1?heavy:family;return true;
    }
    return false;
}
static bool NativeFinalContactD72(const NativeOwner104& owner,const ph::Command& c,const PhysFrame6698& f,
    const NativeParryOpponent176& target,const finalD72::Permit& identity,std::uint64_t now,finalD72::Contact& out) noexcept {
    out={};const auto& r=f.output.impactRecord178r30[0];
    if(!r.valid||!r.gesture59c3||r.gesture59c3!=c.gesture59c3||r.key!=c.key||r.generation!=c.generation||
       r.epoch!=owner.epoch||r.epoch!=c.bodyEpoch||r.request!=c.epoch||r.entityId!=target.id||
       !r.collider||(r.body!=2&&r.body!=3)||!r.tick||r.tick<identity.started||r.tick>now||
       now-r.tick>100||r.sample<identity.sourceSample||r.sample>f.input.sample||!ph::m::finite(r.point))return false;
    float tm[12]{};
    if(!Read(static_cast<unsigned char*>(target.entity)+0x58,tm))return false;
    const double height=r.point.z-double(tm[11]);
    if(!std::isfinite(height)||height<-.5||height>2.6)return false;
    out.tick=r.tick;out.sample=r.sample;out.collider=r.collider;out.target=r.entityId;out.body=r.body;out.part=r.part;
    out.feature=kcd2vr::bodycombo59c7g::Feature(r.feature59c7g);out.height=height;out.heightKnown=true;out.actual=true;
    out.ordinaryWeapon=c.stroke117==ph::CommandKind::Slash||c.stroke117==ph::CommandKind::Thrust;
    return true;
}
static NativeFinalResultD72 NativeFinalPlanD72(const NativeOwner104& owner,const ph::Command& c,
    const PhysFrame6698& f,std::uint64_t now,finalD72::Permit& out) noexcept {
    out={};namespace cz=kcd2vr::combo_zone_assist_cz;namespace dc=kcd2vr::diagnostics::combo;
    auto& pending=g_final_pending_d72;
    const bool same=pending.captured&&finalD72::sameGesture(pending.permit,reinterpret_cast<std::uintptr_t>(owner.player),
        c.gesture59c3,c.key,c.generation,c.bodyEpoch,c.epoch);
    const bool delayedD82=same&&pending.permit.started&&now>=pending.permit.started&&now-pending.permit.started>100;
    if(same&&(pending.retiredD82||!pending.permit.started||now<pending.permit.started||
       (delayedD82&&(!pending.continuationKnownD82||pending.prepareEnteredD82||
        (!pending.contactD82.serial&&now-pending.permit.started>kcd2vr::motion59c4::strikeLifetimeMs5910c1)||
        (pending.contactD82.serial&&!finalD72::fresh(pending.contactD82,now)))))){
        pending.waiting=false;pending.retiredD82=true;finalD72::revoke();return NativeFinalResultD72::Refused;}
    if(delayedD82){NativeOwnedComboInputH14 current{};
        if(!NativeFinalContinuationD82(owner,pending.permit,pending.recoveryD82,now,current)||
           !NativeSameOwnedComboInputH14(pending.continuationD82,current)){
            pending.waiting=false;pending.retiredD82=true;return NativeFinalResultD72::Refused;}}
    if(!same){
        pending={};
        if(!finalD72::hookReady.load(std::memory_order_acquire)||!kcd2vr::instant_d72::Combat()||!cz::enabled.load(std::memory_order_relaxed)||
           !kcd2vr::finisher_cb1::PhysicalFilterEnabledD61()||!MotionClockSelected50()||!CombatSynchronized59c10r9()||
           c.kick59||c.held57||c.masterHandoff23||c.cutFeint62||!c.gesture59c3||
           (c.stroke117!=ph::CommandKind::Slash&&c.stroke117!=ph::CommandKind::Thrust&&c.stroke117!=ph::CommandKind::Pommel))
            return NativeFinalResultD72::Ordinary;
    }
    unsigned char slot=0;void* manager=nullptr;void* back=nullptr;NativeParryOpponent176 target{};
    if(!Read(static_cast<unsigned char*>(owner.state)+0x868,slot)||slot!=1||
       !Read(static_cast<unsigned char*>(owner.actor)+0x3B0,manager)||!manager||
       !Read(static_cast<unsigned char*>(manager)+8,back)||back!=owner.actor||!NativeParryOpponentRead176(owner,target)){
        pending.waiting=false;return same?NativeFinalResultD72::Refused:NativeFinalResultD72::Ordinary;}
    if(same&&(pending.permit.entity!=reinterpret_cast<std::uintptr_t>(owner.entity)||pending.permit.actor!=reinterpret_cast<std::uintptr_t>(owner.actor)||
       pending.permit.state!=reinterpret_cast<std::uintptr_t>(owner.state)||pending.permit.manager!=reinterpret_cast<std::uintptr_t>(manager)||
       pending.permit.target!=reinterpret_cast<std::uintptr_t>(target.actor)||pending.permit.targetEntity!=reinterpret_cast<std::uintptr_t>(target.entity)||
       pending.permit.targetId!=target.id)){pending.waiting=false;return NativeFinalResultD72::Refused;}
    auto reader=[](std::uint64_t at,void* data,std::size_t bytes) noexcept{return Copy37(at,data,bytes);};
    const auto snapshot=dc::Capture(reader,dc::Layout{WhBase()},reinterpret_cast<std::uintptr_t>(manager),reinterpret_cast<std::uintptr_t>(owner.entity));
    if(snapshot.status!=dc::Status::Ok||snapshot.truncated){pending.waiting=false;return same?NativeFinalResultD72::Refused:NativeFinalResultD72::Ordinary;}
    int heavy=-1,light=-1;
    if(!Read(reinterpret_cast<void*>(WhBase()+0x5321014),heavy)||!Read(reinterpret_cast<void*>(WhBase()+0x5321010),light)||
       heavy<0||heavy>128||light<0||light>128||heavy==light){pending.waiting=false;return NativeFinalResultD72::Refused;}
    bool finalOffered=false;const bool captureOffersD82=!pending.captured;
    for(unsigned i=0;i<snapshot.candidatesCopied;++i){const auto& candidate=snapshot.candidates[i];
        if(candidate.state>=snapshot.statesCopied)continue;
        const auto cursor=snapshot.states[candidate.state].cursor;
        const auto* recipe=cz::find(candidate.techniqueRaw.data());const auto* row=kcd2vr::finisher_cb1::findRow(candidate.techniqueRaw.data());
        if(!recipe||!row||!cursor||cursor+1!=candidate.requiredSteps||candidate.requiredSteps!=recipe->count||cursor>=6)continue;
        // Leave unsupported owners/method-only rows on their original native
        // route before capturing a pending input; no fabricated head surface.
        if(!finalD72::weaponEndpoint(*row))continue;
        std::uintptr_t step=0;int family=-1;
        if(!NativeFinalStepD72(candidate,cursor,heavy,light,step,family))continue;
        if(delayedD82){bool original=false;
            for(unsigned n=0;n<pending.countD82;++n){const auto& o=pending.offersD82[n];
                if(o.recipe==candidate.recipe&&o.step==step&&o.cursor==cursor&&o.required==candidate.requiredSteps&&
                   o.input==family&&!std::memcmp(o.technique,candidate.techniqueRaw.data(),16)){original=true;break;}}
            if(!original)continue;
            if(pending.contactD82.serial&&!finalD72::candidate(pending.contactD82,candidate,cursor))continue;
        }
        finalOffered=true;
        if(!pending.captured){auto& p=pending.permit;
            p.serial=finalD72::mint();p.gesture=c.gesture59c3;p.key=c.key;p.generation=c.generation;p.body=c.bodyEpoch;p.request=c.epoch;
            p.started=c.tick;p.sourceSample=c.sample;p.player=reinterpret_cast<std::uintptr_t>(owner.player);p.entity=reinterpret_cast<std::uintptr_t>(owner.entity);
            p.actor=reinterpret_cast<std::uintptr_t>(owner.actor);p.state=reinterpret_cast<std::uintptr_t>(owner.state);p.manager=reinterpret_cast<std::uintptr_t>(manager);
            p.target=reinterpret_cast<std::uintptr_t>(target.actor);p.targetEntity=reinterpret_cast<std::uintptr_t>(target.entity);p.targetId=target.id;
            p.physicalKind=c.stroke117==ph::CommandKind::Thrust?2:c.stroke117==ph::CommandKind::Pommel?3:1;
            pending.captured=true;pending.waiting=true;
            pending.recoveryD82=c.recoveryEpochH30;
            pending.continuationKnownD82=c.kind==ph::CommandKind::Prepare&&c.recoveryEpochH30&&
                !kcd2vr::motion59c4::selected.load()&&kcd2vr::solver_command_h28::empty(c.solverH28)&&
                !f.output.solverOwnedT4[0]&&kcd2vr::solver_command_h28::empty(f.output.solverH28)&&
                (c.stroke117==ph::CommandKind::Slash||c.stroke117==ph::CommandKind::Thrust)&&
                NativeFinalContinuationD82(owner,p,pending.recoveryD82,now,pending.continuationD82);
        }
        if(captureOffersD82&&pending.countD82<pending.offersD82.size()){
            auto& o=pending.offersD82[pending.countD82++];o.recipe=candidate.recipe;o.step=step;o.cursor=cursor;
            o.required=candidate.requiredSteps;o.input=family;std::memcpy(o.technique,candidate.techniqueRaw.data(),16);
        }
        finalD72::Contact contact{};
        if(pending.contactD82.serial){
            if(!NativeFinalContactCurrentD82(pending.contactD82,f,now))continue;
            contact=pending.contactD82.contact;
        }else if(!NativeFinalContactD72(owner,c,f,target,pending.permit,now,contact))continue;
        if(!finalD72::matches(*row,contact))continue;
        if(out.serial&&out.cursor>=cursor)continue;
        out=pending.permit;out.recipe=candidate.recipe;out.nativeStep=step;out.cursor=cursor;out.required=candidate.requiredSteps;
        std::memcpy(out.technique,candidate.techniqueRaw.data(),16);out.input=family;out.zone=recipe->steps[cursor].zone;out.contact=contact;
        out.contactClockD82=pending.contactD82.contactClockD82;
    }
    if(out.serial)return NativeFinalResultD72::Ready;
    if(finalOffered){pending.waiting=true;return NativeFinalResultD72::Deferred;}
    pending.waiting=false;return same?NativeFinalResultD72::Refused:NativeFinalResultD72::Ordinary;
}

// The ordinary queue remains 100 ms. This adapter either leaves that policy
// alone, waits without a native call, or translates a captured final intent
// into one fresh CONTACT event. It never overwrites the queued original.
static finalD72::QueueD82 NativeFinalContactCommandD82(ph::Command& c,const PhysFrame6698& f,std::uint64_t now) noexcept {
    using Q=finalD72::QueueD82;auto& pending=g_final_pending_d72;const auto& p=pending.permit;
    if(now<c.tick||now-c.tick<=100)return Q::Ordinary;
    if(!pending.captured||!finalD72::sameGesture(p,reinterpret_cast<std::uintptr_t>(f.player),
       c.gesture59c3,c.key,c.generation,c.bodyEpoch,c.epoch))return Q::Ordinary;
    const auto refuse=[&](){pending.waiting=false;pending.retiredD82=true;finalD72::revoke();return Q::Refused;};
    const bool prepare=c.kind==ph::CommandKind::Prepare;
    const bool commit=c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust;
    if(pending.retiredD82||!pending.continuationKnownD82||(!prepare&&!commit)||c.side!=0||c.kick59||c.held57||
       c.masterHandoff23||c.cutFeint62||c.feintFE||!c.recoveryEpochH30||c.recoveryEpochH30!=pending.recoveryD82||
       !NativeMeleeCommandCurrentH28(c,f)||!kcd2vr::solver_command_h28::empty(c.solverH28)||
       f.output.solverOwnedT4[0]||!kcd2vr::solver_command_h28::empty(f.output.solverH28)||
       !finalD72::hookReady.load()||!kcd2vr::instant_d72::Combat()||!kcd2vr::combo_zone_assist_cz::enabled.load()||
       !kcd2vr::finisher_cb1::PhysicalFilterEnabledD61()||!MotionClockSelected50()||!CombatSynchronized59c10r9()||
       kcd2vr::motion59c4::selected.load()||f.request!=p.request||f.input.bodyEpoch!=p.body||
       !f.input.allowed||!f.input.attacks||!f.input.coherentMelee119||!f.input.tracked[0]||
       !kcd2vr::contact176::fresh(f.input.tick,now,80)||!f.output.active[0]||!f.output.owned[0]||
       f.profiles[0].key!=p.key||f.profiles[0].generation!=p.generation||
       c.tick<p.started||c.sample<p.sourceSample||c.sample>f.input.sample||
       (f.output.primaryStrokeGesture19&&f.output.primaryStrokeGesture19!=p.gesture)||
       PhysFinalQueuedConflictD82(c,f,now))return refuse();
    NativeOwner104 owner{};NativeParryOpponent176 target{};
    if(!NativeResolve104(f.player,owner)||reinterpret_cast<std::uintptr_t>(owner.entity)!=p.entity||
       reinterpret_cast<std::uintptr_t>(owner.actor)!=p.actor||reinterpret_cast<std::uintptr_t>(owner.state)!=p.state||owner.epoch!=p.body||
       !NativeParryOpponentRead176(owner,target)||reinterpret_cast<std::uintptr_t>(target.actor)!=p.target||
       reinterpret_cast<std::uintptr_t>(target.entity)!=p.targetEntity||target.id!=p.targetId)return refuse();
    if(pending.prepareEnteredD82){
        finalD72::Permit live{};
        if(!commit||pending.releaseEnteredD82||!NativeFinalComboPreparedD72(c,f,now)||
           !finalD72::readIdentity(live)||live.serial!=pending.contactD82.serial)return refuse();
    }else{
        // An orphaned stale commit cannot manufacture a new preparation.
        if(!prepare||now<p.started||(!pending.contactD82.serial&&
           now-p.started>kcd2vr::motion59c4::strikeLifetimeMs5910c1))return refuse();
        finalD72::Permit plan{};const auto result=NativeFinalPlanD72(owner,c,f,now,plan);
        if(result==NativeFinalResultD72::Deferred)return Q::Wait;
        if(result!=NativeFinalResultD72::Ready)return refuse();
        const kcd2vr::motion59c4::Key key{p.gesture,p.key,p.generation,p.body,p.request,pending.recoveryD82,p.player};
        if(!kcd2vr::motion59c4::hybridAttempts54.publicationAllowed(key,kcd2vr::motion59c4::Event::Prepare,plan.contact.tick,now))return refuse();
        if(!pending.contactD82.serial){plan.contactClockD82=plan.contact.tick;pending.contactD82=plan;}
    }
    const auto& contact=pending.contactD82;
    if(!NativeFinalContactCurrentD82(contact,f,now)||c.tick>contact.contact.tick||c.sample>contact.contact.sample)return refuse();
    c.tick=contact.contact.tick;c.sample=contact.contact.sample;
    return Q::Contact;
}
// Reserve before the original call, including uncertain/exceptional returns.
// A repeated pump or duplicate contact cannot dispatch either edge twice.
static bool NativeFinalEnterD82(const finalD72::Permit& p,bool prepare) noexcept {
    if(!p.contactClockD82)return true;
    auto& pending=g_final_pending_d72;
    if(pending.retiredD82||!finalD72::fresh(p,GetTickCount64())||pending.contactD82.serial!=p.serial||pending.contactD82.contact.tick!=p.contact.tick||
       pending.contactD82.contact.sample!=p.contact.sample)return false;
    if(prepare){if(pending.prepareEnteredD82)return false;pending.prepareEnteredD82=true;}
    else {if(!pending.prepareEnteredD82||pending.releaseEnteredD82)return false;pending.releaseEnteredD82=true;}
    pending.waiting=false;return true;
}

static bool NativePendingBlockCommitR18(const NativeOwner104& owner,const ph::Command& c,
    const PhysFrame6698& f,const nc104::Observed& state,std::uint64_t now) noexcept {
    const auto& p=g_prepared_input_r18;const auto& n=g_phys_native6698;
    return nc104::pendingBlockInputR18(state)&&p.owner.player==owner.player&&p.owner.actor==owner.actor&&
        p.owner.state==owner.state&&p.owner.epoch==owner.epoch&&p.source==g_input_source_generation_h14&&
        p.source&&p.input==state.input&&p.gesture&&p.gesture==c.gesture59c3&&p.key==c.key&&
        p.generation==c.generation&&p.request==c.epoch&&p.tick&&now>=p.tick&&now-p.tick<=100&&
        n.attackPrepared&&!n.heldPrepare57&&n.player==owner.player&&n.epoch==owner.epoch&&n.request==f.request&&
        n.attackKey==c.key&&n.attackGeneration117==c.generation&&n.prepareGestureC10R18==c.gesture59c3&&
        n.stroke117==c.stroke117&&n.zone117==c.zone117&&!c.kick59&&!c.held57&&
        (c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust)&&
        f.input.allowed&&f.input.coherentMelee119&&f.input.bodyEpoch==owner.epoch&&
        f.output.active[0]&&f.output.owned[0]&&f.profiles[0].key==c.key&&f.profiles[0].generation==c.generation&&
        NativePreparedOwnerMatches117(owner.player);
}
static bool NativePhysicalAction117(void* player,const ph::Command& c,const PhysFrame6698& f,bool prepare) noexcept {
    if(prepare)g_combo_zone_owner_r94={}; // a new attempt cannot inherit an earlier announcement
    bool comboZonePickedR94=false;NativeParryOpponent176 comboZoneOpponentR94{};
    g_final_pending_d72.waiting=false; // only this attempt may republish a deferred outcome
    if(!NativeMeleeCommandCurrentH28(c,f))return NativeInputReject117("T4_SOLVER_OR_UNOBSERVED_ADMISSION",c);
    const auto mode=diagnostics::lab::Get(diagnostics::lab::Key::F3);
    const bool fullHandling119=f.input.coherentMelee119&&FullWeaponMode120(g_weapon_control_mode.load());
    if(diagnostics::lab::NativeReference())return NativeInputReject117("NATIVE_REFERENCE",c);
    if(mode!=2&&mode!=3&&!fullHandling119)return !f.input.naturalCombat117&&c.stroke117==ph::CommandKind::None&&PhysCallAction6698(player,prepare?0:1,prepare?1:2);
    const auto stroke=c.stroke117;
    const auto now=std::uint64_t(GetTickCount64()),poseTick=animation_filter::combatPoseTick112.load();
    NativeOwner104 owner{};nc104::Observed before{};
    NativeParryOpponent176 observedOpponent59{};int announcedZone59=-1,zoneAck59=-1;
    NativePreopenStanceH14 preopenH14{};bool keptPreopenH14=false;
    NativeOwnedComboInputH14 comboInputH14{};bool ownedComboInputH14=false;
    finalD72::Permit finalPlanD72{};
    if(!f.input.naturalCombat117&&!fullHandling119)return NativeInputReject117("MODE",c);
    if(!g_gameplay_allowed.load())return NativeInputReject117("GAMEPLAY_CONTEXT",c);
    if(!g_head_valid.load()||!g_right_valid.load())return NativeInputReject117("LIVE_TRACKING",c);
    if(!retail_build112::Ready(WhBase())||g_input_contract117.load()!=WhBase())return NativeInputReject117("RETAIL_INPUT_CONTRACT",c);
    if(!g_phys_armed6698.load())return NativeInputReject117("PHYSICS_UNARMED",c);
    if(animation_filter::nativeBodyOwned112.load())return NativeInputReject117("NATIVE_BODY_OWNER",c);
    if(!poseTick||now<poseTick||now-poseTick>100)return NativeInputReject117("COMBAT_POSE_STALE",c);
    if(!NativeArm104())return NativeInputReject117("NATIVE_ADAPTER_UNARMED",c);
    if(f.player!=player||c.epoch!=f.request||c.bodyEpoch!=f.input.bodyEpoch||c.bodyEpoch!=g_pose_epoch6689.load())
        return NativeInputReject117("PLAYER_OR_EPOCH",c);
    if(c.side!=0||c.key!=f.profiles[0].key||c.generation!=f.profiles[0].generation)return NativeInputReject117("WEAPON_GENERATION",c);
    if(stroke!=ph::CommandKind::Slash&&stroke!=ph::CommandKind::Thrust&&stroke!=ph::CommandKind::Pommel)return NativeInputReject117("STROKE_KIND",c);
    if(!NativeResolve104(player,owner))return NativeInputReject117("EXACT_OWNER_RESOLUTION",c);
    struct RejectOwner23{const NativeOwner104* previous;~RejectOwner23(){g_reject_owner23=previous;}} rejectOwner23{g_reject_owner23};
    g_reject_owner23=&owner;
    nc104::ReadReceipt59 observe59{};
    if(!NativeObserve104(owner,before,cc::active(now)?&observe59:nullptr))
        return NativeInputReject117("NATIVE_STATE_READ",c,&observe59,&before);
    const bool pendingBlockR18=nc104::pendingBlockInputR18(before);
    if(pendingBlockR18&&(prepare||!NativePendingBlockCommitR18(owner,c,f,before,now)))
        return NativeInputReject117("PENDING_BLOCK_INPUT_NOT_OWN_ORIGINAL_RELEASE_R18",c);
    const bool blockMetadataH14=nc104::blockMetadataH14(before);
    if(blockMetadataH14&&(!prepare||!NativeBlockMetadataInputH14(owner,c,f,now)))
        return NativeInputReject117("BLOCK_METADATA_NOT_CURRENT_OWNED_INPUT",c);
    NativePreopenGuardH14 preopenGuardH14{};
    if(blockMetadataH14){const auto& guard=g_native_guard112;
        preopenGuardH14={owner,guard.leaseH14,guard.key,guard.generation,c.sample,f.input.sample,guard.side};}
    if(!prepare&&!NativePreparedOwnerMatches117(player))return NativeInputReject117("PREPARED_OWNER_REPLACED",c);
    void* manager=nullptr;void* back=nullptr;std::uint64_t key=0;
    if(!Read(static_cast<unsigned char*>(player)+0x9D0,manager)||manager!=f.manager||manager!=g_henry_item_attach_manager.load()||
       !PhysVft6698(manager,0x3EC9958)||!Read(static_cast<unsigned char*>(manager)+0x38,back)||back!=player||
       !NativeHandKey6697(manager,0,key)||key!=c.key)return NativeInputReject117("HELD_ITEM",c);
    if(prepare){
        if(kcd2vr::attack_xi178r8::enabled.load()&&kcd2vr::attack_xi178r8::rtAwaitUp51.load())
            return NativeInputReject117("XI_PREVIOUS_RELEASE_NOT_DISPATCHED",c);
        {const char* why=nullptr;float distance=-1.f,cosine=-2.f; // R59.16C10R4F: no native swing without an opponent in reach ahead
         if(stroke!=ph::CommandKind::Pommel&&!c.kick59&&!NativeStrokeTowardOpponent117(owner,why,distance,cosine,cc::active(now)?&observedOpponent59:nullptr)){
            const unsigned n=++g_attack_reach_rejects59c10;
            if(n<=20||n%25==0){char m[200]{};std::snprintf(m,sizeof(m),"R59.16C10R4F ATTACK_WITHHELD reason=%s distance=%.2f cosine=%.2f reach=%.2f stroke=%u zone=%d (blade still live; native request not sent)",
                why,double(distance),double(cosine),double(g_attack_reach_m59c10.load()),unsigned(stroke),c.zone117);diagnostic_trace::Write(m);}
            return NativeInputReject117(why,c);}}
        // Do not appropriate a request from a different input source. Native
        // ordinary attack progression may still accept the next combo input.
        if(before.requested&&!g_phys_native6698.attackPrepared){
            ownedComboInputH14=NativeReadOwnedComboInputH14(owner,c,f,now,comboInputH14);
            if(!ownedComboInputH14)return NativeInputReject117("FOREIGN_PENDING_REQUEST",c);
        }
        if(NativeMasterWindow59c17(owner)<=0){
            const auto final=NativeFinalPlanD72(owner,c,f,now,finalPlanD72);
            if(final==NativeFinalResultD72::Deferred)return false;
            if(final==NativeFinalResultD72::Refused)return NativeInputReject117("D72_FINAL_OWNER_SLOT_OR_CONTACT_CHANGED",c);
        }
        // A contact transaction admitted for a final recipe cannot become an
        // ordinary/master-window press if that route changes before dispatch.
        if(g_final_pending_d72.contactD82.contactClockD82&&
           finalD72::sameGesture(g_final_pending_d72.contactD82,reinterpret_cast<std::uintptr_t>(owner.player),
               c.gesture59c3,c.key,c.generation,c.bodyEpoch,c.epoch)&&!finalPlanD72.serial)
            return NativeInputReject117("D82_FINAL_CONTACT_ROUTE_CHANGED",c);
        if(c.masterHandoff23){
            // C23: this preparation exists only because the published live window released the guard. Recheck the
            // live count and the same window/opponent NOW; a closed/changed window is never latched or extended.
            kcd2vr::native_master23::Published pub23{};NativeParryOpponent176 opp23{};int live23=-1;
            const auto spent23=g_master_handoff_spent23.load(std::memory_order_relaxed);
            const char* why23=nullptr;
            if(spent23==c.masterHandoff23)why23="MS_HANDOFF_SERIAL_SPENT";
            else if(!NativeMasterCount23(owner,live23))why23="MS_HANDOFF_WINDOW_UNKNOWN";
            else if(live23<=0)why23="MS_HANDOFF_WINDOW_CLOSED";
            else if(!g_master_published23.read(pub23)||!pub23.available||pub23.serial!=c.masterHandoff23||
                    now<pub23.tick||now-pub23.tick>100)why23="MS_HANDOFF_WINDOW_CHANGED";
            else if(!NativeParryOpponentRead176(owner,opp23)||opp23.id!=pub23.opponentId)why23="MS_HANDOFF_OPPONENT_CHANGED";
            else if(pub23.key!=c.key||pub23.generation!=c.generation||pub23.body!=c.bodyEpoch||pub23.request!=c.epoch)why23="MS_HANDOFF_OWNER_CHANGED";
            if(why23)return NativeInputReject117(why23,c);
            g_master_handoff_spent23.store(c.masterHandoff23,std::memory_order_relaxed);
        }
        const int msWindow59c17=(!c.kick59&&stroke!=ph::CommandKind::Pommel)?NativeMasterWindow59c17(owner):0;
        if(msWindow59c17)g_ms_window_seen59c17.fetch_add(1,std::memory_order_relaxed);
        // C47: a press the master-strike gate refuses stays an ordinary attack, so it takes the VR stroke's zone, not the window's stance.
        const bool msDenied47=MasterGateEdge47(owner,c,stroke,true);
        // DS-F5: TEST receipts only. Scope: PREPARE commands that reach this
        // point after the early checks - not every raw attack press, and the
        // path is a PRE-dispatch candidate, never a completed outcome. No
        // window is forced, no stance is written. F3 capture-gated.
        if(kcd2vr::master_gate47::testForce.load(std::memory_order_relaxed)&&cc::active(now)){
            char m68[480]{};std::snprintf(m68,sizeof(m68),"C68 MS_TEST edge=PREPARE_QUALIFIED key=%016llX sample=%llu generation=%llu stroke=%u zone=%d window=%d gateDenied47=%d mode47=%d keepStance=%d candidatePath=%s nativeOutcome=NOT_YET_OBSERVED writes=NONE",
                (unsigned long long)c.key,(unsigned long long)c.sample,(unsigned long long)c.generation,unsigned(stroke),c.zone117,msWindow59c17,msDenied47?1:0,msH14::mode.load(std::memory_order_relaxed),
                g_ms_keep_stance59c17.load(std::memory_order_relaxed)?1:0,
                (msWindow59c17&&!msDenied47)?"NATIVE_WINDOW_CANDIDATE":(msDenied47&&stroke==ph::CommandKind::Thrust)?"DENIED_BY_STAB_GATE":"ORDINARY_PATH");
            diagnostic_trace::Write(m68);
        }
        // DS-F6: COMBO TEST receipts only. Log the live combo slot and Henry hit
        // flag at every attack press so the verbatim-combo chain can be traced
        // (slot open vs hit-flag advance). The game still decides everything.
        if(kcd2vr::combo_test_ds::force.load(std::memory_order_relaxed)&&cc::active(now)){
            // Codex correction: the combo slot payload is a BYTE at +0x868 (not an
            // int at +0x868/+0x870) and the Henry hit flag is the BOOL VALUE at
            // +0x488 (not the property header at +0x480). 0xFF = unreadable, so a
            // failed read is never mistaken for a 0/1 value.
            unsigned char slot868=0xFF,hit488=0xFF;
            (void)Read(static_cast<unsigned char*>(owner.state)+0x868,slot868);
            (void)Read(static_cast<unsigned char*>(owner.state)+0x488,hit488);
            char m69[320]{};std::snprintf(m69,sizeof(m69),"C69 COMBO_TEST edge=PREPARE_QUALIFIED slot868=%u hitFlag488=%u windowMs=%u result=RECEIPT_ONLY writes=NONE",
                unsigned(slot868),unsigned(hit488),kcd2vr::combo_test_ds::kWindowMs);
            diagnostic_trace::Write(m69);
        }
        if(finalPlanD72.serial){
            if(!NativeZone104(owner.actor,finalPlanD72.zone))return NativeInputReject117("D72_FINAL_AUTHORED_ZONE_CALL",c);
            NativeOwner104 again{};int zone=-1;
            if(!NativeResolve104(player,again)||again.actor!=owner.actor||again.state!=owner.state||again.epoch!=owner.epoch||
               !Read(static_cast<unsigned char*>(owner.state)+0xF8,zone)||zone!=finalPlanD72.zone||
               !NativeHandKey6697(manager,0,key)||key!=c.key)return NativeInputReject117("D72_FINAL_ZONE_OWNER_CHANGED",c);
            announcedZone59=finalPlanD72.zone;zoneAck59=zone;
        }else if(msWindow59c17&&!msDenied47&&g_ms_keep_stance59c17.load(std::memory_order_relaxed)){
            g_ms_window_presses59c17.fetch_add(1,std::memory_order_relaxed);
            int stance=-1;(void)Read(static_cast<unsigned char*>(owner.state)+0xF8,stance);
            announcedZone59=stance; // kept native master stance is observed, not an ACK of a new zone call
            char m[220]{};std::snprintf(m,sizeof(m),"R59.16C10R17 MS_WINDOW_OPEN count=%d stance=%d strokeZone=%d stroke=%u keepStance=1 (the game decides: master strike or ordinary attack)",
                msWindow59c17,stance,c.zone117,unsigned(stroke));diagnostic_trace::Write(m);
        }else if(stroke!=ph::CommandKind::Pommel){
            if(c.zone117!=0&&c.zone117!=1&&c.zone117!=2&&c.zone117!=5&&!(c.kick59&&c.zone117==3))return NativeInputReject117("INVALID_ZONE",c); // R59: zone 3 = companion kick row
            if(!msDenied47&&(stroke==ph::CommandKind::Thrust||stroke==ph::CommandKind::Slash)&&!c.kick59&&!c.held57&&
               msH14::mode.load(std::memory_order_relaxed)==1&&g_ms_keep_stance59c17.load(std::memory_order_relaxed)){
                preopenH14=NativeReadPreopenStanceH14(owner,c,f,now,preopenGuardH14);
                if(!preopenH14.ownerCurrent)return NativeInputReject117("PREOPEN_OWNER_CHANGED",c);
                keptPreopenH14=kcd2vr::preopen_stance_h14::preserve(preopenH14.physical);
            }
            if(keptPreopenH14){announcedZone59=preopenH14.physical.stance;observedOpponent59=preopenH14.opponent;}
            else {
            const int announced48=NativeComboZoneAssistCZ(owner,c,
                NativeHybridApproachZone48(owner,c,now,cc::active(now)?&observedOpponent59:nullptr),now,comboZonePickedR94,comboZoneOpponentR94); // I12
            if(!NativeZone104(owner.actor,announced48))return NativeInputReject117("ZONE_CALL",c);
            NativeOwner104 again{};
            if(!NativeResolve104(player,again)||again.actor!=owner.actor||again.state!=owner.state||again.epoch!=owner.epoch)return NativeInputReject117("OWNER_CHANGED",c);
            int zone=-1;if(!Read(static_cast<unsigned char*>(owner.state)+0xF8,zone)||zone!=announced48)return NativeInputReject117("ZONE_NOT_OBSERVED",c);
            if(!NativeHandKey6697(manager,0,key)||key!=c.key)return NativeInputReject117("ITEM_CHANGED",c);
            announcedZone59=announced48;zoneAck59=zone;
            }
        }
    }
    if(!prepare&&finalD72::sameGesture(g_final_prepared_d72,reinterpret_cast<std::uintptr_t>(player),
        c.gesture59c3,c.key,c.generation,c.bodyEpoch,c.epoch)){
        if(!NativeFinalComboPreparedD72(c,f,now))return NativeInputReject117("D72_FINAL_ORIGINAL_OWNER_CONTACT_OR_DEADLINE",c);
        finalPlanD72=g_final_prepared_d72;
        finalPlanD72.physicalKind=stroke==ph::CommandKind::Thrust?2:stroke==ph::CommandKind::Pommel?3:1;
    }
    // The old physical block must be released before this preparation reaches
    // XI. In a single packet the emitter dispatches LT-btn up before RT down.
    if(prepare&&NativePhysicalGuardHeld117(player)){
        NativeGuardRelease112("DELIBERATE_ATTACK_INPUT");
        if(c.kick59&&NativePhysicalGuardHeld117(player))return NativeInputReject117("KICK_GUARD_RELEASE_UNACKNOWLEDGED",c);
    }
    if(blockMetadataH14&&!NativeBlockMetadataReleasedH14(owner,c,f,GetTickCount64()))
        return NativeInputReject117("BLOCK_METADATA_OWNER_OR_STATE_CHANGED_AFTER_RELEASE",c);
    if(keptPreopenH14){
        // Releasing our guard crossed a native callback. Re-read identity/item
        // and the physical agreement; a genuinely opened native window is not
        // rejected or held open by us. A changed offer stays an ordinary input.
        const auto after=NativeReadPreopenStanceH14(owner,c,f,GetTickCount64(),preopenGuardH14);
        if(!after.ownerCurrent)return NativeInputReject117("PREOPEN_OWNER_CHANGED_AFTER_RELEASE",c);
        const bool samePair=after.opponent.actor==preopenH14.opponent.actor&&after.opponent.state==preopenH14.opponent.state&&
            after.opponent.id==preopenH14.opponent.id&&after.physical.offered==preopenH14.physical.offered;
        const bool retain=samePair&&(kcd2vr::preopen_stance_h14::preserve(after.physical)||
            (after.physical.count>0&&after.physical.count<64&&kcd2vr::preopen_stance_h14::samePhysicalStance(after.physical)));
        if(!retain){
            if(!after.physical.writable)return NativeInputReject117("PREOPEN_NATIVE_STATE_CHANGED",c);
            const int ordinary=NativeHybridApproachZone48(owner,c,GetTickCount64(),nullptr);
            if(!NativeZone104(owner.actor,ordinary))return NativeInputReject117("PREOPEN_FALLBACK_ZONE_CALL",c);
            const auto fallback=NativeReadPreopenStanceH14(owner,c,f,GetTickCount64(),preopenGuardH14);
            if(!fallback.ownerCurrent||fallback.physical.stance!=ordinary)return NativeInputReject117("PREOPEN_FALLBACK_OWNER_OR_ZONE",c);
            announcedZone59=ordinary;zoneAck59=fallback.physical.stance;keptPreopenH14=false;
        }
    }
    if(ownedComboInputH14){
        // Zone announcement/guard release crossed native callbacks. Revalidate
        // the SAME current action and live slot; never carry an old permission.
        NativeOwnedComboInputH14 after{};
        if(!NativeReadOwnedComboInputH14(owner,c,f,GetTickCount64(),after)||!NativeSameOwnedComboInputH14(comboInputH14,after))
            return NativeInputReject117("OWNED_COMBO_CHANGED_BEFORE_DISPATCH",c);
    }
    if(prepare||c.held57)RecordMasterIntent168(owner,c,f); // held prepare classified technique only when the real stroke starts
    int ms7D8in23=-1;(void)NativeMasterCount23(owner,ms7D8in23); // C23 receipt: live count at this edge (-1 unreadable)
    OutcomeEdge23(c,prepare,ms7D8in23); // C23b: before the dispatch, so a DIRECT-transport D4AD94 joins this gesture
    const kcd2vr::outcome23::Owner59 feintOwner59{reinterpret_cast<std::uintptr_t>(player),c.key,c.generation,c.bodyEpoch,c.epoch};
    if(cc::active(now)&&!c.kick59&&c.gesture59c3)kcd2vr::outcome23::with([&](kcd2vr::outcome23::Core& k){
        k.announce59(c.gesture59c3,feintOwner59,reinterpret_cast<std::uintptr_t>(observedOpponent59.actor),observedOpponent59.id,
            prepare?1:2,c.zone117,announcedZone59,zoneAck59,c.sample,now);});
    if(finalPlanD72.serial&&prepare){
        finalD72::Permit again{};
        if(NativeFinalPlanD72(owner,c,f,GetTickCount64(),again)!=NativeFinalResultD72::Ready||
           again.recipe!=finalPlanD72.recipe||again.nativeStep!=finalPlanD72.nativeStep||again.input!=finalPlanD72.input||
           again.contact.tick!=finalPlanD72.contact.tick||again.contact.collider!=finalPlanD72.contact.collider)
            return NativeInputReject117("D72_FINAL_CHANGED_BEFORE_DISPATCH",c);
    }
    const bool finalXiD72=kcd2vr::attack_xi178r8::enabled.load(std::memory_order_acquire);
    if(finalPlanD72.serial){
        if(prepare&&finalXiD72)finalPlanD72.xiSerial=kcd2vr::attack_xi178r8::prepareSerial.load(std::memory_order_acquire)+1;
        if(!finalD72::publish(finalPlanD72))return NativeInputReject117("D72_FINAL_PUBLICATION_BUSY",c);
    }else if(prepare){finalD72::revoke();g_final_prepared_d72={};}
    NativeInputScope117 scope{owner,stroke,prepare};scope.finalD72=finalPlanD72;scope.solverH28=c.solverH28;scope.sourceSampleH28=c.sample;
    const bool hybridCost54=NativeTimingSelected22()&&CombatSynchronized59c10r9()&&!kcd2vr::motion59c4::selected.load(std::memory_order_acquire);
    if(hybridCost54&&!c.kick59&&stroke!=ph::CommandKind::Pommel)scope.costInput54={{c.gesture59c3,c.key,c.generation,c.bodyEpoch,c.epoch,
        c.recoveryEpochH30?c.recoveryEpochH30:kcd2vr::contact176::recoveryEpoch55c1.load(),reinterpret_cast<std::uintptr_t>(player)},reinterpret_cast<std::uintptr_t>(owner.actor),reinterpret_cast<std::uintptr_t>(owner.state),c.tick};
    scope.costInput54.solverH28=c.solverH28;
    kcd2vr::swing_effort59c14::InputScope54 costInputScope54(prepare?&scope.costInput54:nullptr);
    struct Scope {NativeInputScope117* previous;~Scope(){g_input_scope117=previous;}} saved{g_input_scope117};
    g_input_scope117=&scope;
    const int name=stroke==ph::CommandKind::Pommel&&!finalPlanD72.serial?4:prepare?0:1;
    kcd2vr::attack_xi178r8::pendingStroke.store(stroke==ph::CommandKind::Thrust?2:stroke==ph::CommandKind::Pommel?3:1,std::memory_order_relaxed); // V66.178R13
    if(!prepare)NativeTimingWeapon178r22c2(owner,c,f);
    if(!prepare)(void)MasterGateEdge47(owner,c,stroke,false); // C47: the release edge's own attempt
    if(!NativeMeleeCommandCurrentH28(c,f))return NativeInputReject117("T4_ADMITTED_BEFORE_NATIVE_DISPATCH",c);
    if(!NativeFinalEnterD82(finalPlanD72,prepare))return NativeInputReject117("D82_FINAL_CONTACT_EDGE_ALREADY_ENTERED",c);
    kcd2vr::final_combo_d72::DispatchScope dispatchedFinalD72(&scope.finalD72);
    const bool returned=PhysCallAction6698(player,name,prepare?1:2);
    // Returned-but-unobserved is not an acknowledged input edge. The queue may
    // restore preparation only within the unchanged original 100 ms deadline.
    g_input_scope117=saved.previous;
    // V66.178R8: on the XI transport the retail handler runs in the game's own input pass, so the
    // input-class hook cannot witness this edge synchronously; the publication itself is the edge.
    const bool xi178r8=kcd2vr::attack_xi178r8::enabled.load(std::memory_order_relaxed);
    const bool dispatched=returned&&(scope.visited||xi178r8);
    // R116 (Miki: "the swing sound comes ~0.5 s after I start the swing"): the clip's whoosh plays when the hidden
    // native clip reaches it (160-380 ms after the hand's peak). play the same authored trigger NOW, at the released
    // cut/stab; the anim-event tap then skips the clip's own one. kicks only end the mute window. hit timing untouched.
    if(!prepare&&c.kick59)kcd2vr::swing_whoosh_r116::Cancel();
    else if(!prepare&&dispatched&&(stroke==ph::CommandKind::Slash||stroke==ph::CommandKind::Thrust)){
        const int whooshClassR116=f.profiles[0].key==c.key?f.profiles[0].classId:f.profiles[1].key==c.key?f.profiles[1].classId:-1;
        kcd2vr::swing_whoosh_r116::Request(whooshClassR116,stroke==ph::CommandKind::Thrust?kcd2vr::swing_whoosh_r116::Kind::Stab:
            kcd2vr::swing_whoosh_r116::Kind::Cut,GetTickCount64());
    }
    if(ownedComboInputH14&&cc::active(now)){char line[320]{};std::snprintf(line,sizeof(line),
        "H14 COMBO_INPUT_CONTINUATION gesture=%llu previousGesture=%llu release=%llu admission=%llu slot=LIVE_READ_ONLY dispatched=%d nativeResult=ORIGINAL comboCredit=NONE",
        (unsigned long long)c.gesture59c3,(unsigned long long)comboInputH14.native.release.gesture59c3,
        (unsigned long long)comboInputH14.native.release.serial,(unsigned long long)comboInputH14.native.admission,int(dispatched));
        diagnostic_trace::Write(line);}
    if(!xi178r8)kcd2vr::attack_xi178r8::receipt49(prepare,scope.visited);
    ObserveF11Native167(player,prepare?"MELEE_PREPARE":"MELEE_COMMIT",c.side,c.sample,
        c.originZone167,c.origin167,int(stroke),c.key,returned,scope.visited,scope.passed);
    if(!dispatched){finalD72::revoke();g_final_pending_d72.waiting=false;RevokeMasterIntent168();PhysAbortAttack117(player);}
    else NativeRememberSolverLineageH28(c,scope);
    if(dispatched&&prepare){
        g_final_pending_d72.waiting=false;g_final_prepared_d72=finalPlanD72;
        if(finalPlanD72.serial&&finalXiD72&&kcd2vr::attack_xi178r8::prepareSerial.load(std::memory_order_acquire)!=finalPlanD72.xiSerial){
            finalD72::revoke();g_final_prepared_d72={};PhysAbortAttack117(player);return false;
        }
        if(finalPlanD72.serial&&cc::active(now)){char endpoint[540]{};std::snprintf(endpoint,sizeof(endpoint),
            "D72 FINAL_CONTACT gesture=%llu target=%u contactSample=%llu contactTick=%llu height=%.3f feature=%u physicalStroke=%u authoredZone=%d nativeInput=%d cursor=%u/%u actualContact=YES originRequestTick=%llu inputClock=%llu region=HEIGHT_APPROXIMATION nativeOutcome=ORIGINAL",
            (unsigned long long)finalPlanD72.gesture,finalPlanD72.targetId,(unsigned long long)finalPlanD72.contact.sample,
            (unsigned long long)finalPlanD72.contact.tick,finalPlanD72.contact.height,unsigned(finalPlanD72.contact.feature),unsigned(stroke),
            finalPlanD72.zone,finalPlanD72.input,finalPlanD72.cursor,finalPlanD72.required,
            (unsigned long long)finalPlanD72.started,(unsigned long long)(finalPlanD72.contactClockD82?finalPlanD72.contactClockD82:finalPlanD72.started));diagnostic_trace::Write(endpoint);}
        g_input_prepared_owner117=owner;
        g_prepared_input_r18={};
        if(scope.prepareStableR18&&scope.prepareSourceR18==g_input_source_generation_h14&&
           scope.visited&&scope.passed>=0&&c.gesture59c3&&!c.kick59&&(!c.held57||c.controlledHoldH26))
            g_prepared_input_r18={owner,c.gesture59c3,c.key,c.generation,c.epoch,c.tick,scope.prepareSourceR18,scope.passed,c.controlledHoldH26};
        if(comboZonePickedR94)NativeRememberComboZoneR94(owner,c,zoneAck59,comboZoneOpponentR94);
    }
    if(pendingBlockR18&&cc::active(now))diagnostic_trace::Write(
        "R18 PENDING_BLOCK_RELEASE source=EXACT_ORIGINAL_NATIVE_PREPARE forwarded=ORIGINAL_RELEASE outcome=NATIVE");
    nc104::Observed receipt44c2{};
    const bool receiptKnown44c2=NativeObserve104(owner,receipt44c2);
    if(preopenH14.ownerCurrent&&cc::active(now)){
        const auto after=NativeReadPreopenStanceH14(owner,c,f,GetTickCount64(),preopenGuardH14);char line[420]{};
        std::snprintf(line,sizeof(line),
            "H14 PREOPEN_STANCE gesture=%llu kept=%d before{count=%d stance=%d offered=%d origin=%d ordinary=%d} after{owner=%d count=%d stance=%d offered=%d} dispatched=%d nativeAdmission=UNCLAIMED windowWrite=NONE",
            (unsigned long long)c.gesture59c3,int(keptPreopenH14),preopenH14.physical.count,preopenH14.physical.stance,
            preopenH14.physical.offered,c.originZone167,c.zone117,int(after.ownerCurrent),after.physical.count,after.physical.stance,
            after.physical.offered,int(dispatched));diagnostic_trace::Write(line);
    }
    if(cc::active(now)&&!c.kick59&&c.gesture59c3)kcd2vr::outcome23::with([&](kcd2vr::outcome23::Core& k){
        k.announcedDispatch59(c.gesture59c3,feintOwner59,c.sample,dispatched,receiptKnown44c2?receipt44c2.zone:-1,now);});
    char msg[720]{};std::snprintf(msg,sizeof(msg),
        "V66.117 MELEE_INPUT edge=%s stroke=%u zone=%d input=%d handlerObserved=%d key=%016llX sample=%llu ms7D8=%d msHandoff=%llu nativeOutcome=NOT_INFERRED transport=%s before{phase=%d type=%d input=%d requested=%d candidate=%d blocking=%d} after{known=%d phase=%d type=%d input=%d requested=%d candidate=%d blocking=%d}",
        prepare?"PREPARE":"COMMIT",unsigned(stroke),c.zone117,scope.passed,scope.visited?1:(xi178r8?2:0),
        (unsigned long long)c.key,(unsigned long long)c.sample,ms7D8in23,(unsigned long long)c.masterHandoff23,xi178r8?"XI":"DIRECT",
        before.phase,before.type,before.input,int(before.requested),int(before.requestCandidate),int(before.blocking),
        int(receiptKnown44c2),receipt44c2.phase,receipt44c2.type,receipt44c2.input,
        int(receipt44c2.requested),int(receipt44c2.requestCandidate),int(receipt44c2.blocking));
    if(dispatched){if(prepare)++g_rung1_prepare30c1;else {++g_rung1_commit30c1;
        if(stroke==ph::CommandKind::Thrust)++g_rung1_thrusts32c1;
        else if(stroke==ph::CommandKind::Slash)++g_rung1_cuts32c1;}}
    diagnostic_trace::Write(msg);
    if(c.kick59){
        // Reuse the existing edge observation. A returned input handler is not
        // proof that the native resolver selected the kick, played it, or hit.
        // This is a synchronous snapshot only (XI may process the input later).
        const char* observed=!receiptKnown44c2?"UNREADABLE":receipt44c2.type==4?"KICK_TYPE_OBSERVED":
            receipt44c2.type>=0?"OTHER_TYPE_OBSERVED":"NO_TYPE_OBSERVED";
        char kick[448]{};std::snprintf(kick,sizeof(kick),
            "R59.16C10 R3_NATIVE_RECEIPT request=%llu edge=%s dispatched=%d transport=%s observation=%s before{phase=%d type=%d} after{phase=%d type=%d zone=%d requested=%d candidate=%d blocking=%d} nativeActionOrHit=NOT_INFERRED",
            (unsigned long long)c.kickTicket59c3.id,prepare?"PREPARE":"RELEASE",int(dispatched),xi178r8?"XI":"DIRECT",observed,
            before.phase,before.type,receipt44c2.phase,receipt44c2.type,receipt44c2.zone,
            int(receipt44c2.requested),int(receipt44c2.requestCandidate),int(receipt44c2.blocking));
        diagnostic_trace::Write(kick);
    }
    return dispatched;
}
