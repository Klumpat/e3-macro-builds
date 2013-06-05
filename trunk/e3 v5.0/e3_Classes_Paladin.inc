|----------------------------------------------------------------------------|
|- e3_Classes_Paladin.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Paladin specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Paladin's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Paladin class dynamic.	-|
|--------------------------------------------------------------------|
Sub PAL_Main
/if (${Debug}) /echo |- PAL_Main ==>

	| check_CombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${ActionTaken}) {
	
		| Assist Functions.
		/if (${Assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== PAL_Main -|
/return



|--------------------------------------------------------------------|
|- Paladin's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Paladin class dynamic.	-|
|--------------------------------------------------------------------|
Sub PAL_Assist
/if (${Debug}) /echo |- PAL_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== PAL_Assist -|
/return



Sub PAL_Setup
/if (${Debug}) /echo |- PAL_Setup ==>
	
/if (${Debug}) /echo <== PAL_Setup -|
/return



Sub PAL_Background_Events
/if (${Debug}) /echo |- PAL_Background_Events ==>

/if (${Debug}) /echo <== PAL_Background_Events -|
/return



SUB PAL_MacroSettings
/if (${Debug}) /echo |- PAL_MacroSettings ==>

/if (${Debug}) /echo <== PAL_MacroSettings -|
/RETURN



SUB PAL_CharacterSettings
/if (${Debug}) /echo |- PAL_CharacterSettings ==>

/if (${Debug}) /echo <== PAL_CharacterSettings -|
/RETURN



Sub PAL_Aliases
/if (${Debug}) /echo |- PAL_Aliases ==>

/if (${Debug}) /echo <== PAL_Aliases -|
/return