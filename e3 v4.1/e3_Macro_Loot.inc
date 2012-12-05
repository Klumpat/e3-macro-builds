|------------------------------------------------------------------------|
|- e3_Macro_Loot.inc v2.1 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Automatically loots corpses of mobs you have killed.					-|
|------------------------------------------------------------------------|

${Widow[LootWnd].Child[LW_CorpseName].Text.Equal[${Me.CleanName}'s corpse]}

|------------------------------------------------------------|
|- Automatically engage looting functions when a mob dies.	-|
|------------------------------------------------------------|
#event YourKill "#*#TestLoot#*#"
#event YourKill "You have slain #*#!"
#event YourKill "You gain experience!!"
#event YourKill "You gain party experience!!"
#event YourKill "You gained raid experience!"
SUB Event_YourKill
/if (${Debug}) /echo |- Event_YourKill ==>

|--------------------------------------------------------------------------------------------------------------------------------If I am a looter.
	/if (${NinjaLoot_On}) {
	
	|---------------------------------------------------Check that I'm not invisible, and have free inventory space.
		/if (!${Me.Invis} && ${Me.FreeInventory}) {
			/declare i int local
		|------------------------------------Rebuild looted_Corpses list, remove IDs of corpses that have vanished.
			/if (${looted_Corpses.Length}) {
				/varset i 1
				:check_vanished_Corpses
				/if (!${Spawn[id ${looted_Corpses.Arg[${i},-]}].Type.Equal[Corpse]}) /varset looted_Corpses ${looted_Corpses.Replace[${looted_Corpses.Arg[${i},-]}-,]}
				/varcalc i ${i} + 1
				/if (${looted_Corpses.Arg[${i},,].Length}) /goto :check_vanished_Corpses
			}
			
		|------------------------------------------------------------------------Check all the corpses within the seek_Radius to see if they have been looted.  If not, call loot_It
			/for i 1 to ${SpawnCount[npc corpse radius ${seek_Radius}]}
			
				/if (${looted_Corpses.Length} && ${Select[${NearestSpawn[${i},corpse radius ${seek_Radius}].ID},${looted_Corpses.Replace[-,,]}]}) /goto :skip_Loot
				
				/call loot_It ${NearestSpawn[${i},corpse radius ${seek_Radius}].ID}
				/if (${Macro.Return.Equal[JUST_LOOTED]}) /delay 3
				
				:skip_Loot
				
			/next i
			
	|-------------------------------------------------------------Else, if there is no free inventory space, alert the user, and turn off looting.	
		} else /if (!${Me.FreeInventory}) {
			/declare a int local
			/for a 1 to ${Loot_Alert}
				/beep 1
				/delay 3
			/next a
			/varset NinjaLoot_On FALSE
			/docommand ${ChatToggle} I'm full, turning loots off.
		}
	}
	
/if (${Debug}) /echo <== Event_YourKill -|
/RETURN



|------------------------------------------------------------------------|
|- Moves to corpse and begins looting, reads Loot_Ini for loot handles.	-|
|------------------------------------------------------------------------|
|- @param corpseID: ID of the corpse you are trying to loot.			-|
|------------------------------------------------------------------------|
SUB loot_It(int corpseID)
/varset Debug 1
/if (${Debug}) /echo |- loot_It ==>

|-------------------------------------------------------------------Check to see if corpseID belongs to an NPC (NPC corpses have _'s in the name).
	/declare corpseName ${Spawn[id ${corpseID}]}
	/if (${Spawn[id ${corpseID}].LineOfSight} && ${corpseName.Find[_]}) {
	
	|--------------------------------------------------------------------Pause follow functions, and target corpseID
		/if (${NetAdvPath.Status}==1) /squelch /netfollow pause
		/call TrueTarget ${corpseID}
		
	|-------------------------------------------------------------------------If corpse is too far away, move to it.
		/if (${Spawn[id ${corpseID}].Distance} > 20) {
			
		|------------------------------------------------------------------------------------------Engage MoveUtils.
			/squelch /moveto ID ${corpseID}
			/delay 5 ${Me.Moving}
			/declare movementTimer timer local 5s
			
		|---------------------------------------------------------------------------------------Wait to stop moving.
			:moving
			/if (${Debug}) /echo |- loot_It -| :moving
			/delay 1
			/call Background_Events
			/if (${Macro.Return.Equal[SKIP_LOOT]}) /goto :SKIP_LOOT
			/if (${Me.Moving} && ${movementTimer}) /goto :moving
		}
		
	|------------------------------------------------------------------------------------------------------------If corpseID is in range, loot it.
		/if (${Spawn[id ${corpseID}].Distance} < 20 && ${Spawn[id ${corpseID}].DistanceZ} < 20) {
		
		|-------------------------------------------------------------------------If looter is a bard, pause melody.
			/if (${Twist.Twisting}) /call pauseTwist
			
			/loot
			
			/doevents NotYourKill
			/delay 20 ${Window[LootWnd].Open} || ${Macro.Return.Equal[SKIP_LOOT]}
			
			/if (${Macro.Return.Equal[SKIP_LOOT]}) /goto :SKIP_LOOT
			
		|----------------------------------------------------------------------------Count items on the corpse, loop until no new items are found.
			/declare lootTotal int local -1
			/declare x int local 0
			:autoxplootlag
			/varcalc x ${x}+1
			/if (${lootTotal}!=${Corpse.Items}) {
				/varset lootTotal ${Corpse.Items}
				/delay 5
				/if (${x}<5) /goto :autoxplootlag
			}
			
		|----------------------------------------------------------------------------Check each item on the corpse, and decide to keep or destroy.
			/if (${lootTotal}) {
				/declare i int local
				/declare e int local
				/declare iniEntryVariables string local
				/for i 1 to ${lootTotal}
					/delay 1s ${InvSlot[loot${i}].Item.ID} != 0
					/varset iniEntryVariables ${If[${Math.Calc[${InvSlot[loot${i}].Item.Value}/1000].Int},~${Math.Calc[${InvSlot[loot${i}].Item.Value}/1000].Int}p,${If[${Math.Calc[${InvSlot[loot${i}].Item.Value}/100].Int},~${Math.Calc[${InvSlot[loot${i}].Item.Value}/100].Int}g,${If[${Math.Calc[${InvSlot[loot${i}].Item.Value}/10].Int},~${Math.Calc[${InvSlot[loot${i}].Item.Value}/10].Int}s,${If[${Math.Calc[${InvSlot[loot${i}].Item.Value}/1].Int},~${Math.Calc[${InvSlot[loot${i}].Item.Value}/1].Int}c,]}]}]}]}${If[${InvSlot[loot${i}].Item.Stackable},(${InvSlot[loot${i}].Item.StackSize}),]}${If[${InvSlot[loot${i}].Item.NoDrop},(ND),]}${If[${InvSlot[loot${i}].Item.Container},(C),]}
					
				|--------------------------------------If there is no entry in the Loot_Ini for this item, make one.
					/if (!${Ini[${Loot_Ini},${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}${iniEntryVariables}].Length}) {
					
					|---------------------------Import settings from oldloot.ini
						/if (${Ini[Macro_Inis\OldLoot.ini,${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}].Length}) {
							/echo Autoloot: Importing ini setting [${InvSlot[loot${i}].Item.Name} (${Ini[Macro_Inis\OldLoot.ini,${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}]})]
							/call WriteToIni "${Loot_Ini},${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}${iniEntryVariables}" ${If[${InvSlot[loot${i}].Item.Container},Container,${If[${Ini[Macro_Inis\OldLoot.ini,${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}].Equal[NDITEM]},Skip,${If[${Ini[Macro_Inis\OldLoot.ini,${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}].Equal[keep]},Keep${If[${InvSlot[loot${i}].Item.Stackable},/${InvSlot[loot${i}].Item.StackSize},]},${If[${Ini[Macro_Inis\OldLoot.ini,${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}].Equal[destroy]},Destroy,IMPORT_FAILED]}]}]}]} 0
						} else {

							/call WriteToIni "${Loot_Ini},${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}${iniEntryVariables}" ${If[${InvSlot[loot${i}].Item.Container},Container,${If[${InvSlot[loot${i}].Item.NoDrop},Skip,Keep${If[${InvSlot[loot${i}].Item.Stackable},/${InvSlot[loot${i}].Item.StackSize},]}]}]}
							/echo Autoloot: Adding [${InvSlot[loot${i}].Item.Name} (${Ini[${Loot_Ini},${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}${iniEntryVariables}]})]
						}
					}
				
				|----------------------------------------------------------------------------------Destroy the item.
					/if (${Ini[${Loot_Ini},${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}${iniEntryVariables}].Find[Destroy]}) {
						/echo Autoloot: Destroying [${InvSlot[loot${i}].Item.Name}]
						/if (${InvSlot[loot${i}].Item.Lore} && ${FindItem[${InvSlot[loot${i}].Item.Name}].ID}) /goto :skip_Lore
						/call loot_Handle ${i} destroy

				|-------------------------------------------------------------------------------------Keep the item.
					} else /if (${Ini[${Loot_Ini},${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}${iniEntryVariables}].Find[Keep]} || ${Ini[${Loot_Ini},${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}${iniEntryVariables}].Find[Sell]}) {
	
					
					| change this to loop through the arguments in the ini entry, looking for on that passes ${Int[ini entry arg[i,/]}
					|---------------If I have an item limit, check my stock.
						/varset e 1
						:check_MaxStock
/echo /if (${Ini[${Loot_Ini},${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}${iniEntryVariables}].Arg[${e},/].Length})
						/if (${Ini[${Loot_Ini},${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}${iniEntryVariables}].Arg[${e},/].Length}) {
/echo /if (!${Int[${Ini[${Loot_Ini},${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}${iniEntryVariables}].Arg[${e},/]}]})
							/if (!${Int[${Ini[${Loot_Ini},${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}${iniEntryVariables}].Arg[${e},/]}]}) {
								/varcalc e ${e} + 1
								/goto :check_MaxStock
							} else {
/echo /if (${Math.Calc[${FindItemCount[${InvSlot[loot${i}].Item.Name}]} + 1].Int} > ${Ini[${Loot_Ini},${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}${iniEntryVariables}].Arg[2,/]}) 
								/if (${Math.Calc[${FindItemCount[${InvSlot[loot${i}].Item.Name}]} + 1].Int} > ${Ini[${Loot_Ini},${InvSlot[loot${i}].Item.Name.Left[1]},${InvSlot[loot${i}].Item.Name}${iniEntryVariables}].Arg[2,/]}) {
									/echo Autoloot: Fully stocked on [${InvSlot[loot${i}].Item.Name}], Skipping.
									/goto :skip_Lore
								}
							}
						}
						
					|----------------------------If the item is lore, make sure I don't have one.  If I do, skip it.
						/if (${InvSlot[loot${i}].Item.Lore} && ${FindItem[${InvSlot[loot${i}].Item.Name}].ID}) /goto :skip_Lore
						
					|-Check if, the item is lore and I don't have one, or, if the item is not no drop, or if the item is no drop and not lore.
						/if (${InvSlot[loot${i}].Item.Lore} && !${FindItem[${InvSlot[loot${i}].Item.Name}].ID} || !${InvSlot[loot${i}].Item.NoDrop} || ${InvSlot[loot${i}].Item.NoDrop} && !${InvSlot[loot${i}].Item.Lore}) {
							
						|----------------------------Check for free inventory, and that the item is not a container.	
							/if (${Me.FreeInventory} && !${InvSlot[loot${i}].Item.Container}) {
								/echo Autoloot: Looting [${InvSlot[loot${i}].Item.Name}]
								/call loot_Handle ${i} keep
								
						|----------------------If inventory is full, alert the user, and turn off looting functions.
							| change this to continue checking corpses, only linking, instead of looting.
	
							} else /if (!${Me.FreeInventory}) {
								/declare a int local
								/for a 1 to ${Loot_Alert}
									/beep 1
									/delay 3
								/next a
								/varset NinjaLoot_On FALSE
								/docommand ${ChatToggle} I'm full, turning loots off.
							}
						}
					}
					:skip_Lore
				/next i
				
			|-------------------------------------------------If I've finished looting, and there are still items on the corpse, announce in chat.
				/if (${Corpse.Items}) {
					
					/declare linkList string local 
					
					/for i 1 to ${lootTotal}
					
						/if (${InvSlot[loot${i}].Item.ID}) /noparse /varset linkList ${LinkDB[${InvSlot[loot${i}].Item.Name}]}
					
					/next i
					
					/echo ${linkList}
				|**
					/notify LootWND LW_BroadcastButton leftmouseup
					/keypress enter chat
				**|
				|--------------------------------------------------------------Add corpseID to looted_Corpses array.
					:SKIP_LOOT
					/varset looted_Corpses ${looted_Corpses}${corpseID}-
					
				}
			}
			
		|--------------------------------------------------------------------------------------------------------Click 'Done' and return to group.
			/if (${Corpse.Open}) /notify LootWnd DoneButton leftmouseup 
			/if (${returnTwist}) /call unpauseTwist
		}
		/if (${Following}) /call AquireFollow
	}
	
/if (${Debug}) /echo <== loot_It -|
/varset Debug 0
/RETURN JUST_LOOTED



|--------------------------------------------------------------------|
|- Loots items mobs you have killed, to destroy or autoinventory.	-|
|--------------------------------------------------------------------|
|- @param slotNum: The slot number you are looting from.			-|
|- @param handle: What to do with the item once you loot it.		-|
|--------------------------------------------------------------------|
SUB loot_Handle(int slotNum, handle)
/if (${Debug}) /echo |- loot_Handle ==>

	/declare i int local
	:retry_Loot
	/if (${Debug}) /echo |- loot_Handle -| :retry_Loot
	
|--------------------------------------------------------------------------------------------------Try to loot the specified item from the corpse.
	/itemnotify loot${slotNum} leftmouseup
	/delay 5 ${Cursor.ID} || ${Window[ConfirmationDialogBox].Open}
	
|------------------------------------------------------------If the item has been looted, decide what to do with it.
	/if (${Cursor.ID}) {
	
	|----------------------------------------------------------------------------------------------Destroy the item.
		/if (${handle.Equal[destroy]}) {
			:destroy_Item
			/varcalc i ${i}+1
			/destroy
			
		|--------------------------If the destroy attempt was unsuccessful, try again.	
			/if (${Cursor.ID} && ${i}<10) /goto :destroy_Item
		
	|-------------------------------------------------------------------------------------------Else, keep the item.
		} else {
			/call ClearCursor
		}

|-----------------------------------------------------------------Else, if loot attempt was unsuccessful, try again.		
	} else {
	
	|---------------------------------------------------------------------------------------------------------------Click yes on confirmation box.
		/if (${Window[ConfirmationDialogBox].Open}) /notify ConfirmationDialogBox Yes_Button leftmouseup
		/varcalc i ${i}+1
		/if (${i}<10) /goto :retry_Loot
	}
	
/if (${Debug}) /echo <== loot_Handle -|
/RETURN	



|----------------------------------------------------------------|
|- Adds corpses that are not yours to the looted corpse list.	-|
|----------------------------------------------------------------|
#EVENT NotYourKill "Someone is already looting that corpse."
#EVENT NotYourKill "You may not loot this Corpse at this time."
#EVENT NotYourKill "You must first target a corpse to loot!"
SUB EVENT_NotYourKill
/if (${Debug}) /echo |- EVENT_NotYourKill ==>

	/if (${Defined[looted_Corpses]}) /varset looted_Corpses ${looted_Corpses}${corpseID}-
	
/if (${Debug}) /echo <== EVENT_NotYourKill -|
/RETURN SKIP_LOOT



|---------------
|- Check Loot Setting
|-----------------
#EVENT checkLootSetting "<#*#> Check Loot Setting for #1#"
#EVENT checkLootSetting "[MQ2] Check Loot Setting for #1#"
SUB EVENT_checkLootSetting(line, itemToCheck)
/if (${Debug}) /echo |- EVENT_checkLootSetting ==>

	/declare i int local 1
	
	:check_Loot_Ini
	
	/if (!${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Arg[1,${If[${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Find[~]},~,(]}].Equal[${itemToCheck}]}) {
		/varcalc i ${i} + 1
		/if (${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Length}) {
			/goto :check_Loot_Ini
		} else {
			/echo No entry found.
		}
	} else {
		/echo Loot_Ini entry for [${itemToCheck}] is [${Ini[${Loot_Ini},${itemToCheck.Left[1]},${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|]}]}]
	}
	
/if (${Debug}) /echo <== EVENT_checkLootSetting -|
/RETURN



|---------------
|- Update Loot Setting
|-----------------
#EVENT updateLootSetting "<#3#> Set Loot entry for #1# #2#"
#EVENT updateLootSetting "[MQ2] Set Loot entry for #1# #2#"
SUB EVENT_updateLootSetting(line, itemToCheck, iniSetting, ChatSender)
/if (${Debug}) /echo |- EVENT_updateLootSetting ==>

	/if (${ChatSender.Equal[${Me.CleanName}]} || !${Defined[ChatSender]}) {
		/declare i int local 1
		
		:check_Loot_Ini
		/if (!${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Arg[1,${If[${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Find[~]},~,(]}].Equal[${itemToCheck}]}) {
			/varcalc i ${i} + 1
			/if (${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|].Length}) {
				/goto :check_Loot_Ini
			} else {
				/echo No entry found.
			}
		} else {
			/call WriteToIni "${Loot_Ini},${itemToCheck.Left[1]},${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|]}" ${iniSetting} 1
			/echo Loot_Ini entry for [${itemToCheck}] has been updated to [${Ini[${Loot_Ini},${itemToCheck.Left[1]},${Ini[${Loot_Ini},${itemToCheck.Left[1]}].Arg[${i},|]}]}].
		}
	}
	
/if (${Debug}) /echo <== EVENT_updateLootSetting -|
/RETURN



|----------------------------|
|- Setup looting variables.	-|
|----------------------------|
SUB setup_Loot
/if (${Debug}) /echo |- setup_Loot ==>

|-----------------------------------------------------------------------------------------------------------------------Import Macro_Ini settings.
	/call INItoVar "${Macro_Ini},AutoLoot,Looters" Looters string outer
	
	/if (${Select[${Me.CleanName},${Looters.Replace[/,,]}]}) {
		/call INItoVar "${Macro_Ini},Loot,Auto-Loot (On/Off)" ${Auto-Loot} bool outer
		/call INItoVar "${Macro_Ini},Loot,Loot Link Channel" LinkChannel string outer
		/call INItoVar "${Macro_Ini},Loot,Corpse Seek Radius" SeekRadius string outer

	|-------------------------------------------------------------------------------------------------------------------------------Check MQ2Exchange.
		/if (!${Plugin[MQ2LinkDB].Name.Length}) {
			/echo Plugin MQ2LinkDB is not loaded, attempting to resolve...
			/plugin MQ2LinkDB noauto
			/delay 3s ${Plugin[MQ2LinkDB].Name.Length}
			/if (${Plugin[MQ2Exchange].Name.Length}) {
				/link /import
			} else {
				/echo ***WARNING*** Could not load MQ2LinkDB, item links will only be given in say.
				/varset LinkChannel PLUGIN-ERROR
			}
		}
		
	|------------------------------------------------------------------------------------------------------------------------------Setup Loot_Ini.
		/if (!${Defined[Loot_Ini]}) /declare Loot_Ini string outer loot.ini
		/if (!${Ini[${Loot_Ini}].Length}) /call Build_Alphabetized_Ini ${Loot_Ini}

	|--------------------------------------------------------------------------------------------------------------------Create looting variables.
		| Unused at this time --- /declare Loot_OutPut string outer /gu
		/declare looted_Corpses string outer
	}

/if (${Debug}) /echo <== setup_Loot -|
/RETURN