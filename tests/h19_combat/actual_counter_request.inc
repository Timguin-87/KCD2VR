        if(stab40.why==kcd2vr::counter_stab40::Why::Qualified){
            InjectResult35 counterResult{};DecodeNativeHitResult59c7g(counterResult,hitInfo,meleeDetails,ok,post);
            // Read the actual native result; blocked/parried/dead or foreign-target
            // hits never request this counter reaction. No second hit is submitted.
            counterMajorRequested=kcd2vr::counter_stab40::majorReaction(stab40.why,p.kind,!p.weaponItem,apply,p.entityId,counterResult);
        }
