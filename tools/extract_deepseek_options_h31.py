"""Exercise actual DeepSeek menu clicks/opening and the production pose clause."""
from pathlib import Path
import sys
from source_text import read_source

root = Path(__file__).resolve().parents[1]
menu = read_source(root / 'src/vr_menu.cpp')
adapter = read_source(root / 'src/subsystems/base/pose_physical_executor.inl')
rect_start = menu.index('constexpr UiRect kDsVignetteH29')
rect_end = menu.index('\n', menu.index('bool PtIn(', rect_start)) + 1
rects = menu[rect_start:rect_end]
toggle_start = menu.index('void VrMenu::ToggleDeepSeekLabH29()')
toggle_end = menu.index('void VrMenu::GoToPage(', toggle_start)
toggle = menu[toggle_start:toggle_end]
click_start = menu.index('    if(page==MenuPage::DeepSeekLabH29){', menu.index('void VrMenu::HandleClick('))
click_end = menu.index('\n    if(page==MenuPage::Home){', click_start)
click = menu[click_start:click_end]
clause = [line for line in adapter.splitlines() if line.startswith('        pi.wallBackoutSpringH31=')]
assert len(clause) == 1 and 'save()' not in click
assert 'Finger springs: contact and lifetime wiring.' not in menu
assert 'Free-hand finger contact response (experiment)' in menu
assert 'Free empty hands with precise contacts active.' in menu
assert 'ApplyUserMode175(' not in click and 'SetCombatCensusMode(' not in click

prefix = r'''
#include "subsystems/experimental/deepseek_options_h29.h"
#include "vrik/physical_weapon.h"
#include <atomic>
#include <cassert>
#include <cstdio>
namespace diagnostic_trace {unsigned calls=0;void Write(const char*){++calls;}}
namespace kcd2vr {
struct UiRect {int l,t,r,b;};
'''
declarations = r'''
class VrMenu {
public:
 enum class MenuPage {Home,DeepSeekLabH29};
 std::atomic<int> active_page_{int(MenuPage::Home)};
 bool visible=false;unsigned dirty=0;
 bool IsVisible()const{return visible;}
 void SetVisible(bool on){visible=on;}
 void GoToPage(MenuPage page,int){active_page_=int(page);}
 void MarkDirty(){++dirty;}
 void ToggleDeepSeekLabH29();
 void click(int x,int y){
  const auto page=MenuPage(active_page_.load());
'''
adapter_head = r'''
 }
};
namespace ph=physical;
void ActualAdapterH31(ph::Input& pi){
'''
tests = r'''
}
int main(){
 using namespace kcd2vr;
 namespace ds=kcd2vr::deepseek_h29;
 const auto allOff=[](){return !ds::angularVignette.load()&&!ds::wallBackoutSpringH31.load()&&!ds::fingerContactSpringH31.load();};
 assert(allOff());physical::Input input{};assert(!input.wallBackoutSpringH31);
 VrMenu menu;menu.ToggleDeepSeekLabH29();
 assert(menu.IsVisible()&&menu.active_page_.load()==int(VrMenu::MenuPage::DeepSeekLabH29));
 ActualAdapterH31(input);assert(!input.wallBackoutSpringH31&&allOff());
 // The new free-hand response has its own actual click branch and no mode API.
 menu.click(kDsFingerH31.l+1,kDsFingerH31.t+1);
 assert(ds::fingerContactSpringH31.load()&&!ds::angularVignette.load()&&!ds::wallBackoutSpringH31.load()&&menu.dirty==1);
 menu.click(kDsFingerH31.l+1,kDsFingerH31.t+1);assert(allOff()&&menu.dirty==2);
 menu.click(kDsWallH31.l+1,kDsWallH31.t+1);ActualAdapterH31(input);
 assert(input.wallBackoutSpringH31&&ds::wallBackoutSpringH31.load()&&!ds::angularVignette.load()&&!ds::fingerContactSpringH31.load());
 menu.click(kDsFingerH31.l+1,kDsFingerH31.t+1);
 assert(ds::fingerContactSpringH31.load()&&ds::wallBackoutSpringH31.load()&&!ds::angularVignette.load());
 menu.click(kDsVignetteH29.l+1,kDsVignetteH29.t+1);
 assert(ds::angularVignette.load()&&ds::fingerContactSpringH31.load()&&ds::wallBackoutSpringH31.load());
 menu.click(kDsWallH31.l+1,kDsWallH31.t+1);ActualAdapterH31(input);
 assert(!input.wallBackoutSpringH31&&!ds::wallBackoutSpringH31.load()&&ds::angularVignette.load()&&ds::fingerContactSpringH31.load());
 menu.click(kDsFingerH31.l+1,kDsFingerH31.t+1);
 assert(!ds::fingerContactSpringH31.load()&&ds::angularVignette.load()&&!ds::wallBackoutSpringH31.load());
 menu.click(kDsWallH31.l+1,kDsWallH31.t+1);menu.click(kDsFingerH31.l+1,kDsFingerH31.t+1);
 const auto dirty=menu.dirty;menu.click(1,1);assert(menu.dirty==dirty);
 menu.ToggleDeepSeekLabH29();assert(!menu.IsVisible()); // closing preserves all session choices
 menu.ToggleDeepSeekLabH29();assert(menu.IsVisible()&&ds::wallBackoutSpringH31.load()&&ds::fingerContactSpringH31.load()&&ds::angularVignette.load());
 menu.click(kDsOffH29.l+1,kDsOffH29.t+1);ActualAdapterH31(input);
 assert(allOff()&&!input.wallBackoutSpringH31);
 ds::angularVignette=true;ds::wallBackoutSpringH31=true;ds::fingerContactSpringH31=true;ds::DisableAll();assert(allOff());
 // Existing controls stay in their established positions; new row/help fit.
 assert(kDsVignetteH29.t==180&&kDsVignetteH29.b==222&&kDsOffH29.t==260&&kDsOffH29.b==300&&kDsWallH31.t==320&&kDsWallH31.b==358);
 assert(DeepSeekLayoutH31()&&kDsFingerH31.b<kDsFingerHelpH31.t&&kDsFingerHelpH31.b<kDsPendingH31.t&&kDsPendingH31.b<=672);
 const auto beforeBorder=menu.dirty;menu.click(kDsFingerH31.r,kDsFingerH31.t+1);menu.click(kDsFingerH31.l+1,kDsFingerH31.b);
 assert(allOff()&&menu.dirty==beforeBorder);
 menu.click(kDsFingerH31.r-1,kDsFingerH31.b-1);assert(ds::fingerContactSpringH31.load()&&!ds::wallBackoutSpringH31.load());
 menu.click(kDsOffH29.l+1,kDsOffH29.t+1);assert(allOff());
 std::puts("H31 actual DeepSeek menu: three independent launch-OFF session trials, real finger/wall/vignette clicks, ALL OFF, preserved positions/opening and wall adapter PASS; native_calls=NONE mode_writes=NONE");
}
'''
Path(sys.argv[1]).write_text(prefix + rects + declarations + click + adapter_head +
    clause[0] + '\n}\n' + toggle + tests, encoding='utf-8', newline='\n')
