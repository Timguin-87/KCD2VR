from m41_test_support import options,compile_and_run
import json
a=options('native_paper_render_m43')
p,log=compile_and_run(a,a.source_root/'tests/native_paper_render_m43_test.cpp','render',[a.source_root/'src'])
print(log);assert p.returncode==0 and 'failures=0' in log
(a.out/'RESULT.json').write_text(json.dumps(dict(result='PASS',log=log,native_leaves='SUBSTITUTED',headset='NOT_RUN'),indent=2))
