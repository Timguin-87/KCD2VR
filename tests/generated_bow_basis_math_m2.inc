// Actual camera source; regenerate with tools/extract_bow_basis_m2.py.
struct Vec3f { float x, y, z; };
struct Quatf { float x, y, z, w; };

static Quatf Normalize(Quatf q)
{
    const float len2 = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
    if (len2 <= 1e-12f) return {0,0,0,1};
    const float inv = 1.0f / std::sqrt(len2);
    return {q.x*inv, q.y*inv, q.z*inv, q.w*inv};
}

static Quatf Conjugate(Quatf q) { return {-q.x, -q.y, -q.z, q.w}; }

static Quatf Mul(Quatf a, Quatf b)
{
    return {
        a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
        a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
        a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w,
        a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z
    };
}

static Vec3f Rotate(Quatf q, Vec3f v)
{
    const Quatf p{v.x, v.y, v.z, 0.0f};
    const Quatf r = Mul(Mul(q, p), Conjugate(q));
    return {r.x, r.y, r.z};
}

static void XrQuatToCryMatrix(Quatf q, float out[3][3])
{
    q = Normalize(q);
    auto xr_rotate = [q](Vec3f v) { return Rotate(q, v); };
    auto cry_to_xr = [](Vec3f v) { return Vec3f{v.x, v.z, -v.y}; };
    auto xr_to_cry = [](Vec3f v) { return Vec3f{v.x, -v.z, v.y}; };

    const Vec3f cols[3] = {
        xr_to_cry(xr_rotate(cry_to_xr({1,0,0}))),
        xr_to_cry(xr_rotate(cry_to_xr({0,1,0}))),
        xr_to_cry(xr_rotate(cry_to_xr({0,0,1})))
    };
    for (int c = 0; c < 3; ++c) {
        out[0][c] = cols[c].x;
        out[1][c] = cols[c].y;
        out[2][c] = cols[c].z;
    }
}

