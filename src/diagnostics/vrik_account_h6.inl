// Included inside vrik_h29 after Record; observations never own pose/native data.
enum class MetricH6:unsigned {Observed,Throttled,QueueBusy,QueueFull,StaleSession,Discarded,Edges,SuppressedEdges,Duplicate,Backwards,Gap80,Begin,Write,Restore,Publication,NotAttempted,Published,RenderFallback,FingerRefused,FailureCopyBusy,Count};
struct AccountH6 {std::array<std::atomic<std::uint64_t>,unsigned(MetricH6::Count)> value{};};
struct FailureH6 {std::uint64_t session=0,ordinal=0,tick=0,sample=0,sourceTick=0,epoch=0,publication=0,proposedPublicationD49=0;unsigned consumersD49=0;bool pairedPublishedD49=false;std::uintptr_t character=0;int line=0,reason=0;char stage[72]{},end[48]{};};
inline std::array<AccountH6,unsigned(Kind::Count)> accountsH6{};
inline std::array<FailureH6,unsigned(Kind::Count)> failuresH6{};
inline std::atomic_flag failureLockH6=ATOMIC_FLAG_INIT;
inline std::atomic<std::uint64_t> formatterClippedH6{0};
inline std::atomic<std::uint64_t>& metricH6(unsigned k,MetricH6 m) noexcept {return accountsH6[k].value[unsigned(m)];}
inline const char* kindH6(unsigned k) noexcept {return k==unsigned(Kind::Cycle)?"CYCLE":k==unsigned(Kind::Skip)?"SKIP":k==unsigned(Kind::Render)?"RENDER":"PHYSICS";}
inline void outcomeH6(const Record& r,unsigned k) noexcept {
 bool failure=false;
 if(r.kind==Kind::Cycle||r.kind==Kind::Skip){
  const char* ends[]={"BEGIN_NOT_ENTERED","SOLVE_OR_WRITE_REFUSED","RESTORE_FAILED","PUBLICATION_REFUSED","PUBLICATION_NOT_ATTEMPTED","PUBLISHED"};
  const MetricH6 kinds[]={MetricH6::Begin,MetricH6::Write,MetricH6::Restore,MetricH6::Publication,MetricH6::NotAttempted,MetricH6::Published};
  for(unsigned i=0;i<6;++i)if(!std::strcmp(r.end,ends[i])){++metricH6(k,kinds[i]);failure=i!=5;break;}
  if(r.poseData)for(const auto& a:r.arms)if(a.context.fingerCall&&!a.context.fingerOk)++metricH6(k,MetricH6::FingerRefused);
 }else if(r.kind==Kind::Render&&r.reason>=0){++metricH6(k,MetricH6::RenderFallback);failure=true;}
 if(!failure)return;
 if(failureLockH6.test_and_set(std::memory_order_acquire)){++metricH6(k,MetricH6::FailureCopyBusy);return;}
 auto& f=failuresH6[k];if(r.observerOrdinal>f.ordinal){f.session=r.session;f.ordinal=r.observerOrdinal;f.tick=r.tick;f.sample=r.sample;f.sourceTick=r.sourceTick;f.epoch=r.epoch;f.publication=r.publication;f.proposedPublicationD49=r.proposedPublicationD49;f.pairedPublishedD49=r.pairedPublishedD49;f.consumersD49=r.consumersD49;f.character=r.character;f.line=r.line;f.reason=r.reason;std::memcpy(f.stage,r.stage,sizeof(f.stage));std::memcpy(f.end,r.end,sizeof(f.end));}
 failureLockH6.clear(std::memory_order_release);
}
inline void observeH6(Record& r,const Record& prior,bool jump,unsigned k) noexcept {
 const bool same=prior.session==r.session;
 r.sourceComparable=same&&prior.character==r.character&&prior.epoch==r.epoch&&prior.trackingEpoch==r.trackingEpoch&&prior.sourceTick&&r.sourceTick;
 if(r.sourceComparable){r.priorSample=prior.sample;r.priorSourceTick=prior.sourceTick;
  r.sourceDuplicate=r.sample==prior.sample&&r.sourceTick==prior.sourceTick;
  r.sourceBackwards=r.sample<prior.sample||r.sourceTick<prior.sourceTick||(r.timeNs&&prior.timeNs&&r.timeNs<prior.timeNs);
  if(r.sample>=prior.sample)r.sourceAdvance=r.sample-prior.sample;
  if(r.sourceTick>=prior.sourceTick)r.sourceDelta=r.sourceTick-prior.sourceTick;
  if(r.timeNs&&prior.timeNs&&r.timeNs>=prior.timeNs)r.sourceNsDelta=r.timeNs-prior.timeNs;
 }
 if(same&&r.tick>=prior.tick)r.observerDelta=r.tick-prior.tick;
 r.observationEdges=(!same||prior.character!=r.character||prior.epoch!=r.epoch||prior.trackingEpoch!=r.trackingEpoch?1u:0u)|
  (prior.tracking!=r.tracking||prior.allowed!=r.allowed?2u:0u)|(prior.context!=r.context||prior.policy!=r.policy||prior.control!=r.control?4u:0u)|
  (prior.committed!=r.committed||prior.restoreOk!=r.restoreOk||prior.published!=r.published||prior.packetPublished!=r.packetPublished||prior.pairedPublishedD49!=r.pairedPublishedD49||prior.consumersD49!=r.consumersD49||std::strcmp(prior.end,r.end)?8u:0u)|
  (std::strcmp(prior.stage,r.stage)||prior.line!=r.line||prior.reason!=r.reason?16u:0u);
 if(same&&r.poseData&&prior.poseData)for(unsigned side=0;side<2;++side){const auto& a=r.arms[side].context;const auto& b=prior.arms[side].context;
  if(a.finger!=b.finger||a.ownerMask!=b.ownerMask||a.heldKey!=b.heldKey||a.objectKey!=b.objectKey||a.objectGeneration!=b.objectGeneration||a.doorSerial!=b.doorSerial)r.observationEdges|=32u;
  if(a.historyCurrent!=b.historyCurrent||a.conditionAttemptedD44!=b.conditionAttemptedD44||a.conditionedOk!=b.conditionedOk||a.continuityOk!=b.continuityOk||a.solveOk!=b.solveOk)r.observationEdges|=64u;
 }
 if(jump)r.observationEdges|=128u;
 if(r.observationEdges)++metricH6(k,MetricH6::Edges);if(r.sourceDuplicate)++metricH6(k,MetricH6::Duplicate);if(r.sourceBackwards)++metricH6(k,MetricH6::Backwards);
 if(r.sourceComparable&&r.sourceDelta!=UINT64_MAX&&r.sourceDelta>80)++metricH6(k,MetricH6::Gap80);
}
inline bool captureLeaseH6(std::uint64_t session,std::uint64_t now) noexcept {return active(now)&&session==capture168::gate.session[unsigned(capture168::Domain::Vrik)].load(std::memory_order_acquire);}
template<class Writer> inline void summariesH6(std::uint64_t session,std::uint64_t now,Writer&& write) noexcept {
 if(!captureLeaseH6(session,now))return;
 constexpr const char* names[]={"observed","throttled","queueBusy","queueFull","staleSession","discarded","edgeObservations","suppressedEdges","duplicateSource","sourceBackwards","sourceGapOver80ms","beginNotEntered","solveOrWriteRefused","restoreFailed","publicationRefused","publicationNotAttempted","published","renderFallback","fingerRecipeRefused","failureCopyBusy"};
 static_assert(sizeof(names)/sizeof(names[0])==unsigned(MetricH6::Count));
 for(unsigned k=0;k<unsigned(Kind::Count);++k){if(!captureLeaseH6(session,now))return;if(!metricH6(k,MetricH6::Observed).load(std::memory_order_relaxed))continue;
  char text[1490]{};std::size_t used=0;bool clipped=false;
  const auto append=[&](const char* fmt,auto... args){if(clipped)return;const int n=std::snprintf(text+used,sizeof(text)-used,fmt,args...);if(n<0||std::size_t(n)>=sizeof(text)-used){clipped=true;++formatterClippedH6;}else used+=std::size_t(n);};
  append("VRIK_F7 ACCOUNT kind=%s",kindH6(k));for(unsigned m=0;m<unsigned(MetricH6::Count);++m)append(" %s=%llu",names[m],(unsigned long long)accountsH6[k].value[m].load(std::memory_order_relaxed));
  append(" session=%llu tick=%llu counters=PROCESS_TOTALS_NON_ATOMIC_SNAPSHOT coverage=ACTIVE_PUSH_CALLS_ONLY detailMs=25_MIN_250_STABLE formatterClipped=%llu edgeBits=IDENTITY:1,TRACKING:2,CONTEXT:4,OUTCOME:8,STAGE:16,HAND_OWNER:32,ELBOW:64,JUMP:128",(unsigned long long)session,(unsigned long long)now,(unsigned long long)formatterClippedH6.load());if(!captureLeaseH6(session,now))return;write(text);
  FailureH6 f{};bool copy=false;if(!failureLockH6.test_and_set(std::memory_order_acquire)){f=failuresH6[k];copy=true;failureLockH6.clear(std::memory_order_release);}
  if(copy&&f.ordinal&&f.session==session){if(!captureLeaseH6(session,now))return;std::snprintf(text,sizeof(text),"VRIK_F7 LAST_FAILURE kind=%s session=%llu observerOrdinal=%llu observedAt=%llu sample=%llu sourceTick=%llu epoch=%llu character=%llX publication=%llu proposedPublication=%llu pairedFramePublished=%d consumerEntryMask=%X stage=%s line=%d reason=%d end=%s freshness=HISTORICAL_OBSERVATION_NOT_CURRENT_POSE noNativeRefusalInferred=1",kindH6(k),(unsigned long long)f.session,(unsigned long long)f.ordinal,(unsigned long long)f.tick,(unsigned long long)f.sample,(unsigned long long)f.sourceTick,(unsigned long long)f.epoch,(unsigned long long)f.character,(unsigned long long)f.publication,(unsigned long long)f.proposedPublicationD49,f.pairedPublishedD49,f.consumersD49,f.stage,f.line,f.reason,f.end);write(text);}
 }
}
