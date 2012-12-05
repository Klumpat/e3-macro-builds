|--------------------------------------------------------------------------------|
|- e3_Classes_Berserker.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ													-|
|--------------------------------------------------------------------------------|
|- Contains berserker specific functions related to,							-|
|-		Setup and Ini entry information											-|
|-		MainLoop, and AssistLoop hooks											-|
|-		Over-all functionality / flow											-|
|--------------------------------------------------------------------------------|



|------------------------------------------------------------------------|
|- Berserker's Class specific mainLoop hook.							-|
|------------------------------------------------------------------------|
|- Prioritizes macro functions based on the berserker class dynamic.	-|
|------------------------------------------------------------------------|
Sub BER_Main
/if (${Debug}) /echo |- BER_Main ==>

|-----------------------------------------------------------------------------------------------------------------------------------#1 check_CombatBuffs.
	/if (!${idle} && ${PriorityBuffs.Size}) /call checkPriorityBuffs
		/if (${ActionTaken}) /return
		
|------------------------------------------------------------------------------------------------------------------------------------#2 Assist Functions.
	/if (${Assisting}) /call BER_Assist
	
/if (${Debug}) /echo <== BER_Main -|
/return



|------------------------------------------------------------------------|
|- Berserker's Class specific Assist Loop.								-|
|------------------------------------------------------------------------|
|- Prioritizes Assist functions based on the berserker class dynamic.	-|
|------------------------------------------------------------------------|
Sub BER_Assist
/if (${Debug}) /echo |- BER_Assist ==>

|-----------------------------------------------------------------------#1 Check Asssit status.
	/call check_AssistStatus

|--------------------------------------------------------------------------------#2 QuickBurns.
	/if (${use_QBurn}) /call QuickBurns
	
|---------------------------------------------------------------------------------#3 FullBurns.	
	/if (${use_FBurn}) /call FullBurns
	
|--------------------------------------------------------------------------#4 Combat Abilities.
	/if (${MyAbilities.Size}) /call CombatAbilities

/if (${Debug}) /echo <== BER_Assist -|
/return



|------------------------------------------------------------------------|
|- BER_Setup															-|
|------------------------------------------------------------------------|
|- Imports variables from the [Berserker] section of the Character_Ini.	-|
|------------------------------------------------------------------------|
Sub BER_Setup
/if (${Debug}) /echo |- BER_Setup ==>

	| /if (!${Ini[${Character_Ini},${Me.Class}].Length}) /call Make${Me.Class.ShortName}_Ini
	
/if (${Debug}) /echo <== BER_Setup -|
/return



|--------------------------------------------------------------------|
|- MakeBER_Ini														-|
|--------------------------------------------------------------------|
|- Writes entries to the [Berserker] section of the Character_Ini.	-|
|--------------------------------------------------------------------|
Sub MakeBER_Ini
/if (${Debug}) /echo |- BER_Ini ==>

/if (${Debug}) /echo <== BER_Ini -|
/return



|------------------------------------------------------------------------|
|- BER_Background_Events												-|
|------------------------------------------------------------------------|
|- Events to add to the Background_Events, that are berserker specific.	-|
|------------------------------------------------------------------------|
Sub BER_Background_Events

/return