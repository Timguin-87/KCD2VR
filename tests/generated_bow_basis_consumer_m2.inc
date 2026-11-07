// Actual camera source; regenerate with tools/extract_bow_basis_m2.py.
        const bool activityEye59c19=ActivityEyeBase59c19(*matrix,record_render_identity,&roomOriginD76,roomRevisionD76);
        const bool dialogueEye59c10r2=DialogueEyeBase59c10r2(*matrix,record_render_identity,roomRevisionD76,&canonicalOriginD76);
        if(dialogueEye59c10r2)roomOriginD76=canonicalOriginD76;
        Vec3f base_right{matrix->m[0][0],matrix->m[1][0],matrix->m[2][0]};
        Vec3f base_forward{matrix->m[0][1],matrix->m[1][1],matrix->m[2][1]};
        { const float fl=std::sqrt(base_forward.x*base_forward.x+base_forward.y*base_forward.y+base_forward.z*base_forward.z);
          if(fl>1e-4f&&std::isfinite(fl)){ s_native_pitch161.store(std::asin(std::clamp(base_forward.z/fl,-1.0f,1.0f)),std::memory_order_relaxed);
            s_native_pitch_tick161.store(GetTickCount64(),std::memory_order_release);} } // V66.161
        Vec3f base_up{matrix->m[0][2],matrix->m[1][2],matrix->m[2][2]};
        Vec3f base_pos{matrix->m[0][3],matrix->m[1][3],matrix->m[2][3]};
        const Vec3f nativeEye48=base_pos;bool trackedBase48=false;
        // C51R1: ordinary play uses the SAME committed frame as body/hands.
        // C55 pins the exact drawn bow foundation and removes native aim yaw;
        // current entity root travel/turn and fresh HMD/IPD follow once.
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
        const Vec3f basis[3]={base_right,base_forward,base_up};

        const Quatf now_q{pose.qx,pose.qy,pose.qz,pose.qw};
        const Quatf origin_q{origin.qx,origin.qy,origin.qz,origin.qw};
        Quatf delta_xr=Normalize(Mul(Conjugate(Normalize(origin_q)),Normalize(now_q)));
        delta_xr=ApplyV55RotationFilter(delta_xr,s_rotation_filter_mode.load(std::memory_order_acquire),s_rotation_filter_generation.load(std::memory_order_acquire));
        float delta_local[3][3]{}; XrQuatToCryMatrix(delta_xr,delta_local);

        // V66.109: ordinary immersive VR has one final orientation authority: HMD.
        // KCD2 still supplies camera POSITION and its horizontal/root yaw, but
        // native animated pitch/roll (pickup, grab, authored head bob) is stripped
        // before the recentered HMD delta is applied.  This prevents an animation
        // from physically pitching/rolling the user's world. Diagnostic F3 marriage
        // modes retain their historical basis manipulation below.
        Vec3f visual_basis[3]={basis[0],basis[1],basis[2]};
        const int marriage_mode=s_view_marriage_mode.load(std::memory_order_acquire);
        const bool hmd_final_orientation_authority=(marriage_mode==0)&&!s_native_head_rotation_owner.load(std::memory_order_acquire);
        if(hmd_final_orientation_authority) {
            Vec3f f{base_forward.x,base_forward.y,0.0f};
            float fl=std::sqrt(f.x*f.x+f.y*f.y);
            if(fl<1e-5f) {
                // Degenerate vertical native camera: derive horizontal forward from right.
                f={-base_right.y,base_right.x,0.0f};
                fl=std::sqrt(f.x*f.x+f.y*f.y);
            }
            if(fl>=1e-5f) {
                f.x/=fl;f.y/=fl;
                const Vec3f r{f.y,-f.x,0.0f};
                const Vec3f u{0.0f,0.0f,1.0f};
                visual_basis[0]=r;visual_basis[1]=f;visual_basis[2]=u;
            }
        }
        if (s_apply_rotation.load(std::memory_order_relaxed)) {
            const int order=s_rotation_order.load(std::memory_order_acquire);
            if (order==0) {
                // Local HMD rotation after the selected visual base: Bvisual * D.
                for(int c=0;c<3;++c){
                    const Vec3f world{
                        visual_basis[0].x*delta_local[0][c]+visual_basis[1].x*delta_local[1][c]+visual_basis[2].x*delta_local[2][c],
                        visual_basis[0].y*delta_local[0][c]+visual_basis[1].y*delta_local[1][c]+visual_basis[2].y*delta_local[2][c],
                        visual_basis[0].z*delta_local[0][c]+visual_basis[1].z*delta_local[1][c]+visual_basis[2].z*delta_local[2][c]};
                    matrix->m[0][c]=world.x; matrix->m[1][c]=world.y; matrix->m[2][c]=world.z;
                }
            } else {
                // Diagnostic opposite composition: D * B in Cry world coordinates.
                float B[3][3]={{base_right.x,base_forward.x,base_up.x},
                               {base_right.y,base_forward.y,base_up.y},
                               {base_right.z,base_forward.z,base_up.z}};
                float O[3][3]{};
                for(int r=0;r<3;++r) for(int c=0;c<3;++c)
                    O[r][c]=delta_local[r][0]*B[0][c]+delta_local[r][1]*B[1][c]+delta_local[r][2]*B[2][c];
                for(int r=0;r<3;++r) for(int c=0;c<3;++c) matrix->m[r][c]=O[r][c];
            }
        }
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

