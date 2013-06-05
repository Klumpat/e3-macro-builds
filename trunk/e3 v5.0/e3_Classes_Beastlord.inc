|----------------------------------------------------------------------------|
|- e3_Classes_Beastlord.inc v2.2 ( originally designed to work with e3.mac )-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains Beastlord specific functions related to,						-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Beastlord's Class specific mainLoop hook.						-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the Beastlord class dynamic.-|
|--------------------------------------------------------------------|
Sub BST_Main
/if (${Debug}) /echo |- BST_Main ==>

	| check_CombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${ActionTaken}) {
	
		| Assist Functions.
		/if (${Assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== BST_Main -|
/return



|------------------------------------------------------------------------|
|- Beastlord's Class specific Assist Loop.								-|
|------------------------------------------------------------------------|
|- Prioritizes Assist functions based on the Beastlord class dynamic.	-|
|------------------------------------------------------------------------|
Sub BST_Assist
/if (${Debug}) /echo |- BST_Assist ==>

	/call e3_AssistFunctions

/if (${Debug}) /echo <== BST_Assist -|
/return



Sub BST_Setup
/if (${Debug}) /echo |- BST_Setup ==>
	
/if (${Debug}) /echo <== BST_Setup -|
/return



Sub BST_Background_Events
/if (${Debug}) /echo |- BST_Background_Events ==>

/if (${Debug}) /echo <== BST_Background_Events -|
/return



SUB BST_MacroSettings
/if (${Debug}) /echo |- BST_MacroSettings ==>

/if (${Debug}) /echo <== BST_MacroSettings -|
/RETURN



SUB BST_CharacterSettings
/if (${Debug}) /echo |- BST_CharacterSettings ==>

/if (${Debug}) /echo <== BST_CharacterSettings -|
/RETURN



Sub BST_Aliases
/if (${Debug}) /echo |- BST_Aliases ==>

/if (${Debug}) /echo <== BST_Aliases -|
/return