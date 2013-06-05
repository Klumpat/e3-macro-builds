|------------------------------------------------------------------------|
|- e3_Macro_Pets.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains pet related functions for e3.mac							-|
|------------------------------------------------------------------------|

| /tell relays for suspend, return, and shrink pet commands

|----------------------------------------------------------------------------|
|- Pets.incs Mainloop hook.													-|
|----------------------------------------------------------------------------|
|- Checks your pets status. Automatically, resummons, buffs, and shrinks.	-|
|----------------------------------------------------------------------------|
SUB check_PetStatus
/if (${Debug}) /echo |- check_PetStatus ==>

	/if (!${Me.Pet.ID}) /call summonPet
	/if (!${ActionTaken} && ${Me.Pet.ID}) {
	
	|------------------------------------------------------------------------------------------------------------------------------Maintain pet buffs.
		/if (${petBuffs.Size} && !${pet_rebuffTimer}) /call buffPet
		/if (!${ActionTaken}) {
		
		|-----------------------------------------------------------------------------------------------------------------------Check Auto_Shrink.
			/if (${petAutoShrink}) /call check_AutoShrink
		}
	}

/if (${Debug}) /echo <== check_PetStatus -|
/RETURN



|------------------------------------------------------------|
|- Summons a new pet.										-|
|------------------------------------------------------------|
|- Looks for reagents, and checks mana, before summoning.	-|
|------------------------------------------------------------|
SUB summonPet
/if (${Debug}) /echo |- summonPet ==>

	/declare castName string local ${petSpell.Arg[1,/]}
	/declare gemNumber int local ${DefaultGem}

	| /Reagent Check.
	/if (${petSpell.Find[/reagent|]}) {
		/call argueString reagent "${petSpell}"
		/if (${Bool[${Macro.Return}]}) {
			/if (!${FindItemCount[${Macro.Return}]}) {
				/if (!${spamTimer_summonPet}) /echo I am out of [${Macro.Return}] and cannot summon a pet. Please restock me!!!
			
				/call createTimer spamTimer_summonPet 5m
				/goto :skipCast
			}
		}
	}
	
	| /Gem| Check.
	/if (${petSpell.Find[/gem]}) {
		/call argueString gem "${petSpell}"
		/varset gemNumber ${Macro.Return}
	}
	
	/if (${Debug}) /echo |- summon_Pet -| Attempting to summon [${castName}].
	/call e3_Cast ${Me.ID} "${castName}" gem${gemNumber} ${Math.Calc[${Spell[${castName}].RecastTime} + 10]}s
	
	:skipCast
	
/if (${Debug}) /echo <== summonPet -|
/RETURN



#event suspendMinion "<#*#> Suspend Pets"
SUB event_suspendMinion
/if (${Debug}) /echo |- EVENT_suspendMinion ==>

	/if (${Me.Pet.ID}) /docommand ${ChatToggle} Suspending my pet...
	/if (${Me.AltAbilityReady[Suspended Minion]}) {
		/call e3_Cast ${Me.Pet.ID} "Suspended Minion" alt
		/delay 20 !${Me.Pet.ID}
		/varset currently_Suspended
	}
	/if (${Me.Pet.ID}) /pet get lost
	/if (${Defined[suspend_Pets]}) /varset suspend_Pets TRUE

/if (${Debug}) /echo <== EVENT_suspendMinion -|
/RETURN



#event returnMinion "#*#Return Pets#*#"
SUB event_returnMinion
/if (${Debug}) /echo |- EVENT_returnMinion ==>

	/if (${suspend_Pets}) {
	
		/docommand ${ChatToggle} Returning my pet...
		/if (${currently_Suspended}) {
			/call e3_Cast "Suspended Minion" alt
			/varset currently_Suspended FALSE
		}
		/varset suspend_Pets FALSE
	}
	
/if (${Debug}) /echo <== EVENT_returnMinion -|
/RETURN



|--------------------------------|
|- Automatically shrinks pets.	-|
|--------------------------------|
SUB check_AutoShrink
/if (${Debug}) /echo |- check_AutoShrink ==>
	
	/if (${Me.Pet.Height}>1) {
		/if (${FindItemCount[=Symbol of Ancient Summoning]}) {
			/call e3_Cast ${Me.Pet.ID} "Symbol of Ancient Summoning" item
		} else /if (${Me.Book[Tiny Companion]}) {
			/call e3_Cast ${Me.Pet.ID} "Tiny Companion" Gem${DefaultGem} ${If[${Me.Gem[Tiny Companion]},25,90]}
		}
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
	
	| Attempt to shrink pets.
	/if (${Me.Pet.ID} && ${Me.Pet.Height}>1 && ${Me.Book[Tiny Companion]}) /call e3_Cast ${Me.Pet.ID} "Tiny Companion" Gem${DefaultGem} 20s
	
/if (${Debug}) /echo <== event_shrinkPets -|	
/RETURN



|------------------------------------------------------------|
|- Checks pet health, and attempts to cast PetHeal spells.	-|
|------------------------------------------------------------|
SUB PetHeal
/if (${Debug}) /echo |- PetHeal ==>

	/declare castName string local ${petHeal.Arg[1,/]}
	/declare healPct int local ${petHeal.Arg[2,/]}
	/declare gemNumber int local ${DefaultGem}

	| Check Mend Companion AA.
	/if (${Me.AltAbilityReady[Mend Companion]} && ${Me.Pet.PctHPs} < ${petMendPct}) /call e3_Cast ${Me.Pet.ID} "Mend Companion" alt
	
	| Check Pet Health and cast heal spell.
	/if (${Me.Pet.PctHPs} < ${healPct}) /call e3_Cast ${Me.Pet.ID} "${castName}" gem ${gemNumber} ${If[${Me.Gem[${Spell[${Pet_Heal.Arg[1,/]}]}]},25,90]}

/if (${Debug}) /echo <== PetHeal -|
/RETURN




|------------------------------------------------------------------------|
|- Rebuff pet.															-|
|------------------------------------------------------------------------|
|- Casts spells, items, and alternate abilities, from MyPetBuffs array.	-|
|------------------------------------------------------------------------|
SUB buffPet
/if (${Debug}) /echo |- buffPet ==>

	| -Declare local variables.
	/declare buffName string local
	/declare castName string local
	/declare gemNumber int local ${DefaultGem}
	/declare i int local
	
	/for i 1 to ${petBuffs.Size}
	
		/varset castName ${petBuffs[${i}].Arg[1,/]}
	
		| /Reagent Check.
		/if (${petBuffs[${i}].Find[/reagent|]}) {
			/call argueString reagent "${petBuffs[${i}]}"
			/if (${Bool[${Macro.Return}]}) {
				/if (!${FindItemCount[${Macro.Return}]}) {
					/if (${Debug}) /echo |- buffPet -| I do not have any [${Macro.Return}], skipping Pet Buff#${i} [${petBuffs[${i}]}].
					/goto :skip_Buff
				}
			}
		}
		
		| -FindItem.
		/if (${FindItemCount[=${castName}]}) {
			
			|  /BuffName Check.
			/if (${petBuffs[${i}].Find[/buffname|]}) {
				/call argueString buffname "${petBuffs[${i}]}"
				/if (${Bool[${Macro.Return}]}) {
					/varset buffName ${Macro.Return}
				}
			} else {
				/varset buffName ${FindItem[=${castName}].Spell}
			}
			
			/if (!${Spell[${buffName}].StacksPet}) {
				/if (${Debug}) /echo |- buffPet -| [${castName}] will not stack, skipping.
			} else {
				/if (!${Me.PetBuff[${buffName}]}) /call e3_Cast ${Me.Pet.ID} "${castName}" item	
			}
			
		| -AltAbility.
		} else /if (${Me.AltAbility[${castName}]}) {
			
			| /BuffName Check.
			/if (${petBuffs[${i}].Find[/buffname|]}) {
				/call argueString buffname "${petBuffs[${i}]}"
				/if (${Bool[${Macro.Return}]}) {
					/varset buffName ${Macro.Return}
				}
			} else {
				/varset buffName ${Me.AltAbility[${castName}].Spell}
			}
		
			/if (!${Spell[${buffName}].StacksPet}) {
				/if (${Debug}) /echo |- buffPet -| [${castName}] will not stack, skipping.
			} else {
				/if (!${Me.PetBuff[${buffName}]}) /call e3_Cast ${Me.Pet.ID} "${castName}" alt
			}
			
		| -CastSpell.
		} else /if (${Int[${Me.Book[${castName}]}]}) {

			| /BuffName Check.
			/if (${petBuffs[${i}].Find[/buffname|]}) {
				/call argueString buffname "${petBuffs[${i}]}"
				/if (${Bool[${Macro.Return}]}) {
					/varset buffName ${Macro.Return}
				}
			} else {
				/varset buffName ${castName}
			}
			
			| /Gem| Check.
			/if (${petBuffs[${i}].Find[/gem]}) {
				/call argueString gem "${petBuffs[${i}]}"
				/varset gemNumber ${Macro.Return}
			}
			
			/if (!${Spell[${buffName}].StacksPet}) {
				/if (${Debug}) /echo |- buffPet -| [${castName}] will not stack, skipping.
			} else {
				/if (!${Me.PetBuff[${buffName}]}) /call e3_Cast ${Me.Pet.ID} "${castName}" gem${gemNumber} ${Math.Calc[${Spell[${castName}].RecastTime} + 10]}s
			}
			
		| -Found in bank.
		} else /if (${FindItemBankCount[=${castName}]}) {
			/docommand ${ChatToggle} I cannot cast [${castName}] because it's in the bank.  Removing [${castName}] from my Pet Buffs list.
			/call RemoveArrayElement petBuffs "${petBuffs[${i}]}"
			/beep
			
		| -Unrecognized.
		} else {
			/docommand ${ChatToggle} I can't find any items, alternate abilities, or spells named, [${castName}].  Removing [${petBuffs[${i}]}] from my Pet Buffs list.  Please review this entry in [${Character_Ini}].
			/call RemoveArrayElement petBuffs "${petBuffs[${i}]}"
			/beep
		}
	
	:skip_Buff
	/if (!${ActionTaken}) /next i
	
	/varset pet_rebuffTimer ${buffCheck_Interval}s

/if (${Debug}) /echo <== buffPet -|
/RETURN



Sub pet_Setup
/if (${Debug}) /echo |- pet_Setup ==>
	
	/if (${Ini[${Character_Ini},Pets].Length}) {
		/if (${Ini[${Character_Ini},Pets,Pet Spell].Length}) /call INItoVar "${Character_Ini},Pets,Pet Spell" petSpell string outer
			
		/if (${Defined[petSpell]}) {
			/declare Pets bool outer TRUE
			/if (!${Defined[suspend_Pets]}) /declare suspend_Pets bool global FALSE
			/if (!${Defined[currently_Suspended]}) /declare currently_Suspended bool global FALSE
			
			/if (${Ini[${Character_Ini},Pets,Pet Auto-Shrink (On/Off)].Length}) /call INItoVar "${Character_Ini},Pets,Pet Auto-Shrink (On/Off)" petAutoShrink bool outer	
			/if (${Ini[${Character_Ini},Pets,Pet Heal (Spell/Pct)].Length}) /call INItoVar "${Character_Ini},Pets,Pet Heal (Spell/Pct)" petHeal string outer
			/if (${Ini[${Character_Ini},Pets,Pet Mend (Pct)].Length}) /call INItoVar "${Character_Ini},Pets,Pet Mend (Pct)" petMendPct int outer
			/if (${Ini[${Character_Ini},Pets,Pet Buff#1].Length}) {
				/call INItoArray "${Character_Ini},Pets,Pet Buff#" petBuffs
				/declare pet_rebuffTimer timer outer
			}
		}
	}
	
/if (${Debug}) /echo <== pet_Setup -|
/return



Sub pet_Background_Events

/return



SUB pet_MacroSettings
/if (${Debug}) /echo |- pet_MacroSettings ==>

/if (${Debug}) /echo <== pet_MacroSettings -|
/RETURN



SUB pet_CharacterSettings
/if (${Debug}) /echo |- pet_CharacterSettings ==>

	/if (${Select[${Me.Class.ShortName},BST,ENC,MAG,NEC,SHD,SHM]}) {	
		/call WriteToIni "${Character_Ini},Pets,Pet Spell"
		/call WriteToIni "${Character_Ini},Pets,Pet Auto-Shrink (On/Off)"
		/call WriteToIni "${Character_Ini},Pets,Pet Heal (Spell/Pct)"
		/call WriteToIni "${Character_Ini},Pets,Pet Mend (Pct)"
		/call WriteToIni "${Character_Ini},Pets,Pet Buff#1"
	}

/if (${Debug}) /echo <== pet_CharacterSettings -|
/RETURN



Sub pet_Aliases
/if (${Debug}) /echo |- pet_Aliases ==>

/if (${Debug}) /echo <== pet_Aliases -|
/return