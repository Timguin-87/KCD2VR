#pragma once
// A menu request is not a newly presented scene. Keep both facts in one
// finite F2/F5 receipt, without a renderer query when those recorders are off.
#include "capture168.h"
#include <cstdio>
namespace kcd2vr::diagnostics::presentation_d86 {
struct Snapshot {
    bool xrKnown=false;
    float xrZoom=0,dialogueMultiplier=0,sourceFov=0;
    int presentation=-1,mapping=-1;
    std::uint64_t publicationRevision=0,publishedEpoch=0,producedPairs=0,consumedPairSerial=0,presents=0;
    std::uint32_t publishedFrame=0;
    int publishedTechnology=-1;
    bool dlssEnabled=false;
    unsigned dlssFault=0;
};
inline bool selected() noexcept {
    constexpr auto bits=(1u<<unsigned(capture168::Domain::Bug))|(1u<<unsigned(capture168::Domain::Interaction));
    return (capture168::gate.mask.load(std::memory_order_relaxed)&bits)!=0;
}
template<class Reader,class Writer>
inline bool apply(std::uint64_t now,float beforeZoom,float requestedZoom,float storedZoom,
                  int cyclopean,int productionStereo,Reader&& read,Writer&& write) noexcept {
    const bool bug=capture168::active(capture168::Domain::Bug,now);
    const bool interaction=capture168::active(capture168::Domain::Interaction,now);
    if(!bug&&!interaction)return false;
    const auto bugSession=bug?capture168::gate.session[unsigned(capture168::Domain::Bug)].load(std::memory_order_acquire):0;
    const auto interactionSession=interaction?capture168::gate.session[unsigned(capture168::Domain::Interaction)].load(std::memory_order_acquire):0;
    const auto s=read();
    char line[1280]{};
    std::snprintf(line,sizeof(line),"%s PRESENTATION_D86 edge=MENU_APPLY tick=%llu F2session=%llu F5session=%llu beforeZoom=%.9g requestedZoom=%.9g storedZoom=%.9g xrKnown=%d xrZoom=%.9g dialogueMultiplier=%.9g combinedZoom=%.9g sourceFovDegrees=%.9g cyclopean=%d productionStereo=%d presentation=%d mapping=%d publicationRevision=%llu publishedEpoch=%llu publishedFrame=%u publishedTechnology=%d producedPairs=%llu consumedPairSerial=%llu presents=%llu dlssEnabled=%d dlssFault=%u snapshot=NONATOMIC queueEvidence=SUBMITTED_NOT_SCANOUT postApplyNewPixels=NOT_YET_OBSERVED settingsSource=RUNTIME_NOT_EXIT_INI",
        bug?"D42_F2":"D42_F5",(unsigned long long)now,(unsigned long long)bugSession,(unsigned long long)interactionSession,
        double(beforeZoom),double(requestedZoom),double(storedZoom),s.xrKnown,double(s.xrZoom),double(s.dialogueMultiplier),double(s.xrZoom*s.dialogueMultiplier),double(s.sourceFov),
        cyclopean,productionStereo,s.presentation,s.mapping,(unsigned long long)s.publicationRevision,(unsigned long long)s.publishedEpoch,s.publishedFrame,s.publishedTechnology,
        (unsigned long long)s.producedPairs,(unsigned long long)s.consumedPairSerial,(unsigned long long)s.presents,s.dlssEnabled,s.dlssFault);
    write(line);return true;
}
}
