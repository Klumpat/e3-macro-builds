|------------------------------------------------------------------------|
|- e3_Macro_Heals.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains heal related functions for e3.mac							-|
|------------------------------------------------------------------------|

| add group heals
| add heals for non netbot group members
| add an event to create an array of non netbot heal targets.
| Add a list for tank only heals?

|------------------------------------------------------------|
|- Heals.inc's mainLoop hook.								-|	
|------------------------------------------------------------|					
|- Calls heal functions, as an extention of the main loop.	-|	
|------------------------------------------------------------|
SUB CheckHeals
/if (${Debug}) /echo |- CheckHeals  ==>

	| Check MainTank.
	/if (${tankList.Size}) /call check_Tanks
	/if (!${ActionTaken}) {
	
		| Check priorityTargets.
		/if (${priorityTargets.Size}) /call check_PriorityTargets
		/if (!${ActionTaken}) {
		
			| Check NetBots.
			/if (${NetBots.Counts}) /call check_NetBots
			/if (!${ActionTaken}) {
			
				| Check HoT.
				/if (${Defined[HoT_Spell]} && ${NetBots.Counts}) /call check_HoT
				/if (${recentHeal_Timer}) /varset ActionTaken TRUE
			}
		}
	}
	
/if (${Debug}) /echo <== CheckHeals -|
/RETURN



|--------------------------------------------------------------------------------------------------------------------|
|- Uses Items, Spells, Abilities, AAs, and Disciplines listed in the [LifeSupport] section of the Character_Ini.	-|
|--------------------------------------------------------------------------------------------------------------------|
SUB check_lifeSupport
/if (${Debug}) /echo |- check_lifeSupport ==>
	
	/if (${Target.ID}) /declare oldTarget int local ${Target.ID}
	/declare castName string local
	/declare minHealth int local
	
	/declare i int local
	
	/for i 1 to ${lifeSupport.Size}
	
		/varset castName ${lifeSupport[${i}].Arg[1,/]}
		/varset minHealth ${lifeSupport[${i}].Arg[2,/]}
		
		/if (${Me.PctHPs} && ${Me.PctHPs} < ${minHealth}) {
		
			| /Reagent Check.
			/if (${lifeSupport[${i}].Find[/reagent|]}) {
				/call get_SpellInfo reagent "${lifeSupport[${i}]}"
				/if (${Bool[${Macro.Return}]}) {
					/if (!${FindItemCount[${Macro.Return}]}) /goto :skip
				}
			}

			/if (${Me.SpellReady[${castName}]}) {
				/call e3_Cast ${Me.ID} "${castName}" gem
			} else /if (${Me.AltAbilityReady[${castName}]} && !${itemTimer_${Me.AltAbility[${castName}].ID}}) {
				/call e3_Cast ${Me.ID} "${castName}" alt
			} else /if (${FindItemCount[=${castName}]} && !${FindItem[${castName}].Timer} && !${itemTimer_${FindItem[${castName}].ID}}) {
				/call e3_Cast ${Me.ID} "${castName}" item
			} else /if (${Me.CombatAbilityReady[${castName}]} && !${discTimer_${castName.Replace[ ,]}}) {
				/call e3_Cast ${Me.ID} "${castName}" disc
			} else /if (${Me.AbilityReady[${castName}]}) {
				/doability "${castName}"
			}
		}
		
	/if (!${ActionTaken}) {
		:skip
		/next i
	} else {
		/if (${oldTarget}) /call TrueTarget ${oldTarget}
	}

/if (${Debug}) /echo <== check_lifeSupport -|
/RETURN




|----
|- check_Tanks
|-----
|- Find the tank to be healed, and passes the name to determineHeal function.
|---
SUB check_Tanks
/if (${Debug}) /echo |- check_Tanks ==>

	/declare currentTarget string local
	/declare i int local
	
	/for i 1 to ${tankList.Size}
	
		/if (${SpawnCount[pc ${tankList[${i}]}]} && ${Spawn[pc ${tankList[${i}]}].Distance} < ${If[${Select[${Me.Class.ShortName},CLR,PAL]},100,200]}) {
			/varset currentTarget ${tankList[${i}]}
			/if (${NetBots[${currentTarget}].PctHPs} && ${NetBots[${currentTarget}].PctHPs} < 100) {
				/if (${Debug}) /echo |- check_Tanks -| [${currentTarget} -- ${NetBots[${currentTarget}].PctHPs}]
				/call determineHeal ${currentTarget}
			}
		}

	/if (!${Bool[${currentTank}]}) /next i
	
/if (${Debug}) /echo <== check_Tanks -|
/RETURN



|------------------------------------------------|
|- check_PriorityTargets						-|
|------------------------------------------------|
|- Checks NetBots listed as PriorityTargets.	-|
|------------------------------------------------|
SUB check_PriorityTargets
/if (${Debug}) /echo |- check_PriorityTargets ==>

	/declare currentTarget string local
	/declare i int local
	
	/for i 1 to ${priorityTargets.Size}
	
		/if (${SpawnCount[pc ${priorityTargets[${i}]}]} && ${Spawn[pc ${priorityTargets[${i}]}].Distance} < ${If[${Select[${Me.Class.ShortName},CLR,PAL]},100,200]}) {
			/if (!${Bool[${currentTarget}]} || ${NetBots[${priorityTargets[${i}]}].PctHPs} < ${NetBots[${currentTarget}].PctHPs}) /varset currentTarget ${NetBots.Client[${i}]}
		}
	
	/next i

	/if (${NetBots[${currentTarget}].PctHPs} && ${NetBots[${currentTarget}].PctHPs} < 100) {
		/if (${Debug}) /echo |- check_PriorityTargets -| [${currentTarget} -- ${NetBots[${currentTarget}].PctHPs}]
		/call determineHeal ${currentTarget}
	}

/if (${Debug}) /echo <== check_PriorityTargets -|
/RETURN



|----------------------------------------------------|
|- check_NetBots									-|
|----------------------------------------------------|
|- Checks all NetBots, and heals whoever is lowest.	-|
|----------------------------------------------------|
SUB check_NetBots
/if (${Debug}) /echo |- check_NetBots ==>

	/declare currentTarget string local
	/declare i int local
	
	/for i 1 to ${NetBots.Counts}
		/if (${SpawnCount[pc ${NetBots.Client[${i}]}]} && ${Spawn[pc ${NetBots.Client[${i}]}].Distance} < ${If[${Select[${Me.Class.ShortName},CLR,PAL]},100,200]}) {
			/if (!${Bool[${currentTarget}]} || ${NetBots[${NetBots.Client[${i}]}].PctHPs} < ${NetBots[${currentTarget}].PctHPs}) /varset currentTarget ${NetBots.Client[${i}]}
		}
	/next i

	/if (${NetBots[${currentTarget}].PctHPs} && ${NetBots[${currentTarget}].PctHPs} < 100) {
		/if (${Debug}) /echo |- check_NetBots -| [${currentTarget} -- ${NetBots[${currentTarget}].PctHPs}]
		/call determineHeal ${currentTarget}
	}

/if (${Debug}) /echo <== check_NetBots -|
/RETURN



SUB determineHeal(healTarget)
/if (${Debug}) /echo |- determineHeal ==>	
	
	/declare healToCheck string local
	/declare healToUse string local
	/declare castType string local
	/declare gemNumber int local ${DefaultGem}
	/declare healPct int local
	/declare healthDifferential int local 100

	/declare i int local
	
	/for i 1 to ${healSpells.Size}
	
		| /Reagent Check.
		/if (${healSpells[${i}].Find[/reagent|]}) {
			/call argueString reagent "${healSpells[${i}]}"
			/if (${Bool[${Macro.Return}]}) {
				/if (!${FindItemCount[=${Macro.Return}]}) {
					/if (${Debug}) /echo |- determineHeal -| I do not have any [${Macro.Return}], skipping Heal#${i} [${healSpells[${i}]}].
					/goto :skipHeal
				}
			}
		}
		
		/varset healToCheck ${healSpells[${i}].Arg[1,/]}
		
		| /HealPct Check
		/if (${healSpells[${i}].Find[/HealPct|]}) {
			/call argueString HealPct "${healSpells[${i}]}"
			/varset healPct ${Macro.Return}
		}

		/if (${NetBots[${healTarget}].PctHPs} < ${healPct}) {
			
			| ----------------------------------------- Item -----------------------------------------|
			/if (${FindItemCount[=${healToCheck}]}) {
				| Check Timer 
				/if (${FindItem[=${nukeName}].Timer} || ${itemTimer_${FindItem[=${nukeName}].ID}}) {
					/echo [${healToCheck}] is not ready! [${If[${FindItem[=${healToCheck}].Timer},${FindItem[=${healToCheck}].Timer} ticks,${Math.Calc[${itemTimer_${FindItem[=${healToCheck}].ID}}/10].Int} sec]}] until this item is available.
				} else {
					
					| Check Range 
					/if (${Spawn[pc ${healTarget}].Distance} > ${Spell[${FindItem[=${healToCheck}].Spell}].MyRange}) {
						/echo [${healTarget}] is out of range for [${healToCheck}].
					} else {
						/if (${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int} < ${healthDifferential}) {
							/varset healthDifferential ${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int}
							/varset healToUse ${healToCheck}
							/varset castType item
						}
					}
				}
				
			| ----------------------------------------- Alt Ability -----------------------------------------|
			} else /if (${Me.AltAbility[${healToCheck}]}) {
				| Check Timer 
				/if (!${Me.AltAbilityReady[${healToCheck}]} || ${altTimer_${Me.AltAbility[${nukeName}].ID}}) {
					/echo [${healToCheck}] wont be ready for another [${altTimer_${Me.AltAbility[${healToCheck}].ID}}] seconds!
				} else {
					
					| Check Range 
					/if (${Spawn[pc ${healTarget}].Distance} > ${Spell[${Me.AltAbility[${healToCheck}].Spell}].MyRange}) {
						/echo [${healTarget}] is out of range for [${healToCheck}].
					} else {
						/if (${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int} < ${healthDifferential}) {
							/varset healthDifferential ${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int}
							/varset healToUse ${healToCheck}
							/varset castType alt
						}
					}
				}
				
			| ----------------------------------------- Spell -----------------------------------------|	
			} else /if (${Me.Book[${healToCheck}]}) {
			
				| /Gem| Check.
				/if (${healSpells[${i}].Find[/gem|]}) {
					/call argueString gem "${healSpells[${i}]}"
					/varset gemNumber ${Macro.Return}
				}
				
				| Check mana 
				/if (${Me.CurrentMana[${healToCheck}]} < ${Spell[${healToCheck}].Mana}) {
					/echo I don't have enough mana to cast [${healToCheck}].
				} else {
					
					| Check Range 
					/if (${Spawn[pc ${healTarget}].Distance} > ${Spell[${healToCheck}].MyRange}) {
						/echo [${healTarget}] is out of range for [${healToCheck}].
					} else {
						/if (${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int} < ${healthDifferential}) {
							/varset healthDifferential ${Math.Calc[${healPct} - ${NetBots[${healTarget}].PctHPs}].Int}
							/varset healToUse ${healToCheck}
							/varset castType gem${gemNumber}
						}
					}
				}
			}
		}
	
	:skipHeal
	/next i
	
	| cast heal
	/if (${Bool[${healToUse}]}) {
		/call e3_Cast ${Spawn[pc ${healTarget}].ID} "${healToUse}" ${castType} ${If[${castType.Find[gem]},${If[${Me.Gem[${healToUse}]},3s,9s]},]}
		/if (${castReturn.Equal[CAST_SUCCESS]}) /varset recentHeal_Timer 20
	}

/if (${Debug}) /echo <== determineHeal -|
/RETURN



|----------------------------------------|
|- Checks to make while casting heals.	-|
|----------------------------------------|
SUB Heal_CastingChecks
/if (${Debug}) /echo |- HoT_CastingChecks ==>

	/if (${castEndTime}<7 && ${NetBots[${Target.CleanName}].PctHPs}>=90) /call interrupt

/if (${Debug}) /echo <== HoT_CastingChecks -|
/RETURN



|----------------|
|- check_HoT	-|
|----------------|
|-
|-----------
SUB check_HoT
/if (${Debug}) /echo |- check_HoT ==>

	/declare healSpell string local ${HoT_Spell.Arg[1,/]}
	/declare healGem int local ${HoT_Spell.Arg[2,/].Right[1]}
	/declare healPct int local ${HoT_Spell.Arg[3,/]}

	/if (${Me.CurrentMana} < ${Spell[${healSpell}].Mana}) {
		/if (${Debug}) /echo |- check_HoT -| I don't have enough mana to cast [${healSpell}].
	} else {
	
		/declare i int local
		/declare healTarget string local NOTARGET
		/declare buffList string local
		
	|-------------------------------------------------------------------------------------------------------Check Tanks
		/if (${Select[TankOnly,${HoT_Mode}]} || ${Select[Tank,${HoT_Mode}]} || ${Select[MTOnly,${HoT_Mode}]}) {
		
			/for i 1 to ${TankList.Size}
		
			/if (!${SpawnCount[pc ${TankList[${i}]}]} || ${SpawnCount[pc ${TankList[${i}]}]} && ${Spawn[pc ${TankList[${i}]}].Distance} > ${Spell[${healSpell}].MyRange}) {
				/next i
			} else {

				/if (${NetBots[${TankList[${i}]}].PctHPs} < ${healPct}) {

					/varset buffList ${NetBots[${TankList[${i}]}].Buff.Replace[ ,,]}

					/if (!${Select[${Spell[${healSpell}].ID},${buffList}]}) {

						/call e3_Cast ${NetBots[${TankList[${i}]}].ID} "${healSpell}" gem${healGem} ${If[${Me.Gem[${healSpell}]},3s,9s]} HoT_CastingChecks
						/varset recentHeal_Timer 30
					}
				}
			}
		}
		/if (!${ActionTaken}) {
		
		|-------------------------------------------------------------------------------------------------------Check Priority
			/if (${Select[Priority,${HoT_Mode}]} || ${Select[PriorityTargets,${HoT_Mode}]}) {
			
				/for i 1 to ${priorityTargets.Size}
		
				/if (!${SpawnCount[pc ${priorityTargets[${i}]}]} || ${SpawnCount[pc ${priorityTargets[${i}]}]} && ${Spawn[pc ${priorityTargets[${i}]}].Distance} > ${Spell[${healSpell}].MyRange}) {
					/next i
				} else {

					/if (${NetBots[${priorityTargets[${i}]}].PctHPs} < ${healPct}) {

						/varset buffList ${NetBots[${priorityTargets[${i}]}].Buff.Replace[ ,,]}

						/if (!${Select[${Spell[${healSpell}].ID},${buffList}]}) {

							/call e3_Cast ${NetBots[${priorityTargets[${i}]}].ID} "${healSpell}" gem${healGem} ${If[${Me.Gem[${healSpell}]},3s,9s]} HoT_CastingChecks
							/varset recentHeal_Timer 30
						}
					}
				}
			}
			/if (!${ActionTaken}) {
				
			|-------------------------------------------------------------------------------------------------------Check NetBots
				/if (${Select[All,${HoT_Mode}]}) {
					/for i 1 to ${NetBots.Counts}
		
					/if (!${SpawnCount[pc ${NetBots.Client[${i}]}]} || ${SpawnCount[pc ${NetBots.Client[${i}]}]} && ${Spawn[pc ${NetBots.Client[${i}]}].Distance} > ${Spell[${healSpell}].MyRange}) {
						/next i
					} else {

						/if (${NetBots[${NetBots.Client[${i}]}].PctHPs} < ${healPct}) {

							/varset buffList ${NetBots[${NetBots.Client[${i}]}].Buff.Replace[ ,,]}

							/if (!${Select[${Spell[${healSpell}].ID},${buffList}]}) {

								/call e3_Cast ${NetBots[${NetBots.Client[${i}]}].ID} "${healSpell}" gem${healGem} ${If[${Me.Gem[${healSpell}]},3s,9s]} HoT_CastingChecks
								/varset recentHeal_Timer 30
							}
						}
					}
				}
				| next mode |
			}
		}
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
SUB heal_Setup
/if (${Debug}) /echo |- heal_Setup ==>

	| Life Support
	/if (${Ini[${Character_Ini},Life Support,Life Support#1].Length}) /call IniToArray "${Character_Ini},Life Support,Life Support#" lifeSupport
	
	| Heals
	/if (${Ini[${Character_Ini},Heals,Heal#1].Length})  /call IniToArray "${Character_Ini},Heals,Heal#" healSpells
	/if (${Ini[${Character_Ini},Heals,Tank#1].Length})  /call IniToArray "${Character_Ini},Heals,Tank#" tankList
	/if (${Ini[${Character_Ini},Heals,Important Bot#1].Length})  /call IniToArray "${Character_Ini},Heals,Important Bot#" priorityTargets
	/if (${Ini[${Character_Ini},Heals,HoT Mode (Tanks Only/All/Off)].Length})  /call INItoVar "${Character_Ini},Heals,HoT Mode (Tanks Only/All/Off)" hotMode string outer
	/if (${Ini[${Character_Ini},Heals,Heal Over Time (Spell/Pct)].Length})  /call INItoVar "${Character_Ini},Heals,Heal Over Time (Spell/Pct)" hotSpell string outer
	/if (${Ini[${Character_Ini},Heals,Pet Heal (Spell/Pct)].Length})  /call INItoVar "${Character_Ini},Heals,Pet Heal (Spell/Pct)" petHeal string outer

	/if (${Defined[healSpells]}) {
		/declare Heals bool outer TRUE
		/declare recentHeal_Timer timer outer
	}

/if (${Debug}) /echo <== heal_Setup -|
/RETURN


 
Sub heal_Background_Events

/return



SUB heal_MacroSettings
/if (${Debug}) /echo |- assist_MacroSettings ==>

/if (${Debug}) /echo <== heal_MacroSettings -|
/RETURN



SUB heal_CharacterSettings
/if (${Debug}) /echo |- heal_CharacterSettings ==>

	| LifeSupport entries.
	/call WriteToIni "${Character_Ini},Life Support,Life Support#1"

	| If I'm a healer, add e3_Macro_Heals.inc entries.
	/if (${Me.Class.HealerType}) {
		/call WriteToIni "${Character_Ini},Heals,Heal#1"
		/call WriteToIni "${Character_Ini},Heals,Tank#1"
		/call WriteToIni "${Character_Ini},Heals,Important Bot#1"
		| to be added after r4
		| /call WriteToIni "${Character_Ini},Heals,HoT Mode (Tanks Only/All/Off)"
		| /call WriteToIni "${Character_Ini},Heals,Heal Over Time (Spell/Pct)"
		| /call WriteToIni "${Character_Ini},Heals,Pet Heal (Spell/Pct)"
		| /if (${Me.Class.ShortName.Equal[CLR]} || ${Me.Class.ShortName.Equal[DRU]}) /call WriteToIni "${Character_Ini},Heals,Join CH-Chain (On/Off)"
	}

/if (${Debug}) /echo <== heal_CharacterSettings -|
/RETURN



Sub heal_Aliases
/if (${Debug}) /echo |- heal_Aliases ==>

/if (${Debug}) /echo <== heal_Aliases -|
/return