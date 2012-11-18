|------------------------------------------------------------------------------------|
|- e3_Macro_CorpseSummoning.inc v1.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ														-|
|------------------------------------------------------------------------------------|
|- Contains functions to automatically use the corpse summoners in the guild lobby.	-|
|------------------------------------------------------------------------------------|



#event UseCoprseSummoner "<#*#> Use corpse summoner"
#event UseCoprseSummoner "[MQ2] Use corpse summoner"
SUB event_UseCoprseSummoner()
| /varset Debug TRUE
/if (${Debug}) /echo |- event_UseCoprseSummoner ==>

|---------------------------------------------------------------------------------------------------------------------------------------------------------Make sure I am in The Guild Lobby.
	/if (${Zone.ID}!=344) {
		/echo ERROR: I have to be in The Guild Lobby, to use the corpse summoner.  Move there, and try again.
	} else {

		/call IniToVar ${Macro_Ini},CorpseSummoning,SoulStone SoulStoneToUse string outer
		/call IniToVar ${Macro_Ini},CorpseSummoning,SoulStoneCost SoulStoneCost string outer
		/declare GiveUpTimer	int outer 600
	
	|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------Check Money.
		/if (${Me.Platinum} < ${SoulStoneCost} && !${FindItemCount[=${SoulStoneToUse}]}) /call withdraw_Plat
		
	|-------------------------------------------------------------------------------------------------------------------------------------------------------------------Check for soulstone.
		/if (${Me.Platinum} >= ${SoulStoneCost} && !${FindItemCount[=${SoulStoneToUse}]}) /call buy_SoulStone "${SoulStoneToUse}"
		
	|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------Summon Corpse.
		/if (${FindItemCount[=${SoulStoneToUse}]}) /call summon_Corpse "${SoulStoneToUse}" 
		
		/if (${Defined[SoulStoneToUse]}) /deletevar SoulStoneToUse
		/if (${Defined[SoulStoneCost]}) /deletevar SoulStoneCost
		/if (${Defined[GiveUpTimer]}) /deletevar GiveUpTimer
	}

/if (${Debug}) /echo <== event_UseCoprseSummoner -|
| /varset Debug FALSE
/RETURN



SUB withdraw_Plat
/if (${Debug}) /echo |- withdraw_Plat ==>

|------------------------------------------------------------------------------------Move to the middle of the west side of The Guild Lobby, via e3_Macro_SmoothMoves.inc > event_run_Route.
	/call event_run_Route X "DestThrone/West" ${Me.CleanName}
	
|-------------------------------------------------------------------------------------------------------------------------------------------------------------Look for the west side banker.
	/if (!${Spawn[npc A_Shrewd_Banker001].LineOfSight}) {
		/echo ERROR: I couldn't find the west side banker.  Move to the Throne area, or the west side of The Guild Lobby, and try again.
	} else {
	
	|--------------------------------------------------------------------------------------------------------------------------------------------------------------------Move to the banker.
		/call MoveToSpawn ${Spawn[npc A_Shrewd_Banker001].ID}
		
		/if (${Target.Distance} > 15) {
			/echo ERROR: I failed to move to the banker. Move me to the west side of The Guild Lobby, and try again.
		} else {
			
		|-------------------------------------------------------------------------------------------------------------------------------------Create a retryTimer and set it for 30 seconds.
			/declare retryTimer timer local ${GiveUpTimer}
			/declare miscTimer timer local
			/declare i int local
			
			:withdraw_Loop
			
		|-----------------------------------------------------------------------------------------------------------------------------------------------------------------Target the banker.
			/if (${Target.ID} != ${Spawn[npc A_Shrewd_Banker001].ID}) /call TrueTarget ${Spawn[npc A_Shrewd_Banker001].ID}
			
		|--------------------------------------------------------------------------------------------------------------------------------------------------------------Open the bank window.
			/click right target
			/delay 10 ${Window[BigBankWnd].Open}
			
		|-------------------------------------------------------------------------------------------------------------If the bank window has not opened.
			/if (!${Window[BigBankWnd].Open}) {
				
			|-------------------------------------------------------------------------------------------------------------If there is still time, retry.
				/if (${retryTimer}) {
					/goto :withdraw_Loop
				
			|--------------------------------------------------------------------------------------------------------------------Else, announce to user.
				} else {
					/echo ERROR: Failed to open the bank window.
				}
				
		|--------------------------------------------------------------------------------------------------------------------------------------------------Else, if the bank window is open.
			} else {
			
			|-------------------------------------------------------------------------------------Open the platinum QuantityWnd.
				/notify BigBankWnd BIGB_Money0 leftmouseup
				/delay 5 ${Window[QuantityWnd].Open}
				
			|----------------------------------------------------------------------------------If the platinum QuantityWnd did not open.
				/if (!${Window[QuantityWnd].Open}) {
					
				|--------------------------------------------------------If there is still time, retry.
					/if (${retryTimer}) {
						/goto :withdraw_Loop
					
				|--------------------------------------------------------Else, announce to user.
					} else {
						/echo ERROR: Failed to open the platinum withdraw box.
					}
					
			|-------------------------------------------------------------------------------------Else, if the platinum QuantityWnd did open.
				} else {
				
				|----------------------------------------Clear the text in the platinum QuantityWnd.
					/varset miscTimer 30
					
					:clearText
					/nomodkey /keypress backspace chat hold
					/if (${miscTimer}) /goto :clearText
				
				|------------------------------------------------------------------------------------Withdraw money.
					/varset miscTimer 3s
					
					/for i 1 to ${SoulStoneCost.Length}
					
						/if (${Debug}) /echo |- withdraw_Plat -| Entering [${SoulStoneCost.Mid[${i},1]}]
						/keypress ${SoulStoneCost.Mid[${i},1]} chat
						/delay 3
						
					/next i
					
					/notify QuantityWnd QTYW_Accept_Button leftmouseup
					/delay 2
					/autoinventory
					
				|--------------------------------------------------------------If I didn't get enough money, try again. Otherwise, close the bank.
					/if (${Me.Platinum} < 170) {
						/if (${retryTimer}) {
							/goto :withdraw_Loop
						} else {
							/echo ERROR: Failed to withdraw platinum from the bank.
						}
					} else  {
						/notify BigBankWnd BIGB_DoneButton leftmouseup
					}		
				}
			}
		}
	}

/if (${Debug}) /echo <== withdraw_Plat -|
/RETURN



SUB buy_SoulStone(SoulStoneToUse)
/if (${Debug}) /echo |- buy_SoulStone ==>

|------------------------------------------------------------------------------------Move to the middle of the west side of The Guild Lobby, via e3_Macro_SmoothMoves.inc > event_run_Route.
	/call event_run_Route X "DestThrone/West" ${Me.CleanName}
	
|-------------------------------------------------------------------------------------------------------------------------------------------------------------Look for the west side banker.
	/if (!${Spawn[npc A_Disciple_of_Luclin000].LineOfSight}) {
		/echo ERROR: I couldn't find the soulstone merchant.  Move to the Throne area, or the west side of The Guild Lobby, and try again.
	} else {
	
	|--------------------------------------------------------------------------------------------------------------------------------------------------------------------Move to the banker.
		/call MoveToSpawn ${Spawn[npc A_Disciple_of_Luclin000].ID}
		
		/if (${Target.Distance} > 15) {
			/echo ERROR: I failed to move to the soulstone merchant. Move me to the west side of The Guild Lobby, and try again.
		} else {
		
		
		|-------------------------------------------------------------------------------------------------------------------------------------Create a retryTimer and set it for 30 seconds.
			/declare retryTimer timer local ${GiveUpTimer}
			/declare miscTimer timer local
			
			/declare i int local
			/declare merchantTotal int local
		
			:buyLoop
			
		|-------------------------------------------------------------------------------------------------------------------------------------------------------Open the soulstone merchant.
			/click right target
			/delay 10 ${Window[MerchantWnd].Open}
			
		|-------------------------------------------------------------------------------------------------------------If the soulstone merchant has not opened.
			/if (!${Window[MerchantWnd].Open}) {
				
			|-------------------------------------------------------------------------------------------------------------If there is still time, retry.
				/if (${retryTimer}) {
					/goto :buyLoop
				
			|--------------------------------------------------------------------------------------------------------------------Else, announce to user.
				} else {
					/echo ERROR: Failed to open the soulstone merchant.
				}
				
		|----------------------------------------------------------------------------------------------------------------------------------------------Else, if the merchant window is open.
			} else {
			
			|---------------------------------------------------------Wait for the merchant to populate.
				/varset merchantTotal -1
				:populateMerchant
				/if (${merchantTotal}!=${Window[MerchantWnd].Child[ItemList].Items}) {
					/varset merchantTotal ${Window[MerchantWnd].Child[ItemList].Items}
					/delay 2
					/goto :populateMerchant
				}
				
			|-------------------------------------------------------------------------------------------------------------------------------------------------------------Buy the soulstone.	
				/nomodkey /notify MerchantWnd ItemList listselect ${Window[MerchantWnd].Child[ItemList].List[=${SoulStoneToUse},2]}
				/delay 2
				/if (!${FindItemCount[${SoulStoneToUse}]}) /notify MerchantWnd MW_Buy_Button leftmouseup
				/delay 2
				
			|-------------------------------------------------------------------------------If I failed to buy the soulstone.
				/if (!${FindItemCount[${SoulStoneToUse}]}) {
					/if (${retryTimer}) {
						/goto :buyLoop
					} else {
						/echo ERROR: Failed to buy a soulstone!
					}
				} else {
					/nomodkey /notify MerchantWnd MW_Done_Button leftmouseup
				}
			}
		}
	}

/if (${Debug}) /echo <== buy_SoulStone -|
/RETURN



SUB	summon_Corpse(SoulStoneToUse)
/if (${Debug}) /echo |- summon_Corpse ==>

|------------------------------------------------------------------------------------Move to the middle of the west side of The Guild Lobby, via e3_Macro_SmoothMoves.inc > event_run_Route.
	/call event_run_Route X "DestThrone/West" ${Me.CleanName}
	
|--------------------------------------------------------------------------------------------------------------------------------------------------------------Look for the corpse summoner.
	/if (!${Spawn[npc A_Priest_of_Luclin000].LineOfSight}) {
		/echo ERROR: I couldn't find the corpse summoner.  Move to the Throne area, or the west side of The Guild Lobby, and try again.
	} else {
	
	|-----------------------------------------------------------------------------------------------------------------------------------------------------------Move to the corpse summoner.
		/call MoveToSpawn ${Spawn[npc A_Priest_of_Luclin000].ID}
		
		/if (${Target.Distance} > 15) {
			/echo ERROR: I failed to move to the corpse summoner. Move me to the west side of The Guild Lobby, and try again.
		} else {
		
		|-------------------------------------------------------------------------------------------------------------------------------------Create a retryTimer and set it for 30 seconds.
			/declare retryTimer timer local ${GiveUpTimer}
			/declare miscTimer timer local
		
		|--------------------------------------------------------------------------------------------------------------------------------------------------------Declare counting variables.
			/declare i int local
			/declare e int local
			
			:summonCorpse
			
			/if (${Spawn[${Me}'s].ID}) /goto :check_ForCorpse
			
		|--------------------------------------------------------------Open InventoryWindow.
	
			/if (!${Window[InventoryWindow].Open}) /nomodkey /keypress i
			/delay 10 ${Window[InventoryWindow].Open}
			
		|-------------------------------------------------------------------------------------------------------------If my inventory is not open.
			/if (!${Window[InventoryWindow].Open}) {
				
			|-------------------------------------------------------------------------------------------------------------If there is still time, retry.
				/if (${retryTimer}) {
					/goto :summonCorpse
				
			|--------------------------------------------------------------------------------------------------------------------Else, announce to user.
				} else {
					/echo ERROR: Failed to open my inventory.
				}
				
		|----------------------------------------------------------------------------------------------------------------------------------------------Else, if my inventory is open.
			} else {
			
			|------------------------------------------------------------------Search packslots.
				/for i 1 to 8  
				
				|------------------------If the item in the packslot is a soulstone, pick it up.
					/if (!${Me.Inventory[pack${i}].Container} && ${SoulStoneToUse.Equal[${Me.Inventory[pack${i}]}]}) {
						/itemnotify pack${i} leftmouseup
					} else {
					
				|-------------------------If the item in the packslot is a container, search it.
						/for e 1 to ${Me.Inventory[pack${i}].Container}
							/if (${SoulStoneToUse.Equal[${Me.Inventory[pack${i}].Item[${e}]}]}) {
								/if (!${Window[pack${i}].Open}) /itemnotify pack${i} rightmouseup
								/delay 1s ${Window[pack${i}].Open}
								/itemnotify in pack${i} ${e} leftmouseup
							}
						/next e
					}
				/next i
				
			|----------------------------------------Check that the soulstone was picked up.	
				/if (!${Cursor.ID}) {
					
				|-------------------------------------------------------------------------------------------------------------If there is still time, retry.
					/if (${retryTimer}) {
						/goto :summonCorpse
					
				|--------------------------------------------------------------------------------------------------------------------Else, announce to user.
					} else {
						/echo ERROR: Failed to pick up my soulstone.
					}
					
			|------------------------------------------------------------------------------------------------------------------------------------------Else, if the soulstone was picked up.
				} else {
				
				|-------------------------------------------Open trade with the corpse summoner.
					/click left target
					/delay 10 ${Window[GiveWnd].Open}
					
				|-------------------------------------------------------------------------------------------------------------If the GiveWnd did not open.
					/if (!${Window[GiveWnd].Open}) {
					
					|-------------------------------------------------------------------------------------------------------------If there is still time, retry.
						/if (${retryTimer}) {
							/goto :summonCorpse
						
					|--------------------------------------------------------------------------------------------------------------------Else, announce to user.
						} else {
							/echo ERROR: Failed to open the give window.
						}
						
				|----------------------------------------------------------------------------------------------------------------Else, if the give window is open.
					} else {
					
					|-----------------------------------------------------------------------Give the soulstone to the corpse summoner.
						/delay 5
						/notify GiveWnd GVW_Give_Button leftmouseup
						/delay 10 !${Window[GiveWnd].Open}
						
					|-------------------------------------------------------------------------------------------------------------If the GiveWnd did not open
						/if (${Window[GiveWnd].Open} || ${FindItemCount[=${SoulStoneToUse}]}) {
						
						|-------------------------------------------------------------------------------------------------------------If there is still time, retry.
							/if (${retryTimer}) {
								/goto :summonCorpse
							
						|--------------------------------------------------------------------------------------------------------------------Else, announce to user.
							} else {
								/echo ERROR: Failed to complete the trade.
							}
							
					|----------------------------------------------------------------------------------------------------------------Else, if the give window is open.
						} else {
						
							:check_ForCorpse
						|---------------------------------------------------------------------------------------Loot your summoned corpse.
							/delay 3s ${Spawn[${Me}'s].ID}
							
							/if (!${Spawn[${Me}'s].ID}) {
								/echo ERROR: My corpse did not appear.
							} else {
								/call event_LootNow
								
							|-------------------------------------Move to the middle of the west side of The Guild Lobby, via e3_Macro_SmoothMoves.inc > event_run_Route.
								/call event_run_Route X "DestThrone/West" ${Me.CleanName}
							}
						}
					}
				}
			}
		}
	}

/if (${Debug}) /echo <== summon_Corpse -|
/RETURN