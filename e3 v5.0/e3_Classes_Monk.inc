|------------------------------------------------------------------------|
|- e3_Classes_Monk.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains Monk specific functions related to,							-|
|-		Setup and Ini entry information									-|
|-		MainLoop, and AssistLoop hooks									-|
|-		Over-all functionality / flow									-|
|------------------------------------------------------------------------|



|----------------------------------------------------------------|
|- Monk's Class specific mainLoop hook.							-|
|----------------------------------------------------------------|
|- Prioritizes macro functions based on the Monk class dynamic.	-|
|----------------------------------------------------------------|
Sub MNK_Main
/if (${Debug}) /echo |- MNK_Main ==>

	| check_CombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${ActionTaken}) {
	
		| Assist Functions.
		/if (${Assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== MNK_Main -|
/return



|--------------------------------------------------------------------|
|- Monk's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Monk class dynamic.	-|
|--------------------------------------------------------------------|
Sub MNK_Assist
/if (${Debug}) /echo |- MNK_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== MNK_Assist -|
/return



Sub MNK_Setup
/if (${Debug}) /echo |- MNK_Setup ==>
	
/if (${Debug}) /echo <== MNK_Setup -|
/return



Sub MNK_Background_Events
/if (${Debug}) /echo |- MNK_Background_Events ==>

/if (${Debug}) /echo <== MNK_Background_Events -|
/return



SUB MNK_MacroSettings
/if (${Debug}) /echo |- MNK_MacroSettings ==>

/if (${Debug}) /echo <== MNK_MacroSettings -|
/RETURN



SUB MNK_CharacterSettings
/if (${Debug}) /echo |- MNK_CharacterSettings ==>

/if (${Debug}) /echo <== MNK_CharacterSettings -|
/RETURN



Sub MNK_Aliases
/if (${Debug}) /echo |- MNK_Aliases ==>

/if (${Debug}) /echo <== MNK_Aliases -|
/return