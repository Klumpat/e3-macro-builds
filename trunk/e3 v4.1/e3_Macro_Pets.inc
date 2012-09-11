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
	/if (!${Me.Pet.ID}) /call summon_Pet
		/if (${ActionTaken}) /RETURN
		
|------------------------------------------------------------------------------------------------------------------------------Maintain pet buffs.
	/if (${MyPetBuffs.Size}) /call check_PetBuffs
		/if (${ActionTaken}) /RETURN
		
|-------------------------------------------------------------------------------------------------------------------Check pet size and AutoShrink.
	/if (${Pet_AutoShrink} && ${Me.Pet.Height}>1) /call check_AutoShrink

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
	} else /if (${PetSpell.Arg[2,/].Length} && !${FindItemCount[=${PetSpell.Arg[2,/]}].ID}) {
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
				/call e3_Cast ${Me.Pet.ID} "${MyPetBuffs[${i}].Arg[1,/]}" Gem${DefaultGem} 5s
				/RETURN
			}
			
	|----------------------------------------------------------------------------------------------------------------------------------Cast item.
		} else /if (${MyPetBuffs[${i}].Arg[2,/].Equal[item]}) {
			
		|---------Check to see if your pet already has the buff, and that it will stack with your pet's current buffs.	
			/if (${FindItem[${MyPetBuffs[${i}].Arg[1,/]}].ID} && !${Me.PetBuff[${FindItem[${MyPetBuffs[${i}].Arg[1,/]}].Spell}]} && ${Spell[${FindItem[${MyPetBuffs[${i}].Arg[1,/]}].Spell}].StacksPet}) {
			
			|--------------------------------------------------------------------------------Cast PetBuff, and return.
				/call e3_Cast ${Me.Pet.ID} "${MyPetBuffs[${i}].Arg[1,/]}" item 3s
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
		/call e3_Cast ${Me.Pet.ID} "Tiny Companion" Gem${DefaultGem} 5s
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
		/call e3_Cast "Suspended Minion" alt 5s
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
		/call e3_Cast ${Me.Pet.ID} "${Pet_Heal.Arg[1,/]}" ${Pet_Heal.Arg[2,/]} 5s
	}

/if (${Debug}) /echo <== PetHeal -|
/RETURN



SUB setup_Pets
/if (${Debug}) /echo |- setup_Pets ==>

	/declare i int local
|----------------------------------------------------------------------------------------------------------------------------Validate ini entries.
	/if (${Ini[${Character_Ini},Pets].Length}) {
	
		/call INItoVar ${Character_Ini},Pets,Pet(Spell/Reageant) PetSpell string outer
		
		|------------------------------Validate pet entries.
		/if (${Defined[PetSpell]}) {
			/if (!${Int[${Me.Book[${PetSpell.Arg[1,/]}]}]}) {
				/echo I can't find [${PetSpell.Arg[1,/]}] in my spellbook, disabling pets.
				/deletevar PetSpell
			} else /if (${PetSpell.Arg[2,/].Length} && !${FindItemCount[${PetSpell.Arg[2,/]}]}) {
				/echo I don't have any [${PetSpell.Arg[2,/]}], disabling pets. Please review my settings and reload e3.
				/deletevar PetSpell
			}
			
			/if (${Defined[PetSpell]}) {
				/declare Pets bool outer TRUE
				/declare pet_rebuffTimer timer outer
				/if (!${Defined[suspend_Pets]}) /declare suspend_Pets bool global FALSE
				/if (!${Defined[currently_Suspended]}) /declare currently_Suspended bool global FALSE
				/call INItoVar ${Character_Ini},Pets,Pet_AutoShrink(On/Off) Pet_AutoShrink bool outer
				/call INItoVar ${Character_Ini},Pets,Pet_Heal(Spell/Gem/Pct) Pet_Heal string outer
				/if (${Defined[Pet_Heal]}) {
					/if (!${Int[${Me.Book[${Pet_Heal.Arg[1,/]}]}]}) {
						/echo I can't find [${Pet_Heal.Arg[1,/]}] in my spellbook, disabling Pet_Heals.
						/deletevar Pet_Heal
					}
				}
				/call INItoVar ${Character_Ini},Pets,Pet_Mend(Pct) Pet_MendPct int outer
				/call INItoArray ${Character_Ini},Pets,PetBuff# MyPetBuffs
				/if (${MyPetBuffs.Size}) {
					/for i 1 to ${MyPetBuffs.Size}
					
						/if (!${MyPetBuffs[${i}].Arg[2,/].Length}) {
							/if (!${Int[${Me.Book[${MyPetBuffs[${i}]}]}]}) {
								/echo ERROR: @[PetBuff#${i}] - I do not have [${MyPetBuffs[${i}]}] in my spellbook. Removing it from MyPetBuffs.
								/varset MyPetBuffs[${i}] BAD_ENTRY
							} else {
								/varset MyPetBuffs[${i}] ${MyPetBuffs[${i}]}/gem${DefaultGem}
								/if (${Debug}) /echo @[PetBuffs#${i}] - No spellgem has been specified for [${MyPetBuffs[${i}].Arg[1,/]}], defaulting to [${MyPetBuffs[${i}].Arg[2,/]}].
							}
						} else /if (${MyPetBuffs[${i}].Arg[2,/].Left[3].Equal[gem]}) {
							/if (!${Int[${Me.Book[${MyPetBuffs[${i}].Arg[1,/]}]}]}) {
								/echo ERROR: @[PetBuff#${i}] - I do not have [${MyPetBuffs[${i}]}] in my spellbook. Removing it from MyPetBuffs.
								/varset MyPetBuffs[${i}] BAD_ENTRY
							}
						} else /if (${MyPetBuffs[${i}].Arg[2,/].Equal[item]}) {
							/if (!${FindItemCount[${MyPetBuffs[${i}].Arg[1,/]}]}) {
								/echo ERROR: @[PetBuff#${i}] - I do not have [${MyPetBuffs[${i}].Arg[1,/]}] in my inventory. Removing it from MyPetBuffs.
								/varset MyPetBuffs[${i}] BAD_ENTRY
							}
						} else /if (${Select[${MyPetBuffs[${i}].Arg[2,/]},alt,aa]}) {
							/if (!${Me.AltAbility[${MyPetBuffs[${i}].Arg[1,/]}]}) {
								/echo ERROR: @[PetBuff#${i}] - I do not have any ranks in [${MyPetBuffs[${i}].Arg[1,/]}]. Removing it from MyPetBuffs.
								/varset MyPetBuffs[${i}] BAD_ENTRY
							}
						}
						
					/next i
				}
			}
		}
	}
	
/if (${Debug}) /echo <== setup_Pets -|
/RETURN