#!/usr/bin/env python3
"""Run retained Python checks with explicit current/historical/unavailable reports.

Nothing edits an original assertion, replaces a protected hash, or relabels a
historical failure as a pass. Obsolete comparisons still execute and are recorded
separately, with the replacement rationale from the current user contract.
"""
import argparse
import io
import importlib.util
import json
import shutil
import sys
import unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]

class CheckoutLoader(unittest.TestLoader):
    """Load the discovered test file even when earlier tests prepend tools/.

    tools/test_subsystem_composition_m2.py has the same basename as the actual
    unit test. Normal import-by-name selected that CLI helper on Windows and
    aborted discovery before any original assertions could execute.
    """
    def _get_module_from_name(self,name):
        path=ROOT/'tests'/Path(*name.split('.')).with_suffix('.py')
        if path.is_file() and path.resolve().is_relative_to((ROOT/'tests').resolve()):
            spec=importlib.util.spec_from_file_location(name,path)
            module=importlib.util.module_from_spec(spec)
            sys.modules[name]=module
            spec.loader.exec_module(module)
            return module
        return super()._get_module_from_name(name)

def flatten(suite):
    for item in suite:
        if isinstance(item,unittest.TestSuite):yield from flatten(item)
        else:yield item

class Result(unittest.TestResult):
    def __init__(self):
        super().__init__();self.events=[]
    def addSuccess(self,test):
        self.events.append({'id':test.id(),'status':'passed'})
    def addFailure(self,test,error):
        detail=str(error[1])[:800]
        self.failures.append((test,detail))
        self.events.append({'id':test.id(),'status':'failed','detail':detail})
    def addError(self,test,error):
        detail=type(error[1]).__name__+': '+str(error[1])[:800]
        self.errors.append((test,detail))
        self.events.append({'id':test.id(),'status':'error','detail':detail})
    def addSkip(self,test,reason):
        super().addSkip(test,reason)
        self.events.append({'id':test.id(),'status':'skipped','detail':reason})

def run(cases):
    result=Result();unittest.TestSuite(cases).run(result)
    return {'total':result.testsRun,'passed':sum(x['status']=='passed' for x in result.events),
            'failed':len(result.failures),'errors':len(result.errors),'skipped':len(result.skipped),
            'results':result.events}

def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--out',required=True)
    args=parser.parse_args();out=Path(args.out).resolve();out.mkdir(parents=True,exist_ok=True)
    manifest=json.loads((ROOT/'tests/historical_expectations175.json').read_text())
    historical=manifest['tests'];suite=CheckoutLoader().discover(str(ROOT/'tests'),pattern='test_*.py')
    active=[];archive=[];unavailable=[];seen=set()
    cmake_case='test_calibration_wiring_v66100.Wiring.test_actual_seed_preserves_existing_ini'
    for case in flatten(suite):
        name=case.id();seen.add(name)
        if name in historical:archive.append(case)
        elif name==cmake_case and not shutil.which('cmake'):
            unavailable.append({'id':name,'status':'not_run','reason':'CMake is absent; the real CMake INI seeding script cannot run in this environment.'})
        else:active.append(case)
    missing=historical.keys()-seen
    if missing:raise SystemExit('Historical manifest names missing tests: '+','.join(sorted(missing)))
    report={'python':sys.version.split()[0],'active':run(active),'historical':run(archive),
            'historical_rationale':historical,'unavailable':unavailable,'native_execution':False,
            'historical_hashes_rewritten':False}
    (out/'results.json').write_text(json.dumps(report,indent=2)+'\n')
    for group in ('active','historical'):
        data=report[group]
        print(f"{group.upper()}: {data['passed']}/{data['total']} passed; failures={data['failed']}, errors={data['errors']}, skipped={data['skipped']}")
        for item in data['results']:
            if item['status']!='passed':print(f"  {item['status'].upper()} {item['id']}")
    for item in unavailable:print('UNAVAILABLE '+item['id']+': '+item['reason'])
    return int(bool(report['active']['failed'] or report['active']['errors']))

if __name__=='__main__':raise SystemExit(main())
