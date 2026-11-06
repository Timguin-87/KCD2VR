// Actual xr_manager.cpp compositor block.
            const float lag=hud_lag_.load(std::memory_order_relaxed);
            const auto& sr=ui88_hud.safe;
            const bool lazy=lag>0.001f&&layer_frame.head_pose_valid&&ui88_hud.surface.valid&&hud_lag_gameplay_.load(std::memory_order_relaxed)&&
                sr.right-sr.left>64.0f&&sr.bottom-sr.top>64.0f;
            const auto nowNs=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
            if(lazy){
                const XrQuaternionf head=QNormalize20(layer_frame.head_pose.orientation);
                const float dt=hud_lag_valid_?std::clamp(float(double(nowNs-hud_lag_last_ns_)*1.0e-9),0.0f,0.05f):0.0f;
                if(!hud_lag_valid_){hud_lag_q_=head;hud_lag_valid_=true;}
                const float tau=0.05f+0.30f*lag;                         // seconds
                hud_lag_q_=QNlerp20(hud_lag_q_,head,dt>0.0f?1.0f-std::exp(-dt/tau):0.0f);
                const float maxLag=(3.0f+7.0f*lag)*0.01745329f;          // 3..10 degrees
                const float angle=QAngle20(head,hud_lag_q_);
                if(std::isfinite(angle)&&angle>maxLag)hud_lag_q_=QNlerp20(head,hud_lag_q_,maxLag/angle);
                const V3f c{0,-0.02f,-hudDistance};
                const V3f w=QRotate(hud_lag_q_,c);const auto& hp=layer_frame.head_pose.position;
                hud_quad.space=local_space_;hud_quad.pose.orientation=hud_lag_q_;hud_quad.pose.position={hp.x+w.x,hp.y+w.y,hp.z+w.z};
            }else hud_lag_valid_=false;
            hud_lag_last_ns_=nowNs;
