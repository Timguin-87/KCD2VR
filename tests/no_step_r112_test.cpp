// Production predicates and dispatch over bounded native-memory fixtures.
// The original position leaf counts calls; it does not execute the game mover.
#include "motion_fixture_prefix_r112.inc"
#include "vrik/user_modes175.h"
static std::atomic<bool> g_combat_integrated178r22c2{true},g_synchronized59c10r9{true};
#include "motion_position_r112.inc"
static int checks=0,failures=0;
static void check(bool ok,const char* name){++checks;if(!ok)++failures;std::printf("%s %s\n",ok?"PASS":"FAIL",name);}
int main(){
    g_motion_contract112=baseD37;g_position_original36=&PositionLeafD40;
    AreaD37<0xB00> player,actor;AreaD37<0xD00> state;
    AreaD37<0x100> manager,slots,alignment,commands,entity,equipment;
    AreaD37<0x200> action;AreaD37<0x280> row;AreaD37<0x60> helper,type;
    nativeD37={player.ptr(),entity.ptr(),actor.ptr(),state.ptr(),1};handsD37.player=player.ptr();g_last_player_entity=entity.ptr();
    player.put(0,baseD37+0x3AA4B98);player.put(0x38,entity.ptr());player.put(0x30,std::uint32_t(77));
    actor.put(0,baseD37+0x3A80610);actor.put(0x1F8,player.ptr());actor.put(0x200,manager.ptr());actor.put(0x250,alignment.ptr());
    manager.put(0x38,slots.ptr());slots.put(0,action.ptr());action.put(0,baseD37+0x3A61C58);
    action.put(0x40,manager.ptr());action.put(0x70,actor.ptr());action.put(0x3C,0);action.put(0x28,std::uint8_t(1));
    action.put(0xA0,helper.ptr());action.put(0x88,baseD37+0x3A61440);action.put(0x58,row.ptr());
    helper.put(0,baseD37+0x3A61040);helper.put(8,actor.ptr());helper.put(0x20,action.address()+0x88);
    row.put(0x148,type.ptr());type.put(8,0);state.put(0x38,4);state.put(0x218,0);
    alignment.put(0,baseD37+0x3A633B0);alignment.put(8,actor.ptr());
    alignment.put(0x30,commands.ptr());alignment.put(0x38,commands.address()+0x30);alignment.put(0x40,commands.address()+0x100);commands.put(0,std::uint8_t(2));
    selectedD37=false;
    player.put(0x9D0,equipment.ptr());equipment.put(0,baseD37+0x3EC9958);equipment.put(0x38,player.ptr());g_henry_item_attach_manager=equipment.ptr();
    auto& frame=g_phys_frame6698.value;frame.player=player.ptr();frame.manager=equipment.ptr();
    frame.output.owned[0]=frame.output.active[0]=true;frame.profiles[0].primary=frame.profiles[0].isMelee=true;frame.profiles[0].key=nativeHandKeyD40;
    const auto expect=[&](bool veto,const char* label,std::uintptr_t caller=0,void* owner=nullptr){
        const auto before=positionCallsD40;const auto count=g_position_veto36.load();
        NativePositionDispatch36(owner?owner:alignment.ptr(),commands.ptr(),caller?caller:baseD37+0xA70FA8);
        check(positionCallsD40==before+unsigned(!veto)&&g_position_veto36.load()==count+unsigned(veto),label);
    };
    // The retained integrated and synchronized modes have the same native owner
    // configuration; the synchronization bit only changes contact authority.
    for(bool sync:{true,false}){
        const auto config=kcd2vr::user175::configuration(sync?kcd2vr::user175::Mode::CombatSynchronized:kcd2vr::user175::Mode::CombatIntegrated);
        g_combat_experiment175=config.experiment;g_physical_combat177=config.physical177;g_weapon_control_mode=config.weapon;
        g_synchronized59c10r9=sync;
        for(int kind:{0,1})for(int phase:{4,8,16}){
            type.put(8,kind);state.put(0x38,phase);
            expect(true,sync?"synchronized exact ordinary slash/thrust retains early no-step":"R112_FIXED: integrated exact ordinary slash/thrust also owns early no-step");
        }
    }
    g_synchronized59c10r9=true;type.put(8,0);state.put(0x38,4);
    g_combat_integrated178r22c2=false;expect(false,"legacy experiment without integrated native combat cannot acquire early no-step");g_combat_integrated178r22c2=true;
    g_no_step55=false;expect(false,"explicit no-step OFF remains native");g_no_step55=true;
    referenceD37=true;expect(false,"Native Reference preserves queued native position");referenceD37=false;
    for(int phase:{0,1,2,32,64,128,256}){state.put(0x38,phase);expect(false,"idle walking guard prepare/recovery phase cannot borrow an ordinary strike veto");}state.put(0x38,4);
    for(int kind:{5,6,7,8,55,62}){type.put(8,kind);expect(false,"unbound punch kick special and paired action preserve native movement");}type.put(8,0);
    bodyRequiredD37=true;expect(false,"protected native mount dismount or paired body retains command");bodyRequiredD37=false;
    animation_filter::nativeBodyOwned112=true;expect(false,"published native-body owner retains command");animation_filter::nativeBodyOwned112=false;
    for(auto* gate:{&g_ctx_menu,&g_ctx_dialogue,&g_ctx_cutscene,&g_ctx_scripted}){*gate=true;expect(false,"menu dialogue cutscene or scripted context preserves native command");*gate=false;}
    trackingD37.tick=919;expect(false,"stale tracking cannot acquire movement veto");trackingD37.tick=1000;
    trackingD37.head_held=true;expect(false,"estimated head pose cannot own no-step");trackingD37.head_held=false;
    g_left_valid=false;expect(false,"missing controller retains existing native fallback");g_left_valid=true;
    frame.input.tick=919;expect(false,"stale physical owner cannot acquire no-step");frame.input.tick=1000;
    frame.output.owned[0]=false;expect(false,"unowned weapon cannot borrow pose-only body");frame.output.owned[0]=true;
    frame.output.active[0]=false;frame.output.handlingOnly55c1[0]=true;
    g_synchronized59c10r9=false;expect(false,"integrated handling-only recovery remains ineligible");
    g_synchronized59c10r9=true;expect(true,"synchronized checked handling recovery retains existing exception");
    frame.output.active[0]=true;frame.output.handlingOnly55c1[0]=false;
    slots.put(0,helper.ptr());expect(false,"replaced current native action revokes veto");slots.put(0,action.ptr());
    equipment.put(0x38,entity.ptr());expect(false,"equipment owner backlink must still be current");equipment.put(0x38,player.ptr());
    expect(false,"unrelated native caller remains untouched",baseD37+0xA70FA9);
    actor.put(0x250,equipment.ptr());expect(false,"wrong alignment manager backlink remains native");actor.put(0x250,alignment.ptr());
    commands.put(0,std::uint8_t(1));expect(false,"combined paired command remains native");commands.put(0,std::uint8_t(3));expect(false,"rotation command remains outside position hook");commands.put(0,std::uint8_t(2));
    alignment.put(0x38,commands.address()+0x31);expect(false,"malformed queue cannot acquire command");alignment.put(0x38,commands.address()+0x30);
    g_motion_contract112=0;expect(false,"unverified existing native contract remains native");g_motion_contract112=baseD37;
    AreaD37<0xB00> npcActor,npcPlayer;AreaD37<0x100> npcEntity,npcAlignment;
    npcActor.put(0,baseD37+0x3A80610);npcActor.put(0x1F8,npcPlayer.ptr());npcPlayer.put(0,baseD37+0x3AA4B98);npcPlayer.put(0x38,npcEntity.ptr());
    npcAlignment.put(0,baseD37+0x3A633B0);npcAlignment.put(8,npcActor.ptr());expect(false,"fully typed NPC owner cannot borrow Henry's no-step",0,npcAlignment.ptr());
    const auto actorBefore=actor.bytes;const auto entityBefore=entity.bytes;const auto commandsBefore=commands.bytes;
    expect(true,"current exact owner recovers immediately after refusals");
    check(actorBefore==actor.bytes&&entityBefore==entity.bytes&&commandsBefore==commands.bytes,"veto performs no actor transform rewind queue edit or clock write");
    std::printf("no_step_r112 %d/%d passed; native_mover_and_HMD=NOT_RUN\n",checks-failures,checks);return failures?1:0;
}
