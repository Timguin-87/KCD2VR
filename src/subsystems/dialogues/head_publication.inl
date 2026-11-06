            // Publish before native-body / gameplay-write gates. Dialogue may
            // preserve its native animation, yet still needs a fresh Henry eye.
            // Reuse the already verified character+0xA08 model transform.
            ModelWorld6692 body{};
            if(ReadModelWorld6692(skinChar,body)&&kcd2vr::native_roomscale_d76::SameSerial(
                kcd2vr::native_roomscale_d76::Surface::Character,reinterpret_cast<std::uintptr_t>(skinChar),body.roomOriginD76.serial)) {
                const V3 worldHead=ModelToWorld6692(body,hp);
                g_dialogue_head59c10r2.publish({std::uint64_t(GetTickCount64()),g_pose_epoch6689.load(std::memory_order_acquire),
                    reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire)),
                    reinterpret_cast<std::uintptr_t>(skinChar),{worldHead.x,worldHead.y,worldHead.z},
                    {body.q.x,body.q.y,body.q.z,body.q.w},body.scale,{body.t.x,body.t.y,body.t.z},true,body.roomOriginD76});
            }
