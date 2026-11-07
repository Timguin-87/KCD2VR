#define KCD2VR_NATIVE_MORTAR69_NO_MAIN
#include "native_mortar69_test.cpp"
int main(){
 prepare69();put(memory_parent.data(),0,g_base.load()+0x3A5BB90);
 const auto rd=[](std::uintptr_t at,auto& v){return SehCopy(reinterpret_cast<void*>(at),&v,sizeof(v));};
 kcd2vr::alchemy_tool18::Binding out{};const auto& b=req69.left;
 ck(!kcd2vr::alchemy_tool18::read(rd,g_base.load(),b.instance,b.player,b.character,b.attachment,b.entity,out),"native/reference caller has no Place binding grant");
 ck(kcd2vr::alchemy_tool18::read(rd,g_base.load(),b.instance,b.player,b.character,b.attachment,b.entity,out,true)&&out.type==2&&out.side==1,"qualified G3 actual left mortar binding");
 const auto& r=req69.right;
 ck(kcd2vr::alchemy_tool18::read(rd,g_base.load(),r.instance,r.player,r.character,r.attachment,r.entity,out,true)&&out.type==3&&out.side==0,"qualified G3 actual right pestle binding");
 put(rightAtt.data(),0x20,std::uintptr_t(0));
 ck(!kcd2vr::alchemy_tool18::read(rd,g_base.load(),r.instance,r.player,r.character,r.attachment,r.entity,out,true),"real native Detach removes physical grip authority");
 std::printf("native binding70: %u checks PASS\n",checks);return 0;
}
