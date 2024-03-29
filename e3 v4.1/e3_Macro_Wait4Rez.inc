|----------------------------------------------------------------------------|
|- e3_Macro_Wait4Rez.inc v1.4 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Based off Wait4Rez.inc by A_Druid_00 (originally written by fantum409)	-|
|----------------------------------------------------------------------------|
|- Accepts rezzes and loots corpses after being killed.						-|
|----------------------------------------------------------------------------|
|- Use,	[Wait4Rez] in /t or /echo to manually call Wait4Rez.				-|
|- Use, [Loot Now] in /t or /echo to manually skip rez acceptance,			-|
|- and begin looting.														-|
|----------------------------------------------------------------------------|



|----------------------------------------------------------------|
|- Declares variables and settings used in Wait4Rez functions.	-|
|----------------------------------------------------------------|
SUB setup_Wait4Rez
/if (${Debug}) /echo |- Wait4Rez_Setup ==>
	
	/call INItoVar "${Macro_Ini},Wait4Rez,Wait4Rez (On/Off)" use_Wait4Rez bool outer
	/call INItoVar "${Macro_Ini},Wait4Rez,Wait4Rez Alert (Beeps)" Wait4RezAlert int outer
	/call INItoVar "${Macro_Ini},Wait4Rez,Loot Bag Delay" BagDelay int outer
	/call INItoVar "${Macro_Ini},Wait4Rez,Loot Item Delay" ItemDelay int outer	
	
/if (${Debug}) /echo <== Wait4Rez_Setup -|
/RETURN



|------------------------------------------------|
|- Automatically engages Wait4Rez when you die.	-|
|------------------------------------------------|
#EVENT dead	"You have been slain by #*#"
#EVENT dead	"You died."
SUB EVENT_dead
/if (${Debug}) /echo |- Event_Dead ==>
	
	/if (!${Defined[use_Wait4Rez]}) /call Wait4Rez_Setup
	/if (${use_Wait4Rez}) {
	
		|--------------------------------------------------------------------------Alert master that a bot has died.
		/if (${Wait4RezAlert}) {
			/declare a int local
			/for a 1 to ${Wait4RezAlert}
				/beep
				/delay 1
			/next a
		}
		
		|--------------------------------------------------------------------------------------------Wait for a Rez.
		/docommand ${ChatToggle} I'm dead. Waiting for rez.
		/if (!${Defined[Waiting4Rez]}) /declare Waiting4Rez bool outer TRUE
		/call Wait4Rez
	}
	
	|---------------------------------------------------------------------------Stop attacking(for use with e3.mac).
	/if (${Defined[e3_Version]}) {
		/if (${Assisting} || ${Me.Combat}) /call AssistOff
	}
/if (${Debug}) /echo <== Event_Dead -|
/RETURN



|--------------------------------------------|
|- Manually engages Wait4Rez on command.	-|
|--------------------------------------------|
#EVENT waitNow "#*# tells you, 'Wait4Rez'"
#EVENT waitNow "<#*#> All Wait4Rez"
SUB EVENT_waitNow
/if (${Debug}) /echo |- Event_WaitNow ==>
	
	/if (!${Defined[use_Wait4Rez]}) /call Wait4Rez_Setup
	/if (${use_Wait4Rez}) {
	
		|------------------------------------------------------------------------If I'm not already waiting, wait for a rez.
		/if (!${Waiting4Rez}) {
			/if (!${Defined[Waiting4Rez]}) /declare Waiting4Rez bool outer TRUE
			/call Wait4Rez
		}
	}
	
/if (${Debug}) /echo <== Event_WaitNow -|
/RETURN



|--------------------------------------------------------------------------------------------------------|
|- Manual bypasses for rez acceptance and zoning features, to loot corpses that are already close by.	-|
|--------------------------------------------------------------------------------------------------------|
#EVENT lootNow "[MQ2] Loot Corpse#*#"
#EVENT lootNow "[MQ2] Loot Now"
#EVENT lootNow "<#*#> Loot Now#*#"
#EVENT lootNow "#*# tells you, 'Loot Now#*#"
#EVENT lootNow "<#*#> Loot Corpse#*#"
#EVENT lootNow "#*# tells you, 'Loot Corpse#*#"
SUB EVENT_lootNow
/if (${Debug}) /echo |- Event_lootNow ==>
	
	/if (!${Defined[use_Wait4Rez]}) /call Wait4Rez_Setup
	/if (${use_Wait4Rez}) {
		/echo Looting....
		
		|--------------------------------------------------------------Call Wait4Rez, and use functionCall to pass LOOT_NOW.
		/call Wait4Rez LOOT_NOW
	}
	
/if (${Debug}) /echo <== Event_lootNow -|	
/RETURN



|------------------------------------------------------------------------------------|
|- Consents NetBots, loads spells, accepts rez, and loots your body after zoning.	-|
|------------------------------------------------------------------------------------|
|- @param fuctionCall: Allows you to pass a string to be checked within Wait4Rez,	-|
|- to call various functions. 														-|
|------------------------------------------------------------------------------------|
SUB Wait4Rez(functionCall)
/if (${Debug}) /echo |- Wait4Rez${If[${Defined[functionCall]}, ${functionCall},]} ==>
|--------------------------------------------------------------------------------------------------------------------------Get ready to be rezzed.

	|---------------------------------------------------------------------------------Declare counting variable 'i'.
	/declare i int local 0
	
	|---------------------------------------------------------------------------Check for a recognized functionCall.
	/if (${functionCall.Equal[LOOT_NOW]}) /goto :Looting
	/echo Awaiting rez: auto-accepting, and looting my corpse...

	|-----------------------------------------------------------------------------------------------Consent NetBots.
	/for i 1 to ${NetBots.Counts}
		/consent ${NetBots.Client[${i}]}
	/next i

	|---------------------------------------------------------------------------------------------------Load Spells.
	/if (${Me.Class.CanCast}) /call check_Gems

|----------------------------------------------------------------------------------------------------------------------------------Wait for a rez.
	:rezmeffs
	/if (${Debug}) /echo |- Wait4Rez -| Waiting to accept rez.
	
	/call Background_Events
	/delay 10 ${Window[ConfirmationDialogBox].Open}
	/if (!${Window[ConfirmationDialogBox].Open}) /goto :rezmeffs

	|------------------------------------------------------------------------------------------Click yes to rez box.
	/nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup

	|--------------------------------------------------------------Respawn Window stuff ( This is untested on PEQ ).
	/if (${Window[RespawnWnd].Open}) {
		/nomodkey /notify RespawnWnd RW_OptionsList listselect 2 
		/delay 1s
		/nomodkey /notify RespawnWnd RW_SelectButton leftmouseup
		/delay 2s 
	}

	|-------------------------------------------------------------------------------------------------Wait for zone.
	:zoneffs
	/if (${Debug}) /echo |- Wait4Rez -| Waiting to zone.
	
	/delay 300 ${Spawn[${Me}'s].ID}
	/if (!${Spawn[${Me}'s].ID}) /goto :zoneffs
	/delay 30
	
|-------------------------------------------------------------------------------------------------------------------------------------Loot corpse.
	/docommand ${ChatToggle} Looting...
	:Looting
	/if (${Debug}) /echo |- Wait4Rez -| Attempting to loot my corpse.
	
	/if (!${Defined[lootTotal]}) /declare lootTotal int local 0
	/call TrueTarget ${Spawn[${Me}'s].ID}
	/if (${Target.Distance}<100 && ${Target.CleanName.Equal[${Me}'s corpse]}) {
	
	|-----------------------------------------------------------------------------------------------Pull the corpse.
		/if (${Target.Distance}>15) /corpse
		/delay 20 ${Target.Distance}<15
		/loot
		/delay 20 ${Me.State.Equal[BIND]}
		/if (${Me.State.NotEqual[BIND]}) /goto :Looting
		
	|-------------------------------------------Wait for all the items on your corpse to appear in your loot window.
		/varset lootTotal -1
		/varset i 0
		:autoxplootlag
		/varcalc i ${i}+1
		/if (${lootTotal}!=${Corpse.Items}) {
			/varset lootTotal ${Corpse.Items}
			/delay 5
			/if (${i}<50) /goto :autoxplootlag
		}
		
	|-----------------------------------------------------------------------------------------Loot Items one by one.
		/if (${lootTotal}) {
			/for i 1 to ${lootTotal}
			/if (${Me.State.Equal[BIND]} && ${Corpse.CleanName.Equal[${Me.CleanName}'s corpse]}) {
					/delay 1s ${InvSlot[loot${i}].Item.ID}
					
				|--------------------------------------------Call rez_loot_Handle, pass the slotNumber to loot from.
					/call rez_loot_Handle ${i}	
				/next i
			}
		}
		
	|---------------------------------------------------------If I've tried to loot all the items, close the corpse.

		/notify LootWnd DoneButton leftmouseup 
		/delay 1s !${Window[LootWnd].Open}
		
		
	|---------------------------------------------------------------------------Check you don't still have a corpse.
		/if (!${Me.State.Equal[BIND]}) {
			/delay 2s !${Spawn[${Me}'s].ID}
			/if (${Spawn[${Me}'s].ID}) /goto :Looting
		}
		/docommand ${ChatToggle} Ready to die again!
	}
	/doevents flush
/RETURN



|------------------------------------------------------------------------------------------------------------|
|- Attempts to loot an item from a given corpse inventory slot, and autoequip them via 'SUB ClearCursor'	-|
|------------------------------------------------------------------------------------------------------------|
|- @param int slotNum: The slot number to loot an item from.												-|
|------------------------------------------------------------------------------------------------------------|
SUB rez_loot_Handle(int slotNum)
/if (${Debug}) /echo |- rez_loot_Handle ==>

	:retry_Loot
	/if (${Debug}) /echo |- rez_loot_Handle -| Attempting to handle an item.
	
|------------------------------------------------------------If there is no item on the cursor, try to loot an item from the given inventory slot.
	/if (!${Cursor.ID}) {
		/if (${InvSlot[loot${slotNum}].Item.Container}) /declare isContainer bool local TRUE
		/itemnotify loot${slotNum} rightmouseup
		/delay 3s !${InvSlot[loot${slotNum}].Item.ID}
		/if (${Cursor.ID}) /goto :retry_Loot
		/delay ${If[${isContainer},${BagDelay},${ItemDelay}]}

|--------------------------------------------------------------------------------------If there is an item on the cursor, try to autoinventory it.		
	} else {
		/call ClearCursor
	}
	
/if (${Debug}) /echo <== rez_loot_Handle -|
/RETURN