|----------------------------------------------------------------------------|
|- e3_Classes_Shadow Knight.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Shadow Knight specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Shadow Knight's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Shadow Knight class dynamic.	-|
|--------------------------------------------------------------------|
Sub SHD_Main
/if (${Debug}) /echo |- SHD_Main ==>

	| check_CombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${ActionTaken}) {
	
		| Assist Functions.
		/if (${Assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== SHD_Main -|
/return



|--------------------------------------------------------------------|
|- Shadow Knight's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Shadow Knight class dynamic.	-|
|--------------------------------------------------------------------|
Sub SHD_Assist
/if (${Debug}) /echo |- SHD_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== SHD_Assist -|
/return



Sub SHD_Setup
/if (${Debug}) /echo |- SHD_Setup ==>
	
/if (${Debug}) /echo <== SHD_Setup -|
/return



Sub SHD_Background_Events
/if (${Debug}) /echo |- SHD_Background_Events ==>

/if (${Debug}) /echo <== SHD_Background_Events -|
/return



SUB SHD_MacroSettings
/if (${Debug}) /echo |- SHD_MacroSettings ==>

/if (${Debug}) /echo <== SHD_MacroSettings -|
/RETURN



SUB SHD_CharacterSettings
/if (${Debug}) /echo |- SHD_CharacterSettings ==>

/if (${Debug}) /echo <== SHD_CharacterSettings -|
/RETURN



Sub SHD_Aliases
/if (${Debug}) /echo |- SHD_Aliases ==>

/if (${Debug}) /echo <== SHD_Aliases -|
/return