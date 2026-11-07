// C49 first EXPERIMENTS page + GRAPHICS shadow row: the production layout constants and value mapping (src/combat_menu49.h).
#include "../src/combat_menu49.h"
#include "../src/immersion_menu45.h" // C45
#include <cassert>
#include <cmath>
#include <cstdio>
namespace m = kcd2vr::combat_menu49;
static bool near(float a, float b) { return std::fabs(a - b) < 1e-4f; }
int main() {
    unsigned checks = 0; const auto ck = [&](bool ok) { ++checks; assert(ok); };
    // every item of the first page with its drawn extent (a slider label sits 25 px above the bar)
    struct Item { m::Rect r; bool slider; };
    const Item page[] = {{m::kProximity, true}, {m::kBlockSpeed, true}, {m::kWeight, true}, {m::kSkill, true},
                         {m::kInstantFree, false}, {m::kImpaleKill, false}, {m::kDecapKill, false}, {m::kDefaults54, false}, {m::kWindowsH41, false}, {m::kShowBlockH24, false}, {m::kNote, false}, {m::kMore, false}, {m::kFlow, false}};
    const unsigned n = sizeof(page) / sizeof(page[0]);
    ck(n == m::kCount && m::clean());             // the header's own list (the vr_menu.cpp static_assert) covers the same rects
    for (unsigned i = 0; i < n; ++i) {
        const auto e = m::extent(page[i].r, page[i].slider);
        ck(e.l < e.r && e.t < e.b);
        ck(m::inside(e, m::kClip));                 // nothing clipped by the right-page clip (y <= 672)
        ck(e.t >= m::kHeader.b);                    // nothing under the title / BACK / VIEW-UI-CONTROLS row
        for (unsigned j = i + 1; j < n; ++j) ck(!m::overlap(e, m::extent(page[j].r, page[j].slider)));
    }
    // the slider bars are wide enough for the 80 % track + value badge used by DrawSlider (r.l >= 590)
    for (unsigned i = 0; i < 4; ++i) ck(page[i].r.l >= 590 && page[i].r.r - page[i].r.l >= 400);
    // GRAPHICS row: 3 shadow steps + brightness, no overlap, same row as before (234..256), between SAVE (196..230) and XR (260..290)
    const m::Rect row[] = {m::kGfxShadow[0], m::kGfxShadow[1], m::kGfxShadow[2], m::kGfxEye};
    const m::Rect resApply{700, 196, 960, 230}, xrPrev{620, 260, 650, 290}, xrNext{1008, 260, 1038, 290}, xrApply{874, 260, 998, 290};
    for (unsigned i = 0; i < 4; ++i) {
        ck(row[i].t == 234 && row[i].b == 256 && m::inside(row[i], m::kClip));
        ck(!m::overlap(row[i], resApply) && !m::overlap(row[i], xrPrev) && !m::overlap(row[i], xrNext) && !m::overlap(row[i], xrApply));
        for (unsigned j = i + 1; j < 4; ++j) ck(!m::overlap(row[i], row[j]) && row[i].r <= row[j].l);
    }
    ck(m::kGfxShadow[0].l == 620 && m::kGfxEye.r == 1038);
    // 1) proximity: 0.20..0.60 m, 1 cm steps, round trip
    ck(near(m::proximityFromT(0.f), .20f) && near(m::proximityFromT(1.f), .60f) && near(m::proximityFromT(.5f), .40f));
    ck(near(m::proximityFromT(-3.f), .20f) && near(m::proximityFromT(NAN), .20f) && near(m::proximityT(.4f), .5f));
    // 2) NPC block speed: one slider -> C39 Low/High; 3.0 = the C39 defaults 2.0 / 3.0; 1.0 = native (by-skill off)
    {const auto b = m::blockFromSlider(3.f); ck(b.bySkill && near(b.lo, 2.f) && near(b.hi, 3.f));}
    {const auto b = m::blockFromSlider(1.f); ck(!b.bySkill && near(b.lo, 1.f) && near(b.hi, 1.f));}
    {const auto b = m::blockFromSlider(2.f); ck(b.bySkill && near(b.lo, 1.5f) && near(b.hi, 2.f));}
    {const auto b = m::blockFromSlider(9.f); ck(b.bySkill && near(b.hi, 3.f) && near(b.lo, 2.f));}
    {const auto b = m::blockFromSlider(NAN); ck(!b.bySkill);}
    for (int k = 0; k <= 20; ++k) { const float s = m::blockFromT(k / 20.f); const auto b = m::blockFromSlider(s);
        ck(b.lo >= 1.f && b.lo <= b.hi && b.hi <= 3.f && near(m::blockSlider(b.bySkill, b.hi), b.bySkill ? b.hi : 1.f)); }
    ck(near(m::blockT(true, 3.f), 1.f) && near(m::blockT(false, 3.f), 0.f) && near(m::blockT(true, 2.f), .5f));
    // 3) weight: the WEAPON FEEL range 0.20..2.00 in 0.05 steps
    ck(near(m::weightFromT(0.f), .2f) && near(m::weightFromT(1.f), 2.f) && near(m::weightFromT(m::weightT(.6f)), .6f) && near(m::weightFromT(m::weightT(.95f)), .95f));
    // 4) skill: 4 stops OFF/LOW/MEDIUM/HIGH, round trip
    for (int s = 0; s <= 3; ++s) ck(m::skillFromT(m::skillT(s)) == s);
    ck(m::skillFromT(.16f) == 0 && m::skillFromT(.17f) == 1 && m::skillFromT(.6f) == 2 && m::skillFromT(.9f) == 3 && m::skillFromT(5.f) == 3);
    // 5) C45 kill-move row: half width each, same row, between FREE SWING and the note
    ck(m::kImpaleKill.l == 620 && m::kImpaleKill.r == 824 && m::kDecapKill.l == 834 && m::kDecapKill.r == 1038);
    ck(m::kImpaleKill.t == m::kDecapKill.t && m::kImpaleKill.b == m::kDecapKill.b && m::kImpaleKill.t >= m::kInstantFree.b && m::kImpaleKill.b <= m::kNote.t);
    ck(m::kFlow.b <= m::kClip.b && m::kMore.b <= m::kFlow.t);
    // IMPALE ON KILL STAB toggle over VrImpaleHang: ON -> OFF keeps nothing here (the mod remembers lastOn), OFF -> ON restores lastOn, 2 when unknown
    for (int h = 1; h <= 3; ++h) ck(m::impaleToggle(h, h) == 0);
    ck(m::impaleToggle(0, 1) == 1 && m::impaleToggle(0, 2) == 2 && m::impaleToggle(0, 3) == 3 && m::impaleToggle(0, 0) == 2 && m::impaleToggle(0, 9) == 2 && m::impaleToggle(0, -1) == 2);
    // DECAPITATION cycle OFF -> HIDE HEAD -> LOOSE NECK -> OFF, the IMMERSION state pair as one value
    ck(m::decapState(false, 1) == 0 && m::decapState(true, 0) == 1 && m::decapState(true, 1) == 2 && m::decapState(true, 2) == 3);
    ck(m::decapNext(0) == 1 && m::decapNext(1) == 2 && m::decapNext(2) == 3 && m::decapNext(3) == 0);
    // 6) C45 IMMERSION page grid: no overlap, inside the clip, under the header; the new MINIGAME HANDS row is full width
    namespace im = kcd2vr::immersion_menu45;
    ck(im::clean());
    for (unsigned i = 0; i < im::kCount; ++i) {
        const auto e = m::extent(im::kPage[i].r, im::kPage[i].slider);
        ck(e.l < e.r && e.t < e.b && m::inside(e, m::kClip) && e.t >= m::kHeader.b);
        for (unsigned j = i + 1; j < im::kCount; ++j) ck(!m::overlap(e, m::extent(im::kPage[j].r, im::kPage[j].slider)));
    }
    ck(im::kWeaponRelease.l == 620 && im::kWeaponRelease.r == 1038 && im::kWeaponRelease.t >= im::kMinigameHands.b && im::kWeaponRelease.b <= im::kSmoothDialogue.t);
    ck(!m::overlap(im::kDialogueUiScale, im::kPreciseContacts) && !m::overlap(m::extent(im::kDialogueUiScale, true), im::kTargetDwell)); // the C44 collision is gone
    ck(!m::overlap(m::extent(im::kTargetDwell, true), im::kAutoPalm) && !m::overlap(m::extent(im::kWeaponGrip, true), im::kDecapitation));
    for (unsigned i = 0; i < im::kCount; ++i) if (im::kPage[i].slider) ck(im::kPage[i].r.l >= 590 && im::kPage[i].r.r - im::kPage[i].r.l >= 200);
    // C49 BLOODY FIGHTS: right half of the PRECISE CONTACTS row, same height, in the page list, touching nothing
    ck(im::kBloodyFights.t == im::kPreciseContacts.t && im::kBloodyFights.b == im::kPreciseContacts.b && im::kPreciseContacts.r < im::kBloodyFights.l);
    ck(im::kPreciseContacts.l == 620 && im::kBloodyFights.r == 1038 && im::kBloodyFights.r - im::kBloodyFights.l >= 200 && im::kPreciseContacts.r - im::kPreciseContacts.l >= 200);
    { bool listed = false; for (unsigned i = 0; i < im::kCount; ++i) if (im::kPage[i].r.l == im::kBloodyFights.l && im::kPage[i].r.t == im::kBloodyFights.t && !im::kPage[i].slider) listed = true; ck(listed); }
    ck(!m::overlap(im::kBloodyFights, im::kBowReload) && !m::overlap(im::kBloodyFights, im::kRagdoll) && !m::overlap(im::kBloodyFights, im::kPreciseContacts));
    std::printf("combat_menu49: %u layout/mapping checks PASS\n", checks);
}
