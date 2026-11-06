R"DAGGER54(
-- C54 cosmetic only: actual Henry inventory WUID + retail class/model evidence.
-- No inventory transfer, world entity, physics body or right-hand draw is created.
if not KCD2VR_DaggerChest54 then
  local S={protocol=1,owner=nil,key=nil,class=nil,path=nil,bone=nil,name=nil,blocked=false,created=false,bound=false}
  local models={['09b9021d-fdb4-4aee-8d53-78bdeb073c1d']='objects/manmade/weapons/daggers/dagger_common.cgf',['2ab2abbc-1620-4ee0-924f-cbe1c90adcc4']='objects/manmade/weapons/daggers/dagger_reward.cgf',['36099b07-dae2-4b7b-bcb2-5b580e872ec3']='objects/manmade/weapons/daggers/dagger_bruncvik.cgf',['39826a87-69aa-45cc-94e1-170a4520e2e9']='objects/manmade/weapons/daggers/dagger_bruncvik.cgf',['3a640e5d-d8bd-4e8b-b61d-8cd5180e79e7']='objects/manmade/weapons/daggers/dagger_common.cgf',['6195801f-e7e4-429c-9db9-8b31a62126c8']='objects/manmade/weapons/daggers/dagger_a.cgf',['7b31ad0f-1443-4421-a43f-f380dde5bdf0']='objects/manmade/weapons/daggers/dagger_common.cgf',['80591e34-92ce-44d7-a4ed-b2fcd073eb6e']='objects/manmade/weapons/daggers/dagger_common.cgf',['884d8895-7406-46a7-9e86-ac8c4a1f70c1']='objects/manmade/weapons/daggers/dagger_common.cgf',['b24cef83-a8d7-4d2a-9ae0-079beccfa9df']='objects/manmade/weapons/daggers/dagger_common.cgf'}
  local function say(s) if System and System.LogAlways then System.LogAlways('KCD2VR DAGGER54 '..s) end end
  local function norm(s) return type(s)=='string' and string.lower(string.gsub(s,'\\','/')) or nil end
  local function entity(id) return System and System.GetEntity and System.GetEntity(id) or nil end
  local function owned(v)
    return v and S.owner==v and S.name and v.GetAttachmentCGF and v.GetAttachmentBone and
      norm(v:GetAttachmentCGF(0,S.name))==S.path and norm(v:GetAttachmentBone(0,S.name))==norm(S.bone)
  end
  local function clear()
    local v=S.owner
    -- Compare the actual current owner AND our exact mesh/bone before deleting.
    -- Never restore native visibility, destroy a foreign attachment or touch an item.
    if v and entity(v.id)==v and S.created and S.name and v.GetAttachmentBone and v.GetAttachmentCGF and
       norm(v:GetAttachmentBone(0,S.name))==norm(S.bone) and
       (owned(v) or (not S.bound and v:GetAttachmentCGF(0,S.name)==nil)) then v:DestroyAttachment(0,S.name) end
    S.owner=nil S.key=nil S.class=nil S.path=nil S.bone=nil S.name=nil S.blocked=false S.created=false S.bound=false
  end
  local function renew(v,key,name)
    S.leaseSerial=(S.leaseSerial or 0)+1
    local serial=S.leaseSerial
    -- At most3 pending1Hz renewals. If the native frame/ASI stops, only our
    -- still-identical cosmetic binding can expire; no inventory object is touched.
    Script.SetTimer(2500,function()
      if S.owner==v and S.key==key and S.name==name and S.leaseSerial==serial then clear() end
    end)
  end
  local function item(key)
    if not ItemManager or type(ItemManager.GetItem)~='function' then return nil end
    local q=ItemManager.GetItem(key)
    if type(q)~='table' or q.id~=key or type(q.class)~='string' then return nil end
    return q
  end
  local function current(v,key,cls)
    if not v or entity(v.id)~=v or not v.inventory or type(v.inventory.FindItem)~='function' then return nil end
    -- FindItem proves current membership; GetItem alone is a global item lookup.
    local actual=v.inventory:FindItem(cls)
    if actual~=key then return nil end
    local q=item(key)
    if not q or string.lower(q.class)~=cls or (q.amount and q.amount<=0) then return nil end
    -- Native drawn/loose item entity means its real lifecycle owns presentation.
    if q.entity and q.entity~=0 then return nil end
    return q
  end
  function S.refresh(id,generation,name,bone,scan,enabled)
    if not enabled then clear() return end
    local v=entity(id)
    if not Script or type(Script.SetTimer)~='function' or not v or v~=g_localActor or not v.inventory or type(v.inventory.GetInventoryTable)~='function' or
       type(v.inventory.FindItem)~='function' or not v.CreateBoneAttachment or not v.SetAttachmentCGF or
       not v.GetAttachmentCGF or not v.GetAttachmentBone or not v.SetAttachmentPos or not v.SetAttachmentAngles or
       not v.DestroyAttachment then clear() return end
    if S.owner and (S.owner~=v or S.name~=name or S.bone~=bone) then clear() end
    local key,cls,path=S.key,S.class,S.path
    if key and not current(v,key,cls) then clear() key=nil cls=nil path=nil end
    if scan then
      -- Full enumeration happens on explicit identity/equipment/menu-close edges only.
      -- Unknown/oversized/ambiguous inventories produce no cosmetic substitute.
      local inv=v.inventory:GetInventoryTable()
      local foundKey,foundClass,foundPath,count=nil,nil,nil,0
      if type(inv)~='table' then clear() return end
      for _,candidate in pairs(inv) do
        count=count+1
        if count>512 then clear() say('refused=INVENTORY_SCAN_CAP') return end
        local q=item(candidate)
        local c=q and string.lower(q.class) or nil
        local m=c and models[c] or nil
        if m and current(v,candidate,c) then
          if foundKey and foundKey~=candidate then clear() say('refused=AMBIGUOUS_CARRIED_DAGGERS') return end
          foundKey,foundClass,foundPath=candidate,c,m
        end
      end
      if key and key~=foundKey then clear() end
      key,cls,path=foundKey,foundClass,foundPath
    end
    if not key then return end
    if S.owner and owned(v) then renew(v,key,name) return end
    -- A foreign or native rebinding wins; do not overwrite it or rebind every tick.
    if S.owner then S.blocked=true return end
    if v:GetAttachmentBone(0,name)~=nil or v:GetAttachmentCGF(0,name)~=nil then
      say('refused=NAME_ALREADY_BOUND') return
    end
    if not current(v,key,cls) then return end
    S.owner=v S.key=key S.class=cls S.path=path S.bone=bone S.name=name S.generation=generation
    v:CreateBoneAttachment(0,bone,name)
    S.created=norm(v:GetAttachmentBone(0,name))==norm(bone)
    -- Check identity/membership again after the create boundary and before binding.
    if entity(id)~=v or not current(v,key,cls) then clear() return end
    if norm(v:GetAttachmentBone(0,name))~=norm(bone) then clear() say('refused=CHEST_BONE_NOT_OBSERVED') return end
    v:SetAttachmentCGF(0,name,path)
    -- Placement is a bounded visual starting fit, not a physical dagger/grip claim.
    v:SetAttachmentPos(0,name,{x=-0.10,y=-0.12,z=0.04})
    v:SetAttachmentAngles(0,name,{x=0,y=0,z=0.20})
    if not owned(v) then clear() say('refused=MODEL_BIND_NOT_OBSERVED') return end
    S.bound=true
    renew(v,key,name)
    say('visible=1 model='..path..' nativeDraw=UNIMPLEMENTED')
  end
  KCD2VR_DaggerChest54=S
end
)DAGGER54"
