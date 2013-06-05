|--------------------------------------------------------------------|
|- e3_Macro_Assists.inc	( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ										-|
|--------------------------------------------------------------------|
|- Contains assisting related functions for e3.mac.					-|
|--------------------------------------------------------------------|



SUB e3_AssistFunctions

/if (${Debug}) /echo |- e3_AssistFunctions ==>

	| #1 Check Asssit status.
	/call check_AssistStatus

	| #2 QuickBurns.
	| /if (${use_QBurn}) /call QuickBurns
	
	| #3 FullBurns.	
	| /if (${use_FBurn}) /call FullBurns
	
	| #4 Combat Abilities.	
	/if (${MyAbilities.Size}) /call CombatAbilities
	
	
	| #5 Debuffs on assist.
	| /if (${Assist_Debuffs.Size}) /call Debuffs_OnAssist
	/if (!${ActionTaken}) {
	
		| #6 Dots on assist.
		| /if (${Assist_DoTs.Size}) /call DoTs_OnAssist
		/if (!${ActionTaken}) {
		
			| #7 Nukes.
			/if (${MyNukes.Size} && !${NukeTimer}) /call Nukes
		}
	}

/if (${Debug}) /echo <== e3_AssistFunctions -|

/RETURN



|--------------------------------------------------------------------------------|
|- Engages assist functions on ChatSender, and bots who are in the same zone.	-|
|--------------------------------------------------------------------------------|
|- Use, 'Assist on ${Target.ID}' in /tell, /group, or /bc.						-|
|--------------------------------------------------------------------------------|
#event Assist "<#1#> Assist on #2#"
#event Assist "#1# tell your party, 'Assist on #2#'"
#event Assist "#1# tells the group, 'Assist on #2#'"
#event Assist "#1# tells you, 'Assist on #2#'"
SUB event_Assist(line, ChatSender, int MobID)
/if (${Debug}) /echo |- event_Assist ==>
	
	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
		|    |--------Included Bot-----------|
		/if (!${Bool[${Macro.Return.Arg[3,_]}]}) {
			/if (!${Defined[PauseTarget]}) /declare PauseTarget int outer ${MobID}
		} else {
	
			/if (${Defined[PauseTarget]}) /deletevar PauseTarget
		
			| Check that you're not assisting on a corpse, pet or pc
			/if (${Spawn[id ${MobID}].Type.Equal[Corpse]} || !${Select[${Spawn[id ${MobID}].Type},${AcceptableTargetTypes}]}) {
				/if (${Toggle-PvP}) /goto :assistAnyway
				/if (${Debug}) /echo |- event_EngageAssist -| [${Spawn[id ${MobID}].CleanName}] is not an acceptable TargetType.
			} else {
		
				:assistAnyway
				
				| Check target distance.
				/if (${Spawn[${MobID}].Distance} > ${AssistBreakDistance}) {
					/docommand ${ChatToggle} ${Spawn[${MobID}].CleanName} is too far away.
				} else {
				
					| Wait to catch up to group, before engage.
					/if (${Following} && ${Spawn[${FollowTarget}].Distance} > ${LeashLength} && ${Me.Moving}) {
						/declare Assist_Moving_Timer timer local 100
						:movetoAssist_Loop
						/call Background_Events
						/if (${Spawn[${FollowTarget}].Distance} > ${LeashLength} && ${Assist_Moving_Timer}) /goto :movetoAssist_Loop
					}

					| Disengage follow.
					/if (${Stick.Active}) /squelch /stick off
					/if (${NetAdvPath.State}) /squelch /netfollow pause
					
					| Set Assist Variables.
					/varset Assisting TRUE
					/varset AssistTarget ${MobID}
					
					| If I am the assist caller, or I am feigning, don't allow macro control.
					/if (${ChatSender.Equal[${Me.CleanName}]} || ${Me.Feigning}) {
						/varset AllowControl FALSE
					} else {
						/varset AllowControl TRUE
					}
					
					| If the macro is controlling me, engage.
					/if (${AllowControl}) {

						| Output to user
						/if (${Verbosity}) /docommand ${ChatToggle} Attacking - ${AssistTarget} - ${Spawn[${AssistTarget}].CleanName}
					
						| Target AssistTarget.
						/if (${Target.ID} != ${AssistTarget}) /call TrueTarget ${AssistTarget}
						
						| Melee / Ranged assist
						/if (${AssistType.Equal[melee]}) {
						
							/attack on
							/if (${Int[${AssistDistanceSetting}]}) {
								/varset AssistStickDistance ${AssistDistanceSetting}
							} else {
								/if (${AssistDistanceSetting.Equal[MaxMelee]} || ${AssistDistanceSetting.Equal[auto]}) /call Get_AssistStickDistance
							}

							/call CreateTimer getCloserTimer 70
							/call StickToAssistTarget
							
						} else /if (${AssistType.Equal[ranged]}) {
							/autofire on
							/squelch /stick hold moveback ${RangedStickDistance}
						}
						
						| Send in pets.
						/if (${Me.Pet.ID}) {
							/if (${Target.ID} != ${AssistTarget}) /call TrueTarget ${AssistTarget}
							/delay 3
							/if (${Debug}) /echo |- Event_Assist -| Pet Check: Sending pet in
							/pet attack
						}
					}
					
					/varset combatTimer ${combatDelay}
					/if (${Debug}) /echo |- event_EngageAssist -| Assist=${Assisting} || AssistTarget=${AssistTarget} || AllowControl=${AllowControl}
				}
			}
		}
	}
	
/if (${Debug}) /echo <== event_Assist -|	
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

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]}) {

		/if (${Assisting} && ${AllowControl} && ${Verbosity}) /docommand ${ChatToggle} Backing off.
		/if (!${Defined[PauseTarget]}) /declare PauseTarget int outer ${AssistTarget}
		/if (${Me.Casting.ID}) /stopcast
		/if (${Me.Pet.ID}) /pet back off
		/if (${Me.Combat}) /attack off
		/if (${Me.AutoFire}) /autofire off
		/if (${Debuff_Targets.Size}) /deletevar Debuff_Targets
		/if (${DoT_Targets.Size}) /deletevar DoT_Targets
		/delay 30 !${Me.Casting.ID} && !${Me.Combat} && !${Me.AutoFire}
		/call AssistOff
	}

/if (${Debug}) /echo <== Event_BackOff -|
/RETURN



|----------------------------------------------------------------|
|- Turns off Assist_Functions, and resets assisting variables.	-|
|----------------------------------------------------------------|
SUB AssistOff
/if (${Debug}) /echo |- AssistOff -| ==>
	
	| Interrupt spells, turn off attack.
	/if (${Me.Casting.ID}) /call Interrupt
	/if (${Me.Combat} && ${Target.Type.Equal[PC]} || ${Me.Combat} && !${Target.ID} || ${Me.Combat} && ${Target.Type.Equal[corpse]}) /attack off
	/if (${Me.AutoFire} && ${Target.Type.Equal[PC]} || ${Me.Combat} && !${Target.ID} || ${Me.Combat} && ${Target.Type.Equal[corpse]}) /autofire off
	
	| Reset assist variables.
	/varset Assisting FALSE
	/varset AssistTarget 0
	/varset AllowControl FALSE
	
	| Reset Nuke variables.
	/if (${Defined[NukeTimer]}) /deletevar NukeTimer
	
	| Check loot, and reacquire follow.
	/if (${Select[${Stick},ON]}) /squelch /stick off
	/if (${Following}) /call AquireFollow
	/doevents YourKill
	
/if (${Debug}) {
	/echo |- AssistOff -| Assisting: [${Assisting}] || AssistTarget: [${AssistTarget}]
	/echo |- AssistOff -| <==
}
/RETURN



|--------------------------------------------------------|
|- Calculates a stick distance to your AssistTarget.	-|
|--------------------------------------------------------|
SUB Get_AssistStickDistance
/if (${Debug}) /echo |- Set_AssistStickDistance ==>

	/varset AssistStickDistance ${Math.Calc[${Spawn[${Target.ID}].MaxRangeTo}*.8].Int}

	| Check MaxRangeTo, make sure it is not out of bounds.
	/if (${AssistStickDistance} > 25) /varset AssistStickDistance 25

/if (${Debug}) {
	/echo |- Set_AssistStickDistance -| AssistStickDistance = [${AssistStickDistance}]
	/echo <== Set_AssistStickDistance -|
}
/RETURN



|----------------------------------------|
|- Engages /stick on an AssistTarget.	-|
|----------------------------------------|
SUB StickToAssistTarget
/if (${Debug}) /echo |- StickToAssistTarget ==>
	
	/if (${AssistStickPoint.Equal[behind]}) {
		
		/if (${Debug}) /echo |- StickToAssistTarget -| Attempting to stick behind [${AssistStickDistance}].
		/squelch /stick moveback snaproll 15 uw
		/delay 10 !${Me.Moving} || ${Target.Distance} < 15
		/squelch /stick hold moveback behind ${AssistStickDistance} uw	
		/delay 10 ${Me.Moving}
		
	} else /if (${AssistStickPoint.Equal[behindonce]}) {
	
		/if (${Debug}) /echo |- StickToAssistTarget -| Attempting to stick behindonce [${AssistStickDistance}].
		/squelch /stick moveback snaproll 15 uw
		/delay 10 !${Me.Moving} || ${Target.Distance} < 20
		/squelch /stick hold moveback behindonce ${AssistStickDistance} uw
		/delay 10 ${Me.Moving}
		
	} else /if (${AssistStickPoint.Equal[pin]}) {
	
		/if (${Debug}) /echo |- StickToAssistTarget -| Attempting to stick pin [${AssistStickDistance}].
		/squelch /stick moveback snaproll 15 uw
		/delay 10 !${Me.Moving} || ${Target.Distance} < 20
		/squelch /stick hold moveback pin ${AssistStickDistance} uw
		/delay 10 ${Me.Moving}
		
	} else /if (${AssistStickPoint.Equal[front]}) {
	
		/if (${Debug}) /echo |- StickToAssistTarget -| Attempting to stick front [${AssistStickDistance}].
		/squelch /stick hold front ${AssistStickDistance} uw
		/delay 10 ${Me.Moving}
		
	} else /if (${AssistStickPoint.Equal[!front]}) {
		
		/if (${Debug}) /echo |- StickToAssistTarget -| Attempting to stick !front [${AssistStickDistance}].
		/squelch /stick moveback snaproll 15 uw
		/delay 10 !${Me.Moving} || ${Target.Distance} < 20
		/squelch stick hold moveback !front ${AssistStickDistance} uw
		/delay 10 ${Me.Moving}
	} else {

		/if (${Debug}) /echo |- StickToAssistTarget -| [${AssistStickPoint}] is not a valid stick point.  Defaulting to stick behind.
		/squelch /stick moveback snaproll 15 uw
		/delay 10 !${Me.Moving} || ${Target.Distance} < 15
		/squelch /stick hold moveback behind ${AssistStickDistance} uw	
		/delay 10 ${Me.Moving}
	}
	
	/declare miscTimer timer local 50
	:waitToStop
	/if (${Debug} && ${spamTimer_waitToStop}) {
		/echo |- StickToAssistTarget -| :waitToStop
		/call createTimer spamTimer_waitToStop 30
	}

	/call Background_Events
	
	/if (${Me.Moving}) {
		/if (${miscTimer}) /goto :waitToStop
	}

/if (${Debug}) /echo <== StickToAssistTarget -|	
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
					/if (!${Me.AutoFire} && ${Target.Distance} > ${AssistStickDistance} || ${Me.Combat} && ${Macro.Return} && !${Select[${AssistStickPoint},front,behindonce]}) /call StickToAssistTarget
				
					/if (!${Me.AutoFire} && !${Me.Combat}) /attack on
					
				| Ranged functions
				} else /if (${AssistType.Equal[Ranged]}) {
				
					| Check Target.
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



|----------------------------------------------------------------|
|- Checks to see if the AssistTarget has died, calls Assistoff.	-|
|----------------------------------------------------------------|
SUB check_AssistTarget
/if (${Debug}) /echo |- check_AssistTarget ==>

	/if (${Spawn[${AssistTarget}].Type.Equal[Corpse]} || ${Select[${Spawn[${AssistTarget}].Type.Equal[Corpse]},NULL]}) /call AssistOff
	/varset combatTimer ${combatDelay}
	
/if (${Debug}) /echo <== check_AssistTarget -|
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



|------------------------------------------------------------|
|- Turns off attack, when a mob you're attacking enrages.	-|
|------------------------------------------------------------|
#event EnrageOn "#1# has become ENRAGED."
SUB Event_EnrageOn(line, RageOn)
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



#EVENT getCloser "Your target is too far away, get closer!"
SUB EVENT_getCloser
/if (${Debug}) /echo |- EVENT_getCloser ==>

	/if (${Assisting} && ${AllowControl} && !${getCloserTimer}) {
		
		/call CreateTimer getCloserTimer 50
		
		| If the spawn has moved, stick back to the target
		/if (${Spawn[id ${AssistTarget}].Distance} <= ${AssistStickDistanceSetting} && ${AssistStickDistanceSetting} > 12) /varcalc AssistStickDistance ${AssistStickDistance} - 2
		/if (${Debug}) /echo |- EVENT_getCloser -| AssistStickDistance has been set to [${AssistStickDistance}]
	}

/if (${Debug}) /echo <== EVENT_getCloser -|
/RETURN



SUB check_AutoAssist
/if (${Debug}) /echo |- check_AutoAssist ==>

	| Check NetBots to see if anyone is in combat.
	/declare i int local
	/for i 1 to ${NetBots.Counts}
	/if (${NetBots[${NetBots.Client.Arg[${i}, ]}].Attacking} || ${Spell[${NetBots[${NetBots.Client.Arg[${i}, ]}].Casting}].SpellType.Equal[Detrimental]}) {
		/if (${NetBots[${NetBots.Client.Arg[${i}, ]}].TargetHP} < ${EngagePct} && ${Select[${Spawn[id ${NetBots[${NetBots.Client.Arg[${i}, ]}].TargetID}].Type},${AcceptableTargetTypes}]} && ${NetBots[${NetBots.Client.Arg[${i}, ]}].TargetID} != ${PauseTarget}) {
			/if (${Debug}) /echo |- check_AutoAssist -| ${NetBots[${NetBots.Client.Arg[${i}, ]}]} is attacking, assisting...
			/call EVENT_Assist PLACEHOLDER ${NetBots.Client.Arg[${i}, ]} ${NetBots[${NetBots.Client.Arg[${i}, ]}].TargetID}
		}
	}
	/if (!${Assisting}) /next i

/if (${Debug}) /echo <== check_AutoAssist -|
/RETURN



|------------------------------------------------|
|- Uses combat abilities, AAs, and disciplines.	-|
|------------------------------------------------|
SUB CombatAbilities
/if (${Debug}) /echo |- CombatAbilities ==>

	/if (${Target.ID} != ${AssistTarget}) {
		/if (${Debug}) /echo |- CombatAbilities -| Abilities are only used on AssistTargets.
	} else {
	
		| Check Target distance.
		/if (${Target.Distance} > ${Spawn[${Target.ID}].MaxRangeTo}) {
			/if (${Debug}) /echo |- CombatAbilities -| I am too far away from [${Spawn[id ${AssistTarget}].CleanName}] to use abilities.
		} else {
		
			/declare abilityName string local
		
			/declare i int local
		
			| For 1 to the size of MyAbilities array, use each ability.
			/for i 1 to ${MyAbilities.Size}
				
				/varset abilityName ${MyAbilities[${i}].Arg[1,/]}
		
				| Use ability.
				/if (${Me.Ability[${abilityName}]}) {
					/if (${Me.AbilityReady[${abilityName}]}) {
				
						| Use Backstab.
						/if (${abilityName.Equal[Backstab]}) {
							/call Attempt_Backstab
							
						| Use Bash.
						} else /if (${abilityName.Equal[Bash]}) {
							/call Attempt_Bash
							
						| Use Taunt.
						} else /if (${abilityName.Equal[Taunt]}) {
							/call Attempt_Taunt
							
						| Use a misc Ability.
						} else {

						| Misc.
							/doability "${abilityName}"
						}
					}	
					
				| Use alternate ability
				} else /if (${Me.AltAbility[${abilityName}]}) {
					/if (!${Me.AltAbilityReady[${abilityName}]} || ${altTimer_${Me.AltAbility[${abilityName}].ID}}) {
						/if (${Debug}) /echo |- CombatAbilities -| Alternate Ability [${abilityName}] is not ready.
					} else {
						/call e3_Cast ${AssistTarget} "${abilityName}" alt
					}
				
				| Use combat ability (discipline)
				} else /if (${Me.CombatAbility[${abilityName}]}) {
					/if (!${Me.CombatAbilityReady[${abilityName}]} || ${discTimer_${abilityName.Replace[ ,]}}) {
						/if (${Debug}) /echo |- CombatAbilities -| Discipline [${abilityName}] is not ready.
					} else {
					
						| Rogue Assassin's Strike.
						/if (${abilityName.Equal[Thief's Vengeance]} || ${abilityName.Equal[Assassin's Strike]} || ${abilityName.Equal[Kyv Strike]} || ${abilityName.Equal[Ancient: Chaos Strike]}) {
							/call Attempt_Backstab
						
						| Warrior Bellow.	
						} else /if (${abilityName.Equal[Provoke]} || ${abilityName.Equal[Bellow]} || ${abilityName.Equal[Berate]} || ${abilityName.Equal[Incite]} || ${abilityName.Equal[Bellow of the Mastruq]} || ${abilityName.Equal[Ancient: Chaos Cry]}) {
							/call Attempt_Bellow
							
						| All other disciplines.
						} else {
							/if (!${disc_Timer_${abilityName.Replace[ ,]}}) /call e3_Cast ${AssistTarget} "${abilityName}" disc
						}
					}
				}
			
			/delay 2
			/next i
		}
	}

/if (${Debug}) /echo <== CombatAbilities -|
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
		/if (${Select[${Me.TargetOfTarget},${Me.CleanName}]}) {
			| /if (${Me.Skill[Taunt]} < Find max skill formula) /goto :tauntAnyway
			/if (${Debug}) /echo |- Attempt_Target -| I currently have aggro on [${Target.CleanName}], skipping taunt.
		} else {
			:tauntAnyway
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
	
	/declare nukeName string local
	/declare minNukeMana int local 0
	/declare gemNumber int local ${DefaultGem}
	
	/declare i int local
	
	/for i 1 to ${MyNukes.Size}
	
		/varset nukeName ${MyNukes[${i}].Arg[1,/]}
	
		| /Reagent Check.
		/if (${MyNukes[${i}].Find[/reagent|]}) {
			/call argueString reagent "${MyNukes[${i}]}"
			/if (${Bool[${Macro.Return}]}) {
				/if (!${FindItemCount[=${Macro.Return}]}) {
					/if (${Debug}) /echo |- Nukes -| I do not have any [${Macro.Return}], skipping Nuke#${i} [${MyNukes[${i}]}].
					/goto :skipNuke
				}
			}
		}
	
		| /Gem| Check.
		/if (${MyNukes[${i}].Find[/gem|]}) {
			/call argueString gem "${MyNukes[${i}]}"
			/varset gemNumber ${Macro.Return}
		}
	
		| /MinMana| Check.
		/if (${MyNukes[${i}].Find[/MinMana|]}) {
			/call argueString MinMana "${MyNukes[${i}]}"
			/varset minNukeMana ${Macro.Return}
		}
		
		| -------------------------------------------------------------------------Cast item
		/if (${FindItemCount[=${nukeName}]}) {
			
			| check item timers
			/if (${FindItem[=${nukeName}].Timer} || ${itemTimer_${FindItem[=${nukeName}].ID}}) {
				/echo [${nukeName}] is not ready! [${If[${FindItem[=${nukeName}].Timer},${FindItem[=${nukeName}].Timer} ticks,${Math.Calc[${itemTimer_${FindItem[=${nukeName}].ID}}/10].Int} sec]}] until this item is available.
			} else {
				/call e3_Cast ${AssistTarget} "${nukeName}" item
			}
			
		| --------------------------------------------------------------------------Cast alt
		} else /if (${Me.AltAbility[${nukeName}]}) {
			
			| check item timers
			/if (!${Me.AltAbilityReady[${nukeName}]} || ${altTimer_${Me.AltAbility[${nukeName}].ID}}) {
				/echo [${nukeName}] wont be ready for another [${altTimer_${Me.AltAbility[${nukeName}].ID}}] seconds!
			} else {
				/call e3_Cast ${AssistTarget} "${nukeName}" alt
			}
		| -------------------------------------------------------------------------Cast Spell
		} else /if (${Me.Book[${nukeName}]}) {
			/if (!${Me.Gem[${nukeName}]}) {
				/call e3_Cast ${AssistTarget} "${nukeName}" gem${gemNumber} ${If[${Me.Gem[${nukeName}]},3s,${If[${Me.Combat} || ${AssistType.Equal[melee]},0s,${Math.Calc[${Spell[${nukeName}].RecastTime} + 3].Int}s]}]}
			} else {
				/call e3_Cast ${AssistTarget} "${nukeName}" gem${gemNumber}
			}
		}	
		
	:skipNuke
	/if (!${ActionTaken}) /next i
	
	| If cast was a success, update NukeTimer
	/if (${castReturn.Equal[CAST_SUCCESS]}) {
	
		| /Delay| Check.
		/if (${MyNukes[${i}].Find[/Delay|]}) {
			/call argueString Delay "${MyNukes[${i}]}"
			/call createTimer NukeTimer ${Macro.Return}s
		}
	}
	
/if (${Debug}) /echo <== Nukes -|
/RETURN

























|----------------------------------------------------------------------------------------|
|- Casts spells listed in the [Debuffs] section of the Character_Ini, on AssistTargets.	-|
|----------------------------------------------------------------------------------------|
SUB Debuffs_OnAssist
/if (${Debug}) /echo |- Debuffs_OnAssist ==>

	/declare castName string local
	/declare castType string local
	/declare minMana int local
	/declare timerName string local
	/declare timerSetting string local
	/declare castAttempt int local 1
	
	/declare i int local
	
	/for i 1 to ${Assist_Debuffs.Size}
	
		/varset castName ${Assist_Debuffs[${i}].Arg[1,/]}
		/varset castType ${Assist_Debuffs[${i}].Arg[2,/]}
		/varset minMana ${Assist_Debuffs[${i}].Arg[3,/]}
		/varset timerName spellTimer_${Assist_Debuffs[${i}].Arg[1,/].Replace[ ,]}
		
		/if (${${timerName}}) {
			/if (${Debug}) /echo |- Debuffs_OnAssist -| recastTimer not yet met.
		} else {
			
			/if (${minMana} && ${Me.PctMana} < ${minMana}) {
				/if (${Debug}) /echo |- Debuffs_OnAssist -| Not enough mana to cast [${castName}].
			} else {
			
				:recast
				/if (${castType.Left[3].Equal[gem]}) {
					
					/if (${Spawn[id ${AssistTarget}].Distance} > ${Spell[${castName}].MyRange}) {
						/if (${Debug}) /echo |- Debuffs_OnAssist -| [${Spawn[id ${AssistTarget}].CleanName}] is out of range.
					} else {
						/call e3_Cast ${AssistTarget} "${castName}" ${castType} ${If[${Me.Gem[${castName}]},3s,9s]}
						/varset timerSetting ${Math.Calc[${Spell[${castName}].Duration}*6-6].Int}s
					}
				} else /if (${castType.Equal[item]}) {
					/if (${itemTimer_${FindItem[=${castName}].ID}}) {
						/if (${Debug}) /echo |- Debuffs_OnAssist -| recastTimer not yet met.
					} else {
						/if (${Spawn[id ${AssistTarget}].Distance} > ${Spell[${FindItem[${castName}].Spell}].MyRange}) {
							/if (${Debug}) /echo |- Debuffs_OnAssist -| [${Spawn[id ${AssistTarget}].CleanName}] is out of range.
						} else {
							/if (${Debug}) /echo |- Debuffs_OnAssist -| Attempting to cast [${castName}]...
							/call e3_Cast ${AssistTarget} "${castName}" ${castType}
							/varset timerSetting ${Math.Calc[${Spell[${FindItem[${castName}].Spell}].Duration}*6-6].Int}s
						}
					}
				} else /if (${castType.Equal[alt]} || ${castType.Equal[aa]}) {
					
					/if (${Spawn[id ${AssistTarget}].Distance} > ${Spell[${Me.AltAbility[${castName}].Spell}].MyRange}) {
						/if (${Debug}) /echo |- Debuffs_OnAssist -| [${Spawn[id ${AssistTarget}].CleanName}] is out of range.
					} else {
						/if (${Debug}) /echo |- Debuffs_OnAssist -| Attempting to cast [${castName}]...
						/call e3_Cast ${AssistTarget} "${castName}" ${castType}
						/varset timerSetting ${Math.Calc[${Spell[${Me.AltAbility[${castName}].Spell}].Duration}*6-6].Int}s
					}
				}
				
			|---------------------------------------------If the spell was resisted.
				/if (${castReturn.Equal[CAST_RESISTED]}) {
					/if (${Target.ID}) /docommand ${ChatToggle} **Warning** ${Target.CleanName} resisted ${If[${castType.Equal[item]},${FindItem[=${castName}].Spell},${castName}]}.
					/varcalc castAttempt ${castAttempt} + 1
					/if (${castAttempt} < 4) /goto :recast

			|----------------------------------------------If the spell did not take hold.
				} else /if (${castReturn.Equal[CAST_NOHOLD]}) {
					/call createTimer ${timerName} 30m
					
			|----------------------------------------------If the target was immune.		
				} else /if (${castReturn.Equal[CAST_IMMUNE]}) {
					/if (${Target.ID}) /docommand ${ChatToggle} **Warning** ${Target.CleanName} is IMMUNE to ${If[${castType.Equal[item]},${FindItem[=${castName}].Spell},${castName}]}.
					/call createTimer ${timerName} 30m
					
			|---------------------------------------------If the cast was successful.		
				} else /if (${castReturn.Equal[CAST_SUCCESS]}) {
					/if (${Target.ID}) /docommand ${ChatToggle} Landed ${If[${castType.Equal[item]},${FindItem[=${castName}].Spell},${castName}]} on ${Target.CleanName}!!.
					/call createTimer ${timerName} ${timerSetting}
				}
				
				| continue here |
				
			}
		}
	/if (!${ActionTaken}) /next i

/if (${Debug}) /echo <== Debuffs_OnAssist -|
/RETURN
























Sub assist_Setup
/if (${Debug}) /echo |- assist_Setup ==>
	
	| Create variables used in Assist scripts.
	/declare Toggle-PvP bool outer FALSE
	/declare Assisting bool outer FALSE
	/declare AssistTarget int outer 0
	/declare AllowControl bool outer FALSE
	/declare AssistStickDistance int outer
	
| ------------------------------------- Import Macro_Ini Settings.
	/call IniToVar "${Macro_Ini},Assists,Max Engage Distance" AssistBreakDistance int outer
	/call INItoVar "${Macro_Ini},Assists,Acceptable Target Types" AcceptableTargetTypes string outer
	/call INItoVar "${Macro_Ini},Assists,Auto-Assist (On/Off)" auto_AssistOn bool outer
	
	| Validate EngagePct.
	/call INItoVar "${Macro_Ini},Assists,Auto-Assist Engage Percent" EngagePct int outer
	/if (!${Defined[EngagePct]}) {
		/if (${Debug}) /echo ERROR: @[EngagePct] - EngagePct is invalid, defaulting to 98%
		/declare EngagePct int outer 98
	} else /if (${Defined[EngagePct]} && !${EngagePct}) {
		/if (${Debug}) /echo ERROR: @[EngagePct] - EngagePct is invalid, defaulting to 98%
		/varset EngagePct 98
	}
	
| ------------------------------------- Import Character_Ini Settings.

	| [Melee/Ranged].
	/if (${Ini[${Character_Ini},Melee/Ranged,Assist Type (Melee/Ranged)].Length}) /call IniToVar "${Character_Ini},Melee/Ranged,Assist Type (Melee/Ranged)" AssistType string outer
	/if (${Ini[${Character_Ini},Melee/Ranged,Melee Stick Point].Length}) /call IniToVar "${Character_Ini},Melee/Ranged,Melee Stick Point" AssistStickPoint string outer
	/if (${Ini[${Character_Ini},Melee/Ranged,Melee Distance].Length}) /call IniToVar "${Character_Ini},Melee/Ranged,Melee Distance" AssistDistanceSetting string outer
	/if (${Ini[${Character_Ini},Melee/Ranged,Ranged Distance].Length}) /call IniToVar "${Character_Ini},Melee/Ranged,Ranged Distance" RangedStickDistance string outer

	| [Melee Abilities].
	/if (${Ini[${Character_Ini},Melee Abilities,Ability#1].Length}) /call INItoArray "${Character_Ini},Melee Abilities,Ability#" MyAbilities

	| [Nukes].
	/if (${Ini[${Character_Ini},Nukes,Nuke#1].Length}) /call INItoArray "${Character_Ini},Nukes,Nuke#" MyNukes
	
	| [DoTs].
	
	|- On Assist
	/if (${Ini[${Character_Ini},DoTs,DoT on Assist#1].Length}) /call INItoArray "${Character_Ini},DoTs,DoT on Assist#" Assist_DoTs

	|- On Command
	/if (${Ini[${Character_Ini},DoTs,DoT on Command#1].Length}) /call INItoArray "${Character_Ini},DoTs,DoT on Command#" Command_DoTs

	| [Debuffs].

	|- On Assist
	/if (${Ini[${Character_Ini},Debuffs,Debuff on Assist#1].Length}) /call INItoArray "${Character_Ini},Debuffs,Debuff on Assist#" Assist_Debuffs

	|- On Command
	/if (${Ini[${Character_Ini},Debuffs,Debuff on Command#1].Length}) /call INItoArray "${Character_Ini},Debuffs,Debuff on Command#" Command_Debuffs
	
	| [Rain].
	/if (${Ini[${Character_Ini},Rain,Rain#1].Length}) /call INItoArray ${Character_Ini},Rain,Rain# rainSpells

	| [QuickBurns].
	/if (${Ini[${Character_Ini},Burn,Quick Burn#1].Length}) /call IniToArray "${Character_Ini},Burn,Quick Burn#" QuickBurns
	
	| [FullBurns].	
	/if (${Ini[${Character_Ini},Burn,Full Burn#1].Length}) /call IniToArray "${Character_Ini},Burn,Full Burn#" FullBurns
	
	| If Assisting variables are defined, turn assists on.
	/if (${Defined[AssistType]} || ${Defined[MyAbilities]} || ${Defined[MyNukes]} || ${Defined[Assist_DoTs]} || ${Defined[Command_DoTs]} || ${Defined[Assist_Debuffs]} || ${Defined[Command_Debuffs]} || ${Defined[Rain_Spells]} || ${Defined[QuickBurns]} || ${Defined[FullBurns]}) /declare AssistsOn bool outer TRUE

/if (${Debug}) /echo <== assist_Setup -|
/return



Sub assist_Background_Events

	/if (${Assisting}) /call check_AssistTarget
	/doevents BackOff
	/doevents EnrageOn
	/doevents EnrageOff
	/doevents getCloser
	
/return

	/doevents DebuffsOff
	/doevents DoTsOff
	/doevents RainOff
	/doevents TauntOff
	/doevents DebuffsOn
	/doevents DoTsOn
	/doevents RainOn
	/doevents TauntOn
	/doevents QuickBurn
	/doevents FullBurn
	/doevents toggle-PvP

SUB assist_MacroSettings
/if (${Debug}) /echo |- assist_MacroSettings ==>

	| e3_Macro_Assist.inc entries.
	/call WriteToIni "${Macro_Ini},Assists,Auto-Assist (On/Off)" On
	/call WriteToIni "${Macro_Ini},Assists,Auto-Assist Engage Percent" 98
	/call WriteToIni "${Macro_Ini},Assists,Max Engage Distance" 250
	/call WriteToIni "${Macro_Ini},Assists,Acceptable Target Types" NPC,Pet
	
/if (${Debug}) /echo <== assist_MacroSettings -|
/RETURN



SUB assist_CharacterSettings
/if (${Debug}) /echo |- assist_CharacterSettings ==>

	| If I'm a melee class, add Assist_Stick entries.
	/if (${Select[${Me.Class.ShortName},BRD,BER,BST,MNK,PAL,RNG,ROG,SHD,WAR]}) {
		/if (!${Ini[${Character_Ini},Melee/Ranged].Length}) {
			/call WriteToIni "${Character_Ini},Melee/Ranged,Assist Type (Melee/Ranged)" Melee
			/call WriteToIni "${Character_Ini},Melee/Ranged,Melee Stick Point" behind
			/call WriteToIni "${Character_Ini},Melee/Ranged,Melee Distance" MaxMelee
			/call WriteToIni "${Character_Ini},Melee/Ranged,Ranged Distance" 100
		}
	}

	| If I'm a melee class, add Melee Abilities entries.
	/if (${Select[${Me.Class.ShortName},BER,BST,MNK,PAL,RNG,ROG,SHD,WAR]}) {
		/if (!${Ini[${Character_Ini},Melee Abilities].Length}) /call WriteToIni "${Character_Ini},Melee Abilities,Ability#1"
	}

	| If I'm a casting class, add Nuke entries.
	/if (${Select[${Me.Class.ShortName},BST,DRU,ENC,MAG,PAL,NEC,RNG,SHD,SHM,WIZ]}) {
		/if (!${Ini[${Character_Ini},Nukes].Length}) /call WriteToIni "${Character_Ini},Nukes,Nuke#1"
	}

	| If I'm a casting class, add DoT entries.
	/if (${Select[${Me.Class.ShortName},BST,DRU,ENC,NEC,RNG,SHD,SHM]}) {
		/if (!${Ini[${Character_Ini},DoTs].Length}) {
			/call WriteToIni "${Character_Ini},DoTs,DoT on Assist#1"
			/call WriteToIni "${Character_Ini},DoTs,DoT on Command#1"
		}
	}

	| If I'm a casting class, add Debuff entries.
	/if (${Select[${Me.Class.ShortName},BST,DRU,ENC,MAG,NEC,SHM]}) {
		/if (!${Ini[${Character_Ini},Debuffs].Length}) {
			/call WriteToIni "${Character_Ini},Debuffs,Debuff on Assist#1"
			/call WriteToIni "${Character_Ini},Debuffs,Debuff on Command#1"
		}
	}

	| If I'm a casting class, add Rain entries.
	/if (${Select[${Me.Class.ShortName},DRU,MAG,SHM,WIZ]}) {
		/if (!${Ini[${Character_Ini},Rain].Length}) /call WriteToIni "${Character_Ini},Rain,Rain#1"
	}

	| Burn entries.
	/if (!${Ini[${Character_Ini},Burn].Length}) {
		/call WriteToIni "${Character_Ini},Burn,Quick Burn#1"
		/call WriteToIni "${Character_Ini},Burn,Full Burn#1"
	}

/if (${Debug}) /echo <== assist_CharacterSettings -|
/RETURN



Sub assist_Aliases
/if (${Debug}) /echo |- assist_Aliases ==>

	/squelch /alias /AssistOn delete
	/squelch /alias /AssistMe delete
	/squelch /alias /BackOff delete
	
	/alias /AssistOn /bc Assist on ${Target.ID}
	/alias /AssistMe /bc Assist on ${Target.ID}
	/alias /BackOff /bc Back off

/if (${Debug}) /echo <== assist_Aliases -|
/return