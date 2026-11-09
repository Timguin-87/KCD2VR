// R154 alternate-eye stale-eye warp. See aer_warp_r154.h for the tier ladder.
#include "aer_warp_r154.h"
#include "device_hook.h"
#include "diagnostic_trace.h"

#include <windows.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <utility>
#include <vector>

namespace kcd2vr::aer_warp_r154 {
namespace {
using Microsoft::WRL::ComPtr;

std::atomic<int> g_mode{kMotionVectors};
std::atomic<bool> g_tint{false};
std::atomic<std::uint64_t> g_ts_freq{0};

// ------------------------------------------------------------------ shaders
// Camera-local space is CryEngine's: x right, y forward (depth), z up. uv is
// the image's [0,1]^2 with y down. Both cameras of a warp share the game's
// symmetric frustum (tanX/tanY), so "uv" always means the full rendered image.
//
// Constant layout (48 words): G0 G1 G2 (uint4), F0 F1 F2 (float4),
// A0..A2 rows of one rigid transform, B0..B2 rows of another.
constexpr const char* kHlsl = R"(
cbuffer C : register(b0) {
    uint4 G0; uint4 G1; uint4 G2;
    float4 F0; float4 F1; float4 F2;
    float4 A0; float4 A1; float4 A2;
    float4 B0; float4 B1; float4 B2;
};
float3 Xform(float4 a, float4 b, float4 c, float3 v) { return float3(dot(a.xyz, v) + a.w, dot(b.xyz, v) + b.w, dot(c.xyz, v) + c.w); }
float3 Rot(float4 a, float4 b, float4 c, float3 v) { return float3(dot(a.xyz, v), dot(b.xyz, v), dot(c.xyz, v)); }
float2 ProjectUv(float3 l, float2 t) { const float y = max(l.y, 1e-4); return float2(0.5 + 0.5 * l.x / (y * t.x), 0.5 - 0.5 * l.z / (y * t.y)); }
float3 RayAt(float2 uv, float2 t) { return float3((uv.x * 2.0 - 1.0) * t.x, 1.0, (1.0 - uv.y * 2.0) * t.y); }

groupshared uint gsStats[8];
void StatsBegin(uint gi) { if (gi < 8) gsStats[gi] = 0; GroupMemoryBarrierWithGroupSync(); }
void StatAdd(uint i) { InterlockedAdd(gsStats[i], 1u); }

#if defined(EVAL)
// ---------------------------------------------------------------- live eye (game list, before NGX)
// G0 = (renderW, renderH, depthMode, flags: 1 stale depth is the previous frame, 2 write cross, 4 write own)
// G1 = (depthX, depthY, mvX, mvY)   G2 = (mvW, mvH, staleW, staleH)
// F0 = (mvScaleX, mvScaleY, curTanX, curTanY)   F1 = (near, far, prevTanX, prevTanY)
// F2 = (2 * live eye offset along right, gate factor, minimum object motion px, depth tolerance)
// A = current -> previous camera rows, B = previous -> current camera rows
Texture2D<float2> GameMV : register(t0);
Texture2D<float> GameDepth : register(t1);
Texture2D<float> StaleDepth : register(t2);
RWTexture2D<float> LiveDepthOut : register(u0);
RWTexture2D<float4> OwnOut : register(u1);
RWTexture2D<float4> CrossOut : register(u2);
RWStructuredBuffer<uint> Stats : register(u3);
void StatsEnd(uint gi) { GroupMemoryBarrierWithGroupSync(); if (gi < 8 && gsStats[gi] != 0) InterlockedAdd(Stats[gi], gsStats[gi]); }

float LinearZ(float d, uint mode, float n, float f) {
    // Same conventions as the R149 model check: 0 reversed, 1 standard, 2 linear/far, 3 metres.
    if (mode == 3) return max(d, n);
    if (mode == 2) return max(d * f, n);
    const float dr = mode == 0 ? d : 1.0 - d;
    return n * f / max(n + dr * (f - n), 1e-7);
}

// The game's vector of a live pixel points at the previous render: the stale
// eye's image. A static point predicts that vector exactly from the two camera
// banks; what is left over is the surface's own motion. For a moving surface the
// previous position (its pixel in the stale image, at the stale eye's own depth)
// gives (a) where the stale pixel must go now -- the live surface position seen
// from the stale eye's target camera (cross field, on the stale grid) and (b)
// the surface's world motion over the last frame (own field, on the live grid),
// which the next warp of THIS eye carries one frame on for pixels the other eye
// cannot see.
void Live(uint2 p) {
    const float2 render = float2(G0.xy);
    const float d = GameDepth.Load(int3(int2(p) + int2(G1.xy), 0));
    const float z = LinearZ(d, G0.z, F1.x, F1.y);
    LiveDepthOut[p] = z;
    const uint flags = G0.w;
    if ((flags & 1u) == 0) return;
    float4 own = float4(0.0, 0.0, 0.0, 0.0);
    const float2 uv = (float2(p) + 0.5) / render;
    const float3 L = RayAt(uv, F0.zw) * z;
    const int2 mvPixel = int2(G1.zw) + int2((float2(p) + 0.5) * float2(G2.xy) / render);
    float2 mv = GameMV.Load(int3(mvPixel, 0));
    if (!all(isfinite(mv)) || any(abs(mv) > 60000.0)) mv = float2(0.0, 0.0);
    const float2 gamePx = mv * F0.xy;
    const float3 Ls = Xform(A0, A1, A2, L);
    if (Ls.y > F1.x * 0.5) {
        const float2 uvS = ProjectUv(Ls, F1.zw);
        const float2 uvInf = ProjectUv(Rot(A0, A1, A2, L), F1.zw);
        // Depth-model sensitivity: only motion clearly above what a depth error
        // would produce counts as the surface moving (same gate as R149).
        const float gate = max(F2.z, 1.0 + F2.y * length((uvS - uvInf) * render));
        const float2 objectPx = gamePx - (uvS - uv) * render;
        if (dot(objectPx, objectPx) > gate * gate) {
            StatAdd(0);
            const float2 uvQ = uv + gamePx / render;
            const float2 stale = float2(G2.zw);
            const float2 q = uvQ * stale;
            if (q.x >= 0.0 && q.y >= 0.0 && q.x < stale.x && q.y < stale.y) {
                const int2 qi = int2(q);
                const float z1 = StaleDepth.Load(int3(qi, 0));
                // The stale pixel must show THIS surface (an occluder there means
                // the surface was hidden from the stale eye). Its depth may differ
                // by the surface's own travel along the view.
                if (z1 > 0.0 && abs(z1 - Ls.y) <= F2.w * Ls.y + 0.02) {
                    StatAdd(1);
                    const float3 Lprev = RayAt(uvQ, F1.zw) * z1;
                    own = float4(L - Xform(B0, B1, B2, Lprev), 1.0);
                    if ((flags & 2u) != 0) {
                        const float3 LT = L + float3(F2.x, 0.0, 0.0);
                        if (LT.y > F1.x * 0.5) {
                            const float2 uvT = ProjectUv(LT, F0.zw);
                            CrossOut[qi] = float4((uvT - uvQ) * stale, LT.y, 1.0);
                        }
                    }
                } else StatAdd(2);
            } else StatAdd(3);
        }
    }
    if ((flags & 4u) != 0) OwnOut[p] = own;
}

#if defined(PASS_CLEAR_CROSS)
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID) {
    if (id.x < G2.z && id.y < G2.w) CrossOut[id.xy] = float4(0.0, 0.0, 0.0, 0.0);
}
#else
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID, uint gi : SV_GroupIndex) {
    StatsBegin(gi);
    if (id.x < G0.x && id.y < G0.y) Live(id.xy);
    StatsEnd(gi);
}
#endif

#else
// ---------------------------------------------------------------- stale eye (XR blit list)
// Forward splat of the stale eye's render grid into the target grid (same size):
// a depth key (nearer wins) then the winner's offset, then a resolve that fills
// cracks and holes. The blit adds the offset to its source uv and samples once.
// G0 = (gridW, gridH, flags: 1 cross field, 2 own field, hole search texels)
// G1 = (backward iterations, debug tint, 0, 0)
// F0 = (staleTanX, staleTanY, targetTanX, targetTanY)   F1 = (min depth, same-surface tolerance, far depth, 0)
// A = stale -> target camera rows, B = target -> stale camera rows
Texture2D<float> StaleDepth : register(t0);
Texture2D<float4> Cross : register(t1);
Texture2D<float4> Own : register(t2);
Texture2D<uint> KeyIn : register(t3);
Texture2D<float2> OffIn : register(t4);
RWTexture2D<uint> Key : register(u0);
RWTexture2D<float2> Off : register(u1);
RWTexture2D<float4> OffFinal : register(u2);
RWStructuredBuffer<uint> Stats : register(u3);
void StatsEnd(uint gi) { GroupMemoryBarrierWithGroupSync(); if (gi < 8 && gsStats[gi] != 0) InterlockedAdd(Stats[8 + gi], gsStats[gi]); }

static const int2 kN8[8] = { int2(-1, -1), int2(0, -1), int2(1, -1), int2(-1, 0), int2(1, 0), int2(-1, 1), int2(0, 1), int2(1, 1) };
static const int2 kPairs[8] = { int2(-1, 0), int2(1, 0), int2(0, -1), int2(0, 1), int2(-1, -1), int2(1, 1), int2(1, -1), int2(-1, 1) };
static const int2 kAxis[4] = { int2(1, 0), int2(-1, 0), int2(0, 1), int2(0, -1) };

bool Inside(int2 p) { return p.x >= 0 && p.y >= 0 && p.x < (int)G0.x && p.y < (int)G0.y; }
// Nearer must win: 1/depth grows as the surface comes closer, and a positive
// float's bits order like its value. The two low bits carry the tier (1 static,
// 2 own motion, 3 measured motion) for the debug tint and the statistics.
uint MakeKey(float depthT, uint tier) { return (asuint(1.0 / max(depthT, 1e-6)) & ~3u) | tier; }
float KeyDepth(uint k) { return 1.0 / max(asfloat(k & ~3u), 1e-12); }

// Where stale render texel q lands in the target image (uv), at what depth.
bool StaleDst(int2 q, out float2 dstUv, out float depthT, out uint tier) {
    dstUv = float2(0.0, 0.0); depthT = 0.0; tier = 0u;
    const float z = StaleDepth.Load(int3(q, 0));
    if (!(z > F1.x)) return false;
    const float2 grid = float2(G0.xy);
    const float2 uv = (float2(q) + 0.5) / grid;
    const uint flags = G0.z;
    if ((flags & 1u) != 0) {
        // 1. The live eye saw this surface move: its position now, seen from here.
        float4 c = Cross.Load(int3(q, 0));
        // 2. Its own field says it moves but nothing landed on it (a gap in the
        //    scatter): a neighbour on the same surface.
        if (!(c.w > 0.5) && (flags & 2u) != 0 && Own.Load(int3(q, 0)).w > 0.5) {
            bool found = false;
            [unroll] for (int k = 0; k < 8; ++k) {
                const int2 n = q + kN8[k];
                if (found || !Inside(n)) continue;
                const float4 cn = Cross.Load(int3(n, 0));
                if (cn.w > 0.5 && abs(StaleDepth.Load(int3(n, 0)) - z) <= F1.y * z) { c = cn; found = true; }
            }
        }
        if (c.w > 0.5 && c.z > F1.x) { dstUv = uv + c.xy / grid; depthT = c.z; tier = 3u; return true; }
    }
    // 3. The live eye did not see it: the surface's own last motion, carried on.
    // 4. Otherwise static world: camera rotation + translation through its depth.
    float3 L = RayAt(uv, F0.xy) * z;
    tier = 1u;
    if ((flags & 2u) != 0) {
        const float4 o = Own.Load(int3(q, 0));
        if (o.w > 0.5) { L += o.xyz; tier = 2u; }
    }
    const float3 LT = Xform(A0, A1, A2, L);
    if (!(LT.y > F1.x)) return false;
    dstUv = ProjectUv(LT, F0.zw);
    depthT = LT.y;
    return true;
}

#if defined(PASS_CLEAR)
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID) {
    if (id.x >= G0.x || id.y >= G0.y) return;
    Key[id.xy] = 0u;
    Off[id.xy] = float2(0.0, 0.0);
}
#elif defined(PASS_SPLAT_KEY)
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID) {
    if (id.x >= G0.x || id.y >= G0.y) return;
    float2 dst; float dT; uint tier;
    if (!StaleDst(int2(id.xy), dst, dT, tier)) return;
    const int2 t = int2(floor(dst * float2(G0.xy)));
    if (!Inside(t)) return;
    InterlockedMax(Key[t], MakeKey(dT, tier));
}
#elif defined(PASS_SPLAT_OFF)
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID) {
    if (id.x >= G0.x || id.y >= G0.y) return;
    float2 dst; float dT; uint tier;
    if (!StaleDst(int2(id.xy), dst, dT, tier)) return;
    const int2 t = int2(floor(dst * float2(G0.xy)));
    if (!Inside(t)) return;
    // Only the winner publishes where its colour is in the stale image.
    if (Key[t] != MakeKey(dT, tier)) return;
    Off[t] = (float2(id.xy) + 0.5) / float2(G0.xy) - dst;
}
#elif defined(PASS_RESOLVE)
bool Reached(int2 p, out uint k) { k = 0u; if (!Inside(p)) return false; k = KeyIn.Load(int3(p, 0)); return k != 0u; }

// A lone unreached texel between reached ones: a surface that spread apart.
// Same surface on both sides first, else the farther side (background beside
// a moving edge).
bool CrackSource(int2 p, out int2 from, out uint kOut) {
    from = p; kOut = 0u;
    uint sameK = 0u, farK = 0xFFFFFFFFu; int2 sameF = p, farF = p; bool haveFar = false;
    [unroll] for (int i = 0; i < 8; i += 2) {
        const int2 a = p + kPairs[i], b = p + kPairs[i + 1];
        uint ka, kb;
        const bool ra = Reached(a, ka);
        const bool rb = Reached(b, kb);
        if (!ra || !rb) continue;
        const bool aNear = ka >= kb;
        const uint kn = aNear ? ka : kb, kf = aNear ? kb : ka;
        const float dn = KeyDepth(kn), df = KeyDepth(kf);
        if (df - dn <= F1.y * dn) { if (kn > sameK) { sameK = kn; sameF = aNear ? a : b; } }
        else if (kf < farK) { farK = kf; farF = aNear ? b : a; haveFar = true; }
    }
    if (sameK != 0u) { from = sameF; kOut = sameK; return true; }
    if (haveFar) { from = farF; kOut = farK; return true; }
    return false;
}

// Reached, but by something behind a surface whose neighbours close around it:
// a magnified surface's scatter gap that background leaked into.
bool CoveredCrack(int2 p, uint kHere, out int2 from, out uint kOut) {
    from = p; kOut = 0u;
    const float dHere = KeyDepth(kHere);
    [unroll] for (int i = 0; i < 8; i += 2) {
        const int2 a = p + kPairs[i], b = p + kPairs[i + 1];
        uint ka, kb;
        const bool ra = Reached(a, ka);
        const bool rb = Reached(b, kb);
        if (!ra || !rb) continue;
        const bool aNear = ka >= kb;
        const uint kn = aNear ? ka : kb, kf = aNear ? kb : ka;
        const float dn = KeyDepth(kn), df = KeyDepth(kf);
        if (df - dn > F1.y * dn) continue;
        if (df >= dHere * (1.0 - F1.y)) continue;
        if (kn > kOut) { kOut = kn; from = aNear ? a : b; }
    }
    return kOut != 0u;
}

// A real hole (disocclusion): what was behind a moving or nearer surface. The
// stale eye never saw it; extend the BACKGROUND beside it, at that surface's own
// rotation + 6DoF warp -- the first reached texel on each axis, the farthest wins.
bool BackgroundSource(int2 p, out int2 from, out uint kOut) {
    from = p; kOut = 0u;
    uint best = 0xFFFFFFFFu; bool found = false;
    [unroll] for (int d = 0; d < 4; ++d) {
        [loop] for (int i = 1; i <= (int)G0.w; ++i) {
            const int2 n = p + kAxis[d] * i;
            if (!Inside(n)) break;
            const uint k = KeyIn.Load(int3(n, 0));
            if (k != 0u) { if (k < best) { best = k; from = n; found = true; } break; }
        }
    }
    if (found) kOut = best;
    return found;
}

// Nothing reached within reach: the lower tier for the whole pixel -- a backward
// rotation + 6DoF warp through the stale depth (fixed point from the rotation-
// only position), else rotation alone.
void BackwardWarp(int2 t, out float2 off, out float depth, out uint tier) {
    const float2 grid = float2(G0.xy);
    const float2 x = (float2(t) + 0.5) / grid;
    const float2 s0 = ProjectUv(Rot(B0, B1, B2, RayAt(x, F0.zw)), F0.xy);
    float2 s = s0;
    bool ok = false;
    float dT = F1.z;
    [loop] for (uint i = 0; i < G1.x; ++i) {
        if (s.x < 0.0 || s.y < 0.0 || s.x >= 1.0 || s.y >= 1.0) break;
        const float z = StaleDepth.Load(int3(int2(s * grid), 0));
        if (!(z > F1.x)) break;
        const float3 LT = Xform(A0, A1, A2, RayAt(s, F0.xy) * z);
        if (!(LT.y > F1.x)) break;
        const float2 xs = ProjectUv(LT, F0.zw);
        dT = LT.y;
        const float2 err = (x - xs) * grid;
        if (dot(err, err) < 0.25) { ok = true; break; }
        s += s0 - ProjectUv(Rot(B0, B1, B2, RayAt(xs, F0.zw)), F0.xy);
    }
    off = (ok ? s : s0) - x;
    depth = ok ? dT : F1.z;
    tier = ok ? 6u : 7u;
}

[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID, uint gi : SV_GroupIndex) {
    StatsBegin(gi);
    if (id.x < G0.x && id.y < G0.y) {
        const int2 t = int2(id.xy);
        const uint k = KeyIn.Load(int3(t, 0));
        float2 off = float2(0.0, 0.0);
        float depth = F1.z;
        uint tier = 7u;
        int2 from; uint kn;
        if (k != 0u) {
            off = OffIn.Load(int3(t, 0)); depth = KeyDepth(k); tier = k & 3u;
            if (CoveredCrack(t, k, from, kn)) { off = OffIn.Load(int3(from, 0)); depth = KeyDepth(kn); tier = 4u; }
        } else if (CrackSource(t, from, kn)) {
            off = OffIn.Load(int3(from, 0)); depth = KeyDepth(kn); tier = 4u;
        } else if (BackgroundSource(t, from, kn)) {
            off = OffIn.Load(int3(from, 0)); depth = KeyDepth(kn); tier = 5u;
        } else {
            BackwardWarp(t, off, depth, tier);
        }
        // w = tier (+16 asks the blit for the debug tint)
        OffFinal[t] = float4(off, depth, float(tier) + (G1.y != 0u ? 16.0 : 0.0));
        StatAdd(tier & 7u);
    }
    StatsEnd(gi);
}
#endif
#endif
)";

struct Constants {
    std::uint32_t g0[4]{}, g1[4]{}, g2[4]{};
    float f0[4]{}, f1[4]{}, f2[4]{};
    float a[12]{}, b[12]{};
};
static_assert(sizeof(Constants) == 48 * 4, "root constants are 48 words");

// rows = [R_K^T R_N | R_K^T (C_N - C_K)]: camera N local point -> camera K local
// (double on the CPU so kilometre world coordinates cancel before the float cast).
void RelativeRows(const Camera& n, const Camera& k, float rows[12]) noexcept {
    const double d[3]{n.pos[0] - k.pos[0], n.pos[1] - k.pos[1], n.pos[2] - k.pos[2]};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            double a = 0.0;
            for (int r = 0; r < 3; ++r) a += k.rot[r * 3 + i] * n.rot[r * 3 + j];
            rows[i * 4 + j] = static_cast<float>(a);
        }
        double b = 0.0;
        for (int r = 0; r < 3; ++r) b += k.rot[r * 3 + i] * d[r];
        rows[i * 4 + 3] = static_cast<float>(b);
    }
}
double AngleDeg(const Camera& a, const Camera& b) noexcept {
    double trace = 0.0;
    for (int k = 0; k < 3; ++k) for (int r = 0; r < 3; ++r) trace += a.rot[r * 3 + k] * b.rot[r * 3 + k];
    return std::acos(std::clamp((trace - 1.0) * 0.5, -1.0, 1.0)) * 57.29577951308232;
}
double DistM(const Camera& a, const Camera& b) noexcept {
    const double dx = a.pos[0] - b.pos[0], dy = a.pos[1] - b.pos[1], dz = a.pos[2] - b.pos[2];
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

// ------------------------------------------------------------------ GPU state
constexpr unsigned kSetSize = 9;      // 5 SRV + 4 UAV
constexpr unsigned kSets = 128;
constexpr unsigned kStatsWords = 16;  // [0..7] live-eye pass, [8..15] warp resolve tiers
constexpr unsigned kRing = 8;

struct Tex {
    ComPtr<ID3D12Resource> res;
    unsigned w{}, h{};
    DXGI_FORMAT fmt{DXGI_FORMAT_UNKNOWN};
    D3D12_RESOURCE_STATES state{D3D12_RESOURCE_STATE_COMMON};
};

struct EyeRecord {
    bool valid{};
    std::uint32_t renderFrame{};
    std::uint64_t poseFrame{};
    Camera cam{};
    float eyeOffsetWorld{};
    bool offsetKnown{};
    unsigned w{}, h{};
    unsigned depthSlot{};
    bool ownValid{};          // own[eye] holds this frame's motion
    bool crossValid{};        // cross[eye] holds this frame's field on the other eye's grid
    std::uint64_t crossStalePose{};
    std::uint32_t crossStaleFrame{};
};

struct StatsRing {
    ComPtr<ID3D12Resource> buffer, zero, readback[kRing];
    const std::uint32_t* mapped[kRing]{};
    std::uint64_t ticket[kRing]{};
    unsigned cursor{};
    D3D12_RESOURCE_STATES state{D3D12_RESOURCE_STATE_COPY_DEST};
};
struct TimerRing {
    ComPtr<ID3D12QueryHeap> heap;
    ComPtr<ID3D12Resource> readback;
    const std::uint64_t* mapped{};
    std::uint64_t ticket[kRing]{};
    unsigned cursor{};
    unsigned active{kRing};   // slot opened by Begin, kRing = none
    double sumMs{};
    std::uint64_t samples{};
};

struct Counters {
    std::uint64_t evaluations{}, evalSkipped{}, evalMotion{}, evalNoStale{}, evalPrevMismatch{}, evalNoOffset{};
    std::uint64_t warps{}, warpReused{}, warpCross{}, warpOwn{};
    std::uint64_t fail[12]{};
    std::uint64_t live[8]{}, tiers[8]{};
};
const char* const kFailNames[12]{"mode","gpu","no-stale-record","no-live-record","eye-offset","size","camera-jump",
    "model","textures","no-request","busy","other"};
enum Fail { FMode, FGpu, FNoStale, FNoLive, FEyeOffset, FSize, FJump, FModel, FTextures, FNoRequest, FBusy, FOther };

struct State {
    std::mutex mutex;
    ComPtr<ID3D12Device> device;
    ComPtr<ID3D12RootSignature> root;
    ComPtr<ID3D12PipelineState> clearCross, live, clear, splatKey, splatOff, resolve;
    ComPtr<ID3D12DescriptorHeap> heap;
    UINT inc{};
    unsigned cursor{};
    bool broken{};
    const char* brokenReason{""};

    Tex depth[2][2], own[2], cross[2];
    EyeRecord rec[2][2];
    unsigned latest[2]{};
    std::uint32_t ownFrame[2]{}, crossFrame[2]{};
    bool ownFrameValid[2]{}, crossFrameValid[2]{};

    Tex key, off, offFinal;
    bool cacheValid{};
    std::uint64_t cacheLive{}, cacheStale{};
    std::uint32_t cacheFlags{};

    StatsRing evalStats, xrStats;
    TimerRing evalTimer, xrTimer;
    std::uint64_t evalUses{}, xrUses{};
    std::uint64_t lastUse{};
    std::uint64_t lastLog{};
    Counters c{};
    std::vector<std::pair<std::uint64_t, ComPtr<ID3D12Resource>>> retired;
};
State g;

void Break(const char* reason) noexcept {
    if (!g.broken) {
        char line[200]{};
        std::snprintf(line, sizeof(line), "R154 AER_WARP gpu=DISABLED reason=%s fallback=ROTATION_WARP", reason ? reason : "unknown");
        diagnostic_trace::WriteStatus(line);
    }
    g.broken = true;
    g.brokenReason = reason ? reason : "unknown";
}

D3D12_HEAP_PROPERTIES HeapProps(D3D12_HEAP_TYPE type) noexcept {
    D3D12_HEAP_PROPERTIES h{};
    h.Type = type; h.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    h.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN; h.CreationNodeMask = h.VisibleNodeMask = 1;
    return h;
}
D3D12_RESOURCE_DESC BufferDesc(UINT64 bytes, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE) noexcept {
    D3D12_RESOURCE_DESC d{};
    d.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; d.Width = bytes; d.Height = 1; d.DepthOrArraySize = 1;
    d.MipLevels = 1; d.Format = DXGI_FORMAT_UNKNOWN; d.SampleDesc.Count = 1; d.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; d.Flags = flags;
    return d;
}

void Retire(ComPtr<ID3D12Resource>& r) noexcept {
    // GPU work may still read it (queued frames); free it seconds later.
    if (r) g.retired.emplace_back(static_cast<std::uint64_t>(GetTickCount64()) + 3000ull, std::move(r));
    r.Reset();
}
void FreeRetired(bool all) noexcept {
    const auto now = static_cast<std::uint64_t>(GetTickCount64());
    g.retired.erase(std::remove_if(g.retired.begin(), g.retired.end(),
        [&](const auto& e) { return all || e.first <= now; }), g.retired.end());
}

bool MakeTex(Tex& t, unsigned w, unsigned h, DXGI_FORMAT fmt) noexcept {
    if (t.res && t.w == w && t.h == h && t.fmt == fmt) return true;
    Retire(t.res);
    t = Tex{};
    D3D12_RESOURCE_DESC d{};
    d.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; d.Width = w; d.Height = h; d.DepthOrArraySize = 1;
    d.MipLevels = 1; d.Format = fmt; d.SampleDesc.Count = 1; d.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    d.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    auto heap = HeapProps(D3D12_HEAP_TYPE_DEFAULT);
    if (FAILED(g.device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &d, D3D12_RESOURCE_STATE_COMMON,
                                                 nullptr, IID_PPV_ARGS(&t.res)))) {
        t = Tex{};
        return false;
    }
    t.w = w; t.h = h; t.fmt = fmt; t.state = D3D12_RESOURCE_STATE_COMMON;
    return true;
}

bool InitStats(StatsRing& s) noexcept {
    auto def = HeapProps(D3D12_HEAP_TYPE_DEFAULT), up = HeapProps(D3D12_HEAP_TYPE_UPLOAD), rb = HeapProps(D3D12_HEAP_TYPE_READBACK);
    auto uavDesc = BufferDesc(kStatsWords * 4, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    auto plain = BufferDesc(kStatsWords * 4);
    if (FAILED(g.device->CreateCommittedResource(&def, D3D12_HEAP_FLAG_NONE, &uavDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&s.buffer))) ||
        FAILED(g.device->CreateCommittedResource(&up, D3D12_HEAP_FLAG_NONE, &plain, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&s.zero))))
        return false;
    void* z = nullptr;
    if (FAILED(s.zero->Map(0, nullptr, &z)) || !z) return false;
    std::memset(z, 0, kStatsWords * 4);
    s.zero->Unmap(0, nullptr);
    for (unsigned i = 0; i < kRing; ++i) {
        void* m = nullptr;
        const D3D12_RANGE all{0, kStatsWords * 4};
        if (FAILED(g.device->CreateCommittedResource(&rb, D3D12_HEAP_FLAG_NONE, &plain, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&s.readback[i]))) ||
            FAILED(s.readback[i]->Map(0, &all, &m)) || !m)
            return false;
        s.mapped[i] = static_cast<const std::uint32_t*>(m); // readback heaps may stay mapped
    }
    s.state = D3D12_RESOURCE_STATE_COPY_DEST;
    return true;
}
bool InitTimer(TimerRing& t) noexcept {
    D3D12_QUERY_HEAP_DESC q{};
    q.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP; q.Count = kRing * 2;
    if (FAILED(g.device->CreateQueryHeap(&q, IID_PPV_ARGS(&t.heap)))) return false;
    auto rb = HeapProps(D3D12_HEAP_TYPE_READBACK);
    auto plain = BufferDesc(kRing * 2 * 8);
    void* m = nullptr;
    const D3D12_RANGE all{0, kRing * 2 * 8};
    if (FAILED(g.device->CreateCommittedResource(&rb, D3D12_HEAP_FLAG_NONE, &plain, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&t.readback))) ||
        FAILED(t.readback->Map(0, &all, &m)) || !m)
        return false;
    t.mapped = static_cast<const std::uint64_t*>(m);
    return true;
}

bool Compile(const char* define, ComPtr<ID3DBlob>& code) noexcept {
    const D3D_SHADER_MACRO evalMacros[] = {{"EVAL", "1"}, {define, "1"}, {nullptr, nullptr}};
    const D3D_SHADER_MACRO warpMacros[] = {{define, "1"}, {nullptr, nullptr}};
    const bool eval = std::strcmp(define, "PASS_CLEAR_CROSS") == 0 || std::strcmp(define, "PASS_LIVE") == 0;
    ComPtr<ID3DBlob> errors;
    const HRESULT hr = D3DCompile(kHlsl, std::strlen(kHlsl), "KCD2VR_R154_AER_WARP", eval ? evalMacros : warpMacros, nullptr,
                                  "main", "cs_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &code, &errors);
    if (SUCCEEDED(hr) && code) return true;
    char line[640]{};
    std::snprintf(line, sizeof(line), "R154 AER_WARP shader=%s compile=FAILED hr=0x%08X %.480s", define, static_cast<unsigned>(hr),
                  errors ? static_cast<const char*>(errors->GetBufferPointer()) : "");
    diagnostic_trace::WriteStatus(line);
    return false;
}

bool Init(ID3D12Device* device) noexcept {
    if (g.broken || !device) return false;
    if (g.device) return g.device.Get() == device;
    g.device = device;
    D3D12_DESCRIPTOR_RANGE ranges[2]{};
    ranges[0] = {D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 0, 0, 0};
    ranges[1] = {D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 4, 0, 0, 0};
    D3D12_ROOT_PARAMETER params[3]{};
    params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    params[0].Constants = {0, 0, 48};
    params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    params[1].DescriptorTable = {1, &ranges[0]};
    params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    params[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    params[2].DescriptorTable = {1, &ranges[1]};
    params[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    const D3D12_ROOT_SIGNATURE_DESC rootDesc{3, params, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE};
    ComPtr<ID3DBlob> bytes, errors;
    if (FAILED(D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, &bytes, &errors)) || !bytes ||
        FAILED(device->CreateRootSignature(0, bytes->GetBufferPointer(), bytes->GetBufferSize(), IID_PPV_ARGS(&g.root)))) {
        Break("root-signature"); return false;
    }
    struct P { const char* define; ComPtr<ID3D12PipelineState>* out; };
    const P passes[] = {{"PASS_CLEAR_CROSS", &g.clearCross}, {"PASS_LIVE", &g.live}, {"PASS_CLEAR", &g.clear},
                        {"PASS_SPLAT_KEY", &g.splatKey}, {"PASS_SPLAT_OFF", &g.splatOff}, {"PASS_RESOLVE", &g.resolve}};
    for (const auto& p : passes) {
        ComPtr<ID3DBlob> code;
        if (!Compile(p.define, code)) { Break("shader"); return false; }
        D3D12_COMPUTE_PIPELINE_STATE_DESC pso{};
        pso.pRootSignature = g.root.Get();
        pso.CS = {code->GetBufferPointer(), code->GetBufferSize()};
        if (FAILED(device->CreateComputePipelineState(&pso, IID_PPV_ARGS(p.out->ReleaseAndGetAddressOf())))) { Break("pso"); return false; }
    }
    D3D12_DESCRIPTOR_HEAP_DESC hd{};
    hd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    hd.NumDescriptors = kSetSize * kSets;
    hd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (FAILED(device->CreateDescriptorHeap(&hd, IID_PPV_ARGS(&g.heap)))) { Break("descriptor-heap"); return false; }
    g.inc = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    if (!InitStats(g.evalStats) || !InitStats(g.xrStats)) { Break("stats"); return false; }
    if (!InitTimer(g.evalTimer) || !InitTimer(g.xrTimer)) { Break("timer"); return false; }
    diagnostic_trace::WriteStatus("R154 AER_WARP gpu=READY tiers=ROTATION/6DOF/MOTION_VECTORS");
    return true;
}

void ReleaseAll() noexcept {
    // Called only after seconds without use: nothing queued references these.
    for (auto& e : g.depth) for (auto& t : e) t = Tex{};
    for (auto& t : g.own) t = Tex{};
    for (auto& t : g.cross) t = Tex{};
    g.key = Tex{}; g.off = Tex{}; g.offFinal = Tex{};
    for (auto& e : g.rec) for (auto& r : e) r = EyeRecord{};
    g.ownFrameValid[0] = g.ownFrameValid[1] = g.crossFrameValid[0] = g.crossFrameValid[1] = false;
    g.cacheValid = false;
    FreeRetired(true);
}

// ------------------------------------------------------------------ descriptors and barriers
struct Set { D3D12_CPU_DESCRIPTOR_HANDLE cpu{}; D3D12_GPU_DESCRIPTOR_HANDLE gpu{}; };
Set NextSet() noexcept {
    const unsigned slot = g.cursor++ % kSets;
    Set s{g.heap->GetCPUDescriptorHandleForHeapStart(), g.heap->GetGPUDescriptorHandleForHeapStart()};
    s.cpu.ptr += static_cast<SIZE_T>(slot) * kSetSize * g.inc;
    s.gpu.ptr += static_cast<UINT64>(slot) * kSetSize * g.inc;
    return s;
}
D3D12_CPU_DESCRIPTOR_HANDLE At(const Set& s, unsigned i) noexcept { auto h = s.cpu; h.ptr += static_cast<SIZE_T>(i) * g.inc; return h; }
D3D12_GPU_DESCRIPTOR_HANDLE AtGpu(const Set& s, unsigned i) noexcept { auto h = s.gpu; h.ptr += static_cast<UINT64>(i) * g.inc; return h; }
void Srv(ID3D12Resource* r, DXGI_FORMAT f, D3D12_CPU_DESCRIPTOR_HANDLE at) noexcept {
    D3D12_SHADER_RESOURCE_VIEW_DESC d{};
    d.Format = f; d.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    d.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; d.Texture2D.MipLevels = 1;
    g.device->CreateShaderResourceView(r, &d, at);   // r == nullptr: null descriptor
}
void Uav(ID3D12Resource* r, DXGI_FORMAT f, D3D12_CPU_DESCRIPTOR_HANDLE at) noexcept {
    D3D12_UNORDERED_ACCESS_VIEW_DESC d{};
    d.Format = f; d.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    g.device->CreateUnorderedAccessView(r, nullptr, &d, at);
}
void StatsUav(ID3D12Resource* r, D3D12_CPU_DESCRIPTOR_HANDLE at) noexcept {
    D3D12_UNORDERED_ACCESS_VIEW_DESC d{};
    d.Format = DXGI_FORMAT_UNKNOWN; d.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    d.Buffer.NumElements = kStatsWords; d.Buffer.StructureByteStride = 4;
    g.device->CreateUnorderedAccessView(r, nullptr, &d, at);
}

// Every barrier of this file bypasses DeviceHook's ResourceBarrier detour. Our
// textures rest in COMMON between lists, and COMMON == PRESENT: recorded on the
// game's list through the detour, each return to COMMON was taken as the game
// presenting that texture, and the headset showed this pass's R32 depth (red).
void Barrier(ID3D12GraphicsCommandList* list, UINT n, const D3D12_RESOURCE_BARRIER* b) noexcept {
    DeviceHook::ResourceBarrierBypass(list, n, b);
}

struct Barriers {
    D3D12_RESOURCE_BARRIER b[12]{};
    UINT n{};
    void To(Tex& t, D3D12_RESOURCE_STATES s) noexcept {
        if (!t.res || t.state == s || n >= 12) return;
        auto& x = b[n++];
        x.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        x.Transition.pResource = t.res.Get();
        x.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        x.Transition.StateBefore = t.state; x.Transition.StateAfter = s;
        t.state = s;
    }
    void Raw(ID3D12Resource* r, D3D12_RESOURCE_STATES a, D3D12_RESOURCE_STATES s) noexcept {
        if (!r || a == s || n >= 12) return;
        auto& x = b[n++];
        x.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        x.Transition.pResource = r;
        x.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        x.Transition.StateBefore = a; x.Transition.StateAfter = s;
    }
    void Flush(ID3D12GraphicsCommandList* list) noexcept { if (n) Barrier(list, n, b); n = 0; }
};
void UavBarrier(ID3D12GraphicsCommandList* list) noexcept {
    D3D12_RESOURCE_BARRIER b{};
    b.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    b.UAV.pResource = nullptr;
    Barrier(list, 1, &b);
}

void StatsBegin(ID3D12GraphicsCommandList* list, StatsRing& s) noexcept {
    // The readback for this ring slot was copied kRing uses ago: consume it first.
    const unsigned slot = s.cursor % kRing;
    if (s.ticket[slot]) {
        std::uint32_t v[kStatsWords]{};
        std::memcpy(v, s.mapped[slot], sizeof(v));
        if (&s == &g.evalStats) for (int i = 0; i < 8; ++i) g.c.live[i] += v[i];
        else for (int i = 0; i < 8; ++i) g.c.tiers[i] += v[8 + i];
        s.ticket[slot] = 0;
    }
    // COPY_DEST between command lists (StatsEnd returns it there).
    list->CopyBufferRegion(s.buffer.Get(), 0, s.zero.Get(), 0, kStatsWords * 4);
    D3D12_RESOURCE_BARRIER b{};
    b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; b.Transition.pResource = s.buffer.Get();
    b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    b.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST; b.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    Barrier(list, 1, &b);
    s.state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
}
void StatsEnd(ID3D12GraphicsCommandList* list, StatsRing& s, std::uint64_t use) noexcept {
    const unsigned slot = s.cursor++ % kRing;
    D3D12_RESOURCE_BARRIER b{};
    b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; b.Transition.pResource = s.buffer.Get();
    b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    b.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS; b.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
    Barrier(list, 1, &b);
    list->CopyBufferRegion(s.readback[slot].Get(), 0, s.buffer.Get(), 0, kStatsWords * 4);
    b.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE; b.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    Barrier(list, 1, &b);
    s.state = D3D12_RESOURCE_STATE_COPY_DEST;
    s.ticket[slot] = use;
}
void TimerBegin(ID3D12GraphicsCommandList* list, TimerRing& t) noexcept {
    const unsigned slot = t.cursor % kRing;
    if (t.ticket[slot]) {
        const std::uint64_t a = t.mapped[slot * 2], b = t.mapped[slot * 2 + 1];
        const auto f = g_ts_freq.load(std::memory_order_relaxed);
        if (f && b > a && b - a < f) { t.sumMs += double(b - a) * 1000.0 / double(f); ++t.samples; }
        t.ticket[slot] = 0;
    }
    list->EndQuery(t.heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, slot * 2);
    t.active = slot;
}
void TimerEnd(ID3D12GraphicsCommandList* list, TimerRing& t, std::uint64_t use) noexcept {
    if (t.active >= kRing) return;
    const unsigned slot = t.active;
    list->EndQuery(t.heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, slot * 2 + 1);
    list->ResolveQueryData(t.heap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, slot * 2, 2, t.readback.Get(), slot * 16);
    t.ticket[slot] = use;
    ++t.cursor;
    t.active = kRing;
}

void Bind(ID3D12GraphicsCommandList* list, const Set& set, const Constants& c) noexcept {
    ID3D12DescriptorHeap* heaps[] = {g.heap.Get()};
    list->SetDescriptorHeaps(1, heaps);
    list->SetComputeRootSignature(g.root.Get());
    list->SetComputeRoot32BitConstants(0, 48, &c, 0);
    list->SetComputeRootDescriptorTable(1, AtGpu(set, 0));
    list->SetComputeRootDescriptorTable(2, AtGpu(set, 5));
}

// ------------------------------------------------------------------ live eye
void Evaluate(ID3D12GraphicsCommandList* list, const LiveEyeInputs& in, int mode) noexcept {
    const unsigned e = in.eye & 1u, o = e ^ 1u;
    // This frame replaces the older of this eye's two records.
    const unsigned slot = g.rec[e][g.latest[e]].valid ? (g.latest[e] ^ 1u) : g.latest[e];
    g.rec[e][slot] = EyeRecord{};
    if (!in.camerasOk || !in.depth || !in.motion || in.renderW < 16 || in.renderH < 16) { ++g.c.evalSkipped; return; }
    if (!in.modelValid || in.depthMode < 0 || in.depthMode > 3) { ++g.c.evalSkipped; ++g.c.fail[FModel]; return; }
    const unsigned W = in.renderW, H = in.renderH;
    const bool wantMotion = mode >= kMotionVectors;   // 6DoF needs only the depth
    if (!MakeTex(g.depth[e][slot], W, H, DXGI_FORMAT_R32_FLOAT) ||
        (wantMotion && (!MakeTex(g.own[e], W, H, DXGI_FORMAT_R16G16B16A16_FLOAT) ||
                        !MakeTex(g.cross[e], W, H, DXGI_FORMAT_R16G16B16A16_FLOAT)))) {
        ++g.c.fail[FTextures]; Break("textures"); return;
    }
    // The other eye's newest record must be the frame just before, with the same
    // grid, and its camera must be the previous bank the game's vectors refer to.
    const EyeRecord& s = g.rec[o][g.latest[o]];
    bool haveStale = s.valid && s.renderFrame + 1u == in.renderFrame && s.w == W && s.h == H && g.depth[o][s.depthSlot].res;
    if (!haveStale) ++g.c.evalNoStale;
    if (haveStale && (DistM(s.cam, in.prev) > 0.01 || AngleDeg(s.cam, in.prev) > 0.05)) { haveStale = false; ++g.c.evalPrevMismatch; }
    const bool motion = wantMotion && haveStale && g.own[e].res && g.cross[e].res;
    const bool cross = motion && in.eyeOffsetKnown;
    if (motion && !in.eyeOffsetKnown) ++g.c.evalNoOffset;

    Constants c{};
    c.g0[0] = W; c.g0[1] = H; c.g0[2] = static_cast<std::uint32_t>(in.depthMode);
    c.g0[3] = (motion ? 1u : 0u) | (cross ? 2u : 0u) | (motion ? 4u : 0u);
    c.g1[0] = in.depthX; c.g1[1] = in.depthY; c.g1[2] = in.mvX; c.g1[3] = in.mvY;
    c.g2[0] = in.mvW ? in.mvW : W; c.g2[1] = in.mvH ? in.mvH : H; c.g2[2] = W; c.g2[3] = H;
    c.f0[0] = in.mvScaleX; c.f0[1] = in.mvScaleY; c.f0[2] = in.cur.tanX; c.f0[3] = in.cur.tanY;
    c.f1[0] = in.cur.nearZ; c.f1[1] = in.cur.farZ; c.f1[2] = in.prev.tanX; c.f1[3] = in.prev.tanY;
    c.f2[0] = 2.0f * in.eyeOffsetWorld; c.f2[1] = 0.15f; c.f2[2] = 0.75f; c.f2[3] = 0.08f;
    RelativeRows(in.cur, in.prev, c.a);   // current -> previous
    RelativeRows(in.prev, in.cur, c.b);   // previous -> current

    const Set set = NextSet();
    Srv(in.motion, in.motionView, At(set, 0));
    Srv(in.depth, in.depthView, At(set, 1));
    Srv(haveStale ? g.depth[o][s.depthSlot].res.Get() : nullptr, DXGI_FORMAT_R32_FLOAT, At(set, 2));
    Srv(nullptr, DXGI_FORMAT_R32_UINT, At(set, 3));
    Srv(nullptr, DXGI_FORMAT_R32G32_FLOAT, At(set, 4));
    Uav(g.depth[e][slot].res.Get(), DXGI_FORMAT_R32_FLOAT, At(set, 5));
    Uav(motion ? g.own[e].res.Get() : nullptr, DXGI_FORMAT_R16G16B16A16_FLOAT, At(set, 6));
    Uav(cross ? g.cross[e].res.Get() : nullptr, DXGI_FORMAT_R16G16B16A16_FLOAT, At(set, 7));
    StatsUav(g.evalStats.buffer.Get(), At(set, 8));

    const auto use = ++g.evalUses;
    // Every resource of ours is COMMON between command lists: the game's lists and
    // the XR blit list record independently, so no state is carried across them.
    Barriers b;
    b.To(g.depth[e][slot], D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    if (motion) b.To(g.own[e], D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    if (cross) b.To(g.cross[e], D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    if (haveStale) b.To(g.depth[o][s.depthSlot], D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    b.Flush(list);
    StatsBegin(list, g.evalStats);
    TimerBegin(list, g.evalTimer);
    Bind(list, set, c);
    if (cross) {
        list->SetPipelineState(g.clearCross.Get());
        list->Dispatch((W + 7) / 8, (H + 7) / 8, 1);
        UavBarrier(list);
    }
    list->SetPipelineState(g.live.Get());
    list->Dispatch((W + 7) / 8, (H + 7) / 8, 1);
    TimerEnd(list, g.evalTimer, use);
    StatsEnd(list, g.evalStats, use);
    b.To(g.depth[e][slot], D3D12_RESOURCE_STATE_COMMON);
    b.To(g.own[e], D3D12_RESOURCE_STATE_COMMON);
    b.To(g.cross[e], D3D12_RESOURCE_STATE_COMMON);
    if (haveStale) b.To(g.depth[o][s.depthSlot], D3D12_RESOURCE_STATE_COMMON);
    b.Flush(list);

    EyeRecord& r = g.rec[e][slot];
    r.valid = true; r.renderFrame = in.renderFrame; r.poseFrame = in.poseFrame; r.cam = in.cur;
    r.eyeOffsetWorld = in.eyeOffsetWorld; r.offsetKnown = in.eyeOffsetKnown;
    r.w = W; r.h = H; r.depthSlot = slot;
    if (motion) {
        // own[e] / cross[e] now belong to this frame; the older record loses them.
        g.rec[e][slot ^ 1u].ownValid = false; g.rec[e][slot ^ 1u].crossValid = false;
        r.ownValid = true; g.ownFrame[e] = in.renderFrame; g.ownFrameValid[e] = true;
        if (cross) {
            r.crossValid = true; r.crossStalePose = s.poseFrame; r.crossStaleFrame = s.renderFrame;
            g.crossFrame[e] = in.renderFrame; g.crossFrameValid[e] = true;
        }
        ++g.c.evalMotion;
    }
    g.latest[e] = slot;
    ++g.c.evaluations;
}

// ------------------------------------------------------------------ stale eye
const EyeRecord* FindRecord(unsigned eye, std::uint64_t pose) noexcept {
    if (!pose) return nullptr;
    for (const auto& r : g.rec[eye & 1u]) if (r.valid && r.poseFrame == pose) return &r;
    return nullptr;
}

bool Warp(ID3D12GraphicsCommandList* list, const StaleWarpRequest& req, StaleWarpResult& out, int mode) noexcept {
    const unsigned se = static_cast<unsigned>(req.staleEye) & 1u, le = se ^ 1u;
    const EyeRecord* s = FindRecord(se, req.stalePoseFrame);
    if (!s) { out.reason = "no-stale-record"; ++g.c.fail[FNoStale]; return false; }
    const EyeRecord* l = FindRecord(le, req.livePoseFrame);
    if (!l) { out.reason = "no-live-record"; ++g.c.fail[FNoLive]; return false; }
    if (!l->offsetKnown) { out.reason = "eye-offset"; ++g.c.fail[FEyeOffset]; return false; }
    if (s->w != l->w || s->h != l->h) { out.reason = "size"; ++g.c.fail[FSize]; return false; }
    // The stale eye's target: the live camera with the eye separation reversed.
    Camera t = l->cam;
    const double right[3]{l->cam.rot[0], l->cam.rot[3], l->cam.rot[6]};
    for (int i = 0; i < 3; ++i) t.pos[i] -= 2.0 * double(l->eyeOffsetWorld) * right[i];
    // A cut, teleport or load: nothing in the stale image is worth moving.
    if (DistM(s->cam, t) > 1.5 || AngleDeg(s->cam, t) > 35.0) { out.reason = "camera-jump"; ++g.c.fail[FJump]; return false; }
    const bool haveCross = mode >= kMotionVectors && l->crossValid && l->crossStalePose == s->poseFrame &&
        g.crossFrameValid[le] && g.crossFrame[le] == l->renderFrame;
    const bool haveOwn = mode >= kMotionVectors && s->ownValid && g.ownFrameValid[se] && g.ownFrame[se] == s->renderFrame;
    const bool tint = g_tint.load(std::memory_order_acquire);
    const unsigned W = s->w, H = s->h;
    const std::uint32_t flags = (haveCross ? 1u : 0u) | (haveOwn ? 2u : 0u) | (tint ? 8u : 0u) | (static_cast<std::uint32_t>(mode) << 4);

    out.tierMode = mode;
    // A re-presented frame (the game slower than the headset) reuses the map.
    if (g.cacheValid && g.cacheLive == req.livePoseFrame && g.cacheStale == req.stalePoseFrame && g.cacheFlags == flags &&
        g.offFinal.res && g.offFinal.w == W && g.offFinal.h == H && g.offFinal.state == D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) {
        out.offsets = g.offFinal.res.Get(); out.reason = "reused"; ++g.c.warpReused; return true;
    }
    g.cacheValid = false;
    if (!MakeTex(g.key, W, H, DXGI_FORMAT_R32_UINT) || !MakeTex(g.off, W, H, DXGI_FORMAT_R32G32_FLOAT) ||
        !MakeTex(g.offFinal, W, H, DXGI_FORMAT_R32G32B32A32_FLOAT)) {
        out.reason = "textures"; ++g.c.fail[FTextures]; Break("textures"); return false;
    }
    Tex& sd = g.depth[se][s->depthSlot];
    if (!sd.res) { out.reason = "no-stale-record"; ++g.c.fail[FNoStale]; return false; }

    Constants c{};
    c.g0[0] = W; c.g0[1] = H; c.g0[2] = (haveCross ? 1u : 0u) | (haveOwn ? 2u : 0u); c.g0[3] = 12u;
    c.g1[0] = 4u; c.g1[1] = tint ? 1u : 0u;
    c.f0[0] = s->cam.tanX; c.f0[1] = s->cam.tanY; c.f0[2] = t.tanX; c.f0[3] = t.tanY;
    c.f1[0] = std::max(1.0e-3f, s->cam.nearZ * 0.5f); c.f1[1] = 0.03f; c.f1[2] = std::max(1000.0f, s->cam.farZ); c.f1[3] = 0.0f;
    RelativeRows(s->cam, t, c.a);   // stale -> target
    RelativeRows(t, s->cam, c.b);   // target -> stale

    const Set splatSet = NextSet();
    Srv(sd.res.Get(), DXGI_FORMAT_R32_FLOAT, At(splatSet, 0));
    Srv(haveCross ? g.cross[le].res.Get() : nullptr, DXGI_FORMAT_R16G16B16A16_FLOAT, At(splatSet, 1));
    Srv(haveOwn ? g.own[se].res.Get() : nullptr, DXGI_FORMAT_R16G16B16A16_FLOAT, At(splatSet, 2));
    Srv(nullptr, DXGI_FORMAT_R32_UINT, At(splatSet, 3));
    Srv(nullptr, DXGI_FORMAT_R32G32_FLOAT, At(splatSet, 4));
    Uav(g.key.res.Get(), DXGI_FORMAT_R32_UINT, At(splatSet, 5));
    Uav(g.off.res.Get(), DXGI_FORMAT_R32G32_FLOAT, At(splatSet, 6));
    Uav(nullptr, DXGI_FORMAT_R32G32B32A32_FLOAT, At(splatSet, 7));
    StatsUav(g.xrStats.buffer.Get(), At(splatSet, 8));
    const Set resolveSet = NextSet();
    Srv(sd.res.Get(), DXGI_FORMAT_R32_FLOAT, At(resolveSet, 0));
    Srv(haveCross ? g.cross[le].res.Get() : nullptr, DXGI_FORMAT_R16G16B16A16_FLOAT, At(resolveSet, 1));
    Srv(haveOwn ? g.own[se].res.Get() : nullptr, DXGI_FORMAT_R16G16B16A16_FLOAT, At(resolveSet, 2));
    Srv(g.key.res.Get(), DXGI_FORMAT_R32_UINT, At(resolveSet, 3));
    Srv(g.off.res.Get(), DXGI_FORMAT_R32G32_FLOAT, At(resolveSet, 4));
    Uav(nullptr, DXGI_FORMAT_R32_UINT, At(resolveSet, 5));
    Uav(nullptr, DXGI_FORMAT_R32G32_FLOAT, At(resolveSet, 6));
    Uav(g.offFinal.res.Get(), DXGI_FORMAT_R32G32B32A32_FLOAT, At(resolveSet, 7));
    StatsUav(g.xrStats.buffer.Get(), At(resolveSet, 8));

    const auto use = ++g.xrUses;
    Barriers b;
    b.To(sd, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    if (haveCross) b.To(g.cross[le], D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    if (haveOwn) b.To(g.own[se], D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    b.To(g.key, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    b.To(g.off, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    b.To(g.offFinal, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    b.Flush(list);
    StatsBegin(list, g.xrStats);
    TimerBegin(list, g.xrTimer);
    const UINT gx = (W + 7) / 8, gy = (H + 7) / 8;
    Bind(list, splatSet, c);
    list->SetPipelineState(g.clear.Get());    list->Dispatch(gx, gy, 1); UavBarrier(list);
    list->SetPipelineState(g.splatKey.Get()); list->Dispatch(gx, gy, 1); UavBarrier(list);
    list->SetPipelineState(g.splatOff.Get()); list->Dispatch(gx, gy, 1);
    b.To(g.key, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    b.To(g.off, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    b.Flush(list);
    list->SetComputeRootDescriptorTable(1, AtGpu(resolveSet, 0));
    list->SetComputeRootDescriptorTable(2, AtGpu(resolveSet, 5));
    list->SetPipelineState(g.resolve.Get());  list->Dispatch(gx, gy, 1);
    TimerEnd(list, g.xrTimer, use);
    StatsEnd(list, g.xrStats, use);
    b.To(g.offFinal, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    b.To(sd, D3D12_RESOURCE_STATE_COMMON);
    if (haveCross) b.To(g.cross[le], D3D12_RESOURCE_STATE_COMMON);
    if (haveOwn) b.To(g.own[se], D3D12_RESOURCE_STATE_COMMON);
    b.To(g.key, D3D12_RESOURCE_STATE_COMMON);
    b.To(g.off, D3D12_RESOURCE_STATE_COMMON);
    b.Flush(list);

    g.cacheValid = true; g.cacheLive = req.livePoseFrame; g.cacheStale = req.stalePoseFrame; g.cacheFlags = flags;
    out.offsets = g.offFinal.res.Get();
    out.reason = haveCross ? "motion-vectors" : haveOwn ? "own-motion" : "6dof";
    ++g.c.warps;
    if (haveCross) ++g.c.warpCross;
    if (haveOwn) ++g.c.warpOwn;
    return true;
}

void LogLocked(std::uint64_t now, bool force) noexcept {
    if (!g.lastLog) { g.lastLog = now; return; }
    if (!force && now - g.lastLog < 30000) return;
    g.lastLog = now;
    auto& c = g.c;
    const double evalMs = g.evalTimer.samples ? g.evalTimer.sumMs / double(g.evalTimer.samples) : -1.0;
    const double warpMs = g.xrTimer.samples ? g.xrTimer.sumMs / double(g.xrTimer.samples) : -1.0;
    const double tierTotal = double(c.tiers[1] + c.tiers[2] + c.tiers[3] + c.tiers[4] + c.tiers[5] + c.tiers[6] + c.tiers[7]);
    auto pct = [&](std::uint64_t v) { return tierTotal > 0.0 ? 100.0 * double(v) / tierTotal : 0.0; };
    char fails[256]{};
    int at = 0;
    for (int i = 0; i < 12 && at < int(sizeof(fails)) - 32; ++i)
        if (c.fail[i]) at += std::snprintf(fails + at, sizeof(fails) - at, "%s%s:%llu", at ? "," : "", kFailNames[i], (unsigned long long)c.fail[i]);
    char line[960]{};
    std::snprintf(line, sizeof(line),
        "R154 AER_WARP mode=%s 30s evals=%llu motionEvals=%llu noStale=%llu prevMismatch=%llu noEyeOffset=%llu skipped=%llu "
        "warps=%llu reused=%llu withMV=%llu withOwn=%llu fallbacks={%s} gpuMs{live:%.3f warp:%.3f} "
        "livePx{moving:%llu matched:%llu occluded:%llu offscreen:%llu} tiers%%{static:%.1f own:%.1f mv:%.1f crack:%.1f holeBg:%.1f hole6dof:%.1f rotation:%.1f} gpu=%s",
        Mode() == kMotionVectors ? "MOTION_VECTORS" : Mode() == kSixDof ? "6DOF" : "ROTATION",
        (unsigned long long)c.evaluations, (unsigned long long)c.evalMotion, (unsigned long long)c.evalNoStale,
        (unsigned long long)c.evalPrevMismatch, (unsigned long long)c.evalNoOffset, (unsigned long long)c.evalSkipped,
        (unsigned long long)c.warps, (unsigned long long)c.warpReused, (unsigned long long)c.warpCross, (unsigned long long)c.warpOwn,
        at ? fails : "none", evalMs, warpMs,
        (unsigned long long)c.live[0], (unsigned long long)c.live[1], (unsigned long long)c.live[2], (unsigned long long)c.live[3],
        pct(c.tiers[1]), pct(c.tiers[2]), pct(c.tiers[3]), pct(c.tiers[4]), pct(c.tiers[5]), pct(c.tiers[6]), pct(c.tiers[7]),
        g.broken ? g.brokenReason : "ok");
    diagnostic_trace::WriteStatus(line);
    c = Counters{};
    g.evalTimer.sumMs = g.xrTimer.sumMs = 0.0;
    g.evalTimer.samples = g.xrTimer.samples = 0;
}
} // namespace

void SetMode(int mode) noexcept {
    mode = std::clamp(mode, 0, 2);
    if (g_mode.exchange(mode, std::memory_order_acq_rel) != mode) {
        char line[96]{};
        std::snprintf(line, sizeof(line), "R154 AER_WARP mode=%s", mode == 2 ? "MOTION_VECTORS" : mode == 1 ? "6DOF" : "ROTATION");
        diagnostic_trace::WriteStatus(line);
    }
}
int Mode() noexcept { return g_mode.load(std::memory_order_acquire); }
void SetDebugTint(bool on) noexcept { g_tint.store(on, std::memory_order_release); }
bool DebugTint() noexcept { return g_tint.load(std::memory_order_acquire); }
void SetTimestampFrequency(std::uint64_t hz) noexcept { if (hz) g_ts_freq.store(hz, std::memory_order_release); }

void OnLiveEyeEvaluate(ID3D12GraphicsCommandList* list, const LiveEyeInputs& in) noexcept {
    const int mode = Mode();
    if (mode < kSixDof || !list) return;
    std::lock_guard<std::mutex> lock(g.mutex);
    if (g.broken) return;
    ID3D12Device* raw = nullptr;
    if (FAILED(list->GetDevice(IID_PPV_ARGS(&raw))) || !raw) return;
    ComPtr<ID3D12Device> device;
    device.Attach(raw);
    if (!Init(device.Get())) return;
    FreeRetired(false);
    g.lastUse = static_cast<std::uint64_t>(GetTickCount64());
    Evaluate(list, in, mode);
}

bool WarpStaleEye(ID3D12GraphicsCommandList* list, const StaleWarpRequest& req, StaleWarpResult& out) noexcept {
    out = StaleWarpResult{};
    const int mode = Mode();
    if (mode < kSixDof) { out.reason = "mode"; return false; }
    if (!list) { out.reason = "no-request"; return false; }
    std::lock_guard<std::mutex> lock(g.mutex);
    if (g.broken || !g.device) { out.reason = "gpu"; ++g.c.fail[FGpu]; return false; }
    g.lastUse = static_cast<std::uint64_t>(GetTickCount64());
    return Warp(list, req, out, mode);
}

void Tick() noexcept {
    std::unique_lock<std::mutex> lock(g.mutex, std::try_to_lock);
    if (!lock.owns_lock()) return;
    const auto now = static_cast<std::uint64_t>(GetTickCount64());
    if (!g.lastUse) return;
    // GPU memory is only held while the warp runs.
    if (now - g.lastUse > 5000) {
        if (g.key.res || g.depth[0][0].res || g.own[0].res) {
            LogLocked(now, true);
            ReleaseAll();
            diagnostic_trace::Write("R154 AER_WARP released (not running)");
        }
        return;
    }
    LogLocked(now, false);
}
}
