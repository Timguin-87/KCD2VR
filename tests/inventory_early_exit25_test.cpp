// C25: Inventory Theater early exit gate (src/ui_inventory_early_exit25.h). Host-only state machine.
#include "../src/ui_inventory_early_exit25.h"
#include <cstdio>
#include <initializer_list>
using namespace kcd2vr::ui_inventory_early_exit25;
static int failures = 0;
#define CHECK(c) do { if (!(c)) { std::printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); ++failures; } } while (0)
static Input frame(std::uint64_t t, bool modal, bool roots = true, bool press = true, bool owner = true) {
    Input i; i.nowMs = t; i.inventoryOwner = owner; i.modalVisible = modal; i.modalKnownClosed = !modal; i.rootsVisible = roots; i.closePressRecent = press;
    i.primaryRootVisible=modal;i.primaryRootKnownClosed=!modal;return i;
}
int main() {
    { // B closes a child modal, not the real inventory root: no early gameplay.
        Gate g;g.Update(frame(0,true));auto child=frame(10,false);
        child.primaryRootVisible=true;child.primaryRootKnownClosed=false;
        CHECK(g.Update(child)==Edge::None);CHECK(!g.Active());
        child.nowMs=30;child.primaryRootVisible=false; // UNKNOWN is not closed
        CHECK(g.Update(child)==Edge::None);CHECK(!g.Active());
    }
    { // Codex/quest/modal-only context never showed InventoryList.
        Gate g;auto a=frame(0,true);a.primaryRootVisible=false;g.Update(a);
        CHECK(g.Update(frame(10,false))==Edge::None);CHECK(!g.Active());
    }
    { // Switching inventory tabs/owners is not whole-UI closure, even after B.
        for(bool uncertain:{false,true}) {
            Gate g;g.Update(frame(0,true));auto a=frame(10,false);
            a.otherPrimaryRootVisible=!uncertain;a.uiUncertain=uncertain;
            CHECK(g.Update(a)==Edge::None);CHECK(!g.Active());
        }
    }
    { // Modal hides first; only the later known InventoryList close can release.
        Gate g;g.Update(frame(0,true));auto a=frame(10,false);
        a.primaryRootVisible=true;a.primaryRootKnownClosed=false;
        CHECK(g.Update(a)==Edge::None);CHECK(!g.Active());
        CHECK(g.Update(frame(30,false))==Edge::Begin);CHECK(g.Active());
    }
    { // close: press, modal drops, panels linger, owner releases -> Begin then Complete
        Gate g; CHECK(g.Update(frame(0, true)) == Edge::None);
        CHECK(g.Update(frame(10, false)) == Edge::Begin); CHECK(g.Active());
        CHECK(g.Update(frame(300, false)) == Edge::None); CHECK(g.Active());
        CHECK(g.Update(frame(500, false, false, true, false)) == Edge::Complete); CHECK(!g.Active());
    }
    { // modal drop without a close press (e.g. dialog answered with A) -> no early exit
        Gate g; g.Update(frame(0, true, true, false));
        CHECK(g.Update(frame(10, false, true, false)) == Edge::None); CHECK(!g.Active());
    }
    { // modal returns (B closed only a confirmation that reopens) -> cancel, Theater restored
        Gate g; g.Update(frame(0, true)); CHECK(g.Update(frame(10, false)) == Edge::Begin);
        CHECK(g.Update(frame(200, true)) == Edge::CancelModalReturned); CHECK(!g.Active());
    }
    { // A positively re-opened InventoryList restores Theater on this exact tick.
        for(bool modalKnown:{false,true}) {
            Gate g;g.Update(frame(0,true));CHECK(g.Update(frame(10,false))==Edge::Begin);
            auto reopened=frame(200,false);reopened.primaryRootVisible=true;
            reopened.primaryRootKnownClosed=false;reopened.modalKnownClosed=modalKnown;
            CHECK(g.Update(reopened)!=Edge::None);CHECK(!g.Active());
            reopened.nowMs=210;CHECK(g.Update(reopened)==Edge::None);CHECK(!g.Active());
            // The re-open does not retire genuine fast close on its next close edge.
            CHECK(g.Update(frame(230,false))==Edge::Begin);CHECK(g.Active());
        }
    }
    { // Positive visibility wins even if an inconsistent old known-closed bit remains.
        Gate g;g.Update(frame(0,true));auto visible=frame(10,false);
        visible.primaryRootVisible=true;
        CHECK(g.Update(visible)==Edge::None);CHECK(!g.Active());
    }
    { // inventory still up after 1 s -> cancel; no re-arm without a new modal drop
        Gate g; g.Update(frame(0, true)); CHECK(g.Update(frame(10, false)) == Edge::Begin);
        CHECK(g.Update(frame(1010, false)) == Edge::None); CHECK(g.Active());
        CHECK(g.Update(frame(1011, false)) == Edge::CancelTimeout); CHECK(!g.Active());
        CHECK(g.Update(frame(1500, false)) == Edge::None); CHECK(!g.Active());
    }
    { // observer loss (modal not known) is not a close
        Gate g; g.Update(frame(0, true)); Input i = frame(10, false); i.modalKnownClosed = false;
        CHECK(g.Update(i) == Edge::None); CHECK(!g.Active());
    }
    { // roots already gone (owner about to release on its own) -> nothing to shorten
        Gate g; g.Update(frame(0, true)); CHECK(g.Update(frame(10, false, false)) == Edge::None);
    }
    { // disabled by INI, or not the Inventory owner (Shop/Map/other) -> never active
        Gate g; Input a = frame(0, true); a.enabled = false; g.Update(a); Input b = frame(10, false); b.enabled = false;
        CHECK(g.Update(b) == Edge::None); CHECK(!g.Active());
        Gate s; s.Update(frame(0, true, true, true, false)); CHECK(s.Update(frame(10, false, true, true, false)) == Edge::None); CHECK(!s.Active());
    }
    std::printf(failures ? "inventory_early_exit25: %d FAILURES\n" : "inventory_early_exit25: all passed\n", failures);
    return failures ? 1 : 0;
}
