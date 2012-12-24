|----------------------------------------------------------------------------|
|- e3_Classes_Cleric.inc v2.2 ( originally designed to work with e3.mac )	-|
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

|-------------------------------------------------------------------------------------------------------------------------------------#1 check_ChainHeal.
	/if (${Chain_Status.Equal[HEALING]}) /call ChainHeal
	/if (!${ActionTaken}) {
	
	|-----------------------------------------------------------------------------------------------------------------------------------------#2 check_Heals.
		/if (${Heals}) /call CheckHeals
		/if (${DivineArb_Pct} && ${Group}>2 && ${Me.AltAbilityReady[Divine Arbitration]}) /call check_DivineArb
		/if (!${ActionTaken}) {
		
		|-------------------------------------------------------------------------------------------------------------------------------------#3 Celestial Regen.
			/if (${celestialRegen_Pct} && ${Group}>2 && ${Me.AltAbilityReady[Celestial Regeneration]}) /call check_celestialRegen
		
		|-----------------------------------------------------------------------------------------------------------------------------------#4 check_CombatBuffs.
			/if (!${idle} && ${CombatBuffs.Size}) /call CombatBuffs
			/if (!${ActionTaken}) {
			
			|--------------------------------------------------------------------------------------------------------------------------------#5 check_CommandDebuffs.
				/if (${Debuff_Targets.Size}) /call Debuffs_OnCommand
				/if (!${ActionTaken}) {
				
				|------------------------------------------------------------------------------------------------------------------------------------#6 Assist Functions.
					/if (${Assisting}) /call CLR_Assist
					/if (!${ActionTaken}) {
					
					|------------------------------------------------------------------------------------------------------------------------------------------#7 Auto Yaulp.
						/if (${AutoYaulp} && !${Me.Sitting} && !${medBreak} && ${Me.PctMana} < 95)  /call auto_Yaulp
					}
				}
			}
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
	/if (!${ActionTaken}) {
		
	|----------------------------------------------------------------------------#5 Dots on assist.
		/if (${Assist_DoTs.Size}) /call DoTs_OnAssist
		/if (!${ActionTaken}) {
		
		|-------------------------------------------------------------------------------------#6 Nukes.		
			/if (${MyNukes.Size} && !${NukeTimer}) /call Nukes
		}
	}
	
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
	/call INItoVar "${Character_Ini},Cleric,Auto-Yaulp (On/Off)" AutoYaulp bool outer
	/call INItoVar "${Character_Ini},Cleric,Divine Arbitration (Pct)" DivineArb_Pct int outer
	/call INItoVar "${Character_Ini},Cleric,Celestial Regeneration (Pct)" celestialRegen_Pct int outer

/if (${Debug}) /echo <== CLR_Setup -|
/return



|----------------------------------------------------------------|
|- MakeCLR_Ini													-|
|----------------------------------------------------------------|
|- Writes entries to the [Cleric] section of the Character_Ini.	-|
|----------------------------------------------------------------|
Sub MakeCLR_Ini
/if (${Debug}) /echo |- CLR_Ini ==>

	/call WriteToIni "${Character_Ini},Cleric,Auto-Yaulp (On/Off)" On 0
	/call WriteToIni "${Character_Ini},Cleric,Divine Arbitration (Pct)" 30 0
	/call WriteToIni "${Character_Ini},Cleric,Celestial Regeneration (Pct)" 65 0
	
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
	/if ( ${Me.Class.ShortName.Equal[CLR]} && ${Spawn[${ChatSender}'s].ID}) {
		/tell ${ChatSender} Wait4Rez
		/if (${Select[${Me.Race},High Elf]}) /em pops a toot!
		/if (${FindItem[Water Sprinkler of Nem Ankh].ID}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Water Sprinkler of Nem Ankh" item
		} else /if (${Me.Book[Reviviscence]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Reviviscence" Gem${DefaultGem} 45s
		} else /if (${Me.Book[Ressurection]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Ressurection" Gem${DefaultGem} 45s
		} else /if (${Me.Book[Restoration]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Restoration" Gem${DefaultGem} 45s
		} else /if (${Me.Book[Resuscitate]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Resuscitate" Gem${DefaultGem} 45s
		} else /if (${Me.Book[Renewal]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Renewal" Gem${DefaultGem} 45s
		} else /if (${Me.Book[Revive]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Revive" Gem${DefaultGem} 45s
		} else /if (${Me.Book[Reparation]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Reparation" Gem${DefaultGem} 45s
		} else /if (${Me.Book[Reconstitution]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Reconstitution" Gem${DefaultGem} 45s
		} else /if (${Me.Book[Reanimation]}) {
			/call e3_Cast ${Spawn[${ChatSender}'s].ID} "Reanimation" Gem${DefaultGem} 45s
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
		/call e3_Cast ${Me.ID} ${Me.AltAbility[Divine Arbitration].ID} alt
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
		/call e3_Cast ${Me.ID} ${Me.AltAbility[Celestial Regeneration].ID} alt
	}
}
/return

SUB auto_Yaulp
/if (${Debug}) /echo |- auto_Yaulp ==>

	/if (${Me.SpellReady[Yaulp VI]} && !${Bool[${Me.Buff[Yaulp VI]}]}) {
		/call e3_Cast 0 "Yaulp VI" gem0
	} else /if (${Me.SpellReady[Yaulp V]} && !${Bool[${Me.Buff[Yaulp V]}]}) {
		/call e3_Cast 0 "Yaulp V" gem0
	} else /if (${Me.SpellReady[Yaulp IV]} && !${Bool[${Me.Buff[Yaulp IV]}]}) {
		/call e3_Cast 0 "Yaulp IV" gem0
	} else /if (${Me.SpellReady[Yaulp III]} && !${Bool[${Me.Buff[Yaulp III]}]}) {
		/call e3_Cast 0 "Yaulp III" gem0
	} else /if (${Me.SpellReady[Yaulp II]} && !${Bool[${Me.Buff[Yaulp II]}]}) {
		/call e3_Cast 0 "Yaulp II" gem0
	} else /if (${Me.SpellReady[Yaulp]} && !${Bool[${Me.Buff[Yaulp]}]}) {
		/call e3_Cast 0 "Yaulp" gem0
	}

/if (${Debug}) /echo <== auto_Yaulp -|
/RETURN