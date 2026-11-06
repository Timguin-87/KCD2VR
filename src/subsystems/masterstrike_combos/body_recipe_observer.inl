 static bc::Matcher<bc::recipes.size()> matcher;
 const auto c=kcd2vr::contact59c7g::physicalCandidate(r);
 const auto progress=matcher.observe(bc::recipes,c);
 const bool capture=Active()||kcd2vr::contact58c1::experiment.load(std::memory_order_relaxed);
 if(!capture)return r;
 char line[768]{};
 std::snprintf(line,sizeof(line),
  "C7G CONTACT_RECIPE serial=%llu attempt=%llu target=%u route=%s accepted=%d applied=%d continued=%d regionCandidate=%s material=%d nativePart=%d feature=%d nextCandidates=%u matches=%llX duplicate=%d nativeTechnique=NOT_DISPATCHED regionDomain=UNCONFIRMED hitGate=UNCHANGED",
  (unsigned long long)r.serial,(unsigned long long)r.attempt,r.target,direct?"VR_DIRECT":"NATIVE_COMPARISON",
  r.accepted,r.applied,r.continuation,bc::name(c.region),r.bodyMaterial,r.nativePart,r.feature,progress.advanced,
  (unsigned long long)progress.completed,progress.duplicate);
 diagnostic_trace::Write(line);
