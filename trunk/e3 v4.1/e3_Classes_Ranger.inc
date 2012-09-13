|----------------------------------------------------------------------------|
|- e3_Classes_Ranger.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains ranger specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Ranger's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the ranger class dynamic.	-|
|--------------------------------------------------------------------|
Sub RNG_Main
/if (${Debug}) /echo |- RNG_Main ==>

|------------------------------------------------------------------------------------------------------------------------------------#1 Assist Functions.
	/if (${Assisting}) /call RNG_Assist
		/if (${ActionTaken}) /return
		
|-----------------------------------------------------------------------------------------------------------------------------------------#2 check_Heals.
	/if (${Heals}) /call CheckHeals
		/if (${ActionTaken}) /return

|-----------------------------------------------------------------------------------------------------------------------------------#3 check_CombatBuffs.
	/if (!${idle} && ${PriorityBuffs.Size}) /call checkPriorityBuffs
		/if (${ActionTaken}) /return
		
|------------------------------------------------------------------------------------------------------------------------------------------#4 check_DoTs.	
	/if (${DoT_Targets.Size}) /call DoTs_OnCommand
		/if (${ActionTaken}) /return
		
|-----------------------------------------------------------------------------------------------------------------------------------------#5 Auto Forage.
	/if (${autoForage}) /call AutoForage
	
/if (${Debug}) /echo <== RNG_Main -|
/return



|--------------------------------------------------------------------|
|- Ranger's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the ranger class dynamic.	-|
|--------------------------------------------------------------------|
Sub RNG_Assist
/if (${Debug}) /echo |- RNG_Assist ==>

|-----------------------------------------------------------------------#1 Check Asssit status.
	/call check_AssistStatus

|--------------------------------------------------------------------------------#2 QuickBurns.
	/if (${use_QBurn}) /call QuickBurns
	
|---------------------------------------------------------------------------------#3 FullBurns.	
	/if (${use_FBurn}) /call FullBurns

|--------------------------------------------------------------------------#4 Combat Abilities.	
	/if (${MyAbilities.Size}) /call CombatAbilities
	
|-------------------------------------------------------------------------------------#5 Nukes.		
	/if (${MyNukes.Size} && !${NukeTimer}) /call Nukes
		/if (${ActionTaken}) /return
		
|----------------------------------------------------------------------------#6 Dots on assist.
	/if (${Assist_DoTs.Size}) /call DoTs_OnAssist

/if (${Debug}) /echo <== RNG_Assist -|	
/return



|------------------------------------------------------------------------|
|- RNG_Setup															-|
|------------------------------------------------------------------------|
|- Imports variables from the [Ranger] section of the Character_Ini.	-|
|------------------------------------------------------------------------|
Sub RNG_Setup
/if (${Debug}) /echo |- RNG_Setup ==>

	/if (!${Ini[${Character_Ini},${Me.Class}].Length}) /call Make${Me.Class.ShortName}_Ini

/if (${Debug}) /echo <== RNG_Setup -|
/return



|----------------------------------------------------------------|
|- MakeRNG_Ini													-|
|----------------------------------------------------------------|
|- Writes entries to the [Ranger] section of the Character_Ini.	-|
|----------------------------------------------------------------|
Sub MakeRNG_Ini
/if (${Debug}) /echo |- RNG_Ini ==>

	/call WriteToIni "${Character_Ini},${Me.Class},Auto_Forage(On/Off)" "*Do you want this character to automatically forage?" 0

	
/if (${Debug}) /echo <== RNG_Ini -|
/return



|------------------------------------------------------------------------|
|- RNG_Background_Events												-|
|------------------------------------------------------------------------|
|- Events to add to the Background_Events, that are shaman specific.	-|
|------------------------------------------------------------------------|
Sub RNG_Background_Events

/return