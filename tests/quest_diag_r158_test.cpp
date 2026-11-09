// R158 Shift+F5 quest progression recorder: row gate (change + 2 s repeat), NPC table (new / changed / gone /
// untracked), Lua reply parser (serial check, bounded, cleaned), state row text, Quest capture domain routing.
#include "../src/diagnostics/quest_diag_r158.h"
#include "../src/diagnostics/capture168.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
namespace q=kcd2vr::quest_diag_r158;
namespace cap=kcd2vr::diagnostics::capture168;
static int checks=0;
#define CK(x) do{assert(x);++checks;}while(0)
int main(){
    // gate: first = changed, same text inside 2 s = skip, at 2 s = repeat, new text = changed again
    q::Gate g;CK(g.Due("a=1",1000)==1);CK(g.Due("a=1",1500)==0);CK(g.Due("a=1",2999)==0);CK(g.Due("a=1",3000)==2);
    CK(g.Due("a=1",3001)==0);CK(g.Due("a=2",3002)==1);CK(g.Due("a=2",100)==2); // clock went back: repeat, no stall
    g.Reset();CK(g.Due("a=2",5000)==1);CK(g.Due(nullptr,5000)==0);

    // NPC table
    q::NpcTable t;std::vector<std::string> gone;
    auto end=[&]{return t.EndPass([&](const char* id,const char* name){gone.push_back(std::string(id)+"/"+name);});};
    t.BeginPass();CK(t.Observe("1A","hans","dead=0")==1);CK(t.Observe("2B","rosa","dead=0")==1);CK(end()==0);CK(t.Count()==2);
    t.BeginPass();CK(t.Observe("1A","hans","dead=0")==0);CK(t.Observe("2B","rosa","chatRequest=1")==2);CK(end()==0);
    t.BeginPass();CK(t.Observe("2B","rosa","chatRequest=1")==0);CK(end()==1);CK(gone.size()==1&&gone[0]=="1A/hans");CK(t.Count()==1);
    t.BeginPass();CK(t.Observe("1A","hans","dead=0")==1); // back in range = new again
    for(unsigned i=0;i<q::kMaxNpcs;++i){char id[8];std::snprintf(id,sizeof id,"X%u",i);(void)t.Observe(id,"x","r");}
    CK(t.Observe("FULL","x","r")==-1);CK(t.Count()==q::kMaxNpcs); // never dropped silently, written as untracked
    t.Reset();CK(t.Count()==0);

    // reply parser
    std::vector<std::string> got;
    auto sink=[&](char k,const char* id,const char* name,const char* rest){got.push_back(std::string(1,k)+"["+id+"]["+name+"]"+rest);};
    CK(q::ParseReply("Q158|7\nP|combatDanger=1 npcs25m=2\nN|0000A1|hans_capon|d=3.0 dead=0\nN|B2|rosa|d=9.5 chatRequest=1\nE|2_more",7,sink));
    CK(got.size()==4);CK(got[0]=="P[][]combatDanger=1 npcs25m=2");CK(got[1]=="N[0000A1][hans_capon]d=3.0 dead=0");
    CK(got[2]=="N[B2][rosa]d=9.5 chatRequest=1");CK(got[3]=="E[][]2_more");
    got.clear();CK(!q::ParseReply("Q158|6\nP|x",7,sink));CK(got.empty());          // an older answer is not this one
    CK(!q::ParseReply("Q158|71\nP|x",7,sink));CK(!q::ParseReply("Q158|BAD",7,sink));CK(!q::ParseReply(nullptr,7,sink));CK(got.empty());
    CK(q::ParseReply("Q158|7",7,sink)&&got.empty());                               // header only = parsed, no rows
    CK(q::ParseReply("Q158|7\nN|noname\nN||x|y\nZ|junk\nP|ok\x01|bar",7,sink));    // malformed N lines skipped, text cleaned
    CK(got.size()==1&&got[0]=="P[][]ok??bar");
    got.clear();std::string longRow="Q158|8\nP|"+std::string(2000,'a');CK(q::ParseReply(longRow.c_str(),8,sink));
    CK(got.size()==1&&got[0].size()==5+q::kRowMax-1);                              // bounded to one row

    // state row: unknown reads stay visible as -1 / ?
    q::StateIn s{};char row[q::kRowMax]{};CK(q::FormatState(row,sizeof row,s)>0);
    CK(std::strstr(row,"henryInCombat=-1 dialogue{known=0 active=0 instance=0")!=nullptr);
    CK(std::strstr(row,"inputMap=?")!=nullptr&&std::strstr(row,"wh_ui_TutorialsEnabled=? wh_cs_ForceBlock=?")!=nullptr);
    CK(std::strstr(row,"opponent{seen=0 id=0 reciprocal=0 skill=-1}")!=nullptr&&std::strstr(row,"lua=NOT_ASKED")!=nullptr);
    s.henryInCombat=1;s.dlgKnown=true;s.dlgInputKnown=true;s.dlgInputEnabled=false;s.tutorialsCvar=1;s.forceBlockCvar=0;
    s.opponentFresh=true;s.opponent=0xABC;s.opponentReciprocal=true;s.opponentSkill=24;s.lua="OK";
    CK(q::FormatState(row,sizeof row,s)>0);
    CK(std::strstr(row,"henryInCombat=1 ")&&std::strstr(row,"inputMap=DISABLED")&&std::strstr(row,"wh_ui_TutorialsEnabled=1 wh_cs_ForceBlock=0"));
    CK(std::strstr(row,"opponent{seen=1 id=ABC reciprocal=1 skill=24}")&&std::strstr(row,"lua=OK"));
    s.opponentFresh=false;CK(q::FormatState(row,sizeof row,s)>0);CK(std::strstr(row,"opponent{seen=0 id=0 reciprocal=0 skill=-1}")); // stale = not shown
    CK(std::strlen(row)<q::kRowMax-60);

    // Quest capture domain: own 180 s lease, own session, not part of any(), begin/end receipts routed to it
    cap::Gate gate;CK(cap::DomainCount==10&&unsigned(cap::Domain::Quest)==9&&unsigned(cap::Domain::Physics)==8);
    gate.select(cap::Domain::Quest,true,1000);CK(gate.active(cap::Domain::Quest,1001));CK(!gate.any(1001));
    CK(!gate.active(cap::Domain::Interaction,1001));                               // plain F5 recorder untouched
    CK(gate.poll(1000+cap::Duration)==(1u<<9));CK(!gate.active(cap::Domain::Quest,1000+cap::Duration));
    CK(gate.receipt(cap::Domain::Quest,gate.session[9].load(),1000+cap::Duration+4000));
    CK(!gate.receipt(cap::Domain::Quest,gate.session[9].load(),1000+cap::Duration+6000));
    CK(std::strcmp(cap::name(cap::Domain::Quest),"QUEST")==0);
    CK(cap::receiptDomain("V66.168 CAPTURE domain=QUEST session=1 state=BEGIN")==int(cap::Domain::Quest));
    CK(cap::receiptDomain("V66.168 CAPTURE domain=PHYSICS session=1 state=BEGIN")==int(cap::Domain::Physics));
    std::printf("PASS R158 quest recorder: %d checks (gate, NPC table, reply parser, state row, Quest domain)\n",checks);
}
