#include "../src/subsystems/hud_ui_controls/runtime/prompt_family_d49.h"
#include <cstdio>
using kcd2vr::prompt_family_d49::Result;
int main(){
 unsigned checks=0,fail=0,writes=0;int value=1;bool available=true,allow=true;
 auto test=[&](bool b){++checks;if(!b)++fail;};
 auto read=[&](int& v){v=value;return available;};
 auto write=[&](int v){++writes;if(allow)value=v;return true;};
 auto sync=[&](int v){return kcd2vr::prompt_family_d49::reassert(v,read,write);};
 test(sync(2)==Result::Changed&&value==2&&writes==1);
 test(sync(2)==Result::Unchanged&&writes==1);
 test(sync(4)==Result::Changed&&value==4&&writes==2);
 value=1;allow=false;test(sync(2)==Result::Rejected&&value==1&&writes==3);
 available=false;test(sync(2)==Result::Unavailable&&writes==3);
 available=true;test(sync(1)==Result::Unavailable&&writes==3);
 auto recursiveWrite=[&](int v){++writes;value=v;test(sync(v)==Result::Unchanged);return true;};
 test(kcd2vr::prompt_family_d49::reassert(2,read,recursiveWrite)==Result::Changed&&writes==4);
 std::printf("D49 prompt native-CVar result policy %u/%u; native engine NOT executed\n",checks-fail,checks);
 return fail?1:0;
}
