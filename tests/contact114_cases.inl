// Execute production observer/forwarding with synthetic retail identities.
// No game instructions, fabricated hit resolution or OS input are executed.
{
 namespace capture=kcd2vr::diagnostics::combat_capture;
 diagnostics::lab::Set(diagnostics::lab::Key::F3,3);
 g_contact_until114=hostTickCount64+180000;
 kcd2vr::retail_build112::hostRetailVerified112=true;
 std::array<unsigned char,0x300> collisions{},enemyCollisions{};
 std::array<unsigned char,0x88> raw{};
 put(collisions.data(),base+0x3B91300);put(collisions.data()+8,actor.data());
 put(collisions.data()+0x10,base+0x3B912E8);put(actor.data()+0x218,collisions.data());
 put(enemyCollisions.data(),base+0x3B91300);put(enemyCollisions.data()+0x10,base+0x3B912E8);
 put(player.data()+0x30,std::uint32_t(123));g_last_player_entity=entity.data();g_henry_item_attach_manager=manager.data();
 PublishContactIdentity114(player.data(),actor.data(),entity.data());
 capture::head=capture::tail=0;put(raw.data()+0x24,std::uint32_t(987));
 const auto unchanged=raw;
 ObserveNativeContact114(collisions.data()+0x10,nullptr,nullptr,raw.data());
 capture::Event event{};
 ck(capture::pop(event)&&event.kind==capture::Kind::Contact&&event.flags==1&&event.id==987,
    "Henry outgoing native contact observed with raw target ID, not labelled accepted damage");
 ck(raw==unchanged,"contact observer never mutates payload; forwarding tested in shared-hook fixture");
 const auto beforeUnrelated=g_read_queries101R2.load();
 for(unsigned n=0;n<1000;++n)ObserveNativeContact114(enemyCollisions.data()+0x10,nullptr,nullptr,raw.data());
 ck(!capture::pop(event)&&g_read_queries101R2.load()-beforeUnrelated<=1000,
    "1000 unrelated NPC contacts discarded by observer with at most one page qualification per callback");
 put(raw.data()+0x24,std::uint32_t(123));
 ObserveNativeContact114(enemyCollisions.data()+0x10,nullptr,nullptr,raw.data());
 ck(capture::pop(event)&&event.flags==2,"native target ID admits incoming Henry contact from enemy callback");
 const auto beforeLimit=g_contact_limited114.load();
 ObserveNativeContact114(enemyCollisions.data()+0x10,nullptr,nullptr,raw.data());
 ck(!capture::pop(event)&&g_contact_limited114.load()==beforeLimit+1,"incoming contact diagnostics limited without suppressing native callback");
 hostTickCount64+=81;
 ObserveNativeContact114(enemyCollisions.data()+0x10,nullptr,nullptr,raw.data());
 ck(!capture::pop(event),"expired identity snapshot fails closed");
 PublishContactIdentity114(player.data(),actor.data(),entity.data());
 put(player.data()+0x9D0,static_cast<void*>(nullptr));
 ObserveNativeContact114(enemyCollisions.data()+0x10,nullptr,nullptr,raw.data());
 ck(!capture::pop(event),"cached prefilter cannot bypass broken live Henry backlink");
 put(player.data()+0x9D0,manager.data());
 kcd2vr::retail_build112::hostRetailVerified112=false;
 ObserveNativeContact114(enemyCollisions.data()+0x10,nullptr,nullptr,raw.data());
 ck(!capture::pop(event),"unqualified retail SHA disables all contact observation");
 kcd2vr::retail_build112::hostRetailVerified112=true;
 diagnostics::lab::Set(diagnostics::lab::Key::F3,0);
 ObserveNativeContact114(enemyCollisions.data()+0x10,nullptr,nullptr,raw.data());
 ck(!capture::pop(event),"mode off still forwards without recording contacts");
 g_contact_identity114.publish(ContactIdentity114{});
}
