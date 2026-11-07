// Explicit host model of the independently disassembled native handler latches.
// Does not load WHGame and does not stand in for a headset acceptance test.
static bool model117=false,specialAllowed117=true,route117=true,realBeginR18=false;
static int primaryLatch117=0,specialLatch117=0,beginInput117=-1,releaseInput117=-1;
static unsigned begins117=0,commits117=0,blockPresses117=0,blockReleases117=0;
static std::array<unsigned char,0x30> control117{};
static void ModelAction117(const std::string& name,int mode){
 if(!model117)return;
 if(name=="block"){blockPresses117+=mode==1;blockReleases117+=mode==2;return;}
 if(!route117)return;
 auto edge=[&](bool prepare,bool special){
  int id=-1;Read(reinterpret_cast<void*>(WhBase()+(special?0x5321018:0x5321014)),id);
  const auto caller=WhBase()+(special?(prepare?0x27C0998:0x27C0952):(prepare?0x27C08E6:0x27C0916));
  if(prepare&&realBeginR18){
   const auto saved=hostReturnAddressValue;hostReturnAddressValue=reinterpret_cast<void*>(caller);
   NativeInputBegin117(control117.data(),id);hostReturnAddressValue=saved;
   id=g_input_scope117?g_input_scope117->passed:-1;
  }else id=NativeInputClass117(control117.data(),id,caller,prepare);
  if(prepare){beginInput117=id;++begins117;put(nativeState+0x2A8,std::uint8_t(1));put(nativeState+0x248,id);}
  else {releaseInput117=id;++commits117;put(nativeState+0x2A8,std::uint8_t(0));put(nativeState+0x2D8,std::uint8_t(1));}
 };
 if(name=="attack_abort"&&mode==1){primaryLatch117=specialLatch117=2;put(nativeState+0x2A8,std::uint8_t(0));}
 if(name=="prepare_to_attack"&&primaryLatch117==0){edge(true,false);primaryLatch117=1;}
 if(name=="attack_primary"){if(primaryLatch117==1)edge(false,false);primaryLatch117=0;}
 if(name=="attack_special"){
  if(mode==1&&primaryLatch117!=1&&specialAllowed117){edge(true,true);specialLatch117=1;}
  if(mode==2){if(specialLatch117==1)edge(false,true);specialLatch117=0;}
 }
}
