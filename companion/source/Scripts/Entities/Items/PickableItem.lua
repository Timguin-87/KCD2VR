PickableItem = {
	Properties={

		bInteractiveCollisionClass = false,

		Script = {
			Misc = ''
		},
	},

	-- item -- game item attached from C++

	npcOnly = false,	-- synchronized from item in code

	InteractorPriority = 2,
}

-- =============================================================================
function PickableItem:OnPropertyChange()
	self.item:Reset()
	self:SetInteractiveCollisionType()
end

-- =============================================================================
function PickableItem:GetActions(user, firstFast)
	output = {}

	if (not user) then
		return output; -- canot be used by AI
	end

   if ( self.npcOnly ) then
      return output
   end

	if self.item:CanUse(user.id) == false then
		return output
	end

	self.user = user

	if (self.item:CanSteal(self.user.id)) then
		AddInteractorAction( output, firstFast, Action():hint( "@ui_hud_stealItem" ):action( "use" ):hintType( AHT_HOLD ):func( PickableItem.OnUsedHold ):interaction( inr_pickableSteal ) )
	elseif (self.item:BelongsToDeadBody()) then
		AddInteractorAction( output, firstFast, Action():hint( "@ui_hud_loot" ):action( "use" ):func( PickableItem.OnUsed ):interaction( inr_pickableLoot ) )
	else
		AddInteractorAction( output, firstFast, Action():hint( "@ui_pickup_item" ):action( "use" ):func( PickableItem.OnUsed ):interaction( inr_pickablePickup ) )
	end

	return output
end

-- =============================================================================
function PickableItem:IsCrossCenteringEnabled()
	return true
end

-- =============================================================================
function PickableItem:GetUsableName(slot)
	return self.item:GetUIName()
end

-- =============================================================================
function PickableItem:PhysicalizeThis()
end

-- =============================================================================
function PickableItem:Use(user)
	if (user) then
		local lab = rawget(_G, "KCD2VR_Lab")
		local ok, prepared, why = true, false, 1
		if lab and type(lab.TryPreparePickupR24) == "function" then
			ok, prepared, why = pcall(lab.TryPreparePickupR24, self, user)
			if not ok then why = 2
			elseif prepared == true then why = 0
			elseif type(why) ~= "number" or why < 0 or why > 18 or why ~= math.floor(why) then why = 3 end
		end
		-- F5 only, through the existing native getter into KCD2VR.log.
		-- No engine-side log file, script-error text or per-frame timer is added.
		local get = System and System.GetCVar
		if type(get) == "function" then
			local captureOK, capture = pcall(get, "kcd2vr_door_capture_f5")
			if captureOK and capture == 1 then
				pcall(get, "kcd2vr_pickup_prepare_note_v1:" .. tostring(why))
			end
		end
		return self.item:OnUsed(user.id)
	end

	return false
end

-- =============================================================================
function PickableItem:OnEnablePhysics()
	-- When loading streamable layer, OnLevelLoaded has been sent way before.
	-- Nevertheless, interactive collision class must be set for the entity
	self:SetInteractiveCollisionType()
end

-- =============================================================================
function PickableItem:OnLevelLoaded()
	self:SetInteractiveCollisionType()
end

-- =============================================================================
function PickableItem:SetInteractiveCollisionType()
	local filtering = {}

	if(self.Properties.bInteractiveCollisionClass == true) then
		filtering.collisionClass = 262144; -- gcc_interactive from GamePhysicsCollisionClasses.h
	else
		filtering.collisionClassUNSET = 262144
	end

	self:SetPhysicParams(PHYSICPARAM_COLLISION_CLASS, filtering )
end

-- =============================================================================
function PickableItem:OnUsed(user)
	-- disable use action for stolen items
	if (self.user and self.item:CanSteal(self.user.id)) then
		return true
	end

--	if (self.item:IsFromShop() and UIAction) then
--		Shops.OpenInventoryForItem(self.item:GetId(), false)
--	else

	-- item OnUsed()
	return self:Use(user)
--	end
end

-- =============================================================================
function PickableItem:OnUsedHold(user, slot)

	if (user) then
		return self.item:OnSteal(user.id)
	end

	return false
end

-- =============================================================================
function CreateItemTable(name)
	if (not _G[name]) then
		_G[name] = table.DeepCopy(Item)
	end
end