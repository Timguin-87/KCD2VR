// D56: positive script-side male HUMAN proof, requested without side effects.
// The qualified original-first script callback binds its answer to the same
// native entity + character before and after the Lua getter; no guessed ABI.
namespace eligibilityD56=kcd2vr::gore_eligibility_d56;
static kcd2vr::core::SnapshotMailbox<eligibilityD56::Request> g_eligibilityRequestsD56[8];
static kcd2vr::core::SnapshotMailbox<eligibilityD56::Receipt> g_eligibilityReceiptsD56[8];
static std::atomic<std::uint64_t> g_eligibilitySerialD56{0},g_eligibilityUntilD56{0};
static bool EligibilityOwnerD56(const eligibilityD56::Owner& o) noexcept {
    if(!o.target||!o.entity||!o.character||o.epoch!=g_pose_epoch6689.load(std::memory_order_acquire))return false;
    void* e=FlinchEntityById178r11(o.target);
    return reinterpret_cast<std::uintptr_t>(e)==o.entity&&e!=g_last_player_entity.load(std::memory_order_acquire)&&
        reinterpret_cast<std::uintptr_t>(CharacterForEntity6689(e))==o.character;
}
static bool EligibilityWantedD56() noexcept {return g_eligibilityUntilD56.load(std::memory_order_acquire)!=0;}
static void EligibilityServiceD56(const kcd2vr::native_script_t2::Context& context) noexcept {
    const auto until=g_eligibilityUntilD56.load(std::memory_order_acquire);if(!until)return;
    if(context.now>until){auto expected=until;(void)g_eligibilityUntilD56.compare_exchange_strong(expected,0);return;}
    static std::uint64_t done[8]{};
    for(unsigned i=0;i<8;++i){eligibilityD56::Request q{};
        if(!g_eligibilityRequestsD56[i].read(q)||!q.serial||q.serial==done[i]||context.now<q.tick||context.now-q.tick>500)continue;
        done[i]=q.serial;eligibilityD56::Receipt answer{q,context.now,context.generation,eligibilityD56::Result::Refused};
        char input[96]{},reply[160]{},expected[160]{};
        std::snprintf(input,sizeof(input),"D56|%llu|%u",(unsigned long long)q.serial,q.owner.target);
        std::snprintf(expected,sizeof(expected),"D56|%llu|%u|MALE_HUMAN",(unsigned long long)q.serial,q.owner.target);
        if(EligibilityOwnerD56(q.owner)&&context.CallPacket("kcd2vr_gore_eligibility_d56",input,reply,sizeof(reply))&&
           !std::strcmp(reply,expected)&&context.Current()&&EligibilityOwnerD56(q.owner))answer.result=eligibilityD56::Result::MaleHuman;
        (void)g_eligibilityReceiptsD56[i].publish(answer);
        char line[240]{};std::snprintf(line,sizeof(line),"D56 GORE_ELIGIBILITY target=%u request=%llu result=%s nativeIdentity=EXACT_ENTITY_AND_CHARACTER scriptIdentity=ENTITY_HUMAN_SOUL_RECHECKED",
            q.owner.target,(unsigned long long)q.serial,answer.result==eligibilityD56::Result::MaleHuman?"MALE_HUMAN":"REFUSED_OR_UNKNOWN");diagnostic_trace::Write(line);
    }
}
static bool InstallEligibilityD56() noexcept {
    return kcd2vr::native_script_t2::Register(&EligibilityServiceD56,&EligibilityWantedD56)&&kcd2vr::native_script_t2::Install();
}
static eligibilityD56::Result EligibilityD56(std::uint32_t target,std::uintptr_t identity,void* character,std::uint64_t now) noexcept {
    const eligibilityD56::Owner o{target,identity,reinterpret_cast<std::uintptr_t>(character),g_pose_epoch6689.load(std::memory_order_acquire)};
    if(!now||!EligibilityOwnerD56(o))return eligibilityD56::Result::Refused;
    const unsigned slot=target%8;eligibilityD56::Receipt r{};
    if(g_eligibilityReceiptsD56[slot].read(r)&&eligibilityD56::fresh(r,o,now))return r.result;
    eligibilityD56::Request q{};
    if(!g_eligibilityRequestsD56[slot].read(q)||!eligibilityD56::same(q.owner,o)||now<q.tick||now-q.tick>80||
       (r.request.serial==q.serial&&r.tick&&now>=r.tick&&now-r.tick>80)){
        const auto serial=g_eligibilitySerialD56.fetch_add(1,std::memory_order_acq_rel)+1;
        if(!serial)return eligibilityD56::Result::Refused;
        q={o,serial,now};(void)g_eligibilityRequestsD56[slot].publish(q);
    }
    g_eligibilityUntilD56.store(now+500,std::memory_order_release);return eligibilityD56::Result::Pending;
}
static bool EligibleTargetD56(std::uint32_t target,std::uint64_t now) noexcept {
    void* e=FlinchEntityById178r11(target);void* ci=e?CharacterForEntity6689(e):nullptr;
    return e&&ci&&EligibilityD56(target,reinterpret_cast<std::uintptr_t>(e),ci,now)==eligibilityD56::Result::MaleHuman;
}
