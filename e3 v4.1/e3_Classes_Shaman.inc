|----------------------------------------------------------------------------|
|- e3_Classes_Shaman.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains shaman specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Shaman's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the shaman class dynamic.	-|
|--------------------------------------------------------------------|
Sub SHM_Main
/if (${Debug}) /echo |- SHM_Main ==>

|--------------------------------------------------------------------------------------------------------------------------------#1 check_CommandDebuffs.
	/if (${Debuff_Targets.Size}) /call Debuffs_OnCommand
		/if (${ActionTaken}) /return
	
|-----------------------------------------------------------------------------------------------------------------------------------------#2 check_Heals.
	/if (${Heals}) /call CheckHeals
		/if (${ActionTaken}) /return
	
|-----------------------------------------------------------------------------------------------------------------------------------#3 check_CombatBuffs.
	/if (!${idle} && ${PriorityBuffs.Size}) /call checkPriorityBuffs
		/if (${ActionTaken}) /return

|------------------------------------------------------------------------------------------------------------------------------------------#4 check_DoTs.	
	/if (${DoT_Targets.Size}) /call DoTs_OnCommand
		/if (${ActionTaken}) /return
		
|------------------------------------------------------------------------------------------------------------------------------------------#5 check_Rain.	
	/if (${do_Rain} && ${Rain_Spells.Size}) /call Rain
		/if (${ActionTaken}) /return

|------------------------------------------------------------------------------------------------------------------------------------#6 Assist Functions.
	/if (${Assisting}) /call SHM_Assist
		/if (${ActionTaken}) /return

|-------------------------------------------------------------------------------------------------------------------------------------------#7 AutoCanni.
	
	/echo /if (${AutoCanni} && ${Me.PctMana} < 95) /call autoCanni
	/if (${AutoCanni} && ${Me.PctMana} < 95) /call autoCanni
	
|--------------------------------------------------------------------------------------------------------------------------------------------#8 Pet heal.
	/if (${Defined[Pet_Heal]} && ${Me.Pet.ID}) /call PetHeal
	
/if (${Debug}) /echo <== SHM_Main -|
/return



|--------------------------------------------------------------------|
|- Shaman's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the shaman class dynamic.	-|
|--------------------------------------------------------------------|
Sub SHM_Assist
/if (${Debug}) /echo |- SHM_Assist ==>

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
	
/if (${Debug}) /echo <== SHM_Assist -|
/return



|------------------------------------------------------------------------|
|- SHM_Setup															-|
|------------------------------------------------------------------------|
|- Imports variables from the [Shaman] section of the Character_Ini.	-|
|------------------------------------------------------------------------|
Sub SHM_Setup
/if (${Debug}) /echo |- SHM_Setup ==>

	/if (!${Ini[${Character_Ini},Shaman].Length}) /call MakeSHM_Ini
	/varset Debug TRUE
	/call INItoVar ${Character_Ini},Shaman,AutoCanni(On/Off) AutoCanni bool outer
	/varset Debug FALSE
/if (${Debug}) /echo <== SHM_Setup -|
/return



|----------------------------------------------------------------|
|- MakeSHM_Ini													-|
|----------------------------------------------------------------|
|- Writes entries to the [Shaman] section of the Character_Ini.	-|
|----------------------------------------------------------------|
Sub MakeSHM_Ini
/if (${Debug}) /echo |- SHM_Ini ==>

	/call WriteToIni ${Character_Ini},Shaman,AutoCanni(On/Off)
	
/if (${Debug}) /echo <== SHM_Ini -|
/return



|------------------------------------------------------------------------|
|- SHM_Background_Events												-|
|------------------------------------------------------------------------|
|- Events to add to the Background_Events, that are Shaman specific.	-|
|------------------------------------------------------------------------|
Sub SHM_Background_Events

/return



|--------------------------------------------------------|
|- autoCanni											-|
|--------------------------------------------------------|
|- Uses cannibalize spells and AAs when under 95% mana.	-|
|- Spells must be memmed to be used automatically.		-|
|--------------------------------------------------------|
SUB autoCanni
/if (${Debug}) /echo |- autoCanni ==>

|--------------------------------------------------------------------------------------------------------------------------------------AltAbility Cannibalization.
	/if (${Me.PctHPs} > 65 && ${Me.PctMana} < 65 && !${alt_Timer_${Me.AltAbility[Cannibalization].ID}}) /call e3_Cast 0 "Cannibalization" alt

|------------------------------------------------------------------------------------------------------------------------------------------------AutoCanni spells.
	/if (${AutoCanni} && ${Me.PctMana} < 95 && ${Me.PctHPs} > 35) {
		/if (${medBreak}) /varset medBreak FALSE
		/if (${Me.SpellReady[Ancient: Chaotic Pain]}) {
			/call e3_Cast 0 "Ancient: Chaotic Pain" gem0
		} else /if (${Me.SpellReady[Cannibalize IV]}) {
			/call e3_Cast 0 "Cannibalize IV" gem0
		} else /if (${Me.SpellReady[Cannibalize III]}) {
			/call e3_Cast 0 "Cannibalize III" gem0
		} else /if (${Me.SpellReady[Cannibalize II]}) {
			/call e3_Cast 0 "Cannibalize II" gem0
		} else /if (${Me.SpellReady[Cannibalize]}) {
			/call e3_Cast 0 "Cannibalize" gem0
		}
	}
	
/if (${Debug}) /echo <== autoCanni -|
/RETURN