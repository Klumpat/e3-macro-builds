|----------------------------------------------------------------------------|
|- e3_Classes_Wizard.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Wizard specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Wizard's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Wizard class dynamic.	-|
|--------------------------------------------------------------------|
Sub WIZ_Main
/if (${Debug}) /echo |- WIZ_Main ==>

	| check_CombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${ActionTaken}) {
	
		| Assist Functions.
		/if (${Assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== WIZ_Main -|
/return



|--------------------------------------------------------------------|
|- Wizard's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Wizard class dynamic.	-|
|--------------------------------------------------------------------|
Sub WIZ_Assist
/if (${Debug}) /echo |- WIZ_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== WIZ_Assist -|
/return



Sub WIZ_Setup
/if (${Debug}) /echo |- WIZ_Setup ==>
	
/if (${Debug}) /echo <== WIZ_Setup -|
/return



Sub WIZ_Background_Events
/if (${Debug}) /echo |- WIZ_Background_Events ==>

/if (${Debug}) /echo <== WIZ_Background_Events -|
/return



SUB WIZ_MacroSettings
/if (${Debug}) /echo |- WIZ_MacroSettings ==>

/if (${Debug}) /echo <== WIZ_MacroSettings -|
/RETURN



SUB WIZ_CharacterSettings
/if (${Debug}) /echo |- WIZ_CharacterSettings ==>

/if (${Debug}) /echo <== WIZ_CharacterSettings -|
/RETURN



Sub WIZ_Aliases
/if (${Debug}) /echo |- WIZ_Aliases ==>

/if (${Debug}) /echo <== WIZ_Aliases -|
/return