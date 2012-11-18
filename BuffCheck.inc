|----------------------------------------------------------------------------|
|- e3_Macro_BuffCheck.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Casts and maintains buff spells on you and your NetBots.					-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------------------|
|- buffCheck's mainloop hook.													-|
|--------------------------------------------------------------------------------|
|- Calls buff functions, returning to the mainloop whenever a spell is cast.	-|
|--------------------------------------------------------------------------------|
SUB buffCheck
/if (${Debug}) /echo |- buffCheck ==>

	/if (${Cursor.ID} || ${Corpse.Open}) {
		/varset rebuffTimer ${buffCheck_Interval}
	} else {
		/if (!${Me.Combat} && !${Me.Feigning} && !${Me.Moving} && !${Assisting} && ${Me.PctHPs} > 70 && ${Select[${Cursor.ID},NULL]}) {
		
		|--------------------------------------------------------------------------------------SelfBuffs.
			/if (${Me.FreeBuffSlots} > ${MaxBuffs}) /call buffSelf
			/if (${ActionTaken}) /RETURN
		
		|--------------------------------------------------------------------------------------BuffOthers.
			/if (${BuffTargets.Size}) /call buffOthers
			/if (${ActionTaken}) /RETURN
			
		|----------------------------------------------------------If all buffs are cast, set rebuffTimer.
			/varset rebuffTimer ${buffCheck_Interval}
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
	/for i 1 to ${MyBuffs.Size}

	|------------------------------------------------------------------------------------------------------------------------------------Cast a spell.
		/if (${MyBuffs[${i}].Arg[2,/].Left[3].Equal[gem]}) {
			/if (${Me.Book[${MyBuffs[${i}].Arg[1,/]}]}) {
			
			|---------------------------------------------------------------------------------------Check buff duration.
				/if (${Math.Calc[${Me.Buff[${MyBuffs[${i}].Arg[1,/]}].Duration}*6]} < 60 && ${Spell[${MyBuffs[${i}].Arg[1,/]}].Stacks}) {
					/if (${Debug}) /echo |- buffSelf -| Casting Spell: [${Spell[${MyBuffs[${i}].Arg[1,/]}]} -- ${MyBuffs[${i}].Arg[2,/]} -- ${If[${Me.Gem[${Spell[${MyBuffs[${i}]}]}]},25,90]}] on [${Me.ID}]
					/call e3_Cast ${Me.ID} "${MyBuffs[${i}].Arg[1,/]}" ${MyBuffs[${i}].Arg[2,/]} ${If[${Me.Gem[${Spell[${MyBuffs[${i}]}]}]},25,90]}
					/RETURN
				}
			}
			
	|------------------------------------------------------------------------------------------------------------------------------------Cast an item.
		} else /if (${MyBuffs[${i}].Arg[2,/].Equal[item]}) {
			/if (${FindItem[${MyBuffs[${i}].Arg[1,/]}].ID} && ${Math.Calc[${Me.Buff[${FindItem[${MyBuffs[${i}].Arg[1,/]}].Spell}].Duration}*6-12]} < ${buffCheck_Interval} && ${Spell[${FindItem[${MyBuffs[${i}].Arg[1,/]}].Spell}].Stacks}) {
				/if (${Debug}) /echo |- buffSelf -| Casting Item: [${MyBuffs[${i}].Arg[1,/]}] on [${If[${Spell[${FindItem[${MyBuffs[${i}].Arg[1,/]}].Spell}].TargetType.NotEqual[Self]},${Me.ID},0]}]
				/call e3_Cast ${Me.ID} "${MyBuffs[${i}].Arg[1,/]}" item
				/RETURN
			}
			
	|-----------------------------------------------------------------------------------------------------------------------------------------Cast AA.
		} else /if (${MyBuffs[${i}].Arg[2,/].Equal[alt]}) {
			/if (${Math.Calc[${Me.Buff[${MyBuffs[${i}].Arg[1,/]}].Duration}*6-12]} < ${buffCheck_Interval} && !${alt_Timer_${Me.AltAbility[${MyBuffs[${i}].Arg[1,/]}].ID}}) {
				/if (${Debug}) /echo |- buffSelf -| Casting AltAbility: [${MyBuffs[${i}].Arg[1,/]}] on [${If[${Spell[${MyBuffs[${i}].Arg[1,/]}].TargetType.NotEqual[Self]},${Me.ID},0]}]
				/call e3_Cast ${Me.ID} "${MyBuffs[${i}].Arg[1,/]}" alt
				/RETURN
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

	/for i 1 to ${BuffTargets.Size}
	
	|-------------------------------------------------------------------------------------------------------------------------If the spawn is in zone.
		/if (${SpawnCount[pc ${BuffTargets[${i}].Arg[1,/]}]} && ${NetBots[${BuffTargets[${i}].Arg[1,/]}].Zone} == ${Zone.ID}) {
		
		|----------------------------------------------------------Check for group only buffs.
			/if (${BuffTargets[${i}].Find[/group]}) {
				/for x 1 to ${Group}
					/varset groupList ${groupList}${Group.Member[${x}]}
				/next x
				/if (!${Select[${BuffTargets[${i}].Arg[1,/]},${groupList}]}) /next i
			}
		
		|--------------------------------------------------------------------------------------------------------------------------------Cast a spell.
			/if (${BuffTargets[${i}].Arg[3,/].Left[3].Equal[gem]}) {
			
			|----------------------------------------------------------------------------Check for buff via NetBots.
				/if (!${NetBots[${BuffTargets[${i}].Arg[1,/]}].Buff.Find[${Spell[${BuffTargets[${i}].Arg[2,/]}].ID}]}) {
					/if (${Debug}) /echo |- buffOthers -| Casting Spell: [${BuffTargets[${i}].Arg[2,/]}] on [${Spawn[pc ${BuffTargets[${i}].Arg[1,/]}].ID}]
					/call e3_Cast ${Spawn[pc ${BuffTargets[${i}].Arg[1,/]}].ID} "${BuffTargets[${i}].Arg[2,/]}" ${BuffTargets[${i}].Arg[3,/]} ${If[${Me.Gem[${BuffTargets[${i}].Arg[2,/]}]},25,90]}
					/RETURN
				}
			} else	/if (${BuffTargets[${i}].Arg[3,/].Equal[item]}) {
				/if (!${NetBots[${BuffTargets[${i}].Arg[1,/]}].Buff.Find[${Spell[${FindItem[${BuffTargets[${i}].Arg[2,/]}].Spell}].ID}]}) {
					/if (${Debug}) /echo |- buffOthers -| Casting Item: [${BuffTargets[${i}].Arg[2,/]}] on [${Spawn[pc ${BuffTargets[${i}].Arg[1,/]}].ID}]
					/call e3_Cast ${Spawn[pc ${BuffTargets[${i}].Arg[1,/]}].ID} "${BuffTargets[${i}].Arg[2,/]}" item
					/RETURN
				}
			} else /if (${MyBuffs[${i}].Arg[3,/].Equal[alt]}) {
				/if (!${NetBots[${BuffTargets[${i}].Arg[1,/]}].Buff.Find[${Spell[${Me.AltAbility[${BuffTargets[${i}].Arg[2,/]}].ID}].ID}]} && !${alt_Timer_${Me.AltAbility[${BuffTargets[${i}].Arg[2,/]}].ID}}) {
					/if (${Debug}) /echo |- buffOthers -| Casting AltAbility: [${BuffTargets[${i}].Arg[2,/]}] on [${Spawn[pc ${BuffTargets[${i}].Arg[1,/]}].ID}]
					/call e3_Cast ${Spawn[pc ${BuffTargets[${i}].Arg[1,/]}].ID} "${BuffTargets[${i}].Arg[2,/]}" alt
					/RETURN
				}
			}
		}
	/next i
	
/if (${Debug}) /echo <== buffOthers -|
/RETURN



|--------------------------------------------------------|
|- Cast and maintain high priority buffs on a NetBot.	-|
|--------------------------------------------------------|
SUB checkPriorityBuffs
/if (${Debug}) /echo |- CombatBuffs ==>

	/declare i int local
	/declare x int local
	/declare groupList string local

	/for i 1 to ${PriorityBuffs.Size}
	
	|-------------------------------------------------------------------------------------------------------------------------If the spawn is in zone.
		/if (${SpawnCount[pc ${PriorityBuffs[${i}].Arg[1,/]}]} && ${NetBots[${PriorityBuffs[${i}].Arg[1,/]}].Zone} == ${Zone.ID}) {
		
		|----------------------------------------------------------Check for group only buffs.
			/if (${PriorityBuffs[${i}].Find[/group]}) {
				/for x 1 to ${Group}
					/varset groupList ${groupList}${Group.Member[${x}]},
				/next x
				/if (!${Select[${PriorityBuffs[${i}].Arg[1,/]},${groupList}]}) /next i
			}
			
		|--------------------------------------------------------------------------------------------------------------------------------Cast a spell.
			/if (${PriorityBuffs[${i}].Arg[3,/].Left[3].Equal[gem]}) {
			
			|--------------------------------------------------------------------------------Check for buff via NetBots.
				/if (!${NetBots[${PriorityBuffs[${i}].Arg[1,/]}].Buff.Find[${Spell[${PriorityBuffs[${i}].Arg[2,/]}].ID}]}) {
					/if (${Debug}) /echo |- CombatBuffs -| Casting Spell: [${PriorityBuffs[${i}].Arg[2,/]}] on [${Spawn[pc ${PriorityBuffs[${i}].Arg[1,/]}].ID}]
					/call e3_Cast ${Spawn[pc ${PriorityBuffs[${i}].Arg[1,/]}].ID} "${PriorityBuffs[${i}].Arg[2,/]}" ${PriorityBuffs[${i}].Arg[3,/]} ${If[${Me.Gem[${BuffTargets[${i}].Arg[2,/]}]},25,90]}
					/RETURN
				}

		|--------------------------------------------------------------------------------------------------------------------------------Cast an item.			
			} else /if (${PriorityBuffs[${i}].Arg[3,/].Equal[item]}) {
				/if (!${NetBots[${PriorityBuffs[${i}].Arg[1,/]}].Buff.Find[${Spell[${FindItem[${PriorityBuffs[${i}].Arg[2,/]}].Spell}].ID}]}) {
					/if (${Debug}) /echo |- CombatBuffs -| Casting Item: [${PriorityBuffs[${i}].Arg[2,/]}] on [${Spawn[pc ${PriorityBuffs[${i}].Arg[1,/]}].ID}]
					/call e3_Cast ${Spawn[pc ${PriorityBuffs[${i}].Arg[1,/]}].ID} "${PriorityBuffs[${i}].Arg[2,/]}" item
					/RETURN
				}
				
		|-------------------------------------------------------------------------------------------------------------------------------------Cast AA.
			} else /if (${PriorityBuffs[${i}].Arg[3,/].Equal[AA]} || ${PriorityBuffs[${i}].Arg[3,/].Equal[Alt]}) {
				/if (!${NetBots[${PriorityBuffs[${i}].Arg[1,/]}].Buff.Find[${Me.AltAbility[${PriorityBuffs[${i}].Arg[2,/]}].ID}]} && !${alt_Timer_158}) {
					/if (${Debug}) /echo |- CombatBuffs -| Casting AltAbility: [${PriorityBuffs[${i}].Arg[2,/]}] on [${Spawn[pc ${PriorityBuffs[${i}].Arg[1,/]}].ID}]
					/call e3_Cast ${Spawn[pc ${PriorityBuffs[${i}].Arg[1,/]}].ID} "${PriorityBuffs[${i}].Arg[2,/]}" alt				
					/RETURN
				}
			}	
		}
	/next i

/if (${Debug}) /echo <== buffOthers -|
/RETURN



|----------------------------------------------------|
|- Command group leaders to request group buffs.	-|
|----------------------------------------------------|
#event BuffAll "<#*#> Buff All"
SUB event_BuffAll
/if (${Debug}) /echo |- buffOthers ==>

	/if (${Select[${Group.Leader},${Me}]}) {
		/bc Buff me
	}

/if (${Debug}) /echo <== buffOthers -|
/RETURN



|--------------------------------------------------------|
|- Relays buff requests from /tell and /group to EQBC.	-|
|--------------------------------------------------------|
|- @param line: 'Name tells you, 'Buff me'.'			-|
|- @param buffTarget: The target to cast buffs on.		-|
|--------------------------------------------------------|
#event groupBuffs "#1# tells you, 'Buffme'"
#event groupBuffs "#1# tells you, 'Buff me'"
#event groupBuffs "#1# tells you, 'Buffpet'"
#event groupBuffs "#1# tells you, 'Buff pet'"
#event groupBuffs "#1# tells you, 'Buff my pet'"
SUB event_groupBuffs(line, buffTarget)
/if (${Debug}) /echo |- event_groupBuffs ==>

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
			/if (${MyGroupBuffs.Size}) {
				/if (${SpawnCount[pc ${Spawn[id ${buffTarget}].CleanName}]}) /tell ${Spawn[id ${buffTarget}].CleanName} Casting buffs on ${Spawn[id ${buffTarget}].CleanName}...
				/bc Buffs on ${buffTarget}
				
		|-------------------------------------------------------------------------------------------------If I don't have buffs, but others might.
			} else /if (!${MyGroupBuffs.Size} && ${NetBots.Counts}>1) {
				/if (${SpawnCount[pc ${Spawn[id ${buffTarget}].CleanName}]}) /tell ${Spawn[id ${buffTarget}].CleanName} Requesting buffs on ${Spawn[id ${buffTarget}].CleanName}...
				/bc Buffs on ${buffTarget}
				
		|-------------------------------------------------------------------------------------------If I'm not a buffer, and no other bots are on.
			} else /if (!${MyGroupBuffs.Size} && ${NetBots.Counts}<=1) {
				/if (${SpawnCount[pc ${Spawn[id ${buffTarget}].CleanName}]}) /tell ${Spawn[id ${buffTarget}].CleanName} Sorry, I'm not currently set up to cast any buffs, try again later. (This is an automated response)
			}
		}	
	}
	
/if (${Debug}) /echo <== event_groupBuffs -|
/RETURN



|------------------------------------------------------------|
|- Casts MyGroupBuffs array on the specified buffTarget.	-|
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
	/if (${MyGroupBuffs.Size}) {
	
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
			
		|--------------------------------------------------------------------------------------------------Cast MyGroupBuffs array, on buffTarget.
			/declare i int local
			
			/for i 1 to ${MyGroupBuffs.Size}
				/if (${Me.Book[${MyGroupBuffs[${i}].Arg[1,/]}]}) {
					/if (${Debug}) /echo |- GroupBuffs -| Casting Spell: [${MyGroupBuffs[${i}].Arg[1,/]}] on [${buffTarget}]
					/call e3_Cast ${buffTarget} "${MyGroupBuffs[${i}].Arg[1,/]}" gem${DefaultGem} 300
				} else /if (${MyGroupBuffs[${i}].Arg[2,/].Equal[item]}) {
					/if (${Debug}) /echo |- GroupBuffs -| Casting Spell: [${MyGroupBuffs[${i}].Arg[1,/]}] on [${buffTarget}]
					/call e3_Cast ${buffTarget} "${MyGroupBuffs[${i}].Arg[1,/]}" item
				} else /if (${MyGroupBuffs[${i}].Arg[2,/].Equal[alt]} || ${MyGroupBuffs[${i}].Arg[2,/].Equal[aa]}) {
					/if (${Debug}) /echo |- GroupBuffs -| Casting Spell: [${MyGroupBuffs[${i}].Arg[1,/]}] on [${buffTarget}]
					/call e3_Cast ${buffTarget} "${MyGroupBuffs[${i}].Arg[1,/]}" alt
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

	/declare i int local
	/declare MaxBuffs int outer 0
	/if (${Me.AltAbility[Mystical Attuning]}) /varcalc MaxBuffs ${Me.AltAbility[Mystical Attuning]} / -5 + 1
	/call IniToVar ${Macro_Ini},Buffs,Allow_BuffRequests allow_BuffRequests bool outer
	/if (${Ini[${Character_Ini},Buffs].Length}) {
		/declare buffCheck_Interval int outer 150
		/declare BuffCheck bool outer TRUE
		
	|---------------------------------------------------------------------------------------------------------------------------------------Self Buffs.
		/if (${Ini[${Character_Ini},Buffs,SelfBuff#1].Length}) /call INItoArray ${Character_Ini},Buffs,SelfBuff# MyBuffs
		/if (${Ini[${Character_Ini},Buffs,Buff#1].Length}) /call INItoArray ${Character_Ini},Buffs,Buff# MyBuffs
		
		|------------------------------------------------------------------------------------------------Validate MyBuffs array.
		/if (${MyBuffs.Size}) {
			/for i 1 to ${MyBuffs.Size}
				/if (!${MyBuffs[${i}].Arg[2,/].Length}) {
					/if (!${Int[${Me.Book[${MyBuffs[${i}]}]}]}) {
						/echo ERROR: @[SelfBuff#${i}] - I do not have [${MyBuffs[${i}]}] in my spellbook. Removing it from SelfBuffs.
						/varset MyBuffs[${i}] BAD_ENTRY
					} else {
						/varset MyBuffs[${i}] ${MyBuffs[${i}]}/gem${DefaultGem}
						/if (${Debug}) /echo @[SelfBuff#${i}] - No spellgem has been specified for [${MyBuffs[${i}].Arg[1,/]}], defaulting to [${MyBuffs[${i}].Arg[2,/]}].
					}
				} else /if (${MyBuffs[${i}].Arg[2,/].Left[3].Equal[gem]}) {
					/if (!${Int[${MyBuffs[${i}].Arg[2,/].Right[1]}]}) {
						/echo ERROR: @[SelfBuff#${i}] - [${MyBuffs[${i}].Arg[2,/].Right[1]}] is not an acceptable spellgem, defaulting to [${DefaultGem}]
						/varset MyBuffs[${i}] ${MyBuffs[${i}].Arg[1,/]}/gem${DefaultGem}
					}
					/if (!${Int[${Me.Book[${MyBuffs[${i}].Arg[1,/]}]}]}) {
						/echo ERROR: @[SelfBuff#${i}] - I do not have [${MyBuffs[${i}].Arg[1,/]}] in my spellbook. Removing it from SelfBuffs.
						/varset MyBuffs[${i}] BAD_ENTRY
					}
				} else /if (${MyBuffs[${i}].Arg[2,/].Equal[item]}) {
					/if (!${FindItem[${MyBuffs[${i}].Arg[1,/]}].ID}) {
						/echo ERROR: @[SelfBuff#${i}] - I do not have [${MyBuffs[${i}].Arg[1,/]}] in my inventory. Removing it from SelfBuffs.
						/varset MyBuffs[${i}] BAD_ENTRY
					}
				} else /if (${MyBuffs[${i}].Arg[2,/].Equal[alt]} || ${MyBuffs[${i}].Arg[2,/].Equal[aa]}) {
					/if (!${Me.AltAbility[${MyBuffs[${i}].Arg[1,/]}].ID}) {
						/echo ERROR: @[SelfBuff#${i}] - I do not have any ranks in[${MyBuffs[${i}].Arg[1,/]}]. Removing it from SelfBuffs.
						/varset MyBuffs[${i}] BAD_ENTRY
					}
				} else /if (${MyBuffs[${i}].Arg[2,/].Length}) {
					/echo ERROR: @[SelfBuff#${i}] - [${MyBuffs[${i}].Arg[2,/]}] is not a recognized CastType.  Use /gem# for spells, /item for items, or /alt for Alternate Abilities.
					/varset MyBuffs[${i}] BAD_ENTRY
				}
			/next i
			
			/call RemoveArrayElement MyBuffs BAD_ENTRY
		}
		
	|----------------------------------------------------------------------------------------------------------------------------------------------Buff Others.
		/if (${Ini[${Character_Ini},Buffs,BuffOther#1].Length}) /call INItoArray ${Character_Ini},Buffs,BuffOther# BuffTargets
		
		|------------------------------------------------------------------------------------------------Validate BuffTargets array.
		/if (${BuffTargets.Size}) {
			/for i 1 to ${BuffTargets.Size}
				/if (!${BuffTargets[${i}].Arg[3,/].Length}) {
					/if (!${Int[${Me.Book[${BuffTargets[${i}].Arg[2,/]}]}]}) {
						/echo ERROR: @[BuffOther#${i}] - I do not have [${BuffTargets[${i}].Arg[2,/]}] in my spellbook. Removing it from BuffOthers.
						/varset BuffTargets[${i}] BAD_ENTRY
					} else {
						/varset BuffTargets[${i}] ${BuffTargets[${i}]}/gem${DefaultGem}
						/if (${Debug}) /echo @[BuffOther#${i}] - No spellgem has been specified for [${BuffTargets[${i}].Arg[2,/]}], defaulting to [${BuffTargets[${i}].Arg[3,/]}].
					}
				} else /if (${BuffTargets[${i}].Arg[3,/].Left[3].Equal[gem]}) {
					/if (!${Int[${BuffTargets[${i}].Arg[3,/].Right[1]}]}) {
						/echo ERROR: @[BuffOther#${i}] - [${BuffTargets[${i}].Arg[3,/].Right[1]}] is not an acceptable spellgem, defaulting to [${DefaultGem}]
						/varset BuffTargets[${i}] ${BuffTargets[${i}].Arg[1,/]}/${BuffTargets[${i}].Arg[2,/]}/gem${DefaultGem}
					}
					/if (!${Int[${Me.Book[${BuffTargets[${i}].Arg[2,/]}]}]}) {
						/echo ERROR: @[BuffOther#${i}] - I do not have [${BuffTargets[${i}].Arg[2,/]}] in my spellbook. Removing it from BuffOthers.
						/varset BuffTargets[${i}] BAD_ENTRY
					}
				} else /if (${BuffTargets[${i}].Arg[3,/].Equal[item]}) {
					/if (!${FindItem[${BuffTargets[${i}].Arg[2,/]}].ID}) {
						/echo ERROR: @[BuffOther#${i}] - I do not have [${BuffTargets[${i}].Arg[2,/]}] in my inventory. Removing it from BuffOthers.
						/varset BuffTargets[${i}] BAD_ENTRY
					}
				} else /if (${BuffTargets[${i}].Arg[3,/].Equal[alt]} || ${BuffTargets[${i}].Arg[3,/].Equal[aa]}) {
					/if (!${Me.AltAbility[${BuffTargets[${i}].Arg[2,/]}].ID}) {
						/echo ERROR: @[BuffOther#${i}] - I do not have any ranks in[${BuffTargets[${i}].Arg[2,/]}]. Removing it from BuffOthers.
						/varset BuffTargets[${i}] BAD_ENTRY
					}
				} else /if (${BuffTargets[${i}].Arg[3,/].Equal[group]}) {
					/varset BuffTargets[${i}] ${BuffTargets[${i}].Arg[1,/]}/${BuffTargets[${i}].Arg[2,/]}/gem${DefaultGem}/group
					/if (${Debug}) /echo @[BuffOther#${i}] - No spellgem has been specified for [${BuffTargets[${i}].Arg[2,/]}], defaulting to [${BuffTargets[${i}].Arg[3,/]}].
				} else /if (${BuffTargets[${i}].Arg[3,/].Length}) {
					/echo ERROR: @[BuffOther#${i}] - [${BuffTargets[${i}].Arg[3,/]}] is not a recognized CastType.  Use /gem# for spells, /item for items, or /alt for Alternate Abilities.
					/varset BuffTargets[${i}] BAD_ENTRY
				}
			/next i
			
			/call RemoveArrayElement BuffTargets BAD_ENTRY
		}
		
	|-------------------------------------------------------------------------------------------------------------------------------------------Group Buffs
		/if (${Ini[${Character_Ini},Buffs,GroupBuff#1].Length}) /call INItoArray ${Character_Ini},Buffs,GroupBuff# MyGroupBuffs
		
		|------------------------------------------------------------------------------------------------Validate MyGroupBuffs array.
		/if (${MyGroupBuffs.Size}) {
			/for i 1 to ${MyGroupBuffs.Size}
			/if (!${MyGroupBuffs[${i}].Arg[2,/].Length}) {
					/if (!${Int[${Me.Book[${MyGroupBuffs[${i}]}]}]}) {
						/echo ERROR: @[MyGroupBuff#${i}] - I do not have [${MyGroupBuffs[${i}]}] in my spellbook. Removing it from GroupBuffs.
						/varset MyGroupBuffs[${i}] BAD_ENTRY
					} else {
						/varset MyGroupBuffs[${i}] ${MyGroupBuffs[${i}]}/gem${DefaultGem}
						/if (${Debug}) /echo @[MyGroupBuff#${i}] - No spellgem has been specified for [${MyGroupBuffs[${i}].Arg[1,/]}], defaulting to [${MyGroupBuffs[${i}].Arg[2,/]}].
					}
				} else /if (${MyGroupBuffs[${i}].Arg[2,/].Left[3].Equal[gem]}) {
					/if (!${Int[${MyGroupBuffs[${i}].Arg[2,/].Right[1]}]}) {
						/echo ERROR: @[MyGroupBuff#${i}] - [${MyGroupBuffs[${i}].Arg[2,/].Right[1]}] is not an acceptable spellgem, defaulting to [${DefaultGem}]
						/varset MyGroupBuffs[${i}] ${MyGroupBuffs[${i}].Arg[1,/]}/gem${DefaultGem}
					}
					/if (!${Int[${Me.Book[${MyGroupBuffs[${i}].Arg[1,/]}]}]}) {
						/echo ERROR: @[MyGroupBuff#${i}] - I do not have [${MyGroupBuffs[${i}].Arg[1,/]}] in my spellbook. Removing it from GroupBuffs.
						/varset MyGroupBuffs[${i}] BAD_ENTRY
					}
				} else /if (${MyGroupBuffs[${i}].Arg[2,/].Equal[item]}) {
					/if (!${FindItem[${MyGroupBuffs[${i}].Arg[1,/]}].ID}) {
						/echo ERROR: @[MyGroupBuff#${i}] - I do not have [${MyGroupBuffs[${i}].Arg[1,/]}] in my inventory. Removing it from GroupBuffs.
						/varset MyGroupBuffs[${i}] BAD_ENTRY
					}
				} else /if (${MyGroupBuffs[${i}].Arg[2,/].Equal[alt]} || ${MyGroupBuffs[${i}].Arg[2,/].Equal[aa]}) {
					/if (!${Me.AltAbility[${MyGroupBuffs[${i}].Arg[1,/]}].ID}) {
						/echo ERROR: @[MyGroupBuff#${i}] - I do not have any ranks in[${MyGroupBuffs[${i}].Arg[1,/]}]. Removing it from GroupBuffs.
						/varset MyGroupBuffs[${i}] BAD_ENTRY
					}
				} else /if (${MyGroupBuffs[${i}].Arg[2,/].Length}) {
					/echo ERROR: @[MyGroupBuff#${i}] - [${MyGroupBuffs[${i}].Arg[2,/]}] is not a recognized CastType.  Use /gem# for spells, /item for items, or /alt for Alternate Abilities.
					/varset MyGroupBuffs[${i}] BAD_ENTRY
				}
			/next i
			
			/call RemoveArrayElement MyGroupBuffs BAD_ENTRY
		}

	|-----------------------------------------------------------------------------------------------------------------------------------CombatBuffs
		/if (${Ini[${Character_Ini},Buffs,CombatBuff#1].Length}) /call INItoArray ${Character_Ini},Buffs,CombatBuff# PriorityBuffs
		
		|------------------------------------------------------------------------------------------------Validate PriorityBuffs array.
		/if (${PriorityBuffs.Size}) {
			/for i 1 to ${PriorityBuffs.Size}
				/if (!${PriorityBuffs[${i}].Arg[3,/].Length}) {
					/if (!${Int[${Me.Book[${PriorityBuffs[${i}].Arg[2,/]}]}]}) {
						/echo ERROR: @[CombatBuffs#${i}] - I do not have [${PriorityBuffs[${i}].Arg[2,/]}] in my spellbook. Removing it from CombatBuffs.
						/varset PriorityBuffs[${i}] BAD_ENTRY
					} else {
						/varset PriorityBuffs[${i}] ${PriorityBuffs[${i}]}/gem${DefaultGem}
						/if (${Debug}) /echo @[CombatBuffs#${i}] - No spellgem has been specified for [${PriorityBuffs[${i}].Arg[2,/]}], defaulting to [${PriorityBuffs[${i}].Arg[3,/]}].
					}
				} else /if (${PriorityBuffs[${i}].Arg[3,/].Left[3].Equal[gem]}) {
					/if (!${Int[${PriorityBuffs[${i}].Arg[3,/].Right[1]}]}) {
						/echo ERROR: @[CombatBuffs#${i}] - [${PriorityBuffs[${i}].Arg[3,/].Right[1]}] is not an acceptable spellgem, defaulting to [${DefaultGem}]
						/varset PriorityBuffs[${i}] ${PriorityBuffs[${i}].Arg[1,/]}/${PriorityBuffs[${i}].Arg[2,/]}/gem${DefaultGem}
					}
					/if (!${Int[${Me.Book[${PriorityBuffs[${i}].Arg[2,/]}]}]}) {
						/echo ERROR: @[CombatBuffs#${i}] - I do not have [${PriorityBuffs[${i}].Arg[2,/]}] in my spellbook. Removing it from CombatBuffs.
						/varset PriorityBuffs[${i}] BAD_ENTRY
					}
				} else /if (${PriorityBuffs[${i}].Arg[3,/].Equal[item]}) {
					/if (!${FindItem[${PriorityBuffs[${i}].Arg[2,/]}].ID}) {
						/echo ERROR: @[CombatBuffs#${i}] - I do not have [${PriorityBuffs[${i}].Arg[2,/]}] in my inventory. Removing it from CombatBuffs.
						/varset PriorityBuffs[${i}] BAD_ENTRY
					}
				} else /if (${PriorityBuffs[${i}].Arg[3,/].Equal[alt]} || ${PriorityBuffs[${i}].Arg[3,/].Equal[aa]}) {
					/if (!${Me.AltAbility[${PriorityBuffs[${i}].Arg[2,/]}].ID}) {
						/echo ERROR: @[CombatBuffs#${i}] - I do not have any ranks in[${PriorityBuffs[${i}].Arg[2,/]}]. Removing it from CombatBuffs.
					}
				} else /if (${PriorityBuffs[${i}].Arg[3,/].Length}) {
					/echo ERROR: @[CombatBuffs#${i}] - [${PriorityBuffs[${i}].Arg[3,/]}] is not a recognized CastType. Use /gem# for spells, /item for items, or /alt for Alternate Abilities.
					/varset PriorityBuffs[${i}] BAD_ENTRY
				}
			/next i
			
			/call RemoveArrayElement PriorityBuffs BAD_ENTRY
		}
		
		/call IniToVar ${Character_Ini},Buffs,InstantIllusion ConstantIllusion string outer
		/if (${Defined[ConstantIllusion]}) {
			/declare illusionRace string outer ${FindItem[${ConstantIllusion}].Spell}
			/varset illusionRace ${illusionRace.Replace[Illusion: ,]}
			/if (${illusionRace.Equal[Guktan]}) /varset illusionRace Froglok
		}
	}
	/if (${MyBuffs.Size} || ${BuffTargets.Size} || ${MyGroupBuffs.Size} || ${PriorityBuffs.Size}) /declare doBuffs bool outer TRUE
	
/if (${Debug}) /echo <== setup_Buffs -|
/RETURN