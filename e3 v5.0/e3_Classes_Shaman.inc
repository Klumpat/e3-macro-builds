|----------------------------------------------------------------------------|
|- e3_Classes_Shaman.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Shaman specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Shaman's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Shaman class dynamic.	-|
|--------------------------------------------------------------------|
Sub SHM_Main
/if (${Debug}) /echo |- SHM_Main ==>

	| check_CommandDebuffs.
	/if (${Debuff_Targets.Size}) /call Debuffs_OnCommand
	/if (!${ActionTaken}) {
	
		| check_Heals.
		/if (${Heals}) /call CheckHeals
		/if (!${ActionTaken}) {
	
			| check_CombatBuffs.
			/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
			/if (!${ActionTaken}) {
	
				| Assist Functions.
				/if (${Assisting}) /call ${Me.Class.ShortName}_Assist
			}
		}
	}

/if (${Debug}) /echo <== SHM_Main -|
/return



|--------------------------------------------------------------------|
|- Shaman's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Shaman class dynamic.	-|
|--------------------------------------------------------------------|
Sub SHM_Assist
/if (${Debug}) /echo |- SHM_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== SHM_Assist -|
/return



Sub SHM_Setup
/if (${Debug}) /echo |- SHM_Setup ==>
	
/if (${Debug}) /echo <== SHM_Setup -|
/return



Sub SHM_Background_Events
/return



SUB SHM_MacroSettings
/if (${Debug}) /echo |- SHM_MacroSettings ==>

/if (${Debug}) /echo <== SHM_MacroSettings -|
/RETURN



SUB SHM_CharacterSettings
/if (${Debug}) /echo |- SHM_CharacterSettings ==>

/if (${Debug}) /echo <== SHM_CharacterSettings -|
/RETURN



Sub SHM_Aliases
/if (${Debug}) /echo |- SHM_Aliases ==>

/if (${Debug}) /echo <== SHM_Aliases -|
/return