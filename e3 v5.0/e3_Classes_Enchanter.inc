|----------------------------------------------------------------------------|
|- e3_Classes_Enchanter.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Enchanter specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Enchanter's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Enchanter class dynamic.	-|
|--------------------------------------------------------------------|
Sub ENC_Main
/if (${Debug}) /echo |- ENC_Main ==>

	| check_CombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${ActionTaken}) {
	
		| Assist Functions.
		/if (${Assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== ENC_Main -|
/return



|--------------------------------------------------------------------|
|- Enchanter's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Enchanter class dynamic.	-|
|--------------------------------------------------------------------|
Sub ENC_Assist
/if (${Debug}) /echo |- ENC_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== ENC_Assist -|
/return



Sub ENC_Setup
/if (${Debug}) /echo |- ENC_Setup ==>
	
/if (${Debug}) /echo <== ENC_Setup -|
/return



Sub ENC_Background_Events
/if (${Debug}) /echo |- ENC_Background_Events ==>

/if (${Debug}) /echo <== ENC_Background_Events -|
/return



SUB ENC_MacroSettings
/if (${Debug}) /echo |- ENC_MacroSettings ==>

/if (${Debug}) /echo <== ENC_MacroSettings -|
/RETURN



SUB ENC_CharacterSettings
/if (${Debug}) /echo |- ENC_CharacterSettings ==>

/if (${Debug}) /echo <== ENC_CharacterSettings -|
/RETURN



Sub ENC_Aliases
/if (${Debug}) /echo |- ENC_Aliases ==>

/if (${Debug}) /echo <== ENC_Aliases -|
/return