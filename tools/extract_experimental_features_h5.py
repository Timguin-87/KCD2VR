"""Actual menu clauses for the isolated host/GDI UI fixture, not a UI rewrite."""
from pathlib import Path
import hashlib,json,sys
root=Path(__file__).resolve().parents[1];out=Path(sys.argv[1]);out.mkdir(parents=True,exist_ok=True)
menu=(root/'src/vr_menu.cpp').read_text(encoding='utf-8-sig')
header=(root/'src/vr_menu.h').read_text(encoding='utf-8-sig')
def block(text,start):
    begin=text.index('{',start);depth=0;state='code';i=begin
    while i<len(text):
        c=text[i];n=text[i+1] if i+1<len(text) else ''
        if state=='line':
            if c=='\n':state='code'
        elif state=='comment':
            if c=='*' and n=='/':state='code';i+=1
        elif state in ['"',"'"]:
            if c=='\\':i+=1
            elif c==state:state='code'
        elif c=='/' and n=='/':state='line';i+=1
        elif c=='/' and n=='*':state='comment';i+=1
        elif c in ['"',"'"]:state=c
        elif c=='{':depth+=1
        elif c=='}':
            depth-=1
            if not depth:return text[start:i+1],text[begin+1:i]
        i+=1
    raise ValueError('unterminated source clause')
def function(name):return block(menu,menu.index(name))[0]
parts={}
start=header.index('enum class MenuPage : int')
parts['enum']=header[start:header.index(';',start)+1]
start=menu.index('constexpr UiRect kHomeModeMono')
end=menu.index('\n',menu.index('static_assert(HomeNavigationH5()',start))+1
parts['rects']=menu[start:end]
start=menu.index('constexpr UiRect kDsVignetteH29');end=menu.index('\n',menu.index('bool PtIn(',start))+1
parts['rects']=menu[menu.index('constexpr UiRect EH5'):start]+menu[start:end]+parts['rects']
parts['rects']+=function('constexpr UiRect U49(')+'\n'
for name in ['kPageBack','kUtilityRecenter','kRangedBow32','kV149UserHolsters']:
    start=menu.index('constexpr UiRect '+name);end=menu.index(';',start)+1;parts['rects']+=menu[start:end]+'\n'
parts['methods']='\n'.join(function('void VrMenu::'+name+'(').replace('VrMenu::','MenuHarness::') for name in ['GoToPage','UpdatePageAnimation','CyclePage','ToggleDeepSeekLabH29'])
_,handle=block(menu,menu.index('void VrMenu::HandleClick('))
prefix=handle[:handle.index('    if(page==MenuPage::ClaudeExperimentsH8')]
back='    if(page!=MenuPage::Home && PtIn(kPageBack,x,y)){ GoToPage(MenuPage::Home,-1); return; }'
assert back in handle
live_back=block(handle,handle.index('    if((page==MenuPage::ExperimentalFeaturesH5'))[0]
parts['click']=prefix+'\n'+live_back+'\n'+back+'\n'
for page in ['RangedOptions32','LiveExperimentsD39','ExperimentalFeaturesH5','DeepSeekLabH29','Home']:
    start=handle.index('    if(page==MenuPage::'+page+'){');parts['click']+=block(handle,start)[0]+'\n'
hub=block(menu,menu.index('} else if(page==MenuPage::ExperimentalFeaturesH5){')+2)[1]
assert 'ApplyState' not in hub and 'save(' not in hub
parts['draw']=hub
parts['live_draw']=block(menu,menu.index('} else if(page==MenuPage::LiveExperimentsD39){')+2)[1]
parts['ranged_draw']=block(menu,menu.index('} else if(page==MenuPage::RangedOptions32){')+2)[1]
draw_start=menu.index('void VrMenu::DrawPanel(')
parts['title']=block(menu,menu.index('    auto pageTitle=',draw_start))[0]+';'
start=menu.index('    if(page==MenuPage::Home){',draw_start);parts['home_left_draw']=block(menu,start)[1]
start=menu.index('    if(page==MenuPage::Home){',menu.index('wchar_t v[128]{};',start));parts['home_draw']=block(menu,start)[1]
helpers='\n'.join(function(name) for name in ['void Fill(','void Line(','void Text(','void DrawCheck(','void DrawButton('])
# Retain real GDI Text; the fixture observer additionally measures its original
# rectangle/font before forwarding to this exact function.
parts['render']=helpers.replace('void Text(','void NativeTextH5(',1).replace('void DrawCheck(','void NativeDrawCheckH5(',1).replace('void DrawButton(','void NativeDrawButtonH5(',1)
start=menu.index('    font_normal_ = CreateFontW');end=menu.index(';',menu.index('    font_hero_ = CreateFontW',start))+1
parts['fonts']=menu[start:end]
assert 'deepseek_h29::DisableAll()' in parts['click']
assert 'experimentalGroupH5_.store(i)' in parts['click']
assert parts['methods'].index('MenuPage::ExperimentalFeaturesH5')<parts['methods'].index('MenuPage::Experiments,')
for key,text in parts.items():(out/f'experimental_menu_h5_{key}.inc').write_text(text+'\n',encoding='utf-8')
inputs=[root/'src/vr_menu.cpp',root/'src/vr_menu.h',root/'src/subsystems/experimental/features_menu_h5.h',root/'src/subsystems/experimental/live_settings_d39.h',root/'src/subsystems/experimental/deepseek_options_h29.h',Path(__file__).resolve()]
(out/'experimental_menu_h5_provenance.json').write_text(json.dumps({'source_sha256':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in inputs},'scope':'Actual enum, Home targets, navigation/opening, selected HandleClick branches, Home/experimental draw clauses, font creation and GDI helpers. Native read/apply/save callbacks are explicit host leaves; full D3D renderer/input pump not executed.'},indent=2)+'\n',encoding='utf-8')
print('H5: actual menu navigation/click/draw/GDI clauses regenerated')
