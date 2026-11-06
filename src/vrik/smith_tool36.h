#pragma once
#include <cstdint>
#include <limits>

namespace kcd2vr::smith_tool36 {
// C37: which native smithing tool is in which of Henry's hands. Pure reader,
// read-only, no native call, no attachment creation, no inventory change.
// Retail C_Blacksmithing (primary vft 3A6A390) keeps its tools in its model
// object [instance+98] (the same +68 subobject getter 72DD20 the phase
// observer uses). Smithing start B674A4 builds them once per minigame:
//   model+2E0 hammer  C_BlacksmithingSlotToolAttachment  (vft 3A7BA48) hand 0
//   model+2E8 tongs   C_BlacksmithingSlotToolAttachment  (vft 3A7BA48) hand 1
//   model+2F0 forgeBag C_BlacksmithingToolAttachment      (vft 3A7BAA8) hand 0
//   model+2F8 workpiece C_BlacksmithingWorkpieceAttachment(vft 3A7BA18) hand 1 (857EF9)
// The anim event listener 8589E8 (C_Blacksmithing+70) calls holder vft+8
// (attach, rdx=player) / +10 (detach) for Attach/Detach Workpiece, Tongs, Hammer.
// Hammer and tongs are INVENTORY ITEMS: 193F148 copies holder+28 (WUID) into a
// pickup descriptor and calls 8CF6FC(player+9D0 item attach manager) ->
// AttachOnly 8D0404 -> 8D0758, which writes [player+990]+260+8*hand = WUID.
// So they live in hand_right / hand_left, not entity_right / entity_left.
// The workpiece goes into the tongs' own "Assetslot" when hand_left already
// holds an object (axe/horseshoe) or into Henry's entity_left (sword).
// The forge bag is a short entity_right pull (animation owned) and is NOT bound.
enum class Tool : int { None=-1, Hammer=0, Tongs=1, Workpiece=2 };
inline constexpr std::uintptr_t kInstanceVft=0x3A6A390,kModelVft=0x3A6A280,kPlayerVft=0x3AA4B98,
    kAttachmentVft=0x3A657C0,kObjectVft=0x3A68F68,kSlotToolVft=0x3A7BA48,kWorkpieceVft=0x3A7BA18,
    kItemVft=0x3A2DA88,kItemManagerVft=0x3EC9958,kRegistryGlobal=0x5487A78;
inline constexpr unsigned kModel=0x98,kHammerSlot=0x2E0,kTongsSlot=0x2E8,kWorkpieceSlot=0x2F8;
inline constexpr unsigned kHand=0x8,kHost=0xC,kEntity=0x10,kAttached=0x14,kAssetSlot=0x20,kKey=0x28;
inline constexpr unsigned kPlayerEntity=0x30,kPlayerSlots=0x990,kHandKeys=0x260;
// G1: retail 23D09E0 -> 1E3E090 is a bounded native registry read, not
// a native call made here. The key's type/index/generation and full C_Item key
// all participate. A station source entity is not necessarily the currently
// attached inventory representation of that same WUID.
struct InventoryRepresentation {
    std::uintptr_t registry=0,entry=0,item=0,link=0,manager=0;
    std::uint16_t generation=0;
};
inline bool same(const InventoryRepresentation& a,const InventoryRepresentation& b) noexcept {
    return a.item&&a.registry==b.registry&&a.entry==b.entry&&a.item==b.item&&a.link==b.link&&
        a.manager==b.manager&&a.generation==b.generation;
}
template<class Reader>
bool inventoryRepresentation(Reader&& rd,std::uintptr_t base,std::uintptr_t player,std::uint64_t key,
    std::uint32_t entity,InventoryRepresentation& out,const char*& refusal) noexcept {
    out={};const auto no=[&](const char* why){refusal=why;return false;};
    // Exactly the native type/index/generation tests at 1E3E090..1E3E0D8.
    const auto index=std::uint32_t(key)&0x3FFFFu;
    if((key>>56)!=2||!index||index>0x3D090u)return no("SMITH_ITEM_KEY_NOT_LIVE_REGISTRY_KEY");
    const auto generation=std::uint16_t(std::uint32_t(key)>>18);
    std::uintptr_t registry=0,item=0,link=0,manager=0,value=0,vft=0;
    std::uint16_t serial=0;std::uint64_t itemKey=0;std::uint32_t id=0;
    const auto offset=std::uintptr_t(0x168)+std::uintptr_t(index)*24;
    if(!rd(base+kRegistryGlobal,registry)||!registry||
       registry>std::numeric_limits<std::uintptr_t>::max()-offset-0x38)
        return no("SMITH_ITEM_REGISTRY_UNAVAILABLE");
    const auto entry=registry+offset;
    if(!rd(entry+0x28,serial)||serial!=generation||!rd(entry+0x38,item)||!item||
       !rd(item,vft)||vft!=base+kItemVft||!rd(item+0x30,itemKey)||itemKey!=key)
        return no("SMITH_ITEM_REGISTRY_MEMBERSHIP_MISMATCH");
    if(!rd(player+0x9D0,manager)||!manager||!rd(manager,vft)||vft!=base+kItemManagerVft||
       !rd(manager+0x38,value)||value!=player)return no("SMITH_ITEM_MANAGER_OWNER_MISMATCH");
    // 82D014: C_Item+88 is an ID record, never a borrowed CEntity pointer.
    if(!rd(item+0x88,link)||!link||!rd(link+0x30,id)||id!=entity)
        return no("SMITH_ITEM_BOUND_REPRESENTATION_MISMATCH");
    if(!rd(base+kRegistryGlobal,value)||value!=registry||!rd(entry+0x28,serial)||serial!=generation||
       !rd(entry+0x38,value)||value!=item||!rd(item+0x30,itemKey)||itemKey!=key||
       !rd(item+0x88,value)||value!=link||!rd(link+0x30,id)||id!=entity||
       !rd(player+0x9D0,value)||value!=manager||!rd(manager+0x38,value)||value!=player)
        return no("SMITH_ITEM_REPRESENTATION_CHANGED_DURING_READ");
    out={registry,entry,item,link,manager,generation};return true;
}
struct Binding {
    std::uintptr_t instance=0,player=0,character=0,attachment=0,model=0,holder=0,object=0;
    std::uint64_t key=0;          // hand-slot WUID (hammer/tongs); 0 for the workpiece
    InventoryRepresentation representation{}; // current native registry member when source differs
    std::uint32_t sourceEntity=0; // original station-linked entity, not necessarily entity
    std::uint32_t entity=0,host=0;int side=-1,tool=-1,phase=-1;
    std::uint64_t lifetime=0;     // copied native minigame generation; pure reader leaves it unset
    explicit operator bool() const noexcept {return instance&&holder&&entity&&side>=0&&side<2&&tool>=0&&tool<=2;}
};
inline bool same(const Binding& a,const Binding& b) noexcept {
    return bool(a)&&a.instance==b.instance&&a.player==b.player&&a.character==b.character&&
        a.attachment==b.attachment&&a.model==b.model&&a.holder==b.holder&&a.object==b.object&&a.key==b.key&&
        a.sourceEntity==b.sourceEntity&&a.representation.item==b.representation.item&&
        (!a.representation.item||same(a.representation,b.representation))&&
        a.entity==b.entity&&a.host==b.host&&a.side==b.side&&a.tool==b.tool&&a.phase==b.phase&&a.lifetime==b.lifetime;
}
// A canonical controller grip is geometry for this exact tool/holder, not a
// phase capability. Keep full same() for all native admission; only this narrow
// comparison may carry an already captured grip across 11 -> 14 -> 16.
inline bool sameGripIdentity67(const Binding& a,const Binding& b) noexcept {
    if(!a||!b)return false;
    auto geometryA=a,geometryB=b;geometryA.phase=geometryB.phase=-1;
    return same(geometryA,geometryB);
}
// Left support grips have different native heating/anvil relations.
// The hammer also attaches during ToAnvil while its native hand slot is still
// settling. Keep that relationship provisional until anvil idle; preserve the
// settled grip through strokes, but never carry it into another transfer.
inline int supportGripStageD56(int phase) noexcept {
    // D83R1: passive post-fall phase15 does not change the tool's local grip.
    return phase==8?1:(phase==11||phase==14||phase==15||phase==16||phase==17)?2:0;
}
inline bool gripCanSettleD56(const Binding& b) noexcept {
    // This only controls the persistent grip cache. Current-original controller
    // placement remains available throughout the native attachment transition.
    if(b.tool==int(Tool::Hammer))return b.phase==11;
    return b.phase==8||b.phase==11;
}
inline bool sameCanonicalGripD56(const Binding& a,const Binding& b) noexcept {
    if(!sameGripIdentity67(a,b))return false;
    if(b.tool==int(Tool::Hammer))
        return supportGripStageD56(a.phase)==2&&supportGripStageD56(b.phase)==2;
    if(b.side!=1)return true;
    const auto stage=supportGripStageD56(b.phase);
    return stage&&supportGripStageD56(a.phase)==stage;
}
// The axe/horse-shoe ToForge ADB detaches the blank from the tongs; heating
// then owns its station placement. This proves ONLY that native detached
// stage, never permission to attach the blank or move its independent entity.
template<class Reader> bool detachedForgeD56(Reader&& rd,std::uintptr_t base,const Binding& b) noexcept {
    if(!b||b.side!=1||b.tool!=int(Tool::Tongs)||b.phase!=8||!b.lifetime||!b.model)return false;
    std::uintptr_t vft=0,holder=0,again=0;std::uint32_t entity=0,host=1,idAgain=0;int hand=-1;
    unsigned char attached=255,nested=255,attachedAgain=255,nestedAgain=255;
    if(!rd(b.model,vft)||vft!=base+kModelVft||!rd(b.model+kWorkpieceSlot,holder)||!holder||
       !rd(holder,vft)||vft!=base+kWorkpieceVft||!rd(holder+kHand,hand)||hand!=1||
       !rd(holder+kEntity,entity)||!entity||!rd(holder+kHost,host)||host||
       !rd(holder+kAttached,attached)||attached||!rd(holder+kAssetSlot,nested)||nested)return false;
    return rd(b.model+kWorkpieceSlot,again)&&again==holder&&rd(holder+kEntity,idAgain)&&idAgain==entity&&
        rd(holder+kHost,host)&&!host&&rd(holder+kAttached,attachedAgain)&&!attachedAgain&&
        rd(holder+kAssetSlot,nestedAgain)&&!nestedAgain;
}
inline const char* name(int tool) noexcept {return tool==0?"HAMMER":tool==1?"TONGS":tool==2?"WORKPIECE":"NONE";}
// Interaction audit: a refused reader must say WHICH native identity failed.
// This is diagnostic evidence only. It is never a Binding and earns no pose,
// attachment, inventory or recipe permission. The optional fields are copied
// only from a class-qualified holder in the existing attachment callback.
struct ReadEvidenceI1 {
    const char* refusal="NOT_OBSERVED";
    std::uintptr_t holder=0,holderVft=0,objectVft=0;
    std::uint64_t holderKey=0,handKey=0;
    std::uint32_t linkedEntity=0,boundEntity=0,host=0,playerEntity=0;
    int side=-1,tool=-1,holderHand=-1;
    unsigned char attached=0;
    bool holderFields=false,keysRead=false;
};
// phase: the current native smith phase (0..20) copied by the exact listener
// observer for THIS minigame epoch. A phase change is a new stage: new binding.
template<class Reader>
bool read(Reader&& rd,std::uintptr_t base,std::uintptr_t instance,std::uintptr_t player,
          std::uintptr_t character,std::uintptr_t attachment,std::uint32_t entity,int phase,Binding& out,
          ReadEvidenceI1* evidence=nullptr) noexcept {
    out={};if(evidence){*evidence={};evidence->boundEntity=entity;}
    const auto refuse=[&](const char* reason){if(evidence)evidence->refusal=reason;return false;};
    if(!instance||!player||!character||!attachment||!entity)return refuse("MISSING_NATIVE_IDENTITY");
    if(phase<0||phase>20)return refuse("UNKNOWN_NATIVE_PHASE");
    std::uintptr_t vft=0,owner=0,manager=0,name=0,object=0,model=0;unsigned char active=0;std::uint32_t id=0;
    if(!rd(instance,vft)||vft!=base+kInstanceVft)return refuse("SMITH_INSTANCE_CLASS");
    if(!rd(instance+0x2C,active)||active!=1)return refuse("SMITH_INSTANCE_INACTIVE");
    if(!rd(instance+0x18,owner)||owner!=player||!rd(player,vft)||vft!=base+kPlayerVft)return refuse("SMITH_PLAYER_OWNER");
    if(
       !rd(attachment,vft)||vft!=base+kAttachmentVft||!rd(attachment+0x28,manager)||!manager||
       !rd(manager+0x70,owner)||owner!=character||!rd(attachment+0x10,name)||!name||
       !rd(attachment+0x20,object)||!object)return refuse("ATTACHMENT_CHARACTER_OR_OBJECT");
    if(!rd(object,vft))return refuse("ATTACHMENT_OBJECT_UNREADABLE");
    if(evidence)evidence->objectVft=vft;
    if(vft!=base+kObjectVft)return refuse("ATTACHMENT_OBJECT_CLASS");
    if(!rd(object+8,id)||id!=entity)return refuse("ATTACHMENT_BOUND_ENTITY");
    char text[13]{};bool ended=false;
    for(unsigned i=0;i<sizeof(text);++i){if(!rd(name+i,text[i]))return refuse("ATTACHMENT_NAME_UNREADABLE");if(!text[i]){ended=true;break;}}
    if(!ended)return refuse("ATTACHMENT_NAME_TOO_LONG");
    const auto equal=[&](const char* expected){unsigned i=0;while(text[i]&&text[i]==expected[i])++i;return text[i]==expected[i];};
    // hand_* = inventory hand slots (hammer/tongs); entity_left = workpiece without tongs.
    const int handSide=equal("hand_right")?0:equal("hand_left")?1:-1;
    const bool workpieceName=equal("entity_left");
    if(handSide<0&&!workpieceName)return refuse("ATTACHMENT_NAME_NOT_SMITH_TOOL");
    std::uint32_t playerId=0;
    if(!rd(instance+kModel,model)||!model||!rd(model,vft)||vft!=base+kModelVft||
       !rd(player+kPlayerEntity,playerId)||!playerId)return refuse("SMITH_MODEL_OR_PLAYER_ENTITY");
    const int side=handSide>=0?handSide:1;
    const int tool=handSide==0?0:handSide==1?1:2;
    const unsigned slot=tool==0?kHammerSlot:tool==1?kTongsSlot:kWorkpieceSlot;
    std::uintptr_t holder=0;int hand=-1;unsigned char attached=0,assetSlot=0;std::uint32_t toolId=0,host=0;std::uint64_t key=0;
    if(evidence){evidence->side=side;evidence->tool=tool;evidence->playerEntity=playerId;}
    if(!rd(model+slot,holder)||!holder)return refuse("SMITH_HOLDER_MISSING");
    if(evidence)evidence->holder=holder;
    if(!rd(holder,vft))return refuse("SMITH_HOLDER_UNREADABLE");
    if(evidence)evidence->holderVft=vft;
    if(vft!=base+(tool==2?kWorkpieceVft:kSlotToolVft))return refuse("SMITH_HOLDER_CLASS");
    if(!rd(holder+kHand,hand)||!rd(holder+kAttached,attached)||
       !rd(holder+kEntity,toolId)||!rd(holder+kHost,host))return refuse("SMITH_HOLDER_FIELDS_UNREADABLE");
    std::uintptr_t slots=0;
    if(evidence){
        evidence->holderFields=true;evidence->holderHand=hand;evidence->attached=attached;
        evidence->linkedEntity=toolId;evidence->host=host;
        // +10 is the station-linked source, while inventory attachment may
        // refer to its current representation. Capture both before joining
        // through exact live registry membership; matching WUIDs alone do not bind.
        if(tool!=2)evidence->keysRead=rd(holder+kKey,evidence->holderKey)&&
            rd(player+kPlayerSlots,slots)&&slots&&rd(slots+kHandKeys+8*side,evidence->handKey);
    }
    if(hand!=side)return refuse("SMITH_HOLDER_HAND_MISMATCH");
    if(attached!=1)return refuse("SMITH_HOLDER_NOT_ATTACHED");
    const auto sourceEntity=toolId;
    if(tool==2&&sourceEntity!=entity)return refuse("SMITH_LINKED_ENTITY_DIFFERS_FROM_BOUND_ENTITY");
    if(host!=playerId)return refuse("SMITH_HOLDER_HOST_MISMATCH");
    if(tool==2){
        // Assetslot (inside the tongs) is not Henry's attachment; only a direct entity_left.
        if(!rd(holder+kAssetSlot,assetSlot)||assetSlot!=0)return refuse("WORKPIECE_IS_NESTED_IN_TONGS");
    }else if(!rd(holder+kKey,key)||!key||!rd(player+kPlayerSlots,slots)||!slots){
        return refuse("SMITH_TOOL_OR_HAND_KEY_UNREADABLE");
    }else{
        std::uint64_t handKey=0;if(!rd(slots+kHandKeys+8*side,handKey)||handKey!=key)return refuse("SMITH_TOOL_KEY_DIFFERS_FROM_CURRENT_HAND");
    }
    InventoryRepresentation representation{};
    if(sourceEntity!=entity){
        if(!sourceEntity)return refuse("SMITH_LINKED_SOURCE_ENTITY_MISSING");
        // A non-runtime key cannot prove a replacement for the old exact-ID
        // guard. Retain its historical refusal for this unsupported case.
        if((key>>56)!=2)return refuse("SMITH_LINKED_ENTITY_DIFFERS_FROM_BOUND_ENTITY");
        const char* why="SMITH_ITEM_REPRESENTATION_UNPROVEN";
        if(!inventoryRepresentation(rd,base,player,key,entity,representation,why))return refuse(why);
    }
    // A replaced holder/object/slot during the read cannot certify a pose.
    std::uintptr_t again=0;std::uint64_t keyAgain=0;
    if(!rd(model+slot,again)||again!=holder||!rd(holder+kAttached,attached)||attached!=1||
       !rd(holder+kEntity,toolId)||toolId!=sourceEntity||!rd(instance+kModel,again)||again!=model||
       !rd(attachment+0x20,again)||again!=object||!rd(object+8,id)||id!=entity)return refuse("SMITH_IDENTITY_CHANGED_DURING_READ");
    if(tool!=2&&(!rd(slots+kHandKeys+8*side,keyAgain)||keyAgain!=key))return refuse("SMITH_HAND_CHANGED_DURING_READ");
    if(representation.item){
        InventoryRepresentation final{};const char* why="SMITH_ITEM_REPRESENTATION_UNPROVEN";
        if(!inventoryRepresentation(rd,base,player,key,entity,final,why)||!same(representation,final))
            return refuse("SMITH_ITEM_REPRESENTATION_CHANGED_DURING_READ");
        // The final registry sample cannot outlive its hand/holder membership.
        if(!rd(holder+kKey,keyAgain)||keyAgain!=key||!rd(slots+kHandKeys+8*side,keyAgain)||keyAgain!=key||
           !rd(player+kPlayerSlots,again)||again!=slots||!rd(model+slot,again)||again!=holder||
           !rd(holder+kAttached,attached)||attached!=1||!rd(holder+kEntity,toolId)||toolId!=sourceEntity||
           !rd(holder+kHand,hand)||hand!=side||!rd(holder+kHost,host)||host!=playerId)
            return refuse("SMITH_HAND_CHANGED_DURING_REPRESENTATION_READ");
    }
    out.instance=instance;out.player=player;out.character=character;out.attachment=attachment;
    out.representation=representation;out.sourceEntity=sourceEntity;
    out.model=model;out.holder=holder;out.object=object;out.key=key;out.entity=entity;out.host=host;
    out.side=side;out.tool=tool;out.phase=phase;if(evidence)evidence->refusal="ACCEPTED_CURRENT_BINDING";return true;
}
}
