|----------------------------------------------------------------------------|
|- e3_Macro_buffCheck.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|----------------------------------------------------------------------------|
|- Contains buffCheck functions for Spells, AAs, Items, and Discs.			-|
|----------------------------------------------------------------------------|



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

		/if (!${Me.Combat} && !${Assisting} && !${Me.Feigning} && !${Me.Moving} && ${Me.PctHPs} > ${min_BuffCheckHealth}) {

			| SelfBuffs.
			/if (${SelfBuffs.Size} && ${Me.FreeBuffSlots} > ${MaxBuffs}) /call buffSelf

			/if (!${ActionTaken}) {
			
				| BotBuffs.
				/if (${Ini[${Character_Ini},Buffs,Bot Buff#1].Length}) /call buffBots
				| /varset rebuffTimer ${buffRecheck_Delay}
			}
		}
	}
	
/if (${Debug}) /echo <== buffCheck -|
/RETURN



|------------------------------------------------|
|- Maintain buffs that have instant cast times.	-|
|------------------------------------------------|
SUB buffInstants
/if (${Debug}) /echo |- buffInstants ==>

	| -Declare local variables.
	/declare buffName string local
	/declare castName string local
	/declare gemNumber string local ${DefaultGem}
	
	/declare i int local
	
	/for i 1 to ${InstantBuffs.Size}
	
		/varset castName ${InstantBuffs[${i}].Arg[1,/]}
		
		| /Reagent Check.
		/if (${InstantBuffs[${i}].Find[/reagent|]}) {
			/call argueString reagent "${InstantBuffs[${i}]}"
			/if (${Bool[${Macro.Return}]}) {
				/if (!${FindItemCount[=${Macro.Return}]}) {
					/if (${Debug}) /echo |- buffInstants -| I do not have any [${Macro.Return}], skipping Instant Buff#${i} [${InstantBuffs[${i}]}].
					/goto :skipBuff
				}
			}
		}
		
		| -FindItem.
		/if (${FindItemCount[=${castName}]}) {
			
			| /BuffName Check.
			/if (${InstantBuffs[${i}].Find[/buffname|]}) {
				/call argueString buffname "${InstantBuffs[${i}]}"
				/if (${Bool[${Macro.Return}]}) /varset buffName ${Macro.Return}
			} else {
				/varset buffName ${FindItem[=${castName}].Spell}
			}
			
			/if (!${Spell[${buffName}].Stacks}) {
				/if (${Debug}) /echo |- buffInstants -| [${castName}] will not stack, skipping.
			} else {
				/if (${Me.Buff[${buffName}].Duration} > 10) {
					/if (${Debug}) /echo |- buffInstants -| [${Me.Buff[${buffName}].Duration}] ticks left on [${buffName}(${castName})].
				} else {
					/if (${FindItem[=${castName}].CastTime} <= .5) {
						/call e3_Cast ${Me.ID} "${castName}" item
					} else {
						/docommand ${ChatToggle} Instant Buffs must have a cast time of .5 or less.  Removing [${InstantBuffs[${i}]}] from my Instant Buff list.  Please review this entry in [${Character_Ini}].
						/call RemoveArrayElement InstantBuffs "${InstantBuffs[${i}]}"
						/beep
					}
				}
			}
			
		| -AltAbility.
		} else /if (${Me.AltAbility[${castName}]}) {
		
			/if (${Me.AltAbilityReady[${castName}]} && !${alt}) {
			
				| /BuffName Check.
				/if (${InstantBuffs[${i}].Find[/buffname|]}) {
					/call argueString buffname "${InstantBuffs[${i}]}"
					/if (${Bool[${Macro.Return}]}) {
						/varset buffName ${Macro.Return}
					}
				} else {
					/varset buffName ${Me.AltAbility[${castName}].Spell}
				}
			
				/if (!${Spell[${buffName}].Stacks}) {
					/if (${Debug}) /echo |- buffInstants -| [${castName}] will not stack, skipping.
				} else {
					/if (${Me.Buff[${buffName}].Duration} > 10) {
						/if (${Debug}) /echo |- buffInstants -| [${Me.Buff[${buffName}].Duration}] ticks left on [${buffName}(${castName})].
					} else {
						/if (${Spell[${Me.AltAbility[${castName}].Spell}].CastTime} <= .5) {
							/call e3_Cast ${Me.ID} "${castName}" alt
						} else {
							/docommand ${ChatToggle} Instant Buffs must have a cast time of .5 or less.  Removing [${InstantBuffs[${i}]}] from my Instant Buff list.  Please review this entry in [${Character_Ini}].
							/call RemoveArrayElement InstantBuffs "${InstantBuffs[${i}]}"
							/beep
						}
					}
				}
			}
		| CastSpell.
		} else /if (${Int[${Me.Book[${castName}]}]}) {

			| /BuffName Check.
			/if (${InstantBuffs[${i}].Find[/buffname|]}) {
				/call argueString buffname "${InstantBuffs[${i}]}"
				/if (${Bool[${Macro.Return}]}) {
					/varset buffName ${Macro.Return}
				}
			} else {
				/varset buffName ${castName}
			}
			
			| /Gem| Check.
			/if (${InstantBuffs[${i}].Find[/gem|]}) {
				/call argueString gem "${InstantBuffs[${i}]}"
				/varset gemNumber ${Macro.Return}
			}
			
			/if (!${Spell[${buffName}].Stacks}) {
				/if (${Debug}) /echo |- buffInstants -| [${castName}] will not stack, skipping.
			} else {
				/if (${Me.Buff[${buffName}].Duration} > 10) {
					/if (${Debug}) /echo |- buffInstants -| [${Me.Buff[${buffName}].Duration}] ticks left on [${buffName}(${castName})].
				} else {
					/if (${Spell[${castName}].CastTime} <= .5) {
						/call e3_Cast ${Me.ID} "${castName}" gem${gemNumber}
					} else {
						/docommand ${ChatToggle} Instant Buffs must have a cast time of .5 or less.  Removing [${InstantBuffs[${i}]}] from my Instant Buff list.  Please review this entry in [${Character_Ini}].
						/call RemoveArrayElement InstantBuffs "${InstantBuffs[${i}]}"
						/beep
					}
				}
			}
			
		| Found in bank.
		} else /if (${FindItemBankCount[=${castName}]}) {
			/docommand ${ChatToggle} I cannot cast [${castName}] because it's in the bank.  Removing [${castName}] from my Instant Buff list.
			/call RemoveArrayElement InstantBuffs "${InstantBuffs[${i}]}"
			/beep
		| Unrecognized.
		} else {
			/docommand ${ChatToggle} I can't find any items, alternate abilities, or spells named, [${castName}].  Removing [${InstantBuffs[${i}]}] from my Instant Buff list.  Please review this entry in [${Character_Ini}].
			/call RemoveArrayElement InstantBuffs "${InstantBuffs[${i}]}"
			/beep
		}

	:skipBuff
	/if (!${ActionTaken}) /next i

/if (${Debug}) /echo <== buffInstants -|
/RETURN



|------------------------------------------------|
|- Cast and maintain buffs on this character.	-|
|------------------------------------------------|
SUB buffSelf
/if (${Debug}) /echo |- buffSelf ==>

	| -Declare local variables.
	/declare buffName string local
	/declare castName string local
	/declare gemNumber int local ${DefaultGem}

	/declare i int local
	
	/for i 1 to ${SelfBuffs.Size}
	
		/varset castName ${SelfBuffs[${i}].Arg[1,/]}
		
		| /Reagent Check.
		/if (${SelfBuffs[${i}].Find[/reagent|]}) {
			/call argueString reagent "${SelfBuffs[${i}]}"
			/if (${Bool[${Macro.Return}]}) {
				/if (!${FindItemCount[=${Macro.Return}]}) {
					/if (${Debug}) /echo |- buffSelf -| I do not have any [${Macro.Return}], skipping Self Buff#${i} [${SelfBuffs[${i}]}].
					/goto :skip_Buff
				}
			}
		}
		
		| -FindItem.
		/if (${FindItemCount[=${castName}]}) {
			
			|  /BuffName Check.
			/if (${SelfBuffs[${i}].Find[/buffname|]}) {
				/call argueString buffname "${SelfBuffs[${i}]}"
				/if (${Bool[${Macro.Return}]}) {
					/varset buffName ${Macro.Return}
				}
			} else {
				/varset buffName ${FindItem[=${castName}].Spell}
			}

			/if (!${Spell[${buffName}].Stacks}) {
				/if (${Debug}) /echo |- buffSelf -| [${castName}] will not stack, skipping.
			} else {
				/if (${Me.Buff[${buffName}].Duration} > 10) {
					/if (${Debug}) /echo |- buffSelf -| [${Me.Buff[${buffName}].Duration}] ticks left on [${buffName}(${castName})].
				} else {
					/call e3_Cast ${Me.ID} "${castName}" item
				}
			}
		| -AltAbility.
		} else /if (${Me.AltAbility[${castName}]}) {
			
			| /BuffName Check.
			/if (${SelfBuffs[${i}].Find[/buffname|]}) {
				/call argueString buffname "${SelfBuffs[${i}]}"
				/if (${Bool[${Macro.Return}]}) {
					/varset buffName ${Macro.Return}
				}
			} else {
				/varset buffName ${Me.AltAbility[${castName}].Spell}
			}
		
			/if (!${Spell[${buffName}].Stacks}) {
				/if (${Debug}) /echo |- buffSelf -| [${castName}] will not stack, skipping.
			} else {
				/if (${Me.Buff[${buffName}].Duration} > 10) {
					/if (${Debug}) /echo |- buffSelf -| [${Me.Buff[${buffName}].Duration}] ticks left on [${buffName}(${castName})].
				} else {
					/call e3_Cast ${Me.ID} "${castName}" alt
				}
			}
			
		| -CastSpell.
		} else /if (${Int[${Me.Book[${castName}]}]}) {

			| /BuffName Check.
			/if (${SelfBuffs[${i}].Find[/buffname|]}) {
				/call argueString buffname "${SelfBuffs[${i}]}"
				/if (${Bool[${Macro.Return}]}) {
					/varset buffName ${Macro.Return}
				}
			} else {
				/varset buffName ${castName}
			}
			
			| /Gem| Check.
			/if (${SelfBuffs[${i}].Find[/gem|]}) {
				/call argueString gem "${SelfBuffs[${i}]}"
				/varset gemNumber ${Macro.Return}
			}
			
			/if (!${Spell[${buffName}].Stacks}) {
				/if (${Debug}) /echo |- buffSelf -| [${castName}] will not stack, skipping.
			} else {
				/if (${Me.Buff[${buffName}].Duration} > 10) {
					/if (${Debug}) /echo |- buffSelf -| [${Me.Buff[${buffName}].Duration}] ticks left on [${buffName}(${castName})].
				} else {
					/call e3_Cast ${Me.ID} "${castName}" gem${gemNumber} ${Math.Calc[${Spell[${castName}].RecastTime} + 10]}s
				}
			}
			
		| -Found in bank.
		} else /if (${FindItemBankCount[=${castName}]}) {
			/docommand ${ChatToggle} I cannot cast [${castName}] because it's in the bank.  Removing [${castName}] from my Self Buff list.
			/call RemoveArrayElement SelfBuffs "${SelfBuffs[${i}]}"
			/beep
		| -Unrecognized.
		} else {
			/docommand ${ChatToggle} I can't find any items, alternate abilities, or spells named, [${castName}].  Removing [${SelfBuffs[${i}]}] from my Self Buff list.  Please review this entry in [${Character_Ini}].
			/call RemoveArrayElement SelfBuffs "${SelfBuffs[${i}]}"
			/beep
		}
	
	:skip_Buff
	/if (!${ActionTaken}) /next i
	
/if (${Debug}) /echo <== buffSelf -|
/RETURN



|----------------------------------------|
|- Cast and maintain buffs on a NetBot.	-|
|----------------------------------------|
SUB buffBots
/if (${Debug}) /echo |- buffBots ==>
	
	| -Declare local variables.
	/declare buffTarget string local
	/declare buffName string local
	/declare castName string local
	/declare gemNumber string local ${DefaultGem}
	/declare groupList string local
	
	/declare i int local
	/declare x int local
	
	/for i 1 to ${BotBuffs.Size}
	
		/varset buffTarget ${BotBuffs[${i}].Arg[1,/]}
		/varset castName ${BotBuffs[${i}].Arg[2,/]}
		
		| -If the spawn is in zone.
		/if (${SpawnCount[pc ${buffTarget}]} && ${NetBots[${buffTarget}].Zone} == ${Zone.ID}) {
		
			| -Check for group only buffs.
			/if (${BotBuffs[${i}].Find[/group]}) {
				/for x 1 to ${Group}
					/varset groupList ${groupList}${Group.Member[${x}]},
				/next x
				/if (!${Select[${buffTarget},${groupList}]}) {
					/if (${Debug}) /echo |- buffBots -| [@Bot Buff#${i}] - [${buffTarget}] is not in my group, skipping Bot Buff#${i} [${BotBuffs[${i}]}].
					/goto :skip_Buff
				}
			}
			
			| /Reagent Check.
			/if (${BotBuffs[${i}].Find[/reagent|]}) {
				/call argueString reagent "${BotBuffs[${i}]}"
				/if (${Bool[${Macro.Return}]}) {
					/if (!${FindItemCount[=${Macro.Return}]}) {
						/if (!${missingComponentsTimer_botbuff#${i}}) {
							/echo |- buffBots -| I do not have any [${Macro.Return}], skipping Bot Buff#${i} [${BotBuffs[${i}]}].
							/call createTimer missingComponentsTimer_botbuff#${i} 5m
						}
						/goto :skip_Buff
					}
				}
			}
			
			| -Check for CAST_NOHOLD timers.
			/if (${NOHOLD_Timer_BotBuff#${i}}) {
				/if (${Debug}) /echo |- buffBots -| [@Bot Buff#${i}] - Did not take hold last time I cast it.  Waiting [${NOHOLD_Timer_BotBuff#${i}}] until I try again.
				/goto :skip_Buff
			}
			
			| -FindItem.
			/if (${FindItemCount[=${castName}]}) {
				
				| /BuffName Check.
				/if (${BotBuffs[${i}].Find[/buffname|]}) {
					/call argueString buffname "${BotBuffs[${i}]}"
					/if (${Bool[${Macro.Return}]}) {
						/varset buffName ${Macro.Return}
					}
				} else {
					/varset buffName ${FindItem[=${castName}].Spell}
				}
				
				/if (!${NetBots[${buffTarget}].Buff.Find[${Spell[${buffName}].ID}]}) {
					/if (${Spawn[pc ${buffTarget}].Distance} < ${Spell[${buffName}].MyRange}) {					
						/if (${Debug}) /echo |- buffBots -| Casting Item: [${castName} (${FindItem[=${castName}].Spell})] on [${buffTarget}].
						/call e3_Cast ${Spawn[pc ${buffTarget}].ID} "${castName}" item
						/if (${castReturn.Equal[CAST_NOHOLD]}) /call createTimer NOHOLD_Timer_BotBuff#${i} ${Math.Calc[${Spell[${buffName}].Duration} * 60 / 2].Int}	
					}
				}
				
			| -AltAbility.
			} else /if (${Me.AltAbility[${castName}]}) {	
				
				| /BuffName Check.
				/if (${BotBuffs[${i}].Find[/buffname|]}) {
					/call argueString buffname "${BotBuffs[${i}]}"
					/if (${Bool[${Macro.Return}]}) {
						/varset buffName ${Macro.Return}
					}
				} else {
					/varset buffName ${Me.AltAbility[${castName}].Spell}
				}
				
				/if (!${NetBots[${buffTarget}].Buff.Find[${Spell[${castName}].ID}]}) {
					/if (${Spawn[pc ${buffTarget}].Distance} < ${Spell[${castName}].MyRange}) {
						/if (${Debug}) /echo |- buffBots -| Casting Item: [${castName} (${FindItem[=${castName}].Spell})] on [${buffTarget}].
						/call e3_Cast ${Spawn[pc ${buffTarget}].ID} "${castName}" alt						
						/if (${castReturn.Equal[CAST_NOHOLD]}) /call createTimer NOHOLD_Timer_BotBuff#${i} ${Math.Calc[${Spell[${castName}].Duration} * 60 / 2].Int}
					}
				}
				
			| -CastSpell.
			} else /if (${Int[${Me.Book[${castName}]}]}) {	
				
				| /BuffName Check.
				/if (${BotBuffs[${i}].Find[/buffname|]}) {
					/call argueString buffname "${BotBuffs[${i}]}"
					/if (${Bool[${Macro.Return}]}) {
						/varset buffName ${Macro.Return}
					}
				} else {
					/varset buffName ${castName}
				}
				
				| /Gem| Check.
				/if (${BotBuffs[${i}].Find[/gem|]}) {
					/call argueString gem "${BotBuffs[${i}]}"
					/varset gemNumber ${Macro.Return}
				}
				
				/if (!${NetBots[${buffTarget}].Buff.Find[${Spell[${buffName}].ID}]}) {
					/if (${Spawn[pc ${buffTarget}].Distance} < ${Spell[${castName}].MyRange}) {
							/if (${Debug}) /echo |- buffBots -| Casting Spell: [${castName}] on [${buffTarget}].
							/call e3_Cast ${Spawn[pc ${buffTarget}].ID} "${castName}" gem${gemNumber} ${If[${Me.Gem[${castName}]},30,${Math.Calc[(${Spell[${castName}].RecastTime} + 5) * 10].Int}]}						
							/if (${castReturn.Equal[CAST_NOHOLD]}) /call createTimer NOHOLD_Timer_BotBuff#${i} ${Math.Calc[${Spell[${castName}].Duration} * 60 / 2].Int}
					}
				}

			| -Found in bank.
			} else /if (${FindItemBankCount[=${castName}]}) {
				/docommand ${ChatToggle} I cannot cast [${castName}] because it's in the bank!  Removing [${castName}] from my Bot Buffs list.
				/call RemoveArrayElement BotBuffs "${BotBuffs[${i}]}"
				/beep
				
			| -Unrecognized.
			} else {
				/docommand ${ChatToggle} I can't find any items, alternate abilities, or spells named, [${castName}].  Removing [${BotBuffs[${i}]}] from my Bot Buffs list.  Please review this entry in [${Character_Ini}].
				/call RemoveArrayElement BotBuffs "${BotBuffs[${i}]}"
				/beep
			}
		} else {
			/if (${Debug}) /echo |- buffBots -| [${buffTarget}] is not in the same zone.
		}

	:skip_Buff
	/if (!${ActionTaken}) /next i
	
/if (${Debug}) /echo <== buffBots -|
/RETURN


	
|--------------------------------------------------------|
|- Cast and maintain high priority buffs on a NetBot.	-|
|--------------------------------------------------------|
SUB combatBuffs
/if (${Debug}) /echo |- CombatBuffs ==>
	
	| -Declare local variables.
	/declare buffTarget string local
	/declare buffName string local
	/declare castName string local
	/declare gemNumber string local ${DefaultGem}
	/declare groupList string local
	
	/declare i int local
	/declare x int local
	
	/for i 1 to ${CombatBuffs.Size}
	
		/varset buffTarget ${CombatBuffs[${i}].Arg[1,/]}
		/varset castName ${CombatBuffs[${i}].Arg[2,/]}
		
		| -If the spawn is in zone.
		/if (${SpawnCount[pc ${buffTarget}]} && ${NetBots[${buffTarget}].Zone} == ${Zone.ID}) {
		
			| --Check for group only buffs.
			/if (${CombatBuffs[${i}].Find[/group]}) {
				/for x 1 to ${Group}
					/varset groupList ${groupList}${Group.Member[${x}]},
				/next x
				/if (!${Select[${buffTarget},${groupList}]}) {
					/if (${Debug}) /echo ERROR: [@Combat Buff#${i}] - [${buffTarget}] is not in my group, skipping.
					/goto :skip_Buff
				}
			}
			
			| /Reagent Check.
			/if (${CombatBuffs[${i}].Find[/reagent|]}) {
				/call argueString reagent "${CombatBuffs[${i}]}"
				/if (${Bool[${Macro.Return}]}) {
					/if (!${FindItemCount[=${Macro.Return}]}) {
						/if (!${missingComponentsTimer_botbuff#${i}}) {
							/echo |- Combat Buff -| I do not have any [${Macro.Return}], skipping Combat Buff#${i} [${CombatBuffs[${i}]}].
							/call createTimer missingComponentsTimer_botbuff#${i} 5m
						}
						/goto :skip_Buff
					}
				}
			}
			
			| -Check for CAST_NOHOLD timers.
			/if (${NOHOLD_Timer_CombatBuffs#${i}}) {
				/if (${Debug}) /echo ERROR: [@Combat Buff#${i}] - Did not take hold last time I cast it.  Waiting [${NOHOLD_Timer_CombatBuffs#${i}}] until I try again.
				/goto :skip_Buff
			}
			
			| -FindItem.
			/if (${FindItemCount[=${castName}]}) {
				
				| /BuffName Check.
				/if (${CombatBuffs[${i}].Find[/buffname|]}) {
					/call argueString buffname "${CombatBuffs[${i}]}"
					/if (${Bool[${Macro.Return}]}) {
						/varset buffName ${Macro.Return}
					}
				} else {
					/varset buffName ${FindItem[=${castName}].Spell}
				}
				
				/if (!${NetBots[${buffTarget}].Buff.Find[${Spell[${buffName}].ID}]}) {
					/if (${Spawn[pc ${buffTarget}].Distance} < ${Spell[${buffName}].MyRange}) {
						/if (${Debug}) /echo |- CombatBuffs -| Casting Item: [${castName} (${FindItem[=${castName}].Spell})] on [${buffTarget}].
						/call e3_Cast ${Spawn[pc ${buffTarget}].ID} "${castName}" item							
						/if (${castReturn.Equal[CAST_NOHOLD]}) /call createTimer NOHOLD_Timer_CombatBuffs#${i} ${Math.Calc[${Spell[${buffName}].Duration} * 60 / 2].Int}
					}
				}
				
			| -AltAbility.
			} else /if (${Me.AltAbility[${castName}]}) {	
				
				| /BuffName Check.
				/if (${CombatBuffs[${i}].Find[/buffname|]}) {
					/call argueString buffname "${CombatBuffs[${i}]}"
					/if (${Bool[${Macro.Return}]}) {
						/varset buffName ${Macro.Return}
					}
				} else {
					/varset buffName ${Me.AltAbility[${castName}].Spell}
				}
				
				/if (!${NetBots[${buffTarget}].Buff.Find[${Spell[${castName}].ID}]}) {
					/if (${Spawn[pc ${buffTarget}].Distance} < ${Spell[${castName}].MyRange}) {
						/if (${Debug}) /echo |- CombatBuffs -| Casting Item: [${castName} (${FindItem[=${castName}].Spell})] on [${buffTarget}].
						/call e3_Cast ${Spawn[pc ${buffTarget}].ID} "${castName}" alt						
						/if (${castReturn.Equal[CAST_NOHOLD]}) /call createTimer NOHOLD_Timer_CombatBuffs#${i} ${Math.Calc[${Spell[${castName}].Duration} * 60 / 2].Int}
					}
				}
				
			| -CastSpell.
			} else /if (${Int[${Me.Book[${castName}]}]}) {	
				
				| /BuffName Check.
				/if (${BotBuffs[${i}].Find[/buffname|]}) {
					/call argueString buffname "${CombatBuffs[${i}]}"
					/if (${Bool[${Macro.Return}]}) {
						/varset buffName ${Macro.Return}
					}
				} else {
					/varset buffName ${castName}
				}
				
				| /Gem| Check.
				/if (${BotBuffs[${i}].Find[/gem|]}) {
					/call argueString gem "${CombatBuffs[${i}]}"
					/varset gemNumber ${Macro.Return}
				}
				
				/if (!${NetBots[${buffTarget}].Buff.Find[${Spell[${buffName}].ID}]}) {
					/if (${Spawn[pc ${buffTarget}].Distance} < ${Spell[${castName}].MyRange}) {
						/if (${Debug}) /echo |- buffBots -| Casting Spell: [${castName}] on [${buffTarget}].
						/call e3_Cast ${Spawn[pc ${buffTarget}].ID} "${castName}" gem${gemNumber} ${If[${Me.Gem[${castName}]},0,${Math.Calc[(${Spell[${castName}].RecastTime} + 5) * 10].Int}]}					
						/if (${castReturn.Equal[CAST_NOHOLD]}) /call createTimer NOHOLD_Timer_CombatBuffs#${i} ${Math.Calc[${Spell[${castName}].Duration} * 60 / 2].Int}
					}
				}

			| -Found in bank.
			} else /if (${FindItemBankCount[=${castName}]}) {
				/docommand ${ChatToggle} I cannot cast [${castName}] because it's in the bank!  Removing [${castName}] from my Combat Buffs list.
				/call RemoveArrayElement CombatBuffs "${CombatBuffs[${i}]}"
				/beep
				
			| -Unrecognized.
			} else {
				/docommand ${ChatToggle} I can't find any items, alternate abilities, or spells named, [${castName}].  Removing [${CombatBuffs[${i}]}] from my Combat Buffs list.  Please review this entry in [${Character_Ini}].
				/call RemoveArrayElement CombatBuffs "${CombatBuffs[${i}]}"
				/beep
			}
		} else {
			/if (${Debug}) /echo |- CombatBuffs -| [${buffTarget}] is not in the same zone.
		}

	:skip_Buff
	/if (!${ActionTaken}) /next i
	
/if (${Debug}) /echo <== CombatBuffs -|
/RETURN



|--------------------------------------------------------|
|- Casts GroupBuffs array on the specified buffTarget.	-|
|--------------------------------------------------------|
|- @param line: '<Name> Buff me'.						-|
|- @param ChatSender: The buff requestor.				-|
|- @param buffTarget: The target to cast buffs on.		-|
|--------------------------------------------------------|
#event castGroupBuffs "<#1#> Buff #*#"
SUB event_castGroupBuffs(line, ChatSender)
/if (${Debug}) /echo |- event_castGroupBuffs ==>
	
	/if (${GroupBuffs.Size}) {
		| -Verify the event
		/call verifyEvent "${ChatSender}" "${line}"
		/varset ChatSender ${Macro.Return.Arg[1,_]}
		|    |--------Included Bot-----------|
		/if (${Bool[${Macro.Return.Arg[3,_]}]}) {
		
			| -------------------Find buffTarget------------------- |
			/declare buffTarget int local
			/declare castName string local
			/declare gemNumber string local gem${DefaultGem}
			/declare groupList string local
			
			/declare i int local
			/declare x int local
			
			/if (${Int[${line.Arg[3, ]}]}) {
				/varset buffTarget ${line.Arg[3, ]}
			} else /if (${line.Find[Buff me]}) {
				/varset buffTarget ${Spawn[pc ${ChatSender}].ID}
			} else /if (${line.Find[Buff pet]} || ${line.Find[Buff my pet]}) {
				/varset buffTarget ${Spawn[pet ${Spawn[pc ${ChatSender}].Pet}].ID}
			} else  /if (${SpawnCount[pc ${line.Arg[3, ]}]}) {
				/varset buffTarget ${Spawn[pc ${line.Arg[3, ]}].ID}
			} else /if (${line.Find['s pet]}) {
				/varset buffTarget ${Spawn[pet ${Spawn[pc ${line.Arg[3, ].Arg[1,']}].Pet}].ID}
			} else /if (${line.Find[s pet]}) {
				/varset buffTarget ${Spawn[pet ${Spawn[pc ${line.Arg[3, ].Left[${Math.Calc[${line.Arg[3, ].Length} - 1].Int}]}].Pet}].ID}
			}
			
			| -------------------Verify the buffTarget------------------- |
			/if (!${buffTarget}) {
				/echo Could not find a matching SpawnID.
			} else {
		
				/if (!${SpawnCount[id ${buffTarget} radius 200]}) {
					/bc [${Spawn[id ${buffTarget}].CleanName}] is not in range.
				} else {
					
					/for i 1 to ${GroupBuffs.Size}
					
						/varset castName ${GroupBuffs[${i}].Arg[1,/]}
				
						| /Reagent Check.
						/if (${GroupBuffs[${i}].Find[/reagent|]}) {
							/call argueString reagent "${GroupBuffs[${i}]}"
							/if (${Bool[${Macro.Return}]}) {
								/if (!${FindItemCount[=${Macro.Return}]}) {
									/echo I do not have any [${Macro.Return}], skipping Group Buff#${i} [${GroupBuffs[${i}]}].
									/goto :skip_Buff
								}
							}
						}
						
						| -Check for group only buffs.
						/if (${GroupBuffs[${i}].Find[/group]}) {
							/for x 1 to ${Group}
								/varset groupList ${groupList}${Group.Member[${x}]},
							/next x
							/if (!${Select[${buffTarget},${groupList}]}) {
								/echo [${buffTarget}] is not in my group, skipping Group Buff#${i} [${GroupBuffs[${i}]}].
								/goto :skip_Buff
							}
						}

						/if (${FindItemCount[=${castName}]}) {
							/call e3_Cast ${buffTarget} "${castName}" item
						} else /if (${Me.AltAbility[${castName}]}) {
							/call e3_Cast ${buffTarget} "${castName}" alt
						} else /if (${Int[${Me.Book[${castName}]}]}) {
							| /Gem| Check.
							/if (${GroupBuffs[${i}].Find[/gem|]}) {
								/call argueString gem| "${GroupBuffs[${i}]}"
								/varset gemNumber gem${Macro.Return}
							}
							/call e3_Cast ${buffTarget} "${castName}" gem${gemNumber} ${Math.Calc[${Spell[${castName}].RecastTime} + 30]}s
						} else /if (${FindItemBankCount[=${castName}]}) {
							/docommand ${ChatToggle} I cannot cast [${castName}] because it's in the bank!
						} else {
							/docommand ${ChatToggle} I can't find any items, alternate abilities, or spells named, [${castName}].  Removing [${GroupBuffs[${i}]}] from my Group Buffs list.  Please review this entry in [${Character_Ini}].
							/call RemoveArrayElement GroupBuffs "${GroupBuffs[${i}]}"
							/beep
						}
					
					:skip_Buff
					/next i
					
					/docommand ${ChatToggle} Done buffing ${Spawn[id ${buffTarget}].CleanName}.
				}
			}
		}
	}
	
/if (${Debug}) /echo <== castGroupBuffs -|
/RETURN



|--------------------------------------------------------|
|- Relays buff requests from /tell and /group to EQBC.	-|
|--------------------------------------------------------|
|- @param line: 'Name tells you, 'Buff me'.'			-|
|- @param buffTarget: The target to cast buffs on.		-|
|--------------------------------------------------------|
#event buffRequestRelay "#1# tells you, 'Buff #*#'"
#event buffRequestRelay "#1# tells you, 'Buff #*# |-|#2#'"
SUB event_buffRequestRelay(line, ChatSender, eventParams)
/if (${Debug}) /echo |- event_buffRequestRelay ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (!${Bool[${Macro.Return.Arg[2,_]}]}) {
		
		/call IniToVar "${Macro_Ini},Buff Requests,Allow Buff Requests (On/Off)" allow_BuffRequests bool local
		
		| If the user is not valid, check allow_BuffRequests
		/if (${Macro.Return}) {
			
			| If allow_BuffRequests=TRUE, check restricked PCs
			/if (${Select[${ChatSender},${Ini[${Macro_Ini},Buff Requests,Restricted PCs]}]}) {			
				/tell ${ChatSender} I ain't buffin you, sucka!
			} else {
				/goto :allowAccess
			}
			
		| If allow_BuffRequests=FALSE
		} else {
			/tell ${ChatSender} Sorry, I'm not currently buffing.
		}
	} else {
		
		:allowAccess
		
		| -------------------Find buffTarget------------------- |
		/declare buffTarget int local
		/declare castName string local
		/declare gemNumber string local gem${DefaultGem}
		/declare groupList string local
		
		/declare i int local
		/declare x int local
		
		/if (${Int[${line.Arg[3, ]}]}) {
			/varset buffTarget ${line.Arg[3, ]}
		} else /if (${line.Find[Buff me]}) {
			/varset buffTarget ${Spawn[pc ${ChatSender}].ID}
		} else /if (${line.Find[Buff pet]} || ${line.Find[Buff my pet]}) {
			/varset buffTarget ${Spawn[pet ${Spawn[pc ${ChatSender}].Pet}].ID}
		} else  /if (${SpawnCount[pc ${line.Arg[3, ]}]}) {
			/varset buffTarget ${Spawn[pc ${line.Arg[3, ]}].ID}
		} else /if (${line.Find['s pet]}) {
			/varset buffTarget ${Spawn[pet ${Spawn[pc ${line.Arg[3, ].Arg[1,']}].Pet}].ID}
		} else /if (${line.Find[s pet]}) {
			/varset buffTarget ${Spawn[pet ${Spawn[pc ${line.Arg[3, ].Left[${Math.Calc[${line.Arg[3, ].Length} - 1].Int}]}].Pet}].ID}
		}
		
		| -------------------Verify the buffTarget------------------- |
		/if (!${buffTarget}) {
			/echo Could not find a matching SpawnID.
		} else {
			/if (!${SpawnCount[id ${buffTarget} radius 200]}) {
				/bc [${Spawn[id ${buffTarget}].CleanName}] is not in range.
			} else {
			
				/if (${Assisting} && !${Bool[${Macro.Return.Arg[2,_]}]}) {
					/tell ${ChatSender} I'm busy, try again in a few minutes.
				} else {
			
					/if (${GroupBuffs.Size}) {
						/tell ${ChatSender} Casting buffs on ${Spawn[id ${buffTarget}].CleanName}...
						/bc ${If[${Defined[eventParams]},Buff ${buffTarget} -${eventParams},Buff ${buffTarget}]}
						
					| If I don't have buffs, but others might.
					} else /if (!${GroupBuffs.Size} && ${NetBots.Counts}>1) {
						/tell ${ChatSender} Requesting buffs on ${Spawn[id ${buffTarget}].CleanName}...
						/bc ${If[${Defined[eventParams]},Buff ${buffTarget} -${eventParams},Buff ${buffTarget}]}
						
					| If I'm not a buffer, and no other bots are on.
					} else /if (!${GroupBuffs.Size} && ${NetBots.Counts}<=1) {
						/tell ${ChatSender} Sorry, I'm not currently set up to cast any buffs, try again later. (This is an automated response)
					}
				}
			}
		}
	}

/if (${Debug}) /echo <== event_buffRequestRelay -|
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



Sub buffCheck_Setup
/if (${Debug}) /echo |- buffCheck_Setup ==>
	
	| -Adjust MaxBuffs to account for the Mystical Attuning AAs.
	/declare MaxBuffs int outer 0
	/if (${Me.AltAbility[Mystical Attuning]}) /varcalc MaxBuffs ${MaxBuffs} - (${Me.AltAbility[Mystical Attuning]} / 5 - 1)

	/declare BuffCheck bool outer FALSE
	/declare min_BuffCheckHealth int outer 70
	/declare buffRecheck_Delay int outer 150
	
	/if (${Ini[${Macro_Ini},Buff Requests,Allow Buff Requests (On/Off)].Length}) {
		/call IniToVar "${Macro_Ini},Buff Requests,Allow Buff Requests (On/Off)" allow_BuffRequests bool outer
	} else {
		/declare allow_BuffRequests bool outer TRUE
	}
	
	/if (${Ini[${Character_Ini},Buffs,Instant Buff#1].Length}) /call IniToArray "${Character_Ini},Buffs,Instant Buff#" InstantBuffs
	/if (${Ini[${Character_Ini},Buffs,Self Buff#1].Length}) /call IniToArray "${Character_Ini},Buffs,Self Buff#" SelfBuffs
	/if (${Ini[${Character_Ini},Buffs,Group Buff#1].Length}) /call IniToArray "${Character_Ini},Buffs,Group Buff#" GroupBuffs
	/if (${Ini[${Character_Ini},Buffs,Bot Buff#1].Length}) /call IniToArray "${Character_Ini},Buffs,Bot Buff#" BotBuffs
	/if (${Ini[${Character_Ini},Buffs,Combat Buff#1].Length}) /call IniToArray "${Character_Ini},Buffs,Combat Buff#" CombatBuffs
	
	/if (${InstantBuffs.Size} || ${SelfBuffs.Size} || ${BotBuffs.Size} || ${CombatBuffs.Size}) /varset BuffCheck TRUE
	
/if (${Debug}) /echo <== buffCheck_Setup -|
/return



Sub buffCheck_Background_Events
/return



SUB buffCheck_MacroSettings
/if (${Debug}) /echo |- buffCheck_MacroSettings ==>

	/call WriteToIni "${Macro_Ini},Buff Requests,Allow Buff Requests (On/Off)" On
	/call WriteToIni "${Macro_Ini},Buff Requests,Restricted PCs"
	/call WriteToIni "${Macro_Ini},Buff Requests,Restricted Spells"
	
/if (${Debug}) /echo <== buffCheck_MacroSettings -|
/RETURN



SUB buffCheck_CharacterSettings
/if (${Debug}) /echo |- buffCheck_CharacterSettings ==>

	/call WriteToIni "${Character_Ini},Buffs,Instant Buff#1"
	/call WriteToIni "${Character_Ini},Buffs,Self Buff#1"
	/call WriteToIni "${Character_Ini},Buffs,Bot Buff#1"
	/call WriteToIni "${Character_Ini},Buffs,Combat Buff#1"
	/call WriteToIni "${Character_Ini},Buffs,Group Buff#1"

/if (${Debug}) /echo <== buffCheck_CharacterSettings -|
/RETURN



Sub buffCheck_Aliases
/if (${Debug}) /echo |- buffCheck_Aliases ==>

	/squelch /alias /Buff delete
	/squelch /alias /BuffMe delete
	/squelch /alias /Buffs delete
	/squelch /alias /BuffT delete
	/squelch /alias /BuffIt delete
	/squelch /alias /BuffAll delete
	/squelch /alias /BuffGroups delete
	/squelch /alias /BuffPet delete
	
	/alias /Buff /bc Buff me
	/alias /BuffMe /bc Buff me
	/alias /Buffs /bc Buff me
	/alias /BuffT /bc Buff ${Target.ID}
	/alias /BuffIt /bc Buff ${Target.ID}
	/alias /BuffAll /bc Buff All
	/alias /BuffGroups /bc Buff All
	/alias /BuffPet /bc Buff ${Spawn[pet ${Spawn[id ${Target.ID}].Pet}].ID}

/if (${Debug}) /echo <== buffCheck_Aliases -|
/return