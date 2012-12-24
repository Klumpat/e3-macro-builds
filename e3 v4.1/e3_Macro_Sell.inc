|------------------------------------------------------------------------|
|- e3_Macro_Sell.inc v1.5 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Companion file for e3_Macro_Loot.inc.								-|
|- Sells items flagged with [/Sell] in your Loot_Ini.					-|
|------------------------------------------------------------------------|



|----------------------------------------------------|
|- syncInventory									-|
|----------------------------------------------------|
|- Adds items from you inventory to your Loot_Ini.	-|
|----------------------------------------------------|
#EVENT syncInventory "<#*#> Inventory Items"
#EVENT syncInventory "[MQ2] Inventory Items"
#EVENT syncInventory "<#*#> Update Loot.ini"
#EVENT syncInventory "[MQ2] Update Loot.ini"
#EVENT syncInventory "<#*#> Sync Inventory"
#EVENT syncInventory "[MQ2] Sync Inventory"
#EVENT syncInventory "<#*#> Sync Loot.ini"
#EVENT syncInventory "[MQ2] Sync Loot.ini"
SUB EVENT_syncInventory
/if (${Debug}) /echo |- event_inventoryItems ==>
	
	/echo Syncing inventory with ${Loot_Ini}...

	/declare i int local
	/declare x int local
	
	/declare iniEntryVariables string local
	/declare itemName string local
	/declare newIniSetting string local
	
	/declare LastItemName string local
	
|-----------------------------------------------------------------------------Check inventory slots 1 - 8 for items, and containers.
	/for i 1 to 8
	
	|------------------------------------------------------------------------If there is an item in inventory slot ${i}
		/if (${InvSlot[pack${i}].Item.ID}) {
		
		|-----------------------------------------------------------Set variables...
			/varset itemName ${InvSlot[pack${i}].Item.Name}
			/varset iniEntryVariables ${If[${Math.Calc[${InvSlot[pack${i}].Item.Value}/1000].Int},~${Math.Calc[${InvSlot[pack${i}].Item.Value}/1000].Int}p,${If[${Math.Calc[${InvSlot[pack${i}].Item.Value}/100].Int},~${Math.Calc[${InvSlot[pack${i}].Item.Value}/100].Int}g,${If[${Math.Calc[${InvSlot[pack${i}].Item.Value}/10].Int},~${Math.Calc[${InvSlot[pack${i}].Item.Value}/10].Int}s,${If[${Math.Calc[${InvSlot[pack${i}].Item.Value}/1].Int},~${Math.Calc[${InvSlot[pack${i}].Item.Value}/1].Int}c,]}]}]}]}${If[${InvSlot[pack${i}].Item.Stackable},(${InvSlot[pack${i}].Item.StackSize}),]}${If[${InvSlot[pack${i}].Item.NoDrop},(ND),]}${If[${InvSlot[pack${i}].Item.Container},(C),]}
		
		|--------------------------------------------------------If there is no entry listed in the Loot.ini, for this item, make one.
			/if (!${Ini[${Loot_Ini},${itemName.Left[1]},${itemName}${iniEntryVariables}].Length}) {
			
			|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------Import setting from the oldLoot_Ini.
				/if (${Ini[${oldLoot_Ini},${itemName.Left[1]},${itemName}].Length}) {
					/echo Importing setting for [${itemName}] - [${Ini[${oldLoot_Ini},${itemName.Left[1]},${itemName}]}].
					/varset newIniSetting ${If[${Ini[${oldLoot_Ini},${itemName.Left[1]},${itemName}].Equal[NDITEM]},Skip,${Ini[${oldLoot_Ini},${itemName.Left[1]},${itemName}]}]}
				} else {
					/varset newIniSetting ${If[${InvSlot[pack${i}].Item.Container},Container,${If[${InvSlot[pack${i}].Item.NoDrop},Skip,Keep]}]}
				}
				
			|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------If the new item is stackable, add a stackcount to the entry.
				/if (${InvSlot[pack${i}].Item.Stackable} && ${InvSlot[pack${i}].Item.StackSize} > 1) /varset newIniSetting ${newIniSetting}/${InvSlot[pack${i}].Item.StackSize}
				
			|---------------------------------------------------------------------------------------------------------------------Add new entry.
				/call WriteToIni "${Loot_Ini},${itemName.Left[1]},${itemName}${iniEntryVariables}" ${newIniSetting} 0
				/echo Adding [${itemName}] to [${Loot_Ini}] - [${Ini[${Loot_Ini},${itemName.Left[1]},${itemName}${iniEntryVariables}]}].
			}
			
		|----------------------------------------------------------------------------------------------------------------------------------------------If the item is a container..
			/if (${InvSlot[pack${i}].Item.Container}) {	
			
				/for x 1 to ${InvSlot[pack${i}].Item.Container}
					
					/if (${Me.Inventory[pack${i}].Item[${x}].ID}) {
					
						/varset itemName ${Me.Inventory[pack${i}].Item[${x}]}
						/varset iniEntryVariables ${If[${Math.Calc[${Me.Inventory[pack${i}].Item[${x}].Value}/1000].Int},~${Math.Calc[${Me.Inventory[pack${i}].Item[${x}].Value}/1000].Int}p,${If[${Math.Calc[${Me.Inventory[pack${i}].Item[${x}].Value}/100].Int},~${Math.Calc[${Me.Inventory[pack${i}].Item[${x}].Value}/100].Int}g,${If[${Math.Calc[${Me.Inventory[pack${i}].Item[${x}].Value}/10].Int},~${Math.Calc[${Me.Inventory[pack${i}].Item[${x}].Value}/10].Int}s,${If[${Math.Calc[${Me.Inventory[pack${i}].Item[${x}].Value}/1].Int},~${Math.Calc[${Me.Inventory[pack${i}].Item[${x}].Value}/1].Int}c,]}]}]}]}${If[${Me.Inventory[pack${i}].Item[${x}].Stackable},(${Me.Inventory[pack${i}].Item[${x}].StackSize}),]}${If[${Me.Inventory[pack${i}].Item[${x}].NoDrop},(ND),]}${If[${Me.Inventory[pack${i}].Item[${x}].Container},(C),]}

					|--------------------------------------------------------If there is no entry listed in the Loot.ini, for this item, make one.
						/if (!${Ini[${Loot_Ini},${itemName.Left[1]},${itemName}${iniEntryVariables}].Length}) {
						
						|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------Import setting from the oldLoot_Ini.
							/if (${Ini[${oldLoot_Ini},${itemName.Left[1]},${itemName}].Length}) {
								/echo Importing setting for [${itemName}] - [${Ini[${oldLoot_Ini},${itemName.Left[1]},${itemName}]}].
								/varset newIniSetting ${If[${Ini[${oldLoot_Ini},${itemName.Left[1]},${itemName}].Equal[NDITEM]},Skip,${Ini[${oldLoot_Ini},${itemName.Left[1]},${itemName}]}]}
							} else {
								/varset newIniSetting ${If[${Me.Inventory[pack${i}].Item[${x}].Container},Container,${If[${Me.Inventory[pack${i}].Item[${x}].NoDrop},Skip,Keep]}]}
							}
							
						|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------If the new item is stackable, add a stackcount to the entry.
							/if (${Me.Inventory[pack${i}].Item[${x}].Stackable} && ${Me.Inventory[pack${i}].Item[${x}].StackSize} > 1) /varset newIniSetting ${newIniSetting}/${Me.Inventory[pack${i}].Item[${x}].StackSize}	
						
						|---------------------------------------------------------------------------------------------------------------------Add new entry.
							/call WriteToIni "${Loot_Ini},${itemName.Left[1]},${itemName}${iniEntryVariables}" ${newIniSetting} 0
							/echo Adding [${itemName}] to [${Loot_Ini}] - [${Ini[${Loot_Ini},${itemName.Left[1]},${itemName}${iniEntryVariables}]}].
						
						}
					}
			
				/next x
			}
		}
	
	/next i
	
/if (${Debug}) /echo <== event_inventoryItems -|
/RETURN



|------------------------------------------------------------------------------------------------------------------------------------|
|- EVENT_sellItems																													-|
|------------------------------------------------------------------------------------------------------------------------------------|
|- Commands bots, including yourself, to open trade with the nearest merchant, and sell items flagged as [/Sell] in your Loot_Ini.	-|
|------------------------------------------------------------------------------------------------------------------------------------|
#EVENT sellItems "[MQ2] Sell Items"
#EVENT sellItems "<#*#> Sell Items"
#EVENT sellItems "[MQ2] AutoSell"
#EVENT sellItems "<#*#> AutoSell"
#EVENT sellItems "[MQ2] Auto Sell"
#EVENT sellItems "<#*#> Auto Sell"
SUB EVENT_sellItems
/if (${Debug}) /echo |- EVENT_sellItems ==>
	
	|--------------------------------------------------------Record starting location.
		/declare startingLoc string local ${Me.Loc.Replace[ ,]}
		
	|----------------Sync inventory with Loot_Ini.
		/call EVENT_syncInventory
		
	|-------------------------------------------------------Open trade with the nearest merchant.
		/call openMerchant
		/if (${Window[MerchantWnd].Open}) {
		
		|--------------------------------------------------------------Open bags.
			/call openBags
		
		|-----------------------------------------------------------------Sell Items.
			/call sellItems
			
		|------------------------------------------------------------Move back to starting location.	
			/squelch /moveto loc ${startingLoc.Arg[1,,]} ${startingLoc.Arg[2,,]}
			
		|---------------------------------------------------------Close merchant.
			/if (${Window[MerchantWnd].Open}) /call closeMerchant
		
		|-------------------------------------------------------------Destroy bad items.
			/if (${DestroyableItems.Size}) /call destroyItems
		
		|------------------------------------------------------------Close bags.
			/call closeBags
			/if (${Window[InventoryWindow].Open}) /keypress i 
			
			/echo I have finished selling.
		}
	
/if (${Debug}) /echo <== EVENT_sellItems -|
/RETURN



|----------------------------------------|
|- Opens all containers in inventory.	-|
|----------------------------------------|
SUB openBags
/if (${Debug}) /echo |- openBags ==>

|-------------------------------------------Open all bags in inventory.
	/declare i int local
	/declare retryTimer timer local
	/declare miscTimer timer local
	
	/for i 1 to 8
		
		/varset miscTimer 30
		/if (${Me.Inventory[pack${i}].Container}) {
			
		:openBag
		/if (${Debug}) /echo |- openBags -| :openBag
		|---------------------------------------------------------------------------------Open container
			/if (!${Window[pack${i}].Open}) {
				/itemnotify pack${i} rightmouseup
				/delay 5 ${Window[pack${i}].Open} || ${Window[TradeSkillWnd].Open}
				/if (!${Window[pack${i}].Open} && !${Window[TradeSkillWnd].Open} && ${miscTimer}) /goto :openBag
				
			|--------------------------------------------------------Open tradeskill container.
				/if (${Window[TradeSkillWnd].Open}) {
					:openExperimentWnd
					/notify TradeSkillWnd ExperimentButton leftmouseup
					/delay 5 !${Window[TradeSkillWnd].Open}
					/if (${Window[TradeSkillWnd].Open} && ${miscTimer}) /goto :openExperimentWnd
				}
			}
		}
	
	/next i
	
/if (${Debug}) /echo <== openBags -|
/RETURN



|----------------------------------------|
|- Closes all containers in inventory.	-|
|----------------------------------------|
SUB closeBags
/if (${Debug}) /echo |- closeBags ==>

|-------------------------------------------Close all bags in inventory.
	/declare i int local
	/declare retryTimer timer local
	/declare miscTimer timer local
	
	/for i 1 to 8
		
		/varset miscTimer 30
		/if (${Me.Inventory[pack${i}].Container}) {
			
		:closeBag
		/if (${Debug}) /echo |- closeBags -| :closeBag
		
		|---------------------------------------------------------------------------------Open container
			/if (${Window[pack${i}].Open}) {
				/itemnotify pack${i} rightmouseup
				/delay 5 !${Window[pack${i}].Open}
				/if (${Window[pack${i}].Open} && ${miscTimer}) /goto :closeBag
			}
		}
	
	/next i
	
/if (${Debug}) /echo <== closeBags -|
/RETURN



|--------------------------------------------|
|- Opens trade with the nearest merchant.	-|
|--------------------------------------------|
SUB openMerchant
/if (${Debug}) /echo |- openMerchant ==>

	/declare retryTimer timer local
	/declare miscTimer timer local 
	
	/if (!${Spawn[Merchant].ID} || !${Spawn[Merchant].LineOfSight}) {
		/echo There are no merchants nearby!
	} else {
		
		/call TrueTarget ${Spawn[Merchant].ID}
	
	|------------------------------------------------------------------------------------------------------------------------------Move to the merchant.
		:movetoMerchant
		/if (${Debug}) /echo |- openMerchant -| :movetoMerchant
		
		/if (${Target.Distance} > 20) {
			
		|-----------------------------------------------------------------------Set timers and engage /moveto.
			/varset retryTimer 300
			/varset miscTimer 100
			
			/squelch /moveto id ${Target.ID} dist 10
			/delay 5 ${Me.Moving}
			
		|---------------------------------------------------------------------------------Wait to stop moving.
			:moving
			/if (${Debug}) /echo |- openMerchant -| Move to merchant loop.
			/delay 1
			/if (${Me.Moving} && ${miscTimer}) /goto :moving
			
		|-----------------------------------------------------------------------If I'm still not close enough.
			/if (${Target.Distance} > 20) {
				
			|-----------------------------------------------------------------------If I have time, try again.
				/if (${retryTimer}) {
					/goto :movetoMerchant
				} else {
				
				|-------------------------------------------------------------If I'm out of time, output and exit.
					/echo Failed to move to the nearest merchant.  Move me near a merchant, and try again.
				}
			}
		}
		
	|--------------------------------------------------------------------------------------------------------------------------Open Trade with merchant.
		/if (${Target.Distance} < 20) {
			
		|----------------------------------------------------------------------Set timers, and declare variables.
			/varset miscTimer 100
			/declare merchantTotal int local
			
		|------------------------------------------------------Right click merchant, and wait for window to open.
			:openMerchant
			/if (${Debug}) /echo |- openMerchant -| :openMerchant
			
			/if (!${Window[MerchantWnd].Open}) /click right target
			/delay 5 ${Window[MerchantWnd].Open}
			/if (!${Window[MerchantWnd].Open}) {
				/if (${miscTimer}) {
					/goto :openMerchant
				} else {
					/echo Failed to open trade with [${Target.CleanName}].  Please try again.
				}
			}
			
		|-------------------------------------------------------------Wait for merchant's item list to populate.
			/varset miscTimer 100
			/varset merchantTotal -1
			
			:populateMerchant
			/if (${Debug}) /echo |- openMerchant -| :populateMerchant
			/if (${merchantTotal}!=${Window[MerchantWnd].Child[ItemList].Items}) {
				/varset merchantTotal ${Window[MerchantWnd].Child[ItemList].Items}
				/delay 2
				/if (${miscTimer}) /goto :populateMerchant
			}
		}
	}

/if (${Debug}) /echo <== openMerchant -|
/RETURN



|--------------------|
|- closeMerchant	-|
|--------------------|
SUB closeMerchant
/if (${Debug}) /echo |- closeMerchant ==>

	/declare miscTimer timer local 50
	
	:closeMerchant
	/if (${Debug}) /echo |- closeMerchant -| :closeMerchant
	/notify MerchantWnd MW_Done_Button leftmouseup
	/delay 5 ${Window[MerchantWnd].Open}
	/if (${Window[MerchantWnd].Open} && ${miscTimer}) /goto :closeMerchant
	
	
/if (${Debug}) /echo <== closeMerchant -|
/RETURN



|----------------------------------------------------------------------------------------|
|- Sells items in your inventory that have been flagged as [/Sell] in your Loot_Ini.	-|
|----------------------------------------------------------------------------------------|
SUB sellItems
| /varset Debug TRUE
/if (${Debug}) /echo |- sellItems ==>

	/declare i int local
	/declare e int local
	
	/declare retryTimer timer local
	/declare miscTimer timer local

	/declare iniEntryVariables string local
	
|-------------------------------------------------------------------------------------------Scan inventory for items to sell.
	/for i 1 to 8

		/if (${InvSlot[pack${i}].Item.ID}) {
		
		|--------------------------------------------------------------------------------------If the item is a container, look through it's contents.
			/if (!${InvSlot[pack${i}].Item.Container}) {

			|------------------------------------------------Set ini entry variables per item.
				/varset iniEntryVariables ${If[${Math.Calc[${Me.Inventory[pack${i}].Value}/1000].Int},~${Math.Calc[${Me.Inventory[pack${i}].Value}/1000].Int}p,${If[${Math.Calc[${Me.Inventory[pack${i}].Value}/100].Int},~${Math.Calc[${Me.Inventory[pack${i}].Value}/100].Int}g,${If[${Math.Calc[${Me.Inventory[pack${i}].Value}/10].Int},~${Math.Calc[${Me.Inventory[pack${i}].Value}/10].Int}s,${If[${Math.Calc[${Me.Inventory[pack${i}].Value}/1].Int},~${Math.Calc[${Me.Inventory[pack${i}].Value}/1].Int}c,]}]}]}]}${If[${Me.Inventory[pack${i}].Stackable},(${Me.Inventory[pack${i}].StackSize}),]}${If[${Me.Inventory[pack${i}].NoDrop},(ND),]}${If[${Me.Inventory[pack${i}].Container},(C),]}

				/if (${Ini[${Loot_Ini},${InvSlot[pack${i}].Item.Name.Left[1]},${InvSlot[pack${i}].Item.Name}${iniEntryVariables}].Find[Destroy]}) {
					/echo Flagging [${InvSlot[pack${i}].Item.Name}] to be destroyed...
					/call BuildArray DestroyableItems "${InvSlot[pack${i}].Item.Name}"
					
			|-----------------------------------------Search Loot_Ini for items flagged [/Sell].
				} else /if (${Ini[${Loot_Ini},${InvSlot[pack${i}].Item.Name.Left[1]},${InvSlot[pack${i}].Item.Name}${iniEntryVariables}].Find[Sell]}) {
				
					/if (!${InvSlot[pack${i}].Item.Value}) {
						/echo [${Target.CleanName}] will not buy [${InvSlot[pack${i}].Item.Name}].  Flagging it to be destroyed.
						/call BuildArray DestroyableItems "${InvSlot[pack${i}].Item.Name}"
					} else {
						
					|--------------------------------------------------------------------------------------------------------Select the item to sell
						/varset retryTimer 30

						:retrySelectx
						/if (${Debug}) /echo |- sellItems -| :retrySelectx
								
						/itemnotify in pack${i} leftmouseup
						/delay 10 ${SelectedItem.ID}==${FindItem[=${Me.Inventory[pack${i}]}].ID}
								
						/if (${SelectedItem.ID} != ${FindItem[=${Me.Inventory[pack${i}].Item[${e}]}].ID}) {
							/if (${retryTimer}) {
								/goto :retrySelectx
							} else {
								/echo ERROR: Failed to select [${Me.Inventory[pack${i}].Item[${e}]}], skipping.
							}
						} else {
							/if (!${Window[MerchantWnd].Child[MW_Sell_Button].Enabled}) {
								/echo [${Target.CleanName}] will not buy [${Me.Inventory[pack${i}]}].  Flagging it to be destroyed.
								/call BuildArray DestroyableItems "${Me.Inventory[pack${i}]}"
							} else {

								/echo Selling [${Me.Inventory[pack${i}]}].
										
								/varset retryTimer 50
								
								:SellItem
								/if (${Debug}) /echo |- sellItems -| :SellItem
								
								/notify MerchantWnd MW_Sell_Button leftmouseup
								/delay 5 ${Window[QuantityWnd].Open} || !${Bool[${Me.Inventory[pack${i}]}]}
								
							|----------------------------------------------If the quantity window is open.
								/if (${Window[QuantityWnd].Open}) {
									/notify QuantityWnd QTYW_Accept_Button leftmouseup
									/delay 10 !${Window[QuantityWnd].Open}
								}
								
								/delay 5 !${Bool[${Me.Inventory[pack${i}]}]}
							
								/if (${Me.Inventory[pack${i}]} && ${retryTimer}) {
									/goto :SellItem
								} else {
									/echo ERROR: Failed to sell [${Me.Inventory[pack${i}]}], skipping.
								}
							}
						}
					}
				}
			} else {
				
				/for e 1 to ${Me.Inventory[pack${i}].Container}
				
				|------------------------------------------------------------------------------------------------------Check that there is an item in this slot.
					/if (${Me.Inventory[pack${i}].Item[${e}].ID}) {
			
					|------------------------------------------------Set ini entry variables per item.
						/varset iniEntryVariables ${If[${Math.Calc[${Me.Inventory[pack${i}].Item[${e}].Value}/1000].Int},~${Math.Calc[${Me.Inventory[pack${i}].Item[${e}].Value}/1000].Int}p,${If[${Math.Calc[${Me.Inventory[pack${i}].Item[${e}].Value}/100].Int},~${Math.Calc[${Me.Inventory[pack${i}].Item[${e}].Value}/100].Int}g,${If[${Math.Calc[${Me.Inventory[pack${i}].Item[${e}].Value}/10].Int},~${Math.Calc[${Me.Inventory[pack${i}].Item[${e}].Value}/10].Int}s,${If[${Math.Calc[${Me.Inventory[pack${i}].Item[${e}].Value}/1].Int},~${Math.Calc[${Me.Inventory[pack${i}].Item[${e}].Value}/1].Int}c,]}]}]}]}${If[${Me.Inventory[pack${i}].Item[${e}].Stackable},(${Me.Inventory[pack${i}].Item[${e}].StackSize}),]}${If[${Me.Inventory[pack${i}].Item[${e}].NoDrop},(ND),]}${If[${Me.Inventory[pack${i}].Item[${e}].Container},(C),]}
						
						/if (${Ini[${Loot_Ini},${Me.Inventory[pack${i}].Item[${e}].Name.Left[1]},${Me.Inventory[pack${i}].Item[${e}]}${iniEntryVariables}].Find[Destroy]}) {
							/echo Flagging [${Me.Inventory[pack${i}].Item[${e}]}] to be destroyed...
							/call BuildArray DestroyableItems "${Me.Inventory[pack${i}].Item[${e}]}"
							
					|-----------------------------------------Search Loot_Ini for items flagged [/Sell].
						} else /if (${Ini[${Loot_Ini},${Me.Inventory[pack${i}].Item[${e}].Name.Left[1]},${Me.Inventory[pack${i}].Item[${e}]}${iniEntryVariables}].Find[Sell]}) {
							
						|--------------------------------------------------------------------------------------------------------Check that the item has value.
							/if (!${Me.Inventory[pack${i}].Item[${e}].Value}) {
								/echo [${Target.CleanName}] will not buy [${Me.Inventory[pack${i}].Item[${e}]}].  Flagging it to be destroyed.
								/call BuildArray DestroyableItems "${Me.Inventory[pack${i}].Item[${e}]}"
							} else {
							
							|--------------------------------------------------------------------------------------------------------Select the item to sell
								/varset retryTimer 30

								:retrySelect
								/if (${Debug}) /echo |- sellItems -| :retrySelect
								
								/itemnotify in pack${i} ${e} leftmouseup
								/delay 10 ${SelectedItem.ID}==${FindItem[=${Me.Inventory[pack${i}].Item[${e}]}].ID}
								
								/if (${SelectedItem.ID} != ${FindItem[=${Me.Inventory[pack${i}].Item[${e}]}].ID}) {
									/if (${retryTimer}) {
										/goto :retrySelect
									} else {
										/echo ERROR: Failed to select [${Me.Inventory[pack${i}].Item[${e}]}], skipping.
									}
								} else {
								
									/if (!${Window[MerchantWnd].Child[MW_Sell_Button].Enabled}) {
										/echo [${Target.CleanName}] will not buy [${Me.Inventory[pack${i}].Item[${e}]}].  Flagging it to be destroyed.
										/call BuildArray DestroyableItems "${Me.Inventory[pack${i}].Item[${e}]}"
									} else {
									
										/echo Selling [${Me.Inventory[pack${i}].Item[${e}]}].
										
										/varset retryTimer 50
										
										:SellItem_Pack
										/if (${Debug}) /echo |- sellItems -| :SellItem_Pack
										
										/notify MerchantWnd MW_Sell_Button leftmouseup
										/delay 5 ${Window[QuantityWnd].Open} || !${Bool[${Me.Inventory[pack${i}].Item[${e}]}]}
										
									|----------------------------------------------If the quantity window is open.
										/if (${Window[QuantityWnd].Open}) {
											/notify QuantityWnd QTYW_Accept_Button leftmouseup
											/delay 10 !${Window[QuantityWnd].Open}
										}
										
										/delay 5 !${Bool[${Me.Inventory[pack${i}].Item[${e}]}]}
									
										/if (${Me.Inventory[pack${i}].Item[${e}]} && ${retryTimer}) {
											/goto :SellItem_Pack
										} else {
											/echo ERROR: Failed to sell [${Me.Inventory[pack${i}].Item[${e}]}].
										}
									}
								}
							}
						}
					}
				/varset iniEntryVariables
				/next e
			}
		}
	/varset iniEntryVariables
	/next i
	
/if (${Debug}) /echo <== sellItems -|
| /varset Debug FALSE
/RETURN



SUB destroyItems
/if (${Debug}) /echo |- destroyItem ==>
	
	/declare i int local
	/declare e int local
	/declare y int local
	
	/declare retryTimer timer local
	/declare badItem string local
	
	/for i 1 to ${DestroyableItems.Size}
	
		/varset badItem ${DestroyableItems[${i}]}

		/for e 1 to 8
		
			/if (!${Me.Inventory[pack${e}].Container}) {
				
				/if (${badItem.Equal[${Me.Inventory[pack${e}]}]}) {
					/varset retryTimer 50
					
					:retry_SlotPickup
					/itemnotify pack${e} leftmouseup
					/delay 5 ${Cursor.ID} || ${Window[QuantityWnd].Open}
					
				|----------------------------------------------If the quantity window is open.
					/if (${Window[QuantityWnd].Open}) {
						/notify QuantityWnd QTYW_Accept_Button leftmouseup
						/delay 10 !${Window[QuantityWnd].Open}
					}
					
					/delay 5 ${Cursor.ID}
					
					/if (${badItem.Equal[${Cursor}]}) {
						/echo Destroying [${Cursor}].
						/destroy
					} else {
						/if (${retryTimer}) {
							/goto :retry_SlotPickup
						} else {
							/echo ERROR: I failed to destroy [${badItem}], skipping.
						}
					}
				}
			} else {
			
				/for y 1 to ${Me.Inventory[pack${e}].Container}
				
					/if (${badItem.Equal[${Me.Inventory[pack${e}].Item[${y}]}]}) {
					
						/varset retryTimer 50
					
						:retry_BagPickup
						/itemnotify in pack${e} ${y} leftmouseup
						/delay 5 ${Cursor.ID} || ${Window[QuantityWnd].Open}
						
					|----------------------------------------------If the quantity window is open.
						/if (${Window[QuantityWnd].Open}) {
							/notify QuantityWnd QTYW_Accept_Button leftmouseup
							/delay 10 !${Window[QuantityWnd].Open}
						}
						
						/delay 5 ${Cursor.ID}
						
						/if (${badItem.Equal[${Cursor}]}) {
							/echo Destroying [${Cursor}].
							/destroy
						} else {
							/if (${retryTimer}) {
								/goto :retry_BagPickup
							} else {
								/echo ERROR: I failed to destroy [${badItem}], skipping.
							}
						}
					}
				/next y
			}
		
		/next e
		
	/next i

/if (${Debug}) /echo <== destroyItem -|
/RETURN