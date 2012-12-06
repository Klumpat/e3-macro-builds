|------------------------------------------------------------------------|
|- e3_Macro_Pets.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains pet related functions for e3.mac							-|
|------------------------------------------------------------------------|



|----------------------------------------------------------------------------|
|- Pets.incs Mainloop hook.													-|
|----------------------------------------------------------------------------|
|- Checks your pets status. Automatically, resummons, buffs, and shrinks.	-|
|----------------------------------------------------------------------------|
SUB check_PetStatus
/if (${Debug}) /echo |- check_PetStatus ==>

|--------------------------------------------------------------------------------------------------------------------------------Summon a new pet.
	/if (!${Me.Pet.ID}) {
		/call summon_Pet
		/if (${ActionTaken}) /RETURN
	} else {
	
	|------------------------------------------------------------------------------------------------------------------------------Maintain pet buffs.
		/if (${MyPetBuffs.Size}) /call check_PetBuffs
			/if (${ActionTaken}) /RETURN
			
	|-------------------------------------------------------------------------------------------------------------------Check pet size and AutoShrink.
		/if (${Pet_AutoShrink} && ${Me.Pet.Height}>1) /call check_AutoShrink
	}
/if (${Debug}) /echo <== check_PetStatus -|
/RETURN



|------------------------------------------------------------|
|- Summon a new pet.										-|
|------------------------------------------------------------|
|- Looks for reageants, and checks mana, before summoning.	-|
|------------------------------------------------------------|
SUB summon_Pet
/if (${Debug}) /echo |- summon_Pet ==>

|--------------------------------------------If a reageant is listed, and its available, or if no reageant is listed, summon a pet.
	/if (${PetSpell.Arg[2,/].Length} && ${FindItemCount[=${PetSpell.Arg[2,/]}]} || ${Defined[PetSpell]} && !${PetSpell.Arg[2,/].Length}) {
		/if (${Debug}) /echo |- summon_Pet -| Attempting to summon [${PetSpell.Arg[1,/]}].
		/call e3_Cast ${Me.ID} "${PetSpell.Arg[1,/]}" gem${DefaultGem} ${If[${Me.Gem[${Spell[${PetSpell.Arg[1,/]}]}]},25,90]}
		
|--------------------------------------------If I don't have the required reageants to summon a pet, alter, and turn pets off.
	} else /if (${PetSpell.Arg[2,/].Length} && !${FindItemCount[=${PetSpell.Arg[2,/]}]}) {
		/if (${Debug} && ${spamTimer}) {
			/echo I do not have the required reageant to cast [${PetSpell.Arg[1,/]}].
			/varset Pets FALSE
		}	
	}
	
/if (${Debug}) /echo <== summon_Pet -|
/RETURN



|------------------------------------------------------------------------|
|- Rebuff pet.															-|
|------------------------------------------------------------------------|
|- Casts spells, items, and alternate abilities, from MyPetBuffs array.	-|
|------------------------------------------------------------------------|
SUB check_PetBuffs
/if (${Debug}) /echo |- check_PetBuffs ==>

	/declare i int local
	/for i 1 to ${MyPetBuffs.Size}
	
	|----------------------------------------------------------------------------------------------------------------------------------Cast spell.
		/if (!${MyPetBuffs[${i}].Arg[2,/].Length} || ${MyPetBuffs[${i}].Arg[2,/].Left[3].Equal[gem]}) {
		
		|---------Check to see if your pet already has the buff, and that it will stack with your pet's current buffs.
			/if (!${Me.PetBuff[${MyPetBuffs[${i}].Arg[1,/]}]} && ${Spell[${MyPetBuffs[${i}].Arg[1,/]}].StacksPet}) {

			|----------------------------------------------------------------------------Cast PetBuff, and return.	
				/if (${Debug}) /echo |- check_PetBuffs -| Attempting to cast [${MyPetBuffs[${i}].Arg[1,/]}] on [${Spawn[id ${Me.Pet.ID}].CleanName} - ${Me.Pet.ID}] with a retry timer of [].
				/call e3_Cast ${Me.Pet.ID} "${MyPetBuffs[${i}].Arg[1,/]}" Gem${DefaultGem} ${If[${Me.Gem[${Spell[${MyPetBuffs[${i}].Arg[1,/]}]}]},25,90]}
				/RETURN
			}
			
	|----------------------------------------------------------------------------------------------------------------------------------Cast item.
		} else /if (${MyPetBuffs[${i}].Arg[2,/].Equal[item]}) {
			
		|---------Check to see if your pet already has the buff, and that it will stack with your pet's current buffs.	
			/if (${FindItem[${MyPetBuffs[${i}].Arg[1,/]}].ID} && !${Me.PetBuff[${FindItem[${MyPetBuffs[${i}].Arg[1,/]}].Spell}]} && ${Spell[${FindItem[${MyPetBuffs[${i}].Arg[1,/]}].Spell}].StacksPet}) {
			
			|--------------------------------------------------------------------------------Cast PetBuff, and return.
				/call e3_Cast ${Me.Pet.ID} "${MyPetBuffs[${i}].Arg[1,/]}" item
				/RETURN
			}
			
	|-----------------------------------------------------------------------------------------------------------------------------Cast AltAbility.		
		} else /if (${MyPetBuffs[${i}].Arg[2,/].Equal[alt]}) {
			
		|---------Check to see if your pet already has the buff, and that it will stack with your pet's current buffs.	
			/if (!${Me.PetBuff[${Me.AltAbility[${MyPetBuffs[${i}].Arg[1,/]}].Spell}]} && !${alt_Timer_${Me.AltAbility[${MyPetBuffs[${i}].Arg[1,/]}].ID}}) {
				
			|--------------------------------------------------------------------------------Cast PetBuff, and return.		
				/call e3_Cast ${Me.Pet.ID} "${MyPetBuffs[${i}].Arg[1,/]}" alt
				/RETURN
			}
		}
	/next i
	/varset pet_rebuffTimer ${buffCheck_Interval}s
	
/if (${Debug}) /echo <== check_PetBuffs -|
/RETURN



|--------------------------------|
|- Automatically shrinks pets.	-|
|--------------------------------|
SUB check_AutoShrink
/if (${Debug}) /echo |- check_AutoShrink ==>
	
	/if (${FindItemCount[=Symbol of Ancient Summoning]}) {
		/call e3_Cast ${Me.Pet.ID} "Symbol of Ancient Summoning" item
	} else /if (${Me.Book[Tiny Companion]}) {
		/call e3_Cast ${Me.Pet.ID} "Tiny Companion" Gem${DefaultGem} ${If[${Me.Gem[Tiny Companion]},25,90]}
	}

/if (${Debug}) /echo <== check_AutoShrink -|
/RETURN



|----------------------------|
|- Manually shrinks pets.	-|
|----------------------------|
|- Use, /bc Shrink Pets		-|
|----------------------------|
#event shrinkPets "<#*#> Shrink Pets"
SUB event_shrinkPets
/if (${Debug}) /echo |- event_shrinkPets ==>
	
|--------------------------------------------------------------------------------------------------------------------------Attempt to shrink pets.
	/if (${Me.Pet.ID} && ${Me.Pet.Height}>1 && ${Me.Book[Tiny Companion]}) /call e3_Cast ${Me.Pet.ID} "Tiny Companion" Gem${BuffGem} 20s
	
/if (${Debug}) /echo <== event_shrinkPets -|	
/RETURN
#event suspendMinion "<#*#> Suspend Pets"
SUB event_suspendMinion
/if (${Me.Pet.ID}) /docommand ${ChatToggle} Suspending my pet
/if (${Me.AltAbilityReady[Suspended Minion]}) {
	/call e3_Cast ${Me.Pet.ID} "Suspended Minion" alt
	/delay 5s !${Me.Pet.ID}
	/varset currently_Suspended
}
/if (${Me.Pet.ID}) /pet get lost
/if (${Defined[suspend_Pets]}) /varset suspend_Pets TRUE
/RETURN
#event returnMinion "#*#Return Pets#*#"
SUB event_returnMinion
/if (${suspend_Pets}) /docommand ${ChatToggle} Returning my pet
	/if (${currently_Suspended}) {
		/call e3_Cast "Suspended Minion" alt
		/varset currently_Suspended FALSE
	}
	/if (${suspend_Pets}) /varset suspend_Pets FALSE
}
/RETURN



|------------------------------------------------------------|
|- Checks pet health, and attempts to cast PetHeal spells.	-|
|------------------------------------------------------------|
SUB PetHeal
/if (${Debug}) /echo |- PetHeal ==>

|-------------------------------------------------------------------------------------------------------------------------Check Mend Companion AA.
	/if (${Me.AltAbilityReady[Mend Companion]} && ${Me.Pet.PctHPs} < ${Pet_MendPct}) /call e3_Cast ${Me.Pet.ID} "Mend Companion" alt
	
|-------------------------------------------------------------------------------------------------------------Check Pet Health and cast heal spell.
	/if (${Me.Pet.PctHPs} < ${Pet_Heal.Arg[3,/]}) {
		/call e3_Cast ${Me.Pet.ID} "${Pet_Heal.Arg[1,/]}" ${Pet_Heal.Arg[2,/]} ${If[${Me.Gem[${Spell[${Pet_Heal.Arg[1,/]}]}]},25,90]}
	}

/if (${Debug}) /echo <== PetHeal -|
/RETURN



SUB setup_Pets
/if (${Debug}) /echo |- setup_Pets ==>

	/declare i int local
	
/if (${Ini[${Character_Ini},Pets].Length}) {

|-------------------------------------------------------------------------------------------------------------------------------------------Import [Pet Spell].
	/varcalc Validation_Arg ${Validation_Arg} + 1
	:Validate_PetSpell
	/varset Validation_Count 0
	
	/if (${Ini[${Character_Ini},Pets,Pet (Spell/Gem/Reageant)].Length}) /call INItoVar "${Character_Ini},Pets,Pet (Spell/Gem/Reageant)" PetSpell string outer
	
	/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length} || ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${Validation_Arg},-]} != ${Validation_Count}) /varset Validation_On TRUE

|------------------------------------------------------------------------------------------------------------------------------------Validate PetSpell.
	/if (${Validation_On}) {	
	|------------------------------------------------------------------------------------------------------------------------------Missing castType.	
		/if (!${PetSpell.Arg[2,/].Length}) {
			/echo [Pet Spell] - [${PetSpell}] does not have a specified castType.  Attempting to fix...
			
			/if (${Int[${Me.Book[${PetSpell.Arg[1,/]}]}]}) {
				/varset PetSpell ${PetSpell}/gem${If[${Me.Gem[${PetSpell}]},${Me.Gem[${PetSpell}]},${DefaultGem}]}
				/call WriteToIni "${Character_Ini},Pets,Pet (Spell/Gem/Reageant)" "${PetSpell}" 1
				/echo [${PetSpell.Arg[1,/]}] was found in my spellbook.  Setting [${PetSpell.Arg[1,/]}] to [${PetSpell.Arg[2,/]}].
			} else /if (${FindItemCount[${PetSpell.Arg[1,/]}]}) {
				/varset PetSpell ${PetSpell}/item
				/call WriteToIni "${Character_Ini},Pets,Pet (Spell/Gem/Reageant)" "${PetSpell}" 1
				/echo [${PetSpell.Arg[1,/]}] was found in my inventory.  Setting [${PetSpell.Arg[1,/]}] to [${PetSpell.Arg[2,/]}].
			} else {
				/varset PetSpell ${PetSpell}/BAD_ENTRY
				/call WriteToIni "${Character_Ini},Pets,Pet (Spell/Gem/Reageant)" "${PetSpell}" 1
				/echo ERROR: @[Pet Spell] - Could not find a compatible castType for [${PetSpell}].  Flagging [Pet Spell] as a BAD_ENTRY.
			}
		} else {
			
			/if (${PetSpell.Arg[2,/].Find[gem]}) {
				/if (!${Int[${Me.Book[${PetSpell.Arg[1,/]}]}]}) {
					/varset PetSpell ${PetSpell.Arg[1,/]}/BAD_ENTRY
					/call WriteToIni "${Character_Ini},Pets,Pet (Spell/Gem/Reageant)" "${PetSpell}" 1
					/echo ERROR: @[Pet Spell] - [${PetSpell.Arg[1,/]}] was not found in my spellbook.  Flagging [Pet Spell] as a BAD_ENTRY.
				}
			} else /if (${PetSpell.Arg[2,/].Equal[item]}) {
				/if (!${FindItemCount[${PetSpell.Arg[1,/]}]}) {
					/varset PetSpell ${PetSpell.Arg[1,/]}/BAD_ENTRY
					/call WriteToIni "${Character_Ini},Pets,Pet (Spell/Gem/Reageant)" "${PetSpell}" 1
					/echo ERROR: @[Pet Spell] - I do not have [${MyNukes[${i}].Arg[1,/]}] in my inventory.  Flagging [Pet Spell] as a BAD_ENTRY.
				}
			} else /if (${PetSpell.Arg[2,/].Equal[BAD_ENTRY]}) {
				/echo ERROR: @[Pet Spell] - There is a problem with the user entry for [${PetSpell.Arg[1,/]}] in [${Character_Ini}].  Please review this entry, and restart the macro.
			} else {
				/if (${Int[${Me.Book[${PetSpell.Arg[1,/]}]}]}) {
					/varset PetSpell ${PetSpell.Arg[1,/]}/gem${If[${Me.Gem[${PetSpell}]},${Me.Gem[${PetSpell}]},${DefaultGem}]}/${PetSpell.Arg[2,/]}
					/call WriteToIni "${Character_Ini},Pets,Pet (Spell/Gem/Reageant)" "${PetSpell}" 1
					/echo [${PetSpell.Arg[1,/]}] was found in my spellbook.  Setting [${PetSpell.Arg[1,/]}] to [${PetSpell.Arg[2,/]}].
				} else /if (${FindItemCount[${PetSpell.Arg[1,/]}]}) {
					/varset PetSpell ${PetSpell}/item/${PetSpell.Arg[2,/]}
					/call WriteToIni "${Character_Ini},Pets,Pet (Spell/Gem/Reageant)" "${PetSpell}" 1
					/echo [${PetSpell.Arg[1,/]}] was found in my inventory.  Setting [${PetSpell.Arg[1,/]}] to [${PetSpell.Arg[2,/]}].
				} else {
					/varset PetSpell ${PetSpell}/BAD_ENTRY
					/call WriteToIni "${Character_Ini},Pets,Pet (Spell/Gem/Reageant)" "${PetSpell}" 1
					/echo ERROR: @[Pet Spell] - Could not find a compatible castType for [${PetSpell}].  Flagging [Pet Spell] as a BAD_ENTRY.
				}
			}
		}	

		/call rewrite_ValidationKey ${Validation_Count} ${Validation_Arg}
		
		/varset Validation_On FALSE
		/goto :Validate_PetSpell
	}
	
	/if (${Defined[PetSpell]}) {
		/declare Pets bool outer TRUE
		/declare pet_rebuffTimer timer outer
		/if (!${Defined[suspend_Pets]}) /declare suspend_Pets bool global FALSE
		/if (!${Defined[currently_Suspended]}) /declare currently_Suspended bool global FALSE
		/if (${Ini[${Character_Ini},Pets,Pet Auto-Shrink (On/Off)].Length}) /call INItoVar "${Character_Ini},Pets,Pet Auto-Shrink (On/Off)" Pet_AutoShrink bool outer
		
	|---------------------------------------------------------------------------------------------------------------------------------Import Pet_Heal
		/varcalc Validation_Arg ${Validation_Arg} + 1
		:Validate_PetHeal
		/varset Validation_Count 0
		
		/if (${Ini[${Character_Ini},Pets,Pet Heal (Spell/Gem/Pct)].Length}) /call INItoVar "${Character_Ini},Pets,Pet Heal (Spell/Gem/Pct)" Pet_Heal string outer
		
		/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length} || ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${Validation_Arg},-]} != ${Validation_Count}) /varset Validation_On TRUE

		/if (${Validation_On}) {
			/if (${Defined[Pet_Heal]}) {
			
			|------------------------------------------------------------------------------------------------------------------------------Missing castType.	
				/if (!${Pet_Heal.Arg[2,/].Length}) {
					/echo [Pet Heal] - [${Pet_Heal}] does not have a specified castType.  Attempting to fix...
					
					/if (${Int[${Me.Book[${Pet_Heal.Arg[1,/]}]}]}) {
						/varset Pet_Heal ${Pet_Heal}/gem${If[${Me.Gem[${Pet_Heal}},${Me.Gem[${Pet_Heal}]},${DefaultGem}]}
						/call WriteToIni "${Character_Ini},Pets,Pet Heal (Spell/Gem/Pct)" "${Pet_Heal}" 1
						/echo [${Pet_Heal.Arg[1,/]}] was found in my spellbook.  Setting [${Pet_Heal.Arg[1,/]}] to [${Pet_Heal.Arg[2,/]}].
					}
				} else {
					
					/if (${Pet_Heal.Arg[2,/].Find[gem]}) {
						/if (!${Int[${Me.Book[${Pet_Heal.Arg[1,/]}]}]}) {
							/varset Pet_Heal ${Pet_Heal.Arg[1,/]}/BAD_ENTRY
							/call WriteToIni "${Character_Ini},Pets,Pet Heal (Spell/Gem/Pct)" "${Pet_Heal}" 1
							/echo ERROR: @[Pet Heal] - [${Pet_Heal.Arg[1,/]}] was not found in my spellbook.  Flagging [Pet Heal] as a BAD_ENTRY.
						}
					} else /if (${Pet_Heal.Arg[2,/].Equal[BAD_ENTRY]}) {
						/echo ERROR: @[Pet Heal] - There is a problem with the user entry for [${Pet_Heal.Arg[1,/]}] in [${Character_Ini}].  Please review this entry, and restart the macro.
					} else {
						/varset Pet_Heal ${Pet_Heal.Arg[1,/]}/BAD_ENTRY
						/call WriteToIni "${Character_Ini},Pets,Pet Heal (Spell/Gem/Pct)" "${Pet_Heal}" 1
						/echo ERROR: @[Pet Heal] - [${Pet_Heal.Arg[2,/]}] is not an acceptable castType.  Flagging [Pet Heal] as a BAD_ENTRY.
					}
				}
			}
			
			/call rewrite_ValidationKey ${Validation_Count} ${Validation_Arg}
		
			/varset Validation_On FALSE
			/goto :Validate_PetHeal
		}
	
	|---------------------------------------------------------------------------------------------------------------------------------Import Mend Percent
		/call INItoVar "${Character_Ini},Pets,Pet Mend (Pct)" Pet_MendPct int outer
		
	|---------------------------------------------------------------------------------------------------------------------------------Import PetBuffs
		/varcalc Validation_Arg ${Validation_Arg} + 1
		:Validate_PetBuffs
		/varset Validation_Count 0
		
		/if (${Ini[${Character_Ini},Pets,Pet Buff#1].Length}) /call INItoArray "${Character_Ini},Pets,Pet Buff#" MyPetBuffs
		
		/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length} || ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${Validation_Arg},-]} != ${Validation_Count}) /varset Validation_On TRUE

		/if (${Validation_On}) {
			/if (${MyPetBuffs.Size}) {
				
				/for i 1 to ${MyPetSpells.Size}
				
				|------------------------------------------------------------------------------------------------------------------------------Missing castType.	
					/if (!${MyPetBuffs[${i}].Arg[2,/].Length}) {
						/echo [PetBuff#${i}] - [${MyPetBuffs[${i}]}] does not have a specified castType.  Attempting to fix...
						
						/if (${Int[${Me.Book[${MyPetBuffs[${i}]}]}]}) {
							/varset MyPetBuffs[${i}] ${MyPetBuffs[${i}]}/gem${If[${Me.Gem[${MyPetBuffs[${i}]}]},${Me.Gem[${MyPetBuffs[${i}]}]},${DefaultGem}]}
							/call WriteToIni "${Character_Ini},Pets,Pet Buff#${i}" "${MyPetBuffs[${i}]}" 1
							/echo [${MyPetBuffs[${i}].Arg[1,/]}] was found in my spellbook.  Setting [${MyPetBuffs[${i}].Arg[1,/]}] to [${MyPetBuffs[${i}].Arg[2,/]}].
						} else /if (${FindItemCount[${MyPetBuffs[${i}].Arg[1,/]}]}) {
							/varset MyPetBuffs[${i}] ${MyPetBuffs[${i}]}/item
							/call WriteToIni "${Character_Ini},Pets,Pet Buff#${i}" "${MyPetBuffs[${i}]}" 1
							/echo [${MyPetBuffs[${i}].Arg[1,/]}] was found in my inventory.  Setting [${MyPetBuffs[${i}].Arg[1,/]}] to [${MyPetBuffs[${i}].Arg[2,/]}].
						} else /if (${Me.AltAbility[${FullBurns[${i}]}]}) {
							/varset MyPetBuffs[${i}] ${MyPetBuffs[${i}]}/alt
							/call WriteToIni "${Character_Ini},Pets,Pet Buff#${i}" "${MyPetBuffs[${i}]}" 1
							/echo I have ranks spent in [${MyPetBuffs[${i}].Arg[1,/]}].  Setting [${MyPetBuffs[${i}].Arg[1,/]}] to [${MyPetBuffs[${i}].Arg[2,/]}]	
						} else {
							/varset MyPetBuffs[${i}] ${MyPetBuffs[${i}]}/BAD_ENTRY
							/call WriteToIni "${Character_Ini},Pets,Pet Buff#${i}" "${MyPetBuffs[${i}]}" 1
							/echo ERROR: @[Pet Buff#${i}] - Could not find a compatible castType for [${MyPetBuffs[${i}]}].  Flagging [Pet Buff#${i}] as a BAD_ENTRY.
						}
						
					} else {
						/if (${MyPetBuffs[${i}].Find[gem]}) {
							/if (!${Int[${Me.Book[${MyPetBuffs[${i}].Arg[1,/]}]}]}) {
								/varset MyPetBuffs[${i}] ${MyPetBuffs[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Pets,Pet Buff#${i}" "${MyPetBuffs[${i}]}" 1
								/echo ERROR: @[Pet Buff#${i}] - [${MyPetBuffs[${i}].Arg[1,/]}] was not found in my spellbook.  Flagging [Pet Buff#${i}] as a BAD_ENTRY.
							}
						} else /if (${MyPetBuffs[${i}].Arg[2,/].Equal[item]}) {
							/if (!${FindItemCount[${MyPetBuffs[${i}].Arg[1,/]}]}) {
								/varset MyPetBuffs[${i}] ${MyPetBuffs[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Pets,Pet Buff#${i}" "${MyPetBuffs[${i}]}" 1
								/echo ERROR: @[Pet Buff#${i}] - I do not have [${MyPetBuffs[${i}].Arg[1,/]}] in my inventory.  Flagging [Pet Buff#${i}] as a BAD_ENTRY.
							}
						} else /if (${MyPetBuffs[${i}].Arg[2,/].Equal[alt]} || ${MyPetBuffs[${i}].Arg[2,/].Equal[aa]}) {
							/if (!${Me.AltAbility[${MyPetBuffs[${i}].Arg[1,/]}]}) {
								/varset MyPetBuffs[${i}] ${MyPetBuffs[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Pets,Pet Buff#${i}" "${MyPetBuffs[${i}]}" 1
								/echo ERROR: @[Pet Buff#${i}] - I do not have any ranks in [${MyPetBuffs[${i}].Arg[1,/]}].  Flagging [Pet Buff#${i}] as a BAD_ENTRY.
							}
						} else /if (${PetSpell.Arg[2,/].Equal[BAD_ENTRY]}) {
							/echo ERROR: @[Pet Buff#${i}] - There is a problem with the user entry for [${MyPetBuffs[${i}].Arg[1,/]}] in [${Character_Ini}].  Please review this entry, and restart the macro.
						} else {
							/varset MyPetBuffs[${i}] ${/varset MyPetBuffs[${i}].Arg[1,/]}/BAD_ENTRY
							/call WriteToIni "${Character_Ini},Pets,Pet Buff#${i}" "${MyPetBuffs[${i}]}" 1
							/echo ERROR: @[Pet Buff#${i}] - [${MyPetBuffs[${i}].Arg[2,/]}] is not an acceptable castType.  Flagging [Pet Buff#${i}] as a BAD_ENTRY.
						}
					}
				
				/next i
			}
			
			/call rewrite_ValidationKey ${Validation_Count} ${Validation_Arg}
		
			/varset Validation_On FALSE
			/goto :Validate_PetBuffs
		}
	}
}	
	
/if (${Debug}) /echo <== setup_Pets -|
/RETURN