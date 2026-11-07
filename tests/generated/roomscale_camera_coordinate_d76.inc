// Generated from camera_hook.cpp by run_roomscale_camera_coordinate_d76.py
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

static void CopyCurrentPoseAndOrigin(std::mutex& mutex, const XrPose& src_pose, const XrPose& src_origin,
                                      std::atomic<float>& scale_atomic,
                                      XrPose& out_pose, XrPose& out_origin, float& out_scale,
                                      const std::atomic<std::uint64_t>* frame_source=nullptr,
                                      std::uint64_t* out_frame=nullptr,
                                      const hj1::Sample* family_source=nullptr,hj1::Sample* out_family=nullptr,
                                      const std::atomic<std::uint64_t>* generation_source=nullptr,std::uint64_t* out_generation=nullptr,
                                      native_roomscale_d76::Origin* out_room_origin=nullptr,std::uint64_t* out_room_revision=nullptr,
                                      const std::uint64_t* source_epoch=nullptr,std::uint64_t* out_source_epoch=nullptr)
{
    std::lock_guard<std::mutex> lock(mutex);
    out_pose = src_pose;
    out_origin = src_origin;
    if(out_room_origin)(void)native_roomscale_d76::ReadTrackingOriginD76(*out_room_origin);
    if(out_room_revision)*out_room_revision=roomscale_d76::originRevision.load(std::memory_order_acquire);
    out_scale = scale_atomic.load(std::memory_order_relaxed);
    if(frame_source&&out_frame)*out_frame=frame_source->load(std::memory_order_relaxed);
    if(family_source&&out_family)*out_family=*family_source;
    if(generation_source&&out_generation)*out_generation=generation_source->load(std::memory_order_relaxed);
    if(source_epoch&&out_source_epoch)*out_source_epoch=*source_epoch;
}


nr::Origin SelectTrackedOrigin(nr::Origin canonicalOriginD76,std::uint64_t roomRevisionD76,bool&trackedBase48){nr::Origin roomOriginD76=canonicalOriginD76;const bool dialogueEye59c10r2=false,activityEye59c19=false;Vec3f base_pos{},base_forward{0,1,0},base_right{1,0,0},base_up{0,0,1};
        float frameX51=0,frameY51=0,frameZ51=0,frameHx55=0,frameHy55=0;
        bool independentBowBasis64=false;native_roomscale_d76::Origin trackedOriginD76{};
        if(!dialogueEye59c10r2&&!activityEye59c19&&s_view_marriage_mode.load()==0&&
           !s_native_head_rotation_owner.load()&&frik_pose_probe::TrackedFrameCamera55(base_pos.x,base_pos.y,base_pos.z,
                base_forward.x,base_forward.y,base_forward.z,frameX51,frameY51,frameZ51,frameHx55,frameHy55,&trackedOriginD76)&&
           roomscale_d76::compatible(canonicalOriginD76,trackedOriginD76,roomRevisionD76)){
            trackedBase48=true;roomOriginD76=trackedOriginD76;
            base_pos={frameX51,frameY51,frameZ51};
            // Only a current exact bow frame with a positively neutral local
            // heading qualifies this. Native aim yaw must not rotate the view;
            // real character-world turns and the fresh HMD delta remain live.
            const float headingLengthSquared=frameHx55*frameHx55+frameHy55*frameHy55;
            if(std::isfinite(headingLengthSquared)&&headingLengthSquared>.5f){
                const float invHeadingLength=1.f/std::sqrt(headingLengthSquared);
                base_forward={frameHx55*invHeadingLength,frameHy55*invHeadingLength,0};
                // Cry camera columns are right/forward/up. Level ALL axes:
                // right = forward x world-up; keeping animated right/up here
                // made this foundation a shear for raw-basis consumers.
                base_right={base_forward.y,-base_forward.x,0};
                base_up={0,0,1};
                independentBowBasis64=true;
            }
        }
        if(roomOriginD76.owner.epoch&&roomOriginD76.owner.epoch!=roomRevisionD76)roomOriginD76=canonicalOriginD76;
return roomOriginD76;}

bool OldCamera(Matrix34f* matrix,XrPose pose,XrPose origin,Vec3f base_pos,Quatf origin_q,const Vec3f* visual_basis,float world_scale=1,bool record_render_identity=false){
const Vec3f base_right=visual_basis[0],base_forward=visual_basis[1],base_up=visual_basis[2];float displayCenterD75[3]{};
const bool apply_position=s_apply_position.load(),hmd_final_orientation_authority=true;const int marriage_mode=0;const std::uint64_t pose_frame=1;
kcd2vr::native_roomscale_d76::Origin roomOriginD76{};kcd2vr::native_roomscale_d76::ReadTrackingOriginD76(roomOriginD76);
    {
        // V60.8: tracked translation and fixed camera calibration are separate.
        // The fixed offset is intentionally applied even when positional tracking is disabled,
        // so it can test whether Henry/near-body scale is fundamentally a camera-distance issue.
        Vec3f cry_local{0.0f, 0.0f, 0.0f};
        if (apply_position)
        {
            const auto residualD76=roomscale_d76::residual({pose.pos_x-origin.pos_x,
                pose.pos_y-origin.pos_y,pose.pos_z-origin.pos_z},roomOriginD76);
            const Vec3f xr_delta_world{residualD76.x,residualD76.y,residualD76.z};
            const Vec3f xr_delta = Rotate(Conjugate(Normalize(origin_q)), xr_delta_world);
            cry_local = {xr_delta.x, -xr_delta.z, xr_delta.y};
        }
        cry_local.x += g_recenter_anchor_x.load(std::memory_order_acquire);
        cry_local.y += g_recenter_anchor_y.load(std::memory_order_acquire);
        cry_local.z += g_recenter_anchor_z.load(std::memory_order_acquire);
        cry_local.x += CameraHook::GetCameraSideOffsetMeters();
        const float centerLocalXD75=cry_local.x;
        cry_local.x += CameraHook::GetAfrEyeOffsetMeters();
        // Cry local +Y is camera forward and +Z is up. Positive menu 'back' therefore subtracts Y.
        cry_local.y -= CameraHook::GetCameraBackOffsetMeters();
        cry_local.z += CameraHook::GetCameraHeightOffsetMeters();
        const Vec3f world_delta{
            base_right.x * cry_local.x + base_forward.x * cry_local.y + base_up.x * cry_local.z,
            base_right.y * cry_local.x + base_forward.y * cry_local.y + base_up.y * cry_local.z,
            base_right.z * cry_local.x + base_forward.z * cry_local.y + base_up.z * cry_local.z
        };
        matrix->m[0][3] = base_pos.x + world_delta.x * world_scale;
        matrix->m[1][3] = base_pos.y + world_delta.y * world_scale;
        matrix->m[2][3] = base_pos.z + world_delta.z * world_scale;
        // Display-only center before IPD, using D76's consumed-origin residual.
        // Keep every rendered position expression and eye-offset load intact.
        displayCenterD75[0]=base_pos.x+(base_right.x*centerLocalXD75+base_forward.x*cry_local.y+base_up.x*cry_local.z)*world_scale;
        displayCenterD75[1]=base_pos.y+(base_right.y*centerLocalXD75+base_forward.y*cry_local.y+base_up.y*cry_local.z)*world_scale;
        displayCenterD75[2]=base_pos.z+(base_right.z*centerLocalXD75+base_forward.z*cry_local.y+base_up.z*cry_local.z)*world_scale;
    }

for(int i=0;i<3;++i)lastNameCenterD76R1[i]=displayCenterD75[i];return true;
}

bool PublishedCamera(Matrix34f* matrix,XrPose pose,XrPose origin,Vec3f base_pos,Quatf origin_q,const Vec3f* visual_basis,float world_scale=1,bool record_render_identity=false){
const Vec3f base_right=visual_basis[0],base_forward=visual_basis[1],base_up=visual_basis[2];float displayCenterD75[3]{};
const bool apply_position=s_apply_position.load(),hmd_final_orientation_authority=true;const int marriage_mode=0;const std::uint64_t pose_frame=1;
kcd2vr::native_roomscale_d76::Origin roomOriginD76{};kcd2vr::native_roomscale_d76::ReadTrackingOriginD76(roomOriginD76);
        if(!s_apply_rotation.load(std::memory_order_relaxed) && !s_apply_position.load(std::memory_order_relaxed)){
            PublishVisualForwardFromCameraMatrix(matrix);PublishVisualPositionFromCameraMatrix(matrix);
            (void)g_visual_ray_d75.publish({}); // no tracked name ray in the native Theater path
            if(record_render_identity)s_last_applied_pose_frame_index.store(pose_frame,std::memory_order_release);
            return true;
        }

        {
            Vec3f cry_local{0.0f,0.0f,0.0f};
            if (s_apply_position.load(std::memory_order_relaxed)) {
                const auto residualD76=roomscale_d76::residual({pose.pos_x-origin.pos_x,
                    pose.pos_y-origin.pos_y,pose.pos_z-origin.pos_z},roomOriginD76);
                const Vec3f xr_world{residualD76.x,residualD76.y,residualD76.z};
                const Vec3f xr_delta=Rotate(Conjugate(Normalize(origin_q)),xr_world);
                cry_local={xr_delta.x,-xr_delta.z,xr_delta.y};
            }
            // V66.178R58: roomscale follow bookkeeping, once per rendered frame. The entity movement is the native
            // camera base delta projected on the camera-local axes; only its component toward the player is consumed.
            if(record_render_identity&&!roomscale_d76::Enabled()&&g_roomscale_enabled58.load(std::memory_order_acquire)&&s_apply_position.load(std::memory_order_relaxed)){
                const bool visual58=hmd_final_orientation_authority||marriage_mode>=2;
                RoomscaleBookkeeping58(base_pos,cry_local,visual58?visual_basis[0]:base_right,
                    visual58?visual_basis[1]:base_forward,world_scale);
            }
            cry_local.x += g_recenter_anchor_x.load(std::memory_order_acquire);
            cry_local.y += g_recenter_anchor_y.load(std::memory_order_acquire);
            cry_local.z += g_recenter_anchor_z.load(std::memory_order_acquire);
            cry_local.x += GetCameraSideOffsetMeters();
            const float centerLocalXD75=cry_local.x;
            cry_local.x += GetAfrEyeOffsetMeters();
            cry_local.y -= GetCameraBackOffsetMeters();
            cry_local.z += GetCameraHeightOffsetMeters();
            const bool use_visual_position_basis=hmd_final_orientation_authority||marriage_mode>=2;
            const Vec3f& pos_right=(use_visual_position_basis?visual_basis[0]:base_right);
            const Vec3f& pos_forward=(use_visual_position_basis?visual_basis[1]:base_forward);
            const Vec3f& pos_up=(use_visual_position_basis?visual_basis[2]:base_up);
            const Vec3f wd{
                pos_right.x*cry_local.x+pos_forward.x*cry_local.y+pos_up.x*cry_local.z,
                pos_right.y*cry_local.x+pos_forward.y*cry_local.y+pos_up.y*cry_local.z,
                pos_right.z*cry_local.x+pos_forward.z*cry_local.y+pos_up.z*cry_local.z};
            matrix->m[0][3]=base_pos.x+wd.x*world_scale;
            matrix->m[1][3]=base_pos.y+wd.y*world_scale;
            matrix->m[2][3]=base_pos.z+wd.z*world_scale;
            // Alternating eyes share one center-head focus; roomscale and
            // activity bases above remain the exact D76 render calculation.
            displayCenterD75[0]=base_pos.x+(pos_right.x*centerLocalXD75+pos_forward.x*cry_local.y+pos_up.x*cry_local.z)*world_scale;
            displayCenterD75[1]=base_pos.y+(pos_right.y*centerLocalXD75+pos_forward.y*cry_local.y+pos_up.y*cry_local.z)*world_scale;
            displayCenterD75[2]=base_pos.z+(pos_right.z*centerLocalXD75+pos_forward.z*cry_local.y+pos_up.z*cry_local.z)*world_scale;

        }
for(int i=0;i<3;++i)lastNameCenterD76R1[i]=displayCenterD75[i];return true;
}
