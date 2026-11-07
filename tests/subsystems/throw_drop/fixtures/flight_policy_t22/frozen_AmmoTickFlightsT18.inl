static void AmmoTickFlightsT18(std::uint64_t now,bool allowed) noexcept {
    for(int n=0;n<16;++n){auto& f=g_ammo_flight18[n];if(!f.serial)continue;
        void* arrow=nullptr;void* entity=nullptr;
        if(!AmmoFlightIdentityT18(f,arrow,entity)){
            (void)g_ammo_contact_id18[n].publish({});f={};ac18::flights.fetch_sub(1);continue;
        }
        const bool contacted=g_ammo_contact18[n].load(std::memory_order_acquire)==f.serial;
        if(contacted||f.load!=inventory_lifecycle_t8::loadEpisode18.load()||now<f.born||f.ageMs>15000){
            if(f.slotKnown)(void)AmmoSlotT18(entity,f.slot);
            AmmoNoteT18(contacted?"NATIVE_CONTACT":"FLIGHT_POLICY_RETIRED",-1,f.serial,f.key,"damage=NATIVE_CARROW_ONLY embedding=NATIVE rotation=RESTORED");
            (void)g_ammo_contact_id18[n].publish({});f={};ac18::flights.fetch_sub(1);continue;
        }
        const auto prior=f.tick;f.tick=now;if(!allowed||now<=prior||now-prior>50)continue;f.ageMs+=now-prior;
        float tm[12]{};ng13::Pose pose{};
        if(!SehCopy(static_cast<unsigned char*>(entity)+0x58,tm,sizeof(tm))||!ng13::fromMatrix(tm,pose)||
           !f.policy.observe({pose.p[0],pose.p[1],pose.p[2]}))continue;
        // Native render-proxy slot transform only. Particle/collision root and
        // its native trajectory remain untouched by visual tumbling.
        if(f.slotKnown){
            float rotated[12]{};const float angle=f.policy.furthestHorizontal*(f.kind==1?1.5707963f:1.0471976f);
            const float c=std::cos(angle),s=std::sin(angle);
            for(int r=0;r<3;++r){const int j=r*4;rotated[j]=f.slot[j];
                rotated[j+1]=f.slot[j+1]*c+f.slot[j+2]*s;rotated[j+2]=-f.slot[j+1]*s+f.slot[j+2]*c;rotated[j+3]=f.slot[j+3];}
            (void)AmmoSlotT18(entity,rotated);
        }
        void* body=Rd<void*>(arrow,0x70,nullptr);
        if(!Vft13(body,0x3B0C698))continue; // only the proven native arrow particle
        DropDynamicsT1 d{};int ok=0;
        __try{ok=Native13<int(__fastcall*)(void*,void*)>(0xD85828)(body,&d);}
        __except(EXCEPTION_EXECUTE_HANDLER){ok=0;}
        flight17::Vec3 v{};
        if(ok&&f.policy.tailVelocity({d.v[0],d.v[1],d.v[2]},float(now-prior)*.001f,v)&&
           AmmoFlightIdentityT18(f,arrow,entity)&&g_ammo_contact18[n].load()!=f.serial&&Rd<void*>(arrow,0x70,nullptr)==body){
            const float corrected[3]{v.x,v.y,v.z};
            __try{Native13<void(__fastcall*)(void*,void*,const float*,float,void*,int)>(0x2AAD50C)(nullptr,body,corrected,1.f,nullptr,0);}
            __except(EXCEPTION_EXECUTE_HANDLER){if(f.slotKnown)(void)AmmoSlotT18(entity,f.slot);(void)g_ammo_contact_id18[n].publish({});f={};ac18::flights.fetch_sub(1);}
        }
    }
}
