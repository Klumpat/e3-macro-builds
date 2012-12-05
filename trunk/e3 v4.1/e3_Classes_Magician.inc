|----------------------------------------------------------------------------|
|- e3_Classes_Magician.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains magician specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Magician's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the magician class dynamic.	-|
|--------------------------------------------------------------------|
Sub MAG_Main
/if (${Debug}) /echo |- MAG_Main ==>

|--------------------------------------------------------------------------------------------------------------------------------#1 check_CommandDebuffs.
	/if (${Debuff_Targets.Size}) /call Debuffs_OnCommand
		/if (${ActionTaken}) /return

|------------------------------------------------------------------------------------------------------------------------------------------#2 check_Rain.	
	/if (${do_Rain} && ${Rain_Spells.Size}) /call Rain
		/if (${ActionTaken}) /return

|------------------------------------------------------------------------------------------------------------------------------------#3 Assist Functions.
	/if (${Assisting}) /call MAG_Assist
		/if (${ActionTaken}) /return
		
|--------------------------------------------------------------------------------------------------------------------------------------------#4 Pet heal.
	/if (${Defined[Pet_Heal]} && ${Me.Pet.ID}) /call PetHeal
	
/if (${Debug}) /echo <== MAG_Main -|	
/return



|------------------------------------------------------------------------|
|- Magician's Class specific Assist Loop.								-|
|------------------------------------------------------------------------|
|- Prioritizes Assist functions based on the magician class dynamic.	-|
|------------------------------------------------------------------------|
Sub MAG_Assist
/if (${Debug}) /echo |- MAG_Assist ==>

|-----------------------------------------------------------------------#1 Check Asssit status.
	/call check_AssistStatus

|--------------------------------------------------------------------------------#2 QuickBurns.
	/if (${use_QBurn}) /call QuickBurns
	
|---------------------------------------------------------------------------------#3 FullBurns.	
	/if (${use_FBurn}) /call FullBurns

|-------------------------------------------------------------------------#4 Debuffs on assist.
	/if (${Assist_Debuffs.Size}) /call Debuffs_OnAssist
		/if (${ActionTaken}) /return
			
|-------------------------------------------------------------------------------------#5 Nukes.		
	/if (${MyNukes.Size} && !${NukeTimer}) /call Nukes

/if (${Debug}) /echo <== MAG_Assist -|	
/return



|------------------------------------------------------------------------|
|- MAG_Setup															-|
|------------------------------------------------------------------------|
|- Imports variables from the [Magician] section of the Character_Ini.	-|
|------------------------------------------------------------------------|
Sub MAG_Setup
/if (${Debug}) /echo |- MAG_Setup ==>

	/if (!${Ini[${Character_Ini},Magician].Length}) /call MakeMAG_Ini
	/call INItoArray "${Character_Ini},Magician,Summoned Pet Items#" petItems
	
/if (${Debug}) /echo <== MAG_etup -|	
/return



|--------------------------------------------------------------------|
|- MakeMAG_Ini														-|
|--------------------------------------------------------------------|
|- Writes entries to the [Magician] section of the Character_Ini.	-|
|--------------------------------------------------------------------|
Sub MakeMAG_Ini
/if (${Debug}) /echo |- MAG_Ini ==>

	/call WriteToIni "${Character_Ini},Magician,Summoned Pet Items#1" "*List any spells / items you want to use to use when issued the 'Distribute pet items' command." 0
	
/if (${Debug}) /echo <== MAG_Ini -|	
/return



|------------------------------------------------------------------------|
|- MAG_Background_Events												-|
|------------------------------------------------------------------------|
|- Events to add to the Background_Events, that are magician specific.	-|
|------------------------------------------------------------------------|
Sub MAG_Background_Events

/return

#event petWeapons "#1# tells you, 'Pet Weapons'"
#event petWeapons "<#1#> Pet Weapons"
sub event_petWeapons(string line, string ChatSender)
| /varset Debug TRUE
/if (${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}
/if (${Debug}) /echo |- EVENT_petWeapons ==>

	/if (${petItems.Size}) {
		
		/if (!${SpawnCount[pc ${ChatSender}]}) {
			/if (${Debug}) /echo |- EVENT_petWeapons -| [${ChatSender}] is not in ${Zone}.
		} else {
			
			/if (${Spawn[pc ${ChatSender}].Distance} > 100) {
				/echo [${ChatSender}] is too far away.
			} else {
			
				/if (!${Spawn[pc ${ChatSender}].LineOfSight}) {
					/echo I cannot see [${ChatSender}].
				} else {
					
					/call MoveToSpawn ${Spawn[pet ${Spawn[pc ${ChatSender}].Pet}].ID}
						
						/declare i int local
						/declare x int local
						/declare e int local
						/declare retryTimer timer local
						/declare miscTimer timer local
						/declare summonedItem string local
						
						/for i 1 to ${petItems.Size}
						
						/if (${Target.Distance} < 20) {
							/call e3_Cast 0 "${petItems[${i}].Arg[1,/]}" ${petItems[${i}].Arg[2,/]} ${If[${petItems[${i}].Arg[2,/].Left[3].Equal[gem]},10s,]}
							/delay 20 ${Cursor.ID}
							
							/if (${Cursor.ID}) {
								
								/varset summonedItem ${Cursor.Name}
								/varset retryTimer 50
								
								:OpenTrade_Loop
								
								/click left target
								/delay 10 ${Window[GiveWnd].Open}
								
								/if (!${Window[GiveWnd].Open}) {
									/if (${retryTimer}) {
										/goto :OpenTrade_Loop
									} else {
										/echo Failed to open trade with ${Target.CleanName}.
									}
								} else {
									
									/varset retryTimer 30
									:WaitAccept_Loop
									
									/notify GiveWnd GVW_Give_Button LeftMouseUp
									/delay 5 !${Window[GiveWnd].Open}
									
									/if (${Window[GiveWnd].Open}) {
										/if (${retryTimer}) {
											/goto :WaitAccept_Loop
										} else {
											/echo Failed to open trade with ${Target.CleanName}.
										}
									}
								}
							}
							
							/if (${FindItemCount[=${summonedItem}]}) {
								
								/if (${Debug}) /echo |- EVENT_petWeapons -| Dropped [${summonedItem}] in my inventory.
							|--------------------------------------------------------------Open InventoryWindow.
								/varset miscTimer 3s
								
								:openInventory
								/if (!${Window[InventoryWindow].Open}) /nomodkey /keypress i
								/delay 5 ${Window[InventoryWindow].Open}
								/if (!${Window[InventoryWindow].Open} && ${miscTimer}) /goto :openInventory
					
							|------------------------------------------------------------------Search packslots.
								/varset e 0
								/for x 1 to 8  
								
								|------------------------If the item in the packslot is a soulstone, pick it up.
									/if (!${Me.Inventory[pack${x}].Container} && ${summonedItem.Equal[${Me.Inventory[pack${x}]}]}) {
										/if (${Debug}) /echo |- EVENT_petWeapons -| Picking up [${summonedItem}] form pack${x}
										/itemnotify pack${x} leftmouseup
									} else {
									
								|-------------------------If the item in the packslot is a container, search it.
										/for e 1 to ${Me.Inventory[pack${x}].Container}
											/if (${summonedItem.Equal[${Me.Inventory[pack${x}].Item[${e}]}]}) {
												/if (!${Window[pack${x}].Open}) /itemnotify pack${x} rightmouseup
												/delay 1s ${Window[pack${x}].Open}
												/if (${Debug}) /echo |- EVENT_petWeapons -| Picking up [${summonedItem}] form pack${x} slot${e}
												/itemnotify in pack${x} ${e} leftmouseup
											}
										/next e
									}
								/next x
								
								/if (${retryTimer}) {
									/goto :OpenTrade_Loop
								} else {
									/if (${Cursor.Name.Equal[${summonedItem}]}) /destroy 
								}
							}
						}
					/next i
				}
			}
		}
	}

/if (${Debug}) /echo <== EVENT_petWeapons -|
| /varset Debug FALSE
/return



sub event_distribute_Weapons
/if (${Me.Class.ShortName.Equal[MAG]} && ${petItems.Size}) {
	/declare centerLoc string local ${Me.Loc.Replace[ ,]}
	/declare i int local
	/declare x int local
	/declare y int local
	/for i 1 to ${SpawnCount[radius 100]}
		/if (${NearestSpawn[${i}].Type.Equal[PC]} && ${Spawn[pc ${NearestSpawn[${i}]}].Pet.ID} && ${Spawn[id ${Spawn[pc ${NearestSpawn[${i}]}].Pet.ID}].LineOfSight}) {
			/call Triangulate_Distance ${centerLoc} ${Spawn[pc ${NearestSpawn[${i}]}].Pet.Loc.Replace[ ,]}
			/if (${Macro.Return}<100) {
				/call BuildArray PetsNearMe ${Spawn[pc ${NearestSpawn[${i}]}].Pet}--${Spawn[pc ${NearestSpawn[${i}]}].Pet.ID} stack
			}
		}
	/next i
	/for x 1 to ${PetsNearMe.Size}
		/call TrueTarget ${PetsNearMe[${x}].Arg[2,--]}
		/if (${Target.ID} == ${PetsNearMe[${x}].Arg[2,--]}) {
			/moveto ID ${PetsNearMe[${x}].Arg[2,--]} dist 7
			/delay 3s ${Me.Moving}
			/delay 10s !${Me.Moving} || ${Spawn[id ${PetsNearMe[${x}].Arg[2,--]}].Distance}<3
			/if (${Spawn[id ${PetsNearMe[${x}].Arg[2,--]}].Distance}<10) {
				/face fast ${Spawn[id ${PetsNearMe[${x}].Arg[2,--]}].CleanName}
				/echo I have arrived at ${Spawn[id ${PetsNearMe[${x}].Arg[2,--]}].CleanName}
				/for y 1 to ${petItems.Size}
					/call e3_Cast 0 "${petItems[${y}].Arg[1,/]}" ${If[!${petItems[${y}].Arg[2,/].Length},gem${DefaultGem} ${Math.Calc[${Spell[${petItems[${y}].Arg[1,/]}].RecastTime}+5]}s,${petItems[${y}].Arg[2,/]}]}
					:cursor_Loop
					/if (${Cursor.ID}) {
						/click left target
						/delay 1s ${Window[GiveWnd].Open}
						/goto :cursor_Loop
					}
					/delay 5s ${Window[GiveWnd].Open}
					/if (${Window[GiveWnd].Open}) {
						/notify GiveWnd GVW_Give_Button LeftMouseUp
						/delay 2s !${Window[GiveWnd].Open}
					}
				/next y
				
			}
		}
	/next x
	/squelch /target clear
	/moveto loc ${centerLoc.Arg[1,,]} ${centerLoc.Arg[2,,]}
}
/if (${Defined[PetsNearMe]}) /deletevar PetsNearMe
/return

#event CoH_Group "#*# tells the group, 'CoH the group"
#event CoH_Group "#*# tells the group, 'CoH group"

sub event_CoH_Group
/if (${Me.Class.ShortName.Equal[MAG]}) {

}
/return