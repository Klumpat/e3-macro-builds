|------------------------------------------------------------------------|
|- e3_Classes_Monk.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains monk specific functions related to,							-|
|-		Setup and Ini entry information									-|
|-		MainLoop, and AssistLoop hooks									-|
|-		Over-all functionality / flow									-|
|------------------------------------------------------------------------|



|----------------------------------------------------------------|
|- Monk's Class specific mainLoop hook.							-|
|----------------------------------------------------------------|
|- Prioritizes macro functions based on the monk class dynamic.	-|
|----------------------------------------------------------------|
Sub MNK_Main
/if (${Debug}) /echo |- MNK_Main ==>

|-----------------------------------------------------------------------------------------------------------------------------------#1 check_CombatBuffs.
	/if (!${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${ActionTaken}) {
	
	|------------------------------------------------------------------------------------------------------------------------------------#2 Assist Functions.
		/if (${Assisting}) /call MNK_Assist
		/if (!${ActionTaken}) {
		
		|-----------------------------------------------------------------------------------------------------------------------------------------#3 Auto Forage.
			/if (${autoForage}) /call AutoForage
		}
	}

/if (${Debug}) /echo <== MNK_Main -|	
/return



|--------------------------------------------------------------------|
|- Monk's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the monk class dynamic.	-|
|--------------------------------------------------------------------|
Sub MNK_Assist
/if (${Debug}) /echo |- MNK_Assist ==>

|-----------------------------------------------------------------------#1 Check Asssit status.
	/call check_AssistStatus

|--------------------------------------------------------------------------------#2 QuickBurns.
	/if (${use_QBurn}) /call QuickBurns
	
|---------------------------------------------------------------------------------#3 FullBurns.	
	/if (${use_FBurn}) /call FullBurns
	
|--------------------------------------------------------------------------#4 Combat Abilities.	
	/if (${MyAbilities.Size}) /call CombatAbilities

/if (${Debug}) /echo <== MNK_Assist -|	
/return



|--------------------------------------------------------------------|
|- MNK_Setup														-|
|--------------------------------------------------------------------|
|- Imports variables from the [Monk] section of the Character_Ini.	-|
|--------------------------------------------------------------------|
Sub MNK_Setup
/if (${Debug}) /echo |- MNK_Setup ==>

	| /if (!${Ini[${Character_Ini},${Me.Class}].Length}) /call Make${Me.Class.ShortName}_Ini
	
/if (${Debug}) /echo <== MNK_Setup -|
/return



|----------------------------------------------------------------|
|- MakeMNK_Ini													-|
|----------------------------------------------------------------|
|- Writes entries to the [Monk] section of the Character_Ini.	-|
|----------------------------------------------------------------|
Sub MakeMNK_Ini
/if (${Debug}) /echo |- MNK_Ini ==>

	/call WriteToIni "${Character_Ini},${Me.Class},Auto-Forage (On/Off)" "*Do you want this character to automatically forage?"
	
/if (${Debug}) /echo <== MNK_Ini -|	
/return



|--------------------------------------------------------------------|
|- MNK_Background_Events											-|
|--------------------------------------------------------------------|
|- Events to add to the Background_Events, that are monk specific.	-|
|--------------------------------------------------------------------|
Sub MNK_Background_Events

/return