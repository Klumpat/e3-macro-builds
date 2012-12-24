|--------------------------------------------------------------------------------|
|- e3_Classes_Enchanter.inc v2.2 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ													-|
|--------------------------------------------------------------------------------|
|- Contains enchanter specific functions related to,							-|
|-		Setup and Ini entry information											-|
|-		MainLoop, and AssistLoop hooks											-|
|-		Over-all functionality, and flow of the macro.							-|
|--------------------------------------------------------------------------------|



|------------------------------------------------------------------------|
|- Enchanter's Class specific mainLoop hook.							-|
|------------------------------------------------------------------------|
|- Prioritizes macro functions based on the enchanter class dynamic.	-|
|------------------------------------------------------------------------|
Sub ENC_Main
/if (${Debug}) /echo |- ENC_Main ==>

|-----------------------------------------------------------------------------------------------------------------------------------#1 check_PriorityBuffs (Rune).
	/if (!${idle} && ${RuneTargets.Size}) /call ChainRune
	/if (!${ActionTaken}) {
	
	|-----------------------------------------------------------------------------------------------------------------------------------#1 check_PriorityBuffs (Rune).
		/if (!${idle} && ${CombatBuffs.Size}) /call CombatBuffs
		/if (!${ActionTaken}) {
		
		|------------------------------------------------------------------------------------------------------------------------------------------------#2 check_AutoMez.
			/if (!${idle} && ${do_AutoMez} && ${SpawnCount[npc radius 100]} > 2) /call event_AutoMez
			/if (!${ActionTaken}) {
			
			|------------------------------------------------------------------------------------------------------------------------------------------------#3 check_Debuffs.
				/if (${Debuff_Targets.Size}) /call Debuffs_OnCommand
				/if (!${ActionTaken}) {
				
				|---------------------------------------------------------------------------------------------------------------------------------------------------#4 check_DoTs.	
					/if (${DoT_Targets.Size}) /call DoTs_OnCommand
					/if (!${ActionTaken}) {
					
					|---------------------------------------------------------------------------------------------------------------------------------------------------#5 check_Rain.	
						/if (${do_Rain} && ${Rain_Spells.Size}) /call Rain
						/if (!${ActionTaken}) {
						
						|---------------------------------------------------------------------------------------------------------------------------------------------#6 Assist Functions.
							/if (${Assisting}) /call ENC_Assist
						}
					}
				}
			}
		}
	}
	
/if (${Debug}) /echo <== ENC_Main -|
/return



|------------------------------------------------------------------------|
|- Enchanter's Class specific Assist Loop.								-|
|------------------------------------------------------------------------|
|- Prioritizes Assist functions based on the enchanter class dynamic.	-|
|------------------------------------------------------------------------|
Sub ENC_Assist
/if (${Debug}) /echo |- ENC_Assist ==>

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
	
/if (${Debug}) /echo <== ENC_Assist -|
/return



|------------------------------------------------------------------------|
|- ENC_Setup															-|
|------------------------------------------------------------------------|
|- Imports variables from the [Enchanter] section of the Character_Ini.	-|
|------------------------------------------------------------------------|
Sub ENC_Setup
/if (${Debug}) /echo |- ENC_Setup ==>

	/if (${Ini[${Character_Ini},Enchanter,Rune (Spell/Gem)].Length}) /call IniToVar "${Character_Ini},Enchanter,Rune (Spell/Gem)" chainRune string outer
	
	/if (${Ini[${Character_Ini},Enchanter,${Character_Ini},Enchanter,Auto-Mez (On/Off)].Length}) /call IniToVar "${Character_Ini},Enchanter,Auto-Mez (On/Off)" autoMez bool outer
	/if (${Ini[${Character_Ini},Enchanter,Auto-Mez (Spell/Gem/MaxLevel)].Length}) /call IniToVar "${Character_Ini},Enchanter,Auto-Mez (Spell/Gem/MaxLevel)" autoMez_Spell string outer
	
	/if (${Ini[${Character_Ini},Enchanter,Chain Stun#1].Length}) /call IniToArray "${Character_Ini},Enchanter,Chain Stun#" chainStuns
	
/if (${Debug}) /echo <== ENC_Setup -|
/return

	/if (!${Ini[${Character_Ini},Enchanter].Length}) /call MakeENC_Ini

|--------------------------------------------------------------------|
|- MakeENC_Ini														-|
|--------------------------------------------------------------------|
|- Writes entries to the [Enchanter] section of the Character_Ini.	-|
|--------------------------------------------------------------------|
Sub MakeENC_Ini
/if (${Debug}) /echo |- ENC_Ini ==>

	/call WriteToIni "${Character_Ini},Enchanter,Rune (Spell/Gem)"
|** Work in progress...
	/call WriteToIni "${Character_Ini},Enchanter,Auto-Mez (On/Off)"
	/call WriteToIni "${Character_Ini},Enchanter,Auto-Mez (Spell/Gem/MaxLevel)"
	
	/call WriteToIni "${Character_Ini},Enchanter,Chain Stun#1"
**|
/if (${Debug}) /echo <== ENC_Ini -|
/return



|------------------------------------------------------------------------|
|- ENC_Background_Events												-|
|------------------------------------------------------------------------|
|- Events to add to the Background_Events, that are enchanter specific.	-|
|------------------------------------------------------------------------|
Sub ENC_Background_Events
/if (${Debug}) /echo |- ENC_Background_Events ==>

	/doevents add_RuneTarget
	/doevents remove_RuneTarget
	
/if (${Debug}) /echo <== ENC_Background_Events -|
/return



#event add_RuneTarget "<#1#> Rune on"
#event add_RuneTarget "<#*#> Rune on #1#"
sub event_add_RuneTarget(string line, string RuneTarget)
/if (${Me.Class.ShortName.Equal[ENC]}) {
/if (${Debug}) /echo |- event_add_RuneTarget ==>
	
	/if (!${SpawnCount[pc ${RuneTarget}]}) {
		/docommand ${ChatToggle} I don't see any PCs matching [${RuneTarget}]...
	} else {
	
		/varset RuneTarget ${Spawn[pc ${RuneTarget}].CleanName}
	
		/if (!${Defined[RuneTargets]}) {
			/declare RuneTargets[1] string outer
			/varset RuneTargets[1] ${RuneTarget}
			/docommand ${ChatToggle} Maintaining [${chainRune.Arg[1,/]}] on [${RuneTarget}].
		} else {
			
			/declare i int local
			
			/declare RuneTarget_Names string local
			
			/for i 1 to ${RuneTargets.Size}
			
				/varset RuneTarget_Names ${RuneTarget_Names}${RuneTargets[${i}]},
			
			/next i
			
			/if (${Select[${RuneTarget},${RuneTarget_Names}]}) {
				/docommand ${ChatToggle} I am already maintaining [${chainRune.Arg[1,/]}] on [${RuneTarget}].
			} else {
				/call BuildArray RuneTargets ${RuneTarget}
				/docommand ${ChatToggle} Maintaining [${chainRune.Arg[1,/]}] on [${RuneTarget_Names}${RuneTarget}].
			}
		}
	}
	
/if (${Debug}) /echo <== add_RuneTarget -|
}
/return



#event remove_RuneTarget "<#1#> Rune Off"
#event remove_RuneTarget "<#*#> Rune Off #1#"
#event remove_RuneTarget "<#*#> End Rune on #1#"
#event remove_RuneTarget "<#*#> All runes off"
#event remove_RuneTarget "<#*#> end runes"
sub event_remove_RuneTarget(line, ChatSender)
/if (${Me.Class.ShortName.Equal[ENC]}) {
/if (${Debug}) /echo |- remove_RuneTarget ==>

	/if (${Defined[RuneTargets]}) {

		/if (${Defined[ChatSender]}) {
		
			/declare i int local
			
			/for i 1 to ${RuneTargets.Size}

				/if (${RuneTargets[${i}].Equal[${ChatSender}]}) {
					/docommand ${ChatToggle} No longer maintaining [${chainRune.Arg[1,/]}] on [${RuneTargets[${i}]}].
					/call RemoveArrayElement RuneTargets "${RuneTargets[${i}]}"
				}
			
			/next i
			
		} else {
			/docommand ${ChatToggle} Ending chain runes.
			/deletevar RuneTargets
		}
		
	}
	
/if (${Debug}) /echo <== remove_RuneTarget -|
}
/return



SUB chainRune
/if (${Debug}) /echo |- chainRune ==>

	/declare i int local
	
	/declare runeSpell string local ${chainRune.Arg[1,/]}
	/declare runeGem string local ${chainRune.Arg[2,/].Right[1]}
	/if (${chainRune.Arg[3,/].Length} && ${chainRune.Arg[3,/].Find[reagent::]}) /declare runeReagent string local ${chainRune.Arg[3,/].Arg[2,::]}
	
	/if (${Defined[runeReagent]} && !${FindItemCount[${runeReagent}]}) {
		/docommand ${ChatToggle} I have run out of [${runeReagent}] and can no longer maintain [${runeSpell}].  Ending chain runes.
		/deletevar RuneTargets
	} else {
	
		/for i 1 to ${RuneTargets.Size}
			/if (${Spawn[pc ${RuneTargets[${i}]}].Distance} < ${Spell[${runeSpell}].MyRange} && ${Me.CurrentMana} > ${Spell[${runeSpell}].Mana} && !${NetBots[${RuneTargets[${i}]}].Buff.Find[${Spell[${runeSpell}].ID}]}) {
				/call e3_Cast ${NetBots[${RuneTargets[${i}]}].ID} "${runeSpell}" gem${runeGem} ${If[${Me.Gem[${runeSpell}]},3s,${Spell[${runeSpell}].RecastTime}s]}
			}
		/if (!${ActionTaken}) /next i
	}

/if (${Debug}) /echo <== chainRune -|
/RETURN

|** Work in progress...

#event Stuns_On "<#*#> Stuns On"
#event Stuns_Off "<#*#> Stuns Off"
Sub Event_Stuns_On

/return

/call INItoVar "${Character_Ini},Enchanter,Cast Stuns (On/Off)" StunsOn bool outer 
/if (${StunsOn}) {
	/declare StunZone int local ${Zone.ID}
	/call INItoArray ${Character_Ini},Enchanter,Stun# Stun_Spells
	/call INItoVar ${Macro_Ini},Settings,AE_Radius AE_Radius int outer
	/if (${Stun_Spells.Size} > 0) {
		/declare i int local
		/for i 1 to ${Stun_Spells.Size}
			/if (!${Me.SpellReady[${Stun_Spells[${i}].Arg[1,/]}]}) /memspellset Stuns
			:Memloop
			/if (${Window[SpellBookWnd].Open}) /goto :Memloop
		/next i
		/declare do_Stuns bool outer TRUE
		/docommand ${ChatToggle} Starting stuns
		:Stun_Loop
		| /if (${SpawnCount[npc radius ${Math.Calc[${AE_Radius}+50]}]} > 0) {
			/for i 1 to ${Stun_Spells.Size}
				/if (!${do_Stuns}) /goto :end
				/call e3_Cast ${Me.ID} "${Stun_Spells[${i}].Arg[1,/]}" Gem${Stun_Spells[${i}].Arg[2,/]} 4s
			/next i
		| }
		/if (${Zone.ID} != ${StunZone}) /call Event_Stuns_Off
		/if (${do_Stuns}) /goto :Stun_Loop 
	}
	:end
}

Sub Event_Stuns_Off

/return

/if (${Defined[do_Stuns]}) {
	/deletevar do_Stuns
	/docommand ${ChatToggle} Ending stuns
	/call interrupt
	/doevents flush
	/varset CombatTimer 7s
}

Sub StunCheck

/return

/doevents Stuns_Off

| --------------------------------------------------------------------------------------------------------------------------------------------------------------

/notify largeDialogWindow LDW_YesButton leftmouse up

${Window[ConfirmationDialogBox].Child[CD_TextOutput].Text.Find[ re]}

#event AutoMez "<#*#> Mez All"
sub event_AutoMez
/if (${Debug}) /echo |- AutoMez -| ==>
/if (!${Defined[AutoMez_Spell]} && ${Ini[${Character_Ini},Enchanter,Auto-Mez (Spell/Gem/MaxLevel)].Length}) {
	/call INItoVar "${Character_Ini},Enchanter,Auto-Mez (Spell/Gem/MaxLevel)" UNDEFINED string local
	/declare AutoMez_Spell string outer ${Macro.Return.Arg[1,/]}
	/declare AutoMez_Gem int outer ${Macro.Return.Arg[2,/]}
	/declare MaxMezLevel int outer ${Macro.Return.Arg[3,/]}
}
/call INItoVar ${Macro_Ini},Settings,AE_Radius Rain_Radius int outer
/declare i int local
/for i 1 to ${SpawnCount[npc radius ${Rain_Radius}]}
/if (${Spawn[${NearestSpawn[${i},NPC radius ${Rain_Radius}]}].LineOfSight} && ${Spawn[${NearestSpawn[${i},NPC radius ${Rain_Radius}]}].Type.NotEqual[Untargetable]} && ${Spawn[${NearestSpawn[${i},NPC radius ${Rain_Radius}]}].Type.NotEqual[Pet]} && ${Spawn[${NearestSpawn[${i},NPC radius ${Rain_Radius}]}].Level} <= ${MaxMezLevel}) {
	/call TrueTarget ${NearestSpawn[${i},NPC radius ${Rain_Radius}].ID}
	/if (${Spawn[${Me.TargetOfTarget}].Type.Equal[PC]}) {
		/if (${Debug}) /echo Spawn: [${NearestSpawn[${i},NPC radius ${Rain_Radius}].CleanName},${NearestSpawn[${i},NPC radius ${Rain_Radius}].ID}] || TargetOfTarget: [${Me.TargetOfTarget}]
		/call MezTarget
	}
	/squelch /target clear
}
/delay 1
/next i
/if (${Debug}) {
	/echo |- AutoMez -| <==
}
/return
sub MezTarget
/declare GiveUpValue int local 3
/if (${${Target.ID}${Spell[${AutoMez_Spell}].ID}} == 0) {
	/if (!${Defined[${Target.ID}${Spell[${AutoMez_Spell}].ID}]}) /declare ${Target.ID}${Spell[${AutoMez_Spell}].ID} timer outer 0
	:recast
	/call e3_Cast ${Target.ID} "${AutoMez_Spell}" Gem${AutoMez_Gem} 5s
	/if (${Select[${castReturn},CAST_RESISTED]}) {
		/if (!${Defined[ResistRetry]}) /declare ResistRetry int local 0
		/docommand ${ChatToggle} **Warning** ${Target.CleanName} resisted ${AutoMez_Spell}.
		/if (${Defined[ResistRetry]}) /varcalc ResistRetry ${ResistRetry}+1
		/if (${ResistRetry} < ${GiveUpValue}) /goto :recast
	} else /if (${Select[${castReturn},CAST_IMMUNE]}) {
		/varset ${Target.ID}${Spell[${AutoMez_Spell}].ID} 20000000
		/docommand ${ChatToggle} **Warning** ${Target.CleanName} is IMMUNE to ${AutoMez_Spell}.
	} else /if (${Select[${castReturn},CAST_CANNOTSEE]} && ${spamTimer}==0) {
		/if (!${Defined[Retry]}) /declare LOSRetry int local 0
		/docommand ${ChatToggle} **Warning** I cannot see ${Target.CleanName}!!.
		/if (${Defined[spamTimer]}) /varset spamTimer 15s
		/if (${Defined[LOSRetry]}) /varcalc LOSRetry ${LOSRetry}+1
		/if (${LOSRetry} < ${GiveUpValue}) /goto :recast
	} else /if (${Select[${castReturn},CAST_OUTOFMANA]} && ${spamTimer}==0) {
		/docommand ${ChatToggle} **Warning** I do not have enough mana to cast ${MyDebuffs[${i}]}!!.
		/if (${Defined[spamTimer]}) /varset spamTimer 15s
	} else /if (${Select[${castReturn},CAST_OUTOFRANGE]} && ${spamTimer}==0) {
		/docommand ${ChatToggle} **Warning** ${MyDebuffs[${i}]} is out of range for ${MyDebuffs[${i}]}!!.
		/if (${Defined[spamTimer]}) /varset spamTimer 15s
	} else /if (${Select[${castReturn},CAST_SUCCESS]})	{
		/docommand ${ChatToggle} Landed ${AutoMez_Spell} on ${Target.CleanName}!!.
		/if (${Defined[${Target.ID}${Spell[${AutoMez_Spell}].ID}]}) /varset ${Target.ID}${Spell[${AutoMez_Spell}].ID} ${Math.Calc[${Spell[${AutoMez_Spell}].Duration} * 6-6]}s
	}
}
/delay 5s !${Me.Casting.ID}
/return
#event AutoMez_Off "<#*#> End AutoMez"
#event AutoMez_Off "<#*#> AutoMez off"
sub event_AutoMez_Off
/if (${do_AutoMez}) {
	/docommand ${ChatToggle} I will no longer mez targets.
	/varset do_AutoMez FALSE
}
/return
#event AutoMez_On "<#*#> Start AutoMez"
#event AutoMez_On "<#*#> AutoMez On"
sub event_AutoMez_On
/if (!${do_AutoMez}) {
	/docommand ${ChatToggle} Attempting to mez adds.
	/varset do_AutoMez TRUE
}
/return