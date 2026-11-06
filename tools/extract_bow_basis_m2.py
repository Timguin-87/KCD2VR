"""Extract actual camera basis selection and both HMD consumers for a host test."""
import hashlib
import json
from pathlib import Path
from source_text import read_source

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "src/camera_hook.cpp"
source = read_source(SOURCE)


def part(start, end):
    a = source.index(start)
    return source[a:source.index(end, a + len(start))]


math = part("struct Vec3f {", "static uintptr_t g_hist_cview=")
math += part("static Quatf Normalize(", "static Quatf SlerpShortest(")
math += part("static Vec3f Rotate(", "// C47: accepted XR input")
math += part("static void XrQuatToCryMatrix(", "static bool IsExactGameCView(")
consumer = part("        const bool activityEye59c19=", "        // V64.22B: publish")
consumer += part("        // V66.109: ordinary immersive VR", "        // V64.37 F3 marriage lab.")
consumer += part("        if (s_apply_rotation.load(std::memory_order_relaxed)) {\n            const int order=", "        PublishVisualForwardFromCameraMatrix(matrix);\n        if(hmd_final_orientation_authority")
position = part("            const bool use_visual_position_basis=", "        // V65.09 F5: publish position")
position = position[:position.rindex("        }")]
payloads = {"generated_bow_basis_math_m2.inc": math,
            "generated_bow_basis_consumer_m2.inc": consumer + position}
receipt = {"source": "src/camera_hook.cpp", "source_sha256": hashlib.sha256(SOURCE.read_bytes()).hexdigest(),
           "boundary": "Actual selected camera statements/math; current TrackedFrameCamera55 admission is a test leaf; no native/headset execution",
           "members": {}}
for name, body in payloads.items():
    payload = ("// Actual camera source; regenerate with tools/extract_bow_basis_m2.py.\n" + body).encode()
    (ROOT / "tests" / name).write_bytes(payload)
    receipt["members"][name] = hashlib.sha256(payload).hexdigest()
(ROOT / "tests/bow_basis_m2.provenance.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
