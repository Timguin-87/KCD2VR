static bool ChargeEffort5916(const void* hs) noexcept {
    if(!g_swing_stamina.load())return true;
    if(g_effort_fault.load())return false;
    namespace se=kcd2vr::swing_effort59c14;namespace mo=kcd2vr::motion59c4;
    const auto key=g_key5912;se::Record cost{};const auto costClaim=se::ledger.reserve(key,GetTickCount64(),cost);
    if(costClaim!=se::Claim::Ready&&costClaim!=se::Claim::Complete){
        if(Allow(4,8)){char m[200]{};std::snprintf(m,sizeof(m),"C10R19C1 SWING_STAMINA attempt=%llu ledger=%u charged=UNKNOWN reason=NO_EXACT_PENDING_COST",(unsigned long long)key.gesture,unsigned(costClaim));diagnostic_trace::Write(m);}return false;}
    const auto receipt=mo::attempts.claimEffort(key,GetTickCount64());
    if(receipt==mo::EffortClaim::Duplicate){if(costClaim==se::Claim::Ready)se::ledger.finish(key,se::Status::Refused);return costClaim==se::Claim::Complete;}
    if(receipt!=mo::EffortClaim::Ready){if(costClaim==se::Claim::Ready)se::ledger.finish(key,se::Status::Refused);return false;}
    if(costClaim==se::Claim::Complete)return mo::attempts.finishEffort(key,true);
    const auto wuid=EffortWuid5919(hs,key);
    float before=0,raised=0,lowerBefore=0,after=0;
    int up=0;
    if(wuid){
        se::OwnRaiseScope own(key,wuid,float(se::ownRaiseMultiplier(cost.factor)));
        up=EffortNative5916(hs,true,before,raised);
    }
    if(up==0){
        se::ledger.finish(key,se::Status::Refused);mo::attempts.finishEffort(key,false);
        char m[220]{};std::snprintf(m,sizeof(m),"C10R19C1 SWING_STAMINA attempt=%llu route=OWN_PAIR raised=NOT_CALLED debit=NONE retry=NONE",(unsigned long long)key.gesture);diagnostic_trace::Write(m);return false;}
    // Once native entry happened, even an exception is terminal; cleanup once is not a retry/rollback.
    const int down=mo::same(g_key5912,key)?EffortNative5916(hs,false,lowerBefore,after):0;
    const bool paid=up==1&&down==1;
    se::ledger.finish(key,paid?se::Status::Complete:se::Status::Indeterminate);
    if(!paid){g_effort_fault.store(true);diagnostic_trace::Write("C10R19C1 SWING_STAMINA_FAULT outcome=INDETERMINATE nativeEntry=YES retry=NONE attackAdmission=UNCHANGED");}
    char m[400]{};std::snprintf(m,sizeof(m),
        "C10R19C1 SWING_STAMINA attempt=%llu weapon=%llu body=%llu request=%llu raised=%d lowered=%d currentBefore=%.3f currentAfter=%.3f deltaObserved=%.3f factor=%.3f route=EXACT_OWN_PAIR nativeAsync=VANILLA_UNCREDITED retry=NONE",
        (unsigned long long)key.gesture,(unsigned long long)key.weapon,(unsigned long long)key.body,(unsigned long long)key.request,up,down,before,after,after-before,cost.factor);
    const bool recorded=mo::attempts.finishEffort(key,paid);
    diagnostic_trace::Write(m);return paid&&recorded;
}
