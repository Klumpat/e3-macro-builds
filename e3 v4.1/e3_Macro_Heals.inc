|------------------------------------------------------------------------|
|- e3_Macro_Heals.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains heal related functions for e3.mac							-|
|------------------------------------------------------------------------|



|------------------------------------------------------------|
|- Heals.inc's mainLoop hook.								-|	
|------------------------------------------------------------|					
|- Calls heal functions, as an extention of the main loop.	-|	
|------------------------------------------------------------|
SUB CheckHeals
/if (${Debug}) /echo |- CheckHeals  ==>

|-------------------------------------------------------------------------------------------------------Check MainTank.
	/if (${Defined[MainTank]}) /call check_MainTank
		/if (${ActionTaken}) /RETURN

|------------------------------------------------------------------------------------------------Check priorityTargets.
	/if (${priorityTargets.Size}) /call check_PriorityTargets
		/if (${ActionTaken}) /RETURN
		
|-------------------------------------------------------------------------------------------------Check NetBots.
	/if (${NetBots.Counts}) /call check_NetBots
		/if (${ActionTaken}) /RETURN
		
|-------------------------------------------------------------------------------------------------Check NetBots.	
	/if (${Defined[HoT_Spell]} && ${NetBots.Counts}) /call check_HoT
	
	/if (${recentHeal_Timer}) /varset ActionTaken TRUE
	
/if (${Debug}) /echo <== CheckHeals -|
/RETURN



|----
|- check_MainTank
|-----
|- Checks 
|---
SUB check_MainTank
/if (${Debug}) /echo |- check_MainTank ==>

	/declare i int local 1
	
|------------------------------------------------------------------------------------------------------------------Check the MainTank string for the first tank within range, and set them as the main tank.
	:check_Tank
	/if (${Debug}) /echo |- check_MainTank -| :check_Tank
	
	/if (!${SpawnCount[pc ${MainTank.Arg[${i},/]}]} || ${NetBots[${MainTank.Arg[${i},/]}].Zone} != ${Zone.ID}) {
		/varcalc i ${i} + 1
		/if (${MainTank.Arg[${i},/].Length}) /goto :check_Tank
	} else {
		/if (${NetBots[${MainTank.Arg[${i},/]}].PctHPs} < ${MTQuick_Heal.Arg[3,/]} && ${Me.CurrentMana} > ${Spell[${MTQuick_Heal.Arg[1,/]}].Mana} && ${Spawn[pc ${MainTank.Arg[${i},/]}].Distance} < ${Spell[${MTQuick_Heal.Arg[1,/]}].MyRange}) {
			/if (${Debug}) /echo |- check_MainTank -| [${MainTank.Arg[${i},/]} -- ${NetBots[${MainTank.Arg[${i},/]}].PctHPs}] casting [${MTQuick_Heal.Arg[1,/]}]
			/call e3_Cast ${NetBots[${MainTank.Arg[${i},/]}].ID} "${MTQuick_Heal.Arg[1,/]}" ${MTQuick_Heal.Arg[2,/]} ${If[${Me.Gem[${MTQuick_Heal.Arg[1,/]}]},0s,5s]} Heal_CastingChecks
			/varset recentHeal_Timer 30
		} else /if (${NetBots[${MainTank.Arg[${i},/]}].PctHPs} < ${MainTank_Heal.Arg[3,/]} && ${Me.CurrentMana} > ${Spell[${MainTank_Heal.Arg[1,/]}].Mana} && ${Spawn[pc ${MainTank.Arg[${i},/]}].Distance} < ${Spell[${MainTank_Heal.Arg[1,/]}].MyRange}) {
			/if (${Debug}) /echo |- check_MainTank -| [${MainTank.Arg[${i},/]} -- ${NetBots[${MainTank.Arg[${i},/]}].PctHPs}] casting [${MainTank_Heal.Arg[1,/]}]
			/call e3_Cast ${NetBots[${MainTank.Arg[${i},/]}].ID} "${MainTank_Heal.Arg[1,/]}" ${MainTank_Heal.Arg[2,/]} ${If[${Me.Gem[${MainTank_Heal.Arg[1,/]}]},0s,5s]} Heal_CastingChecks
			/varset recentHeal_Timer 30
		}
	}
	
/if (${Debug}) /echo <== check_MainTank -|
/RETURN



|------------------------------------------------|
|- check_PriorityTargets						-|
|------------------------------------------------|
|- Checks NetBots listed as PriorityTargets.	-|
|------------------------------------------------|
SUB check_PriorityTargets
/if (${Debug}) /echo |- check_PriorityTargets ==>

	/declare i int local
	
	/for i 1 to ${priorityTargets.Size}
		/if (${SpawnCount[pc ${priorityTargets[${i}]}]} && ${NetBots[${priorityTargets[${i}]}].Zone} == ${Zone.ID}) {
			/if (${NetBots[${priorityTargets[${i}]}].PctHPs} < ${Patch_Heal.Arg[3,/]} && ${Me.CurrentMana} > ${Spell[${Patch_Heal.Arg[1,/]}].Mana} && ${Spawn[pc ${priorityTargets[${i}]}].Distance} < ${Spell[${Patch_Heal.Arg[1,/]}].MyRange}) {
				/if (${Debug}) /echo |- check_PriorityTargets -| [${priorityTargets[${i}]} -- ${NetBots[${priorityTargets[${i}]}].PctHPs}] casting [${Patch_Heal.Arg[1,/]}]
				/call e3_Cast ${NetBots[${priorityTargets[${i}]}].ID} "${Patch_Heal.Arg[1,/]}" ${Patch_Heal.Arg[2,/]} ${If[${Me.Gem[${Patch_Heal.Arg[1,/]}]},0s,5s]} Heal_CastingChecks
				/varset recentHeal_Timer 30
			}
		}
	/next i

/if (${Debug}) /echo <== check_PriorityTargets -|
/RETURN



|----------------------------------------------------|
|- check_NetBots									-|
|----------------------------------------------------|
|- Checks all NetBots, and heals whoever is lowest.	-|
|----------------------------------------------------|
SUB check_NetBots
/if (${Debug}) /echo |- check_NetBots ==>

	/declare i int local
	
	/for i 1 to ${NetBots.Counts}
		/if (${SpawnCount[pc ${NetBots.Client[${i}]}]} && ${NetBots[${NetBots.Client[${i}]}].Zone} == ${Zone.ID}) {
			/if (${NetBots[${NetBots.Client[${i}]}].PctHPs} < ${Patch_Heal.Arg[3,/]} && ${Me.CurrentMana} > ${Spell[${Patch_Heal.Arg[1,/]}].Mana} && ${Spawn[pc ${NetBots.Client[${i}]}].Distance} < ${Spell[${Patch_Heal.Arg[1,/]}].MyRange}) {
				/if (${Debug}) /echo |- check_NetBots -| [${NetBots.Client[${i}]} -- ${NetBots[${NetBots.Client[${i}]}].PctHPs}] casting [${Patch_Heal.Arg[1,/]}]
				/call e3_Cast ${NetBots[${NetBots.Client[${i}]}].ID} "${Patch_Heal.Arg[1,/]}" ${Patch_Heal.Arg[2,/]} ${If[${Me.Gem[${Patch_Heal.Arg[1,/]}]},0s,5s]} Heal_CastingChecks
				/varset recentHeal_Timer 30
			}
		}
	/next i

/if (${Debug}) /echo <== check_NetBots -|
/RETURN



|----------------|
|- check_HoT	-|
|----------------|
|-
|-----------
SUB check_HoT
/if (${Debug}) /echo |- check_HoT ==>

	/if (!${Defined[netBuffs]}) /declare netBuffs string outer
	/declare i int local
	
|---------------------------------------------------------------------MTOnly.
	/if (${HoT_Mode.Equal[MTOnly]}) {
		:check_Tank
		/if (${Debug}) /echo |- check_HoT -| :check_Tank
		
		/if (${SpawnCount[pc ${MainTank.Arg[${i},/]}]} && ${NetBots[${MainTank.Arg[${i},/]}].Zone} == ${Zone.ID} && !${Select[${Spell[${HoT_Spell.Arg[1,/]}].ID},${NetBots[${Spawn[pc ${MainTank.Arg[${i},/]}]}].ShortBuff.Replace[ ,,]}]}) {
			/if (${NetBots[${MainTank.Arg[${i},/]}].PctHPs} < ${HoT_Spell.Arg[3,/]} && ${Me.CurrentMana} > ${Spell[${HoT_Spell.Arg[1,/]}].Mana} && ${Spawn[pc ${MainTank.Arg[${i},/]}].Distance} < ${Spell[${HoT_Spell.Arg[1,/]}].MyRange}) {
				/if (${Debug}) /echo |- check_HoT -| [${MainTank.Arg[${i},/]} -- ${NetBots[${MainTank.Arg[${i},/]}].PctHPs}] casting [${HoT_Spell.Arg[1,/]}]
				/call e3_Cast ${NetBots[${MainTank.Arg[${i},/]}].ID} "${HoT_Spell.Arg[1,/]}" ${HoT_Spell.Arg[2,/]} ${If[${Me.Gem[${HoT_Spell.Arg[1,/]}]},0s,5s]} HoT_CastingChecks
				/varset recentHeal_Timer 30
			}
		} else /if (${MainTank.Arg[${Math.Calc[${i} + 1]},/].Length}) {
			/varcalc i ${i} + 1
			/if (${MainTank.Arg[${i},/].Length}) /goto :check_Tank
		}
		
|------------------------------------------------------------------------All.
	} else /if (${HoT_Mode.Equal[All]}) {
		/for i 1 to ${NetBots.Counts}
			/if (${SpawnCount[pc ${NetBots.Client[${i}]}]} && ${NetBots[${NetBots.Client[${i}]}].Zone} == ${Zone.ID} && !${Select[${Spell[${HoT_Spell.Arg[1,/]}].ID},${NetBots[${Spawn[pc ${NetBots.Client[${i}]}]}].ShortBuff.Replace[ ,,]}]}) {
				/if (${NetBots[${NetBots.Client[${i}]}].PctHPs} < ${HoT_Spell.Arg[3,/]} && ${Me.CurrentMana} > ${Spell[${HoT_Spell.Arg[1,/]}].Mana} && ${Spawn[pc ${NetBots.Client[${i}]}].Distance} < ${Spell[${HoT_Spell.Arg[1,/]}].MyRange}) {
					/if (${Debug}) /echo |- check_HoT -| [${NetBots.Client[${i}]} -- ${NetBots[${NetBots.Client[${i}]}].PctHPs}] casting [${HoT_Spell.Arg[1,/]}]
					/call e3_Cast ${NetBots[${NetBots.Client[${i}]}].ID} "${HoT_Spell.Arg[1,/]}" ${HoT_Spell.Arg[2,/]} ${If[${Me.Gem[${HoT_Spell.Arg[1,/]}]},0s,5s]} HoT_CastingChecks
					/varset recentHeal_Timer 30
				}
			}
		/next i
	}

/if (${Debug}) /echo <== check_HoT -|
/RETURN



SUB check_Pets
/declare i int local
/for i 1 to ${NetBots.Counts}
/if (${NetBots[${NetBots.Client[${i}]}].PetID}) {
	/if (${NetBots[${NetBots.Client[${i}]}].PetHP} < ${PetHeal_Pct} && ${Spawn[${NetBots[${NetBots.Client[${i}]}].PetID}].Distance} < ${Spell[${PetHeal_Spell}].MyRange}) {
		/if (${Target.ID} != ${NetBots[${NetBots.Client[${i}]}].PetID}) /call TrueTarget ${NetBots[${NetBots.Client[${i}]}].PetID}
		/call e3_Cast ${NetBots[${NetBots.Client[${i}]}].PetID} "${Spell[${PetHeal_Spell}].ID}" Gem${PetHeal_Gem} 3s
		/varset ActionTaken TRUE
	}
}
/next i
/RETURN



|----------------------------------------|
|- Checks to make while casting heals.	-|
|----------------------------------------|
SUB Heal_CastingChecks
/if (${Debug}) /echo |- HoT_CastingChecks ==>

	/if (${castEndTime}<5 && ${Target.PctHPs}>=95) /call interrupt

/if (${Debug}) /echo <== HoT_CastingChecks -|
/RETURN



|--------------------------------------------|
|- Checks to make while casting HoT spells.	-|
|--------------------------------------------|
SUB HoT_CastingChecks
/if (${Debug}) /echo |- HoT_CastingChecks ==>

	/if (${Me.Casting.ID}==${Spell[${HoT_Spell.Arg[1,/]}].ID} && ${Select[${Spell[${HoT_Spell.Arg[1,/]}].ID},${NetBots[${Target.CleanName}].ShortBuff.Replace[ ,,]}]}) /call interrupt

/if (${Debug}) /echo <== HoT_CastingChecks -|
/RETURN



|--------------------------------------------------------------------|
|- setup_Heals														-|
|--------------------------------------------------------------------|
|- Imports and validates ini settings from Character Ini,[Heals].	-|
|--------------------------------------------------------------------|
SUB setup_Heals
/if (${Debug}) /echo |- setup_Heals ==>

	/if (${Ini[${Character_Ini},Heals].Length}) {
	
|------------------------------------------------------------------------------------------------------MainTank_Heal.
	/call INItoVar "${Character_Ini},Heals,Main Tank" MainTank string outer
	
	/if (${Defined[MainTank]}) {
		/call INItoVar "${Character_Ini},Heals,Main Tank Heal (Spell/Gem/Pct)" MainTank_Heal string outer
		
		/if (${Defined[MainTank_Heal]}) {
			/if (!${Int[${Me.Book[${MainTank_Heal.Arg[1,/]}]}]}) {
				/echo ERROR: @[MainTank_Heal] - I do not have [${MainTank_Heal.Arg[1,/]}] in my spellbook. Disabling MainTank_Heals.
				/deletevar MainTank_Heal
			}
		}
		
	|-------------------------------------------------------------------------------------------------------MTQuick_Heal.
		/call INItoVar "${Character_Ini},Heals,Main Tank Quick Heal (Spell/Gem/Pct)" MTQuick_Heal string outer
		/if (${Defined[MTQuick_Heal]}) {
			/if (!${Int[${Me.Book[${MTQuick_Heal.Arg[1,/]}]}]}) {
				/echo ERROR: @[MainTank_QuickHeal] - I do not have [${MTQuick_Heal.Arg[1,/]}] in my spellbook. Disabling MainTank_QuickHeals.
				/deletevar MTQuick_Heal
			}
		}
	}	
	
|---------------------------------------------------------------------------------------------------------Patch_Heal.
	/call INItoArray "${Character_Ini},Heals,Priority Target#" priorityTargets
	/call INItoVar "${Character_Ini},Heals,Patch Heal (Spell/Gem/Pct)" Patch_Heal string outer
	/if (${Defined[Patch_Heal]}) {
		/if (!${Int[${Me.Book[${Patch_Heal.Arg[1,/]}]}]}) {
			/echo ERROR: @[Patch_Heal] - I do not have [${Patch_Heal.Arg[1,/]}] in my spellbook. Disabling Patch_Heals.
			/deletevar Patch_Heal
		}
	}
	
|----------------------------------------------------------------------------------------------------------------HoT.
	/call INItoVar "${Character_Ini},Heals,HoT Mode (MTOnly/All/Off)" HoT_Mode string outer
	/if (${Defined[HoT_Mode]}) {
		/if (!${Select[${HoT_Mode},MTOnly,All,Off]}) {
			/echo ERROR: @[HoT_Mode] - [${HoT_Mode}] is not a recognized heal over time mode, disabling HoT functions. Please review [${Character_Ini}] and reload e3.
			/deletevar HoT_Mode
		} else {
			/if (${HoT_Mode.Equal[off]}) {
				/deletevar HoT_Mode
			} else {
				/call INItoVar "${Character_Ini},Heals,Heal Over Time (Spell/Gem/Pct)" HoT_Spell string outer
				/if (${Defined[HoT_Spell]}) {
					/if (!${Int[${Me.Book[${HoT_Spell.Arg[1,/]}]}]}) {
						/echo ERROR: @[Heal_Over_Time] - I do not have [${HoT_Spell.Arg[1,/]}] in my spellbook. Disabling Heal_Over_Time.
						/deletevar HoT_Mode
						/deletevar HoT_Spell
					}
				}
			}
		}
	}
	
|----------------------------------------------------------------------------------------------Pet_Heal.
	/call INItoVar "${Character_Ini},Heals,Pet Heal (Spell/Gem/Pct)" Pet_Heal string outer
	/if (${Defined[Pet_Heal]}) {
		/if (!${Int[${Me.Book[${Pet_Heal.Arg[1,/]}]}]}) {
			/echo ERROR: @[Pet_Heal] - I do not have [${Pet_Heal.Arg[1,/]}] in my spellbook. Disabling Pet_Heals.
			/deletevar Pet_Heal
		}
	}
	
	/call IniToVar ${Character_Ini},Heals,Join_HealChain(On/Off) join_HealChain bool outer
	/if (${join_HealChain}) {
		
	|---------------------------------------------------Check for necessary spells.
		/if (${Me.Class.ShortName.Equal[Clr]} && !${Me.Book[Complete Heal]} || ${Me.Class.ShortName.Equal[Dru]} && !${Me.Book[Karana's Renewal]}) {
			/echo ERROR: @[Join_HealChain(On/Off)] - I don't have [${If[${Me.Class.ShortName.Equal[Clr]},Complete Heal,Karana's Renewal]}] in my spellbook, disabling HealChain.
			/deletevar join_HealChain
		} else {
			| Add Heal Chain variables here |
			
		}
	}
	/if (${Defined[MainTank_Heal]} || ${Defined[MTQuick_Heal]} || ${Defined[Patch_Heal]} || ${Defined[HoT_Spell]} || ${Defined[Pet_Heal]} || ${join_HealChain}) {
		/declare Heals bool outer TRUE
		/declare recentHeal_Timer timer outer
		| /call event_CheckGroup
	}
}
	
/if (${Debug}) /echo <== setup_Heals -|
/RETURN