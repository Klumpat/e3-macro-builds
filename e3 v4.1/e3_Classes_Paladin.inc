|----------------------------------------------------------------------------|
|- e3_Classes_Paladin.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains paladin specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Paladin's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the paladin class dynamic.	-|
|--------------------------------------------------------------------|
Sub PAL_Main
/if (${Debug}) /echo |- PAL_Main ==>

|-----------------------------------------------------------------------------------------------------------------------------------------#1 check_Heals.
	/if (${Heals}) /call CheckHeals
		/if (${ActionTaken}) /return

|-----------------------------------------------------------------------------------------------------------------------------------#2 check_CombatBuffs.
	/if (!${idle} && ${PriorityBuffs.Size}) /call checkPriorityBuffs
		/if (${ActionTaken}) /return

|------------------------------------------------------------------------------------------------------------------------------------#3 Assist Functions.
	/if (${Assisting}) /call PAL_Assist

/if (${Debug}) /echo <== PAL_Main -|
/return



|--------------------------------------------------------------------|
|- Paladin's Class specific Assist Loop.							-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the paladin class dynamic.	-|
|--------------------------------------------------------------------|
Sub PAL_Assist
/if (${Debug}) /echo |- PAL_Assist ==>

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

/if (${Debug}) /echo <== PAL_Assist -|	
/return



|------------------------------------------------------------------------|
|- PAL_Setup															-|
|------------------------------------------------------------------------|
|- Imports variables from the [Paladin] section of the Character_Ini.	-|
|------------------------------------------------------------------------|
Sub PAL_Setup
/if (${Debug}) /echo |- PAL_Setup ==>
	
	/if (!${Ini[${Character_Ini},${Me.Class}].Length}) /call Make${Me.Class.ShortName}_Ini

|-------------------------------------------------------------------------------------------------Auto_Bash
	/call IniToVar "${Character_Ini},${Me.Class},Auto_Bash(Off)" UNDEFINED bool local
	/if (${Macro.Return}) /call BuildArray MyAbilities Bash/Ability

|-------------------------------------------------------------------------------------------------Auto_Taunt
	/call IniToVar "${Character_Ini},${Me.Class},Auto_Taunt(On/Off)" UNDEFINED bool local
	/if (${Macro.Return}) {
		/call BuildArray MyAbilities Taunt/Ability
		/declare AutoAggro bool outer TRUE
	}
	
/if (${Debug}) /echo <== PAL_Setup -|
/return



|--------------------------------------------------------------------|
|- MakePAL_Ini														-|
|--------------------------------------------------------------------|
|- Writes entries to the [Paladin] section of the Character_Ini.	-|
|--------------------------------------------------------------------|
Sub MakePAL_Ini
/if (${Debug}) /echo |- PAL_Ini ==>

	/call WriteToIni "${Character_Ini},${Me.Class},Auto_Bash(On/Off)" On 0
	/call WriteToIni "${Character_Ini},${Me.Class},Auto_Taunt(On/Off)" Off 0
	
/if (${Debug}) /echo <== PAL_Ini -|
/return



|------------------------------------------------------------------------|
|- PAL_Background_Events												-|
|------------------------------------------------------------------------|
|- Events to add to the Background_Events, that are paladin specific.	-|
|------------------------------------------------------------------------|
Sub PAL_Background_Events

/return