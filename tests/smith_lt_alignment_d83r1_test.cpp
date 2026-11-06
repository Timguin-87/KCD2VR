// Full production floor adapter and nested workpiece reader. Memory, bounds and
// model transforms are controlled leaves; no engine or native hit is simulated.
#define KCD2VR_SMITH_FLOOR_FIXTURE_ONLY_D62
#include "smith_native_floor_d56_test.cpp"

static bool held(SmithContactHistoryR38& tx,V3& goal,Q4& q,float lt=1.f){
    return SmithNativeFloorD56(reinterpret_cast<void*>(character),4,1,pose,sample,5,now,goal,q,tx,lt,&q);
}
static void checkNativePlacement(const V3& goal,const Q4& q){
    const auto& sock=g_tool_socket59c10[1].value;
    QuatT28 wrist{};ToT(wrist,goal);QToQuatT(wrist,q);
    const auto actual=AbsoluteFromRelative(wrist,sock.grip67);
    const auto native=AbsoluteFromRelative(pose.original[0],sock.relative);
    for(unsigned i=0;i<3;++i)check(near(actual.t[i],native.t[i]),"held LT visible tool stays at the current native tool translation");
    const auto aq=QNormalize(QFromQuatT(actual)),nq=QNormalize(QFromQuatT(native));
    check(std::fabs(aq.x*nq.x+aq.y*nq.y+aq.z*nq.z+aq.w*nq.w)>.99999f,"held LT preserves current native tool rotation");
}
int main(){
    for(bool nested:{false,true})for(float scale:{1.f,2.f}){
        setup(nested);kcd2vr::crafting_d67::setSmithSemiAnchors(true);
        kcd2vr::station_ws_j27::toolCollision=false;modelWorld.scale=scale;
        modelWorld.q={0,0,.38268343f,.92387953f};
        auto& sock=g_tool_socket59c10[1].value;
        QToQuatT(sock.grip67,Q4{0,.1305262f,0,.9914449f});ToT(sock.grip67,{.03f,-.04f,.08f});
        QToQuatT(sock.relative,Q4{.258819f,0,0,.9659258f});ToT(sock.relative,{.01f,.02f,.03f});
        SmithContactHistoryR38 tx{};V3 goal{.2f,.3f,1.8f};Q4 q{};
        check(held(tx,goal,q),"qualified held-LT placement admitted");checkNativePlacement(goal,q);
        // Native fore/aft is driven separately by actual station input. A
        // lateral tracked reach with no native response cannot move the mesh.
        refresh();goal={.35f,.4f,1.8f};q={};
        check(held(tx,goal,q),"fresh tracked lateral movement remains qualified");checkNativePlacement(goal,q);
        refresh();pose.original[0].t[0]+=.025f;pose.original[0].t[1]-=.045f;
        goal={.37f,.45f,2.f};q={};
        check(held(tx,goal,q),"native workpiece movement is followed during held LT");checkNativePlacement(goal,q);
        // Phase15 must not recapture a different canonical local grip.
        const auto capturedBinding=sock.smith36;
        current.phase=15;refresh();sock.smith36=capturedBinding;goal={.4f,.5f,2.f};q={};
        check(st::sameCanonicalGripD56(capturedBinding,current),"same attached grip survives passive phase15");
        check(held(tx,goal,q),"current phase15 retains explicit LT support");checkNativePlacement(goal,q);
        current.phase=11;refresh();goal={.5f,.6f,2.1f};q={};
        const auto wanted=goal;
        check(!held(tx,goal,q,0.f)&&near(goal.x,wanted.x)&&near(goal.y,wanted.y)&&near(goal.z,wanted.z),
              "release restores the current controller goal with no retained slide tail");
        check(boundsReads==0,"explicit LT remains independent of collision-query availability");
    }
    for(bool nested:{false,true}){
        setup(nested);kcd2vr::crafting_d67::setSmithSemiAnchors(false);
        auto tx=ReadSmithContactHistoryR38();V3 goal{.2f,.3f,.5f};
        check(solve(tx,goal),"native idle floor acquired");const auto floor=tx.nativeFloorD56.worldBottom;
        current.phase=16;refresh();goal.z=.4f;check(solve(tx,goal),"same-owner fall retains floor");
        current.phase=15;refresh();pose.original[0].t[2]=4;goal.z=.4f;
        check(solve(tx,goal)&&tx.nativeFloorD56.worldBottom==floor&&near(goal.z,1),
              "passive phase15 retains the idle floor instead of learning a transient pose");
        ++kcd2vr::activity_input163::witness175.smithEpochC1;refresh();goal.z=.4f;
        check(!solve(tx,goal)&&!tx.nativeFloorD56.have,"new station cannot reuse floor in phase15");
        setup(nested);current.phase=15;refresh();tx={};goal.z=.4f;
        check(!solve(tx,goal),"phase15 without an earlier idle floor cannot acquire one");
    }
    std::printf("D83R1 LT/native alignment: %u checks PASS; actual floor adapter, GPU/native collision/headset NOT_RUN\n",checks);
}
