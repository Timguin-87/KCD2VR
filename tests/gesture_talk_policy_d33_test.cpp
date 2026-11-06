// Optional pointing preference stays OFF by default and honors explicit saved ON; headset glyphs unchanged.
#include "vrik/gesture_talk_policy_d33.h"
#include <cstdio>
int main(){
    namespace g=kcd2vr::gesture_talk_policy_d33;unsigned checks=0,fails=0;
    auto check=[&](bool ok,const char* what){++checks;if(!ok){++fails;std::printf("FAIL %s\n",what);}};
    check(!g::loadTalk(0,false),"fresh INI: gesture talk off");
    check(g::loadTalk(0,true),"explicit saved ON without a version is honored");
    check(!g::loadTalk(1,false),"after migration: saved off stays off");
    check(g::loadTalk(1,true),"after migration: explicit ON from the menu stays on");
    check(g::loadTalk(2,true),"later version keeps explicit ON");
    check(g::glyphForHeadset(L"psvr2")==g::kGlyphPlayStation,"PSVR2 -> PlayStation glyphs");
    check(g::glyphForHeadset(L"quest3")==g::kGlyphXbox,"Quest 3 -> Xbox glyphs");
    check(g::glyphForHeadset(L"steamframe")==g::kGlyphXbox,"Steam Frame -> Xbox");
    check(g::glyphForHeadset(L"PSVR2")==g::kGlyphXbox,"ids are the setup's lower-case ids");
    check(g::glyphForHeadset(nullptr)==g::kGlyphXbox,"no id -> Xbox");
    check(g::kGlyphXbox==2&&g::kGlyphPlayStation==4,"values match VrPadGlyphStyle 2/4");
    std::printf("gesture_talk_policy_d33 %u checks, %u failures\n",checks,fails);return fails?1:0;
}
