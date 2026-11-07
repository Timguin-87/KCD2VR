from test_gore_d52_lua import GoreD52, SOURCE

l=GoreD52().runtime()
allow=l.execute(SOURCE.read_text()+'\nreturn headPartPath')
for optional in (True, False):
    l.globals().KCD2VR_Gore.partFilter=optional
    for path,name in [
        ('objects/characters/humans/male/head/head.skin','head'),
        ('objects/characters/humans/male/hair/hair.skin','hair'),
        ('objects/characters/humans/male/beard/beard.skin','beard'),
        ('objects/characters/humans/male/clothing/head/helmet/helmet.skin','helmet'),
        ('objects/characters/humans/male/clothing/head/cap/cap27/cap27.skin','cap27-head'),
    ]:
        assert allow(path,name), (optional,path,name)
    for path,name in [
        ('objects/characters/humans/male/clothing/head/coif/coif.skin','head'),
        ('objects/characters/humans/male/clothing/head/hood/hood.skin','hood'),
        ('objects/characters/humans/male/clothing/head/cap/cap.skin','cap_neck'),
        ('objects/characters/humans/male/clothing/head/helmet/helmet.skin','helmet_neck_cover'),
        ('objects/characters/humans/male/head/head.skin','cowl'),
        ('objects/characters/humans/male/body/body.skin','head'),
        ('objects/props/head/fake.skin','helmet'),
    ]:
        assert not allow(path,name), (optional,path,name)
print('PASS actual gore Lua: head/hair/beard/helmet/head-only cap admitted; coif/hood/cowl/neck/body/unknown rejected with legacy filter both ON and OFF')
