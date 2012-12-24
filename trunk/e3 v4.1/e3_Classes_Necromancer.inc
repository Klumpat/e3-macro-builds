|--------------------------------------------------------------------------------|
|- e3_Classes_Necromancer.inc v2.4 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ													-|
|--------------------------------------------------------------------------------|
|- Contains necromancer specific functions related to,							-|
|- Setup and Ini entry information												-|
|- MainLoop, and AssistLoop hooks												-|
|- Over-all functionality / flow												-|
|--------------------------------------------------------------------------------|



|------------------------------------------------------------------------|
|- Necromancer's Class specific mainLoop hook.							-|
|------------------------------------------------------------------------|
|- Prioritizes macro functions based on the necromancer class dynamic.	-|
|------------------------------------------------------------------------|
Sub NEC_Main
/if (${Debug}) /echo |- NEC_Main ==>

|-----------------------------------------------------------------------------------------------------------------------------------------#1 Auto-Lich.	
	/if (${autoLich} && !${Me.Buff[${lichSpell}].ID} && ${Me.PctMana} < 90) /call check_Lich
	/if (!${ActionTaken}) {
	
	|--------------------------------------------------------------------------------------------------------------------------------------#2 Twitch Casters.	
		/if (${auto_ManaDump}) /call manaDump
		/if (!${ActionTaken}) {
		
		|--------------------------------------------------------------------------------------------------------------------------------#2 check_CommandDebuffs.
			/if (${Debuff_Targets.Size}) /call Debuffs_OnCommand
			/if (!${ActionTaken}) {
			
			|------------------------------------------------------------------------------------------------------------------------------------------#3 check_DoTs.	
				/if (${DoT_Targets.Size}) /call DoTs_OnCommand
				/if (!${ActionTaken}) {
				
				|-----------------------------------------------------------------------------------------------------------------------------------#4 check_CombatBuffs.
					/if (!${idle} && ${CombatBuffs.Size}) /call CombatBuffs
					/if (!${ActionTaken}) {
					
					|------------------------------------------------------------------------------------------------------------------------------------#5 Assist Functions.
						/if (${Assisting}) /call NEC_Assist
						/if (!${ActionTaken}) {
						
						|--------------------------------------------------------------------------------------------------------------------------------------------#6 Pet heal.
							/if (${Defined[Pet_Heal]} && ${Me.Pet.ID}) /call PetHeal
						}
					}
				}
			}
		}
	}

/if (${Debug}) /echo <== NEC_Main -|	
/return



|------------------------------------------------------------------------|
|- Necromancer's Class specific Assist Loop.							-|
|------------------------------------------------------------------------|
|- Prioritizes Assist functions based on the necromancer class dynamic.	-|
|------------------------------------------------------------------------|
Sub NEC_Assist
/if (${Debug}) /echo |- NEC_Assist ==>

|-----------------------------------------------------------------------#1 Check Asssit status.
	/call check_AssistStatus
	
|--------------------------------------------------------------------------------#2 QuickBurns.
	/if (${use_QBurn}) /call QuickBurns

|---------------------------------------------------------------------------------#3 FullBurns.	
	/if (${use_FBurn}) /call FullBurns
	
|----------------------------------------------------------------------------------#4 Life Tap.
	/if (${Defined[lifeTap]}) /call check_LifeTap
	/if (!${ActionTaken}) {
	
	|----------------------------------------------------------------------------#5 Dots on assist.
		/if (${Assist_DoTs.Size}) /call DoTs_OnAssist
		/if (!${ActionTaken}) {
		
		|-------------------------------------------------------------------------#6 Debuffs on assist.
			/if (${Assist_Debuffs.Size}) /call Debuffs_OnAssist
			/if (!${ActionTaken}) {
			
			|-------------------------------------------------------------------------------------#7 Nukes.		
				/if (${MyNukes.Size} && !${NukeTimer}) /call Nukes
			}
		}
	}

/if (${Debug}) /echo <== NEC_Assist -|	
/return



|----------------------------------------------------------------------------|
|- NEC_Setup																-|
|----------------------------------------------------------------------------|
|- Imports variables from the [Necromancer] section of the Character_Ini.	-|
|----------------------------------------------------------------------------|
Sub NEC_Setup
/if (${Debug}) /echo |- NEC_Setup ==>

	/if (${Ini[${Character_Ini},Necromancer,LifeTap (Spell/Gem/Pct)].Length}) /call INItoVar "${Character_Ini},Necromancer,LifeTap (Spell/Gem/Pct)" lifeTap string outer
	
	/if (${Ini[${Character_Ini},Necromancer,Auto-Lich (On/Off)].Length}) /call INItoVar "${Character_Ini},Necromancer,Auto-Lich (On/Off)" autoLich bool outer
	
	/if (${Ini[${Character_Ini},Necromancer,Allow Twitch requests (On/Off)].Length}) /call INItoVar "${Character_Ini},Necromancer,Allow Twitch requests (On/Off)" allowTwitch bool outer
	/if (${Ini[${Character_Ini},Necromancer,Min Mana to Allow Twitch Requests (Pct)].Length}) /call INItoVar "${Character_Ini},Necromancer,Min Mana to Allow Twitch Requests (Pct)" minTwitchMana int outer
	
	/if (${Ini[${Character_Ini},Necromancer,Auto-ManaDump (On/Off)].Length}) /call INItoVar "${Character_Ini},Necromancer,Auto-ManaDump (On/Off)" auto_ManaDump bool outer
	/if (${Ini[${Character_Ini},Necromancer,Auto-ManaDump Engage (Pct)].Length}) /call INItoVar "${Character_Ini},Necromancer,Auto-ManaDump Engage (Pct)" manaDump_Pct int outer
	/if (${Ini[${Character_Ini},Necromancer,Auto-ManaDump Classes].Length}) /call INItoVar "${Character_Ini},Necromancer,Auto-ManaDump Classes" manaDump_Classes string outer
	/if (${Defined[manaDump_Classes]} && ${manaDump_Classes.Find[/]}) /varset manaDump_Classes ${manaDump_Classes.Replace[/,,]}
	
	/if (${autoLich}) {
		
		/declare lichSpell string outer
		
		/if (${Me.Book[Ancient: Seduction of Chaos]}) {
			/varset lichSpell Ancient: Seduction of Chaos
			/if (${Debug}) /echo |- NEC_Setup -| Auto-Lich: lichSpell=[${lichSpell}].
		} else /if (${Me.Book[Seduction of Saryrn]}) {
			/varset lichSpell Seduction of Saryrn
			/if (${Debug}) /echo |- NEC_Setup -| Auto-Lich: lichSpell=[${lichSpell}].
		} else /if (${Me.Book[Ancient: Master of Death]}) {
			/varset lichSpell Ancient: Master of Death
			/if (${Debug}) /echo |- NEC_Setup -| Auto-Lich: lichSpell=[${lichSpell}].
		} else /if (${Me.Book[Arch Lich]}) {
			/varset lichSpell Arch Lich
			/if (${Debug}) /echo |- NEC_Setup -| Auto-Lich: lichSpell=[${lichSpell}].
		} else /if (${Me.Book[Demi Lich]}) {
			/varset lichSpell Demi Lich
			/if (${Debug}) /echo |- NEC_Setup -| Auto-Lich: lichSpell=[${lichSpell}].
		} else /if (${Me.Book[Lich]}) {
			/varset lichSpell Lich
			/if (${Debug}) /echo |- NEC_Setup -| Auto-Lich: lichSpell=[${lichSpell}].
		} else /if (${Me.Book[Call of Bones]}) {
			/varset lichSpell Call of Bones
			/if (${Debug}) /echo |- NEC_Setup -| Auto-Lich: lichSpell=[${lichSpell}].
		} else /if (${Me.Book[Allure of Death]}) {
			/varset lichSpell Allure of Death
			/if (${Debug}) /echo |- NEC_Setup -| Auto-Lich: lichSpell=[${lichSpell}].
		} else /if (${Me.Book[Dark Pact]}) {
			/varset lichSpell Dark Pact
			/if (${Debug}) /echo |- NEC_Setup -| Auto-Lich: lichSpell=[${lichSpell}].
		}
	}
	
/if (${Debug}) /echo <== NEC_Setup -|
/return



|------------------------------------------------------------------------|
|- MakeNEC_Ini															-|
|------------------------------------------------------------------------|
|- Writes entries to the [Necromancer] section of the Character_Ini.	-|
|------------------------------------------------------------------------|
Sub MakeNEC_Ini
/if (${Debug}) /echo |- NEC_Ini ==>

	/call WriteToIni "${Character_Ini},Necromancer,LifeTap (Spell/Gem/Pct)" "*Lifetap spell to cast, gem to mem it to, and percent to cast it at."
	/call WriteToIni "${Character_Ini},Necromancer,Auto-Lich (On/Off)"
	
	/call WriteToIni "${Character_Ini},Necromancer,Auto-ManaDump (On/Off)"
	/call WriteToIni "${Character_Ini},Necromancer,Auto-ManaDump Engage (Pct)"
	/call WriteToIni "${Character_Ini},Necromancer,Auto-ManaDump Classes"
	
	/call WriteToIni "${Character_Ini},Necromancer,Allow Twitch requests (On/Off)"
	/call WriteToIni "${Character_Ini},Necromancer,Min Mana to Allow Twitch Requests (Pct)"
	
/if (${Debug}) /echo <== NEC_Ini -|
/return

	

|----------------------------------------------------------------------------|
|- NEC_Background_Events													-|
|----------------------------------------------------------------------------|
|- Events to add to the Background_Events, that are necromancer specific.	-|
|----------------------------------------------------------------------------|
Sub NEC_Background_Events
/if (${Debug}) /echo |- NEC_Background_Events ==>

	/if (${autoLich} && ${Me.Buff[${lichSpell}].ID}) {
		/if (${Me.PctHPs} < 20 || ${Me.PctMana} == 100 && !${combatTimer}) /nomodkey /notify BuffWindow Buff${Math.Calc[${Me.Buff[${lichSpell}].ID}-1].Int} leftmouseup	
	}

/if (${Debug}) /echo <== NEC_Background_Events -|
/return



SUB check_LifeTap
/if (${Debug}) /echo |- check_LifeTap ==>

	/declare lifeTap_Spell string local ${lifeTap.Arg[1,/]}
	/declare lifeTap_CastType string local ${lifeTap.Arg[2,/]}
	/declare lifeTap_Pct int local ${lifeTap.Arg[3,/]}

	/if (${Me.PctHPs} < ${lifeTap_Pct}) {
	
	|------------------------------------------------Check range.
	
		/if (${Spawn[${AssistTarget}].Distance} > ${Spell[${lifeTap_Spell}].MyRange}) {
			/if (${Debug}) /echo |- check_LifeTap -| [${Spawn[${AssistTarget}].CleanName}] is too far away to cast [${lifeTap_Spell}].
		} else {
		
		|---------------------------------------------Check mana.
			/if (${Me.CurrentMana} < ${Spell[${lifeTap_Spell}].Mana}) {
				/if (${Debug}) /echo |- check_LifeTap -| I don't have enough mana to cast [${lifeTap_Spell}].
			} else {
			
				/call e3_Cast ${AssistTarget} "${lifeTap_Spell}" ${lifeTap_CastType} ${If[${Me.Gem[${lifeTap_Spell}]},3s,${Math.Calc[${Spell[${lifeTap_Spell}].RecastTime} + 3]}]}

			}
		}
	}

/if (${Debug}) /echo <== check_LifeTap -|
/RETURN
	
	
	
SUB check_Lich
/if (${Debug}) /echo |- check_Lich ==>

|------------------------------------------------------------------If I don't have lich on.
	/if (${Spell[${lichSpell}].Stacks} && !${activeTimer} && !${combatTimer}) /call e3_Cast ${Me.ID} "${lichSpell}" gem${DefaultGem} ${If[${Me.Gem[${lichSpell}]},3s,7s]}
		
/if (${Debug}) /echo <== check_Lich -|
/RETURN	



#EVENT twitchMe "#*# tells you, 'Twitch on #1# #2#'"
#EVENT twitchMe "#*# tells the group, 'Twitch on #1# #2#'"
#EVENT twitchMe "#1# tells you, 'Twitch #2#'"
#EVENT twitchMe "#1# tells the group, 'Twitch #2#'"
SUB EVENT_twitchMe(line, ChatSender, int TwitchNum)
/if (${Me.Class.ShortName.Equal[NEC]} && ${TwitchNum}) {
/if (${Debug}) /echo |- EVENT_twitchMe ==>

	/if (${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}

|------------------------------------------------------------------------------Check for PC in zone.
	/if (!${SpawnCount[pc ${ChatSender}]}) {
		/tell ${ChatSender} We're not in the same zone, dumbass!
	} else {
	
	|--------------------------------------------------------------------------------Check range.
		/if (${Spawn[pc ${ChatSender}].Distance} > 200) {
			/tell ${ChatSender} You're too far away to twitch
		} else {
	
		|--------------------------------------------------------------If I'm not allowed to twitch
			/if (!${allowTwitch}) {
				/tell ${ChatSender} I'm not currently mana dumping others.  Try talking to my master, sucka!
			} else {
			
			|------------------------------------------------------------If I don't have enough mana to twitch
				/if (${Me.PctMana} < ${minTwitchMana} || ${Me.PctMana} <= 5) {
					/tell ${ChatSender} I do not have enough mana to twitch right now.  Try again later, sucka!
				} else {
				
				|--------------------------------------------------------If they ask for too much.
					/if (${TwitchNum} > 20 && !${NetBots[${ChatSender}].ID}) {
						/tell ${ChatSender} Whoa whoa whoa, hang on just a minute.  You want a mana battery, make your own necro.  You can requet twitches a maximum of [20] at a time.  Try again, sucka!
					} else {
					
					|----------------------------------------------------Okay...I guess I'll twitch you.
						/if (${Verbosity}) /docommand ${ChatToggle} Twitching ${Spawn[${InFrom}].CleanName} ${TwitchNum} times.
						/tell ${ChatSender} Twitching you [${TwitchNum}] times.
						
						/declare twitchSpells[2] string local
							/varset twitchSpells[1] Covetous Subversion
							/varset twitchSpells[2] Sedulous Subversion
							
						/declare i int local
						
						/call TrueTarget ${Spawn[pc ${InFrom}].ID}
						
					|-----------------------------------------------------Check for spells to be memmed
						/if (!${Me.Gem[Covetous Subversion]} || !${Me.Gem[Sedulous Subversion]}) {
							
							/declare spellsetMem_Timer timer local 100
							/docommand ${ChatToggle} Loading twitch spells...
							/memspellset TwitchSpells
							:Memloop
							/if (${Debug}) /echo |- EVENT_twitchMe -| :Memloop
							
							/call Background_Events
							/if (${Debug}) /echo spellsetMem_Timer = [${spellsetMem_Timer}]
							/if (${Window[SpellBookWnd].Open} && ${spellsetMem_Timer}) /goto :Memloop
							/if (${Me.Sitting}) /stand
						}
						
						/for i 1 to ${TwitchNum}
							
							/echo Twitching [${Spawn[pc ${ChatSender}].CleanName}] #[${i}] of [${TwitchNum}].
							:tryagain
							/if (${Me.SpellReady[Covetous Subversion]}) {
								/call e3_Cast ${Spawn[pc ${ChatSender}].ID} "Covetous Subversion" gem${Me.Gem[Covetous Subversion]} 3s
							} else /if (${Me.SpellReady[Sedulous Subversion]}) {
								/call e3_Cast ${Spawn[pc ${ChatSender}].ID} "Sedulous Subversion" gem${Me.Gem[Sedulous Subversion]} 3s
							} else /if (${Me.SpellReady[Rapacious Subvention]}) {
								/call e3_Cast ${Spawn[pc ${ChatSender}].ID} "Rapacious Subvention" gem${Me.Gem[Rapacious Subvention]} 3s
							} else {
								/goto :tryagain
							}
							
						/next i
					}
				}
			}
		}
	}
	
/if (${Debug}) /echo <== EVENT_twitchMe -|
}
/RETURN



SUB manaDump
/if (${Debug}) /echo |- manaDump ==>

	/declare i int local
	/declare manaDump_Target string local
	
	/for i 1 to ${NetBots.Counts}
		/if (${Select[${NetBots[${NetBots.Client[${i}]}].Class.ShortName},${manaDump_Classes}]}) {
			/if (${SpawnCount[pc ${NetBots.Client[${i}]}]} && ${Spawn[pc ${NetBots.Client[${i}]}].Distance} < 200) {
				/if (${NetBots[${NetBots.Client[${i}]}].PctMana} < ${NetBots[${manaDump_Target}].PctMana} || !${manaDump_Target.Length}) {
					/varset manaDump_Target ${NetBots.Client[${i}]}
					/if (${Debug}) /echo |- manaDump -| manaDump_Target=${manaDump_Target}
				}
			}	
		}

	/next i

|-----------------------------------------------------Check for spells to be memmed
	/if (!${Me.Gem[Covetous Subversion]} || !${Me.Gem[Sedulous Subversion]}) {
		
		/declare spellsetMem_Timer timer local 100
		/docommand ${ChatToggle} Loading twitch spells...
		/memspellset TwitchSpells
		:Memloop
		/if (${Debug}) /echo |- EVENT_twitchMe -| :Memloop
		
		/call Background_Events
		/if (${Debug}) /echo spellsetMem_Timer = [${spellsetMem_Timer}]
		/if (${Window[SpellBookWnd].Open} && ${spellsetMem_Timer}) /goto :Memloop
		/if (${Me.Sitting}) /stand
	}
	
	/if (${NetBots[${manaDump_Target}].PctMana} < ${manaDump_Pct} && ${Me.PctMana} > 5) {

		/if (${Me.SpellReady[Covetous Subversion]}) {
			/if (${Debug}) /echo |- manaDump -| Casting [Covetous Subversion] on [${manaDump_Target}]}]
			/call e3_Cast ${NetBots[${manaDump_Target}].ID} "Covetous Subversion" gem${Me.Gem[Covetous Subversion]} 3s
		} else /if (${Me.SpellReady[Sedulous Subversion]}) {
			/if (${Debug}) /echo |- manaDump -| Casting [Sedulous Subversion] on [${manaDump_Target}]]
			/call e3_Cast ${NetBots[${manaDump_Target}].ID} "Sedulous Subversion" gem${Me.Gem[Sedulous Subversion]} 3s
		} else /if (${Me.SpellReady[Rapacious Subvention]}) {
			/if (${Debug}) /echo |- manaDump -| Casting [Rapacious Subvention] on [${manaDump_Target}]
			/call e3_Cast ${NetBots[${manaDump_Target}].ID} "Rapacious Subvention" gem${Me.Gem[Rapacious Subvention]} 3s
		}
	}
	
/if (${Debug}) /echo <== manaDump -|
/RETURN



#EVENT start_manaDump "<#*#> Mana dump #1#"
SUB EVENT_start_ManaDump(line, DumpClasses, int DumpPct)
/if (${Me.Class.ShortName.Equal[NEC]}) {
/if (${Debug}) /echo |- EVENT_start_ManaDump ==>

|------------------------------------------------------------------------------Reset DumpClasses.
	/if (${DumpClasses.Find[healer]} || ${DumpClasses.Find[priest]}) {
		/varset DumpClasses CLR,DRU,SHM
	} else /if (${DumpClasses.Find[caster]}) {
		/varset DumpClasses ENC,MAG,WIZ
	}

|-------------------------------------------------------------------------------If I was already manaDumping.
	/if (${Defined[auto_ManaDump]}) {
	
		/varset auto_ManaDump TRUE
		/if (${Defined[DumpPct]}) /varset manaDump_Pct ${DumpPct}
		/if (!${Select[${DumpClasses},on,start,engage]}) /varset manaDump_Classes ${DumpClasses}
	
|-------------------------------------------------------------------------------If I was not already manaDumping.
	} else {

		/declare auto_ManaDump bool outer TRUE
		/declare manaDump_Pct int outer ${If[${DumpPct},${DumpPct},95]}
		/declare manaDump_Classes string outer ${DumpClasses}

	}
	
	/docommand ${ChatToggle} Mana dumping [${manaDump_Classes}] who fall below [${manaDump_Pct}m].
	
/if (${Debug}) /echo <== EVENT_start_ManaDump -|	
}
/RETURN



#EVENT end_manaDump "<#*#> Mana dump off"
#EVENT end_manaDump "<#*#> End mana dump"
SUB EVENT_end_ManaDump
/if (${Me.Class.ShortName.Equal[NEC]}) {
/if (${Debug}) /echo |- EVENT_end_ManaDump ==>

	/if (${Defined[auto_ManaDump]}) {
	
		/varset auto_ManaDump FALSE
		/docommand ${ChatToggle} No longer mana dumping [${manaDump_Classes}].

	}

/if (${Debug}) /echo <== EVENT_end_ManaDump -|	
}
/return