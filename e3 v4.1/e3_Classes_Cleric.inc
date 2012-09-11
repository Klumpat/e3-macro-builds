|----------------------------------------------------------------------------|
|- e3_Classes_Cleric.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains cleric specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Cleric's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the clerc class dynamic.	-|
|--------------------------------------------------------------------|
Sub CLR_Main
/if (${Debug}) /echo |- CLR_Main ==>

/if (${DivineArb_Pct} && ${Group}>2 && !${alt_Timer_157} && ${Me.AltAbilityReady[157]}) /call check_DivineArb

|-----------------------------------------------------------------------------------------------------------------------------------------#1 check_Heals.
	/if (${Heals}) /call CheckHeals
		/if (${ActionTaken}) /return

|-------------------------------------------------------------------------------------------------------------------------------------#2 Celestial Regen.
	/if (${celestialRegen_Pct} && ${Group}>2 && !${alt_Timer_38}) /call check_celestialRegen

|-----------------------------------------------------------------------------------------------------------------------------------#2 check_CombatBuffs.
	/if (!${idle} && ${PriorityBuffs.Size}) /call checkPriorityBuffs
		/if (${ActionTaken}) /return
		
|--------------------------------------------------------------------------------------------------------------------------------#3 check_CommandDebuffs.
	/if (${Debuff_Targets.Size}) /call Debuffs_OnCommand
		/if (${ActionTaken}) /return
		
|------------------------------------------------------------------------------------------------------------------------------------#4 Assist Functions.
	/if (${Assisting}) /call SHM_Assist
		/if (${ActionTaken}) /return
		
|------------------------------------------------------------------------------------------------------------------------------------------#5 Auto Yaulp.
/if (${AutoYaulp} && !${Me.Sitting} && !${medBreak} && ${Me.PctMana} < 95) {
	/if (${Me.SpellReady[Yaulp VI]} && !${Bool[${Me.Buff[Yaulp VI]}]}) {
		/call e3_Cast ${Me.ID} "Yaulp VI" gem0
	} else /if (${Me.SpellReady[Yaulp V]} && !${Bool[${Me.Buff[Yaulp V]}]}) {
		/call e3_Cast ${Me.ID} "Yaulp V" gem0
	} else /if (${Me.SpellReady[Yaulp IV]} && !${Bool[${Me.Buff[Yaulp IV]}]}) {
		/call e3_Cast ${Me.ID} "Yaulp IV" gem0
	} else /if (${Me.SpellReady[Yaulp III]} && !${Bool[${Me.Buff[Yaulp III]}]}) {
		/call e3_Cast ${Me.ID} "Yaulp III" gem0
	} else /if (${Me.SpellReady[Yaulp II]} && !${Bool[${Me.Buff[Yaulp II]}]}) {
		/call e3_Cast ${Me.ID} "Yaulp II" gem0
	} else /if (${Me.SpellReady[Yaulp]} && !${Bool[${Me.Buff[Yaulp]}]}) {
		/call e3_Cast ${Me.ID} "Yaulp" gem0
	}
}

/if (${Debug}) /echo <== CLR_Main -|
/return



|--------------------------------------------------------------------|
|- Cleric's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the cleric class dynamic.	-|
|--------------------------------------------------------------------|
Sub CLR_Assist
/if (${Debug}) /echo |- CLR_Assist ==>

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

/if (${Debug}) /echo <== CLR_Assist -|
/return



|------------------------------------------------------------------------|
|- CLR_Setup															-|
|------------------------------------------------------------------------|
|- Imports variables from the [Cleric] section of the Character_Ini.	-|
|------------------------------------------------------------------------|
Sub CLR_Setup
/if (${Debug}) /echo |- CLR_Setup ==>

	/if (!${Ini[${Character_Ini},Cleric].Length}) /call MakeCLR_Ini
	/call INItoVar ${Character_Ini},Cleric,Auto_Yaulp(On/Off) AutoYaulp bool outer
	/call INItoVar ${Character_Ini},Cleric,DivineArbitration_Pct DivineArb_Pct int outer
	/call INItoVar ${Character_Ini},Cleric,CelestialRegeneration_Pct celestialRegen_Pct int outer

/if (${Debug}) /echo <== CLR_Setup -|
/return



|----------------------------------------------------------------|
|- MakeCLR_Ini													-|
|----------------------------------------------------------------|
|- Writes entries to the [Cleric] section of the Character_Ini.	-|
|----------------------------------------------------------------|
Sub MakeCLR_Ini
/if (${Debug}) /echo |- CLR_Ini ==>

	/call WriteToIni ${Character_Ini},Cleric,Auto_Yaulp(On/Off) On 0
	/call WriteToIni ${Character_Ini},Cleric,DivineArbitration_Pct 30 0
	/call WriteToIni ${Character_Ini},Cleric,CelestialRegeneration_Pct 65 0
	
/if (${Debug}) /echo <== CLR_Ini -|
/return



|------------------------------------------------------------------------|
|- CLR_Background_Events												-|
|------------------------------------------------------------------------|
|- Events to add to the Background_Events, that are cleric specific.	-|
|------------------------------------------------------------------------|
Sub CLR_Background_Events

/return

#event Rez "<#1#> Rez"
#event Rez "<#1#> Rezz"
#event Rez "<#1#> Rez me"
#event Rez "<#1#> Rezz me"
#event Rez "<#*#> RezOn #1#"
#event Rez "<#*#> Rez On #1#"
#event Rez "#1# tells you, 'Rez'"
#event Rez "#1# tells you, 'Rezz'"
#event Rez "#1# tells you, 'Rez me'"
#event Rez "#1# tells you, 'Rezz me'"
#event Rez "#*# tells you, 'RezOn #1#'"
#event Rez "#*# tells you, 'Rez On #1#'"
#event Rez "#1# tells the group, 'Rez'"
#event Rez "#1# tells the group, 'Rezz'"
#event Rez "#1# tells the group, 'Rez me'"
#event Rez "#1# tells the group, 'Rezz me'"
#event Rez "#*# tells the group, 'RezOn #1#'"
#event Rez "#*# tells the group, 'Rez On #1#'"
sub Event_Rez(string line, string ChatSender)
/if (${Me.Class.ShortName.Equal[CLR]}) {
	/if (${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}
	/if ( ${Select[${Me.Class},Cleric]} && ${Spawn[${ChatSender}'s].ID}) {
		/tell ${ChatSender} Wait4Rez
		/if (${FindItem[Water Sprinkler of Nem Ankh].ID}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Water Sprinkler of Nem Ankh" item 0s
		} else /if (!${FindItem[Water Sprinkler of Nem Ankh].ID}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Reviviscence" Gem${DefaultGem} 45s
		}
	} else /if (!${Spawn[${ChatSender}'s].ID}) {
		/tell ${ChatSender} I don't see any of your corpses in ${Zone}.
	}
}
/return

sub check_DivineArb
/declare i int local
/for i 0 to ${Group}
	/if (${Spawn[${Group.Member[${i}]}].PctHPs}<${DivineArb_Pct} && ${Spawn[${Group.Member[${i}]}].PctHPs}) {
		/call e3_Cast 0 157 alt
	}
/next i
/return

sub check_celestialRegen
/declare i int local
/declare denominator int local 0
/declare average_HPs int local 1
/for i 1 to ${Group}
	/if (${SpawnCount[pc ${Group.Member[${i}]}]}) {
		/varcalc average_HPs ${average_HPs} + ${Group.Member[${i}].PctHPs}
		/varcalc denominator + 1
	}
/next i
/if (${denominator}>1) {
	/varcalc average_HPs ${average_HPs} / ${denominator}
	/if (${average_HPs}<${celestialRegen_Pct}) {
		/call e3_Cast ${Me.ID} 38 alt
	}
}
/return