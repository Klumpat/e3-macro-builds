|----------------------------------------------------------------------------|
|- e3_Classes_Druid.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Druid specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Druid's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Druid class dynamic.	-|
|--------------------------------------------------------------------|
Sub DRU_Main
/if (${Debug}) /echo |- DRU_Main ==>

	| check_CombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${ActionTaken}) {
	
		| Assist Functions.
		/if (${Assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== DRU_Main -|
/return



|--------------------------------------------------------------------|
|- Druid's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Druid class dynamic.	-|
|--------------------------------------------------------------------|
Sub DRU_Assist
/if (${Debug}) /echo |- DRU_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== DRU_Assist -|
/return



Sub DRU_Setup
/if (${Debug}) /echo |- DRU_Setup ==>
	
/if (${Debug}) /echo <== DRU_Setup -|
/return



Sub DRU_Background_Events
/if (${Debug}) /echo |- DRU_Background_Events ==>

/if (${Debug}) /echo <== DRU_Background_Events -|
/return



SUB DRU_MacroSettings
/if (${Debug}) /echo |- DRU_MacroSettings ==>

/if (${Debug}) /echo <== DRU_MacroSettings -|
/RETURN



SUB DRU_CharacterSettings
/if (${Debug}) /echo |- DRU_CharacterSettings ==>

/if (${Debug}) /echo <== DRU_CharacterSettings -|
/RETURN



Sub DRU_Aliases
/if (${Debug}) /echo |- DRU_Aliases ==>

/if (${Debug}) /echo <== DRU_Aliases -|
/return