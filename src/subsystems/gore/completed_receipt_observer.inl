 (void)kcd2vr::blood49::offer(r); // C49 BLOODY FIGHTS: a wounding hit by Henry, acted on once in the next native frame
 decap59c7e::Evaluate(p,r,phaseBefore,phaseAfter);
 impale40n::Evaluate(p,r,now); // C40: armour verdict for an undefended body thrust (edge, only while VrImpaleByArmour is on)
 if(r.death==1&&r.kind==1&&r.accepted&&r.applied&&r.fields&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){ // C44: every killing thrust says whether it may impale
  const float need44=kcd2vr::contact58c1::impaleKillEnergy44.load(std::memory_order_relaxed);
  const bool pierce97=kcd2vr::contact58c1::piercing97(r);
  const bool undefended44=pierce97&&r.bodyContact&&r.blocked==0&&r.perfect==0&&r.parry==0&&std::isfinite(r.damage)&&r.damage>0;
  // R97: feature + tip depth say why a blade-contact thrust is (not) a stab: BLADE needs the tip >= 2 cm under the touched surface
  const bool bladeGraze97=!pierce97&&r.feature==int(kcd2vr::strike_feature_d41::Feature::Blade)&&kcd2vr::strike_feature_d41::sword(r.weaponClass);
  const bool axial155=kcd2vr::contact58c1::axialThrust155(r); // R155: the point travelled along the blade at its fastest contact
  char line[400]{};std::snprintf(line,sizeof(line),"C44 IMPALE_KILL serial=%llu target=%u kind=THRUST death=1 energy=%.2f need=%.2f dmg=%.2f bodyContact=%d blocked=%d feature=%d tipDepthCm=%.1f axialCos=%.2f verdict=%s hang=%d rules=NATIVE_KILL_UNCHANGED",
   (unsigned long long)r.serial,r.target,double(kcd2vr::contact58c1::thrustPower96(r)),double(need44),double(r.damage),r.bodyContact?1:0,r.blocked,
   r.feature,std::isfinite(r.thrustTipDepth96)?double(r.thrustTipDepth96)*100.0:-1.0,std::isfinite(r.thrustAxialCos96)?double(r.thrustAxialCos96):-1.0,
   !axial155?"BLADE_ACROSS_THE_MOTION_NOT_A_STAB":bladeGraze97?"BLADE_TOUCH_TIP_NOT_UNDER_SURFACE":!undefended44?"NOT_A_CLEAN_BODY_THRUST":kcd2vr::contact58c1::strongEnough44(r,need44)?"IMPALE_REQUESTED":"TOO_WEAK_BELOW_VrImpaleKillEnergy",kcd2vr::gore32::impaleHang.load(std::memory_order_relaxed)?1:0);
  diagnostic_trace::Write(line);}
