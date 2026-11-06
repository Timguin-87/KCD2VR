// The complete production owner TU and actual native event->transfer hook.
// Only known native calls/memory/thread identity are substituted by this fixture.
#define KCD2VR_RESOURCE_D60_NO_MAIN
#include "alchemy_resource_receipt_d60_test.cpp"
static int checksPendingOwnerD72=0;
#define O72(x) do{++checksPendingOwnerD72;if(!(x)){std::printf("FAIL %d %s\n",__LINE__,#x);return 1;}}while(0)
int main(){
 using namespace kcd2vr::action_time_owner59c11;
 const auto begin=[](){reset();testAttachManager=manager.data();put(action.data(),kActContext,static_cast<void*>(actionContext.data()));
  std::thread job([](){pickupThread19c4=19728;attach();});job.join();};
 ng13::Binding b{};
 reset();O72(!ReadPendingAttachmentPresentationD72(b)); // no accepted native operation
 begin();O72(attaches==1&&notices==1&&transfers==0&&g_owner.state==PendingAttach);
 O72(!g_pick13.attached&&!g_pick13.socketObserved28&&!ReadGraspBinding(b));
 O72(ReadPendingAttachmentPresentationD72(b)&&b.acceptedAttachTickD72==1000&&b.entity==72&&b.key==key);
 O72(!b.attached&&!b.place&&!b.socketOnly28&&g_owner.state==PendingAttach&&finishes==0);
 g_pick13.pendingTerminal=ng13::Terminal::Drop;PublishBinding13();O72(!ReadPendingAttachmentPresentationD72(b));
 begin();g_cleanup_claim19c4=g_pick13.bind.token;O72(!ReadPendingAttachmentPresentationD72(b));
 begin();g_native_fault13=true;O72(!ReadPendingAttachmentPresentationD72(b));
 begin();g_context13=false;O72(!ReadPendingAttachmentPresentationD72(b));
 begin();frame(1001,0);O72(!ReadPendingAttachmentPresentationD72(b));
 begin();++g_use_generation55;O72(!ReadPendingAttachmentPresentationD72(b));
 begin();frame(1081);O72(!ReadPendingAttachmentPresentationD72(b));
 begin();g_pick13.attachAcknowledgedD72=false;PublishBinding13();O72(!ReadPendingAttachmentPresentationD72(b));
 begin();auto bad=g_pick13.bind;bad.acceptedAttachTickD72=1000;++bad.key;g_binding13.publish(bad);O72(!ReadPendingAttachmentPresentationD72(b));
 begin();frame(1020);UpdateDetour(controller.data(),.02f);O72(g_owner.state==Held&&ReadGraspBinding(b)&&!ReadPendingAttachmentPresentationD72(b));
 O72(attaches==1&&notices==1&&transfers==0); // new path cannot replay admission/inventory
 std::printf("D72 actual native accepted-receipt owner: %d PASS\n",checksPendingOwnerD72);return 0;
}
