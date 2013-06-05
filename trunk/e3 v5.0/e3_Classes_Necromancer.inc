|----------------------------------------------------------------------------|
|- e3_Classes_Necromancer.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Necromancer specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Necromancer's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Necromancer class dynamic.	-|
|--------------------------------------------------------------------|
Sub NEC_Main
/if (${Debug}) /echo |- NEC_Main ==>

	| check_CombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${ActionTaken}) {
	
		| Assist Functions.
		/if (${Assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== NEC_Main -|
/return



|--------------------------------------------------------------------|
|- Necromancer's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Necromancer class dynamic.	-|
|--------------------------------------------------------------------|
Sub NEC_Assist
/if (${Debug}) /echo |- NEC_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== NEC_Assist -|
/return



Sub NEC_Setup
/if (${Debug}) /echo |- NEC_Setup ==>
	
/if (${Debug}) /echo <== NEC_Setup -|
/return



Sub NEC_Background_Events
/if (${Debug}) /echo |- NEC_Background_Events ==>

/if (${Debug}) /echo <== NEC_Background_Events -|
/return



SUB NEC_MacroSettings
/if (${Debug}) /echo |- NEC_MacroSettings ==>

/if (${Debug}) /echo <== NEC_MacroSettings -|
/RETURN



SUB NEC_CharacterSettings
/if (${Debug}) /echo |- NEC_CharacterSettings ==>

/if (${Debug}) /echo <== NEC_CharacterSettings -|
/RETURN



Sub NEC_Aliases
/if (${Debug}) /echo |- NEC_Aliases ==>

/if (${Debug}) /echo <== NEC_Aliases -|
/return