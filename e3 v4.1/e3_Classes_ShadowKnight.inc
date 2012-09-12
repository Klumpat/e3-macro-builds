|--------------------------------------------------------------------------------|
|- e3_Classes_ShadowKnight.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ													-|
|--------------------------------------------------------------------------------|
|- Contains shadow knight specific functions related to,						-|
|-		Setup and Ini entry information											-|
|-		MainLoop, and AssistLoop hooks											-|
|-		Over-all functionality / flow											-|
|--------------------------------------------------------------------------------|



|----------------------------------------------------------------------------|
|- Shadow Knight's Class specific mainLoop hook.							-|
|----------------------------------------------------------------------------|
|- Prioritizes macro functions based on the shadow knight class dynamic.	-|
|----------------------------------------------------------------------------|
Sub SHD_Main
/if (${Debug}) /echo |- SHD_Main ==>

|-----------------------------------------------------------------------------------------------------------------------------------#1 check_CombatBuffs.
	/if (!${idle} && ${PriorityBuffs.Size}) /call checkPriorityBuffs
		/if (${ActionTaken}) /return

|------------------------------------------------------------------------------------------------------------------------------------#2 Assist Functions.
	/if (${Assisting}) /call SHD_Assist
		/if (${ActionTaken}) /return
		
|------------------------------------------------------------------------------------------------------------------------------------------#3 check_DoTs.	
	/if (${DoT_Targets.Size}) /call DoTs_OnCommand
	
/if (${Debug}) /echo <== SHD_Main -|
/return



|----------------------------------------------------------------------------|
|- Shadow Knight's Class specific Assist Loop.								-|
|----------------------------------------------------------------------------|
|- Prioritizes Assist functions based on the shadow knight class dynamic.	-|
|----------------------------------------------------------------------------|
Sub SHD_Assist
/if (${Debug}) /echo |- SHD_Assist ==>

|-----------------------------------------------------------------------#1 Check Asssit status.
	/call check_AssistStatus

|--------------------------------------------------------------------------------#2 QuickBurns.
	/if (${use_QBurn}) /call QuickBurns
	
|---------------------------------------------------------------------------------#3 FullBurns.	
	/if (${use_FBurn}) /call FullBurns

|--------------------------------------------------------------------------#4 Combat Abilities.	
	/if (${MyAbilities.Size}) /call CombatAbilities
	
|----------------------------------------------------------------------------------#5 Life Tap.
	/if (${Me.PctHPs}<${LifeTapPct} && ${Me.CurrentMana}>${Spell[${LifeTapSpell}].Mana} && ${Spawn[${AssistTarget}].Distance}<${Spell[${LifeTapSpell}].MyRange}) {
		/call e3_Cast ${AssistTarget} "${LifeTapSpell}" Gem${LifeTapGem} 0s
	}
	
|-------------------------------------------------------------------------------------#6 Nukes.		
	/if (${MyNukes.Size} && !${NukeTimer}) /call Nukes
	
|-------------------------------------------------------------------------#7 Debuffs on assist.
	/if (${Assist_Debuffs.Size}) /call Debuffs_OnAssist
		/if (${ActionTaken}) /return
		
|----------------------------------------------------------------------------#8 Dots on assist.
	/if (${Assist_DoTs.Size}) /call DoTs_OnAssist
		/if (${ActionTaken}) /return

/if (${Debug}) /echo <== SHD_Assist -|
/return



|----------------------------------------------------------------------------|
|- SHD_Setup																-|
|----------------------------------------------------------------------------|
|- Imports variables from the [Shadow Knight] section of the Character_Ini.	-|
|----------------------------------------------------------------------------|
Sub SHD_Setup
/if (${Debug}) /echo |- SHD_Setup ==>

	/if (!${Ini[${Character_Ini},${Me.Class}].Length}) /call Make${Me.Class.ShortName}_Ini

|-------------------------------------------------------------------------------------------------Auto_Bash
	/call IniToVar "${Character_Ini},${Me.Class},Auto_Bash(On/Off)" UNDEFINED bool local
	/if (${Macro.Return}) /call BuildArray MyAbilities Bash/Ability

|-------------------------------------------------------------------------------------------------Auto_Taunt
	/call IniToVar "${Character_Ini},${Me.Class},Auto_Taunt(On/Off)" UNDEFINED bool local
	/if (${Macro.Return}) {
		/call BuildArray MyAbilities Taunt/Ability
		/declare do_Taunt bool outer TRUE
	}

	/if (${Ini[${Character_Ini},${Me.Class},LifeTap(Spell/Gem/Pct)].Length}) {
		/call INItoVar "${Character_Ini},${Me.Class},LifeTap(Spell/Gem/Pct)" UNDEFINED string local
		/declare LifeTapSpell string outer ${Macro.Return.Arg[1,/]}
		/declare LifeTapPct int outer ${Macro.Return.Arg[3,/]}
		/declare LifeTapGem int outer ${Macro.Return.Arg[2,/]}
	}

/if (${Debug}) /echo <== SHD_Setup -|	
/return



|------------------------------------------------------------------------|
|- MakeSHD_Ini															-|
|------------------------------------------------------------------------|
|- Writes entries to the [Shadow Knight] section of the Character_Ini.	-|
|------------------------------------------------------------------------|
Sub MakeSHD_Ini
/if (${Debug}) /echo |- SHD_Ini ==>
	/call WriteToIni "${Character_Ini},${Me.Class},Auto_Bash(On/Off)" On 0
	/call WriteToIni "${Character_Ini},${Me.Class},Auto_Taunt(On/Off)" Off 0
	/call WriteToIni "${Character_Ini},${Me.Class},LifeTap(Spell/Gem/Pct)" "*Lifetap spell to cast, gem to mem it to, and percent to cast it at."
	
/if (${Debug}) /echo <== SHD_Ini -|
/return



|----------------------------------------------------------------------------|
|- SHD_Background_Events													-|
|----------------------------------------------------------------------------|
|- Events to add to the Background_Events, that are shadow knight specific.	-|
|----------------------------------------------------------------------------|
Sub SHD_Background_Events

/return