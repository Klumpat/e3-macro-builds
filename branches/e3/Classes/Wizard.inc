|----------------------------------------------------------------------------|
|- e3_Classes_Wizard.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains wizard specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Wizard's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the wizard class dynamic.	-|
|--------------------------------------------------------------------|
Sub WIZ_Main
/if (${Debug}) /echo |- WIZ_Main ==>

|------------------------------------------------------------------------------------------------------------------------------------------#1 check_Rain.	
	/if (${do_Rain} && ${Rain_Spells.Size}) /call Rain
		/if (${ActionTaken}) /return
	
|------------------------------------------------------------------------------------------------------------------------------------#2 Assist Functions.
	/if (${Assisting}) /call WIZ_Assist
		/if (${ActionTaken}) /return
		
|-----------------------------------------------------------------------------------------------------------------------------------#3 check_CombatBuffs.
	/if (!${idle} && ${PriorityBuffs.Size}) /call checkPriorityBuffs
		/if (${ActionTaken}) /return
		
|----------------------------------------------------------------------------------------------------------------------------------------#4 Auto Harvest.
	/if (${AutoHarvest} && ${Me.PctMana} < ${HarvestAAPct}) /call e3_Cast ${Me.ID} "Harvest of Druzzil" alt

|------------------------------------------------------------------------------------------------------------------------------------------#5 Banish Pet.
	/if (${Me.Pet.ID}) /pet get lost

/if (${Debug}) /echo <== WIZ_Main -|	
/return



|--------------------------------------------------------------------|
|- Wizard's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the wizard class dynamic.	-|
|--------------------------------------------------------------------|
Sub WIZ_Assist
/if (${Debug}) /echo |- WIZ_Assist ==>

|-----------------------------------------------------------------------#1 Check Asssit status.
	/call check_AssistStatus

|--------------------------------------------------------------------------------#2 QuickBurns.
	/if (${use_QBurn}) /call QuickBurns
	
|---------------------------------------------------------------------------------#3 FullBurns.	
	/if (${use_FBurn}) /call FullBurns
	
|-------------------------------------------------------------------------------------#4 Nukes.		
	/if (${MyNukes.Size} && !${NukeTimer}) /call Nukes

/if (${Debug}) /echo <== WIZ_Assist -|	
/return



|------------------------------------------------------------------------|
|- WIZ_Setup															-|
|------------------------------------------------------------------------|
|- Imports variables from the [Wizard] section of the Character_Ini.	-|
|------------------------------------------------------------------------|
Sub WIZ_Setup
/if (${Debug}) /echo |- WIZ_Setup ==>

	/if (!${Ini[${Character_Ini},Wizard].Length}) /call MakeWIZ_Ini
	/call INItoVar ${Character_Ini},Wizard,EvacOnCommand(On/Off) castEvac bool outer
	/call INItoVar ${Character_Ini},Wizard,Evac(Spell/Gem) Evac_Spell string outer
	/call INItoVar ${Character_Ini},Wizard,Auto_Harvest(On/Off) AutoHarvest bool outer

/if (${Debug}) /echo <== WIZ_Setup -|
/return



|----------------------------------------------------------------|
|- MakeWIZ_Ini													-|
|----------------------------------------------------------------|
|- Writes entries to the [Wizard] section of the Character_Ini.	-|
|----------------------------------------------------------------|
Sub MakeWIZ_Ini
/if (${Debug}) /echo |- WIZ_Ini ==>

	/call WriteToIni ${Character_Ini},Wizard,EvacOnCommand(On/Off) "*Do you want this character to cast Exodus, or a specified evac spell on command?" 0
	/call WriteToIni ${Character_Ini},Wizard,Evac(Spell/Gem) "*List an evac spell to use if Exodus is not ready, and the gem to mem it to." 0
	/call WriteToIni ${Character_Ini},Wizard,Auto_Harvest(On/Off) "*Do you want this character to use Harvest of Druzzil automatically?" 0

/if (${Debug}) /echo <== WIZ_Ini -|
/return



|------------------------------------------------------------------------|
|- WIZ_Background_Events												-|
|------------------------------------------------------------------------|
|- Events to add to the Background_Events, that are wizard specific.	-|
|------------------------------------------------------------------------|
Sub Wiz_Background_Events

/return