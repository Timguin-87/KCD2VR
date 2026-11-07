#include "fsr_stereo_runtime_d82.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <chrono>
#include <future>
using namespace kcd2vr::fsr_stereo_runtime_d82;
static unsigned checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL %u line%d: %s\n",checks,__LINE__,#x);std::exit(1);}}while(0)
struct Native {unsigned id=0,calls=0;int eye=-1;bool dead=false;};
static Native native[80];static unsigned count=0;static int eye=-1;static bool failCreate=false,failDispatch=false,failRightConfigure=false,blockDispatch=false;
static unsigned failCreateAt=0,aliasCreateAt=0,createCalls=0;
static ffxContext aliasContext=nullptr;
static ffxContext failDestroyContext=nullptr;
static std::atomic<bool> entered{false},releaseDispatch{false};
static ffxDispatchDescUpscale last{};static ffxContext lastContext=nullptr;static const ffxAllocationCallbacks* allocation=nullptr;
static const ffxCreateContextDescHeader* originalCreateDesc=nullptr;
static ffxReturnCode_t Make(ffxContext* c,ffxCreateContextDescHeader* d,const ffxAllocationCallbacks* m){
 ++createCalls;CHECK(d==originalCreateDesc);
 if(failCreate||createCalls==failCreateAt){failCreate=false;*c=nullptr;return FFX_API_RETURN_ERROR_MEMORY;}
 if(createCalls==aliasCreateAt){*c=aliasContext;return 0;}
 CHECK(c&&!*c);CHECK(count<80);auto& n=native[count++];n={count,0,-1,false};*c=&n;aliasContext=*c;allocation=m;return 0;
}
static ffxReturnCode_t Destroy(ffxContext* c,const ffxAllocationCallbacks* m){CHECK(m==allocation);CHECK(c&&*c);auto* n=static_cast<Native*>(*c);CHECK(!n->dead);if(*c==failDestroyContext)return FFX_API_RETURN_ERROR_RUNTIME_ERROR;n->dead=true;*c=nullptr;return 0;}
static ffxReturnCode_t Dispatch(ffxContext* c,const ffxDispatchDescHeader* d){
 CHECK(c&&*c);auto* n=static_cast<Native*>(*c);CHECK(!n->dead);lastContext=*c;
 if(d->type!=FFX_API_DISPATCH_DESC_TYPE_UPSCALE)return 0;
 last=*reinterpret_cast<const ffxDispatchDescUpscale*>(d);
 if(blockDispatch){entered.store(true);while(!releaseDispatch.load())std::this_thread::yield();}
 if(failDispatch){failDispatch=false;return FFX_API_RETURN_ERROR_RUNTIME_ERROR;}
 // Fake SDK owns a history per context: a cross-eye read without reset fails.
 CHECK(last.reset||n->eye==eye);n->eye=eye;++n->calls;return 0;
}
static ffxReturnCode_t Configure(ffxContext* c,const ffxConfigureDescHeader*){CHECK(c&&*c);if(failRightConfigure&&static_cast<Native*>(*c)->id%2==0)return 3;return 0;}
static ffxDispatchDescUpscale Input(){
 ffxDispatchDescUpscale d{};d.header.type=FFX_API_DISPATCH_DESC_TYPE_UPSCALE;
 d.commandList=reinterpret_cast<void*>(0x1000);d.color.resource=reinterpret_cast<void*>(0x2000);d.depth.resource=reinterpret_cast<void*>(0x3000);
 d.motionVectors.resource=reinterpret_cast<void*>(0x4000);d.output.resource=reinterpret_cast<void*>(0x5000);
 d.renderSize={1706,960};d.upscaleSize={2560,1440};d.jitterOffset={0.25f,-0.125f};d.motionVectorScale={1706.f,960.f};
 d.preExposure=0.625f;d.frameTimeDelta=11.111f;d.cameraNear=0.01f;d.cameraFar=10000.f;d.cameraFovAngleVertical=1.8f;
 return d;
}
int main(){
 Runtime r({Make,Destroy,Dispatch,Configure});r.SetEnabled(true);ffxCreateContextDescUpscale desc{};desc.header.type=0x10000;desc.maxRenderSize={1706,960};desc.maxUpscaleSize={2560,1440};
 ffxApiHeader extension{};extension.type=5;desc.header.pNext=&extension;originalCreateDesc=&desc.header;
 ffxAllocationCallbacks allocator{};ffxContext c=nullptr;CHECK(r.Create(&c,&desc.header,&allocator)==0);CHECK(r.Ready());CHECK(count==2);
 auto d=Input();const auto untouched=d;View v{true,true,0,7,100,0xabc};
 eye=0;CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(lastContext==c&&last.reset);CHECK(!r.PairReady(7,100));
 auto sent=last;sent.reset=false;CHECK(std::memcmp(&sent,&d,sizeof(d))==0);CHECK(std::memcmp(&untouched,&d,sizeof(d))==0);
 eye=1;v.eye=1;CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(lastContext!=c&&last.reset);CHECK(r.PairReady(7,100));
 const auto twin=lastContext;
 for(unsigned frame=101;frame<111;++frame){v.frame=frame;for(unsigned e=0;e<2;++e){v.eye=e;eye=int(e);CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(!last.reset);CHECK(lastContext==(e?twin:c));}CHECK(r.PairReady(7,frame));}
 CHECK(r.PairReady(7,100));CHECK(!r.PairReady(8,100));CHECK(!r.PairReady(7,999));
 // Duplicate and vendor failure invalidate the whole exact pair.
 CHECK(r.Dispatch(&c,&d.header,v)!=0);CHECK(!r.PairReady(7,110));
 v.frame=111;v.eye=0;eye=0;CHECK(r.Dispatch(&c,&d.header,v)==0);v.eye=1;eye=1;failDispatch=true;CHECK(r.Dispatch(&c,&d.header,v)!=0);CHECK(!r.PairReady(7,111));
 v.frame=112;for(unsigned e=0;e<2;++e){v.eye=e;eye=int(e);CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(last.reset);}CHECK(r.PairReady(7,112));
 // Native reset supplied only on LEFT must also reset RIGHT for this pair.
 v.frame=113;v.eye=0;eye=0;d.reset=true;CHECK(r.Dispatch(&c,&d.header,v)==0);d.reset=false;v.eye=1;eye=1;CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(last.reset);
 // Menu evaluation passes through, then both eyes reset on return.
 eye=-1;d.reset=true;CHECK(r.Dispatch(&c,&d.header,{})==0);d.reset=false;v.frame=114;
 for(unsigned e=0;e<2;++e){v.eye=e;eye=int(e);CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(last.reset);}
 // Frame gap, epoch and pipeline changes do not reuse incompatible history.
 for(unsigned k=0;k<3;++k){v.frame+=k?1:3;if(k==1)++v.epoch;if(k==2)++v.pipeline;
  for(unsigned e=0;e<2;++e){v.eye=e;eye=int(e);CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(last.reset);}}
 // Invalid scoped identity, missing context and bad resource inputs cannot pass.
 v.frame++;v.valid=false;CHECK(r.Dispatch(&c,&d.header,v)!=0);v.valid=true;
 ffxContext missing=reinterpret_cast<void*>(0xffff);CHECK(r.Dispatch(&missing,&d.header,v)!=0);
 d.output.resource=d.color.resource;CHECK(r.Dispatch(&c,&d.header,v)!=0);d=Input();
 // Reactive-mask dispatch uses the selected context but cannot certify an eye.
 v.frame++;v.eye=1;ffxDispatchDescUpscaleGenerateReactiveMask reactive{};reactive.header.type=0x10006;
 CHECK(r.Dispatch(&c,&reactive.header,v)==0);CHECK(lastContext==twin);CHECK(!r.PairReady(v.epoch,v.frame));
 // PairReady must not wait for a vendor call holding the registry lock.
 v.frame++;v.eye=0;eye=0;blockDispatch=true;entered=false;releaseDispatch=false;
 std::thread worker([&]{CHECK(r.Dispatch(&c,&d.header,v)==0);});while(!entered.load())std::this_thread::yield();
 auto publication=std::async(std::launch::async,[&]{return r.PairReady(v.epoch,v.frame);});
 const bool returnedWhileVendorBlocked=publication.wait_for(std::chrono::seconds(2))==std::future_status::ready;
 releaseDispatch=true;worker.join();blockDispatch=false;
 CHECK(returnedWhileVendorBlocked);CHECK(!publication.get());
 r.SetEnabled(false);CHECK(r.Ready());CHECK(!r.PairReady(7,100));r.SetEnabled(true);CHECK(!r.PairReady(7,100));
 // Same native destruction/allocator boundary releases both contexts once.
 CHECK(r.Destroy(&c,&allocator)==0);CHECK(!c);CHECK(!r.Ready());CHECK(native[0].dead&&native[1].dead);
 // Resize/recreation cannot authorize a previously completed frame.
 CHECK(r.Create(&c,&desc.header,&allocator)==0);CHECK(!r.PairReady(7,100));
 v={true,true,0,10,400,0x123};eye=0;CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(last.reset);
 // Configuration must mirror. A failed twin config retires the pair.
 ffxConfigureDescUpscaleKeyValue config{};config.header.type=0x10007;failRightConfigure=true;
 CHECK(r.Configure(&c,&config.header)!=0);CHECK(!r.Ready());v.eye=1;CHECK(r.Dispatch(&c,&d.header,v)!=0);failRightConfigure=false;
 CHECK(r.Destroy(&c,&allocator)==0);
 // A fresh context requires LEFT first. A RIGHT-only reset after an unreset
 // LEFT cannot certify an asymmetric pair; the next complete pair resets both.
 CHECK(r.Create(&c,&desc.header,&allocator)==0);v={true,true,1,12,500,0x77};eye=1;
 CHECK(r.Dispatch(&c,&d.header,v)!=0);CHECK(!r.PairReady(12,500));
 for(unsigned frame=501;frame<503;++frame){v.frame=frame;for(unsigned e=0;e<2;++e){v.eye=e;eye=int(e);CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(last.reset==(frame==501));}}
 v.frame=503;v.eye=0;eye=0;CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(!last.reset);
 v.eye=1;eye=1;d.reset=true;CHECK(r.Dispatch(&c,&d.header,v)!=0);CHECK(!r.PairReady(12,503));d.reset=false;
 CHECK(r.RecoverableResetCut(12,503,0x77));CHECK(r.GetStatus().resetCuts==1);
 CHECK(!r.RecoverableResetCut(0,503,0x77));CHECK(!r.RecoverableResetCut(12,503,0));
 CHECK(!r.RecoverableResetCut(13,503,0x77));CHECK(!r.RecoverableResetCut(12,504,0x77));CHECK(!r.RecoverableResetCut(12,503,0x78));
 v.frame=504;for(unsigned e=0;e<2;++e){v.eye=e;eye=int(e);CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(last.reset);}
 CHECK(!r.RecoverableResetCut(12,503,0x77));
 // A native reset in both eyes must not cause an unnecessary extra reset of
 // LEFT in the following frame.
 v.frame=505;d.reset=true;for(unsigned e=0;e<2;++e){v.eye=e;eye=int(e);CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(last.reset);}d.reset=false;
 v.frame=506;for(unsigned e=0;e<2;++e){v.eye=e;eye=int(e);CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(!last.reset);}
 CHECK(r.PairReady(12,506));
 // A refused RIGHT and a completely absent RIGHT must both reset the next
 // pair together; otherwise LEFT silently retains a newer history than RIGHT.
 v.frame=507;v.eye=0;eye=0;CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(!last.reset);
 v.eye=1;eye=1;d.output.resource=d.color.resource;CHECK(r.Dispatch(&c,&d.header,v)!=0);CHECK(!r.PairReady(12,507));d=Input();
 v.frame=508;for(unsigned e=0;e<2;++e){v.eye=e;eye=int(e);CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(last.reset);}
 v.frame=509;v.eye=0;eye=0;CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(!last.reset);
 v.frame=510;for(unsigned e=0;e<2;++e){v.eye=e;eye=int(e);CHECK(r.Dispatch(&c,&d.header,v)==0);CHECK(last.reset);}
 CHECK(r.PairReady(12,510));CHECK(r.Destroy(&c,&allocator)==0);CHECK(!r.PairReady(12,506));CHECK(!r.PairReady(12,510));
 // Native failure is returned unchanged. Failed twin creation does not make
 // an unrelated, singly owned native context eligible for stereo.
 failCreate=true;CHECK(r.Create(&c,&desc.header,&allocator)==FFX_API_RETURN_ERROR_MEMORY);CHECK(!c&&!r.Ready());
 failCreateAt=createCalls+2;CHECK(r.Create(&c,&desc.header,&allocator)==0);CHECK(c&&!r.Ready());failCreateAt=0;
 v.frame=511;v.eye=0;CHECK(r.Dispatch(&c,&d.header,v)!=0);CHECK(r.Destroy(&c,&allocator)==0);
 // Captured Mono creations stay native-owned; no unsafe late clone occurs on
 // enable. Context creation with routing enabled is the only twin boundary.
 r.SetEnabled(false);const auto oldCount=count;CHECK(r.Create(&c,&desc.header,&allocator)==0);CHECK(count==oldCount+1);
 r.SetEnabled(true);CHECK(!r.Ready());CHECK(r.Dispatch(&c,&d.header,v)!=0);CHECK(r.Destroy(&c,&allocator)==0);
 // Bounded receipts accept delayed publication of any retained completed
 // pair, not only the newest SDK evaluation.
 CHECK(r.Create(&c,&desc.header,&allocator)==0);v={true,true,0,15,600,0x99};
 for(unsigned frame=600;frame<641;++frame){v.frame=frame;for(unsigned e=0;e<2;++e){v.eye=e;eye=int(e);CHECK(r.Dispatch(&c,&d.header,v)==0);}}
 CHECK(!r.PairReady(15,600));CHECK(r.PairReady(15,610));CHECK(r.PairReady(15,640));CHECK(!r.PairReady(15,641));
 CHECK(r.Destroy(&c,&allocator)==0);
 // An SDK returning an alias cannot cause the native context to be released
 // by the failed twin creation cleanup, or make that context stereo-ready.
 aliasCreateAt=createCalls+2;CHECK(r.Create(&c,&desc.header,&allocator)==0);aliasCreateAt=0;
 CHECK(c&&!static_cast<Native*>(c)->dead&&!r.Ready());CHECK(r.Destroy(&c,&allocator)==0);
 // Destruction failure retains the inaccessible twin rather than losing or
 // reusing its lifetime record; native destruction still runs exactly once.
 CHECK(r.Create(&c,&desc.header,&allocator)==0);auto* primary=static_cast<Native*>(c);
 auto* failedTwin=&native[count-1];failDestroyContext=failedTwin;
 CHECK(r.Destroy(&c,&allocator)==0);CHECK(!c&&primary->dead&&!failedTwin->dead&&!r.Ready());failDestroyContext=nullptr;
 // Context capacity is bounded. A ninth native creation remains native-owned
 // and is refused when evaluated as an immersive view, without overwriting any
 // of the eight live pair records.
 Runtime full({Make,Destroy,Dispatch,Configure});full.SetEnabled(true);ffxContext contexts[9]{};
 for(auto& context:contexts)CHECK(full.Create(&context,&desc.header,&allocator)==0);
 CHECK(full.GetStatus().live==8&&full.GetStatus().twins==8);
 v={true,true,0,19,900,0xa0};eye=0;CHECK(full.Dispatch(&contexts[8],&d.header,v)!=0);CHECK(!full.PairReady(19,900));
 for(auto& context:contexts){CHECK(full.Destroy(&context,&allocator)==0);}
 CHECK(!full.Ready());
 // A cut receipt is usable only until the next dispatch/lifecycle operation.
 Runtime cut({Make,Destroy,Dispatch,Configure});cut.SetEnabled(true);ffxContext cc=nullptr;
 CHECK(cut.Create(&cc,&desc.header,&allocator)==0);View cv{true,true,0,51,1000,0xb0};
 auto cutNow=[&](){d=Input();
  for(unsigned f=0;f<2;++f){++cv.frame;for(unsigned e=0;e<2;++e){cv.eye=e;eye=int(e);CHECK(cut.Dispatch(&cc,&d.header,cv)==0);}}
  ++cv.frame;cv.eye=0;eye=0;CHECK(cut.Dispatch(&cc,&d.header,cv)==0);CHECK(!last.reset);
  cv.eye=1;eye=1;d.reset=true;CHECK(cut.Dispatch(&cc,&d.header,cv)!=0);d.reset=false;
  CHECK(cut.RecoverableResetCut(cv.epoch,cv.frame,cv.pipeline));CHECK(!cut.PairReady(cv.epoch,cv.frame));
 };
 cutNow();CHECK(cut.Dispatch(&cc,&d.header,cv)!=0);CHECK(!cut.RecoverableResetCut(cv.epoch,cv.frame,cv.pipeline)); // duplicate failure isn't a cut
 cutNow();CHECK(cut.Configure(&cc,&config.header)==0);CHECK(!cut.RecoverableResetCut(cv.epoch,cv.frame,cv.pipeline));
 cutNow();cut.SetEnabled(false);cut.SetEnabled(true);CHECK(!cut.RecoverableResetCut(cv.epoch,cv.frame,cv.pipeline));
 cutNow();cut.SetApi({Make,Destroy,Dispatch,Configure});CHECK(!cut.RecoverableResetCut(cv.epoch,cv.frame,cv.pipeline));
 cutNow();const auto cutFrame=cv.frame;CHECK(cut.Destroy(&cc,&allocator)==0);
 CHECK(!cut.RecoverableResetCut(cv.epoch,cutFrame,cv.pipeline));
 CHECK(cut.Create(&cc,&desc.header,&allocator)==0);CHECK(!cut.RecoverableResetCut(cv.epoch,cutFrame,cv.pipeline));
 CHECK(cut.Destroy(&cc,&allocator)==0);

 std::printf("PASS %u checks; actual FFX runtime, per-eye history, native descriptors, lifetime and nonblocking publication\n",checks);
}
