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
| #EVENT AssistOn
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
						/if (${Spawn[${FollowTarget}].Distance} > ${LeashLength}) /goto :movetoAssist_Loop
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
							/squelch /stick hold moveback 40
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

	/varset AssistStickDistanceSetting ${Spawn[${Target.ID}].MaxRangeTo}

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
					
					/if (!${Me.Combat} && ${Target.Distance} < 35 || ${Target.Distance} > 100) /squelch /stick hold moveback 40
					
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
	
|--------------------------------------------------Check loot, and reaquire follow.
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
			
			/if (${MyAbilities[${i}].Arg[2,/].Equal[ability]}) {
				/if (!${Me.AbilityReady[${MyAbilities[${i}].Arg[1,/]}]}) {
					/if (${Debug}) /echo |- CombatAbilities -| Ability [${MyAbilities[${i}].Arg[1,/]}] is not ready.
				} else {
					/if (${MyAbilities[${i}].Arg[1,/].Equal[Backstab]}) {
						/call Attempt_Backstab
					} else /if (${MyAbilities[${i}].Arg[1,/].Equal[Bash]}) {
						/call Attempt_Bash
					} else /if (${MyAbilities[${i}].Arg[1,/].Equal[Taunt]}) {
						/call Attempt_Taunt
					} else /if (${Me.AbilityReady[${MyAbilities[${i}].Arg[1,/]}]}) {
					
					|-----------------------------------------------------------------Misc.
						/doability "${MyAbilities[${i}].Arg[1,/]}"
					}
				}
			} else /if (${MyAbilities[${i}].Arg[2,/].Equal[alt]}) {
				/if (!${Me.AltAbilityReady[${MyAbilities[${i}].Arg[1,/]}]} || ${alt_Timer_${Me.AltAbility[${MyAbilities[${i}].Arg[1,/]}].ID}}) {
					/if (${Debug}) /echo |- CombatAbilities -| Alternate Ability [${MyAbilities[${i}].Arg[1,/]}] is not ready.
				} else {
					/call e3_Cast ${AssistTarget} "${MyAbilities[${i}].Arg[1,/]}" alt
				}
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

	/if (${Me.Inventory[Offhand].Type.NotEqual[Shield]} && !${Me.AltAbility[2 Hand Bash]}) {
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
		
		/if (${Select[${Me.TargetOfTarget},${Me.CleanName}]}) {
			/if (${Debug}) /echo |- Attempt_Target -| I currently have aggro on [${Target.CleanName}], skipping taunt.
		} else {
			/doability Taunt
		}
	}

/if (${Debug}) /echo <== Attempt_Taunt -|
/RETURN



|----------------------------------------------------------------------------------------|
|- Casts nukes listed in the [Nukes] section of the Character_Ini, on AssistTargets.	-|
|----------------------------------------------------------------------------------------|
SUB Nukes
/if (${Debug}) /echo |- Nukes ==>
	
	/declare i int local
	/for i 1 to ${MyNukes.Size}

	|---------------------------------------------------------------Cast a spell.
		/if (${Select[${MyNukes[${i}].Arg[2,/].Left[3]},GEM,Gem,gem]}) {
		
		|--------------------------------------Check minimum mana to cast the spell.
			/if (${Me.PctMana}>=${MyNukes[${i}].Arg[3,/].Replace[m,]}) {
			
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
/varset Debug 1
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
/varset Debug 0
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

|---------------------Validate target.
	/if (${SpawnCount[id ${swarmTarget}]} && ${Spawn[${swarmTarget}].Type.NotEqual[Corpse]} && ${Select[${Spawn[${swarmTarget}].Type},${AcceptableTargetTypes}]} && ${SpawnCount[pc ${ChatSender}]}) {
		
	|-------------------------------------------------------------------------------------------------Magician abilities.
		/if (${Me.Class.ShortName.Equal[MAG]}) {
			/if (${Me.AltAbilityReady[162]} && !${alt_Timer_162}) {
				/call e3_Cast ${swarmTarget} 162 alt
			} else /if (!${Me.AltAbilityReady[162]} || ${alt_Timer_162}) {
				/if (${Verbosity}) /docommand ${ChatToggle} Servant of Ro is not ready.
			}
			/if (${Me.AltAbilityReady[195]} && !${alt_Timer_195}) {
				/delay 5
				/call e3_Cast ${swarmTarget} 195 alt
			} else /if (!${Me.AltAbilityReady[195]} || ${alt_Timer_195}) {
				/if (${Verbosity}) /docommand ${ChatToggle} Host of the Elements is not ready.
			}
			
	|------------------------------------------------------------------------------------------Necromancer abilities.
		} else /if (${Me.Class.ShortName.Equal[NEC]}) {
			/if (${Me.AltAbilityReady[257]} && !${alt_Timer_257}) {
				/call e3_Cast ${swarmTarget} 257 alt
			} else /if (!${Me.AltAbilityReady[257]} || ${alt_Timer_257}) {
				/if (${Verbosity}) /docommand ${ChatToggle} Swarm of Decay is not ready.
			}
			/if (${Me.AltAbilityReady[163]} && !${alt_Timer_163}) {
				/delay 5
				/call e3_Cast ${swarmTarget} 163 alt
			} else /if (!${Me.AltAbilityReady[163]} || ${alt_Timer_163}) {
				/if (${Verbosity}) /docommand ${ChatToggle} Wake The Dead is not ready.
			}
			
	|-----------------------------------------------------------------------------------------------Shaman abilities.
		} else /if (${Me.Class.ShortName.Equal[SHM]}) {
			/if (${Me.AltAbilityReady[165]} && !${alt_Timer_165}) {
				/call e3_Cast ${swarmTarget} 165 alt
			}
			
	|----------------------------------------------------------------------------------------------Wizard abilities.		
		} else /if (${Me.Class.ShortName.Equal[WIZ]}) {
			/if (${Me.AltAbilityReady[196]} && !${alt_Timer_196}) {
				/call e3_Cast ${swarmTarget} 196 alt
			} else /if (!${Me.AltAbilityReady[196]} || ${alt_Timer_196}) {
				/if (${Verbosity}) /docommand ${ChatToggle} Call of Xuzl is not ready.
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
		/if (${Verbosity}) /docommand ${ChatToggle} Using Quick Burns...
		/varset use_QBurn TRUE
	}

/if (${Debug}) /echo <== event_QuickBurn -|
/RETURN



|----------------------------------------------------------------------------------------|
|- QuickBurn's MainLoop hook, calls QuickBurns in succession until your target dies.	-|
|----------------------------------------------------------------------------------------|
SUB QuickBurns
| /varset Debug 1
/if (${Debug}) /echo |- QuickBurn ==>

	/if (${QBurn_Counter} <= ${QuickBurns.Size}) {
		/if (${Select[${QuickBurns[${QBurn_Counter}].Arg[2,/].Left[3]},GEM,Gem,gem]}) {
			/call e3_Cast ${AssistTarget} "${QuickBurns[${QBurn_Counter}].Arg[1,/]}" ${QuickBurns[${QBurn_Counter}].Arg[2,/]} ${Spell[${QuickBurns[${QBurn_Counter}].Arg[1,/]}].Recast}s
			/varcalc QBurn_Counter ${QBurn_Counter} + 1
		} else /if (${Select[${QuickBurns[${QBurn_Counter}].Arg[2,/]},ITEM,Item,item]}) {
			/call e3_Cast ${AssistTarget} "${QuickBurns[${QBurn_Counter}].Arg[1,/]}" item
			/varcalc QBurn_Counter ${QBurn_Counter} + 1
		} else /if (${Select[${QuickBurns[${QBurn_Counter}].Arg[2,/]},ALT,Alt,alt]}) {
			/call e3_Cast ${AssistTarget} "${QuickBurns[${QBurn_Counter}].Arg[1,/]}" alt
			/varcalc QBurn_Counter ${QBurn_Counter} + 1
		} else /if (${Select[${QuickBurns[${QBurn_Counter}].Arg[2,/]},DISC,Disc,disc]}) {
			/if (${Window[CombatAbilityWnd].Child[CAW_CombatEffectLabel].Text.Equal[No Effect]}) { 
				/call e3_Cast ${AssistTarget} "${QuickBurns[${QBurn_Counter}].Arg[1,/]}" disc
				/varcalc QBurn_Counter ${QBurn_Counter} + 1
			}
		}
	} else /if (${QBurn_Counter} > ${QuickBurns.Size}) {
		/varset QBurn_Counter 1
		/varset use_QBurn FALSE
	}
	/varset combatTimer ${combatDelay}

/if (${Debug}) /echo <== QuickBurn -|
| /varset Debug 0
/RETURN



|----------------------------------------------------|
|- Engages FullBurns listed in the Character_Ini.	-|
|----------------------------------------------------|
#event FullBurn "<#1#> Full Burn"
SUB event_FullBurn(line, ChatSender)
/if (${SpawnCount[pc ${ChatSender}]} && ${Defined[use_FBurn]}) {
	/if (${Verbosity}) /docommand ${ChatToggle} Using Full Burns...
	/varset use_FBurn TRUE
}
/RETURN



|----------------------------------------------------------------------------------------------------|
|- FullBurn's MainLoop hook, calls FullBurns, and QuickBurns, in succession until your target dies.	-|
|----------------------------------------------------------------------------------------------------|
SUB FullBurns
/call Background_Events
/if (${FBurn_Counter} <= ${FullBurns.Size}) {
	/if (${Select[${FullBurns[${FBurn_Counter}].Arg[2,/].Left[3]},GEM,Gem,gem]}) {
		/call e3_Cast ${AssistTarget} "${FullBurns[${FBurn_Counter}].Arg[1,/]}" ${FullBurns[${FBurn_Counter}].Arg[2,/]} 5s
		/varcalc FBurn_Counter ${FBurn_Counter} + 1
	} else /if (${Select[${FullBurns[${FBurn_Counter}].Arg[2,/]},ITEM,Item,item]}) {
		/call e3_Cast ${AssistTarget} "${FullBurns[${FBurn_Counter}].Arg[1,/]}" item ${FullBurns[${FBurn_Counter}].Arg[3,/]}
		/varcalc FBurn_Counter ${FBurn_Counter} + 1
	} else /if (${Select[${FullBurns[${FBurn_Counter}].Arg[2,/]},ALT,Alt,alt]}) {
		/if (!${alt_Timer_${Me.AltAbility[${FullBurns[${FBurn_Counter}].Arg[1,/]}].ID}}) /call e3_Cast ${AssistTarget} "${FullBurns[${FBurn_Counter}].Arg[1,/]}" alt
		/varcalc FBurn_Counter ${FBurn_Counter} + 1
	} else /if (${Select[${FullBurns[${FBurn_Counter}].Arg[2,/]},DISC,Disc,disc]}) {
		/if (${Window[CombatAbilityWnd].Child[CAW_CombatEffectLabel].Text.Equal[No Effect]} && !${disc_Timer_${QuickBurns[${QBurn_Counter}].Arg[1,/].Replace[ ,]}}) { 
			/call useDisc "${FullBurns[${FBurn_Counter}].Arg[1,/]}"
			/delay 10
			/varcalc FBurn_Counter ${FBurn_Counter} + 1
		}
	}
} else /if (${FBurn_Counter} > ${FullBurns.Size}) {
	/varset FBurn_Counter 1
	/varset use_FBurn FALSE
	/if (${Defined[use_QBurn]}) /varset use_QBurn TRUE
}
/varset combatTimer ${combatDelay}
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



SUB setup_Assists
/if (${Debug}) /echo |- setup_Assists ==>

	/declare i int local
|-------------------------------------------------------------------------------------------------------------------------Declare Outer Variables.
	
	/declare Toggle-PvP bool outer FALSE
	/declare Assisting bool outer FALSE
	/declare AssistTarget int outer 0
	/declare AllowControl bool outer FALSE
	
|-----------------------------------------------------------------------------------------------------------------------Import Macro Ini Settings.
	/call IniToVar ${Macro_Ini},Assists,AssistBreakDistance AssistBreakDistance int outer
	/call INItoVar ${Macro_Ini},Assists,Allowed_TargetTypes AcceptableTargetTypes string outer
	/call INItoVar ${Macro_Ini},Assists,AutoAssist(On/Off) auto_AssistOn bool outer
	/call INItoVar ${Macro_Ini},Assists,AE_Radius AE_Radius int outer
	/call INItoVar ${Macro_Ini},Assists,EngagePct EngagePct int outer
	
	|--------------------------------------------------------------------------------------Validate EngagePct.
	/if (!${Defined[EngagePct]}) {
		/if (${Debug}) /echo ERROR: @[EngagePct] - EngagePct is invalid, defaulting to 98%
		/declare EngagePct int outer 98
	} else /if (${Defined[EngagePct]} && !${EngagePct}) {
		/if (${Debug}) /echo ERROR: @[EngagePct] - EngagePct is invalid, defaulting to 98%
		/varset EngagePct 98
	}
	
|						|
	

|-------------------------------------------------------------------------------------------------------------------Import Character Ini Settings.

|------------------------------------------------------------------------------------------------------------------------------------[Assists].
	/if (${Ini[${Character_Ini},Assists].Length}) {
		/call IniToVar ${Character_Ini},Assists,AssistType(Melee/Ranged) AssistType string outer
		
		/if (${AssistType.Equal[Melee]}) {
			/call IniToVar ${Character_Ini},Assists,Melee_StickPoint AssistStickPoint string outer
			/call IniToVar ${Character_Ini},Assists,Melee_StickDistance AssistStickDistance string outer
		}
	|-----------------------------If AssistStickPoint, and AssistStickDistance are defined, turn AssistStick on.
		/if (${Defined[AssistStickPoint]} && ${Defined[AssistStickDistance]}) {
			/declare AssistStick_On bool outer TRUE
			/declare AssistStickDistanceSetting int outer
			/if (${AssistStickDistance.NotEqual[MaxMelee]}) /varset AssistStickDistanceSetting ${AssistStickDistance}
		}
	}	
	
	
|						|
	
|----------------------------------------Validate abilities in MyAbilities array
	/if (${MyAbilities.Size}) {
		/for i 1 to ${MyAbilities.Size}
			/if (!${MyAbilities[${i}].Arg[2,/].Length}) {
				/if (!${Me.Ability[${MyAbilities[${i}]}]}) {
					/echo ERROR: @[MyAbilities#${i}] - I do not have [${MyAbilities[${i}]}] listed on my abilities tab. Removing it from MyAbilities.
					/varset MyAbilities[${i}] BAD_ENTRY
				} else {
					/varset MyAbilities[${i}] ${MyAbilities[${i}]}/ability
				}
			} else /if (${MyAbilities[${i}].Arg[2,/].Equal[ability]}) {
				/if (!${Me.Ability[${MyAbilities[${i}].Arg[1,/]}]}) {
					/echo ERROR: @[MyAbilities#${i}] - I do not have [${MyAbilities[${i}].Arg[1,/]}] listed on my abilities tab. Removing it from MyAbilities.
					/varset MyAbilities[${i}] BAD_ENTRY
				}
			}
			/if (${MyAbilities[${i}].Arg[2,/].Equal[alt]} || ${MyAbilities[${i}].Arg[2,/].Equal[aa]}) {
				/if (!${Me.AltAbility[${MyAbilities[${i}].Arg[1,/]}].ID}) {
					/echo ERROR: @[MyAbilities#${i}] - I do not have any ranks spent in [${MyAbilities[${i}]}]. Removing it from MyAbilities.
					/varset MyAbilities[${i}] BAD_ENTRY
				}
			} else /if (${MyAbilities[${i}].Arg[2,/].Equal[disc]} || ${MyAbilities[${i}].Arg[2,/].Equal[discipline]}) {
				/if (!${Me.CombatAbility[${MyAbilities[${i}].Arg[1,/]}]}) {
					/echo ERROR: @[MyAbilities#${i}] - I do not have [${MyAbilities[${i}].Arg[1,/]}] on my Combat Abilities page. Removing it from MyAbilities.
					/varset MyAbilities[${i}] BAD_ENTRY
				}
			}
		/next i
		
		/call RemoveArrayElement MyAbilities BAD_ENTRY
	}
	
	|-------------------------------------------------------------------------------------------------------------------------------------------[Nukes].
	/if (${Ini[${Character_Ini},Nukes].Length}) {
		/call INItoArray ${Character_Ini},Nukes,Nuke# MyNukes
		
	|------------------------------------Validate nukes in MyNukes array
		/if (${MyNukes.Size}) {
			/for i 1 to ${MyNukes.Size}
				/if (${MyNukes[${i}].Arg[2,/].Left[3].Equal[gem]}) {
					/if (!${Int[${Me.Book[${MyNukes[${i}].Arg[1,/]}]}]}) {
						/echo ERROR: @[MyNukes#${i}] - I do not have [${MyNukes[${i}]}] in my spellbook. Removing it from MyNukes.
						/varset MyNukes[${i}] BAD_ENTRY
					}
				} else /if (${MyNukes[${i}].Arg[2,/].Equal[item]}) {
					/if (!${FindItemCount[${MyNukes[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[MyNukes#${i}] - I do not have [${MyNukes[${i}].Arg[1,/]}] in my inventory. Removing it from MyNukes.
						/varset MyNukes[${i}] BAD_ENTRY
					}
				} else /if (${Select[${MyNukes[${i}].Arg[2,/]},alt,aa]}) {
					/if (!${Me.AltAbility[${MyNukes[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[MyNukes#${i}] - I do not have any ranks in [${MyNukes[${i}].Arg[1,/]}]. Removing it from MyNukes.
						/varset MyNukes[${i}] BAD_ENTRY
					}
				}
			/next i
			
			/call RemoveArrayElement MyNukes BAD_ENTRY
		
		|--------------------------------------Declare nuking variables
			/if (${MyNukes.Size}) {
				/declare do_Nukes bool outer TRUE
				/declare NukeTimer timer outer 0
			}
		}
	}
	
|---------------------------------------------------------------------------------------------------------------------------------------------[DoTs].
	/if (${Ini[${Character_Ini},DoTs].Length}) {
		/call INItoArray ${Character_Ini},DoTs,DoT_OnAssist# Assist_DoTs
		
	|------------------------------------Validate DoTs in Assist_DoTs array
		/if (${Assist_DoTs.Size}) {
			/for i 1 to ${Assist_DoTs.Size}
				/if (${Assist_DoTs[${i}].Arg[2,/].Left[3].Equal[gem]}) {
					/if (!${Int[${Me.Book[${Assist_DoTs[${i}].Arg[1,/]}]}]}) {
						/echo ERROR: @[Assist_DoTs#${i}] - I do not have [${Assist_DoTs[${i}]}] in my spellbook. Removing it from Assist_DoTs.
						/varset Assist_DoTs[${i}] BAD_ENTRY
					}
				} else /if (${Assist_DoTs[${i}].Arg[2,/].Equal[item]}) {
					/if (!${FindItemCount[${Assist_DoTs[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[Assist_DoTs#${i}] - I do not have [${Assist_DoTs[${i}].Arg[1,/]}] in my inventory. Removing it from Assist_DoTs.
						/varset Assist_DoTs[${i}] BAD_ENTRY
					}
				} else /if (${Select[${Assist_DoTs[${i}].Arg[2,/]},alt,aa]}) {
					/if (!${Me.AltAbility[${Assist_DoTs[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[Assist_DoTs#${i}] - I do not have any ranks in [${Assist_DoTs[${i}].Arg[1,/]}]. Removing it from Assist_DoTs.
						/varset Assist_DoTs[${i}] BAD_ENTRY
					}
				}
			/next i
			
			/call RemoveArrayElement Assist_DoTs BAD_ENTRY
			
			/if (${Assist_DoTs.Size}) /declare do_AssistDoTs bool outer TRUE
		}

		
		/call INItoArray ${Character_Ini},DoTs,DoT_OnCommand# Command_DoTs
		
	|------------------------------------Validate DoTs in Command_DoTs array
		/if (${Command_DoTs.Size}) {
			/for i 1 to ${Command_DoTs.Size}
				/if (${Command_DoTs[${i}].Arg[2,/].Left[3].Equal[gem]}) {
					/if (!${Int[${Me.Book[${Command_DoTs[${i}].Arg[1,/]}]}]}) {
						/echo ERROR: @[Command_DoTs#${i}] - I do not have [${Command_DoTs[${i}]}] in my spellbook. Removing it from Command_DoTs.
						/varset Command_DoTs[${i}] BAD_ENTRY
					}
				} else /if (${Command_DoTs[${i}].Arg[2,/].Equal[item]}) {
					/if (!${FindItemCount[${Command_DoTs[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[Command_DoTs#${i}] - I do not have [${Command_DoTs[${i}].Arg[1,/]}] in my inventory. Removing it from Command_DoTs.
						/varset Command_DoTs[${i}] BAD_ENTRY
					}
				} else /if (${Select[${Command_DoTs[${i}].Arg[2,/]},alt,aa]}) {
					/if (!${Me.AltAbility[${Command_DoTs[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[Command_DoTs#${i}] - I do not have any ranks in [${Command_DoTs[${i}].Arg[1,/]}]. Removing it from Command_DoTs.
						/varset Command_DoTs[${i}] BAD_ENTRY
					}
				}
			/next i
			
			/call RemoveArrayElement Command_DoTs BAD_ENTRY
			
			/if (${Command_DoTs.Size}) /declare do_CommandDoTs bool outer TRUE
		}
	}
	
|-----------------------------------------------------------------------------------------------------------------------------------------[Debuffs].
	/if (${Ini[${Character_Ini},Debuffs].Length}) {
		/call INItoArray ${Character_Ini},Debuffs,Debuff_OnAssist# Assist_Debuffs
	
		|------------------------------------Validate DoTs in Assist_Debuffs array
		/if (${Assist_Debuffs.Size}) {
			/for i 1 to ${Assist_Debuffs.Size}
				/if (${Assist_Debuffs[${i}].Arg[2,/].Left[3].Equal[gem]}) {
					/if (!${Int[${Me.Book[${Assist_Debuffs[${i}].Arg[1,/]}]}]}) {
						/echo ERROR: @[Assist_Debuffs#${i}] - I do not have [${Assist_Debuffs[${i}]}] in my spellbook. Removing it from Assist_Debuffs.
						/varset Assist_Debuffs[${i}] BAD_ENTRY
					}
				} else /if (${Assist_Debuffs[${i}].Arg[2,/].Equal[item]}) {
					/if (!${FindItemCount[${Assist_Debuffs[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[Assist_Debuffs#${i}] - I do not have [${Assist_Debuffs[${i}].Arg[1,/]}] in my inventory. Removing it from Assist_Debuffs.
						/varset Assist_Debuffs[${i}] BAD_ENTRY
					}
				} else /if (${Select[${Assist_Debuffs[${i}].Arg[2,/]},alt,aa]}) {
					/if (!${Me.AltAbility[${Assist_Debuffs[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[Assist_Debuffs#${i}] - I do not have any ranks in [${Assist_Debuffs[${i}].Arg[1,/]}]. Removing it from Assist_Debuffs.
						/varset Assist_Debuffs[${i}] BAD_ENTRY
					}
				}
			/next i
			
			/call RemoveArrayElement Assist_Debuffs BAD_ENTRY
			
			/if (${Assist_Debuffs.Size}) /declare do_AssistDebuffs bool outer TRUE
		}
		
		/call INItoArray ${Character_Ini},Debuffs,Debuff_OnCommand# Command_Debuffs
		
	|------------------------------------Validate DoTs in Command_Debuffs array
		/if (${Command_Debuffs.Size}) {
			/for i 1 to ${Command_Debuffs.Size}
				/if (${Command_Debuffs[${i}].Arg[2,/].Left[3].Equal[gem]}) {
					/if (!${Int[${Me.Book[${Command_Debuffs[${i}].Arg[1,/]}]}]}) {
						/echo ERROR: @[Command_Debuffs#${i}] - I do not have [${Command_Debuffs[${i}]}] in my spellbook. Removing it from Command_Debuffs.
						/varset Command_Debuffs[${i}] BAD_ENTRY
					}
				} else /if (${Command_Debuffs[${i}].Arg[2,/].Equal[item]}) {
					/if (!${FindItemCount[${Command_Debuffs[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[Command_Debuffs#${i}] - I do not have [${Command_Debuffs[${i}].Arg[1,/]}] in my inventory. Removing it from Command_Debuffs.
						/varset Command_Debuffs[${i}] BAD_ENTRY
					}
				} else /if (${Select[${Command_Debuffs[${i}].Arg[2,/]},alt,aa]}) {
					/if (!${Me.AltAbility[${Command_Debuffs[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[Command_Debuffs#${i}] - I do not have any ranks in [${Command_Debuffs[${i}].Arg[1,/]}]. Removing it from Command_Debuffs.
						/varset Command_Debuffs[${i}] BAD_ENTRY
					}
				}
			/next i
			
			/call RemoveArrayElement Command_Debuffs BAD_ENTRY
			
			/if (${Command_Debuffs.Size}) /declare do_CommandDebuffs bool outer TRUE
		}
	}

	|--------------------------------------------------------------------------------------------------------------------------------------------[Rain].
	/if (${Ini[${Character_Ini},Rain,Rain#1].Length}) {
		/call INItoArray ${Character_Ini},Rain,Rain# Rain_Spells
		
	|---------------------------------------------------------------Validate Rain_Spells array.
		/if (${Rain_Spells.Size}) {
			/for i 1 to ${Rain_Spells.Size}
				/if (${Rain_Spells[${i}].Arg[2,/].Left[3].Equal[gem]}) {
					/if (!${Int[${Me.Book[${Rain_Spells[${i}].Arg[1,/]}]}]}) {
						/echo ERROR: @[Rain_Spells#${i}] - I do not have [${Rain_Spells[${i}]}] in my spellbook. Removing it from Rain_Spells.
						/varset Rain_Spells[${i}] BAD_ENTRY
					}
				} else /if (${Rain_Spells[${i}].Arg[2,/].Equal[item]}) {
					/if (!${FindItemCount[${Rain_Spells[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[Rain_Spells#${i}] - I do not have [${Rain_Spells[${i}].Arg[1,/]}] in my inventory. Removing it from Rain_Spells.
						/varset Rain_Spells[${i}] BAD_ENTRY
					}
				} else /if (${Select[${Rain_Spells[${i}].Arg[2,/]},alt,aa]}) {
					/if (!${Me.AltAbility[${Rain_Spells[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[Rain_Spells#${i}] - I do not have any ranks in [${Rain_Spells[${i}].Arg[1,/]}]. Removing it from Rain_Spells.
						/varset Rain_Spells[${i}] BAD_ENTRY
					}
				}
			/next i
			
			/call RemoveArrayElement Rain_Spells BAD_ENTRY
			
			/if (${Rain_Spells.Size}) {
				/declare do_Rain bool outer FALSE
				/declare RainCounter int outer 1
			}
		}
	}
	
	|--------------------------------------------------------------------------------------------------------------------------------------------[Burn].
	/if (${Ini[${Character_Ini},Burn].Length}) {
		
	|----------------------------------------------------------------------Quick Burns
		/call IniToArray ${Character_Ini},Burn,QuickBurn# QuickBurns
		/if (${QuickBurns.Size}) {
			/for i 1 to ${QuickBurns.Size}
				/if (${QuickBurns[${i}].Arg[2,/].Left[3].Equal[gem]}) {
					/if (!${Int[${Me.Book[${QuickBurns[${i}].Arg[1,/]}]}]}) {
						/echo ERROR: @[QuickBurns#${i}] - I do not have [${QuickBurns[${i}]}] in my spellbook. Removing it from QuickBurns.
						/varset QuickBurns[${i}] BAD_ENTRY
					}
				} else /if (${QuickBurns[${i}].Arg[2,/].Equal[item]}) {
					/if (!${FindItemCount[${QuickBurns[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[QuickBurns#${i}] - I do not have [${QuickBurns[${i}].Arg[1,/]}] in my inventory. Removing it from QuickBurns.
						/varset QuickBurns[${i}] BAD_ENTRY
					}
				} else /if (${QuickBurns[${i}].Arg[2,/].Equal[alt]} || ${QuickBurns[${i}].Arg[2,/].Equal[aa]}) {
					/if (!${Me.AltAbility[${QuickBurns[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[QuickBurns#${i}] - I do not have any ranks in [${QuickBurns[${i}].Arg[1,/]}]. Removing it from QuickBurns.
						/varset QuickBurns[${i}] BAD_ENTRY
					}
				} else /if (${QuickBurns[${i}].Arg[2,/].Equal[disc]} || ${QuickBurns[${i}].Arg[2,/].Equal[discipline]}) {
					/if (!${Me.CombatAbility[${QuickBurns[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[QuickBurns#${i}] - I do not have [${QuickBurns[${i}].Arg[1,/]}] listed on my Combat Abilities page. Removing it from QuickBurns.
						/varset QuickBurns[${i}] BAD_ENTRY
					}
				}
			/next i
			
			/call RemoveArrayElement QuickBurns BAD_ENTRY
			
			/if (${Defined[QuickBurns]}) {
				/declare use_QBurn bool outer FALSE
				/declare QBurn_Counter int outer 1
			}
		}
		
	|-----------------------------------------------------------------------Full Burns
		/call IniToArray ${Character_Ini},Burn,FullBurn# FullBurns
			/if (${FullBurns.Size}) {
			/for i 1 to ${FullBurns.Size}
				/if (${FullBurns[${i}].Arg[2,/].Left[3].Equal[gem]}) {
					/if (!${Int[${Me.Book[${FullBurns[${i}].Arg[1,/]}]}]}) {
						/echo ERROR: @[FullBurns#${i}] - I do not have [${FullBurns[${i}]}] in my spellbook. Removing it from FullBurns.
						/varset FullBurns[${i}] BAD_ENTRY
					}
				} else /if (${FullBurns[${i}].Arg[2,/].Equal[item]}) {
					/if (!${FindItemCount[${FullBurns[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[FullBurns#${i}] - I do not have [${FullBurns[${i}].Arg[1,/]}] in my inventory. Removing it from FullBurns.
						/varset FullBurns[${i}] BAD_ENTRY
					}
				} else /if (${FullBurns[${i}].Arg[2,/].Equal[alt]} || ${FullBurns[${i}].Arg[2,/].Equal[aa]}) {
					/if (!${Me.AltAbility[${FullBurns[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[FullBurns#${i}] - I do not have any ranks in [${FullBurns[${i}].Arg[1,/]}]. Removing it from FullBurns.
						/varset FullBurns[${i}] BAD_ENTRY
					}
				} else /if (${FullBurns[${i}].Arg[2,/].Equal[disc]} || ${FullBurns[${i}].Arg[2,/].Equal[discipline]}) {
					/if (!${Me.CombatAbility[${FullBurns[${i}].Arg[1,/]}]}) {
						/echo ERROR: @[FullBurns#${i}] - I do not have [${FullBurns[${i}].Arg[1,/]}] listed on my Combat Abilities page. Removing it from FullBurns.
						/varset FullBurns[${i}] BAD_ENTRY
					}
				}
			/next i
			
			/call RemoveArrayElement FullBurns BAD_ENTRY
			
			/if (${Defined[FullBurn]}) {
				/declare use_FBurn bool outer FALSE
				/declare FBurn_Counter int outer 1
			}
		}
	}
	
	/if (${Defined[AssistType]} || ${MyAbilities.Size} || ${MyNukes.Size} || ${Assist_DoTs.Size} || ${Command_DoTs.Size} || ${Assist_Debuffs.Size} || ${Command_Debuffs.Size} || ${Rain_Spells.Size} || ${QuickBurns.Size} || ${FullBurns.Size}) /declare AssistsOn bool outer TRUE

/if (${Debug}) /echo <== setup_Assists -|
/RETURN