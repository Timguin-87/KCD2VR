// W5: F9 physics lab rules - diagnostics rungs, corpse grab refusal text (pure, host).
#include "vrik/lab58.h"
#include <cstdio>
#include <cstring>
static int total=0,failed=0;
static void check(bool v,const char* n){++total;if(!v)++failed;std::printf("%s %s\n",v?"PASS":"FAIL",n);}
int main(){
    namespace L=kcd2vr::lab58;
    check(!L::physicsDiagnostics(0)&&L::physicsDiagnostics(1)&&!L::physicsDiagnostics(2)&&L::physicsDiagnostics(3),"F9 rung 1 and 3 are physics diagnostics, 0/2 are not");
    L::f9Rung=1;check(L::physicsDiagnostics(),"mirror read");L::f9Rung=0;check(!L::physicsDiagnostics(),"mirror off");
    char t[96];L::GrabWhyInput g{};
    g.qualified=0;g.unqualified=0;check(std::strcmp(L::grabWhy(g,t,sizeof t),"corpse grab: no body the game killed within reach")==0,"no corpse");
    g.unqualified=2;g.why="NOT_DEAD_BY_GAME";check(std::strstr(L::grabWhy(g,t,sizeof t),"body not ready - NOT_DEAD_BY_GAME")!=nullptr,"unqualified corpse names the proof");
    g.qualified=1;g.nearM=.41;check(std::strcmp(L::grabWhy(g,t,sizeof t),"corpse grab: hand 41 cm from the body, needs 25 cm")==0,"out of reach in cm");
    g.nearM=.2;g.consumed=false;check(std::strcmp(L::grabWhy(g,t,sizeof t),"corpse grab: open the hand near the body, then squeeze")==0,"squeeze not consumed");
    g.consumed=true;check(t[0]=='c'&&L::grabWhy(g,t,sizeof t)[0]==0,"nothing to say when it can grab");
    g.handEmpty=false;check(std::strcmp(L::grabWhy(g,t,sizeof t),"corpse grab: that hand is not empty")==0,"hand not empty first");
    g.fault=true;check(std::strstr(L::grabWhy(g,t,sizeof t),"fault")!=nullptr,"fault first");
    L::GrabWhyInput q{};q.unqualified=1;q.why="BAD\"QUOTE";L::grabWhy(q,t,sizeof t);check(std::strchr(t,'"')==nullptr&&std::strlen(t)<96,"visible text has no quotes and fits");
    char tiny[8];L::grabWhy(g,tiny,sizeof tiny);check(std::strlen(tiny)==7,"small buffer truncates safely");
    std::printf("physics_lab_w5 %d/%d\n",total-failed,total);return failed?1:0;
}
