-- Exact supplied retail script method extraction; generated C55 fixture.
function BasicEntity:SetupModel()
	local Properties = self.Properties
	self:LoadObject(0,Properties.object_Model)

	if (Properties.Physics.bPhysicalize == true) then
		self:PhysicalizeThis()
	end
end

-- =============================================================================

function BasicEntity:PhysicalizeThis()
	-- Init physics.
	local Physics = self.Properties.Physics
	if (CryAction.IsImmersivenessEnabled() == 0) then
		Physics = Physics_DX9MP_Simple
	end
	EntityCommon.PhysicalizeRigid( self,0,Physics,self.bRigidBodyActive )
end

-- =============================================================================
-- OnPropertyChange called only by the editor.

EntityCommon.PhysicalizeRigid = function(entity, nSlot, Properties, bActive)

	if Properties.bPhysicalize == false then

		return

	end



	local Mass = Properties.Mass

	local Density = Properties.Density

	if bActive == false then

		Mass = 0.0

		Density = 0.0

	end



	local physType



	if (Properties.bArticulated == true) then

		physType = PE_ARTICULATED

	else

		if (Properties.bRigidBody == true) then

			physType = PE_RIGID

		else

			physType = PE_STATIC

		end



	end



	local TempPhysParams = EntityCommon.TempPhysParams



	TempPhysParams.density = Density

	TempPhysParams.mass = Mass

	TempPhysParams.flags = 0



	if (Properties.CGFPropsOverride) then

		TempPhysParams.CGFprops = ""

		for key, value in pairs(Properties.CGFPropsOverride) do

			if (type(value) == "table") then

				for key1, value1 in pairs(value) do

					if (value1 ~= "") then

						TempPhysParams.CGFprops = TempPhysParams.CGFprops .. key1 .. "=" .. value1 .. "\n"

					end

				end

			else

				if (value ~= "") then

					TempPhysParams.CGFprops = TempPhysParams.CGFprops .. key .. "=" .. value .. "\n"

				end

			end

		end

	end



	entity:Physicalize(nSlot, physType, TempPhysParams)



	-- entity:SetPhysicParams(PHYSICPARAM_SIMULATION, GlobalPhysicsSimParams )



	if (Mass > 0 or Density > 0) then

		EntityCommon.ApplyCollisionFiltering(entity, {collisionClass = gcc_rigid})

	end



	if (Properties.bInteractLargeObject == true) then

		EntityCommon.ApplyCollisionFiltering(entity, {collisionClass = gcc_large_kickable})

	end



	EntityCommon.ApplyCollisionFiltering(entity, EntityCommon.GetCollisionFiltering(entity))



	if (Properties.Simulation) then

		local SimulationSrc = Properties.Simulation

		local SimulationDst = EntityCommon.TempSimulationParams

		for key, value in next, SimulationDst, nil do

			SimulationDst[key] = nil

		end

		for key, value in next, SimulationSrc, nil do

			if (key ~= "max_time_step" or value < 0.0199 or value > 0.0201) and (key ~= "sleep_speed" or value < 0.0399 or value > 0.0401) then

				SimulationDst[key] = value

			end

		end

		entity:SetPhysicParams(PHYSICPARAM_SIMULATION, SimulationDst)

	end



	local Buoyancy = Properties.Buoyancy

	if (Buoyancy) then

		entity:SetPhysicParams(PHYSICPARAM_BUOYANCY, Buoyancy)

	end



	local ForeignData = Properties.ForeignData

	if (ForeignData and ForeignData.bMovingPlatform == 1) then

		entity:SetPhysicParams(PHYSICPARAM_FOREIGNDATA, {foreignFlags = FOREIGNFLAGS_MOVING_PLATFORM})

	end



	-----------------------------------------------------------------------------

	-- Set physical flags.

	-----------------------------------------------------------------------------

	local PhysFlags = EntityCommon.TempPhysicsFlags

	PhysFlags.flags = 0

	if (Properties.bPushableByPlayers == true) then

		PhysFlags.flags = pef_pushable_by_players

	end

	if (Simulation and Simulation.bFixedDamping and Simulation.bFixedDamping == 1) then

		PhysFlags.flags = PhysFlags.flags + pef_fixed_damping

	end

	if (Simulation and Simulation.bUseSimpleSolver and Simulation.bUseSimpleSolver == 1) then

		PhysFlags.flags = PhysFlags.flags + ref_use_simple_solver

	end

	if (Properties.bCanBreakOthers == nil or Properties.bCanBreakOthers == 0) then

		PhysFlags.flags = PhysFlags.flags + pef_never_break

	end

	if (Properties.MP and Properties.MP.bClientOnly) then

		-- allow breaking on the client

		PhysFlags.flags = PhysFlags.flags + pef_override_impulse_scale

	end

	PhysFlags.flags_mask = pef_fixed_damping + ref_use_simple_solver + pef_pushable_by_players + pef_never_break + pef_override_impulse_scale

	entity:SetPhysicParams(PHYSICPARAM_FLAGS, PhysFlags)

	-----------------------------------------------------------------------------



	if (Properties.bResting == 0) then

		entity:AwakePhysics(1)

	else

		entity:AwakePhysics(0)

	end

end



-- =============================================================================

-- Compare entities by name (for table.sort)
