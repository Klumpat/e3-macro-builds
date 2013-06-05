|----------------------------------------------------------------------------|
|- e3_Classes_CLR.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains CLR specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- CLR's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the CLR class dynamic.	-|
|--------------------------------------------------------------------|
Sub BER_Main
/if (${Debug}) /echo |- BER_Main ==>

	| check_CombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${ActionTaken}) {
	
		| Assist Functions.
		/if (${Assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== BER_Main -|
/return



|--------------------------------------------------------------------|
|- CLR's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the CLR class dynamic.	-|
|--------------------------------------------------------------------|
Sub BER_Assist
/if (${Debug}) /echo |- BER_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== BER_Assist -|
/return



Sub BER_Setup
/if (${Debug}) /echo |- BER_Setup ==>
	
/if (${Debug}) /echo <== BER_Setup -|
/return



Sub BER_Background_Events
/if (${Debug}) /echo |- BER_Background_Events ==>

/if (${Debug}) /echo <== BER_Background_Events -|
/return



SUB BER_MacroSettings
/if (${Debug}) /echo |- BER_MacroSettings ==>

/if (${Debug}) /echo <== BER_MacroSettings -|
/RETURN



SUB BER_CharacterSettings
/if (${Debug}) /echo |- BER_CharacterSettings ==>

/if (${Debug}) /echo <== BER_CharacterSettings -|
/RETURN



Sub BER_Aliases
/if (${Debug}) /echo |- BER_Aliases ==>

/if (${Debug}) /echo <== BER_Aliases -|
/return