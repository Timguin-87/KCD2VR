#include <initializer_list>
#include "../src/diagnostics/geometry_scope40c1.h"
#include <thread>
#include <cstdio>
#include <cstdlib>
namespace g=kcd2vr::geometry40c1;
int main(){int checks=0;auto ck=[&](bool b){++checks;if(!b){std::printf("FAIL geometry_scope40c1 check=%d\n",checks);std::exit(1);}};
 for(int kind:{0,1}){
  ck(g::rejectAuthored51c3(0xC52534,kind,true,true,false)); // no ticket / tracking qualification
  ck(!g::rejectAuthored51c3(0xC52534,kind,false,true,false));
  ck(!g::rejectAuthored51c3(0xC52534,kind,true,false,false));
  ck(!g::rejectAuthored51c3(0xC52534,kind,true,true,true));
 }
 for(int kind:{-1,2,4,9})ck(!g::rejectAuthored51c3(0xC52534,kind,true,true,false));
 ck(!g::rejectAuthored51c3(0xD538D2,1,true,true,false));
 int a=0,d=0,t=0,other=0;
 ck(!g::Scope::matches(&a,&d,&t));
 {g::Scope s(&a,&d,&t);ck(g::Scope::matches(&a,&d,&t));
  ck(!g::Scope::matches(&a,&other,&t));ck(!g::Scope::matches(&other,&d,&t));ck(!g::Scope::matches(&a,&d,&other));
  bool leaked=true;std::thread x([&]{leaked=g::Scope::matches(&a,&d,&t);});x.join();ck(!leaked);
  {g::Scope nested(&other,&d,&t);ck(g::Scope::matches(&other,&d,&t));ck(!g::Scope::matches(&a,&d,&t));}
  ck(g::Scope::matches(&a,&d,&t));
 }ck(!g::Scope::matches(&a,&d,&t));
 for(int type:{0,1})ck(g::ownsOrdinary(0xC52534,type,true,true,true));
 for(int type:{-1,2,4,9})ck(!g::ownsOrdinary(0xC52534,type,true,true,true));
 ck(!g::ownsOrdinary(0xD538D2,1,true,true,true));
 ck(!g::ownsOrdinary(0xC52534,1,false,true,true));
 ck(!g::ownsOrdinary(0xC52534,1,true,false,true));
 ck(!g::ownsOrdinary(0xC52534,1,true,true,false));
 std::printf("PASS geometry_scope40c1 %d checks; native execution NONE\n",checks);
}
