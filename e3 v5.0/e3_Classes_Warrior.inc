|----------------------------------------------------------------------------|
|- e3_Classes_Warrior.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Warrior specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Warrior's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Warrior class dynamic.	-|
|--------------------------------------------------------------------|
Sub WAR_Main
/if (${Debug}) /echo |- WAR_Main ==>

	| check_CombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${ActionTaken}) {
	
		| Assist Functions.
		/if (${Assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== WAR_Main -|
/return



|--------------------------------------------------------------------|
|- Warrior's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Warrior class dynamic.	-|
|--------------------------------------------------------------------|
Sub WAR_Assist
/if (${Debug}) /echo |- WAR_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== WAR_Assist -|
/return



Sub WAR_Setup
/if (${Debug}) /echo |- WAR_Setup ==>
	
/if (${Debug}) /echo <== WAR_Setup -|
/return



Sub WAR_Background_Events
/if (${Debug}) /echo |- WAR_Background_Events ==>

/if (${Debug}) /echo <== WAR_Background_Events -|
/return



SUB WAR_MacroSettings
/if (${Debug}) /echo |- WAR_MacroSettings ==>

/if (${Debug}) /echo <== WAR_MacroSettings -|
/RETURN



SUB WAR_CharacterSettings
/if (${Debug}) /echo |- WAR_CharacterSettings ==>

/if (${Debug}) /echo <== WAR_CharacterSettings -|
/RETURN



Sub WAR_Aliases
/if (${Debug}) /echo |- WAR_Aliases ==>

/if (${Debug}) /echo <== WAR_Aliases -|
/return