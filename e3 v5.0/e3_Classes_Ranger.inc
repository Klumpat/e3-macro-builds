|----------------------------------------------------------------------------|
|- e3_Classes_Ranger.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Ranger specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Ranger's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Ranger class dynamic.	-|
|--------------------------------------------------------------------|
Sub RNG_Main
/if (${Debug}) /echo |- RNG_Main ==>
	
	| check_CombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${ActionTaken}) {
	
		| Assist Functions.
		/if (${Assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== RNG_Main -|
/return



|--------------------------------------------------------------------|
|- Ranger's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Ranger class dynamic.	-|
|--------------------------------------------------------------------|
Sub RNG_Assist
/if (${Debug}) /echo |- RNG_Assist ==>

	/call e3_AssistFunctions
	
	| your code here |
	
/if (${Debug}) /echo <== RNG_Assist -|
/return



Sub RNG_Setup
/if (${Debug}) /echo |- RNG_Setup ==>
	
/if (${Debug}) /echo <== RNG_Setup -|
/return



Sub RNG_Background_Events
/if (${Debug}) /echo |- RNG_Background_Events ==>

/if (${Debug}) /echo <== RNG_Background_Events -|
/return



SUB RNG_MacroSettings
/if (${Debug}) /echo |- RNG_MacroSettings ==>

/if (${Debug}) /echo <== RNG_MacroSettings -|
/RETURN



SUB RNG_CharacterSettings
/if (${Debug}) /echo |- RNG_CharacterSettings ==>

/if (${Debug}) /echo <== RNG_CharacterSettings -|
/RETURN



Sub RNG_Aliases
/if (${Debug}) /echo |- RNG_Aliases ==>

/if (${Debug}) /echo <== RNG_Aliases -|
/return