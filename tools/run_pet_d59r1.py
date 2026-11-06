#!/usr/bin/env python3
"""Focused D59R1 pet checks. Runs actual producer/input fixtures plus gesture/service tests."""
import argparse, concurrent.futures, datetime, hashlib, json, os, pathlib, subprocess, sys
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument("--source", type=pathlib.Path, help="Successor source checkout containing src/ and tests/")
parser.add_argument("--output", type=pathlib.Path, required=True)
parser.add_argument("--compiler", default="g++")
args=parser.parse_args()
candidates=[pathlib.Path(__file__).resolve().parents[1], pathlib.Path.cwd()]
root=args.source or next((p for p in candidates if (p/"tests/pet_producer_d59r1_test.cpp").is_file()),None)
if root is None or not (root/"tests/pet_producer_d59r1_test.cpp").is_file():
    parser.error("Specify --source with the successor checkout path")
root=root.resolve();out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
names=["pet_gesture_d59_test","pet_service_d59_test","misc_input_d59_test","pet_producer_d59r1_test"]
flags=["-std=c++20","-O1","-g","-fsanitize=address,undefined","-fno-omit-frame-pointer","-Isrc"]
def run(name):
    cmd=[args.compiler,*flags,"tests/"+name+".cpp","-o",str(out/name)]
    build=subprocess.run(cmd,cwd=root,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    result={"test":name,"compile_command":cmd,"compile_exit":build.returncode,"run_exit":None}
    log=build.stdout
    if build.returncode==0:
        env=dict(os.environ,ASAN_OPTIONS="detect_leaks=0")
        check=subprocess.run([str(out/name)],cwd=root,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,env=env)
        result["run_exit"]=check.returncode;log+=check.stdout
    (out/(name+".log")).write_text(log)
    result["log"]=str(out/(name+".log"))
    return result
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
    results=list(pool.map(run,names))
files=["src/interaction_alignment.cpp","src/interaction_alignment.h","src/subsystems/interactions/dog_candidate_d59r1.h","src/subsystems/interactions/dog_candidate_d59r1.inl","src/subsystems/interactions/pet_gesture_d59.h","src/subsystems/interactions/misc_gestures_d59.h","src/subsystems/interactions/misc_gestures_d59_input.inl",*["tests/"+n+".cpp" for n in names],"tests/dog_candidate_d59r1_fixture.h"]
summary={"time_utc":datetime.datetime.now(datetime.timezone.utc).isoformat(),"compiler":subprocess.check_output([args.compiler,"--version"],text=True).splitlines()[0],"scope":"Portable production observer/input/recognizer/service tests; native memory leaves simulated; no Windows DLL or headset run", "results":results,"sha256":{f:hashlib.sha256((root/f).read_bytes()).hexdigest() for f in files}}
(out/"results.json").write_text(json.dumps(summary,indent=2)+"\n")
print(json.dumps({"results":results},indent=2))
sys.exit(any(r["compile_exit"] or r["run_exit"]!=0 for r in results))
