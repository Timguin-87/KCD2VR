#pragma once
static int diagnosticD87(){
    namespace d=kcd2vr::reload_diagnostics_d48;namespace cap=kcd2vr::diagnostics::capture168;
    const auto close=[](std::uint64_t now){for(auto domain:{cap::Domain::Interaction,cap::Domain::Vrik,cap::Domain::Reload})cap::gate.select(domain,false,now);};
    const auto drain=[](std::uint64_t now){std::vector<std::string> rows;
        d::drain(now,[&](cap::Domain,std::uint64_t,const char* row){if(std::strstr(row,"D87 RELOAD_NATIVE_DECISION"))rows.emplace_back(row);});return rows;};
    close(1000);d::Event ignored{};while(d::pop(ignored)){}
    {Fixture a;a.source.classId=10;a.spatial.toolBound=false;RetailScopeD82 profile(a,10);start82(a);
        a.put(a.p(a.scope)+0x60,.1f);a.traceReadsD69=true;a.readsD69.clear();x::Clock clock{};
        auto before=d::next.load();C87(!XR34NativeClock(g_crossbow_xr34.status().owner,g_crossbow_xr34.clip(),clock));
        const auto offReads=a.readsD69;C87(d::next.load()==before);
        for(auto domain:{cap::Domain::Interaction,cap::Domain::Vrik,cap::Domain::Reload})cap::gate.select(domain,true,a.now);
        a.readsD69.clear();C87(!XR34NativeClock(g_crossbow_xr34.status().owner,g_crossbow_xr34.clip(),clock));
        C87(a.readsD69==offReads); // recording makes no extra native reads
        for(int n=0;n<5;++n)(void)XR34NativeClock(g_crossbow_xr34.status().owner,g_crossbow_xr34.clip(),clock);
        auto rows=drain(a.now);C87(rows.size()==1);
        C87(rows[0].find("result=CLOCK_PENDING_BLEND")!=std::string::npos&&rows[0].find("scopeLayers=2")!=std::string::npos);
        C87(rows[0].find("recorder=INTERACTION captureMask=0x2A")!=std::string::npos);
        close(a.now);before=d::next.load();(void)XR34NativeClock(g_crossbow_xr34.status().owner,g_crossbow_xr34.clip(),clock);
        C87(d::next.load()==before);a.put(a.p(a.scope)+0x60,0.f);XR34BeforeStopNative(a.parent.data());
    }
    {Fixture a;a.source.classId=10;a.spatial.toolBound=false;RetailScopeD82 profile(a,10);start82(a,false,true);
        std::memcpy(reinterpret_cast<void*>(a.entry()+0x78),reinterpret_cast<void*>(a.entry()),0x78);
        a.put(a.entry()-4,2u);a.put(a.entry()+0x3C,1.f);a.put(a.entry()+0x44,1.f);
        a.put(a.entry()+0x50,std::uint16_t(0x101));a.put(a.entry()+0x78+0x50,std::uint16_t(1));
        cap::gate.select(cap::Domain::Reload,true,a.now);x::Clock clock{};
        C87(!XR34NativeClock(g_crossbow_xr34.status().owner,g_crossbow_xr34.clip(),clock));
        auto rows=drain(a.now);C87(rows.size()==1);
        C87(rows[0].find("result=CLOCK_DUPLICATE_NOT_INERT")!=std::string::npos&&rows[0].find("scopeLayers=2")!=std::string::npos);
        C87(rows[0].find("firstKnown=0x7F")!=std::string::npos&&rows[0].find("firstDynamic=0x101")!=std::string::npos);
        a.put(a.entry()+0x50,std::uint16_t(1));C87(XR34NativeClock(g_crossbow_xr34.status().owner,g_crossbow_xr34.clip(),clock));
        rows=drain(a.now);C87(rows.size()==1&&rows[0].find("result=CLOCK_INERT_DUPLICATE_QUALIFIED")!=std::string::npos);
        close(a.now);XR34BeforeStopNative(a.parent.data());
    }
    {Fixture a;a.source.classId=10;a.spatial.toolBound=false;RetailScopeD82 profile(a,10);start82(a);
        auto info=scan82(a);a.blocks.emplace_back(reinterpret_cast<std::uintptr_t>(&info),sizeof(info));
        cap::gate.select(cap::Domain::Interaction,true,a.now);
        {XR34ScanNative scan(a.character.data()+0x110,false,0,.9f,&info,0);C87(!scan.current);}
        a.blocks.pop_back();auto rows=drain(a.now);C87(rows.size()==1);
        C87(rows[0].find("result=INTERVAL_LIMIT")!=std::string::npos&&rows[0].find("to=0.9000000")!=std::string::npos&&rows[0].find("permitted=0.0000000")!=std::string::npos);
        C87(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f&&a.originalEvents==0);
        close(a.now);
    }
    {Fixture a;a.source.classId=10;a.spatial.toolBound=false;RetailScopeD82 profile(a,10);start82(a);
        g_crossbow_scan_xr34={};std::array<unsigned char,0x58> event{};a.block(event);a.put(a.p(event)+0x1C,0xEF667DEAu);
        cap::gate.select(cap::Domain::Reload,true,a.now);XR34BeforeEventNative(a.parent.data(),event.data());
        auto rows=drain(a.now);C87(rows.size()==1&&rows[0].find("result=EVENT_WITHOUT_SCAN")!=std::string::npos);
        C87(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f&&a.loads==0);
        close(a.now);
    }
    // A temporary higher-priority F5 capture cannot renew the still-active
    // F7 session's quota. Expiry and OFF also deny producer enqueue.
    {auto state=g_crossbow_xr34.status();const std::uint64_t now=10000;
        cap::gate.select(cap::Domain::Vrik,true,now);auto before=d::next.load();
        for(unsigned n=0;n<50;++n){state.owner.serial=100+n;d::recordNativeFaultD87(state,"QUOTA",now);}
        C87(d::next.load()==before+32);C87(drain(now).size()==32);
        const auto expiry=cap::gate.until[unsigned(cap::Domain::Vrik)].load();
        cap::gate.select(cap::Domain::Interaction,true,now+1);state.owner.serial=900;d::recordNativeFaultD87(state,"QUOTA",now+1);
        C87(drain(now+1).size()==1);cap::gate.select(cap::Domain::Interaction,false,now+2);
        before=d::next.load();state.owner.serial=901;d::recordNativeFaultD87(state,"QUOTA",now+2);
        C87(d::next.load()==before&&cap::gate.until[unsigned(cap::Domain::Vrik)].load()==expiry);
        d::recordNativeFaultD87(state,"EXPIRED",expiry);C87(d::next.load()==before);
        close(expiry);d::recordNativeFaultD87(state,"OFF",expiry+1);C87(d::next.load()==before);
    }
    return 0;
}
