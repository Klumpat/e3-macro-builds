|----------------------------------------------------------------------------|
|- e3_Macro_Casting.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Based off of ideas from spell_routines.inc v2.4							-|
|- Originally Written by Rusty and A_Druid_00.								-|
|----------------------------------------------------------------------------|
|- Contains casting functions for Spells, AAs, Items, and Discs.			-|
|----------------------------------------------------------------------------|



|----------------------------------------------------------------------------------------|
|- e3_Cast																				-|
|----------------------------------------------------------------------------------------|
|- Casting handler for e3.mac.															-|
|- Confirms casting requirements, and relays casting calls to type specific functions.	-|
|----------------------------------------------------------------------------------------|
|- @param targetID: The ID of the spawn you want to cast on.							-|
|- @param castName: The name of the spell you want to cast.								-|
|- @param castType: The type of casting to preform, Spell, item, AA, or disc.			-|
|- @param giveUpTimer: Maximum time to wait for the spell to be ready.					-|
|- @param SubToRun: The name of a custom 'sub' to run while casting.					-|
|----------------------------------------------------------------------------------------|
SUB e3_Cast(int targetID, castName, castType, giveUpTimer, SubToRun)
| /varset Debug 1
/if (${Debug}) /echo |- e3_Cast ==>

|--------------------------------------------------------------------Check cursor.
	/if (${Cursor.ID}) {
		/if (${Debug}) /echo |- e3_Cast -| ERROR: Cannot cast while holding [${Cursor}].
	} else {
	
	|-----------------------------------------------------------------Check invis.
		/if (${Me.Invis} && !${castInvis} && ${Me.Class.ShortName.NotEqual[ROG]}) {
			/if (${Debug}) /echo |- e3_Cast -| ERROR: Cannot cast while invisible.
		} else {
			
		|-----------------------------------------------------Reset casting variables.
			/varset no_Cast FALSE
			/call Casting_Events
			/varset castReturn X
			
		|-----------------------------------------If following, check leash.	
			/if (${Assisting} || !${SpawnCount[pc ${FollowTarget}]} || ${SpawnCount[pc ${FollowTarget}]} && ${Spawn[pc ${FollowTarget}].Distance} < ${LeashLength}  && !${Me.Moving} || ${SpawnCount[pc ${FollowTarget}]} && ${Spawn[pc ${FollowTarget}].Distance} > ${LeashLength} && !${Me.Moving}) {
				
		|----------------------------------------------------------------------------------------------Check conditions, based on castType.
				
			|---------------------------------------------------------------------------------Check spell conditions.
				/if (${castType.Left[3].Equal[gem]}) {
					
				|-----------------------------------------------------------------------------Convert spell name to ID
					/if (${Debug}) /echo |- e3_Cast -| /if (!${Int[${castName}]}) /varset castName ${Spell[${castName}].ID}
					/if (!${Int[${castName}]}) /varset castName ${Spell[${castName}].ID}
					
				|--------------------------------------------------------------------------Check that the spell is in your spellbook.
					/if (!${Me.Book[${Spell[${castName}].Name}]}) {
						/if (${Debug}) /echo |- e3_Cast -| ERROR: I do not have [${Spell[${castName}].Name}] in my spellbook.
					} else {
					
					|----------------------------------------------------Check mana.
						/if (${Me.CurrentMana}<${Spell[${castName}].Mana}) {
							/if (${Debug}) /echo |- e3_Cast -| ERROR: Not enough mana to cast [${Spell[${castName}].Name}].
						} else {
						
						|------------------------------------------Check range.
							/if (${Spell[${castName}].MyRange} && ${Spawn[id ${targetID}].Distance} > ${Spell[${castName}].MyRange}) {
								/if (${Debug}) /echo |- e3_Cast -| ERROR: [${Spawn[id ${targetID}].CleanName}] is out of range.
							} else {
							
							|----------------------------------------Check LoS.
								/if (${Spell[${castName}].SpellType.Left[10].NotEqual[Beneficial]} && !${Spawn[id ${targetID}].LineOfSight}) {
									/if (${Debug}) /echo |- e3_Cast -| ERROR: I cannot see [${Spawn[id ${targetID}].CleanName}].
								} else {
								
								|----------------------------------------------------------------Attempt to cast spell.
									/call cast_Spell ${targetID} ${castName} ${castType.Right[1]} ${giveUpTimer} ${SubToRun}
								}
							}
						}
					}
					
			|---------------------------------------------------------------------------------Check item conditions.
				} else /if (${castType.Equal[item]}) {
				
				|--------------------------------------------------------------Check for the item in inventory.
					/if (!${FindItemCount[=${castName}]}) {
						/if (${Debug}) /echo |- e3_Cast -| ERROR: I do not have [${castName}] in my inventory.
					} else {
					
					|-----------------------------------------------------Check for an existing item timer.
						/if (${item_Timer_${FindItem[=${castName}].ID}}) {
							/if (${Debug}) /echo |- e3_Cast -| ERROR: My recast timer is not yet met for [${castName}].
							/varset castReturn CAST_NOTREADY
						} else {
						
						|------------------------------------------Check range.
							/if (${Spell[${FindItem[=${castName}].Spell}].MyRange} && ${Spawn[id ${targetID}].Distance} > ${Spell[${FindItem[=${castName}].Spell}].MyRange}) {
								/if (${Debug}) /echo |- e3_Cast -| ERROR: [${Spawn[id ${targetID}].CleanName}] is out of range.
							} else {
							
							|----------------------------------------Check LoS.
								/if (${Spell[${FindItem[=${castName}].Spell}].SpellType.NotEqual[Beneficial]} && !${Spawn[id ${targetID}].LineOfSight}) {
									/if (${Debug}) /echo |- e3_Cast -| ERROR: I cannot see [${Spawn[id ${targetID}].CleanName}].
								} else {

								|---------------------------------------------------------------------Attempt to cast the item.
									/call cast_Item ${targetID} "${castName}" ${giveUpTimer} ${SubToRun}
								}
							}
						}
					}	
					
			|---------------------------------------------------------------------------------Check alt conditions.
				} else /if (${castType.Equal[alt]}) {
				
				|-----------------------------------------------------------------------------Convert AA name to ID
					/if (!${Int[${castName}]}) /varset castName ${Me.AltAbility[${castName}].ID}
					
				|-----------------------------------------------------Check for an existing item timer.
					/if (!${Me.AltAbilityReady[${castName}]} || ${alt_Timer_${castName}}) {
						/if (${Debug}) /echo |- e3_Cast -| ERROR: My recast timer is not yet met for [${castName}].
						/varset castReturn CAST_NOTREADY
					} else {
					
					|------------------------------------------Check range.
						/if (${Spell[${castName}].MyRange} && ${Spawn[id ${targetID}].Distance} > ${Spell[${castName}].MyRange}) {
							/if (${Debug}) /echo |- e3_Cast -| ERROR: [${Spawn[id ${targetID}].CleanName}] is out of range.
						} else {
			
						|----------------------------------------Check LoS.
							/if (${Spell[${castName}].SpellType.NotEqual[Beneficial]} && !${Spawn[id ${targetID}].LineOfSight}) {
								/if (${Debug}) /echo |- e3_Cast -| ERROR: I cannot see [${Spawn[id ${targetID}].CleanName}].
							} else {
							
							|---------------------------------------------------------------------Attempt to cast the AA.
								/call cast_AltAbility ${targetID} ${castName} ${SubToRun}
							}
						}
					}

			|---------------------------------------------------------------------------------Check disc conditions.		
				} else /if (${castType.Equal[disc]}) {
					
				|--------------------------------------------------------------------------------------------Check for a discipline timer, if it's 0 use the disc.
					/if (${disc_Timer_${castName.Replace[ ,]}}) {	
						/if (${Debug}) /echo |- e3_Cast -| ERROR: My recast timer is not yet met for [${castName}].
						/varset castReturn CAST_NOTREADY
					} else {
					
				|---------------------------------------------------------------------Attempt to use the disc.
						/call useDisc "${castName}"
					}
					
			|--------------------------------------If castType is not recognized.
				} else {
					/if (${Debug}) /echo |- e3_Cast -| ERROR: [${castType}] is not a recognized castType.
				}
				| /if (${no_Cast}) /varset bad_Spells ${bad_Spells}${castName},
			}
		}
	}
	
/if (${Debug}) {
	/echo |- e3_Cast -| castReturn=${castReturn}
	/echo <== e3_Cast -|
}
| /varset Debug 0
/RETURN



|--------------------------------------------------------------------------------|
|- cast_Spell																	-|
|--------------------------------------------------------------------------------|
|- Memorizes and casts spells if they are available within the allotted time.	-|
|--------------------------------------------------------------------------------|
|- @param targetID: The ID of the spawn you want to cast on.					-|
|- @param castName: The name of the spell you want to cast.						-|
|- @param spellGem: The # of the gem to memorize the spell to.					-|
|- @param giveUpTimer: Maximum time to wait for the spell to be ready.			-|
|- @param SubToRun: The name of a custom 'sub' to run while casting.			-|
|--------------------------------------------------------------------------------|
SUB cast_Spell(int targetID, castName, spellGem, timer giveUpTimer, SubToRun)
/if (${Debug}) /echo |- cast_Spell ==>

|-----------------------------------------------------------------------------Wait for spell to be ready.
	:waitToCast
	/if (${Debug}) /echo |- cast_Spell -| :waitToCast
	/if (!${Me.SpellReady[${Spell[${castName}]}]}) {
	
	|------------------------------------------------------------------------------------------------------Memorize spell.
		/if (!${Me.Gem[${Spell[${castName}]}]} && ${Int[${spellGem.Right[1]}]} && !${Window[SpellBookWnd].Open} && !${Me.Moving} && ${giveUpTimer}>5) {
			/if (${Debug}) /echo |- cast_Spell -| Memming [${Spell[${castName}]}] Gem[${spellGem.Right[1]}]
			/memspell ${spellGem.Right[1]} "${Spell[${castName}]}"
			/delay 2
			/call Background_Events
		}
		/if (${giveUpTimer}) {
			/delay 2
			/goto :waitToCast
		} else {
			/if (${Debug}) /echo |- e3_Cast -| ERROR: [${castName}] was not ready within the allotted time.
			/if (${Window[SpellBookWnd].Open}) /stand
			/varset castReturn CAST_NOTREADY
		}
	} else {
|-----------------------------------------------------------------------------------------------------------If the spell is ready to cast.
	
	|------------------------------------------------------------------------------Target the spell target.
		/if (${targetID} && ${Target.ID} != ${targetID}) /call TrueTarget ${targetID}
		
	|----------------------------------------------------------------------------------Pause follow.
		/if (${Following} && ${stopToCast} && !${NetAdvPath.Pause}) /squelch /netfollow pause
		
	|--------------------------------------------------------------------------------Pause Twist.
		/if (${Twist.Twisting}) /call pauseTwist
		
	|---------------------------------------------------------------------------------Cast the spell.	
		:cast_Spell
		/if (${Debug}) /echo |- cast_Spell -| Attempting to cast [${Spell[${castName}]}]
		/cast "${Spell[${castName}]}"
		/delay 5 ${Me.Casting.ID}
		/call Casting_Events
		/varcalc castEndTime ${Me.Casting.MyCastTime}*10
		
		/varset ActionTaken TRUE
		/if (${Me.Casting.ID}) /call e3_CastingChecks ${If[${SubToRun.NotEqual[NULL]},${SubToRun},]}
		
	|--------------------------------------------------------------------------------If the spell fizzled or was interrupted, and I still have time, try again.
		/if (${Select[${castReturn},CAST_FIZZLE,CAST_INTERRUPTED,CAST_STUNNED]} && ${giveUpTimer}) {
			/if (${Debug}) /echo |- cast_Spell -| Cast was interrupted, retrying.
			/goto :cast_Spell
		
	|-----------------------------------------------------------------------------------If there was an unexpected error.
		} else /if (${Select[${castReturn},CAST_OUTDOORS,CAST_COMPONENTS,CAST_GROUPONLY]}) {
			/varset no_Cast TRUE
			/varset castReturn CAST_CANCELLED
		}
		
	|-----------------------------------------------------------------reacquire follow, and return twist.
		/if (${Following} && !${Assisting} && ${NetAdvPath.State} != 1) /call AquireFollow
		/if (${returnTwist}) /squelch /twist start
	}
	
/if (${Debug}) /echo <== cast_Spell -|
/RETURN	



|--------------------------------------------------------------------------------|
|- cast_Item																	-|
|--------------------------------------------------------------------------------|
|- Equips and casts items if they are available within the allotted time.		-|
|--------------------------------------------------------------------------------|
|- @param targetID: The ID of the spawn you want to cast on.					-|
|- @param itemName: The name of the item you want to cast.						-|
|- @param itemTimer: The recast timer of the item you want to cast.				-|
|- @param SubToRun: The name of a custom 'sub' to run while casting.			-|
|--------------------------------------------------------------------------------|
SUB cast_Item(int targetID, itemName, itemTimer, SubToRun)
/if (${Debug}) /echo |- cast_Item ==>

|------------------------------------------------------------------------------Target the item target.
	/if (${targetID} && ${Target.ID} != ${targetID}) /call TrueTarget ${targetID}

|----------------------------------------------------------------------------------Pause follow.
	/if (${Following} && ${stopToCast} && !${NetAdvPath.Pause}) /squelch /netfollow pause
	
|--------------------------------------------------------------------------------Pause Twist.
	/if (${Twist.Twisting}) /call pauseTwist
	
|----------------------------------------------------------------------Attempt to cast the item.
	/declare giveUpTimer timer local 2s
	/declare slotName string local NOSWAP
	/declare oldItemName string local
	/declare swapItemBack bool local FALSE
	/declare lagMultiplier string local 0
	
	:cast_Item
	/if (${Debug}) /echo |- cast_Item -| Attempting to cast [${itemName}].

|------------------------------------------------------------------------------------------------------------If the item must be worn to cast, and is not currently equipped.
	/if (${FindItem[${itemName}].WornSlot[1]} && ${FindItem[${itemName}].EffectType.Find[worn]} && ${FindItem[${itemName}].InvSlot}>22) {
		
		/if (${Debug}) /echo |- cast_Item -| This is a worn item.
		
		/varset slotName ${FindItem[${itemName}].WornSlot[1].Name}
		
	} else /if (${FindItem[${itemName}].EffectType.Find[inventory]} && ${FindItem[${itemName}].InvSlot}>30) {
	
		/if (${Debug}) /echo |- cast_Item -| This is clickable from inventory.
		
		/declare i int local
		
		/for i 1 to 8
		
		|-------------------------------------------------------------------If the item in pack slot i is not a container.
			/if (!${InvSlot[pack${i}].Item.Container}) {
				/varset slotName pack${i}
				
		|-----------------------------------------------------------------------If the item in pack slot i is a container.
			} else {
			
			|----------------------------------------------------------------If I have no free pack slots, alert the user.
				/if (${InvSlot[pack8].Item.Container}) {
					/if (${Debug}) /echo |- cast_Item -| I have run out of inventory space and cannot cast [${itemName}].
					/goto :skipCast
				}
			}
			
		/if (${slotName.Equal[NOSWAP]}) /next i
		
	} else {
		
		/if (${Debug}) /echo |- cast_Item -| This has multiple equippable slots.
		
		/varset slotName ${FindItem[${itemName}].InvSlot.Name}
	}

	/if (${slotName.NotEqual[NOSWAP]} && ${FindItem[${itemName}].InvSlot} != ${InvSlot[${slotName}]}) {
		
		/if (${InvSlot[${slotName}]} < 23) {
			/varset swapItemBack TRUE
			/varset oldItemName ${InvSlot[${slotName}].Item.Name}
			/call WriteToIni PendingExchanges.ini,${MacroQuest.Server},${Me.CleanName} "${oldItemName}/${slotName}" 1
		}
		
		/call SwapItem "${itemName}" ${slotName}
		
		/declare pingDelayTimer timer local ${Math.Calc[${MacroQuest.Ping}+5].Int}
		
		:swapDelayLoop
		/if (${Debug}) /echo |- cast_Item -| :swapDelayLoop
			
			/call Background_Events
		
		/if (${pingDelayTimer}) /goto :swapDelayLoop
		
	}
	
	/cast item "${itemName}"

|-------------------------------------------------------------Wait to start casting.
	/delay 5 ${Me.Casting.ID}
	/call Casting_Events
	/varcalc castEndTime ${Me.Casting.MyCastTime}*10
	/varset ActionTaken TRUE
	/if (${Me.Casting.ID}) /call e3_CastingChecks ${If[${SubToRun.NotEqual[NULL]},${SubToRun},]}
	
|--------------------------------------------------------------------Check cast return.
	/if (${Select[${castReturn},CAST_FIZZLE,CAST_INTERRUPTED,CAST_STUNNED]} && ${giveUpTimer}) {
		/goto :cast_Item
	} else /if (${Select[${castReturn},CAST_OUTDOORS,CAST_NOHOLD]}) {
		/varset no_Cast TRUE
	} else /if (${Select[${castReturn},CAST_CANCELLED]}) {
		/call interrupt
	}
	
|----------------------------------------------------------------------Swap items back.
	
	/if (${swapItemBack} && ${FindItem[${oldItemName}].ID}) {
		
		/varset pingDelayTimer ${Math.Calc[${MacroQuest.Ping}+5].Int}
		
		:swapbackDelayLoop
		/if (${Debug}) /echo |- cast_Item -| :swapbackDelayLoop
			
			/call Background_Events
		
		/if (${pingDelayTimer}) /goto :swapbackDelayLoop
		
		
		/declare retryTimer timer local 30
		
		:SwapBackLoop
		/if (${Debug}) /echo |- cast_Item -| :SwapBackLoop
		
		/call SwapItem "${oldItemName}" ${slotName}
		
		/if (!${InvSlot[${slotName}].Item.Name.Equal[${oldItemName}]}) {
			/if (${retryTimer}) {
				/goto :SwapBackLoop
			} else {
				/echo I have failed to swap [${oldItemName}] back, after casting [${itemName}].
			}
		} else {
			/call WriteToIni PendingExchanges.ini,${MacroQuest.Server},${Me.CleanName} NONE 1
		}
	}
	
|------------------------------------------------------------------If no item timer was given, check the Timers_Ini for one
	/if (!${Defined[itemTimer]}) {
		/declare itemTimer timer local 20
		/if (${Ini[${itemTimers_Ini},${itemName.Left[1]},${itemName}].Length}) {
			/varset itemTimer ${Ini[${itemTimers_Ini},${itemName.Left[1]},${itemName}]}
		} else /if (!${Ini[${itemTimers_Ini},${itemName.Left[1]},${itemName}].Length}) {
			/echo There is no setting recorded for ${itemName}. Adding ${itemName} to ${itemTimers_Ini}, please update this timer to ensure proper macro functionality.
			/ini ${itemTimers_Ini} ${itemName.Left[1]} "${itemName}" 0
		}
	}
	
|----------------------------------------------------------------------------------Create a reuse timer for this item.
	/call CreateTimer item_Timer_${FindItem[${itemName}].ID} ${itemTimer}
	
	/if (${Cursor.Name.Equal[${itemName}]}) /call ClearCursor
	
|-----------------------------------------------------------------reacquire follow, and return twist.
	/if (${Following} && !${Assisting} && ${NetAdvPath.State} != 1) /call AquireFollow
	/if (${returnTwist}) /squelch /twist start	
	:skipCast
	
/if (${Debug}) /echo <== cast_Item -|
/RETURN



|--------------------------------------------------------------------------------|
|- cast_AltAbility																-|
|--------------------------------------------------------------------------------|
|- Casts Alternate Abilities if they are available within the allotted time.	-|
|--------------------------------------------------------------------------------|
|- @param targetID: The ID of the spawn you want to cast on.					-|
|- @param altName: The name of the Alternate Ability you want to cast.			-|
|- @param SubToRun: The name of a custom 'sub' to run while casting.			-|
|--------------------------------------------------------------------------------|
SUB cast_AltAbility(int targetID, altName, SubToRun)
/if (${Debug}) /echo |- cast_AltAbility ==>

|------------------------------------------------------------------------------Target the item target.
	/if (${targetID} && ${Target.ID} != ${targetID}) /call TrueTarget ${targetID}

|----------------------------------------------------------------------------------Pause follow.
	/if (${Following} && ${stopToCast} && !${NetAdvPath.Pause}) /squelch /netfollow pause
	
|--------------------------------------------------------------------------------Pause Twist.
	/if (${Twist.Twisting}) /call pauseTwist
	
|------------------------------------------------If I've just cast an AA, wait a moment before casting another.
	/if (${altRecast_Timer}) /delay 30 !${altRecast_Timer}
	
|----------------------------------------------------------------------Attempt to cast the AA.
	/declare giveUpTimer timer local 20
	
	:cast_Alt
	/if (${Debug}) /echo |- cast_AltAbility -| Attempting to cast [${Me.AltAbility[${altName}].Name}]
	/alt act ${altName}
	
|-------------------------------------------------------------Wait to start casting.
	/delay 5 ${Me.Casting.ID}
	/call Casting_Events
	/varcalc castEndTime ${Me.Casting.MyCastTime}*10
	/varset ActionTaken TRUE
	/if (${Me.Casting.ID}) /call e3_CastingChecks ${If[${SubToRun.NotEqual[NULL]},${SubToRun},]}
	/varset altRecast_Timer 2s
|**
	A recent server side fix has corrected previous issues with a few AltAbility related TLOs, removing the need for macro side AA timers.  I will be removing the following line, and corresponding timer related variables, once I'm confident everything is working correctly.
	/call CreateTimer alt_Timer_${Me.AltAbility[${altName}].ID} ${Me.AltAbility[${altName}].ReuseTime}s
**|
|-----------------------------------------------------------------reacquire follow, and return twist.
	/if (${Following} && !${Assisting} && ${NetAdvPath.State} != 1) /call AquireFollow
	
	/if (${returnTwist}) /squelch /twist start	
	
/if (${Debug}) /echo <== cast_AltAbility -|
/RETURN



SUB useDisc(discToUse)
/if (${Debug}) {
	/echo |- useDisc ==>
	/echo |- useDisc -| Attempting to use [${discToUse}]
}
		
	|---If I'm a bard, and singing, stop twisting.  If I'm casting a spell, wait until I'm done.	
		/if (${Twist.Twisting}) /call pauseTwist
		/if ( ${Me.Casting.ID} ) /delay 10s !${Me.Casting.ID}
		
	|---------------------------------------------------------------------------------Attempt to make a timer for the discipline I'm about to use.
		/if (${Ini[${discTimer_Ini},${Me.Class},${discToUse}].Length}) {
			/call CreateTimer disc_Timer_${discToUse.Replace[ ,]} ${Ini[${discTimer_Ini},${Me.Class},${discToUse}]}
			/varset disc_Timer_${discToUse.Replace[ ,]} ${Math.Calc[${disc_Timer_${discToUse.Replace[ ,]}} + 5]}
		} else /if (!${Ini[${discTimer_Ini},${Me.Class},${discToUse}].Length}) {
			/echo There is no setting recorded for ${discToUse}. Adding ${discToUse} to ${discTimer_Ini}, please update this timer to ensure proper macro functionality.
			/ini ${discTimer_Ini} "${Me.Class}" "${discToUse}" UNKNOWN
		}
		
		/disc ${discToUse}
		
		/if (${returnTwist}) /squelch /twist start

/if (${Debug}) /echo |- useDisc ==>	
/RETURN



|------------------------------------|
|- e3_CastingChecks					-|
|------------------------------------|
|- Checks to preform while casting.	-|
|------------------------------------|
SUB e3_CastingChecks
 /if (${Debug}) /echo |- e3_CastingChecks ==>
	
|----------------------------------------------------------------------Declare variables.
	/declare currentTarget int local ${Target.ID}
	/declare currentTargetType string local ${Target.Type}
	/declare cast_locX float local ${Me.X}
	/declare cast_locY float local ${Me.Y}
	
|----------------------------------------------------------------------wait_For_Cast loop.
	:wait_For_Cast
	/if (${Debug}) /echo |- e3_CastingChecks -| :wait_For_Cast
	/call Casting_Events
	/call Background_Events

|------------------------------------------------------------------------------If the caster is ducking, cancel.
	/if (${Me.State.Equal[DUCK]}) {
		/if (${Debug}) /echo |- e3_CastingChecks -| Cast cancelled.
		/varset castReturn CAST_CANCELLED
	}
|-----------------------------------------------------------------------------If I'm following, and have fallen behind, aquire follow.
	/if (!${Assisting} && ${Following} && ${SpawnCount[pc ${FollowTarget}]} && ${Spawn[${FollowTarget}].Distance} > ${LeashLength} && ${Spawn[${FollowTarget}].Distance} < ${FollowBreakDistance} && !${Me.Moving}) {
		/if (${Debug}) /echo |- e3_CastingChecks -| AquireFollow interrupt.
		/call AquireFollow
		/varset castReturn CAST_CANCELLED
	}
	
|----------------------------------------------------------------------------If the target has moved out of range.
	/if (${Spell[${Me.Casting.ID}].MyRange} && ${Spell[${Me.Casting.ID}].TargetType.NotEqual[Self]} && ${Target.Distance}>${Spell[${Me.Casting.ID}].MyRange}) {
		/if (${Debug}) /echo |- e3_CastingChecks -| Out of range interrupt.
		/call interrupt
		/varset castReturn CAST_CANCELLED
	}
	
|-------------------------------------------------------------------------------------If I've moved, cancel casting.
	/if (${cast_locX}!=${Me.X} || ${cast_locY}!=${Me.Y}) {
		/call Triangulate_Distance ${cast_locY},${cast_locY} ${Me.Loc.Replace[ ,]}
		/if (${Macro.Return}>5) {
			/if (${Debug}) /echo |- e3_CastingChecks -| Caster movement interrupt.
			/call interrupt
			/varset castReturn CAST_INTERRUPTED
		}
	}

|-------------------------------------------------------------------------If I've lost Line of Sight on the target.
	/if (${Spell[${Me.Casting.ID}].SpellType.Equal[Detrimental]} && !${Spawn[id ${Target.ID}].LineOfSight}) {
		/if (${Debug}) /echo |- e3_CastingChecks -| Line of Sight interrupt.
		/call interrupt
		/varset castReturn CAST_CANCELLED
	}
	
|-------------------------------------------------------------------------If the target has died.	
	/if (${Spawn[id ${Target.ID}].Type.Equal[Corpse]} && !${Select[${Spell[${Me.Casting.ID}].TargetType},PB AE,Targeted AE,Corpse]}) {
		/if (${Debug}) /echo |- e3_CastingChecks -| Spawn has died interrupt.
		/call interrupt
		/if (${Assisting}) /call AssistOff
		/varset castReturn CAST_CANCELLED
	}

|------------------------------------------------------------------If there is a custom SUB, run it.
	/if (${SubToRun.NotEqual[NULL]}) /call ${SubToRun}
	
	/delay 1
	
	/if (${Me.Casting.ID} && ${castReturn.NotEqual[CAST_CANCELLED]}) /goto :wait_For_Cast
	
/if (${Debug}) /echo <== e3_CastingChecks -|
/RETURN



SUB SwapItem(itemName,slotName)
/if (${Debug}) /echo |- SwapItem ==>

	/if (${Cursor.ID}) {
		/echo ERROR: |- SwapItem -| SwapItem cannot be preformed while there is an item on your cursor.
	} else {

		/declare startingCount int local ${FindItemCount[${itemName}]}
		/declare retryTimer timer local 50
		
		:try_Again
		
		/if (${Debug}) /echo |- SwapItem -| Attempting to exchange [${itemName}] for [${InvSlot[${slotName}].Item}]
		/exchange "${itemName}" ${slotName}
		/delay 30 ${InvSlot[${slotName}].Item.Name.Equal[${itemName}]} && ${FindItemCount[${itemName}]}==${startingCount}
		/if (${InvSlot[${slotName}].Item.Name.NotEqual[${itemName}]}) {
			/if (${retryTimer}) {
				/goto :try_Again
			} else {
				/if (${Debug}) /echo ERROR: |- SwapItem -| Failed to swap [${InvSlot[${slotName}].Item.Name}] for [${itemName}].
			}
		}
		

		
		/if (${Cursor.ID}) /call ClearCursor
	}

/if (${Debug}) /echo <== SwapItem -|
/RETURN



|----------------------------------------------------------------------------|
|- Handler for long term spells.											-|
|----------------------------------------------------------------------------|
|- Creates timers, and checks the duration of long term detrimental spells.	-|
|----------------------------------------------------------------------------|
SUB Check_LongTermSpell(int SpellTarget, SpellName, CastType, GiveUpTimer, int GiveUpValue)
/if (${Debug}) /echo |- Check_LongTermSpell ==>

|------------------------------Declare variables to store information about the timers being created.
	/declare dot_Timer string local
	/declare timer_Setting string local
	
|-------------------------------Set variables for a spell.
	/if (${CastType.Left[3].Equal[gem]}) {
		/varset dot_Timer ${SpellTarget}_${Spell[${SpellName}].ID}
		/varset timer_Setting ${Math.Calc[${Spell[${SpellName}].Duration}*6-6]}s

|-------------------------------Set variables for an item.
	} else /if (${CastType.Equal[item]}) {
		/varset dot_Timer ${SpellTarget}_${FindItem[${SpellName}].ID}
		/varset timer_Setting ${Math.Calc[${Spell[${FindItem[${SpellName}].Spell}].Duration}*6-6]}s
	
|-------------------------------Set variables for an AA.
	} else /if (${CastType.Left[3].Equal[Alt]}) {
		/varset dot_Timer ${SpellTarget}_${Me.AltAbility[${SpellName}].ID}
		/varset timer_Setting ${Math.Calc[${Spell[${Me.AltAbility[${SpellName}].Spell}].Duration}*6-6]}s
	}
	
|--------------------------------------------Check the duration of the timer of this spell. If it's less than 18 seconds, recast.
	/if (${${dot_Timer}} > 180) {
		/if (${Debug}) /echo |- Check_LongTermSpell -| ${Spawn[id ${SpellTarget}].CleanName}_${SpellName}[${dot_Timer}]= [${${dot_Timer}}]
	} else {
		/if (${Verbosity}) /docommand ${ChatToggle} Attempting to cast ${SpellName} on ${Spawn[${SpellTarget}].CleanName}.
		:recast
		/if (${Debug}) /echo |- Check_LongTermSpell -| Casting [${SpellName}][${CastType}] on [${Spawn[id ${SpellTarget}].CleanName}]. GiveUpTimer=${GiveUpTimer}${If[${Defined[GiveUpValue]},Retries=${GiveUpValue},]}.
		/call e3_Cast ${SpellTarget} "${SpellName}" ${CastType} ${GiveUpTimer}
		
	|---------------------------------------------If the spell was resisted.
		/if (${Select[${castReturn},CAST_RESISTED]}) {
			/if (!${Defined[ResistRetry]}) /declare ResistRetry int local 0
			/if (${Target.ID}) /docommand ${ChatToggle} **Warning** ${Target.CleanName} resisted ${SpellName}.
			/if (${Defined[ResistRetry]}) /varcalc ResistRetry ${ResistRetry}+1
			/if (${ResistRetry}<${GiveUpValue}) /goto :recast
			
	|----------------------------------------------If the target was immune.
		} else /if (${Select[${castReturn},CAST_IMMUNE]}) {
			/call createTimer ${dot_Timer} 35m
			/if (${Target.ID}) /docommand ${ChatToggle} **Warning** ${Target.CleanName} is IMMUNE to ${SpellName}.
			
	|----------------------------------------------If the spell did not take hold.
		} else /if (${Select[${castReturn},CAST_NOHOLD]}) {
			/call createTimer ${dot_Timer} 35m
			
	|---------------------------------------------If the cast was successful.
		} else /if (${Select[${castReturn},CAST_SUCCESS]}) {
			/if (${Verbosity} && ${Target.ID}) /docommand ${ChatToggle} Landed ${SpellName} on ${Target.CleanName}!!.
			/if (${Debug}) /echo |- Check_LongTermSpell -| created timer [${dot_Timer}][${timer_Setting}]
			/call createTimer ${dot_Timer} ${timer_Setting}
		}
	}
	
/if (${Debug}) /echo <== Check_LongTermSpell -|
/RETURN



|------------------------------------|
|- Pauses MQ2Twist while casting.	-|
|------------------------------------|
SUB pauseTwist
/if (${Debug}) /echo |- pauseTwist ==>

	/varset returnTwist 1
	/squelch /twist stop
	/delay 30 !${Me.Casting.ID}
	/delay 5
	
/if (${Debug}) /echo <== pauseTwist -|
/return



|------------------------------------|
|- Unpauses MQ2Twist after casting.	-|
|------------------------------------|
SUB unpauseTwist
/if (${Debug}) /echo |- unpauseTwist ==>

	/varset returnTwist 0
	/squelch /twist start
	
/if (${Debug}) /echo <== unpauseTwist -|
/return



|------------------------------------------------------------------------|
|- Gives popup information about debuffs on connected NetBots screens.	-|
|------------------------------------------------------------------------|
#event PopUp_Resist "#*#**Warning** #1# resisted #2#.#*#"
SUB event_PopUp_Resist(line, Mob, Spell)
/popup ${line}
/RETURN
#event PopUp_Immune "#*#**Warning** #1# is IMMUNE to #2#.#*#"
SUB event_PopUp_Immune(line, Mob, Spell)
/popup ${line}
/RETURN
#event PopUp_LoS "#*#**Warning** I cannot see #1#!#*#"
SUB event_PopUp_LoS(line, Mob)
/popup ${line}
/RETURN
#event PopUp_Landed "#*#Landed #2# on #1#!!.#*#"
SUB event_PopUp_Landed(line, Mob, Spell)
/popup ${line}
/RETURN



|----------------------------------------------------|
|- Interrupets a spell you're currently casting.	-|
|----------------------------------------------------|
|- Sets castReturn to CAST_CANCELLED.				-|
|----------------------------------------------------|
SUB Interrupt
/stopcast
/varset castReturn CAST_CANCELLED
/RETURN



|------------------------------------|
|- Events to check while casting.	-|
|------------------------------------|
SUB Casting_Events
/doevents BeginCast
/doevents Recover
/doevents Fizzle
/doevents Interrupt
/doevents Standing
/doevents FDFail
/doevents OutOfRange
/doevents OutOfMana
/doevents NoLOS
/doevents Resisted2
/doevents Resisted
/doevents Immune
/doevents Stunned
/doevents Collapse
/doevents NoTarget
/doevents NotReady
/doevents NoHold
/doevents PopUp_Resist
/doevents PopUp_Immune
/doevents PopUp_Oom
/doevents PopUp_Range
/doevents PopUp_LoS
/doevents PopUp_Landed
/RETURN



|------------------------------------------------------------------------------------|
|- Sets castReturn to CAST_SUCCESS.													-|
|------------------------------------------------------------------------------------|
|- castReturn begins set as CAST_SUCCESS, until changed by another casting event.	-|
|------------------------------------------------------------------------------------|
#event BeginCast "You begin casting#*#"
SUB Event_BeginCast
/varset castReturn CAST_SUCCESS
/RETURN



|----------------------------------------|
|- Sets castReturn to CAST_COLLAPSE.	-|
|----------------------------------------|
#event Collapse "Your gate is too unstable, and collapses.#*#"
SUB Event_Collapse
/varset castReturn CAST_COLLAPSE
/varset giveUpTimer 200
/RETURN



|------------------------------------------------------------|
|- Sets castReturn to CAST_RESTART, after you feign death.	-|
|------------------------------------------------------------|
#event FDFail "#1# has fallen to the ground.#*#"
SUB Event_FDFail(line, name)
/if (${name.Equal[${Me.Name}]} && ${Defined[castReturn]}) {
  /if (!${Me.Standing}) /stand
  /varset castReturn CAST_RESTART
}
/RETURN



|------------------------------------|
|- Sets castReturn to CAST_FIZZLE.	-|
|------------------------------------|
#event Fizzle "Your spell fizzles#*#"
SUB Event_Fizzle
/varset castReturn CAST_FIZZLE
/RETURN



|------------------------------------|
|- Sets castReturn to CAST_IMMUNE.	-|
|------------------------------------|
#event Immune "Your target is immune to changes in its attack speed#*#"
#event Immune "Your target is immune to changes in its run speed#*#"
#event Immune "Your target cannot be mesmerized#*#"
SUB Event_Immune
/varset castReturn CAST_IMMUNE
/RETURN



|----------------------------------------|
|- Sets castReturn to CAST_INTERRUPTED.	-|
|----------------------------------------|
#event Interrupt "Your casting has been interrupted#*#"
#event Interrupt "Your spell is interrupted#*#"
SUB Event_Interrupt
/if (${castReturn.NotEqual[CAST_CANCELLED]}) /varset castReturn CAST_INTERRUPTED
/RETURN



|----------------------------------------|
|- Sets castReturn to CAST_CANNOTSEE.	-|
|----------------------------------------|
#event NoLOS "You cannot see your target.#*#"
SUB Event_NoLOS
/varset castReturn CAST_CANNOTSEE
/RETURN



|----------------------------------------|
|- Sets castReturn to CAST_NOTARGET.	-|
|----------------------------------------|
#event NoTarget "You must first select a target for this spell!#*#"
SUB Event_NoTarget
/varset castReturn CAST_NOTARGET
/RETURN



|----------------------------------------|
|- Sets castReturn to CAST_NOTREADY.	-|
|----------------------------------------|
#event NotReady "Spell recast time not yet met.#*#"
SUB Event_NotReady
/varset castReturn CAST_NOTREADY
/RETURN



|----------------------------------------|
|- Sets castReturn to CAST_OUTOFMANA.	-|
|----------------------------------------|
#event OutOfMana "Insufficient Mana to cast this spell!#*#"
SUB Event_OutOfMana
/varset castReturn CAST_OUTOFMANA
/RETURN



|----------------------------------------|
|- Sets castReturn to CAST_OUTOFRANGE.	-|
|----------------------------------------|
#event OutOfRange "Your target is out of range, get closer!#*#"
SUB Event_OutOfRange
/varset castReturn CAST_OUTOFRANGE
/RETURN



|------------------------------------|
|- Sets castReturn to CAST_RECOVER.	-|
|------------------------------------|
#event Recover "You haven't recovered yet...#*#"
#event Recover "Spell recovery time not yet met#*#"
SUB Event_Recover
/varset castReturn CAST_RECOVER
/RETURN



|----------------------------------------|
|- Sets castReturn to CAST_RESISTED.	-|
|----------------------------------------|
#EVENT Resisted "Your target resisted the #1# spell#*#"
SUB Event_Resisted(line,  name)
/varset castReturn CAST_RESISTED
/RETURN



|----------------------------------------|
|- Sets castReturn to CAST_INTERRUPT.	-|
|----------------------------------------|
#EVENT Standing "You must be standing to cast a spell#*#"
SUB Event_Standing
/stand
/varset castReturn CAST_INTERRUPT
/RETURN



|------------------------------------|
|- Sets castReturn to CAST_STUNNED.	-|
|------------------------------------|
#EVENT Stunned "You are stunned#*#"
#EVENT Stunned "You can't cast spells while stunned!#*#"
#EVENT Stunned "You *CANNOT* cast spells, you have been silenced!#*#"
SUB EVENT_Stunned
/if (${Debug}) /echo |- EVENT_Stunned ==>

	/if (${Me.Stunned}) {
	  /delay 3s !${Me.Stunned}
	}
	/varset castReturn CAST_STUNNED

/if (${Debug}) /echo <== EVENT_Stunned -|	
/RETURN



|------------------------------------|
|- Sets castReturn to CAST_NOHOLD.	-|
|------------------------------------|
#event NoHold "You must first target a group member#*#"
#event NoHold "Your spell did not take hold#*#"
#event NoHold "Your spell would not have taken hold#*#"
#event NoHold "Your spell is too powerful for your intended target#*#"
SUB Event_NoHold
/varset castReturn CAST_NOHOLD
/RETURN



|------------------------|
|- Currently unused.	-|
|------------------------|
#event Missing_Components "You are missing #*#"
SUB event_Missing_Components
| /echo testing events...
/RETURN



|--------------------|
|- setup_Casting	-|
|--------------------|
SUB setup_Casting
/if (${Debug}) /echo |- setup_Casting ==>

	/declare castReturn string outer
	/declare castEndTime timer outer
	/declare altRecast_Timer timer outer
	/declare no_Cast bool outer FALSE
	/declare bad_Spells string outer PLACEHOLDER
	/declare itemRefreshTime float outer
	
	/if (!${Defined[discTimer_Ini]}) /declare discTimer_Ini string local discTimers.ini
	/if (!${Ini[${discTimer_Ini}].Length}) /call Make_discTimers_Ini
	/if (!${Ini[${itemTimers_Ini}].Length}) /call Build_Alphabetized_Ini ${itemTimers_Ini}

	/call IniToVar ${Macro_Ini},Casting,Default_SpellSet Default_SpellSet string outer
	/call IniToVar ${Macro_Ini},Casting,Default_SpellGem DefaultGem int outer
	/call IniToVar ${Macro_Ini},Casting,PauseStickToCast stopToCast bool outer
	
/if (${Debug}) /echo <== setup_Casting -|
/RETURN