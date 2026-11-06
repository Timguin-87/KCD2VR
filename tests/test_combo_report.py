#!/usr/bin/env python3
"""Synthetic log/data tests, not replayed native combat results."""
import json,sys,unittest
from pathlib import Path
R=Path(__file__).resolve().parents[1];sys.path.insert(0,str(R/'tools'))
import analyze_v6691 as a
import build_combo_profiles_v6691 as p
D=R/'continuity/semantic_index/combo'
def event(call=1,op='STEP_FILTER',session='ABC',epoch=1,tick=1000,**extra):
    fields=dict(session=session,epoch=epoch,call=call,tick=tick,op=op,epochStable=1,comparable=1,observedFieldsChanged=1,acceptedHit='UNKNOWN',comboAccepted='UNKNOWN',finisherEligibility='UNKNOWN',entity=123);fields.update(extra)
    return 'V66.91-F3-COMBO_EVENT '+' '.join(f'{k}={v}' for k,v in fields.items())
def snap(side,call=1,session='ABC',epoch=1,**extra):
    f=dict(session=session,epoch=epoch,call=call,side=side,tick=1000,status='OK',truncated=0,statesTotal=1,statesCopied=1,candidatesCopied=1,details=1,expectedRows=1);f.update(extra)
    return 'V66.91-F3-COMBO_SNAPSHOT '+' '.join(f'{k}={v}' for k,v in f.items())
def candidate(side,call=1,session='ABC',epoch=1,**extra):
    f=dict(session=session,epoch=epoch,call=call,side=side,row=0,state=0,ordinal=0,name='C12',nameExact=1,requiredSteps=3,stepRows=3,techniqueBytesHex='0'*32);f.update(extra)
    return 'V66.91-F3-COMBO_CANDIDATE '+' '.join(f'{k}={v}' for k,v in f.items())
def episode(**kw):return '\n'.join([event(**kw),snap('BEFORE',**{k:v for k,v in kw.items() if k in ('call','session','epoch')}),candidate('BEFORE',**{k:v for k,v in kw.items() if k in ('call','session','epoch')}),snap('AFTER',**{k:v for k,v in kw.items() if k in ('call','session','epoch')}),candidate('AFTER',**{k:v for k,v in kw.items() if k in ('call','session','epoch')})])
class ReportTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):cls.profiles=json.loads((D/'recipe_profiles.json').read_text());cls.links=json.loads((D/'combo_fragment_links.json').read_text())
    def report(self,text,profile='unknown'):return a.analyze(text,self.profiles,self.links,profile)
    def test_episode(self):self.assertEqual(self.report(episode())['episode_count'],1)
    def test_changed_is_not_accepted(self):
        e=self.report(episode())['episodes'][0];self.assertTrue(e['observed_fields_changed']);self.assertEqual(e['combo_accepted'],'UNKNOWN');self.assertEqual(e['accepted_hit'],'UNKNOWN')
    def test_reset_is_not_finisher(self):self.assertEqual(self.report(episode(op='RESET'))['episodes'][0]['finisher_eligibility'],'UNKNOWN')
    def test_recorded_claim_cannot_authorize(self):self.assertEqual(self.report(episode().replace('finisherEligibility=UNKNOWN','finisherEligibility=1'))['episodes'][0]['finisher_eligibility'],'UNKNOWN')
    def test_orphan_candidate(self):self.assertFalse(self.report(candidate('AFTER'))['episodes'][0]['native_comparison_usable'])
    def test_missing_phase(self):self.assertFalse(self.report('\n'.join([event(),snap('BEFORE'),candidate('BEFORE')]))['episodes'][0]['native_comparison_usable'])
    def test_partial_snapshot(self):self.assertIsNone(self.report(episode().replace('truncated=0','truncated=1'))['episodes'][0]['observed_fields_changed'])
    def test_invalid_snapshot(self):self.assertFalse(self.report(episode().replace('status=OK','status=CHANGED_DURING_COPY'))['episodes'][0]['native_comparison_usable'])
    def test_epoch_switch(self):self.assertFalse(self.report(episode(epochStable=0))['episodes'][0]['native_comparison_usable'])
    def test_duplicate_header(self):self.assertFalse(self.report(episode()+'\n'+event())['episodes'][0]['native_comparison_usable'])
    def test_missing_row(self):
        e=self.report(episode().replace(candidate('AFTER'),''))['episodes'][0];self.assertEqual(e['snapshots']['AFTER']['detail_status'],'INCOMPLETE_OR_ORPHAN')
    def test_duplicate_row(self):self.assertFalse(self.report(episode()+'\n'+candidate('AFTER'))['episodes'][0]['snapshots']['AFTER']['detail_complete'])
    def test_detail_rate_limit(self):
        text='\n'.join([event(),snap('BEFORE',details=0,expectedRows=0),snap('AFTER',details=0,expectedRows=0)])
        e=self.report(text)['episodes'][0];self.assertTrue(e['native_comparison_usable']);self.assertEqual(e['snapshots']['AFTER']['detail_status'],'NOT_EMITTED')
    def test_name_alone_ambiguous(self):
        e=self.report(episode())['episodes'][0];c=e['snapshots']['AFTER']['rows'][0]['static_recipe_candidates'];self.assertGreater(len(c),1);self.assertTrue(all(not x['live_recipe_verified'] for x in c))
    def test_profile_user_annotation(self):
        r=self.report(episode(),'simple-combos');self.assertEqual(r['effective_game_load_order'],'UNKNOWN');c=r['episodes'][0]['snapshots']['AFTER']['rows'][0]['static_recipe_candidates'];self.assertTrue(all(x['profile']=='simple-combos' for x in c))
    def test_length_mismatch(self):
        r=self.report(episode().replace('requiredSteps=3','requiredSteps=99'));self.assertEqual(r['episodes'][0]['snapshots']['AFTER']['rows'][0]['static_recipe_candidates'],[])
    def test_bad_name(self):
        r=self.report(episode().replace('nameExact=1','nameExact=0'));self.assertEqual(r['episodes'][0]['snapshots']['AFTER']['rows'][0]['static_recipe_candidates'],[])
    def test_changed_session_no_join(self):self.assertEqual(self.report(episode()+'\n'+episode(session='DEF'))['episode_count'],2)
    def test_changed_epoch_no_join(self):self.assertEqual(self.report(episode()+'\n'+episode(epoch=2))['episode_count'],2)
    def test_old_log_no_invented_combo(self):self.assertEqual(self.report('V66.90-PLAYBACK CLIP epoch=1 seq=1 name="combat_combo_c12_lngsw_lngsw_solo" activated=1')['episode_count'],0)
    def test_malformed(self):self.assertEqual(len(self.report('V66.91-F3-COMBO_EVENT call=1')['malformed_lines']),1)
    def test_static_hit_not_actual(self):
        text='V66.90-PLAYBACK SNAP tick=100 epoch=1 seq=1 role=HENRY entries=1\nV66.90-PLAYBACK CLIP tick=100 epoch=1 seq=1 role=HENRY name="combat_combo_c12_lngsw_lngsw_solo" activated=1 progression=ADVANCED'
        c=self.report(text)['legacy_recordings'][0]['clips'][0];self.assertGreater(len(c['combo_semantic_candidates']),0);self.assertEqual(c['physical_contact_region'],'UNKNOWN')
    def test_empty_still_reports_limits(self):self.assertIn('No native combo episodes',a.markdown(self.report('')))
    def test_dataset_count_and_unique(self):
        self.assertEqual(len(self.profiles['profiles']['simple-combos']),51);self.assertEqual(len({p.key(x) for x in self.profiles['profiles']['simple-combos']}),51)
    def test_actual_differences(self):self.assertEqual(self.profiles['summary']['changed'],13);self.assertEqual(self.profiles['summary']['unchanged'],38)
    def test_actual_final_steps(self):self.assertTrue(all(d['final_step_identical'] for d in self.profiles['differences']))
    def test_not_every_combo_shortened(self):self.assertTrue(any(d['step_count_after']>d['step_count_before'] for d in self.profiles['differences']))
    def test_actual_mod_not_two_step_recipes(self):self.assertEqual(self.profiles['summary']['step_lengths_simple'],{'3':50,'4':1})
    def test_two_distinct_mod_layers(self):
        self.assertEqual(len(self.profiles['source']['members']),2);self.assertEqual(self.profiles['parameters']['simple_overrides'][0]['rpg_param_value'],'2');self.assertFalse(any('adb' in x['name'] for x in self.profiles['source']['members']))
    def test_physical_sequences_not_fabricated(self):self.assertTrue(all(x.get('physical_sequence') is None for profile in self.profiles['profiles'].values() for x in profile))
    def test_no_default_mod_inference(self):self.assertEqual(self.profiles['profile_default'],'unknown')
    def test_original_xml_parser(self):
        rows=p.parse(b'<database><CombatCombos><!-- KneeStrike --><CombatCombo Id="C06" RightWeapon="longsword" LeftWeapon="longsword"><CombatComboSteps><CombatComboStep GuardZone="1"/></CombatComboSteps></CombatCombo></CombatCombos></database>');self.assertEqual(rows[0]['source_comment'],'KneeStrike');self.assertIsNone(rows[0]['physical_sequence'])
    def test_same_id_across_weapons_allowed(self):
        data=b'<database><CombatCombos><CombatCombo Id="C06" RightWeapon="longsword"><CombatComboSteps/></CombatCombo><CombatCombo Id="C06" RightWeapon="blunt"><CombatComboSteps/></CombatCombo></CombatCombos></database>';self.assertEqual(len(p.parse(data)),2)
    def test_duplicate_exact_recipe_rejected(self):
        with self.assertRaises(ValueError):p.parse(b'<database><CombatCombos><CombatCombo Id="C06"><CombatComboSteps/></CombatCombo><CombatCombo Id="C06"><CombatComboSteps/></CombatCombo></CombatCombos></database>')
if __name__=='__main__':unittest.main()
