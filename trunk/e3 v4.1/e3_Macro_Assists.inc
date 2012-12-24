|--------------------------------------------------------------------|
|- e3_Macro_Assists.inc	( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ										-|
|--------------------------------------------------------------------|
|- Contains assisting related functions for e3.mac.					-|
|--------------------------------------------------------------------|



#event Assist "<#1#> Assist me"
#event Assist "#1# tell your party, 'Assist me'"
#event Assist "#1# tells the group, 'Assist me'"
#event Assist "#1# tells you, 'Assist me'"

#event Assist "<#1#> #2# Assist me"
#event Assist "#1# tell your party, '#2# Assist me'"
#event Assist "#1# tells the group, '#2# Assist me'"
#event Assist "#1# tells you, '#2# Assist me'"
|--------------------------------------------------------------------------------|
|- Engages assist functions on ChatSender, and bots who are in the same zone.	-|
|--------------------------------------------------------------------------------|
|- Use, 'Assist on ${Target.ID}' in /tell, /group, or /bc.						-|
|--------------------------------------------------------------------------------|
SUB event_Assist(line, ChatSender, SpecifiedBots)
| /varset Debug 1
/if (${Debug}) /echo |- event_EngageAssist ==>
	
|------------------------------------------------------------------------------------------------------------------Clean-up / Reset Variables.
	/if (${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}
	/if (${ChatSender.Equal[you]}) /varset ChatSender ${Me.CleanName}
	/if (${Defined[PauseTarget]}) /deletevar PauseTarget
	
|--------------------------------------------------------------------Check ChatSender zone.
	/if (!${SpawnCount[pc ${ChatSender}]}) {
		/if (${Debug}) /echo |- event_EngageAssist -| [${ChatSender}] and I are not in the same zone.
	} else {
	
	|--------------------------------------------------------Check SpecifiedBots
		/if (!${Defined[SpecifiedBots]} || ${SpecifiedBots.Equal[all]} || ${Select[${Me.CleanName},${SpecifiedBots}]} || ${Select[${Me.Class.ShortName},${SpecifiedBots}]} || ${Select[${Me.Class},${SpecifiedBots}]} || ${SpecifiedBots.Equal[melee]} && ${Select[${Me.Class.ShortName},${MeleeClasses}]} && ${AssistType.Equal[Melee]} || ${SpecifiedBots.Equal[ranged]} && ${Select[${Me.Class.ShortName},${CasterClasses}]} || ${SpecifiedBots.Equal[ranged]} && ${AssistType.Equal[Ranged]} || ${SpecifiedBots.Equal[casters]} && ${Select[${Me.Class.ShortName},${CasterClasses}]} || ${SpecifiedBots.Equal[Priests]} && ${Select[${Me.Class.ShortName},${PriestClasses}]}) {
	
			/declare MobID int local ${NetBots[${ChatSender}].TargetID}
			
			/if (${Spawn[id ${MobID}].Type.Equal[Corpse]} || !${Select[${Spawn[id ${MobID}].Type},${AcceptableTargetTypes}]}) {
				/if (${Debug}) /echo |- event_EngageAssist -| [${Spawn[id ${MobID}].CleanName}] is not an acceptable TargetType.
			} else {	
				
			|-------------------------------------------------------------------------------------Check target distance.
				/if (${Spawn[${MobID}].Distance} > ${AssistBreakDistance}) {
					/docommand ${ChatToggle} ${Spawn[${MobID}].CleanName} is too far away.
				} else {
				
				|------------------------------------------------------------------Wait to catch up to group, before engage.
					/if (${Following} && ${Spawn[${FollowTarget}].Distance} > ${LeashLength} && ${Me.Moving}) {
						/declare Assist_Moving_Timer timer local 100
						:movetoAssist_Loop
						/call Background_Events
						/if (${Spawn[${FollowTarget}].Distance} > ${LeashLength} && ${Assist_Moving_Timer}) /goto :movetoAssist_Loop
					}
					
				|--------------------------------------------------------------------------------------Disengage follow.
					/if (${Stick.Active}) /squelch /stick off
					/if (${NetAdvPath.State}) /squelch /netfollow pause
					
				|----------------------------------------------------------------------------------------------------------------Set Assist Variables.
					/varset Assisting TRUE
					/varset AssistTarget ${MobID}
					
				|--------------------------------If I am the assist caller, or I am feigning, don't allow macro control.
					/if (${ChatSender.Equal[${Me.CleanName}]} || ${Me.Feigning}) {
						/varset AllowControl FALSE
					} else {
						/varset AllowControl TRUE
					}
				
				|----------------------------------------------------------------------------------------------If the macro is controlling me, engage.
					/if (${AllowControl}) {
						/if (${Verbosity}) /docommand ${ChatToggle} Attacking - ${AssistTarget} - ${Spawn[${AssistTarget}].CleanName}
						
					|-------------------------------------------------------------------------------Target AssistTarget.
						/if (${Target.ID} != ${AssistTarget}) /call TrueTarget ${AssistTarget}
						
					|--------------------------------------------------------------------------------------Engage Stick.
						/if (${AssistType.Equal[Melee]}) {
							/attack on
							/if (${Select[${AssistStickDistance},MaxMelee,Auto]}) /call Get_AssistStickDistance
							/call StickToAssistTarget
						} else /if (${AssistType.Equal[Ranged]}) {
							/autofire on
							/squelch /stick hold moveback ${RangedStickDistance}
						}
						
					|--------------------------------------------------------------------------------------Send in pets.
						/if (${Me.Pet.ID}) {
							/if (${Target.ID} != ${AssistTarget}) /call TrueTarget ${AssistTarget}
							/delay 3
							/if (${Debug}) /echo |- Event_Assist -| Pet Check: Sending pet in
							/pet attack
						}
					}
					
					/if (${Debug}) /echo |- event_EngageAssist -| Assist=${Assisting} || AssistTarget=${AssistTarget} || AllowControl=${AllowControl}
				}
			}
		}
	}
	
/if (${Debug}) /echo <== event_EngageAssist -|
| /varset Debug 0
/RETURN



|--------------------------------------------------------|
|- Calculates a stick distance to your AssistTarget.	-|
|--------------------------------------------------------|
SUB Get_AssistStickDistance
/if (${Debug}) /echo |- Set_AssistStickDistance ==>

	/varset AssistStickDistanceSetting ${Math.Calc[${Spawn[${Target.ID}].MaxRangeTo}*.8].Int}

|-----------------------------------------------------------------------------------------Check MaxRangeTo, make sure it is not out of bounds.
	/if (${AssistStickDistanceSetting} > 25) /varset AssistStickDistanceSetting 25

/if (${Debug}) {
	/echo |- Set_AssistStickDistance -| AssistStickDistanceSetting = [${AssistStickDistanceSetting}]
	/echo <== Set_AssistStickDistance -|
}
/RETURN



|----------------------------------------|
|- Engages /stick on an AssistTarget.	-|
|----------------------------------------|
SUB StickToAssistTarget
| /varset Debug 1
/if (${Debug}) /echo |- StickToAssistTarget ==>
	
	/if (${AssistStickPoint.Equal[behind]}) {
		
		/if (${Debug}) /echo |- StickToAssistTarget -| Attempting to stick behind [${AssistStickDistanceSetting}].
		/squelch /stick moveback snaproll 15 uw
		/delay 10 !${Me.Moving} || ${Target.Distance} < 15
		/squelch /stick hold moveback behind ${AssistStickDistanceSetting} uw	
		/delay 10 ${Me.Moving}
		
	} else /if (${AssistStickPoint.Equal[behindonce]}) {
	
		/if (${Debug}) /echo |- StickToAssistTarget -| Attempting to stick behindonce [${AssistStickDistanceSetting}].
		/squelch /stick moveback snaproll 15 uw
		/delay 10 !${Me.Moving} || ${Target.Distance} < 20
		/squelch /stick hold moveback behindonce ${AssistStickDistanceSetting} uw
		/delay 10 ${Me.Moving}
		
	} else /if (${AssistStickPoint.Equal[pin]}) {
	
		/if (${Debug}) /echo |- StickToAssistTarget -| Attempting to stick pin [${AssistStickDistanceSetting}].
		/squelch /stick moveback snaproll 15 uw
		/delay 10 !${Me.Moving} || ${Target.Distance} < 20
		/squelch /stick hold moveback pin ${AssistStickDistanceSetting} uw
		/delay 10 ${Me.Moving}
		
	} else /if (${AssistStickPoint.Equal[front]}) {
	
		/if (${Debug}) /echo |- StickToAssistTarget -| Attempting to stick front [${AssistStickDistanceSetting}].
		/squelch /stick hold front ${AssistStickDistanceSetting} uw
		/delay 10 ${Me.Moving}
		
	} else /if (${AssistStickPoint.Equal[!front]}) {
		
		/if (${Debug}) /echo |- StickToAssistTarget -| Attempting to stick !front [${AssistStickDistanceSetting}].
		/squelch /stick moveback snaproll 15 uw
		/delay 10 !${Me.Moving} || ${Target.Distance} < 20
		/squelch stick hold moveback !front ${AssistStickDistanceSetting} uw
		/delay 10 ${Me.Moving}
	}
	
	/declare miscTimer timer local 50
	:waitToStop
	/if (${Debug}) /echo |- StickToAssistTarget -| :waitToStop
	
	/doevents getCloser
	/if (${Me.Moving}) {
		/if (${miscTimer}) /goto :waitToStop
	}

/if (${Debug}) /echo <== StickToAssistTarget -|	
| /varset Debug 0
/RETURN



|----------------------------------------------------|
|- Checks range and combat status while assisting.	-|
|----------------------------------------------------|
SUB check_AssistStatus
/if (${Debug}) {
	/echo |- check_AssistStatus ==>
	/echo |- check_AssistStatus -| AssistTarget = [${AssistTarget}] || AllowControl = [${AllowControl}] || Distance check = [${Spawn[id ${AssistTarget}].Distance} < ${AssistBreakDistance}] || TargetType = [${Spawn[id ${AssistTarget}].Type}]
}

|-----------------------------------------------------------------Check range.
	/if (${Spawn[${AssistTarget}].Distance} > ${AssistBreakDistance}) {
		/if (${Debug}) /echo [${Spawn[${AssistTarget}].CleanName} - ${Spawn[${AssistTarget}].ID}] is out of range,  holding assist functions.
	} else {
	
	|------------------------------------------------------------------------------Check feigning
		/if (${Me.Feigning}) {
			/if (${Debug}) /echo I am feigned, holding assist functions. 
		} else  {
	
		|---------------------------------------------------------------------------Check AllowControl bool.
			/if (!${AllowControl}) {
				/if (${Debug}) /echo Macro control is not allowed, holding assist functions. 
			} else {
			
			|-------------------------------------------------------------------------------------------------Melee functions
				/if (${AssistType.Equal[Melee]}) {
					
				|--------------------------------------------------------------------------------------Check Target.
					/if (${Target.ID}!=${AssistTarget}) /call TrueTarget ${AssistTarget}
					
				|---------------------------------------------------------------------------------------------------------If I'm not autofiring, and out of range, stick to the assist target.s
					/call check_IsLookingAt	
					/if (!${Me.AutoFire} && ${Target.Distance} > ${AssistStickDistanceSetting} || ${AssistStickDistance.Equal[MaxMelee]} &&  ${Target.Distance} < ${Math.Calc[${AssistStickDistanceSetting}*.7]} || ${Me.Combat} && ${Macro.Return} && !${Select[${AssistStickPoint},front,behindonce]}) /call StickToAssistTarget
				
					/if (!${Me.AutoFire} && !${Me.Combat}) /attack on
					
			|-------------------------------------------------------------------------------------------------Ranged functions
				} else /if (${AssistType.Equal[Ranged]}) {
				
				|------------------------------------------------------------------------------------Check Target.
					/if (${Target.ID}!=${AssistTarget}) /call TrueTarget ${AssistTarget}
					
					/if (!${Me.Combat} && ${Target.Distance} < 35 || ${Target.Distance} > ${RangedStickDistance}) /squelch /stick hold moveback ${RangedStickDistance}
					
					/if (!${Me.AutoFire}) /autofire on
				}
			}
		}
		/varset combatTimer ${combatDelay}
	}
	
	
/if (${Debug}) /echo <== check_AssistStatus -|
/RETURN



|--------------------------------------------------------|
|- This script was taken from,							-|
|- genbot.mac											-|
|- botcombat.inc										-|
|- Bot combat module.									-|
|- Version 13.4.0										-|
|- Date:07/08/2005										-|
|- Originally by ascii38, maintained by Mortefreddo.	-|
|- This script has been modified to work with e3.mac.	-|
|--------------------------------------------------------|
SUB check_IsLookingAt
/if (${Debug}) /echo |- CheckIsLookingAt ==>

	/declare MobHeading int local 0 
	/declare HeadingToPC int local 
	/declare DeltaX local float 
	/declare DeltaY local float 
	/declare HeadingDelta local float
	/declare FacingMe int local 0
	
	/varcalc MobHeading ${Spawn[id ${AssistTarget}].Heading.Degrees}
	/varcalc DeltaX ${Spawn[id ${Me.ID}].X}-${Spawn[id ${AssistTarget}].X}
	/varcalc DeltaY ${Spawn[id ${Me.ID}].Y}-${Spawn[id ${AssistTarget}].Y}
	
	/if (${DeltaX}>0) {
		/varcalc HeadingToPC ${Math.Atan[${DeltaY}/${DeltaX}]}+270 
	} else /if (${DeltaX}<0) {
		/varcalc HeadingToPC ${Math.Atan[${DeltaY}/${DeltaX}]}+90 
	} else {
		/if (${DeltaY}>0) {
			/varcalc HeadingToPC 90 
		} else {
			/varcalc HeadingToPC 270 
		}
	}

	/varcalc HeadingDelta ${Math.Abs[${HeadingToPC}-${MobHeading}]}
	
	/if (${HeadingDelta}<4 || ${HeadingDelta}>356) {
		/if (${Debug}) /echo |- CheckIsLookingAt -| [${Spawn[id ${AssistTarget}].CleanName}] is facing me.
		/varset FacingMe 1
	}

/if (${Debug}) /echo <== CheckIsLookingAt -|
/RETURN ${FacingMe}



|----------------------------------------------------------------|
|- Checks to see if the AssistTarget has died, calls Assistoff.	-|
|----------------------------------------------------------------|
SUB check_AssistTarget
/if (${Debug}) /echo |- check_AssistTarget ==>

	|-------------------------------------------Check to see if our assist target has died.
	/if (${Spawn[${AssistTarget}].Type.Equal[Corpse]} || ${Select[${Spawn[${AssistTarget}].Type.Equal[Corpse]},NULL]}) {
		/call AssistOff
	}
	/if (${Defined[combatTimer]}) /varset combatTimer ${combatDelay}
	
/if (${Debug}) /echo <== check_AssistTarget -|
/RETURN



|--------------------------------------------------------------------|
|- Backs off an AssistTarget, and labels them as a 'PauseTarget'.	-|
|- You must manually call engage to re-engage a PauseTarget.		-|
|--------------------------------------------------------------------|
|- Use, '/backoff', or, 'Back off' in /tell, /group, or /bc.		-|
|--------------------------------------------------------------------|
#event BackOff "<#1#> Back Off"
#event BackOff "#1# tells the group, 'Back Off'"
#event BackOff "#1# tells you, 'Back Off'"
SUB Event_BackOff(line, ChatSender)
/if (${Debug}) /echo |- Event_BackOff ==>

	/if (${Assisting} && ${AllowControl} && ${Verbosity}) /docommand ${ChatToggle} Backing off.
	/if (!${Defined[PauseTarget]}) /declare PauseTarget int outer ${AssistTarget}
	/if (${Me.Casting.ID}) /call interrupt
	/if (${AllowControl} && ${Target.ID}) /squelch /target clear
	/if (!${Select[${Me.Pet},NULL]}) /pet back off
	/if (${Me.Combat}) /attack off
	/if (${Me.AutoFire}) /autofire off
	/if (${Debuff_Targets.Size}) /deletevar Debuff_Targets
	/if (${DoT_Targets.Size}) /deletevar DoT_Targets
	/call AssistOff
	
/if (${Debug}) /echo <== Event_BackOff -|
/RETURN



|----------------------------------------------------------------|
|- Turns off Assist_Functions, and resets assisting variables.	-|
|----------------------------------------------------------------|
SUB AssistOff
/if (${Debug}) /echo |- AssistOff -| ==>
	
|-----------------------------------------------------------------------------------------------------------Interrupt spells, turn off attack.
	/if (${Me.Combat} && ${Target.Type.Equal[PC]} || ${Me.Combat} && !${Target.ID} || ${Me.Combat} && ${Target.Type.Equal[corpse]}) /attack off
	/if (${Me.AutoFire} && ${Target.Type.Equal[PC]} || ${Me.Combat} && !${Target.ID} || ${Me.Combat} && ${Target.Type.Equal[corpse]}) /autofire off
	
|-----------------------------------------------------------Reset assist variables.
	/varset Assisting FALSE
	/varset AssistTarget 0
	/varset AllowControl FALSE
	
|-------------------------------------------------------------Reset Nuke variables.
	/if (${Defined[NukeTimer]}) /varset NukeTimer 0
	
|--------------------------------------------------Check loot, and reacquire follow.
	/if (${Select[${Stick},ON]}) /squelch /stick off
	/if (${Following}) /call AquireFollow
	/doevents YourKill
	
/if (${Debug}) {
	/echo |- AssistOff -| Assisting: [${Assisting}] || AssistTarget: [${AssistTarget}]
	/echo |- AssistOff -| <==
}
/RETURN



#EVENT getCloser "Your target is too far away, get closer!"
SUB EVENT_getCloser
| /varset Debug 1
/if (${Debug}) /echo |- EVENT_getCloser ==>

	/if (${Assisting} && ${AllowControl} && !${getCloserTimer}) {
		
		/call CreateTimer getCloserTimer 30
		
	|---------------------------------------------------------------------------------------If the spawn has moved, stick back to the target
		/if (${Spawn[id ${AssistTarget}].Distance} <= ${AssistStickDistanceSetting} && ${AssistStickDistanceSetting} > 12) {
			/varcalc AssistStickDistanceSetting ${AssistStickDistanceSetting} - 2
			/call StickToAssistTarget
		}

		/if (${Debug}) /echo |- EVENT_getCloser -| AssistStickDistanceSetting has been set to [${AssistStickDistanceSetting}]
	}

/if (${Debug}) /echo <== EVENT_getCloser -|
| /varset Debug 0
/RETURN



|------------------------------------------------|
|- Uses combat abilities, AAs, and disciplines.	-|
|------------------------------------------------|
SUB CombatAbilities
| /varset Debug 1
/if (${Debug}) /echo |- CombatAbilities ==>

	/if (${Target.ID} != ${AssistTarget}) {
		/if (${Debug}) /echo |- CombatAbilities -| Abilities are only used on AssistTargets.
	} else {
	
	|----------------------------------------------------------------Check Target distance.
		/if (${Target.Distance} > ${Spawn[${Target.ID}].MaxRangeTo}) {
			/if (${Debug}) /echo |- CombatAbilities -| I am too far away from [${Spawn[id ${AssistTarget}].CleanName}] to use abilities.
		} else {
		
			/declare i int local
		
		|----------------------------------------------------------------------------------------For 1 to the size of MyAbilities array, use each ability.
			/for i 1 to ${MyAbilities.Size}
			
		|-----------------------------------------------------------------------If this is an Ability.
			/if (${MyAbilities[${i}].Arg[2,/].Equal[ability]}) {
				
			|---------------------------------------------------------------------If the Ability is not ready.
				/if (!${Me.AbilityReady[${MyAbilities[${i}].Arg[1,/]}]}) {
					/if (${Debug}) /echo |- CombatAbilities -| Ability [${MyAbilities[${i}].Arg[1,/]}] is not ready.
				} else {
					
				|-----------------------------------------------------------------------Use Backstab.
					/if (${MyAbilities[${i}].Arg[1,/].Equal[Backstab]}) {
						/call Attempt_Backstab
						
				|-----------------------------------------------------------------------Use Bash.
					} else /if (${MyAbilities[${i}].Arg[1,/].Equal[Bash]}) {
						/call Attempt_Bash
						
				|-----------------------------------------------------------------------Use Taunt.
					} else /if (${MyAbilities[${i}].Arg[1,/].Equal[Taunt]}) {
						/call Attempt_Taunt
						
				|-----------------------------------------------------------------------Use a misc Ability.
					} else /if (${Me.AbilityReady[${MyAbilities[${i}].Arg[1,/]}]}) {
					
					|-----------------------------------------------------------------Misc.
						/doability "${MyAbilities[${i}].Arg[1,/]}"
					}
				}
				
		|-----------------------------------------------------------------------If this is an Alternate Ability.
			} else /if (${MyAbilities[${i}].Arg[2,/].Equal[alt]}) {
				/if (!${Me.AltAbilityReady[${MyAbilities[${i}].Arg[1,/]}]} || ${alt_Timer_${Me.AltAbility[${MyAbilities[${i}].Arg[1,/]}].ID}}) {
					/if (${Debug}) /echo |- CombatAbilities -| Alternate Ability [${MyAbilities[${i}].Arg[1,/]}] is not ready.
				} else {
					/call e3_Cast ${AssistTarget} "${MyAbilities[${i}].Arg[1,/]}" alt
				}
				
		|-----------------------------------------------------------------------If this is a Discipline.
			} else /if (${MyAbilities[${i}].Arg[2,/].Equal[disc]}) {
				/if (!${Me.CombatAbilityReady[${MyAbilities[${i}].Arg[1,/]}]} || ${disc_Timer_${MyAbilities[${i}].Arg[1,/].Replace[ ,]}}) {
					/if (${Debug}) /echo |- CombatAbilities -| Discipline [${MyAbilities[${i}].Arg[1,/]}] is not ready.
				} else {
					
				|-----------------------------------------------------------Rogue Assassin's Strike.
					/if (${MyAbilities[${i}].Arg[1,/].Equal[Thief's Vengeance]} || ${MyAbilities[${i}].Arg[1,/].Equal[Assassin's Strike]} || ${MyAbilities[${i}].Arg[1,/].Equal[Kyv Strike]} || ${MyAbilities[${i}].Arg[1,/].Equal[Ancient: Chaos Strike]}) {
						/call Attempt_Backstab
					
				|-----------------------------------------------------------Warrior Bellow.	
					} else /if (${MyAbilities[${i}].Arg[1,/].Equal[Provoke]} || ${MyAbilities[${i}].Arg[1,/].Equal[Bellow]} || ${MyAbilities[${i}].Arg[1,/].Equal[Berate]} || ${MyAbilities[${i}].Arg[1,/].Equal[Incite]} || ${MyAbilities[${i}].Arg[1,/].Equal[Bellow of the Mastruq]} || ${MyAbilities[${i}].Arg[1,/].Equal[Ancient: Chaos Cry]}) {
						/call Attempt_Bellow
						
				|-------------------------------------------------------All other disciplines.
					} else {
						/if (!${disc_Timer_${MyAbilities[${i}].Arg[1,/].Replace[ ,]}}) /call useDisc "${MyAbilities[${i}].Arg[1,/]}"
					}
				}
			}
			
			/delay 2
			/next i
		}
	}

/if (${Debug}) /echo <== CombatAbilities -|
| /varset Debug 0
/RETURN



SUB Attempt_Bash
/if (${Debug}) /echo |- Attempt_Bash ==>

	/if (!${Select[${Me.Inventory[Offhand].Type},Shield]} && !${Me.AltAbility[2 Hand Bash]}) {
		/if (${Debug}) /echo |- Attempt_Bash -| You must equip a shield, or train in the alternate ability "2 Hand Bash" to use the ability [Bash].
	} else {
		/doability Bash
	}
	
/if (${Debug}) /echo <== Attempt_Bash -|
/RETURN



SUB Attempt_Taunt
/if (${Debug}) /echo |- Attempt_Taunt ==>

	/if (!${do_Taunt}) {
		/if (${Debug}) /echo |- Attempt_Taunt -| AutoAggro is [${do_Taunt}], skipping taunt.
	} else {
		|**
		/if (${Select[${Me.TargetOfTarget},${Me.CleanName}]}) {
			/if (${Debug}) /echo |- Attempt_Target -| I currently have aggro on [${Target.CleanName}], skipping taunt.
		} else {
			/doability Taunt
		}
		**|
		/doability Taunt
	}

/if (${Debug}) /echo <== Attempt_Taunt -|
/RETURN


| add arguments for additional casting parameters
|----------------------------------------------------------------------------------------|
|- Casts nukes listed in the [Nukes] section of the Character_Ini, on AssistTargets.	-|
|----------------------------------------------------------------------------------------|
SUB Nukes
/if (${Debug}) /echo |- Nukes ==>
	
	/declare i int local
	/for i 1 to ${MyNukes.Size}

	|---------------------------------------------------------------Cast a spell.
		/if (${MyNukes[${i}].Arg[2,/].Find[gem]}) {
		
		|--------------------------------------Check minimum mana to cast the spell.
			/if (${Me.PctMana}>=${Int[${MyNukes[${i}].Arg[3,/]}]}) {
			
			|--------------------------------------------------------------Cast MyNukes[i], if sticking, dont sit to med, if healing, only wait 6 seconds to mem, otherwise take 12
				/if (${Debug}) /echo |- Nukes -| Attempting to cast [${MyNukes[${i}].Arg[1,/]}] on [${Spawn[id ${AssistTarget}].CleanName} - ${AssistTarget}] with a retry timer of [${If[${StickOn} || ${Me.Combat},0s,${If[${Me.Gem[${MyNukes[${i}].Arg[1,/]}]},0s,9s]}]}] and a delay of [${MyNukes[${i}].Arg[4,/]}] before my next nuke.
				/call e3_Cast ${AssistTarget} "${MyNukes[${i}].Arg[1,/]}" ${MyNukes[${i}].Arg[2,/]} ${If[${StickOn} || ${Me.Combat},0s,${If[${Me.Gem[${MyNukes[${i}].Arg[1,/]}]},0s,9s]}]}
			}
			
	|-----------------------------------------------------------------Cast an item.
		} else /if (${Select[${MyNukes[${i}].Arg[2,/]},ITEM,Item,item]}) {
			/if (${Debug}) /echo |- Nukes -| Attempting to cast [${MyNukes[${i}].Arg[1,/]} - ITEM] on [${Spawn[id ${AssistTarget}].CleanName} - ${AssistTarget}] with a delay of [${MyNukes[${i}].Arg[4,/]}] before my next nuke.
			/call e3_Cast ${AssistTarget} "${MyNukes[${i}].Arg[1,/]}" item
			
	|------------------------------------------------------------------Cast an AA.	
		} else /if (${Select[${MyNukes[${i}].Arg[2,/]},ALT,Alt,alt]}) {
			/if (${Debug}) /echo |- Nukes -| Attempting to cast [${MyNukes[${i}].Arg[1,/]} - ALT] on [${Spawn[id ${AssistTarget}].CleanName} - ${AssistTarget}] with a delay of [${MyNukes[${i}].Arg[4,/]}] before my next nuke.
			/call e3_Cast ${AssistTarget} "${MyNukes[${i}].Arg[1,/]}" alt
		}
		
	/if (!${ActionTaken}) /next i
	
|--------------------------------------------------------------------If cast was a success, update NukeTimer
	/if (${Select[${castReturn},CAST_SUCCESS]}) /varset NukeTimer ${MyNukes[${i}].Arg[4,/]}
	
/if (${Debug}) /echo <== Nukes -|
/RETURN



|----------------------------------------------------------------------------------------|
|- Casts spells listed in the [Debuffs] section of the Character_Ini, on AssistTargets.	-|
|----------------------------------------------------------------------------------------|
SUB Debuffs_OnAssist
/if (${Debug}) /echo |- Debuffs_OnAssist ==>

	/declare x int local
	/for x 1 to ${Assist_Debuffs.Size}
			
	|--------------------------------------------------------------------Cast a spell.
		/if (${Assist_Debuffs[${x}].Arg[2,/].Left[3].Equal[gem]}) {
			/if (${Me.PctMana}>${Assist_Debuffs[${x}].Arg[3,/].Replace[m,]} && ${Spawn[id ${AssistTarget}].Distance}<=${Spell[${Assist_Debuffs[${x}].Arg[1,/]}].MyRange} && !${${AssistTarget}_${Spell[${Assist_Debuffs[${x}].Arg[1,/]}].ID}}) {
				/if (${Debug}) /echo |- Debuffs_OnAssist -| Attempting to cast [${Assist_Debuffs[${x}].Arg[1,/]}] on [${Spawn[id ${AssistTarget}].CleanName} - ${AssistTarget}] with a retry timer of [${If[${StickOn} || ${Me.Combat},0s,${If[${Me.Gem[${Assist_Debuffs[${x}].Arg[1,/]}]},0s,9s]}]}].
				/call Check_LongTermSpell ${AssistTarget} "${Assist_Debuffs[${x}].Arg[1,/]}" ${Assist_Debuffs[${x}].Arg[2,/]} ${If[${StickOn} || ${Me.Combat},0s,${If[${Me.Gem[${Assist_Debuffs[${x}].Arg[1,/]}]},0s,9s]}]}
			}
		
	|-----------------------------------------------------------------Cast an item.
		} else /if (${Assist_Debuffs[${x}].Arg[2,/].Equal[item]}) {
			/if (${Spawn[id ${AssistTarget}].Distance}<=${Spell[${FindItem[${Assist_Debuffs[${x}].Arg[1,/]}].Spell}].MyRange} && !${${AssistTarget}_${FindItem[${Assist_Debuffs[${x}].Arg[1,/]}].ID}}) {
				/call Check_LongTermSpell ${AssistTarget} "${Assist_Debuffs[${x}].Arg[1,/]}" item
			}
			
	|------------------------------------------------------------------Cast an AA.
		} else /if (${Assist_Debuffs[${x}].Arg[2,/].Equal[alt]}) {
			/call Check_LongTermSpell ${AssistTarget} "${Assist_Debuffs[${x}].Arg[1,/]}" alt
		}
	
	/if (!${ActionTaken}) /next x
				
/if (${Debug}) /echo <== Debuffs_OnAssist -|
/RETURN



|----------------------------------------------------------------------------------------|
|- Casts Spells listed in the [DoTs] section of the Character_Ini, on AssistTargets.	-|
|----------------------------------------------------------------------------------------|
SUB DoTs_OnAssist
| /varset Debug 1
/if (${Debug}) /echo |- DoTs_OnAssist ==>

	/declare x int local
	/for x 1 to ${Assist_DoTs.Size}
			
	|--------------------------------------------------------------------Cast a spell.
		/if (${Assist_DoTs[${x}].Arg[2,/].Left[3].Equal[gem]}) {
			/if (${Me.PctMana}>${Assist_DoTs[${x}].Arg[3,/].Replace[m,]} && ${Spawn[id ${AssistTarget}].Distance}<=${Spell[${Assist_DoTs[${x}].Arg[1,/]}].MyRange} && !${${AssistTarget}_${Spell[${Assist_DoTs[${x}].Arg[1,/]}].ID}}) {
				/call Check_LongTermSpell ${AssistTarget} "${Assist_DoTs[${x}].Arg[1,/]}" ${Assist_DoTs[${x}].Arg[2,/]} ${If[${StickOn} || ${Me.Combat},0s,${If[${Me.Gem[${Assist_DoTs[${x}].Arg[1,/]}]},0s,9s]}]}
			}
		
	|-----------------------------------------------------------------Cast an item.
		} else /if (${Assist_DoTs[${x}].Arg[2,/].Equal[item]}) {
			/if (${Spawn[id ${AssistTarget}].Distance}<=${Spell[${FindItem[${Assist_DoTs[${x}].Arg[1,/]}].Spell}].MyRange} && !${${AssistTarget}_${FindItem[${Assist_DoTs[${x}].Arg[1,/]}].ID}}) {
				/call Check_LongTermSpell ${AssistTarget} "${Assist_DoTs[${x}].Arg[1,/]}" ${Assist_DoTs[${x}].Arg[2,/]}
			}
			
	|------------------------------------------------------------------Cast an AA.
		} else /if (${Assist_DoTs[${x}].Arg[2,/].Equal[alt]}) {
			/call Check_LongTermSpell ${AssistTarget} "${Assist_DoTs[${x}].Arg[1,/]}" ${Assist_DoTs[${x}].Arg[2,/]}
		}
	
	/if (!${ActionTaken}) /next x
				
/if (${Debug}) /echo <== DoTs_OnAssist -|
| /varset Debug 0
/RETURN



|---------------------------------------------------------------------------------------------|
|- Validates targets and adds them to a list of Debuff_Targets. Used for Debuffs_OnCommand.	 -|
|---------------------------------------------------------------------------------------------|
|- Use, '/Debuff' or 'Debuffs on ${Target.ID}' in /tell, /group, or /bc.					 -|
|---------------------------------------------------------------------------------------------|
#event DebuffsOn "<#2#> Debuffs on #1#"
#event DebuffsOn "#2# tells you, 'Debuffs on #1#'"
#event DebuffsOn "#2# tells the group, 'Debuffs on #1#"
SUB Event_DebuffsOn(line, int DebuffTarget, ChatSender)
/if (${Debug}) /echo |- event_DebuffsOn ==>
	
|-----------------------------------Check if this character debuffs to cast.
	/if (${Command_Debuffs.Size}) {
	
	|-------------------------------------Check that the ChatSender is in the same zone, and that debuffs were not called on a corpse.
		/if (${SpawnCount[pc ${ChatSender}]} && ${Spawn[id ${DebuffTarget}].Type.NotEqual[Corpse]}) {
			
		|---------------------------------------------------------------------------Check range to the debuff target.
			/if (${Spawn[id ${DebuffTarget}].Distance} > ${AssistBreakDistance}) {
				/docommand ${ChatToggle} ${Spawn[${MobID}].CleanName} is too far away.
			} else {
				
		|--------------------------------------------------------Add the debuff target to the Debuff_Targets array.
			
			|-------------------------------------------If the array is not already defined, declare it.
				/if (!${Defined[Debuff_Targets]}) {
					/if (${Debug}) /echo |- Event_AddDebuffTarget -| Adding: [${DebuffTarget}] to DebuffTargets array
					/call BuildArray Debuff_Targets ${DebuffTarget}_${Zone.ID}		
				} else {
				
				|------------------Else, if the array is already defined, make sure this target is not being added twice.
					/declare i int local
					/for i 1 to ${Debuff_Targets.Size}
						/if (${Debuff_Targets[${i}].Arg[1,_]} == ${DebuffTarget}) {
							/docommand ${ChatToggle} I am already debuffing ${Spawn[${DebuffTarget}].CleanName}.
							/RETURN
						}
					/next i
					
				|-------------------------------------If the target was not found on the DebuffTargets array, add it.
					/call BuildArray Debuff_Targets ${DebuffTarget}_${Zone.ID}
				}
			}
		}
	}

/if (${Debug}) /echo <== event_DebuffsOn -|
/RETURN



|--------------------------------------------------------------------|
|- Casts Command_Debuffs on targets listed in DebuffTargets array.	-|
|--------------------------------------------------------------------|
SUB Debuffs_OnCommand
/if (${Debug}) /echo |- Debuffs_OnCommand ==>

|----------------------------------------------------------Declare counting variables.
	/declare i int local
	/declare x int local
	
|----------------------------------------For each target on the Debuff_Targets array.
	/for i 1 to ${Debuff_Targets.Size}
	
	|---------------------------------If the Debuff_Target has died, remove them from the array.
		/if (${Spawn[id ${Debuff_Targets[${i}].Arg[1,_]}].Type.Equal[Corpse]} || ${Select[${Spawn[id ${Debuff_Targets[${i}].Arg[1,_]}].Type.Equal[Corpse]},NULL]}) {
			/call RemoveArrayElement Debuff_Targets ${Debuff_Targets[${i}]}
		} else {
			
		|--------------------------Check Command_Debuffs on the Debuff_Target.
			/for x 1 to ${Command_Debuffs.Size}
		
			|--------------------------------------------------------------------Cast a spell.
				/if (${Command_Debuffs[${x}].Arg[2,/].Left[3].Equal[gem]}) {
					/if (${Me.PctMana}>${Command_Debuffs[${x}].Arg[3,/].Replace[m,]} && ${Spawn[${Debuff_Targets[${i}].Arg[1,_]}].Distance}<=${Spell[${Command_Debuffs[${x}].Arg[1,/]}].MyRange} && !${${Debuff_Targets[${i}].Arg[1,_]}_${Spell[${Command_Debuffs[${x}].Arg[1,/]}].ID}}) {
						/call Check_LongTermSpell ${Debuff_Targets[${i}].Arg[1,_]} "${Command_Debuffs[${x}].Arg[1,/]}" ${Command_Debuffs[${x}].Arg[2,/]} ${If[${StickOn} || ${Me.Combat},0s,${If[${Me.Gem[${MyNukes[${i}].Arg[1,/]}]},0s,9s]}]}
					}
				
			|-----------------------------------------------------------------Cast an item.
				} else /if (${Command_Debuffs[${x}].Arg[2,/].Equal[item]}) {
					/if (${Spawn[${Debuff_Targets[${i}].Arg[1,_]}].Distance}<=${Spell[${FindItem[${Command_Debuffs[${x}].Arg[1,/]}].Spell}].MyRange} && !${${Debuff_Targets[${i}].Arg[1,_]}_${FindItem[${Command_Debuffs[${x}].Arg[1,/]}].ID}}) {
						/call Check_LongTermSpell ${Debuff_Targets[${i}].Arg[1,_]} "${Command_Debuffs[${x}].Arg[1,/]}" ${Command_Debuffs[${x}].Arg[2,/]}
					}
					
			|------------------------------------------------------------------Cast an AA.
				} else /if (${Command_Debuffs[${x}].Arg[2,/].Equal[alt]}) {
					/call Check_LongTermSpell ${Debuff_Targets[${i}].Arg[1,_]} "${Command_Debuffs[${x}].Arg[1,/]}" ${Command_Debuffs[${x}].Arg[2,/]}
				}
			/if (!${ActionTaken}) /next x
	
		}
	/if (${Debuff_Targets.Size} && !${ActionTaken}) /next i
	
/if (${Debug}) /echo <== Debuffs_OnCommand -|
/RETURN



|------------------------------------------------|
|- Cancels debuffs on command on all targets. 	-|
|------------------------------------------------|
#event DebuffsOff "#*#Debuffs Off#*#"
SUB event_DebuffsOff
/if (${Debug}) /echo |- event_DebuffsOff ==>

	/if (${Debuff_Targets.Size}) {
		/if (${Verbosity}) /docommand ${ChatToggle} Ending Debuffs
		/deletevar Debuff_Targets
	}

/if (${Debug}) /echo <== event_DebuffsOff -|
/RETURN



|-----------------------------------------------------------------------------------------|
|- Validates targets and adds them to a list of DoT_Targets. Used for DoTs_OnCommand.	 -|
|-----------------------------------------------------------------------------------------|
|- Use, '/Dots' or 'DoTs on ${Target.ID}' in /tell, /group, or /bc.						 -|
|-----------------------------------------------------------------------------------------|
#event DoTsOn "<#2#> DoTs on #1#"
#event DoTsOn "#2# tells you#*#DoTs on #1#'"
#event DoTsOn "#2# tells the group, 'DoTs on #1#"
SUB Event_DoTsOn(line, int DoTTarget, ChatSender)
/if (${Debug}) /echo |- event_DoTsOn ==>

|-----------------------------------Check if this character DoTs to cast.
	/if (${Command_DoTs.Size}) {
	
	|-------------------------------------Check that the ChatSender is in the same zone, and that doTs were not called on a corpse.
		/if (${SpawnCount[pc ${ChatSender}]} && ${Spawn[id ${DoTTarget}].Type.NotEqual[Corpse]}) {
			
		|---------------------------------------------------------------------------Check range to the DoT target.
			/if (${Spawn[id ${DoTTarget}].Distance} > ${AssistBreakDistance}) {
				/docommand ${ChatToggle} ${Spawn[${MobID}].CleanName} is too far away.
			} else {
				
		|--------------------------------------------------------Add the DoT target to the DoT_Targets array.
			
			|-------------------------------------------If the array is not already defined, declare it.
				/if (!${Defined[DoT_Targets]}) {
					/if (${Debug}) /echo |- Event_DoTsOn -| Adding: [${DoTTarget}] to DoT_Targets array.
					/call BuildArray DoT_Targets ${DoTTarget}_${Zone.ID}		
				} else {
				
				|------------------Else, if the array is already defined, make sure this target is not being added twice.
					/declare i int local
					/for i 1 to ${DoT_Targets.Size}
						/if (${DoT_Targets[${i}].Arg[1,_]} == ${DoTTarget}) {
							/docommand ${ChatToggle} I am already DoTing ${Spawn[${DoTTarget}].CleanName}.
							/RETURN
						}
					/next i
					
				|-------------------------------------If the target was not found on the DoT_Targets array, add it.
					/call BuildArray DoT_Targets ${DoTTarget}_${Zone.ID}
				}
			}
		}
	}

/if (${Debug}) /echo <== event_DoTsOn -|
/RETURN



|----------------------------------------------------------------|
|- Casts Command_DoTs on targets listed in DoT_Targets array.	-|
|----------------------------------------------------------------|
SUB DoTs_OnCommand
| /varset Debug 1
/if (${Debug}) /echo |- DoTs_OnCommand ==>

|----------------------------------------------------------Declare counting variables.
	/declare i int local
	/declare x int local 
	
|----------------------------------------For each target on the DoT_Targets array.
	/for i 1 to ${DoT_Targets.Size}
	
	|---------------------------------If the DoT_Target has died, remove them from the array.
		/if (${Spawn[id ${DoT_Targets[${i}].Arg[1,_]}].Type.Equal[Corpse]} || ${Select[${Spawn[id ${DoT_Targets[${i}].Arg[1,_]}].Type.Equal[Corpse]},NULL]}) {
			/call RemoveArrayElement DoT_Targets ${DoT_Targets[${i}]}
		} else {
			
		|--------------------------Check Command_DoTs on the Debuff_Target.
			/for x 1 to ${Command_DoTs.Size}
		
			|--------------------------------------------------------------------Cast a spell.
				/if (${Command_DoTs[${x}].Arg[2,/].Left[3].Equal[gem]}) {
					/if (${Me.PctMana}>${Command_DoTs[${x}].Arg[3,/].Replace[m,]} && ${Spawn[${DoT_Targets[${i}].Arg[1,_]}].Distance}<=${Spell[${Command_DoTs[${x}].Arg[1,/]}].MyRange} && !${${DoT_Targets[${i}].Arg[1,_]}_${Spell[${Command_DoTs[${x}].Arg[1,/]}].ID}}) {
						/if (${Debug}) /echo |- DoTs_OnCommand -| Attempting to cast [${Command_DoTs[${x}].Arg[1,/]}] on [${Spawn[id ${DoT_Targets[${i}].Arg[1,_]}].CleanName} - ${AssistTarget}] with a retry timer of [${If[${Me.Combat},0s,${If[${Me.Gem[${Command_DoTs[${x}].Arg[1,/]}]},2s,9s]}]}].
						/call Check_LongTermSpell ${DoT_Targets[${i}].Arg[1,_]} "${Command_DoTs[${x}].Arg[1,/]}" ${Command_DoTs[${x}].Arg[2,/]} ${If[${Me.Combat},0s,${If[${Me.Gem[${Command_DoTs[${x}].Arg[1,/]}]},2s,9s]}]}
					}
				
			|-----------------------------------------------------------------Cast an item.
				} else /if (${Command_DoTs[${x}].Arg[2,/].Equal[item]}) {
					/if (${Spawn[${DoT_Targets[${i}].Arg[1,_]}].Distance}<=${Spell[${FindItem[${Command_DoTs[${x}].Arg[1,/]}].Spell}].MyRange} && !${${DoT_Targets[${i}].Arg[1,_]}_${FindItem[${Command_DoTs[${x}].Arg[1,/]}].ID}}) {
						/call Check_LongTermSpell ${DoT_Targets[${i}].Arg[1,_]} "${Command_DoTs[${x}].Arg[1,/]}" ${Command_DoTs[${x}].Arg[2,/]}
					}
					
			|------------------------------------------------------------------Cast an AA.
				} else /if (${Command_DoTs[${x}].Arg[2,/].Equal[alt]}) {
					/call Check_LongTermSpell ${DoT_Targets[${i}].Arg[1,_]} "${Command_DoTs[${x}].Arg[1,/]}" ${Command_DoTs[${x}].Arg[2,/]}
				}
			/if (!${ActionTaken}) /next x
	
		}
	/if (${Debuff_Targets.Size} && !${ActionTaken}) /next i
	
/if (${Debug}) /echo <== DoTs_OnCommand -|
| /varset Debug 0
/RETURN



|--------------------------------------------|
|- Cancels DoTs on command on all targets. 	-|
|--------------------------------------------|
#event DoTsOff "#*#DoTs Off#*#"
SUB event_DoTsOff
/if (${Debug}) /echo |- event_DoTsOff ==>

	/if (${DoT_Targets.Size}) {
		/if (${Verbosity}) /docommand ${ChatToggle} Ending DoTs
		/deletevar DoT_Targets
	}

/if (${Debug}) /echo <== event_DoTsOff -|
/RETURN



|----------------------------|
|- Turns rain functions on.	-|
|----------------------------|
#event RainOn "<#1#> Rain on"
#event RainOn "#1# tells you#*#Rain on'"
#event RainOn "#1# tells the group, 'Rain on"
SUB Event_RainOn(line, ChatSender)
/if (${Debug}) /echo |- Event_RainOn ==>

	/if (${Rain_Spells.Size} &&  && ${SpawnCount[pc ${ChatSender}]}) /varset do_Rain TRUE

/if (${Debug}) /echo <== Event_RainOn -|
/RETURN



|--------------------------------|
|- Turns rain functions off.	-|
|--------------------------------|
#event RainOff "<#1#> Rain off"
#event RainOff "#1# tells you#*#RainOff'"
#event RainOff "#1# tells the group, 'Rain off"
SUB Event_RainOff
/if (${Debug}) /echo |- Event_RainOff ==>

	/if (${Defined[do_Rain]}) {
		/if (${Me.Casting.ID}) /interrupt 
		/varset do_Rain FALSE
		/docommand ${ChatToggle} Ending rain.
		/doevents flush
		/varset combatTimer 10s
	}

/if (${Debug}) /echo <== Event_RainOff -|	
/RETURN



|------------------------------------------------------------------------------------------------------------|
|- Casts spells listed in the [Rain] section of the Character_Ini on mobs which wander into your AE_Radius.	-|
|------------------------------------------------------------------------------------------------------------|
SUB Rain
/if (${Debug}) /echo |- Rain ==>

	/call Background_Events
	/if (${RainCounter} <= ${Rain_Spells.Size}) {
		/if (${SpawnCount[npc radius ${AE_Radius}]}) {
			/call e3_Cast ${NearestSpawn[npc radius ${AE_Radius}].ID} "${Rain_Spells[${RainCounter}].Arg[1,/]}" ${Rain_Spells[${RainCounter}].Arg[2,/]} ${If[${StickOn} || ${Me.Combat},0s,${If[${Me.Gem[${MyNukes[${i}].Arg[1,/]}]},0s,9s]}]}
			/if (${Select[${castReturn},CAST_SUCCESS]})	{
				/if (${Defined[NukeCounter]}) /varcalc RainCounter ${RainCounter}+1
			} else /if (${Select[${castReturn},CAST_NOTREADY]})	{
				/if (${Defined[RainCounter]}) /varcalc RainCounter ${RainCounter}+1
			}
		}
	} else /if (${RainCounter} > ${Rain_Spells.Size}) {
		/varset RainCounter 1
	}
	/varset combatTimer ${combatDelay}

/if (${Debug}) /echo <== Rain -|	
/RETURN



|--------------------------------|
|- Toggles taunt functions on.	-|
|--------------------------------|
#event Taunt_On "[MQ2] Toggle Taunt On"
SUB event_Taunt_On
/varset do_Taunt TRUE
| /echo Taunt is now on.
/RETURN



|--------------------------------|
|- Toggles taunt functions off.	-|
|--------------------------------|
#event Taunt_Off "[MQ2] Toggle Taunt Off"
SUB event_Taunt_Off
/varset do_Taunt FALSE
| /echo Taunt is now off.
/RETURN



|----------------------------------------------------------------|
|- Casts swarm pets from Mages, Necros, Shaman, and Wizards.	-|
|----------------------------------------------------------------|
#event SwarmPets "<#1#> Swarm Pets on #2#"
SUB event_SwarmPets(line, ChatSender, swarmTarget)
/if (${Debug}) /echo |- event_SwarmPets ==>

|--------------------------------------------------------------------------------------------------------------------------Validate target.
	/if (${SpawnCount[id ${swarmTarget}]} && ${Spawn[${swarmTarget}].Type.NotEqual[Corpse]} && ${Select[${Spawn[${swarmTarget}].Type},${AcceptableTargetTypes}]} && ${SpawnCount[pc ${ChatSender}]}) {
	
	|--------------------------------------------------------------------------------------------------------------Magician abilities.
		/if (${Me.Class.ShortName.Equal[MAG]}) {
		
		|---------------------------------------------------------------------------------------Use Servant of Ro AA.
			/if (${Me.AltAbilityReady[Servant of Ro]}) {
				/call e3_Cast ${swarmTarget} ${Me.AltAbility[Servant of Ro].ID} alt
			} else {
				/docommand ${ChatToggle} Servant of Ro is not ready.
			}
			
		|---------------------------------------------------------------------------------------Use Host of the Elements AA.	
			/if (${Me.AltAbilityReady[Host of the Elements]}) {
				/call e3_Cast ${swarmTarget} ${Me.AltAbility[Host of the Elements].ID} alt
			} else {
				/docommand ${ChatToggle} Host of the Elements is not ready.
			}
			
	|-----------------------------------------------------------------------------------------------------------Necromancer abilities.
		} else /if (${Me.Class.ShortName.Equal[NEC]}) {
			
		|---------------------------------------------------------------------------------------Use Swarm of Decay AA.
			/if (${Me.AltAbilityReady[Swarm of Decay]}) {
				/call e3_Cast ${swarmTarget} ${Me.AltAbility[Swarm of Decay].ID} alt
			} else {
				/docommand ${ChatToggle} Swarm of Decay is not ready.
			}
			
		|---------------------------------------------------------------------------------------Use Wake the Dead AA.
			/if (${Me.AltAbilityReady[Wake the Dead]}) {
				/delay 5
				/call e3_Cast ${swarmTarget} ${Me.AltAbility[Wake the Dead].ID} alt
			} else {
				/docommand ${ChatToggle} Wake The Dead is not ready.
			}
		} else /if (${Me.Class.ShortName.Equal[SHM]}) {
		
		|---------------------------------------------------------------------------------------Use Spirit Call AA.
			/if (${Me.AltAbilityReady[Spirit Call]}) {
				/call e3_Cast ${swarmTarget} ${Me.AltAbility[Spirit Call].ID} alt
			} else {
				/docommand ${ChatToggle} Host of the Elements is not ready.
			}
			
	|----------------------------------------------------------------------------------------------Wizard abilities.		
		} else /if (${Me.Class.ShortName.Equal[WIZ]}) {
			
		|---------------------------------------------------------------------------------------Use Call of Xuzl AA.	
			/if (${Me.AltAbilityReady[Call of Xuzl]}) {
				/call e3_Cast ${swarmTarget} ${Me.AltAbility[Call of Xuzl].ID} alt
			} else {
				/docommand ${ChatToggle} Call of Xuzl is not ready.
			}
		}
	}
	/varset combatTimer ${combatDelay}
	
/if (${Debug}) /echo <== event_SwarmPets -|
/RETURN



|----------------------------------------------------|
|- Engages QuickBurns listed in the Character_Ini.	-|
|----------------------------------------------------|
#event QuickBurn "<#1#> Quick Burn"
SUB event_QuickBurn(line, ChatSender)
/if (${Debug}) /echo |- event_QuickBurn ==>

	/if (${SpawnCount[pc ${ChatSender}]} && ${Defined[use_QBurn]}) {
		/docommand ${ChatToggle} Using Quick Burns...
		/varset use_QBurn TRUE
	}

/if (${Debug}) /echo <== event_QuickBurn -|
/RETURN



|----------------------------------------------------------------------------------------|
|- QuickBurn's MainLoop hook, calls QuickBurns in succession until your target dies.	-|
|----------------------------------------------------------------------------------------|
SUB QuickBurns
/if (${Debug}) /echo |- QuickBurns ==>

	/if (!${Assisting}) {
		/echo You must be currently attacking something to burn.
	} else {

		/if (${QBurn_Counter} <= ${QuickBurns.Size}) {
		
			/if (${QuickBurns[${QBurn_Counter}].Arg[2,/].Find[gem]}) {
				/if (${Debug}) /echo |- QuickBurn -| Casting [${QuickBurns[${QBurn_Counter}].Arg[1,/]}] [${QuickBurns[${QBurn_Counter}].Arg[2,/]}] [${Spell[${QuickBurns[${QBurn_Counter}].Arg[1,/]}].Recast}s]
				/call e3_Cast ${AssistTarget} "${QuickBurns[${QBurn_Counter}].Arg[1,/]}" ${QuickBurns[${QBurn_Counter}].Arg[2,/]} ${Spell[${QuickBurns[${QBurn_Counter}].Arg[1,/]}].Recast}s
				/varcalc QBurn_Counter ${QBurn_Counter} + 1
			} else /if (${QuickBurns[${QBurn_Counter}].Arg[2,/].Equal[item]}) {
				/if (${Debug}) /echo |- QuickBurn -| Casting [${QuickBurns[${QBurn_Counter}].Arg[1,/]}] [${QuickBurns[${QBurn_Counter}].Arg[2,/]}]
				/call e3_Cast ${AssistTarget} "${QuickBurns[${QBurn_Counter}].Arg[1,/]}" item
				/varcalc QBurn_Counter ${QBurn_Counter} + 1
			} else /if (${QuickBurns[${QBurn_Counter}].Arg[2,/].Equal[alt]} || ${QuickBurns[${QBurn_Counter}].Arg[2,/].Equal[aa]}) {
				/if (${Debug}) /echo |- QuickBurn -| Casting [${QuickBurns[${QBurn_Counter}].Arg[1,/]}] [${QuickBurns[${QBurn_Counter}].Arg[2,/]}]
				/call e3_Cast ${AssistTarget} "${QuickBurns[${QBurn_Counter}].Arg[1,/]}" alt
				/varcalc QBurn_Counter ${QBurn_Counter} + 1
			} else /if (${QuickBurns[${QBurn_Counter}].Arg[2,/].Equal[disc]}) {
				/if (${Window[CombatAbilityWnd].Child[CAW_CombatEffectLabel].Text.NotEqual[No Effect]}) {
					/if (${Debug}) /echo |- QuickBurn -| Waiting on disc [${Window[CombatAbilityWnd].Child[CAW_CombatEffectLabel].Text}]
				} else {
					/if (${Debug}) /echo |- QuickBurn -| Casting [${QuickBurns[${QBurn_Counter}].Arg[1,/]}] [${QuickBurns[${QBurn_Counter}].Arg[2,/]}]
					/call e3_Cast ${AssistTarget} "${QuickBurns[${QBurn_Counter}].Arg[1,/]}" disc
					/varcalc QBurn_Counter ${QBurn_Counter} + 1
				}
			}
			
		} else {
			/varset QBurn_Counter 1
			/varset use_QBurn FALSE
		}
	}
	/varset combatTimer ${combatDelay}

/if (${Debug}) /echo <== QuickBurns -|
/RETURN



|----------------------------------------------------|
|- Engages FullBurns listed in the Character_Ini.	-|
|----------------------------------------------------|
#event FullBurn "<#1#> Full Burn"
SUB event_FullBurn(line, ChatSender)
/if (${Debug}) /echo |- event_FullBurn ==>

	/if (!${Assisting}) {
			/echo You must be currently attacking something to burn.
	} else {
		/if (${SpawnCount[pc ${ChatSender}]} && ${Defined[use_FBurn]}) {
			/docommand ${ChatToggle} Using Full Burns...
			/varset use_FBurn TRUE
		}
	}
	
/if (${Debug}) /echo <== event_FullBurn -|
/RETURN



|----------------------------------------------------------------------------------------------------|
|- FullBurn's MainLoop hook, calls FullBurns, and QuickBurns, in succession until your target dies.	-|
|----------------------------------------------------------------------------------------------------|
SUB FullBurns
/if (${Debug}) /echo |- FullBurns ==>

	/if (${FBurn_Counter} <= ${FullBurns.Size}) {
	
		/if (${FullBurns[${FBurn_Counter}].Arg[2,/].Find[gem]}) {
			/if (${Debug}) /echo |- FullBurn -| Casting [${FullBurns[${FBurn_Counter}].Arg[1,/]}] [${FullBurns[${FBurn_Counter}].Arg[2,/]}] [${Spell[${FullBurns[${FBurn_Counter}].Arg[1,/]}].Recast}s]
			/call e3_Cast ${AssistTarget} "${FullBurns[${FBurn_Counter}].Arg[1,/]}" ${FullBurns[${FBurn_Counter}].Arg[2,/]} ${Spell[${FullBurns[${FBurn_Counter}].Arg[1,/]}].Recast}s
			/varcalc FBurn_Counter ${FBurn_Counter} + 1
		} else /if (${FullBurns[${FBurn_Counter}].Arg[2,/].Equal[item]}) {
			/if (${Debug}) /echo |- FullBurn -| Casting [${FullBurns[${FBurn_Counter}].Arg[1,/]}] [${FullBurns[${FBurn_Counter}].Arg[2,/]}]
			/call e3_Cast ${AssistTarget} "${FullBurns[${FBurn_Counter}].Arg[1,/]}" item
			/varcalc FBurn_Counter ${FBurn_Counter} + 1
		} else /if (${FullBurns[${FBurn_Counter}].Arg[2,/].Equal[alt]} || ${FullBurns[${FBurn_Counter}].Arg[2,/].Equal[aa]}) {
			/if (${Debug}) /echo |- FullBurn -| Casting [${FullBurns[${FBurn_Counter}].Arg[1,/]}] [${FullBurns[${FBurn_Counter}].Arg[2,/]}]
			/call e3_Cast ${AssistTarget} "${FullBurns[${FBurn_Counter}].Arg[1,/]}" alt
			/varcalc FBurn_Counter ${FBurn_Counter} + 1
		} else /if (${FullBurns[${FBurn_Counter}].Arg[2,/].Equal[disc]}) {
			/if (${Window[CombatAbilityWnd].Child[CAW_CombatEffectLabel].Text.NotEqual[No Effect]}) {
				/if (${Debug}) /echo |- FullBurn -| Waiting on disc [${Window[CombatAbilityWnd].Child[CAW_CombatEffectLabel].Text}]
			} else {
				/if (${Debug}) /echo |- FullBurn -| Casting [${FullBurns[${FBurn_Counter}].Arg[1,/]}] [${FullBurns[${FBurn_Counter}].Arg[2,/]}]
				/call e3_Cast ${AssistTarget} "${FullBurns[${FBurn_Counter}].Arg[1,/]}" disc
				/varcalc FBurn_Counter ${FBurn_Counter} + 1
			}
		}
	
	} else {
		/varset FBurn_Counter 1
		/varset use_FBurn FALSE
		/if (${Defined[use_QBurn]}) /varset use_QBurn TRUE
	}
	
	/varset combatTimer ${combatDelay}

/if (${Debug}) /echo <== FullBurns -|
/RETURN



|------------------------------------------------------------|
|- Turns off attack, when a mob you're attacking enrages.	-|
|------------------------------------------------------------|
#event Enrage "#1# has become ENRAGED."
SUB Event_Enrage(line, RageOn)
/if (${Debug}) /echo Enrage ON, turning attack OFF
	/if (${Me.Combat} && ${Target.ID} == ${Spawn[${RageOn}].ID} && ${Target.PctHPs} < 20) {
		/attack off
		/if (${Me.Pet.ID}) /pet back off
		/if (!${Defined[enrageHold]}) /declare enrageHold bool outer TRUE
	}
/RETURN



|------------------------------------------------|
|- Turns attack back on, after enrage is over.	-|
|------------------------------------------------|
#event EnrageOff "#1# is no longer enraged."
SUB Event_EnrageOff(line, RageOn)
/if (${Debug}) /echo Enrage OFF, turning attack ON
	/if (${enrageHold} && ${Target.ID} == ${Spawn[${RageOn}].ID}) {
		/attack on
		/if (${Me.Pet.ID}) /pet attack
		/deletevar enrageHold
	}
/RETURN



|----------------------------------------------------|
|- Set macro variables to allow combat with PCs.	-|
|----------------------------------------------------|
|- Use, /pvpon, or, /pvpoff.						-|
|----------------------------------------------------|
#event toggle-PvP "#*# Toggle PvP [#1#]"
SUB event_toggle-PvP(line, pvpToggle)

|------------------------------------------------------------------------------Turn PvP On.
	/if (${Select[${pvpToggle},ON,On,on,TRUE,True,true,1]}) {
		/varset Toggle-PvP TRUE
		/varset AcceptableTargetTypes ${AcceptableTargetTypes},PC
		
|------------------------------------------------------------------------------Turn PvP Off.		
	} else {
		/varset Toggle-PvP FALSE
		/call INItoVar ${Macro_Ini},Assists,Allowed_TargetTypes AcceptableTargetTypes string outer
	}

/echo Toggling PvP-Mode [${Toggle-PvP}]
/RETURN



|--------------------|
|- setup_Assists	-|
|--------------------|
SUB setup_Assists
/if (${Debug}) /echo |- setup_Assists ==>

|------------------------------------------------------------------------------------------------------------------------Create variables used in Assist scripts.
	/declare Toggle-PvP bool outer FALSE
	/declare Assisting bool outer FALSE
	/declare AssistTarget int outer 0
	/declare AllowControl bool outer FALSE
	
	/declare i int local
	
|-----------------------------------------------------------------------------------------------------------------------Import Macro_Ini Settings.
	/call IniToVar "${Macro_Ini},Assists,Max Assist Engage Distance" AssistBreakDistance int outer
	/call INItoVar "${Macro_Ini},Assists,Acceptable Target Types" AcceptableTargetTypes string outer
	/call INItoVar "${Macro_Ini},Assists,Auto-Assist (On/Off)" auto_AssistOn bool outer
	/call INItoVar "${Macro_Ini},Assists,Area Effect Radius" AE_Radius int outer
	
|------------------------------------------------------------------------------------------------------------------------------Validate EngagePct.
	/call INItoVar "${Macro_Ini},Assists,Auto-Assist Engage Percent" EngagePct int outer
	/if (!${Defined[EngagePct]}) {
		/if (${Debug}) /echo ERROR: @[EngagePct] - EngagePct is invalid, defaulting to 98%
		/declare EngagePct int outer 98
	} else /if (${Defined[EngagePct]} && !${EngagePct}) {
		/if (${Debug}) /echo ERROR: @[EngagePct] - EngagePct is invalid, defaulting to 98%
		/varset EngagePct 98
	}

|-------------------------------------------------------------------------------------------------------------------Import Character_Ini Settings.

	|-----------------------------------------------------------------------------------------------------------------------------------[Assists].
		/if (${Ini[${Character_Ini},Assists].Length}) {
			/call IniToVar "${Character_Ini},Assists,Assist Type (Melee/Ranged)" AssistType string outer
			
		
			/if (${AssistType.Equal[Melee]}) {
				/call IniToVar "${Character_Ini},Assists,Melee Stick Point" AssistStickPoint string outer
				/call IniToVar "${Character_Ini},Assists,Melee Stick Distance" AssistStickDistance string outer
				/declare AssistStick_On bool outer TRUE
				/declare AssistStickDistanceSetting int outer
				/if (${AssistStickDistance.NotEqual[MaxMelee]}) /varset AssistStickDistanceSetting ${AssistStickDistance}
			} else {
				/call IniToVar "${Character_Ini},Assists,Ranged Stick Distance" RangedStickDistance string outer
			}
		}
		
	| ---------------------------------------------------------------------------------------------------------------------------------------[Abilities].
		/if (${Ini[${Character_Ini},Abilities,Ability#1].Length}) {
			/varcalc Validation_Arg ${Validation_Arg} + 1
			:Validate_Abilities
			/varset Validation_Count 0
			
			/call INItoArray ${Character_Ini},Abilities,Ability# MyAbilities
			
			/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length} || ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${Validation_Arg},-]} != ${Validation_Count}) /varset Validation_On TRUE
			
			/if (${Validation_On} && ${Validation_Count}) {
				/if (${MyAbilities.Size}) {
					
					/for i 1 to ${MyAbilities.Size}
				
					|------------------------------------------------------------------------------------------------------------Auto-fix castType.	
						/if (!${MyAbilities[${i}].Arg[2,/].Length}) {
							/echo [Abilities#${i}] - [${MyAbilities[${i}]}] does not have a specified AbilityType.  Attempting to fix...
							
							/if (${Me.Ability[${MyAbilities[${i}]}]}) {
								/varset MyAbilities[${i}] ${MyAbilities[${i}]}/ability
								/call WriteToIni "${Character_Ini},Abilities,Ability#${i}" "${MyAbilities[${i}]}" 1
								/echo [${MyAbilities[${i}].Arg[1,/]}] was found on my abilities page.  Setting [${MyAbilities[${i}].Arg[1,/]}] to [${MyAbilities[${i}].Arg[2,/]}].
							} else /if (${Me.AltAbility[${MyAbilities[${i}]}]}) {
								/varset MyAbilities[${i}] ${MyAbilities[${i}]}/alt
								/call WriteToIni "${Character_Ini},Abilities,Ability#${i}" "${MyAbilities[${i}]}" 1
								/echo I have ranks spent in [${MyAbilities[${i}].Arg[1,/]}].  Setting [${MyAbilities[${i}].Arg[1,/]}] to [${MyAbilities[${i}].Arg[2,/]}].
							} else /if (${Me.CombatAbility[${MyAbilities[${i}]}]}) {
								/varset MyAbilities[${i}] ${MyAbilities[${i}]}/disc
								/call WriteToIni "${Character_Ini},Abilities,Ability#${i}" "${MyAbilities[${i}]}" 1
								/echo Found discipline [${MyAbilities[${i}].Arg[1,/]}].  Setting [${MyAbilities[${i}].Arg[1,/]}] to [${MyAbilities[${i}].Arg[2,/]}].			
							} else {
								/varset MyAbilities[${i}] ${MyAbilities[${i}]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Abilities,Ability#${i}" "${MyAbilities[${i}]}" 1
								/echo ERROR: @[Ability#${i}] - Could not find a compatible abilityType for [${MyAbilities[${i}].Arg[1,/]}].  Flagging [Ability#${i}] as a BAD_ENTRY.
							}
						} else {

						|------------------------------------------------------------------------------------------------------------Validate MyAbilities array.
							/if (${MyAbilities[${i}].Arg[2,/].Equal[ability]}) {
								/if (!${Me.Ability[${MyAbilities[${i}].Arg[1,/]}]}) {
									/varset MyAbilities[${i}] ${MyAbilities[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Abilities,Ability#${i}" "${MyAbilities[${i}]}" 1
									/echo ERROR: @[Ability#${i}] - I do not have [${MyAbilities[${i}].Arg[1,/]}] on my abilities tab.  Flagging [Ability#${i}] as a BAD_ENTRY.
								}
							} else /if (${MyAbilities[${i}].Arg[2,/].Equal[alt]} || ${MyAbilities[${i}].Arg[2,/].Equal[aa]}) {
								/if (!${Me.AltAbility[${MyAbilities[${i}]}]}) {
									/varset MyAbilities[${i}] ${MyAbilities[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Abilities,Ability#${i}" "${MyAbilities[${i}]}" 1
									/echo ERROR: @[Ability#${i}] - I do not have any ranks in [${MyAbilities[${i}].Arg[1,/]}].  Flagging [Ability#${i}] as a BAD_ENTRY.
								}
							} else /if (${MyAbilities[${i}].Arg[2,/].Equal[disc]}) {
								/if (${Me.CombatAbility[${MyAbilities[${i}]}]}) {
									/varset MyAbilities[${i}] ${MyAbilities[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Abilities,Ability#${i}" "${MyAbilities[${i}]}" 1
									/echo ERROR: @[Ability#${i}] - I cannot find [${MyAbilities[${i}].Arg[1,/]}] listed in my disciplines window.  Flagging [Ability#${i}] as a BAD_ENTRY.
								}
							} else /if (${MyAbilities[${i}].Arg[2,/].Equal[BAD_ENTRY]}) {
								/echo ERROR: @[Ability#${i}] - There is a problem with the user entry for [${MyAbilities[${i}].Arg[1,/]}] in [${Character_Ini}].  Please review this entry, and restart the macro.
							} else {
								/varset MyAbilities[${i}] ${MyAbilities[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Abilities,Ability#${i}" "${MyAbilities[${i}].Arg[1,/]}" 1
								/echo ERROR: @[Ability#${i}] - Could not find a compatible abilityType for [${MyAbilities[${i}].Arg[1,/]}].  Flagging [Ability#${i}] as a BAD_ENTRY.
							}
							
						}
				
					/next i
				}
				
				/call rewrite_ValidationKey ${Validation_Count} ${Validation_Arg}
		
				/varset Validation_On FALSE
				/goto :Validate_Abilities
			}
			
			/for i 1 to ${MyAbilities.Size}
			
			|------------------------------------------------------------------------------------------If the ability is 'Taunt' create do_Taunt variable.
				/if (${MyAbilities[${i}].Arg[1,/].Equal[taunt]}) /declare do_Taunt bool outer TRUE
				
			/next i
		}
		
	| -------------------------------------------------------------------------------------------------------------------------------------------Import [Nukes].
		/if (${Ini[${Character_Ini},Nukes,Nuke#1].Length}) {
			/varcalc Validation_Arg ${Validation_Arg} + 1
			:Validate_Nukes
			/varset Validation_Count 0
			
			/call INItoArray ${Character_Ini},Nukes,Nuke# MyNukes
			
			/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length} || ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${Validation_Arg},-]} != ${Validation_Count}) /varset Validation_On TRUE
			
			/if (${Validation_On} && ${Validation_Count}) {
				/if (${MyNukes.Size}) {
					
					/for i 1 to ${MyNukes.Size}
					
					|------------------------------------------------------------------------------------------------------------Auto-fix castType.		
						/if (!${MyNukes[${i}].Arg[2,/].Length}) {
							/echo [Nuke#${i}] - [${MyNukes[${i}]}] does not have a specified castType.  Attempting to fix...
							
							/if (${Int[${Me.Book[${MyNukes[${i}]}]}]}) {
								/varset MyNukes[${i}] ${MyNukes[${i}]}/gem${If[${Me.Gem[${MyNukes[${i}]}]},${Me.Gem[${MyNukes[${i}]}]},${DefaultGem}]}
								/call WriteToIni "${Character_Ini},Nukes,Nuke#${i}" "${MyNukes[${i}]}" 1
								/echo [${MyNukes[${i}].Arg[1,/]}] was found in my spellbook.  Setting [${MyNukes[${i}].Arg[1,/]}] to [${MyNukes[${i}].Arg[2,/]}].
							} else /if (${FindItemCount[${MyNukes[${i}]}]}) {
								/varset MyNukes[${i}] ${MyNukes[${i}]}/item
								/call WriteToIni "${Character_Ini},Nukes,Nuke#${i}" "${MyNukes[${i}]}" 1
								/echo [${MyNukes[${i}].Arg[1,/]}] was found in my inventory.  Setting [${MyNukes[${i}].Arg[1,/]}] to [${MyNukes[${i}].Arg[2,/]}].
							} else /if (${Me.AltAbility[${MyNukes[${i}]}]}) {
								/varset MyNukes[${i}] ${MyNukes[${i}]}/alt
								/call WriteToIni "${Character_Ini},Nukes,Nuke#${i}" "${MyNukes[${i}]}" 1
								/echo I have ranks spent in [${MyNukes[${i}].Arg[1,/]}].  Setting [${MyNukes[${i}].Arg[1,/]}] to [${MyNukes[${i}].Arg[2,/]}].	
							} else {
								/varset MyNukes[${i}] ${MyNukes[${i}]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Nukes,Nuke#${i}" "${MyNukes[${i}]}" 1
								/echo ERROR: @[Nuke#${i}] - Could not find a compatible castType for [${MyNukes[${i}]}].  Flagging [Nuke#${i}] as a BAD_ENTRY.
							}
						} else {
						
						|--------------------------------------------------------------------------------------------------------------------------Invalid castType.
							/if (${Int[${MyNukes[${i}].Arg[2,/]}]}) {
								/echo [Nuke#${i}] - [${MyNukes[${i}]}] has an invalid castType.  Attempting to fix...
								
								/if (${Int[${Me.Book[${MyNukes[${i}].Arg[1,/]}]}]}) {
									/varset MyNukes[${i}] ${MyNukes[${i}].Arg[1,/]}/gem${If[${Me.Gem[${MyNukes[${i}]}]},${Me.Gem[${MyNukes[${i}]}]},${DefaultGem}]}/${MyNukes[${i}].Arg[2,/]}${If[${MyNukes[${i}].Arg[3,/].Length},/${MyNukes[${i}].Arg[3,/]},]}
									/call WriteToIni "${Character_Ini},Nukes,Nuke#${i}" "${MyNukes[${i}]}" 1
									/echo [Nuke#${i}] has been changed to [${MyNukes[${i}]}].  Please review this entry, and restart the macro if needed.
								} else /if (${FindItemCount[${MyNukes[${i}].Arg[1,/]}]}) {
									/varset MyNukes[${i}] ${MyNukes[${i}]}/item/${MyNukes[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},Nukes,Nuke#${i}" "${MyNukes[${i}]}" 1
									/echo [Nuke#${i}] has been changed to [${MyNukes[${i}]}].  Please review this entry, and restart the macro if needed.
								} else /if (${Me.AltAbility[${MyNukes[${i}]}]}) {
									/varset MyNukes[${i}] ${MyNukes[${i}]}/alt/${MyNukes[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},Nukes,Nuke#${i}" "${MyNukes[${i}]}" 1
									/echo [Nuke#${i}] has been changed to [${MyNukes[${i}]}].  Please review this entry, and restart the macro if needed.
								} else {
									/varset MyNukes[${i}] ${MyNukes[${i}]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Nukes,Nuke#${i}" "${MyNukes[${i}]}" 1
									/echo ERROR: @[Nuke#${i}] - Could not find a compatible castType for [${MyNukes[${i}]}].  Flagging [Nuke#${i}] as a BAD_ENTRY.
								}
							} else /if (${MyNukes[${i}].Arg[2,/].Find[gem]}) {
								/if (!${Int[${Me.Book[${MyNukes[${i}].Arg[1,/]}]}]}) {
									/varset MyNukes[${i}] ${MyNukes[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Nukes,Nuke#${i}" "${MyNukes[${i}]}" 1
									/echo ERROR: @[Nuke#${i}] - [${MyNukes[${i}].Arg[1,/]}] was not found in my spellbook.  Flagging [Nuke#${i}] as a BAD_ENTRY.
								}
							} else /if (${MyNukes[${i}].Arg[2,/].Equal[item]}) {
								/if (!${FindItemCount[${MyNukes[${i}].Arg[1,/]}]}) {
									/varset MyNukes[${i}] ${MyNukes[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Nukes,Nuke#${i}" "${MyNukes[${i}]}" 1
									/echo ERROR: @[Nuke#${i}] - I do not have [${MyNukes[${i}].Arg[1,/]}] in my inventory.  Flagging [Nuke#${i}] as a BAD_ENTRY.
								}
							} else /if (${MyNukes[${i}].Arg[2,/].Equal[alt]} || ${MyNukes[${i}].Arg[2,/].Equal[aa]}) {
								/if (!${Me.AltAbility[${MyNukes[${i}].Arg[1,/]}]}) {
									/varset MyNukes[${i}] ${MyNukes[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Nukes,Nuke#${i}" "${MyNukes[${i}]}" 1
									/echo ERROR: @[Nuke#${i}] - I do not have any ranks in [${MyNukes[${i}].Arg[1,/]}].  Flagging [Nuke#${i}] as a BAD_ENTRY.
								}
							} else /if (${MyNukes[${i}].Arg[2,/].Equal[BAD_ENTRY]}) {
								/echo ERROR: @[Nuke#${i}] - There is a problem with the user entry for [${MyNukes[${i}].Arg[1,/]}] in [${Character_Ini}].  Please review this entry, and restart the macro.
							} else {
								/varset MyNukes[${i}] ${MyNukes[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Nukes,Nuke#${i}" "${MyNukes[${i}]}" 1
								/echo ERROR: @[Nuke#${i}] - [${MyNukes[${i}].Arg[2,/]}] is not an acceptable castType.  Flagging [Nuke#${i}] as a BAD_ENTRY.
							}
						}
					
					/next i
				}
				
				/call rewrite_ValidationKey ${Validation_Count} ${Validation_Arg}
		
				/varset Validation_On FALSE
				/goto :Validate_Nukes
			}
			
		|--------------------------------------Declare nuking variables
			/if (${Defined[MyNukes]}) {
				/declare do_Nukes bool outer TRUE
				/declare NukeTimer timer outer 0
			}
		}
		
	| -------------------------------------------------------------------------------------------------------------------------------------------Import [Assist_DoTs].
		/if (${Ini[${Character_Ini},DoTs,DoT on Assist#1].Length}) {
			/varcalc Validation_Arg ${Validation_Arg} + 1
			:Validate_AssistDoTs
			/varset Validation_Count 0
			
			/call INItoArray "${Character_Ini},DoTs,DoT on Assist#" Assist_DoTs
			
			/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length} || ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${Validation_Arg},-]} != ${Validation_Count}) /varset Validation_On TRUE
				
			/if (${Validation_On} && ${Validation_Count}) {
				/if (${Assist_DoTs.Size}) {
				
					/for i 1 to ${Assist_DoTs.Size}
					
					|------------------------------------------------------------------------------------------------------------------------------Missing castType.	
						/if (!${Assist_DoTs[${i}].Arg[2,/].Length}) {
							/echo [DoT on Assist#${i}] - [${Assist_DoTs[${i}]}] does not have a specified castType.  Attempting to fix...
							
							/if (${Int[${Me.Book[${Assist_DoTs[${i}]}]}]}) {
								/varset Assist_DoTs[${i}] ${Assist_DoTs[${i}]}/gem${If[${Me.Gem[${Assist_DoTs[${i}]}]},${Me.Gem[${Assist_DoTs[${i}]}]},${DefaultGem}]}
								/call WriteToIni "${Character_Ini},DoTs,DoT on Assist#${i}" "${Assist_DoTs[${i}]}" 1
								/echo [${Assist_DoTs[${i}].Arg[1,/]}] was found in my spellbook.  Setting [${Assist_DoTs[${i}].Arg[1,/]}] to [${Assist_DoTs[${i}].Arg[2,/]}].
							} else /if (${FindItemCount[${Assist_DoTs[${i}]}]}) {
								/varset Assist_DoTs[${i}] ${Assist_DoTs[${i}]}/item
								/call WriteToIni "${Character_Ini},DoTs,DoT on Assist#${i}" "${Assist_DoTs[${i}]}" 1
								/echo [${Assist_DoTs[${i}].Arg[1,/]}] was found in my inventory.  Setting [${Assist_DoTs[${i}].Arg[1,/]}] to [${Assist_DoTs[${i}].Arg[2,/]}]
							} else /if (${Me.AltAbility[${Assist_DoTs[${i}]}]}) {
								/varset Assist_DoTs[${i}] ${Assist_DoTs[${i}]}/alt
								/call WriteToIni "${Character_Ini},DoTs,DoT on Assist#${i}" "${Assist_DoTs[${i}]}" 1
								/echo I have ranks spent in [${Assist_DoTs[${i}].Arg[1,/]}].  Setting [${Assist_DoTs[${i}].Arg[1,/]}] to [${Assist_DoTs[${i}].Arg[2,/]}]	
							} else {
								/varset Assist_DoTs[${i}] ${Assist_DoTs[${i}]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},DoTs,DoT on Assist#${i}" "${Assist_DoTs[${i}]}" 1
								/echo ERROR: @[DoT on Assist#${i}] - Could not find a compatible castType for [${Assist_DoTs[${i}]}].  Flagging [DoT on Assist#${i}] as a BAD_ENTRY.
							}
						} else {
				
						|--------------------------------------------------------------------------------------------------------------------------Invalid castType.
							/if (${Int[${Assist_DoTs[${i}].Arg[2,/]}]}) {
								/echo [DoT on Assist#${i}] - [${Assist_DoTs[${i}]}] has an invalid castType.  Attempting to fix...
								
								/if (${Int[${Me.Book[${Assist_DoTs[${i}].Arg[1,/]}]}]}) {
									/varset Assist_DoTs[${i}] ${Assist_DoTs[${i}].Arg[1,/]}/gem${If[${Me.Gem[${Assist_DoTs[${i}]}]},${Me.Gem[${Assist_DoTs[${i}]}]},${DefaultGem}]}/${Assist_DoTs[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},DoTs,DoT on Assist#${i}" "${Assist_DoTs[${i}]}" 1
									/echo [DoT on Assist#${i}] has been changed to [${Assist_DoTs[${i}]}].  Please review this entry, and restart the macro if needed.
								} else /if (${FindItemCount[${Assist_DoTs[${i}].Arg[1,/]}]}) {
									/varset Assist_DoTs[${i}] ${Assist_DoTs[${i}]}/item/${Assist_DoTs[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},DoTs,DoT on Assist#${i}" "${Assist_DoTs[${i}]}" 1
									/echo [DoT on Assist#${i}] has been changed to [${Assist_DoTs[${i}]}].  Please review this entry, and restart the macro if needed.
								} else /if (${Me.AltAbility[${Assist_DoTs[${i}]}]}) {
									/varset Assist_DoTs[${i}] ${Assist_DoTs[${i}]}/alt/${Assist_DoTs[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},DoTs,DoT on Assist#${i}" "${Assist_DoTs[${i}]}" 1
									/echo [DoT on Assist#${i}] has been changed to [${Assist_DoTs[${i}]}].  Please review this entry, and restart the macro if needed.
								} else {
									/varset Assist_DoTs[${i}] ${Assist_DoTs[${i}]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},DoTs,DoT on Assist#${i}" "${Assist_DoTs[${i}]}" 1
									/echo ERROR: @[DoT on Assist#${i}] - Could not find a compatible castType for [${Assist_DoTs[${i}]}].  Flagging [DoT on Assist#${i}] as a BAD_ENTRY.
								}
							} else /if (${Assist_DoTs[${i}].Arg[2,/].Find[gem]}) {
								/if (!${Int[${Me.Book[${Assist_DoTs[${i}].Arg[1,/]}]}]}) {
									/varset Assist_DoTs[${i}] ${Assist_DoTs[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},DoTs,DoT on Assist#${i}" "${Assist_DoTs[${i}]}" 1
									/echo ERROR: @[DoT on Assist#${i}] - [${Assist_DoTs[${i}].Arg[1,/]}] was not found in my spellbook.  Flagging [DoT on Assist#${i}] as a BAD_ENTRY.
								}
							} else /if (${Assist_DoTs[${i}].Arg[2,/].Equal[item]}) {
								/if (!${FindItemCount[${Assist_DoTs[${i}].Arg[1,/]}]}) {
									/varset Assist_DoTs[${i}] ${Assist_DoTs[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},DoTs,DoT on Assist#${i}" "${Assist_DoTs[${i}]}" 1
									/echo ERROR: @[DoT on Assist#${i}] - I do not have [${Assist_DoTs[${i}].Arg[1,/]}] in my inventory.  Flagging [DoT on Assist#${i}] as a BAD_ENTRY.
								}
							} else /if (${Assist_DoTs[${i}].Arg[2,/].Equal[alt]} || ${Assist_DoTs[${i}].Arg[2,/].Equal[aa]}) {
								/if (!${Me.AltAbility[${Assist_DoTs[${i}].Arg[1,/]}]}) {
									/varset Assist_DoTs[${i}] ${Assist_DoTs[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},DoTs,DoT on Assist#${i}" "${Assist_DoTs[${i}]}" 1
									/echo ERROR: @[DoT on Assist#${i}] - I do not have any ranks in [${Assist_DoTs[${i}].Arg[1,/]}].  Flagging [DoT on Assist#${i}] as a BAD_ENTRY.
								}
							} else /if (${Assist_DoTs[${i}].Arg[2,/].Equal[BAD_ENTRY]}) {
								/echo ERROR: @[DoT on Assist#${i}] - There is a problem with the user entry for [${Assist_DoTs[${i}].Arg[1,/]}] in [${Character_Ini}].  Please review this entry, and restart the macro.
							} else {
								/varset Assist_DoTs[${i}] ${Assist_DoTs[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},DoTs,DoT on Assist#${i}" "${Assist_DoTs[${i}]}" 1
								/echo ERROR: @[DoT on Assist#${i}] - [${Assist_DoTs[${i}].Arg[2,/]}] is not an acceptable castType.  Flagging [DoT on Assist#${i}] as a BAD_ENTRY.
							}
						}
						
					/next i
				}
				
				/call rewrite_ValidationKey ${Validation_Count} ${Validation_Arg}
		
				/varset Validation_On FALSE
				/goto :Validate_AssistDoTs
			}
			
			/if (${Defined[Assist_DoTs]}) /declare do_AssistDoTs bool outer TRUE
		}
		
	| -------------------------------------------------------------------------------------------------------------------------------------------Import [Command_DoTs].	
		/if (${Ini[${Character_Ini},DoTs,DoT on Command#1].Length}) {
			/varcalc Validation_Arg ${Validation_Arg} + 1
			:Validate_CommandDoTs
			/varset Validation_Count 0
			
			/call INItoArray "${Character_Ini},DoTs,DoT on Command#" Command_DoTs
			
			/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length} || ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${Validation_Arg},-]} != ${Validation_Count}) /varset Validation_On TRUE
				
			/if (${Validation_On} && ${Validation_Count}) {
			
				/if (${Command_DoTs.Size}) {
					/for i 1 to ${Command_DoTs.Size}
				
					|------------------------------------------------------------------------------------------------------------------------------Missing castType.	
						/if (!${Command_DoTs[${i}].Arg[2,/].Length}) {
							/echo [DoT on Command#${i}] - [${Command_DoTs[${i}]}] does not have a specified castType.  Attempting to fix...
							
							/if (${Int[${Me.Book[${Command_DoTs[${i}]}]}]}) {
								/varset Command_DoTs[${i}] ${Command_DoTs[${i}]}/gem${If[${Me.Gem[${Command_DoTs[${i}]}]},${Me.Gem[${Command_DoTs[${i}]}]},${DefaultGem}]}
								/call WriteToIni "${Character_Ini},DoTs,DoT on Command#${i}" "${Command_DoTs[${i}]}" 1
								/echo [${Command_DoTs[${i}].Arg[1,/]}] was found in my spellbook.  Setting [${Command_DoTs[${i}].Arg[1,/]}] to [${Command_DoTs[${i}].Arg[2,/]}].
							} else /if (${FindItemCount[${Command_DoTs[${i}]}]}) {
								/varset Command_DoTs[${i}] ${Command_DoTs[${i}]}/item
								/call WriteToIni "${Character_Ini},DoTs,DoT on Command#${i}" "${Command_DoTs[${i}]}" 1
								/echo [${Command_DoTs[${i}].Arg[1,/]}] was found in my inventory.  Setting [${Command_DoTs[${i}].Arg[1,/]}] to [${Command_DoTs[${i}].Arg[2,/]}]
							} else /if (${Me.AltAbility[${Command_DoTs[${i}]}]}) {
								/varset Command_DoTs[${i}] ${Command_DoTs[${i}]}/alt
								/call WriteToIni "${Character_Ini},DoTs,DoT on Command#${i}" "${Command_DoTs[${i}]}" 1
								/echo I have ranks spent in [${Command_DoTs[${i}].Arg[1,/]}].  Setting [${Command_DoTs[${i}].Arg[1,/]}] to [${Command_DoTs[${i}].Arg[2,/]}]	
							} else {
								/varset Command_DoTs[${i}] ${Command_DoTs[${i}]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},DoTs,DoT on Command#${i}" "${Command_DoTs[${i}]}" 1
								/echo ERROR: @[DoT on Command#${i}] - Could not find a compatible castType for [${Command_DoTs[${i}]}].  Flagging [DoT on Command#${i}] as a BAD_ENTRY.
							}
						} else {
				
						|--------------------------------------------------------------------------------------------------------------------------Invalid castType.
							/if (${Int[${Command_DoTs[${i}].Arg[2,/]}]}) {
								/echo [DoT on Command#${i}] - [${Command_DoTs[${i}]}] has an invalid castType.  Attempting to fix...
								
								/if (${Int[${Me.Book[${Command_DoTs[${i}].Arg[1,/]}]}]}) {
									/varset Command_DoTs[${i}] ${Command_DoTs[${i}].Arg[1,/]}/gem${If[${Me.Gem[${Command_DoTs[${i}]}]},${Me.Gem[${Command_DoTs[${i}]}]},${DefaultGem}]}/${Command_DoTs[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},DoTs,DoT on Command#${i}" "${Command_DoTs[${i}]}" 1
									/echo [DoT on Command#${i}] has been changed to [${Command_DoTs[${i}]}].  Please review this entry, and restart the macro if needed.
								} else /if (${FindItemCount[${Command_DoTs[${i}].Arg[1,/]}]}) {
									/varset Command_DoTs[${i}] ${Command_DoTs[${i}]}/item/${Command_DoTs[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},DoTs,DoT on Command#${i}" "${Command_DoTs[${i}]}" 1
									/echo [DoT on Command#${i}] has been changed to [${Command_DoTs[${i}]}].  Please review this entry, and restart the macro if needed.
								} else /if (${Me.AltAbility[${Command_DoTs[${i}]}]}) {
									/varset Command_DoTs[${i}] ${Command_DoTs[${i}]}/alt/${Command_DoTs[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},DoTs,DoT on Command#${i}" "${Command_DoTs[${i}]}" 1
									/echo [DoT on Command#${i}] has been changed to [${Command_DoTs[${i}]}].  Please review this entry, and restart the macro if needed.
								} else {
									/varset Command_DoTs[${i}] ${Command_DoTs[${i}]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},DoTs,DoT on Command#${i}" "${Command_DoTs[${i}]}" 1
									/echo ERROR: @[DoT on Command#${i}] - Could not find a compatible castType for [${Command_DoTs[${i}]}].  Flagging [DoT on Command#${i}] as a BAD_ENTRY.
								}
							} else /if (${Command_DoTs[${i}].Arg[2,/].Find[gem]}) {
								/if (!${Int[${Me.Book[${Command_DoTs[${i}].Arg[1,/]}]}]}) {
									/varset Command_DoTs[${i}] ${Command_DoTs[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},DoTs,DoT on Command#${i}" "${Command_DoTs[${i}]}" 1
									/echo ERROR: @[DoT on Command#${i}] - [${Command_DoTs[${i}].Arg[1,/]}] was not found in my spellbook.  Flagging [DoT on Command#${i}] as a BAD_ENTRY.
								}
							} else /if (${Command_DoTs[${i}].Arg[2,/].Equal[item]}) {
								/if (!${FindItemCount[${Command_DoTs[${i}].Arg[1,/]}]}) {
									/varset Command_DoTs[${i}] ${Command_DoTs[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},DoTs,DoT on Command#${i}" "${Command_DoTs[${i}]}" 1
									/echo ERROR: @[DoT on Command#${i}] - I do not have [${Command_DoTs[${i}].Arg[1,/]}] in my inventory.  Flagging [DoT on Command#${i}] as a BAD_ENTRY.
								}
							} else /if (${Command_DoTs[${i}].Arg[2,/].Equal[alt]} || ${Command_DoTs[${i}].Arg[2,/].Equal[aa]}) {
								/if (!${Me.AltAbility[${Command_DoTs[${i}].Arg[1,/]}]}) {
									/varset Command_DoTs[${i}] ${Command_DoTs[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},DoTs,DoT on Command#${i}" "${Command_DoTs[${i}]}" 1
									/echo ERROR: @[DoT on Command#${i}] - I do not have any ranks in [${Command_DoTs[${i}].Arg[1,/]}].  Flagging [DoT on Command#${i}] as a BAD_ENTRY.
								}
							} else /if (${Command_DoTs[${i}].Arg[2,/].Equal[BAD_ENTRY]}) {
								/echo ERROR: @[DoT on Command#${i}] - There is a problem with the user entry for [${Command_DoTs[${i}].Arg[1,/]}] in [${Character_Ini}].  Please review this entry, and restart the macro.
							} else {
								/varset Command_DoTs[${i}] ${Command_DoTs[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},DoTs,DoT on Command#${i}" "${Command_DoTs[${i}]}" 1
								/echo ERROR: @[DoT on Command#${i}] - [${Command_DoTs[${i}].Arg[2,/]}] is not an acceptable castType.  Flagging [DoT on Command#${i}] as a BAD_ENTRY.
							}
						}
						
					/next i	
				
				}
				
				/call rewrite_ValidationKey ${Validation_Count} ${Validation_Arg}
		
				/varset Validation_On FALSE
				/goto :Validate_CommandDoTs
			}
			/if (${Defined[Command_DoTs]}) /declare do_CommandDoTs bool outer TRUE
		}
	
	| -------------------------------------------------------------------------------------------------------------------------------------------Import [Assist_Debuffs].
		/if (${Ini[${Character_Ini},Debuffs,Debuff on Assist#1].Length}) {
			/varcalc Validation_Arg ${Validation_Arg} + 1
			:Validate_AssistDebuffs
			/varset Validation_Count 0
			
			/call INItoArray "${Character_Ini},Debuffs,Debuff on Assist#" Assist_Debuffs
			
			/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length} || ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${Validation_Arg},-]} != ${Validation_Count}) /varset Validation_On TRUE
				
			/if (${Validation_On} && ${Validation_Count}) {
			
				/if (${Assist_Debuffs.Size}) {
				
					/for i 1 to ${Assist_Debuffs.Size}
				
					|------------------------------------------------------------------------------------------------------------------------------Missing castType.	
						/if (!${Assist_Debuffs[${i}].Arg[2,/].Length}) {
							/echo [Debuff on Assist#${i}] - [${Assist_Debuffs[${i}]}] does not have a specified castType.  Attempting to fix...
							
							/if (${Int[${Me.Book[${Assist_Debuffs[${i}]}]}]}) {
								/varset Assist_Debuffs[${i}] ${Assist_Debuffs[${i}]}/gem${If[${Me.Gem[${Assist_Debuffs[${i}]}]},${Me.Gem[${Assist_Debuffs[${i}]}]},${DefaultGem}]}
								/call WriteToIni "${Character_Ini},Debuffs,Debuff on Assist#${i}" "${Assist_Debuffs[${i}]}" 1
								/echo [${Assist_Debuffs[${i}].Arg[1,/]}] was found in my spellbook.  Setting [${Assist_Debuffs[${i}].Arg[1,/]}] to [${Assist_Debuffs[${i}].Arg[2,/]}].
							} else /if (${FindItemCount[${Assist_Debuffs[${i}]}]}) {
								/varset Assist_Debuffs[${i}] ${Assist_Debuffs[${i}]}/item
								/call WriteToIni "${Character_Ini},Debuffs,Debuff on Assist#${i}" "${Assist_Debuffs[${i}]}" 1
								/echo [${Assist_Debuffs[${i}].Arg[1,/]}] was found in my inventory.  Setting [${Assist_Debuffs[${i}].Arg[1,/]}] to [${Assist_Debuffs[${i}].Arg[2,/]}]
							} else /if (${Me.AltAbility[${Assist_Debuffs[${i}]}]}) {
								/varset Assist_Debuffs[${i}] ${Assist_Debuffs[${i}]}/alt
								/call WriteToIni "${Character_Ini},Debuffs,Debuff on Assist#${i}" "${Assist_Debuffs[${i}]}" 1
								/echo I have ranks spent in [${Assist_Debuffs[${i}].Arg[1,/]}].  Setting [${Assist_Debuffs[${i}].Arg[1,/]}] to [${Assist_Debuffs[${i}].Arg[2,/]}]	
							} else {
								/varset Assist_Debuffs[${i}] ${Assist_Debuffs[${i}]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Debuffs,Debuff on Assist#${i}" "${Assist_Debuffs[${i}]}" 1
								/echo ERROR: @[Debuff on Assist#${i}] - Could not find a compatible castType for [${Assist_Debuffs[${i}]}].  Flagging [Debuff on Assist#${i}] as a BAD_ENTRY.
							}
						} else {
				
						|--------------------------------------------------------------------------------------------------------------------------Invalid castType.
							/if (${Int[${Assist_Debuffs[${i}].Arg[2,/]}]}) {
								/echo [Debuff on Assist#${i}] - [${Assist_Debuffs[${i}]}] has an invalid castType.  Attempting to fix...
								
								/if (${Int[${Me.Book[${Assist_Debuffs[${i}].Arg[1,/]}]}]}) {
									/varset Assist_Debuffs[${i}] ${Assist_Debuffs[${i}].Arg[1,/]}/gem${If[${Me.Gem[${Assist_Debuffs[${i}]}]},${Me.Gem[${Assist_Debuffs[${i}]}]},${DefaultGem}]}/${Assist_Debuffs[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},Debuffs,Debuff on Assist#${i}" "${Assist_Debuffs[${i}]}" 1
									/echo [Debuff on Assist#${i}] has been changed to [${Assist_Debuffs[${i}]}].  Please review this entry, and restart the macro if needed.
								} else /if (${FindItemCount[${Assist_Debuffs[${i}].Arg[1,/]}]}) {
									/varset Assist_Debuffs[${i}] ${Assist_Debuffs[${i}]}/item/${Assist_Debuffs[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},Debuffs,Debuff on Assist#${i}" "${Assist_Debuffs[${i}]}" 1
									/echo [Debuff on Assist#${i}] has been changed to [${Assist_Debuffs[${i}]}].  Please review this entry, and restart the macro if needed.
								} else /if (${Me.AltAbility[${Assist_Debuffs[${i}]}]}) {
									/varset Assist_Debuffs[${i}] ${Assist_Debuffs[${i}]}/alt/${Assist_Debuffs[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},Debuffs,Debuff on Assist#${i}" "${Assist_Debuffs[${i}]}" 1
									/echo [Debuff on Assist#${i}] has been changed to [${Assist_Debuffs[${i}]}].  Please review this entry, and restart the macro if needed.
								} else {
									/varset Assist_Debuffs[${i}] ${Assist_Debuffs[${i}]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Debuffs,Debuff on Assist#${i}" "${Assist_Debuffs[${i}]}" 1
									/echo ERROR: @[Debuff on Assist#${i}] - Could not find a compatible castType for [${Assist_Debuffs[${i}]}].  Flagging [Debuff on Assist#${i}] as a BAD_ENTRY.
								}
							} else /if (${Assist_Debuffs[${i}].Arg[2,/].Find[gem]}) {
								/if (!${Int[${Me.Book[${Assist_Debuffs[${i}].Arg[1,/]}]}]}) {
									/varset Assist_Debuffs[${i}] ${Assist_Debuffs[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Debuffs,Debuff on Assist#${i}" "${Assist_Debuffs[${i}]}" 1
									/echo ERROR: @[Debuff on Assist#${i}] - [${Assist_Debuffs[${i}].Arg[1,/]}] was not found in my spellbook.  Flagging [Debuff on Assist#${i}] as a BAD_ENTRY.
								}
							} else /if (${Assist_Debuffs[${i}].Arg[2,/].Equal[item]}) {
								/if (!${FindItemCount[${Assist_Debuffs[${i}].Arg[1,/]}]}) {
									/varset Assist_Debuffs[${i}] ${Assist_Debuffs[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Debuffs,Debuff on Assist#${i}" "${Assist_Debuffs[${i}]}" 1
									/echo ERROR: @[Debuff on Assist#${i}] - I do not have [${Assist_Debuffs[${i}].Arg[1,/]}] in my inventory.  Flagging [Debuff on Assist#${i}] as a BAD_ENTRY.
								}
							} else /if (${Assist_Debuffs[${i}].Arg[2,/].Equal[alt]} || ${Assist_Debuffs[${i}].Arg[2,/].Equal[aa]}) {
								/if (!${Me.AltAbility[${Assist_Debuffs[${i}].Arg[1,/]}]}) {
									/varset Assist_Debuffs[${i}] ${Assist_Debuffs[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Debuffs,Debuff on Assist#${i}" "${Assist_Debuffs[${i}]}" 1
									/echo ERROR: @[Debuff on Assist#${i}] - I do not have any ranks in [${Assist_Debuffs[${i}].Arg[1,/]}].  Flagging [Debuff on Assist#${i}] as a BAD_ENTRY.
								}
							} else /if (${Assist_Debuffs[${i}].Arg[2,/].Equal[BAD_ENTRY]}) {
								/echo ERROR: @[Debuff on Assist#${i}] - There is a problem with the user entry for [${Assist_Debuffs[${i}].Arg[1,/]}] in [${Character_Ini}].  Please review this entry, and restart the macro.
							} else {
								/varset Assist_Debuffs[${i}] ${Assist_Debuffs[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Debuffs,Debuff on Assist#${i}" "${Assist_Debuffs[${i}]}" 1
								/echo ERROR: @[Debuff on Assist#${i}] - [${Assist_Debuffs[${i}].Arg[2,/]}] is not an acceptable castType.  Flagging [Debuff on Assist#${i}] as a BAD_ENTRY.
							}
						}
						
					/next i
				}
				
				/call rewrite_ValidationKey ${Validation_Count} ${Validation_Arg}
				
				/varset Validation_On FALSE
				/goto :Validate_AssistDebuffs
			}
			/if (${Defined[Assist_Debuffs]}) /declare do_AssistDebuffs bool outer TRUE
		}
		
	| -------------------------------------------------------------------------------------------------------------------------------------------[Command_Debuffs].	
		/if (${Ini[${Character_Ini},Debuffs,Debuff on Command#1].Length}) {
			/varcalc Validation_Arg ${Validation_Arg} + 1
			:Validate_CommandDebuffs
			/varset Validation_Count 0
			
			/call INItoArray "${Character_Ini},Debuffs,Debuff on Command#" Command_Debuffs
			
			/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length} || ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${Validation_Arg},-]} != ${Validation_Count}) /varset Validation_On TRUE
				
			/if (${Validation_On} && ${Validation_Count}) {
			
				/if (${Command_Debuffs.Size}) {
				
					/for i 1 to ${Command_Debuffs.Size}
				
					|------------------------------------------------------------------------------------------------------------------------------Missing castType.	
						/if (!${Command_Debuffs[${i}].Arg[2,/].Length}) {
							/echo [Debuff on Command#${i}] - [${Command_Debuffs[${i}]}] does not have a specified castType.  Attempting to fix...
							
							/if (${Int[${Me.Book[${Command_Debuffs[${i}]}]}]}) {
								/varset Command_Debuffs[${i}] ${Command_Debuffs[${i}]}/gem${If[${Me.Gem[${Command_Debuffs[${i}]}]},${Me.Gem[${Command_Debuffs[${i}]}]},${DefaultGem}]}
								/call WriteToIni "${Character_Ini},Debuffs,Debuff on Command#${i}" "${Command_Debuffs[${i}]}" 1
								/echo [${Command_Debuffs[${i}].Arg[1,/]}] was found in my spellbook.  Setting [${Command_Debuffs[${i}].Arg[1,/]}] to [${Command_Debuffs[${i}].Arg[2,/]}].
							} else /if (${FindItemCount[${Command_Debuffs[${i}]}]}) {
								/varset Command_Debuffs[${i}] ${Command_Debuffs[${i}]}/item
								/call WriteToIni "${Character_Ini},Debuffs,Debuff on Command#${i}" "${Command_Debuffs[${i}]}" 1
								/echo [${Command_Debuffs[${i}].Arg[1,/]}] was found in my inventory.  Setting [${Command_Debuffs[${i}].Arg[1,/]}] to [${Command_Debuffs[${i}].Arg[2,/]}]
							} else /if (${Me.AltAbility[${Command_Debuffs[${i}]}]}) {
								/varset Command_Debuffs[${i}] ${Command_Debuffs[${i}]}/alt
								/call WriteToIni "${Character_Ini},Debuffs,Debuff on Command#${i}" "${Command_Debuffs[${i}]}" 1
								/echo I have ranks spent in [${Command_Debuffs[${i}].Arg[1,/]}].  Setting [${Command_Debuffs[${i}].Arg[1,/]}] to [${Command_Debuffs[${i}].Arg[2,/]}]	
							} else {
								/varset Command_Debuffs[${i}] ${Command_Debuffs[${i}]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Debuffs,Debuff on Command#${i}" "${Command_Debuffs[${i}]}" 1
								/echo ERROR: @[Debuff on Command#${i}] - Could not find a compatible castType for [${Command_Debuffs[${i}]}].  Flagging [Debuff on Command#${i}] as a BAD_ENTRY.
							}
						} else {
				
						|--------------------------------------------------------------------------------------------------------------------------Invalid castType.
							/if (${Int[${Command_Debuffs[${i}].Arg[2,/]}]}) {
								/echo [Debuff on Command#${i}] - [${Command_Debuffs[${i}]}] has an invalid castType.  Attempting to fix...
								
								/if (${Int[${Me.Book[${Command_Debuffs[${i}].Arg[1,/]}]}]}) {
									/varset Command_Debuffs[${i}] ${Command_Debuffs[${i}].Arg[1,/]}/gem${If[${Me.Gem[${Command_Debuffs[${i}]}]},${Me.Gem[${Command_Debuffs[${i}]}]},${DefaultGem}]}/${Command_Debuffs[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},Debuffs,Debuff on Command#${i}" "${Command_Debuffs[${i}]}" 1
									/echo [Debuff on Command#${i}] has been changed to [${Command_Debuffs[${i}]}].  Please review this entry, and restart the macro if needed.
								} else /if (${FindItemCount[${Command_Debuffs[${i}].Arg[1,/]}]}) {
									/varset Command_Debuffs[${i}] ${Command_Debuffs[${i}]}/item/${Command_Debuffs[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},Debuffs,Debuff on Command#${i}" "${Command_Debuffs[${i}]}" 1
									/echo [Debuff on Command#${i}] has been changed to [${Command_Debuffs[${i}]}].  Please review this entry, and restart the macro if needed.
								} else /if (${Me.AltAbility[${Command_Debuffs[${i}]}]}) {
									/varset Command_Debuffs[${i}] ${Command_Debuffs[${i}]}/alt/${Command_Debuffs[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},Debuffs,Debuff on Command#${i}" "${Command_Debuffs[${i}]}" 1
									/echo [Debuff on Command#${i}] has been changed to [${Command_Debuffs[${i}]}].  Please review this entry, and restart the macro if needed.
								} else {
									/varset Command_Debuffs[${i}] ${Command_Debuffs[${i}]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Debuffs,Debuff on Command#${i}" "${Command_Debuffs[${i}]}" 1
									/echo ERROR: @[Debuff on Command#${i}] - Could not find a compatible castType for [${Command_Debuffs[${i}]}].  Flagging [Debuff on Command#${i}] as a BAD_ENTRY.
								}
							} else /if (${Command_Debuffs[${i}].Arg[2,/].Find[gem]}) {
								/if (!${Int[${Me.Book[${Command_Debuffs[${i}].Arg[1,/]}]}]}) {
									/varset Command_Debuffs[${i}] ${Command_Debuffs[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Debuffs,Debuff on Command#${i}" "${Command_Debuffs[${i}]}" 1
									/echo ERROR: @[Debuff on Command#${i}] - [${Command_Debuffs[${i}].Arg[1,/]}] was not found in my spellbook.  Flagging [Debuff on Command#${i}] as a BAD_ENTRY.
								}
							} else /if (${Command_Debuffs[${i}].Arg[2,/].Equal[item]}) {
								/if (!${FindItemCount[${Command_Debuffs[${i}].Arg[1,/]}]}) {
									/varset Command_Debuffs[${i}] ${Command_Debuffs[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Debuffs,Debuff on Command#${i}" "${Command_Debuffs[${i}]}" 1
									/echo ERROR: @[Debuff on Command#${i}] - I do not have [${Command_Debuffs[${i}].Arg[1,/]}] in my inventory.  Flagging [Debuff on Command#${i}] as a BAD_ENTRY.
								}
							} else /if (${Command_Debuffs[${i}].Arg[2,/].Equal[alt]} || ${Command_Debuffs[${i}].Arg[2,/].Equal[aa]}) {
								/if (!${Me.AltAbility[${Command_Debuffs[${i}].Arg[1,/]}]}) {
									/varset Command_Debuffs[${i}] ${Command_Debuffs[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Debuffs,Debuff on Command#${i}" "${Command_Debuffs[${i}]}" 1
									/echo ERROR: @[Debuff on Command#${i}] - I do not have any ranks in [${Command_Debuffs[${i}].Arg[1,/]}].  Flagging [Debuff on Command#${i}] as a BAD_ENTRY.
								}
							} else /if (${Command_Debuffs[${i}].Arg[2,/].Equal[BAD_ENTRY]}) {
								/echo ERROR: @[Debuff on Command#${i}] - There is a problem with the user entry for [${Command_Debuffs[${i}].Arg[1,/]}] in [${Character_Ini}].  Please review this entry, and restart the macro.
							} else {
								/varset Command_Debuffs[${i}] ${Command_Debuffs[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Debuffs,Debuff on Command#${i}" "${Command_Debuffs[${i}]}" 1
								/echo ERROR: @[Debuff on Command#${i}] - [${Command_Debuffs[${i}].Arg[2,/]}] is not an acceptable castType.  Flagging [Debuff on Command#${i}] as a BAD_ENTRY.
							}
						}
						
					/next i
				}
				
				/call rewrite_ValidationKey ${Validation_Count} ${Validation_Arg}
		
				/varset Validation_On FALSE
				/goto :Validate_CommandDebuffs
			}
			/if (${Defined[Command_Debuffs]}) /declare do_CommandDebuffs bool outer TRUE
		}
		
	| --------------------------------------------------------------------------------------------------------------------------------------------[Rain].
		/if (${Ini[${Character_Ini},Rain,Rain#1].Length}) {
			/varcalc Validation_Arg ${Validation_Arg} + 1
			:Validate_Rain
			/varset Validation_Count 0
			
			/call INItoArray ${Character_Ini},Rain,Rain# Rain_Spells
		
			/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length} || ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${Validation_Arg},-]} != ${Validation_Count}) /varset Validation_On TRUE
				
			/if (${Validation_On} && ${Validation_Count}) {
			
				/if (${Rain_Spells.Size}) {
				
					/for i 1 to ${Rain_Spells.Size}

					|------------------------------------------------------------------------------------------------------------------------------Missing castType.	
						/if (!${Rain_Spells[${i}].Arg[2,/].Length}) {
							/echo [Rain#${i}] - [${Rain_Spells[${i}]}] does not have a specified castType.  Attempting to fix...
							
							/if (${Int[${Me.Book[${Rain_Spells[${i}]}]}]}) {
								/varset Rain_Spells[${i}] ${Rain_Spells[${i}]}/gem${If[${Me.Gem[${Rain_Spells[${i}]}]},${Me.Gem[${Rain_Spells[${i}]}]},${DefaultGem}]}
								/call WriteToIni "${Character_Ini},Rain,Rain#${i}" "${Rain_Spells[${i}]}" 1
								/echo [${Rain_Spells[${i}].Arg[1,/]}] was found in my spellbook.  Setting [${Rain_Spells[${i}].Arg[1,/]}] to [${Rain_Spells[${i}].Arg[2,/]}].
							} else /if (${FindItemCount[${Rain_Spells[${i}]}]}) {
								/varset Rain_Spells[${i}] ${Rain_Spells[${i}]}/item
								/call WriteToIni "${Character_Ini},Rain,Rain#${i}" "${Rain_Spells[${i}]}" 1
								/echo [${Rain_Spells[${i}].Arg[1,/]}] was found in my inventory.  Setting [${Rain_Spells[${i}].Arg[1,/]}] to [${Rain_Spells[${i}].Arg[2,/]}]
							} else /if (${Me.AltAbility[${Rain_Spells[${i}]}]}) {
								/varset Rain_Spells[${i}] ${Rain_Spells[${i}]}/alt
								/call WriteToIni "${Character_Ini},Rain,Rain#${i}" "${Rain_Spells[${i}]}" 1
								/echo I have ranks spent in [${Rain_Spells[${i}].Arg[1,/]}].  Setting [${Rain_Spells[${i}].Arg[1,/]}] to [${Rain_Spells[${i}].Arg[2,/]}]	
							} else {
								/varset Rain_Spells[${i}] ${Rain_Spells[${i}]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Rain,Rain#${i}" "${Rain_Spells[${i}]}" 1
								/echo ERROR: @[Rain#${i}] - Could not find a compatible castType for [${Rain_Spells[${i}]}].  Flagging [Rain#${i}] as a BAD_ENTRY.
							}
						} else {
							
						|--------------------------------------------------------------------------------------------------------------------------Invalid castType.
							/if (${Int[${Rain_Spells[${i}].Arg[2,/]}]}) {
								/echo [Rain#${i}] - [${Rain_Spells[${i}]}] has an invalid castType.  Attempting to fix...
								
								/if (${Int[${Me.Book[${Rain_Spells[${i}].Arg[1,/]}]}]}) {
									/varset Rain_Spells[${i}] ${Rain_Spells[${i}].Arg[1,/]}/gem${If[${Me.Gem[${Rain_Spells[${i}]}]},${Me.Gem[${Rain_Spells[${i}]}]},${DefaultGem}]}/${Rain_Spells[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},Rain,Rain#${i}" "${Rain_Spells[${i}]}" 1
									/echo [Rain#${i}] has been changed to [${Rain_Spells[${i}]}].  Please review this entry, and restart the macro if needed.
								} else /if (${FindItemCount[${Rain_Spells[${i}].Arg[1,/]}]}) {
									/varset Rain_Spells[${i}] ${Rain_Spells[${i}]}/item/${Rain_Spells[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},Rain,Rain#${i}" "${Rain_Spells[${i}]}" 1
									/echo [Rain#${i}] has been changed to [${Rain_Spells[${i}]}].  Please review this entry, and restart the macro if needed.
								} else /if (${Me.AltAbility[${Rain_Spells[${i}]}]}) {
									/varset Rain_Spells[${i}] ${Rain_Spells[${i}]}/alt/${Rain_Spells[${i}].Arg[2,/]}
									/call WriteToIni "${Character_Ini},Rain,Rain#${i}" "${Rain_Spells[${i}]}" 1
									/echo [Rain#${i}] has been changed to [${Rain_Spells[${i}]}].  Please review this entry, and restart the macro if needed.
								} else {
									/varset Rain_Spells[${i}] ${Rain_Spells[${i}]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Rain,Rain#${i}" "${Rain_Spells[${i}]}" 1
									/echo ERROR: @[Rain#${i}] - Could not find a compatible castType for [${Rain_Spells[${i}]}].  Flagging [Rain#${i}] as a BAD_ENTRY.
								}
							} else /if (${Rain_Spells[${i}].Arg[2,/].Find[gem]}) {
								/if (!${Int[${Me.Book[${Rain_Spells[${i}].Arg[1,/]}]}]}) {
									/varset Rain_Spells[${i}] ${Rain_Spells[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Rain,Rain#${i}" "${Rain_Spells[${i}]}" 1
									/echo ERROR: @[Rain#${i}] - [${Rain_Spells[${i}].Arg[1,/]}] was not found in my spellbook.  Flagging [Rain#${i}] as a BAD_ENTRY.
								}
							} else /if (${Rain_Spells[${i}].Arg[2,/].Equal[item]}) {
								/if (!${FindItemCount[${Rain_Spells[${i}].Arg[1,/]}]}) {
									/varset Rain_Spells[${i}] ${Rain_Spells[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Rain,Rain#${i}" "${Rain_Spells[${i}]}" 1
									/echo ERROR: @[Rain#${i}] - I do not have [${Rain_Spells[${i}].Arg[1,/]}] in my inventory.  Flagging [Rain#${i}] as a BAD_ENTRY.
								}
							} else /if (${Rain_Spells[${i}].Arg[2,/].Equal[alt]} || ${Rain_Spells[${i}].Arg[2,/].Equal[aa]}) {
								/if (!${Me.AltAbility[${Rain_Spells[${i}].Arg[1,/]}]}) {
									/varset Rain_Spells[${i}] ${Rain_Spells[${i}].Arg[1,/]}/BAD_ENTRY
									/call WriteToIni "${Character_Ini},Rain,Rain#${i}" "${Rain_Spells[${i}]}" 1
									/echo ERROR: @[Rain#${i}] - I do not have any ranks in [${Rain_Spells[${i}].Arg[1,/]}].  Flagging [Rain#${i}] as a BAD_ENTRY.
								}
							} else /if (${Rain_Spells[${i}].Arg[2,/].Equal[BAD_ENTRY]}) {
								/echo ERROR: @[Rain#${i}] - There is a problem with the user entry for [${Rain_Spells[${i}].Arg[1,/]}] in [${Character_Ini}].  Please review this entry, and restart the macro.
							} else {
								/varset Rain_Spells[${i}] ${Rain_Spells[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Rain,Rain#${i}" "${Rain_Spells[${i}]}" 1
								/echo ERROR: @[Rain#${i}] - [${Rain_Spells[${i}].Arg[2,/]}] is not an acceptable castType.  Flagging [Rain#${i}] as a BAD_ENTRY.
							}
						}
					/next i
				
				}
				/call rewrite_ValidationKey ${Validation_Count} ${Validation_Arg}
		
				/varset Validation_On FALSE
				/goto :Validate_Rain
			}
			/if (${Defined[Rain_Spells]}) {
				/declare do_Rain bool outer FALSE
				/declare RainCounter int outer 1
			}
		}
	
	| --------------------------------------------------------------------------------------------------------------------------------------------[QuickBurns].
		/if (${Ini[${Character_Ini},Burn,Quick Burn#1].Length}) {
			/varcalc Validation_Arg ${Validation_Arg} + 1
			:Validate_QuickBurns
			/varset Validation_Count 0
			
			/call IniToArray "${Character_Ini},Burn,Quick Burn#" QuickBurns
			
			/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length} || ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${Validation_Arg},-]} != ${Validation_Count}) /varset Validation_On TRUE
				
			/if (${Validation_On} && ${Validation_Count}) {
			
				/if (${QuickBurns.Size}) {
					
					/for i 1 to ${QuickBurns.Size}
				
						/if (!${QuickBurns[${i}].Arg[2,/].Length}) {
							/echo [Quick Burn#${i}] - [${QuickBurns[${i}]}] does not have a specified castType.  Attempting to fix...
							
							/if (${Int[${Me.Book[${QuickBurns[${i}]}]}]}) {
								/varset QuickBurns[${i}] ${QuickBurns[${i}]}/gem${If[${Me.Gem[${QuickBurns[${i}]}]},${Me.Gem[${QuickBurns[${i}]}]},${DefaultGem}]}
								/call WriteToIni "${Character_Ini},Burn,Quick Burn#${i}" "${QuickBurns[${i}]}" 1
								/echo [${QuickBurns[${i}].Arg[1,/]}] was found in my spellbook.  Setting [${QuickBurns[${i}].Arg[1,/]}] to [${QuickBurns[${i}].Arg[2,/]}].
							} else /if (${FindItemCount[${QuickBurns[${i}]}]}) {
								/varset QuickBurns[${i}] ${QuickBurns[${i}]}/item
								/call WriteToIni "${Character_Ini},Burn,Quick Burn#${i}" "${QuickBurns[${i}]}" 1
								/echo [${QuickBurns[${i}].Arg[1,/]}] was found in my inventory.  Setting [${QuickBurns[${i}].Arg[1,/]}] to [${QuickBurns[${i}].Arg[2,/]}]
							} else /if (${Me.AltAbility[${QuickBurns[${i}]}]}) {
								/varset QuickBurns[${i}] ${QuickBurns[${i}]}/alt
								/call WriteToIni "${Character_Ini},Burn,Quick Burn#${i}" "${QuickBurns[${i}]}" 1
								/echo I have ranks spent in [${QuickBurns[${i}].Arg[1,/]}].  Setting [${QuickBurns[${i}].Arg[1,/]}] to [${QuickBurns[${i}].Arg[2,/]}]	
							} else /if (${Me.CombatAbility[${QuickBurns[${i}]}]}) {
								/varset QuickBurns[${i}] ${QuickBurns[${i}]}/disc
								/call WriteToIni "${Character_Ini},Burn,Quick Burn#${i}" "${QuickBurns[${i}]}" 1
								/echo I found [${QuickBurns[${i}].Arg[1,/]}] in my disciplines window.  Setting [${QuickBurns[${i}].Arg[1,/]}] to [${QuickBurns[${i}].Arg[2,/]}]	
							} else {
								/varset QuickBurns[${i}] ${QuickBurns[${i}]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Burn,Quick Burn#${i}" "${QuickBurns[${i}]}" 1
								/echo ERROR: @[Quick Burn#${i}] - Could not find a compatible castType for [${QuickBurns[${i}]}].  Flagging [Quick Burn#${i}] as a BAD_ENTRY.
							}
						}
					
					|--------------------------------------------------------------------------------------------------------------------------Invalid castType.
						/if (${QuickBurns[${i}].Arg[2,/].Find[gem]}) {
							/if (!${Int[${Me.Book[${QuickBurns[${i}].Arg[1,/]}]}]}) {
								/varset QuickBurns[${i}] ${QuickBurns[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Burn,Quick Burn#${i}" "${QuickBurns[${i}]}" 1
								/echo ERROR: @[Quick Burn#${i}] - [${QuickBurns[${i}].Arg[1,/]}] was not found in my spellbook.  Flagging [Quick Burn#${i}] as a BAD_ENTRY.
							}
						} else /if (${QuickBurns[${i}].Arg[2,/].Equal[item]}) {
							/if (!${FindItemCount[${QuickBurns[${i}].Arg[1,/]}]}) {
								/varset QuickBurns[${i}] ${QuickBurns[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Burn,Quick Burn#${i}" "${QuickBurns[${i}]}" 1
								/echo ERROR: @[Quick Burn#${i}] - I do not have [${QuickBurns[${i}].Arg[1,/]}] in my inventory.  Flagging [Quick Burn#${i}] as a BAD_ENTRY.
							}
						} else /if (${QuickBurns[${i}].Arg[2,/].Equal[alt]} || ${QuickBurns[${i}].Arg[2,/].Equal[aa]}) {
							/if (!${Me.AltAbility[${QuickBurns[${i}].Arg[1,/]}]}) {
								/varset QuickBurns[${i}] ${QuickBurns[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Burn,Quick Burn#${i}" "${QuickBurns[${i}]}" 1
								/echo ERROR: @[Quick Burn#${i}] - I do not have any ranks in [${QuickBurns[${i}].Arg[1,/]}].  Flagging [Quick Burn#${i}] as a BAD_ENTRY.
							}
						} else /if (${QuickBurns[${i}].Arg[2,/].Equal[disc]}) {
							/if (!${Me.CombatAbility[${QuickBurns[${i}].Arg[1,/]}]}) {
								/varset QuickBurns[${i}] ${QuickBurns[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Burn,Quick Burn#${i}" "${QuickBurns[${i}]}" 1
								/echo ERROR: @[Quick Burn#${i}] - I do not have [${QuickBurns[${i}].Arg[1,/]}] listed in my disciplines window.  Flagging [Quick Burn#${i}] as a BAD_ENTRY.
							}
						} else /if (${QuickBurns[${i}].Arg[2,/].Equal[BAD_ENTRY]}) {
							/echo ERROR: @[Quick Burn#${i}] - There is a problem with the user entry for [${QuickBurns[${i}].Arg[1,/]}] in [${Character_Ini}].  Please review this entry, and restart the macro.
						} else {
							/varset QuickBurns[${i}] ${QuickBurns[${i}].Arg[1,/]}/BAD_ENTRY
							/call WriteToIni "${Character_Ini},Burn,Quick Burn#${i}" "${QuickBurns[${i}]}" 1
							/echo ERROR: @[Quick Burn#${i}] - [${QuickBurns[${i}].Arg[2,/]}] is not an acceptable castType.  Flagging [Quick Burn#${i}] as a BAD_ENTRY.
						}
					/next i
				}
				
				/call rewrite_ValidationKey ${Validation_Count} ${Validation_Arg}
		
				/varset Validation_On FALSE
				/goto :Validate_QuickBurns
			}
			/if (${Defined[QuickBurns]}) {
				/declare use_QBurn bool outer FALSE
				/declare QBurn_Counter int outer 1
			}
		}
		
	| --------------------------------------------------------------------------------------------------------------------------------------------[FullBurns].	
		/if (${Ini[${Character_Ini},Burn,Full Burn#1].Length}) {
			/varcalc Validation_Arg ${Validation_Arg} + 1
			:Validate_FullBurns
			/varset Validation_Count 0
		
			/call IniToArray "${Character_Ini},Burn,Full Burn#" FullBurns
			
			/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length} || ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${Validation_Arg},-]} != ${Validation_Count}) /varset Validation_On TRUE
				
			/if (${Validation_On} && ${Validation_Count}) {
			
				/if (${FullBurns.Size}) {
				
					/for i 1 to ${FullBurns.Size}
				
						/if (!${FullBurns[${i}].Arg[2,/].Length}) {
							/echo [Full Burn#${i}] - [${FullBurns[${i}]}] does not have a specified castType.  Attempting to fix...
							
							/if (${Int[${Me.Book[${FullBurns[${i}]}]}]}) {
								/varset FullBurns[${i}] ${FullBurns[${i}]}/gem${If[${Me.Gem[${FullBurns[${i}]}]},${Me.Gem[${FullBurns[${i}]}]},${DefaultGem}]}
								/call WriteToIni "${Character_Ini},Burn,Full Burn#${i}" "${FullBurns[${i}]}" 1
								/echo [${FullBurns[${i}].Arg[1,/]}] was found in my spellbook.  Setting [${FullBurns[${i}].Arg[1,/]}] to [${FullBurns[${i}].Arg[2,/]}].
							} else /if (${FindItemCount[${FullBurns[${i}]}]}) {
								/varset FullBurns[${i}] ${FullBurns[${i}]}/item
								/call WriteToIni "${Character_Ini},Burn,Full Burn#${i}" "${FullBurns[${i}]}" 1
								/echo [${FullBurns[${i}].Arg[1,/]}] was found in my inventory.  Setting [${FullBurns[${i}].Arg[1,/]}] to [${FullBurns[${i}].Arg[2,/]}]
							} else /if (${Me.AltAbility[${FullBurns[${i}]}]}) {
								/varset FullBurns[${i}] ${FullBurns[${i}]}/alt
								/call WriteToIni "${Character_Ini},Burn,Full Burn#${i}" "${FullBurns[${i}]}" 1
								/echo I have ranks spent in [${FullBurns[${i}].Arg[1,/]}].  Setting [${FullBurns[${i}].Arg[1,/]}] to [${FullBurns[${i}].Arg[2,/]}]	
							} else /if (${Me.CombatAbility[${FullBurns[${i}]}]}) {
								/varset FullBurns[${i}] ${FullBurns[${i}]}/disc
								/call WriteToIni "${Character_Ini},Burn,Full Burn#${i}" "${FullBurns[${i}]}" 1
								/echo I found [${FullBurns[${i}].Arg[1,/]}] in my disciplines window.  Setting [${FullBurns[${i}].Arg[1,/]}] to [${FullBurns[${i}].Arg[2,/]}]	
							} else {
								/varset FullBurns[${i}] ${FullBurns[${i}]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Burn,Full Burn#${i}" "${FullBurns[${i}]}" 1
								/echo ERROR: @[Full Burn#${i}] - Could not find a compatible castType for [${FullBurns[${i}]}].  Flagging [Full Burn#${i}] as a BAD_ENTRY.
							}
						}
					
					|--------------------------------------------------------------------------------------------------------------------------Invalid castType.
						/if (${FullBurns[${i}].Arg[2,/].Find[gem]}) {
							/if (!${Int[${Me.Book[${FullBurns[${i}].Arg[1,/]}]}]}) {
								/varset FullBurns[${i}] ${FullBurns[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Burn,Full Burn#${i}" "${FullBurns[${i}]}" 1
								/echo ERROR: @[Full Burn#${i}] - [${FullBurns[${i}].Arg[1,/]}] was not found in my spellbook.  Flagging [Full Burn#${i}] as a BAD_ENTRY.
							}
						} else /if (${FullBurns[${i}].Arg[2,/].Equal[item]}) {
							/if (!${FindItemCount[${FullBurns[${i}].Arg[1,/]}]}) {
								/varset FullBurns[${i}] ${FullBurns[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Burn,Full Burn#${i}" "${FullBurns[${i}]}" 1
								/echo ERROR: @[Full Burn#${i}] - I do not have [${FullBurns[${i}].Arg[1,/]}] in my inventory.  Flagging [Full Burn#${i}] as a BAD_ENTRY.
							}
						} else /if (${FullBurns[${i}].Arg[2,/].Equal[alt]} || ${FullBurns[${i}].Arg[2,/].Equal[aa]}) {
							/if (!${Me.AltAbility[${FullBurns[${i}].Arg[1,/]}]}) {
								/varset FullBurns[${i}] ${FullBurns[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Burn,Full Burn#${i}" "${FullBurns[${i}]}" 1
								/echo ERROR: @[Full Burn#${i}] - I do not have any ranks in [${FullBurns[${i}].Arg[1,/]}].  Flagging [Full Burn#${i}] as a BAD_ENTRY.
							}
						} else /if (${FullBurns[${i}].Arg[2,/].Equal[disc]}) {
							/if (!${Me.CombatAbility[${FullBurns[${i}].Arg[1,/]}]}) {
								/varset FullBurns[${i}] ${FullBurns[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Burn,Full Burn#${i}" "${FullBurns[${i}]}" 1
								/echo ERROR: @[Full Burn#${i}] - I do not have [${FullBurns[${i}].Arg[1,/]}] listed in my disciplines window.  Flagging [Full Burn#${i}] as a BAD_ENTRY.
							}
						} else /if (${FullBurns[${i}].Arg[2,/].Equal[BAD_ENTRY]}) {
							/echo ERROR: @[Full Burn#${i}] - There is a problem with the user entry for [${FullBurns[${i}].Arg[1,/]}] in [${Character_Ini}].  Please review this entry, and restart the macro.
						} else {
							/varset FullBurns[${i}] ${FullBurns[${i}].Arg[1,/]}/BAD_ENTRY
							/call WriteToIni "${Character_Ini},Burn,Full Burn#${i}" "${FullBurns[${i}]}" 1
							/echo ERROR: @[Full Burn#${i}] - [${FullBurns[${i}].Arg[2,/]}] is not an acceptable castType.  Flagging [Full Burn#${i}] as a BAD_ENTRY.
						}
					/next i
				}
				
				/call rewrite_ValidationKey ${Validation_Count} ${Validation_Arg}
				
				/varset Validation_On FALSE
				/goto :Validate_FullBurns
			}
			/if (${Defined[FullBurns]}) {
				/declare use_FBurn bool outer FALSE
				/declare FBurn_Counter int outer 1
			}
		}
		
|------------------------------------------------------------------------------------------------------------If Assisting variables are defined, turn assists on.
	/if (${Defined[AssistType]} || ${Defined[MyAbilities]} || ${Defined[MyNukes]} || ${Defined[Assist_DoTs]} || ${Defined[Command_DoTs]} || ${Defined[Assist_Debuffs]} || ${Defined[Command_Debuffs]} || ${Defined[Rain_Spells]} || ${Defined[QuickBurns]} || ${Defined[FullBurns]}) /declare AssistsOn bool outer TRUE

/if (${Debug}) /echo <== setup_Assists -|
/RETURN