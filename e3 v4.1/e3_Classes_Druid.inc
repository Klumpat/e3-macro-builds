|----------------------------------------------------------------------------|
|- e3_Classes_Druid.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains druid specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|


|-------check SoTW and CR for firing on zone

|--------------------------------------------------------------------|
|- Druid's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the druid class dynamic.	-|
|--------------------------------------------------------------------|
Sub DRU_Main
/if (${Debug}) /echo |- DRU_Main ==>

|-------------------------------------------------------------------------------------------------------------------------------------#1 check_ChainHeal.
	/if (${Chain_Status.Equal[HEALING]}) {
		/call ChainHeal
		/if (${ActionTaken}) /return
	}

|-----------------------------------------------------------------------------------------------------------------------------------------#2 check_Heals.
	/if (${Heals}) /call CheckHeals
	/if (${spiritofWood_Pct} && ${Group}>2 && !${alt_Timer_173}) /call check_spiritofWood
	/if (!${idle} && ${CombatBuffs.Size}) /call CombatBuffs
		/if (${ActionTaken}) /return
	
|--------------------------------------------------------------------------------------------------------------------------------#3 check_CommandDebuffs.
	/if (${Debuff_Targets.Size}) /call Debuffs_OnCommand
		/if (${ActionTaken}) /return
		
|------------------------------------------------------------------------------------------------------------------------------------------#4 check_Rain.	
	/if (${do_Rain} && ${Rain_Spells.Size}) /call Rain
		/if (${ActionTaken}) /return
		
|------------------------------------------------------------------------------------------------------------------------------------------#5 check_DoTs.	
	/if (${DoT_Targets.Size}) /call DoTs_OnCommand
		/if (${ActionTaken}) /return
		
|------------------------------------------------------------------------------------------------------------------------------------#6 Assist Functions.
	/if (${Assisting}) /call DRU_Assist
		/if (${ActionTaken}) /return
		
|-----------------------------------------------------------------------------------------------------------------------------------------#7 Auto Forage.
	/if (${autoForage}) /call AutoForage
	
/if (${Debug}) /echo <== DRU_Main -|
/return



|--------------------------------------------------------------------|
|- Druid's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the druid class dynamic.	-|
|--------------------------------------------------------------------|
Sub DRU_Assist
/if (${Debug}) /echo |- DRU_Assist ==>

|-----------------------------------------------------------------------#1 Check Asssit status.
	/call check_AssistStatus

|--------------------------------------------------------------------------------#2 QuickBurns.
	/if (${use_QBurn}) /call QuickBurns
	
|---------------------------------------------------------------------------------#3 FullBurns.	
	/if (${use_FBurn}) /call FullBurns

|-------------------------------------------------------------------------#4 Debuffs on assist.
	/if (${Assist_Debuffs.Size}) /call Debuffs_OnAssist
		/if (${ActionTaken}) /return
		
|----------------------------------------------------------------------------#5 Dots on assist.
	/if (${Assist_DoTs.Size}) /call DoTs_OnAssist
		/if (${ActionTaken}) /return	

|-------------------------------------------------------------------------------------#6 Nukes.		
	/if (${MyNukes.Size} && !${NukeTimer}) /call Nukes
	
/if (${Debug}) /echo <== DRU_Assist -|
/return



|--------------------------------------------------------------------|
|- DRU_Setup														-|
|--------------------------------------------------------------------|
|- Imports variables from the [Druid] section of the Character_Ini.	-|
|--------------------------------------------------------------------|
Sub DRU_Setup
/if (${Debug}) /echo |- DRU_Setup ==>

	/if (!${Ini[${Character_Ini},Druid].Length}) /call MakeDRU_Ini
	/call INItoVar "${Character_Ini},Druid,Evac Spell" Evac_Spell string outer
	/call IniToVar "${Character_Ini},Druid,Spirit of Wood (Pct)" spiritofWood_Pct int outer

/if (${Debug}) /echo <== DRU_Setup -|
/return



|----------------------------------------------------------------|
|- MakeCLR_Ini													-|
|----------------------------------------------------------------|
|- Writes entries to the [Cleric] section of the Character_Ini.	-|
|----------------------------------------------------------------|
Sub MakeDRU_Ini
/if (${Debug}) /echo |- DRU_Ini ==>

	/call WriteToIni "${Character_Ini},Druid,Auto-Forage (On/Off)" "*Do you want this character to automatically forage?" 0
	/call WriteToIni "${Character_Ini},Druid,Evac Spell" "*List an evac spell to use if Exodus is not ready." 0
	/call WriteToIni "${Character_Ini},Druid,Spirit of Wood (Pct)" "*Average group health to cast Spirit of Wood." 0
		
/if (${Debug}) /echo <== DRU_Ini -|
/return



|--------------------------------------------------------------------|
|- DRU_Background_Events											-|
|--------------------------------------------------------------------|
|- Events to add to the Background_Events, that are druid specific.	-|
|--------------------------------------------------------------------|
Sub DRU_Background_Events

/return



|------
|- check_spiritofWood
|-----------------
|- Uses the alternate ability 'Spirit of Wood' at a specified
|---------------------------
sub check_spiritofWood
/declare i int local
/declare average_HPs int local 0
/for i 1 to ${Group}
	/varcalc average_HPs ${average_HPs} + ${Group.Member[${i}].PctHPs}
/next i
/if (${Group}) /varcalc average_HPs ${average_HPs} / ${Group}
/if (${average_HPs}<${spiritofWood_Pct}) {
	/call e3_Cast 0 173 alt
}
/return