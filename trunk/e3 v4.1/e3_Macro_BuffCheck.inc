|--------------------------------------------------------------------------------|
|- buffCheck's mainloop hook.													-|
|--------------------------------------------------------------------------------|
|- Calls buff functions, returning to the mainloop whenever a spell is cast.	-|
|--------------------------------------------------------------------------------|
SUB buffCheck
/if (${Debug}) /echo |- buffCheck ==>

	/if (${Cursor.ID} || ${Corpse.Open}) {
		/varset rebuffTimer 100
	} else {
	
		/if (!${Me.Combat} && !${Assisting} && !${Me.Feigning} && !${Me.Moving} && ${Me.PctHPs} > 70) {
		
		|--------------------------------------------------------------------------------------SelfBuffs.
			/if (${SelfBuffs.Size} && ${Me.FreeBuffSlots} > ${MaxBuffs}) /call buffSelf
			/if (${ActionTaken}) /RETURN
		
		|--------------------------------------------------------------------------------------BuffOthers.
			/if (${BuffOthers.Size}) /call buffOthers
			/if (${ActionTaken}) /RETURN
			
		|----------------------------------------------------------If all buffs are cast, set rebuffTimer.
			/varset rebuffTimer ${buffCheck_Interval}s
		}
	}
	
/if (${Debug}) /echo <== buffCheck -|
/RETURN



|------------------------------------------------|
|- Cast and maintain buffs on this character.	-|
|------------------------------------------------|
SUB buffSelf
/if (${Debug}) /echo |- buffSelf ==>

	/declare i int local
	/declare buffName string local
	/declare castType string local
	
	/for i 1 to ${SelfBuffs.Size}

		/varset buffName ${If[${SelfBuffs[${i}].Arg[1,/].Find[::]},${SelfBuffs[${i}].Arg[1,/].Arg[2,::]},${SelfBuffs[${i}].Arg[1,/]}]}
		/varset castType ${SelfBuffs[${i}].Arg[2,/]}
			
		|------------------------------------------------------------------------------------------------------------------------------------Cast a spell.
			/if (${castType.Find[gem]}) {
				/if (!${Spell[${buffName}].Stacks}) {
					/if (${Debug}) /echo |- buffSelf -| [${buffName}] will not stack.  Skipping.
				} else {
					/if (${Me.Buff[${buffName}].Duration} < ${Math.Calc[${buffCheck_Interval} / 6].Int}) {
						/if (${Debug}) /echo |- buffSelf -| Casting Spell: [${buffName} -- ${castType} -- ${If[${Me.Gem[${buffName}]},3s,9s]}] on [${Me.CleanName}]
						/call e3_Cast ${Me.ID} "${buffName}" ${castType} ${If[${Me.Gem[${buffName}]},30,90]}
						/RETURN
					}
				}
				
		|------------------------------------------------------------------------------------------------------------------------------------Cast an item.
			} else /if (${castType.Equal[item]}) {
				/if (!${Spell[${FindItem[${buffName}].Spell}].Stacks}) {
					/if (${Debug}) /echo |- buffSelf -| [${FindItem[${buffName}].Spell}(${buffName})] will not stack.  Skipping.
				} else {
					/if (!${FindItemCount[${buffName}]}) {
						/if (${Debug}) /echo |- buffSelf -| Item: [${buffName}] not found.  Skipping.
					} else {
						/if (${Me.Buff[${FindItem[${buffName}].Spell}].Duration} < ${Math.Calc[${buffCheck_Interval} / 6].Int}) {
							/if (${Debug}) /echo |- buffSelf -| Casting Item: [${buffName}] on [${Me.CleanName}]
							/call e3_Cast ${Me.ID} "${buffName}" item
							/RETURN
						}
					}
				}

		|-----------------------------------------------------------------------------------------------------------------------------------------Cast an AA.		
			} else /if (${castType.Equal[alt]} || ${castType.Equal[aa]}) {
				/if (!${Spell[${Me.AltAbility[${buffName}].Spell}].Stacks}) {
					/if (${Debug}) /echo |- buffSelf -| [${Me.AltAbility[${buffName}].Spell}] will not stack.  Skipping.
				} else {
					/if (!${Me.AltAbilityReady[${buffName}]}) {
						/if (${Debug}) /echo |- buffSelf -| Alternate Ability: [${buffName}] is not ready.  Skipping.
					} else {
						/if (${Me.Buff[${Me.AltAbility[${buffName}].Spell}].Duration} < ${Math.Calc[${buffCheck_Interval} / 6].Int}) {
							/if (${Debug}) /echo |- buffSelf -| Casting AltAbility: [${buffName}] on [${Me.CleanName}]
							/call e3_Cast ${Me.ID} "${buffName}" alt
							/RETURN
						}
					}
				}
				
		|----------------------------------------------------------------------------------------------------------------------------------------BAD_ENTRY.	
			} else /if (${castType.Equal[BAD_ENTRY]}) {
				/if (${Debug}) /echo ERROR: [@Self Buff#${i}] [${SelfBuffs[${i}]}] has been flagged as a BAD_ENTRY.  Skipping.
			}
		}
	/next i
	
/if (${Debug}) /echo <== buffSelf -|
/RETURN



|----------------------------------------|
|- Cast and maintain buffs on a NetBot.	-|
|----------------------------------------|
SUB buffOthers
/if (${Debug}) /echo |- buffOthers ==>

	/declare i int local
	/declare x int local
	/declare groupList string local
	
	/declare buffTarget string local
	/declare buffName string local
	/declare castType string local
	
	/for i 1 to ${BuffOthers.Size}
	
		/varset buffTarget ${BuffOthers[${i}].Arg[1,/]}
		/varset buffName ${If[${BuffOthers[${i}].Arg[2,/].Find[::]},${BuffOthers[${i}].Arg[2,/].Arg[2,::]},${BuffOthers[${i}].Arg[2,/]}]}
		/varset castType ${BuffOthers[${i}].Arg[3,/]}
	
	|-------------------------------------------------------------------------------------------------------------------------If the spawn is in zone.
		/if (${SpawnCount[pc ${buffTarget}]} && ${NetBots[${buffTarget}].Zone} == ${Zone.ID}) {
		
		|----------------------------------------------------------Check for group only buffs.
			/if (${BuffOthers[${i}].Find[/group]}) {
				/for x 1 to ${Group}
					/varset groupList ${groupList}${Group.Member[${x}]}
				/next x
				/if (!${Select[${buffTarget},${groupList}]}) /next i
			}
		
		|--------------------------------------------------------------------------------------------------------------------------------Cast a spell.
			/if (${castType.Find[gem]}) {
				/if (!${NetBots[${buffTarget}].Buff.Find[${Spell[${buffName}].ID}]}) {
					/if (${Debug}) /echo |- buffOthers -| Casting Spell: [${buffName}] on [${buffTarget}]
					/call e3_Cast ${Spawn[pc ${buffTarget}].ID} "${buffName}" ${castType} ${If[${Me.Gem[${buffName}]},30,90]}
					/RETURN
				}
				
		|--------------------------------------------------------------------------------------------------------------------------------Cast an item.		
			} else /if (${castType.Equal[item]}) {
				/if (!${FindItemCount[${buffName}]}) {
					/if (${Debug}) /echo |- buffOthers -| Item: [${buffName}] not found.  Skipping.
				} else {
					/if (!${NetBots[${buffTarget}].Buff.Find[${Spell[${FindItem[${buffName}].Spell}].ID}]}) {
						/if (${Debug}) /echo |- buffOthers -| Casting Item: [${buffName}] on [${Spawn[pc ${buffTarget}].CleanName}]
						/call e3_Cast ${Spawn[pc ${buffTarget}].ID} "${buffName}" item
						/RETURN
					}
				}
				
		|--------------------------------------------------------------------------------------------------------------------------------Cast an AA.
			} else /if (${castType.Equal[alt]} || ${castType.Equal[aa]}) {
				/if (!${Me.AltAbilityReady[${buffName}]}) {
					/if (${Debug}) /echo |- buffOthers -| Alternate Ability: [${buffName}] is not ready.  Skipping.
				} else {
					/if (!${NetBots[${buffTarget}].Buff.Find[${Spell[${Me.AltAbility[${buffName}].Spell}].ID}]}) {
						/if (${Debug}) /echo |- buffOthers -| Casting AltAbility: [${buffName}] on [${Spawn[pc ${buffTarget}.CleanName}]
						/call e3_Cast ${Me.ID} "${buffName}" alt
						/RETURN
					}
				}

		|----------------------------------------------------------------------------------------------------------------------------------------BAD_ENTRY.	
			} else /if (${castType.Equal[BAD_ENTRY]}) {
				/if (${Debug}) /echo ERROR: [@Buff Other#${i}] [${BuffOthers[${i}]}] has been flagged as a BAD_ENTRY.  Skipping.
			}
		}

	/next i
	
/if (${Debug}) /echo <== buffOthers -|
/RETURN



|--------------------------------------------------------|
|- Cast and maintain high priority buffs on a NetBot.	-|
|--------------------------------------------------------|
SUB CombatBuffs
/if (${Debug}) /echo |- CombatBuffs ==>

	/declare i int local
	/declare x int local
	/declare groupList string local
	
	/declare buffTarget string local
	/declare buffName string local
	/declare castType string local
	
	/for i 1 to ${CombatBuffs.Size}
	
		/varset buffTarget ${CombatBuffs[${i}].Arg[1,/]}
		/varset buffName ${If[${CombatBuffs[${i}].Arg[2,/].Find[::]},${CombatBuffs[${i}].Arg[2,/].Arg[2,::]},${CombatBuffs[${i}].Arg[2,/]}]}
		/varset castType ${CombatBuffs[${i}].Arg[3,/]}
	
	|-------------------------------------------------------------------------------------------------------------------------If the spawn is in zone.
		/if (${SpawnCount[pc ${buffTarget}]} && ${NetBots[${buffTarget}].Zone} == ${Zone.ID}) {
		
		|----------------------------------------------------------Check for group only buffs.
			/if (${CombatBuffs[${i}].Find[/group]}) {
				/for x 1 to ${Group}
					/varset groupList ${groupList}${Group.Member[${x}]}
				/next x
				/if (!${Select[${buffTarget},${groupList}]}) /next i
			}
		
		|--------------------------------------------------------------------------------------------------------------------------------Cast a spell.
			/if (${castType.Find[gem]}) {
				/if (!${NetBots[${buffTarget}].Buff.Find[${Spell[${buffName}].ID}]}) {
					/if (${Debug}) /echo |- CombatBuffs -| Casting Spell: [${buffName}] on [${buffTarget}]
					/call e3_Cast ${Spawn[pc ${buffTarget}].ID} "${buffName}" ${castType} ${If[${Me.Gem[${buffName}]},30,90]}
					/RETURN
				}
				
		|--------------------------------------------------------------------------------------------------------------------------------Cast an item.		
			} else /if (${castType.Equal[item]}) {
				/if (!${FindItemCount[${buffName}]}) {
					/if (${Debug}) /echo |- CombatBuffs -| Item: [${buffName}] not found.  Skipping.
				} else {
					/if (!${NetBots[${buffTarget}].Buff.Find[${Spell[${FindItem[${buffName}].Spell}].ID}]}) {
						/if (${Debug}) /echo |- CombatBuffs -| Casting Item: [${buffName}] on [${Spawn[pc ${buffTarget}].CleanName}]
						/call e3_Cast ${Spawn[pc ${buffTarget}].ID} "${buffName}" item
						/RETURN
					}
				}
				
		|--------------------------------------------------------------------------------------------------------------------------------Cast an AA.
			} else /if (${castType.Equal[alt]} || ${castType.Equal[aa]}) {
				/if (!${Me.AltAbilityReady[${buffName}]}) {
					/if (${Debug}) /echo |- CombatBuffs -| Alternate Ability: [${buffName}] is not ready.  Skipping.
				} else {
					/if (!${NetBots[${buffTarget}].Buff.Find[${Spell[${Me.AltAbility[${buffName}].Spell}].ID}]}) {
						/if (${Debug}) /echo |- CombatBuffs -| Casting AltAbility: [${buffName}] on [${Spawn[pc ${buffTarget}.CleanName}]
						/call e3_Cast ${Me.ID} "${buffName}" alt
						/RETURN
					}
				}

		|----------------------------------------------------------------------------------------------------------------------------------------BAD_ENTRY.	
			} else /if (${castType.Equal[BAD_ENTRY]}) {
				/if (${Debug}) /echo ERROR: [@Combat Buff#${i}] [${CombatBuffs[${i}]}] has been flagged as a BAD_ENTRY.  Skipping.
			}
		}

	/next i

/if (${Debug}) /echo <== CombatBuffs -|
/RETURN



|----------------------------------------------------|
|- Command group leaders to request group buffs.	-|
|----------------------------------------------------|
#event BuffAll "<#*#> Buff All"
SUB event_BuffAll
/if (${Debug}) /echo |- EVENT_BuffAll ==>

	/if (${Select[${Group.Leader},${Me}]}) /bc Buff me

/if (${Debug}) /echo <== EVENT_BuffAll -|
/RETURN



|--------------------------------------------------------|
|- Relays buff requests from /tell and /group to EQBC.	-|
|--------------------------------------------------------|
|- @param line: 'Name tells you, 'Buff me'.'			-|
|- @param buffTarget: The target to cast buffs on.		-|
|--------------------------------------------------------|
#event buffRequestRelay "#1# tells you, 'Buffme'"
#event buffRequestRelay "#1# tells you, 'Buff me'"
#event buffRequestRelay "#1# tells you, 'Buffpet'"
#event buffRequestRelay "#1# tells you, 'Buff pet'"
#event buffRequestRelay "#1# tells you, 'Buff my pet'"
SUB event_buffRequestRelay(line, buffTarget)
/if (${Debug}) /echo |- event_buffRequestRelay ==>

|--------------------------------------------------------------------------------------Clean up buffTarget variable.
	/if (${buffTarget.Left[1].Compare[ ]}<0) /varset buffTarget ${buffTarget.Right[-2].Left[-1]}

|------------------------------------------------------------------------------------------------Check if buffTarget is in zone, and within range.	
	/if (${SpawnCount[pc ${Spawn[${buffTarget}].CleanName}]} && ${Spawn[id ${buffTarget}].Distance}<=200) {
		
	|-------------------------------------------------------------------------------------------------------------------If the buffTarget is 'me'.
		/if (${line.Find[Buffme]} || ${line.Find[Buff me]}) {
			/varset buffTarget ${Spawn[pc ${buffTarget}].ID}
			
	|------------------------------------------------------------------------------------------------------------Else, if the buffTarget is 'pet'.
		} else /if (${line.Find[Buffpet]} || ${line.Find[Buff pet]} || ${line.Find[Buff my pet]}) {
			/varset buffTarget ${Spawn[pet ${Spawn[pc ${buffTarget}].Pet}].ID}
		}
		
	|--------------------------------------------------------------------------------If I'm assisting, wait to buff.
		/if (${Assisting}) {
			/if (${SpawnCount[pc ${Spawn[id ${buffTarget}].CleanName}]}) /tell ${Spawn[id ${buffTarget}].CleanName} I'm busy, try again in a few minutes.
		} else {
		
	|-----------------------------------------------------Output to buffTarget, and relay buff request through EQBC.
		
		|-----------------------------------------------------------------------------------------------------------------If I have buffs to cast.
			/if (${GroupBuffs.Size}) {
				/if (${SpawnCount[pc ${Spawn[id ${buffTarget}].CleanName}]}) /tell ${Spawn[id ${buffTarget}].CleanName} Casting buffs on ${Spawn[id ${buffTarget}].CleanName}...
				/bc Buffs on ${buffTarget}
				
		|-------------------------------------------------------------------------------------------------If I don't have buffs, but others might.
			} else /if (!${GroupBuffs.Size} && ${NetBots.Counts}>1) {
				/if (${SpawnCount[pc ${Spawn[id ${buffTarget}].CleanName}]}) /tell ${Spawn[id ${buffTarget}].CleanName} Requesting buffs on ${Spawn[id ${buffTarget}].CleanName}...
				/bc Buffs on ${buffTarget}
				
		|-------------------------------------------------------------------------------------------If I'm not a buffer, and no other bots are on.
			} else /if (!${GroupBuffs.Size} && ${NetBots.Counts}<=1) {
				/if (${SpawnCount[pc ${Spawn[id ${buffTarget}].CleanName}]}) /tell ${Spawn[id ${buffTarget}].CleanName} Sorry, I'm not currently set up to cast any buffs, try again later. (This is an automated response)
			}
		}	
	}
	
/if (${Debug}) /echo <== event_buffRequestRelay -|
/RETURN



|------------------------------------------------------------|
|- Casts GroupBuffs array on the specified buffTarget.	-|
|------------------------------------------------------------|
|- @param line: '<Name> Buff me'.							-|
|- @param ChatSender: The buff requestor.					-|
|- @param buffTarget: The target to cast buffs on.			-|
|------------------------------------------------------------|
#event castGroupBuffs "<#1#> Buffme"
#event castGroupBuffs "<#1#> Buff me"
#event castGroupBuffs "<#1#> Buffpet"
#event castGroupBuffs "<#1#> Buff pet"
#event castGroupBuffs "<#1#> Buff my pet"
#event castGroupBuffs "<#1#> Buffs on #2#"
SUB event_castGroupBuffs(line, ChatSender, int buffTarget)
/if (${Debug}) /echo |- castGroupBuffs ==>

|-------------------------------------------------------------------------------------------If I have buffs to cast.
	/if (${GroupBuffs.Size}) {
	
	|-------------------------------------------------------------------------If buffTarget is undefined, check the event line to see who to buff.
		/if (!${Defined[buffTarget]}) {
			/declare buffTarget int local
			
		|-----------------------------------------------------------------------------If the buffTarget is 'me'.
			/if (${line.Find[Buffme]} || ${line.Find[Buff me]}) {
				/varset buffTarget ${Spawn[pc ${ChatSender}].ID}
				
		|----------------------------------------------------------------------Else, if the buffTarget is 'pet'.
			} else /if (${line.Find[Buffpet]} || ${line.Find[Buff pet]} || ${line.Find[Buff my pet]}) {
				/varset buffTarget ${Spawn[pet ${Spawn[pc ${ChatSender}].Pet}].ID}
			}
		}
		
	|-------------------------------------------------------------------------------------------------Check if the ChatSender is in the same zone.	
		/if (${SpawnCount[pc ${ChatSender}]}) {
			
		|---------------------------------------------------------------Alert user that you're buffing.
			/if (${Verbosity}) /docommand ${ChatToggle} Buffing ${Spawn[id ${buffTarget}].CleanName}
			
		|--------------------------------------------------------------------------------------------------Cast GroupBuffs array, on buffTarget.
			/declare i int local
			
			/for i 1 to ${GroupBuffs.Size}
				/if (${Me.Book[${GroupBuffs[${i}].Arg[1,/]}]}) {
					/if (${Debug}) /echo |- GroupBuffs -| Casting Spell: [${GroupBuffs[${i}].Arg[1,/]}] on [${buffTarget}]
					/call e3_Cast ${buffTarget} "${GroupBuffs[${i}].Arg[1,/]}" gem${DefaultGem} 300
				} else /if (${GroupBuffs[${i}].Arg[2,/].Equal[item]} && ${Find}) {
					/if (${Debug}) /echo |- GroupBuffs -| Casting Spell: [${GroupBuffs[${i}].Arg[1,/]}] on [${buffTarget}]
					/call e3_Cast ${buffTarget} "${GroupBuffs[${i}].Arg[1,/]}" item
				} else /if (${GroupBuffs[${i}].Arg[2,/].Equal[alt]} || ${GroupBuffs[${i}].Arg[2,/].Equal[aa]}) {
					/if (${Debug}) /echo |- GroupBuffs -| Casting Spell: [${GroupBuffs[${i}].Arg[1,/]}] on [${buffTarget}]
					/call e3_Cast ${buffTarget} "${GroupBuffs[${i}].Arg[1,/]}" alt
				}
			/next i
			/docommand ${ChatToggle} Done buffing ${Spawn[id ${buffTarget}].CleanName}.
		}
	}
	
/if (${Debug}) /echo <== castGroupBuffs -|
/RETURN



|----------------------------|
|- Setup buffing variables.	-|
|----------------------------|
SUB setup_Buffs
/if (${Debug}) /echo |- setup_Buffs ==>

|--------------------------------------------------------------------------------Set Validation variables
	
	/declare i int local
	
|------------------------------------------------------------------------------------Adjust MaxBuffs to account for the Mystical Attuning AAs.
	/declare MaxBuffs int outer 0
	/if (${Me.AltAbility[Mystical Attuning]}) /varcalc MaxBuffs ${Me.AltAbility[Mystical Attuning]} / -5 + 1
	
|-------------------------------------------------------------------------------------------------------------------------------------Check the Character_Ini for elements within the [Buffs] section.
	/if (${Ini[${Character_Ini},Buffs].Length}) {

	|----------------------------------------------------------------------------------------------------------------Check for SelfBuffs.
		/varcalc Validation_Arg ${Validation_Arg} + 1
		:Validate_SelfBuffs
		/varset Validation_Count 0
		
		/if (${Ini[${Character_Ini},Buffs,Self Buff#1].Length}) /call INItoArray "${Character_Ini},Buffs,Self Buff#" SelfBuffs
		
		/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length} || ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${Validation_Arg},-]} != ${Validation_Count}) /varset Validation_On TRUE
		
		/if (${Validation_On}) {
			/if (${SelfBuffs.Size}) {
				
				/for i 1 to ${SelfBuffs.Size}
				
				|------------------------------------------------------------------------------------------------------------Auto-fix castType
					/if (!${SelfBuffs[${i}].Arg[2,/].Length}) {
						/echo [Self Buff#${i}] - [${SelfBuffs[${i}]}] does not have a specified castType.  Attempting to fix...
						
						/if (${Int[${Me.Book[${SelfBuffs[${i}]}]}]}) {
							/varset SelfBuffs[${i}] ${SelfBuffs[${i}]}/gem${If[${Me.Gem[${SelfBuffs[${i}]}]},${Me.Gem[${SelfBuffs[${i}]}]},${DefaultGem}]}
							/call WriteToIni "${Character_Ini},Buffs,Self Buff#${i}" "${SelfBuffs[${i}]}" 1
							/echo [${SelfBuffs[${i}]}] was found in my spellbook.  Setting [${SelfBuffs[${i}].Arg[1,/]}] to cast from [gem${DefaultGem}]
						} else /if (${FindItemCount[${SelfBuffs[${i}]}]}) {
							/varset SelfBuffs[${i}] ${SelfBuffs[${i}]}/item
							/call WriteToIni "${Character_Ini},Buffs,Self Buff#${i}" "${SelfBuffs[${i}]}" 1
							/echo [${SelfBuffs[${i}]}] was found in my inventory.  Setting [${SelfBuffs[${i}].Arg[1,/]}] to [item]
						} else /if (${Me.AltAbility[${SelfBuffs[${i}]}]}) {
							/varset SelfBuffs[${i}] ${SelfBuffs[${i}]}/alt
							/call WriteToIni "${Character_Ini},Buffs,Self Buff#${i}" "${SelfBuffs[${i}]}" 1
							/echo [${SelfBuffs[${i}]}] was found in my Alternate Abilities.  Setting [${SelfBuffs[${i}].Arg[1,/]}] to [alt]
						} else {
							/varset SelfBuffs[${i}] ${SelfBuffs[${i}]}/BAD_ENTRY
							/call WriteToIni "${Character_Ini},Buffs,Self Buff#${i}" "${SelfBuffs[${i}]}" 1
							/echo Could not find a compatible castType for [${SelfBuffs[${i}]}].  Flagging [Self Buff#${i}] as a BAD_ENTRY.
						}
					}
					
				|-----------------------------------------------------------------------------------------------------------Validate SelfBuffs array.
					/if (${SelfBuffs[${i}].Arg[2,/].Find[gem]}) {
						/if (!${Int[${Me.Book[${SelfBuffs[${i}].Arg[1,/]}]}]}) {
							/varset SelfBuffs[${i}] ${SelfBuffs[${i}].Arg[1,/]}/BAD_ENTRY
							/call WriteToIni "${Character_Ini},Buffs,Self Buff#${i}" "${SelfBuffs[${i}]}" 1
							/echo ERROR: @[Self Buff#${i}] - [${SelfBuffs[${i}].Arg[1,/]}] was not found in my spellbook.  Flagging [Self Buff#${i}] as a BAD_ENTRY.
						}
					} else /if (${SelfBuffs[${i}].Arg[2,/].Equal[item]}) {
						/if (!${FindItemCount[${SelfBuffs[${i}].Arg[1,/]}]}) {
							/varset SelfBuffs[${i}] ${SelfBuffs[${i}].Arg[1,/]}/BAD_ENTRY
							/call WriteToIni "${Character_Ini},Buffs,Self Buff#${i}" "${SelfBuffs[${i}]}" 1
							/echo ERROR: @[Self Buff#${i}] - I do not have [${SelfBuffs[${i}].Arg[1,/]}] in my inventory.  Flagging [Self Buff#${i}] as a BAD_ENTRY.
						}
					} else /if (${SelfBuffs[${i}].Arg[2,/].Equal[alt]} || ${SelfBuffs[${i}].Arg[2,/].Equal[aa]}) {
						/if (!${Me.AltAbility[${SelfBuffs[${i}].Arg[1,/]}].ID}) {
							/varset SelfBuffs[${i}] ${SelfBuffs[${i}].Arg[1,/]}/BAD_ENTRY
							/call WriteToIni "${Character_Ini},Buffs,Self Buff#${i}" "${SelfBuffs[${i}]}" 1
							/echo ERROR: @[Self Buff#${i}] - I do not have any ranks in [${SelfBuffs[${i}].Arg[1,/]}].  Flagging [Self Buff#${i}] as a BAD_ENTRY.
						}			
					} else /if (${SelfBuffs[${i}].Arg[2,/].Find[BAD_ENTRY]}) {
							/echo ERROR: @[Self Buff#${i}] - There is a problem with the user entry for [${SelfBuffs[${i}].Arg[1,/]}] in [${Character_Ini}].  Please review this entry, and restart the macro.
					} else {
						/varset SelfBuffs[${i}] ${SelfBuffs[${i}].Arg[1,/]}/BAD_ENTRY
						/call WriteToIni "${Character_Ini},Buffs,Self Buff#${i}" "${SelfBuffs[${i}]}" 1
						/echo ERROR: @[Self Buff#${i}] - [${SelfBuffs[${i}].Arg[2,/]}] is not an acceptable castType.  Flagging [Self Buff#${i}] as a BAD_ENTRY.
					}

				/next i
			}

			/call rewrite_ValidationKey ${Validation_Count} ${Validation_Arg}
			
			/varset Validation_On FALSE
			/goto :Validate_SelfBuffs
		}

	|----------------------------------------------------------------------------------------------------------------Check for BuffOthers.
		/varcalc Validation_Arg ${Validation_Arg} + 1
		:Validate_BuffOthers
		/varset Validation_Count 0
		
		/if (${Ini[${Character_Ini},Buffs,Buff Other#1].Length}) /call INItoArray "${Character_Ini},Buffs,Buff Other#" BuffOthers
		
		/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length} || ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${Validation_Arg},-]} != ${Validation_Count}) /varset Validation_On TRUE
	
		/if (${Validation_On}) {
			/if (${BuffOthers.Size}) {
				
				/for i 1 to ${BuffOthers.Size}
				|------------------------------------------------------------------------------------------------------------Check for Target and spell to cast.
					/if (!${BuffOthers[${i}].Arg[2,/].Length}) {
						/varset BuffOthers[${i}] ${BuffOthers[${i}]}/BAD_ENTRY
						/call WriteToIni "${Character_Ini},Buffs,Buff Other#${i}" "${BuffOthers[${i}]}" 1
						/echo [Buff Other#${i}] - [${BuffOthers[${i}]}]. You must specify a Target, and a buff to cast; ex Buff Other#1=Killians/Spirit of Eagle.    Flagging [Buff Other#${i}] as a BAD_ENTRY.
					} else {
					
					|------------------------------------------------------------------------------------------------------------Auto-fix castType
						/if (!${BuffOthers[${i}].Arg[3,/].Length}) {
							/echo [Buff Other#${i}] - [${BuffOthers[${i}]}] does not have a specified castType.  Attempting to fix...
							
							/if (${Int[${Me.Book[${BuffOthers[${i}].Arg[2,/]}]}]}) {
								/varset BuffOthers[${i}] ${BuffOthers[${i}]}/gem${If[${Me.Gem[${BuffOthers[${i}].Arg[2,/]}]},${Me.Gem[${BuffOthers[${i}].Arg[2,/]}]},${DefaultGem}]}
								/call WriteToIni "${Character_Ini},Buffs,Buff Other#${i}" "${BuffOthers[${i}]}" 1
								/echo [${BuffOthers[${i}]}] was found in my spellbook.  Setting [${BuffOthers[${i}].Arg[1,/]}] to cast from [gem${DefaultGem}]
							} else /if (${FindItemCount[${BuffOthers[${i}].Arg[2,/]}]}) {
								/varset BuffOthers[${i}] ${BuffOthers[${i}]}/item
								/call WriteToIni "${Character_Ini},Buffs,Buff Other#${i}" "${BuffOthers[${i}]}" 1
								/echo [${BuffOthers[${i}]}] was found in my inventory.  Setting [${BuffOthers[${i}].Arg[1,/]}] to [item]
							} else /if (${Me.AltAbility[${BuffOthers[${i}].Arg[2,/]}]}) {
								/varset BuffOthers[${i}] ${BuffOthers[${i}]}/alt
								/call WriteToIni "${Character_Ini},Buffs,Buff Other#${i}" "${BuffOthers[${i}]}" 1
								/echo [${BuffOthers[${i}]}] was found in my Alternate Abilities.  Setting [${BuffOthers[${i}].Arg[1,/]}] to [alt]
							} else {
								/varset BuffOthers[${i}] ${BuffOthers[${i}]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Buffs,Buff Other#${i}" "${BuffOthers[${i}]}" 1
								/echo Could not find a compatible castType for [${BuffOthers[${i}]}].  Flagging [Buff Other#${i}] as a BAD_ENTRY.
							}
						}
						
					|-----------------------------------------------------------------------------------------------------------Validate BuffOthers array.
						/if (${BuffOthers[${i}].Arg[3,/].Find[gem]}) {
							/if (!${Int[${Me.Book[${BuffOthers[${i}].Arg[2,/]}]}]}) {
								/varset BuffOthers[${i}] ${BuffOthers[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Buffs,Buff Other#${i}" "${BuffOthers[${i}]}" 1
								/echo ERROR: @[Buff Other#${i}] - [${BuffOthers[${i}].Arg[1,/]}] was not found in my spellbook.  Flagging [Buff Other#${i}] as a BAD_ENTRY.
							}
						} else /if (${BuffOthers[${i}].Arg[3,/].Equal[item]}) {
							/if (!${FindItem[${BuffOthers[${i}].Arg[2,/]}].ID}) {
								/varset BuffOthers[${i}] ${BuffOthers[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Buffs,Buff Other#${i}" "${BuffOthers[${i}]}" 1
								/echo ERROR: @[Buff Other#${i}] - I do not have [${BuffOthers[${i}].Arg[1,/]}] in my inventory.  Flagging [Buff Other#${i}] as a BAD_ENTRY.
							}
						} else /if (${BuffOthers[${i}].Arg[2,/].Equal[alt]} || ${BuffOthers[${i}].Arg[2,/].Equal[aa]}) {
							/if (!${Me.AltAbility[${BuffOthers[${i}].Arg[1,/]}].ID}) {
								/varset BuffOthers[${i}] ${BuffOthers[${i}].Arg[1,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Buffs,Buff Other#${i}" "${BuffOthers[${i}]}" 1
								/echo ERROR: @[Buff Other#${i}] - I do not have any ranks in [${BuffOthers[${i}].Arg[1,/]}].  Flagging [Buff Other#${i}] as a BAD_ENTRY.
							}			
						} else /if (${BuffOthers[${i}].Arg[2,/].Find[BAD_ENTRY]}) {
							/echo ERROR: @[Buff Other#${i}] - There is a problem with the user entry for [${BuffOthers[${i}].Arg[1,/]}] in [${Character_Ini}].  Please review this entry, and restart the macro.
						} else {
							/varset BuffOthers[${i}] ${BuffOthers[${i}].Arg[1,/]}/BAD_ENTRY
							/call WriteToIni "${Character_Ini},Buffs,Buff Other#${i}" "${BuffOthers[${i}]}" 1
							/echo ERROR: @[Buff Other#${i}] - [${BuffOthers[${i}].Arg[2,/]}] is not an acceptable castType.  Flagging [Buff Other#${i}] as a BAD_ENTRY.
						}
					}	

				/next i
			}

			/call rewrite_ValidationKey ${Validation_Count} ${Validation_Arg}
			
			/varset Validation_On FALSE
			/goto :Validate_BuffOthers
		}

	|----------------------------------------------------------------------------------------------------------------Check for CombatBuffs.
		/varcalc Validation_Arg ${Validation_Arg} + 1
		:Validate_CombatBuffs
		/varset Validation_Count 0
		
		/if (${Ini[${Character_Ini},Buffs,Combat Buff#1].Length}) /call INItoArray "${Character_Ini},Buffs,Combat Buff#" CombatBuffs
		
		/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length} || ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${Validation_Arg},-]} != ${Validation_Count}) /varset Validation_On TRUE

		/if (${Validation_On}) {
			/if (${CombatBuffs.Size}) {
			
				/for i 1 to ${CombatBuffs.Size}
				|------------------------------------------------------------------------------------------------------------Check for Target and spell to cast.
					/if (!${CombatBuffs[${i}].Arg[2,/].Length}) {
						/echo ERROR: @[Combat Buff#${i}] - [${CombatBuffs[${i}]}]. You must specify a Target, and a buff to cast; ex Combat Buff#1=Killians/Spirit of Eagle.
						/call WriteToIni "${Character_Ini},Buffs,Combat Buff#${i}" "${CombatBuffs[${i}]}/PLACEHOLDER/BAD_ENTRY" 1
					} else {
					
					|------------------------------------------------------------------------------------------------------------Auto-fix castType
						/if (!${CombatBuffs[${i}].Arg[3,/].Length}) {
							/echo [Combat Buff#${i}] - [${CombatBuffs[${i}]}] does not have a specified castType.  Attempting to fix...
							
							/if (${Int[${Me.Book[${CombatBuffs[${i}].Arg[2,/]}]}]}) {
								/varset CombatBuffs[${i}] ${CombatBuffs[${i}]}/gem${If[${Me.Gem[${CombatBuffs[${i}].Arg[2,/]}]},${Me.Gem[${CombatBuffs[${i}].Arg[2,/]}]},${DefaultGem}]}
								/call WriteToIni "${Character_Ini},Buffs,Combat Buff#${i}" "${CombatBuffs[${i}]}" 1
								/echo [${CombatBuffs[${i}]}] was found in my spellbook.  Setting [${CombatBuffs[${i}].Arg[2,/]}] to cast from [gem${DefaultGem}]
							} else /if (${FindItemCount[${CombatBuffs[${i}].Arg[2,/]}]}) {
								/varset CombatBuffs[${i}] ${CombatBuffs[${i}]}/item
								/call WriteToIni "${Character_Ini},Buffs,Combat Buff#${i}" "${CombatBuffs[${i}]}" 1
								/echo [${CombatBuffs[${i}]}] was found in my inventory.  Setting [${CombatBuffs[${i}].Arg[2,/]}] to [item]
							} else /if (${Me.AltAbility[${CombatBuffs[${i}].Arg[2,/]}]}) {
								/varset CombatBuffs[${i}] ${CombatBuffs[${i}]}/alt
								/call WriteToIni "${Character_Ini},Buffs,Combat Buff#${i}" "${CombatBuffs[${i}]}" 1
								/echo [${CombatBuffs[${i}]}] was found in my Alternate Abilities.  Setting [${CombatBuffs[${i}].Arg[2,/]}] to [alt]
							} else {
								/varset CombatBuffs[${i}] ${CombatBuffs[${i}]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Buffs,Combat Buff#${i}" "${CombatBuffs[${i}]}" 1
								/echo Could not find a compatible castType for [${CombatBuffs[${i}]}].  Flagging [Combat Buff#${i}] as a BAD_ENTRY.
							}
						}
						
					|-----------------------------------------------------------------------------------------------------------Validate CombatBuffs array.
						/if (${CombatBuffs[${i}].Arg[3,/].Find[gem]}) {
							/if (!${Int[${Me.Book[${CombatBuffs[${i}].Arg[2,/]}]}]}) {
								/varset CombatBuffs[${i}] ${CombatBuffs[${i}].Arg[1,/]}/${CombatBuffs[${i}].Arg[2,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Buffs,Combat Buff#${i}" "${CombatBuffs[${i}]}" 1
								/echo ERROR: @[Combat Buff#${i}] - [${CombatBuffs[${i}].Arg[1,/]}] was not found in my spellbook.  Flagging [Combat Buff#${i}] as a BAD_ENTRY.
							}
						} else /if (${CombatBuffs[${i}].Arg[3,/].Equal[item]}) {
							/if (!${FindItem[${CombatBuffs[${i}].Arg[2,/]}].ID}) {
								/varset CombatBuffs[${i}] ${CombatBuffs[${i}].Arg[1,/]}/${CombatBuffs[${i}].Arg[2,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Buffs,Combat Buff#${i}" "${CombatBuffs[${i}]}" 1
								/echo ERROR: @[Combat Buff#${i}] - I do not have [${CombatBuffs[${i}].Arg[1,/]}] in my inventory.  Flagging [Combat Buff#${i}] as a BAD_ENTRY.
							}
						} else /if (${CombatBuffs[${i}].Arg[3,/].Equal[alt]} || ${CombatBuffs[${i}].Arg[3,/].Equal[aa]}) {
							/if (!${Me.AltAbility[${CombatBuffs[${i}].Arg[2,/]}].ID}) {
								/varset CombatBuffs[${i}] ${CombatBuffs[${i}].Arg[1,/]}/${CombatBuffs[${i}].Arg[2,/]}/BAD_ENTRY
								/call WriteToIni "${Character_Ini},Buffs,Combat Buff#${i}" "${CombatBuffs[${i}]}" 1
								/echo ERROR: @[Combat Buff#${i}] - I do not have any ranks in [${CombatBuffs[${i}].Arg[2,/]}].  Flagging [Combat Buff#${i}] as a BAD_ENTRY.
							}			
						} else /if (${CombatBuffs[${i}].Arg[3,/].Find[BAD_ENTRY]}) {
							/echo ERROR: @[Combat Buff#${i}] - There is a problem with the user entry for [${CombatBuffs[${i}].Arg[1,/]}] in [${Character_Ini}].  Please review this entry, and restart the macro.
						} else {
							/varset CombatBuffs[${i}] ${CombatBuffs[${i}].Arg[1,/]}/${CombatBuffs[${i}].Arg[2,/]}/BAD_ENTRY
							/call WriteToIni "${Character_Ini},Buffs,Combat Buff#${i}" "${CombatBuffs[${i}]}" 1
							/echo ERROR: @[Combat Buff#${i}] - [${CombatBuffs[${i}].Arg[2,/]}] is not an acceptable castType.  Flagging [Combat Buff#${i}] as a BAD_ENTRY.
						}
					}
				/next i
			
			}

			/call rewrite_ValidationKey ${Validation_Count} ${Validation_Arg}
			
			/varset Validation_On FALSE
			/goto :Validate_CombatBuffs
		}	

	|----------------------------------------------------------------------------------------------------------------Check for GroupBuffs.
		/varcalc Validation_Arg ${Validation_Arg} + 1
		:Validate_GroupBuffs
		/varset Validation_Count 0
	
		/if (${Ini[${Character_Ini},Buffs,Group Buff#1].Length}) /call INItoArray "${Character_Ini},Buffs,Group Buff#" GroupBuffs
		
		/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length} || ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${Validation_Arg},-]} != ${Validation_Count}) /varset Validation_On TRUE

		/if (${Validation_On}) {
			/if (${GroupBuffs.Size}) {

				/for i 1 to ${GroupBuffs.Size}
				
				|------------------------------------------------------------------------------------------------------------Auto-fix castType
					/if (!${GroupBuffs[${i}].Arg[2,/].Length}) {
						/echo [Group Buff#${i}] - [${GroupBuffs[${i}]}] does not have a specified castType.  Attempting to fix...
						
						/if (${Int[${Me.Book[${GroupBuffs[${i}]}]}]}) {
							/varset GroupBuffs[${i}] ${GroupBuffs[${i}]}/gem${If[${Me.Gem[${GroupBuffs[${i}]}]},${Me.Gem[${GroupBuffs[${i}]}]},${DefaultGem}]}
							/call WriteToIni "${Character_Ini},Buffs,Group Buff#${i}" "${GroupBuffs[${i}]}" 1
							/echo [${GroupBuffs[${i}]}] was found in my spellbook.  Setting [${GroupBuffs[${i}].Arg[1,/]}] to cast from [gem${DefaultGem}]
						} else /if (${FindItemCount[${GroupBuffs[${i}]}]}) {
							/varset GroupBuffs[${i}] ${GroupBuffs[${i}]}/item
							/call WriteToIni "${Character_Ini},Buffs,Group Buff#${i}" "${GroupBuffs[${i}]}" 1
							/echo [${GroupBuffs[${i}]}] was found in my inventory.  Setting [${GroupBuffs[${i}].Arg[1,/]}] to [item]
						} else /if (${Me.AltAbility[${GroupBuffs[${i}]}]}) {
							/varset GroupBuffs[${i}] ${GroupBuffs[${i}]}/alt
							/call WriteToIni "${Character_Ini},Buffs,Group Buff#${i}" "${GroupBuffs[${i}]}" 1
							/echo [${GroupBuffs[${i}]}] was found in my Alternate Abilities.  Setting [${GroupBuffs[${i}].Arg[1,/]}] to [alt]
						} else {
							/varset GroupBuffs[${i}] ${GroupBuffs[${i}]}/BAD_ENTRY
							/call WriteToIni "${Character_Ini},Buffs,Group Buff#${i}" "${GroupBuffs[${i}]}" 1
							/echo Could not find a compatible castType for [${GroupBuffs[${i}]}].  Flagging [Group Buff#${i}] as a BAD_ENTRY.
						}
					}
				
				|-----------------------------------------------------------------------------------------------------------Validate GroupBuffs array.
					/if (${GroupBuffs[${i}].Arg[2,/].Find[gem]}) {
						/if (!${Int[${Me.Book[${GroupBuffs[${i}].Arg[1,/]}]}]}) {
							/varset GroupBuffs[${i}] ${GroupBuffs[${i}].Arg[1,/]}/BAD_ENTRY
							/call WriteToIni "${Character_Ini},Buffs,Group Buff#${i}" "${GroupBuffs[${i}]}" 1
							/echo ERROR: @[Group Buff#${i}] - [${GroupBuffs[${i}].Arg[1,/]}] was not found in my spellbook.  Flagging [Group Buff#${i}] as a BAD_ENTRY.
						}
					} else /if (${GroupBuffs[${i}].Arg[2,/].Equal[item]}) {
						/if (!${FindItemCount[${GroupBuffs[${i}].Arg[1,/]}]}) {
							/varset GroupBuffs[${i}] ${GroupBuffs[${i}].Arg[1,/]}/BAD_ENTRY
							/call WriteToIni "${Character_Ini},Buffs,Group Buff#${i}" "${GroupBuffs[${i}]}" 1
							/echo ERROR: @[Group Buff#${i}] - I do not have [${GroupBuffs[${i}].Arg[1,/]}] in my inventory.  Flagging [Group Buff#${i}] as a BAD_ENTRY.
						}
					} else /if (${GroupBuffs[${i}].Arg[2,/].Equal[alt]} || ${GroupBuffs[${i}].Arg[2,/].Equal[aa]}) {
						/if (!${Me.AltAbility[${GroupBuffs[${i}].Arg[1,/]}].ID}) {
							/varset GroupBuffs[${i}] ${GroupBuffs[${i}].Arg[1,/]}/BAD_ENTRY
							/call WriteToIni "${Character_Ini},Buffs,Group Buff#${i}" "${GroupBuffs[${i}]}" 1
							/echo ERROR: @[Group Buff#${i}] - I do not have any ranks in [${GroupBuffs[${i}].Arg[1,/]}].  Flagging [Group Buff#${i}] as a BAD_ENTRY.
						}			
					} else /if (${GroupBuffs[${i}].Arg[2,/].Find[BAD_ENTRY]}) {
							/echo ERROR: @[Group Buff#${i}] - There is a problem with the user entry for [${GroupBuffs[${i}].Arg[1,/]}] in [${Character_Ini}].  Please review this entry, and restart the macro.
					} else {
						/varset GroupBuffs[${i}] ${GroupBuffs[${i}].Arg[1,/]}/BAD_ENTRY
						/call WriteToIni "${Character_Ini},Buffs,Group Buff#${i}" "${GroupBuffs[${i}]}" 1
						/echo ERROR: @[Group Buff#${i}] - [${GroupBuffs[${i}].Arg[2,/]}] is not an acceptable castType.  Flagging [Group Buff#${i}] as a BAD_ENTRY.
					}
					
				/next i
			}

			/call rewrite_ValidationKey ${Validation_Count} ${Validation_Arg}
			
			/varset Validation_On FALSE
			/goto :Validate_GroupBuffs
		}		
	}
	
|---------------------------------------------------------------------------------------------------------------------Constant Illusion variables.
	/call IniToVar "${Character_Ini},Buffs,Instant Illusion" ConstantIllusion string outer
	/if (${Defined[ConstantIllusion]}) {
		/declare illusionRace string outer ${FindItem[${ConstantIllusion}].Spell}
		/varset illusionRace ${illusionRace.Replace[Illusion: ,]}
		/if (${illusionRace.Equal[Guktan]}) /varset illusionRace Froglok
	}
	
|-------------------------------------------------------------------------------------------------------------------Setup BuffCheck variables.
	/if (${SelfBuffs.Size} || ${BuffOthers.Size} || ${ConstantIllusion.Length}) {
		/declare BuffCheck bool outer TRUE
		/call IniToVar "${Macro_Ini},Buffs,Allow Buff Requests (On/Off/PWOnly)" allow_BuffRequests ${If[${Ini[${Macro_Ini},Buffs,Allow Buff Requests (On/Off/PWOnly)].Equal[pwonly]},string,bool]} outer
		/call IniToVar "${Macro_Ini},Buffs,BuffCheck Interval (Sec)" buffCheck_Interval int outer
	}

/if (${Debug}) /echo <== setup_Buffs -|
/RETURN