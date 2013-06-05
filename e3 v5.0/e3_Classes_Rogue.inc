|----------------------------------------------------------------------------|
|- e3_Classes_Rogue.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Rogue specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Rogue's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Rogue class dynamic.	-|
|--------------------------------------------------------------------|
Sub ROG_Main
/if (${Debug}) /echo |- ROG_Main ==>

	| check_CombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${ActionTaken}) {
	
		| Assist Functions.
		/if (${Assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== ROG_Main -|
/return



|--------------------------------------------------------------------|
|- Rogue's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Rogue class dynamic.	-|
|--------------------------------------------------------------------|
Sub ROG_Assist
/if (${Debug}) /echo |- ROG_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== ROG_Assist -|
/return



Sub ROG_Setup
/if (${Debug}) /echo |- ROG_Setup ==>
	
/if (${Debug}) /echo <== ROG_Setup -|
/return



Sub ROG_Background_Events
/if (${Debug}) /echo |- ROG_Background_Events ==>

/if (${Debug}) /echo <== ROG_Background_Events -|
/return



SUB ROG_MacroSettings
/if (${Debug}) /echo |- ROG_MacroSettings ==>

/if (${Debug}) /echo <== ROG_MacroSettings -|
/RETURN



SUB ROG_CharacterSettings
/if (${Debug}) /echo |- ROG_CharacterSettings ==>

/if (${Debug}) /echo <== ROG_CharacterSettings -|
/RETURN



Sub ROG_Aliases
/if (${Debug}) /echo |- ROG_Aliases ==>

/if (${Debug}) /echo <== ROG_Aliases -|
/return