// Included by mod.cpp and the production-connected host fixture. The CVar is
// native player-block-only; existing F4/settings/shared-rules ownership is kept.
struct BlockCooldownLease59c17 {
    static constexpr float requestedH24=.001f; // seconds: native 27BCE04 -> 9D6D24
    bool applied=false,had=false;float original=0.f;
    bool Apply(bool want,char* msg,std::size_t cap) noexcept {
        if(want==applied)return false;
        void* con=SafeGetCryConsole();void* v=con?SafeConsoleGetCVar(con,"wh_cs_BlockCooldown"):nullptr;
        if(want){
            applied=true;had=false;
            if(!v){std::snprintf(msg,cap,"H24 BLOCK_COOLDOWN_LEASE APPLY cvar=MISSING");return true;}
            original=SafeCVarGetFloat(v,.2f);
            const bool set=SafeCVarSetFloat(v,requestedH24);
            had=set&&std::fabs(SafeCVarGetFloat(v,-1.f)-requestedH24)<1.e-7f;
            std::snprintf(msg,cap,"H24 BLOCK_COOLDOWN_LEASE APPLY wh_cs_BlockCooldown=%.3g(was %.3g) written=%d scope=PLAYER_BLOCK_INPUT_ONLY nativeReleaseTimer=POSITIVE_DELAY",double(SafeCVarGetFloat(v,-1.f)),double(original),int(had));
        }else{
            bool ok=false;
            if(v&&had&&std::fabs(SafeCVarGetFloat(v,-1.f)-requestedH24)<1.e-7f)ok=SafeCVarSetFloat(v,original);
            applied=false;had=false;
            std::snprintf(msg,cap,"H24 BLOCK_COOLDOWN_LEASE RESTORE wh_cs_BlockCooldown=%.3g restored=%d",double(v?SafeCVarGetFloat(v,-1.f):-1.f),int(ok));
        }
        frik_pose_probe::SetBlockCooldownLease59c17(applied&&had);return true;
    }
    void Restore() noexcept {char m[240]{};if(Apply(false,m,sizeof(m)))diagnostic_trace::Write(m);}
};
static BlockCooldownLease59c17 g_block_cooldown59c17;
