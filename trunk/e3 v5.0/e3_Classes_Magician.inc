|----------------------------------------------------------------------------|
|- e3_Classes_Magician.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Magician specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Magician's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Magician class dynamic.	-|
|--------------------------------------------------------------------|
Sub MAG_Main
/if (${Debug}) /echo |- MAG_Main ==>

	| check_CombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${ActionTaken}) {
	
		| Assist Functions.
		/if (${Assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== MAG_Main -|
/return



|--------------------------------------------------------------------|
|- Magician's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Magician class dynamic.	-|
|--------------------------------------------------------------------|
Sub MAG_Assist
/if (${Debug}) /echo |- MAG_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== MAG_Assist -|
/return



Sub MAG_Setup
/if (${Debug}) /echo |- MAG_Setup ==>
	
/if (${Debug}) /echo <== MAG_Setup -|
/return



Sub MAG_Background_Events
/if (${Debug}) /echo |- MAG_Background_Events ==>

/if (${Debug}) /echo <== MAG_Background_Events -|
/return



SUB MAG_MacroSettings
/if (${Debug}) /echo |- MAG_MacroSettings ==>

/if (${Debug}) /echo <== MAG_MacroSettings -|
/RETURN



SUB MAG_CharacterSettings
/if (${Debug}) /echo |- MAG_CharacterSettings ==>

/if (${Debug}) /echo <== MAG_CharacterSettings -|
/RETURN



Sub MAG_Aliases
/if (${Debug}) /echo |- MAG_Aliases ==>

/if (${Debug}) /echo <== MAG_Aliases -|
/return