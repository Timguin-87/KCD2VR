#pragma once
#include <cstdint>

namespace kcd2vr::alchemy_tool18 {
// D56: the retort is an authored station fixture. Slot5 is a carried empty
// phial; it must not inherit the retort's native-only presentation rule.
inline constexpr bool nativeFixtureD56(int type) noexcept { return type==4; }
// Retail's alchemy tools are entity_right/entity_left attachments, not CItems
// in hand_right/hand_left. Only read the already attached native tool; never
// create an attachment or change the minigame's action/inventory state.
struct Binding {
    std::uintptr_t instance=0,player=0,character=0,attachment=0;
    std::uint32_t entity=0;int side=-1,type=-1;
    std::uintptr_t action=0,actionClass=0; // optional exact action for temporary resource/mortar attachments
    std::uintptr_t object=0; // current native IAttachmentObject, not only a bone/name
    std::uint64_t lifetime=0; // copied native activity generation; pure reader leaves it unset
    explicit operator bool() const noexcept {return instance&&entity&&side>=0&&side<2;}
};
inline bool same(const Binding& a,const Binding& b) noexcept {
    return bool(a)&&a.instance==b.instance&&a.player==b.player&&a.character==b.character&&
        a.attachment==b.attachment&&a.entity==b.entity&&a.side==b.side&&a.type==b.type&&
        a.object==b.object&&a.lifetime==b.lifetime&&a.action==b.action&&a.actionClass==b.actionClass;
}
inline bool sameMortarGrip69(const Binding& a,const Binding& b) noexcept {
 return bool(a)&&bool(b)&&(a.type==2||a.type==3)&&a.type==b.type&&a.side==b.side&&
  a.instance==b.instance&&a.player==b.player&&a.character==b.character&&
  a.attachment==b.attachment&&a.entity==b.entity&&a.object==b.object&&a.lifetime==b.lifetime;
}

// D44: the pestle is attached during the authored take (mortar phase 1).
// Freezing that transient ORIGINAL hand/socket offset can leave it below the
// controller for the complete grind. Only the exact current working mortar
// child may qualify the persistent grip; provisional placement stays live.
// Witness is the copied alchemy_semi46::WorkWitness (Mortar kind == 2).
template<class Witness>
inline bool pestleGripReadyD44(const Binding& b,const Witness& w,std::uint64_t now) noexcept {
 return bool(b)&&b.type==3&&b.side==0&&b.action&&b.lifetime&&
  w.instance==b.instance&&w.lifetime==b.lifetime&&w.parent==b.action&&
  w.child&&w.kind==2&&w.working&&(w.phase==2||w.phase==3)&&
  w.tick&&now>=w.tick&&now-w.tick<=80;
}

template<class Reader>
bool read(Reader&& rd,std::uintptr_t base,std::uintptr_t instance,std::uintptr_t player,
          std::uintptr_t character,std::uintptr_t attachment,std::uint32_t entity,Binding& out,bool place70=false) noexcept {
    out={};if(!instance||!player||!character||!attachment||!entity)return false;
    std::uintptr_t vft=0,owner=0,manager=0,name=0,object=0;unsigned char active=0;
    if(!rd(instance,vft)||vft!=base+0x3F60B08||!rd(instance+0x2C,active)||active!=1||
       !rd(instance+0x18,owner)||owner!=player||!rd(player,vft)||vft!=base+0x3AA4B98||
       !rd(attachment,vft)||vft!=base+0x3A657C0||!rd(attachment+0x28,manager)||!manager||
       !rd(manager+0x70,owner)||owner!=character||!rd(attachment+0x10,name)||!name||
       !rd(attachment+0x20,object)||!object)return false;
    char text[13]{};bool ended=false;
    for(unsigned i=0;i<sizeof(text);++i){if(!rd(name+i,text[i]))return false;if(!text[i]){ended=true;break;}}
    if(!ended)return false;
    const auto equal=[&](const char* expected){unsigned i=0;while(text[i]&&text[i]==expected[i])++i;return text[i]==expected[i];};
    const int side=equal("entity_right")?0:equal("entity_left")?1:-1;
    if(side<0)return false;
    // 2E19628 passes owner+108 to 6C677C, whose +198+4*hand write is
    // owner+2A0/+2A4: ordinary held-tool membership. 21 means empty.
    // Preserve this route even when the action which attached the tool has ended.
    std::uint32_t type=21,id=0;
    const bool ordinary=rd(instance+0x2A0+4*side,type)&&type<=20&&
        rd(instance+0x150+4*type,id)&&id==entity;
    std::uintptr_t first=0,last=0,action=0,actionClass=0,actionOwner=0,director=0;
    unsigned char started=0;int slot=-1;
    bool retained69=false;
    if(!ordinary){
        // Resource and mortar actions attach temporary entities without publishing
        // matching membership for each hand. Only these exact live actions may
        // supplement ordinary membership; they never replace it globally.
        // 73795C/15FC57C pass owner+78 to the native director. 8D2450 and
        // 737A7C read its +38/+40 live slot vector; ordinary tool actions use slot 1.
        if(!rd(instance+0x78,vft)||vft!=base+0x3BFA5B0||
           !rd(instance+0xB0,first)||!rd(instance+0xB8,last)||!first||last<first||
           (last-first)%8||(last-first)<16||(last-first)>256||
           !rd(first+8,action))return false;
        if(!action){
            unsigned char phase=255;std::uint32_t right=21,left=21;
            if(!rd(instance+0x300,phase)||phase!=2||!rd(instance+0x2A0,right)||right!=2||
               !rd(instance+0x2A4,left)||left!=21)return false;
            retained69=true;type=side==1?2:3;
        }else if(!rd(action,actionClass)||!rd(action+0x58,actionOwner)||actionOwner!=instance||
           !rd(action+0x40,director)||director!=instance+0x78||
           !rd(action+0x28,started)||started!=1||!rd(action+0x3C,slot)||slot!=1)return false;
        if(retained69){} // independently joined actual entity_left2 / entity_right3
        else if(actionClass==base+0x3A5C670){ // C_UseResourceAction: 6C29D8 attaches selected carafe LEFT
            if(side!=1||!rd(instance+0x304,type)||type<13||type>16)return false;
        }else if(actionClass==base+0x3F60770||(place70&&actionClass==base+0x3A5BB90)){ // C_UseMortarAction: 6C3C50 left mortar, right pestle
            type=side==1?2:3;
        }else return false; // other workstation actions need their own proven binding
    }
    if(!rd(instance+0x150+4*type,id)||id!=entity||
       !rd(object,vft)||vft!=base+0x3A68F68||!rd(object+8,id)||id!=entity)return false;
    // A replaced slot/object during the read cannot reuse a previous tool pose.
    std::uintptr_t again=0;
    if(ordinary){
        std::uint32_t current=21;
        if(!rd(instance+0x2A0+4*side,current)||current!=type)return false;
    }else if(retained69){
        unsigned char phase=255;std::uint32_t right=21,left=21;
        if(!rd(instance+0xB0,again)||again!=first||!rd(first+8,again)||again||
           !rd(instance+0x300,phase)||phase!=2||!rd(instance+0x2A0,right)||right!=2||
           !rd(instance+0x2A4,left)||left!=21)return false;
    }else if(!rd(instance+0xB0,again)||again!=first||!rd(first+8,again)||again!=action||
       !rd(action,again)||again!=actionClass||!rd(action+0x28,started)||started!=1)return false;
    if(!rd(attachment+0x20,again)||again!=object||!rd(object+8,id)||id!=entity)return false;
    out={instance,player,character,attachment,entity,side,int(type)};
    out.action=action;out.actionClass=actionClass;out.object=object;return true;
}
}
