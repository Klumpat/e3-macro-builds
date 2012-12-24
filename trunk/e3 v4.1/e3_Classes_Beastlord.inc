|--------------------------------------------------------------------------------|
|- e3_Classes_Beastlord.inc v2.1 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ													-|
|--------------------------------------------------------------------------------|
|- Contains beastlord specific functions related to,							-|
|-		Setup and Ini entry information											-|
|-		MainLoop, and AssistLoop hooks											-|
|-		Over-all functionality / flow											-|
|--------------------------------------------------------------------------------|



|------------------------------------------------------------------------|
|- Beastlord's Class specific mainLoop hook.							-|
|------------------------------------------------------------------------|
|- Prioritizes macro functions based on the beastlord class dynamic.	-|
|------------------------------------------------------------------------|
Sub BST_Main
/if (${Debug}) /echo |- BST_Main ==>

|--------------------------------------------------------------------------------------------------------------------------------#1 check_CommandDebuffs.
	/if (${Debuff_Targets.Size}) /call Debuffs_OnCommand
	/if (!${ActionTaken}) {
	
	|-----------------------------------------------------------------------------------------------------------------------------------#3 check_CombatBuffs.
		/if (!${idle} && ${CombatBuffs.Size}) /call CombatBuffs
		/if (!${ActionTaken}) {
		
		|--------------------------------------------------------------------------------------------------------------------------------------------#2 Pet heal.
			/if (${Defined[Pet_Heal]} && ${Me.Pet.ID}) /call PetHeal
			/if (!${ActionTaken}) {
			
			|-----------------------------------------------------------------------------------------------------------------------------------------#4 check_Heals.
				/if (${Heals}) /call CheckHeals
				/if (!${ActionTaken}) {
				
				|------------------------------------------------------------------------------------------------------------------------------------------#6 check_DoTs.	
					/if (${DoT_Targets.Size}) /call DoTs_OnCommand
					/if (!${ActionTaken}) {
					
					|------------------------------------------------------------------------------------------------------------------------------------#5 Assist Functions.		
						/if (${Assisting}) /call BST_Assist
					}
				}
			}
		}
	}

/if (${Debug}) /echo <== BST_Main -|
/return



|------------------------------------------------------------------------|
|- Beastlord's Class specific Assist Loop.								-|
|------------------------------------------------------------------------|
|- Prioritizes Assist functions based on the beastlord class dynamic.	-|
|------------------------------------------------------------------------|
Sub BST_Assist
/if (${Debug}) /echo |- BST_Assist ==>

|-----------------------------------------------------------------------#1 Check Asssit status.
	/call check_AssistStatus

|--------------------------------------------------------------------------------#2 QuickBurns.
	/if (${use_QBurn}) /call QuickBurns
	
|---------------------------------------------------------------------------------#3 FullBurns.	
	/if (${use_FBurn}) /call FullBurns
	
|--------------------------------------------------------------------------#4 Combat Abilities.
	/if (${MyAbilities.Size}) /call CombatAbilities

|-------------------------------------------------------------------------#5 Debuffs on assist.
	/if (${Assist_Debuffs.Size}) /call Debuffs_OnAssist	
	/if (!${ActionTaken}) {
	
	|-------------------------------------------------------------------------------------#6 Nukes.		
		/if (${MyNukes.Size} && !${NukeTimer}) /call Nukes
		/if (!${ActionTaken}) {
		
		|----------------------------------------------------------------------------#7 Dots on assist.
			/if (${Assist_DoTs.Size}) /call DoTs_OnAssist
		}
	}

/if (${Debug}) /echo <== BST_Assist -|	
/return



|------------------------------------------------------------------------|
|- BST_Setup															-|
|------------------------------------------------------------------------|
|- Imports variables from the [Beastlord] section of the Character_Ini.	-|
|------------------------------------------------------------------------|
Sub BST_Setup
/if (${Debug}) /echo |- BST_Setup ==>

	| /if (!${Ini[${Character_Ini},${Me.Class}].Length}) /call Make${Me.Class.ShortName}_Ini
	
/if (${Debug}) /echo <== BST_Setup -|
/return



|--------------------------------------------------------------------|
|- MakeBST_Ini														-|
|--------------------------------------------------------------------|
|- Writes entries to the [Beastlord] section of the Character_Ini.	-|
|--------------------------------------------------------------------|
Sub MakeBST_Ini
/if (${Debug}) /echo |- BST_Ini ==>

/if (${Debug}) /echo <== BST_Ini -|
/return



|------------------------------------------------------------------------|
|- BST_Background_Events												-|
|------------------------------------------------------------------------|
|- Events to add to the Background_Events, that are beastlord specific.	-|
|------------------------------------------------------------------------|
Sub BST_Background_Events

/return