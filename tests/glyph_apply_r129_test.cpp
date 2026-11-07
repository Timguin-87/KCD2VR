// R129: PlayStation button art follows the VR menu choice right away, after a save load and after a restart,
// even when the wh_ui_PreferredInputDevice CVar lease is not available (mod start, console not up yet).
// Drives the same policy header host.inl (ApplyGlyphStyleR129 / ResendGlyphStyleOnUiChangeR129) and
// prompt_device_lock.cpp (ReportGlyphR129) use; the movie/VM side is a stub that records what it was sent.
#include <cstdio>
#include <cstdint>
#include "subsystems/hud_ui_controls/runtime/glyph_apply_r129.h"
namespace ga=kcd2vr::glyph_apply_r129;
static int fails=0,checks=0;
#define CHECK(c) do{++checks;if(!(c)){++fails;std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);}}while(0)
struct Game {               // stub: console, movies, log
    bool console=false;      // CVar reachable
    bool lease=false;        // g_f7_prompt_active_d33
    int queued=0;            // last queued art request (0 none)
    int applied=0;           // AppliedGlyphStyleR129
    int movieArt=2;          // what the HUD movie shows (a new movie starts with Xbox)
    int rows=0;ga::Report report;int lastRowStyle=0,lastRowOk=-1;
    bool tryLease(){if(!lease&&console)lease=true;return lease;}
    void dispatch(bool movieAlive){   // FlashServiceD56 at terminal
        if(!queued)return;const int s=queued;queued=0;
        applied=movieAlive?s:0;if(movieAlive)movieArt=s;
        if(report.take(s,movieAlive)){++rows;lastRowStyle=s;lastRowOk=movieAlive?1:0;}
    }
    void apply(int style){            // ApplyGlyphStyleR129
        const auto p=ga::plan(lease,style,applied);
        if(p.tryLease)tryLease();
        if(p.queueArt)queued=ga::style(style);
    }
    void uiEdge(int saved){if(!lease)tryLease();queued=ga::style(saved);} // ResendGlyphStyleOnUiChangeR129
    void hudRecreated(){movieArt=2;applied=0;}
};
int main(){
    // 1. restart with VrPadGlyphStyle=4, CVar missing at start: art goes out anyway (old code returned before it).
    {Game g;g.apply(4);CHECK(g.queued==4);CHECK(!g.lease);g.dispatch(true);CHECK(g.movieArt==4);CHECK(g.rows==1&&g.lastRowStyle==4&&g.lastRowOk==1);}
    // 2. VR menu click PLAYSTATION while the lease is still missing: queued at once, lease retried on the click.
    {Game g;g.apply(2);g.dispatch(true);CHECK(g.movieArt==2);g.console=true;g.apply(4);CHECK(g.queued==4&&g.lease);g.dispatch(true);CHECK(g.movieArt==4&&g.rows==2);}
    // 3. save load / HUD recreated: the edge re-sends the saved art WITHOUT the lease (old edge needed it).
    {Game g;g.apply(4);g.dispatch(true);g.hudRecreated();CHECK(g.movieArt==2);ga::Edge e;CHECK(e.changed(1,0,0));g.uiEdge(4);g.dispatch(true);CHECK(g.movieArt==4);
     CHECK(g.rows==1);   // same style applied again = no second log row
     CHECK(!e.changed(1,0,0));CHECK(e.changed(2,0,0));CHECK(e.changed(2,1,0));CHECK(e.changed(2,1,8));CHECK(!e.changed(2,1,8));}
    // 4. first Run-loop pass is an edge (statics ~0) = start-up retry even before any HUD change.
    {ga::Edge e;CHECK(e.changed(0,0,0));}
    // 5. unrelated menu clicks with the art confirmed: nothing re-queued, no row spam. A failed dispatch is retried.
    {Game g;g.console=true;g.apply(4);g.dispatch(true);g.apply(4);CHECK(g.queued==0);g.apply(4);CHECK(g.queued==0&&g.rows==1);
     g.apply(2);g.dispatch(false);CHECK(g.applied==0&&g.rows==2&&g.lastRowOk==0); // movie missing: one failure row
     g.apply(2);CHECK(g.queued==2);g.dispatch(true);CHECK(g.movieArt==2&&g.rows==3&&g.lastRowOk==1);} // no latch: next click works
    // 6. plan details: lease held + style change keeps preferred=2; lease missing never blocks the art.
    {auto p=ga::plan(true,4,2);CHECK(p.queueArt&&!p.tryLease&&p.setPreferred);
     p=ga::plan(false,4,4);CHECK(!p.queueArt&&p.tryLease&&!p.setPreferred);
     p=ga::plan(false,4,0);CHECK(p.queueArt&&p.tryLease);
     p=ga::plan(true,7,2);CHECK(!p.queueArt); // anything not 4 is Xbox
     CHECK(ga::style(4)==4&&ga::style(2)==2&&ga::style(0)==2&&ga::style(16)==2);}
    // 7. Report: one row per (style, applied) change.
    {ga::Report r;CHECK(r.take(4,true));CHECK(!r.take(4,true));CHECK(r.take(4,false));CHECK(!r.take(4,false));CHECK(r.take(4,true));CHECK(r.take(2,true));}
    std::printf("glyph_apply_r129 %d/%d %s\n",checks-fails,checks,fails?"FAIL":"PASS");return fails?1:0;
}
