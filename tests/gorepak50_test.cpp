// C50: separate optional gore pak - limb classification, skin filter, Lua event text (pure logic of src/vrik/gorepak50.h)
#include "../src/vrik/gorepak50.h"
#include <limits>
#include <cassert>
#include <cstdio>
#include <string>
using namespace kcd2vr::gorepak50;
static unsigned n=0;static void ck(bool b){++n;assert(b);}
int main(){
    LimbIds l{};for(int i=0;i<8;++i)*limbSlot(l,i)=100+i; // LeftHand 100, LeftForeArm 101, RightHand 102, RightForeArm 103, LeftFoot 104, LeftLeg 105, RightFoot 106, RightLeg 107
    const int fingerChain[]{100,101,90,80};ck(classifyLimb(l,150,fingerChain,4)==Cut::LeftHand);      // a left finger
    ck(classifyLimb(l,101,fingerChain+2,2)==Cut::LeftHand);                                             // left forearm itself
    const int rChain[]{103,91};ck(classifyLimb(l,102,rChain,2)==Cut::RightHand);
    const int lFootChain[]{105,95,1};ck(classifyLimb(l,104,lFootChain,3)==Cut::LeftFoot);ck(classifyLimb(l,105,lFootChain+1,2)==Cut::LeftFoot);
    const int rToe[]{106,107,96,1};ck(classifyLimb(l,160,rToe,4)==Cut::RightFoot);
    const int thigh[]{1};ck(classifyLimb(l,95,thigh,1)==Cut::None);                                     // thigh / torso: nothing
    const int upper[]{80,70};ck(classifyLimb(l,90,upper,2)==Cut::None);                                 // upper arm: nothing (elbow cut = phase 2)
    ck(classifyLimb(l,-1,nullptr,0)==Cut::None);LimbIds none{};ck(classifyLimb(none,100,fingerChain,4)==Cut::None);
    // which skin parts go to the pak
    ck(relevantSkin(Cut::LeftHand,"objects/characters/humans/male/body/male_body_hands.skin"));
    ck(relevantSkin(Cut::RightHand,"objects/characters/humans/male/clothing/hands/gloves/gloves01/gloves01.skin"));
    ck(!relevantSkin(Cut::LeftHand,"objects/characters/humans/male/clothing/feet/boots/boots01.skin"));
    ck(relevantSkin(Cut::LeftFoot,"objects/characters/humans/male/clothing/legs/hose01/hose01.skin")&&relevantSkin(Cut::RightFoot,"objects/characters/humans/male/clothing/feet/x.skin"));
    ck(!relevantSkin(Cut::Head,"objects/characters/humans/male/body/male_body.skin")&&!relevantSkin(Cut::LeftHand,nullptr));
    // tokens and paths
    ck(safeToken("clothing_hands")&&safeToken("objects/a-b/c_d.skin")&&!safeToken("a'b")&&!safeToken("a b")&&!safeToken("a\\b")&&!safeToken("")&&!safeToken(nullptr));
    char p[64];normPath("Objects\\Characters/Humans\\X.SKIN",p,sizeof(p));ck(std::string(p)=="objects/characters/humans/x.skin");
    // H24: production primary selection, in the observed attachment orders.
    const char* face="objects/characters/humans/male/head/m_head_017/m_head_017.skin";
    const char* beard="objects/characters/humans/male/head/m_head_017/beards/m_beard_04.skin";
    const char* helmet="objects/characters/humans/male/clothing/head/helmet/kettle_hat/kettlehat07/kettlehat07.skin";
    char primary[260]{};int rank=0;
    ck(!offerPrimaryHeadSkinH24("clothing_head",helmet,primary,sizeof(primary),rank));
    ck(!offerPrimaryHeadSkinH24("head_beard",beard,primary,sizeof(primary),rank));
    ck(!primary[0]&&rank==0); // no primary: native caller rolls its hide lease back
    ck(offerPrimaryHeadSkinH24("head_face",face,primary,sizeof(primary),rank));
    ck(std::string(primary)==face&&rank==2);
    ck(!offerPrimaryHeadSkinH24("head",beard,primary,sizeof(primary),rank));
    ck(!offerPrimaryHeadSkinH24("head_face",helmet,primary,sizeof(primary),rank));
    ck(std::string(primary)==face); // even misleading main-attachment names cannot supply accessories
    primary[0]=0;rank=0;
    ck(offerPrimaryHeadSkinH24("head",face,primary,sizeof(primary),rank));
    const char* better="Objects\\Characters/Humans/Male/Head/m_head_024/m_head_024.SKIN";
    ck(offerPrimaryHeadSkinH24("HEAD_FACE",better,primary,sizeof(primary),rank));
    ck(std::string(primary)=="objects/characters/humans/male/head/m_head_024/m_head_024.skin");
    ck(!offerPrimaryHeadSkinH24("head",face,primary,sizeof(primary),rank));
    ck(primaryHeadSkinRankH24("head","characters/humans/female/head/female_head.skin")==1);
    ck(primaryHeadSkinRankH24("head_face","objects/head/m_head/hair/head.skin")==0);
    ck(primaryHeadSkinRankH24("head_face","objects/body/tunic.skin")==0);
    ck(primaryHeadSkinRankH24("head_face","objects/head/head.cdf")==0);
    ck(primaryHeadSkinRankH24(nullptr,face)==0&&primaryHeadSkinRankH24("head",nullptr)==0);
    char tiny[8]{};int tinyRank=0;
    ck(!offerPrimaryHeadSkinH24("head",face,tiny,sizeof(tiny),tinyRank)&&tinyRank==0&&!tiny[0]);
    // the Lua text
    Event e{};e.target=45104;e.cut=Cut::LeftHand;e.p[0]=1;e.p[1]=2;e.p[2]=3;e.d[2]=1;e.energy=.9;
    e.nAtt=3;e.attName[0]="body_hands";e.attSkin[0]="objects/characters/humans/male/body/male_body_hands.skin";
    e.attName[1]="bad'name";e.attSkin[1]="x.skin";e.attName[2]="clothing_hands";e.attSkin[2]="objects/g.skin";
    char lua[2000];const int len=buildLua(e,lua,sizeof(lua));ck(len>0);const std::string s(lua);
    ck(s.find("KCD2VR_Gore.protocol==1")!=std::string::npos&&s.find("cut='lh'")!=std::string::npos&&s.find("target=45104")!=std::string::npos);
    ck(s.find("{n='body_hands',s='objects/characters/humans/male/body/male_body_hands.skin'}")!=std::string::npos);
    ck(s.find("bad'name")==std::string::npos&&s.find("{n='clothing_hands',s='objects/g.skin'}")!=std::string::npos);
    ck(s.find("pcall(KCD2VR_Gore.Cut")!=std::string::npos&&s.find("pak script not loaded")!=std::string::npos);
    char small[100];ck(buildLua(e,small,sizeof(small))==0);                                             // never a cut-off chunk
    Event bad=e;bad.p[0]=std::numeric_limits<double>::quiet_NaN();ck(buildLua(bad,lua,sizeof(lua))==0);Event no{};ck(buildLua(no,lua,sizeof(lua))==0);
    Event h{};h.target=7;h.cut=Cut::Head;h.headSkin="objects/characters/humans/heads/h1/head.skin";ck(buildLua(h,lua,sizeof(lua))>0&&std::string(lua).find("headSkin='objects/characters/humans/heads/h1/head.skin'")!=std::string::npos);
    // C75 replaces short hidden tokens with 24 complete attachment skin paths.
    // Exercise the actual maximum copied strings before native hiding can stand.
    const std::string longName(63,'n'),longSkin(199,'s'),longHead(259,'h');
    h.headSkin=longHead.c_str();h.nAtt=24;
    for(int i=0;i<24;++i){h.attName[i]=longName.c_str();h.attSkin[i]=longSkin.c_str();}
    char oldCapacity[7000],fullCapacity[kEventLuaCapacity61];
    ck(buildLua(h,oldCapacity,sizeof(oldCapacity))==0);
    const int complete=buildLua(h,fullCapacity,sizeof(fullCapacity));
    ck(complete>7000&&std::string(fullCapacity).find(longSkin)!=std::string::npos);
    ck(std::string(fullCapacity).find("end",complete-3)!=std::string::npos);
    // D43 elbow/knee: side keys, layer filter, cut paths, own names, the sw list in the Lua event
    ck(std::string(sideKey43(Cut::LeftHand))=="la"&&std::string(sideKey43(Cut::RightHand))=="ra"&&std::string(sideKey43(Cut::LeftFoot))=="ll"&&std::string(sideKey43(Cut::RightFoot))=="rl"&&!sideKey43(Cut::Head)&&!sideKey43(Cut::None));
    ck(relevantSkin43(Cut::LeftHand,"objects/characters/humans/male/body/male_body_arms.skin")&&relevantSkin43(Cut::RightHand,"objects/characters/humans/male/clothing/arms/arms_splited/a/a.skin"));
    ck(relevantSkin43(Cut::LeftHand,"objects/characters/humans/male/clothing/hands/gloves/gloves02/gloves02.skin")&&relevantSkin43(Cut::LeftHand,"objects/characters/humans/male/clothing/arms/arm_armour/x/x.skin"));
    ck(!relevantSkin43(Cut::LeftHand,"objects/characters/humans/male/clothing/legs/hose/x/x.skin")&&!relevantSkin43(Cut::LeftFoot,"objects/characters/humans/male/clothing/hands/x.skin"));
    ck(relevantSkin43(Cut::RightFoot,"objects/characters/humans/male/clothing/feet/boots/shoes/shoes01/shoes01.skin")&&relevantSkin43(Cut::LeftFoot,"objects/characters/humans/female/clothing/f_legs/f_shoes/a.skin"));
    ck(!relevantSkin43(Cut::LeftHand,"objects/characters/humans/male/head/m_head_017/m_head_017.skin")&&!relevantSkin43(Cut::LeftHand,"objects/characters/humans/male/hair/m_hair_03/m_hair_03.skin"));
    ck(!relevantSkin43(Cut::LeftHand,"objects/characters/humans/male/body/male_body_arms_kcdgla.skin")&&!relevantSkin43(Cut::LeftHand,"objects/characters/humans/male/body/male_body_arms.cgf")&&!relevantSkin43(Cut::LeftHand,nullptr));
    ck(!relevantSkin43(Cut::LeftHand,"objects/manmade/weapons/x/arms/y.skin")&&!relevantSkin43(Cut::Head,"objects/characters/humans/male/body/male_body.skin"));
    {char cp[220];ck(cutPath43("objects/characters/humans/male/body/male_body_arms.skin",Cut::LeftHand,cp,sizeof(cp))&&std::string(cp)=="objects/characters/humans/male/body/male_body_arms_kcdgla.skin");
     ck(cutPath43("a/b.skin",Cut::RightFoot,cp,sizeof(cp))&&std::string(cp)=="a/b_kcdgrl.skin");
     ck(!cutPath43("a/b.cgf",Cut::LeftHand,cp,sizeof(cp))&&!cutPath43("a/b.skin",Cut::Head,cp,sizeof(cp))&&!cutPath43("a/b.skin",Cut::LeftHand,cp,10));
     char nm[64];ck(cutName43("body_arms",nm,sizeof(nm))&&std::string(nm)=="kcd2vr_cut_body_arms"&&!cutName43("",nm,sizeof(nm))&&!cutName43("body_arms",nm,8));}
    {Event e43{};e43.target=7;e43.cut=Cut::LeftHand;e43.nAtt=1;e43.attName[0]="body_hands";e43.attSkin[0]="x/hands.skin";
     e43.level43=1;e43.nSw43=2;e43.swOrig[0]="body_arms";e43.swMade[0]="kcd2vr_cut_body_arms";e43.swSkin[0]="x/arms.skin";
     e43.swOrig[1]="bad name";e43.swMade[1]="kcd2vr_cut_x";e43.swSkin[1]="x/y.skin";
     char l43[2000];ck(buildLua(e43,l43,sizeof(l43))>0);const std::string t(l43);
     ck(t.find("att={{n='body_hands',s='x/hands.skin'},},lvl=1,sw={{n='body_arms',c='kcd2vr_cut_body_arms',s='x/arms.skin'},}})")!=std::string::npos);
     ck(t.find("bad name")==std::string::npos); // unsafe tokens never reach Lua
     Event old=e43;old.level43=0;ck(buildLua(old,l43,sizeof(l43))>0);ck(std::string(l43).find("lvl=")==std::string::npos&&std::string(l43).find("att={{n='body_hands',s='x/hands.skin'},}})")!=std::string::npos);}
    std::printf("gorepak50_test: %u checks OK\n",n);return 0;
}
