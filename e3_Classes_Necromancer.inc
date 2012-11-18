|--------------------------------------------------------------------------------|
|- e3_Classes_Necromancer.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ													-|
|--------------------------------------------------------------------------------|
|- Contains necromancer specific functions related to,							-|
|-		Setup and Ini entry information											-|
|-		MainLoop, and AssistLoop hooks											-|
|-		Over-all functionality / flow											-|
|--------------------------------------------------------------------------------|



|------------------------------------------------------------------------|
|- Necromancer's Class specific mainLoop hook.							-|
|------------------------------------------------------------------------|
|- Prioritizes macro functions based on the necromancer class dynamic.	-|
|------------------------------------------------------------------------|
Sub NEC_Main
/if (${Debug}) /echo |- NEC_Main ==>

|--------------------------------------------------------------------------------------------------------------------------------#1 check_CommandDebuffs.
	/if (${Debuff_Targets.Size}) /call Debuffs_OnCommand
		/if (${ActionTaken}) /return
	
|--------------------------------------------------------------------------------------------------------------------------------------#2 Twitch Casters.	
	/if (${ManaBattery}) /call TwitchCasters

|------------------------------------------------------------------------------------------------------------------------------------------#3 check_DoTs.	
	/if (${DoT_Targets.Size}) /call DoTs_OnCommand
		/if (${ActionTaken}) /return
		
		
|-----------------------------------------------------------------------------------------------------------------------------------#4 check_CombatBuffs.
	/if (!${idle} && ${PriorityBuffs.Size}) /call checkPriorityBuffs
		/if (${ActionTaken}) /return
		
		
|------------------------------------------------------------------------------------------------------------------------------------#5 Assist Functions.
	/if (${Assisting}) /call NEC_Assist
		/if (${ActionTaken}) /return
		
|--------------------------------------------------------------------------------------------------------------------------------------------#6 Pet heal.
	/if (${Defined[Pet_Heal]} && ${Me.Pet.ID}) /call PetHeal
		/if (${ActionTaken}) /return
	
|-----------------------------------------------------------------------------------------------------------------------------------------#7 Remove lich.	
	/if (${Me.Buff[Ancient: Seduction of Chaos].ID} && ${Me.PctHPs} < 30) /nomodkey /notify BuffWindow Buff${Math.Calc[${Me.Buff[Seduction of Saryrn].ID}-1].Int} leftmouseup

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
	/if (${Me.PctHPs} < ${LifeTapPct} && ${Me.CurrentMana} > ${Spell[${LifeTapSpell}].Mana} && ${Spawn[${AssistTarget}].Distance} < ${Spell[${LifeTapSpell}].MyRange}) {
		/call e3_Cast ${AssistTarget} "${LifeTapSpell}" Gem${LifeTapGem} ${Spell[${LifeTapSpell}].RecastTime}
	}
	
|----------------------------------------------------------------------------#5 Dots on assist.
	/if (${Assist_DoTs.Size}) /call DoTs_OnAssist
		/if (${ActionTaken}) /return
		
|-------------------------------------------------------------------------#6 Debuffs on assist.
	/if (${Assist_Debuffs.Size}) /call Debuffs_OnAssist
		/if (${ActionTaken}) /return
		
|-------------------------------------------------------------------------------------#7 Nukes.		
	/if (${MyNukes.Size} && !${NukeTimer}) /call Nukes

/if (${Debug}) /echo <== NEC_Assist -|	
/return



|----------------------------------------------------------------------------|
|- NEC_Setup																-|
|----------------------------------------------------------------------------|
|- Imports variables from the [Necromancer] section of the Character_Ini.	-|
|----------------------------------------------------------------------------|
Sub NEC_Setup
/if (${Debug}) /echo |- NEC_Setup ==>

	/if (!${Ini[${Character_Ini},Necromancer].Length}) /call MakeNEC_Ini
	/if (${Ini[${Character_Ini},Necromancer,LifeTap(Spell/Gem/Pct)].Length}) {
		/call INItoVar ${Character_Ini},Necromancer,LifeTap(Spell/Gem/Pct) UNDEFINED string local
		/declare LifeTapSpell string outer ${Macro.Return.Arg[1,/]}
		/declare LifeTapPct int outer ${Macro.Return.Arg[2,/]}
		/declare LifeTapGem int outer ${Macro.Return.Arg[3,/]}
	}

	/declare ManaBattery bool outer FALSE
	/declare TwitchClasses string outer UNDEFINED
	/declare twitch_Pct int outer 95
	
/if (${Debug}) /echo <== NEC_Setup -|
/return



|------------------------------------------------------------------------|
|- MakeNEC_Ini															-|
|------------------------------------------------------------------------|
|- Writes entries to the [Necromancer] section of the Character_Ini.	-|
|------------------------------------------------------------------------|
Sub MakeNEC_Ini
/if (${Debug}) /echo |- NEC_Ini ==>

	/call WriteToIni ${Character_Ini},Necromancer,LifeTap(Spell/Gem/Pct) "*Lifetap spell to cast, gem to mem it to, and percent to cast it at."
	/call WriteToIni ${Character_Ini},Necromancer,TwitchEx1 "*Set TwitchOthers On to allow other characters to request mana dumps."
	/call WriteToIni ${Character_Ini},Necromancer,TwitchEx2 "*You may also specify a minimum percent mana before casting on others by adding it after the On setting."
	/call WriteToIni ${Character_Ini},Necromancer,TwitchEx3 "*Ex. TwitchOthers=On/35"
	/call WriteToIni ${Character_Ini},Necromancer,TwitchOthers(On/Off) On/35
	/call WriteToIni ${Character_Ini},Necromancer,Twitch#1 "*List your twitch spells/gem to mem them to.  Ex Twitch#1=Sedulous Subersion/gem6"
	
/if (${Debug}) /echo <== NEC_Ini -|
/return



|----------------------------------------------------------------------------|
|- NEC_Background_Events													-|
|----------------------------------------------------------------------------|
|- Events to add to the Background_Events, that are necromancer specific.	-|
|----------------------------------------------------------------------------|
Sub NEC_Background_Events

/return

#Event Twitch "#*#, 'Twitchon #1# #2#'"
#Event Twitch "#1# tells you, 'Twitch #2#'"
Sub Event_Twitch(string Line, string InFrom, int TwitchNum)
/if (${InFrom.Left[1].Compare[ ]}<0) /varset InFrom ${InFrom.Right[-2].Left[-1]}
| This takes On / off and % out of the ini
/call INItoVar ${Character_Ini},Necromancer,TwitchOthers(On/Off) UNDEFINED string local
/if (${Select[${Macro.Return.Arg[1,/]},Off,off,0]}) {
	/tell ${InFrom} I'm not currently mana dumping others.
	/return
} else /if (${Select[${Macro.Return.Arg[1,/]},On,on,1]}) {
	/if (${TwitchNum} >= 35) { 
		/reply Hell no.  Go make your own necro, and leech all of his fucking mana.
		/return
	}
	/if (${Macro.Return.Arg[2,/].Length}) {
		/declare MinMana int local ${Macro.Return.Arg[2,/]}
	} else /if (!${Macro.Return.Arg[2,/].Length}) {
		/declare MinMana int local 5
	}
	/if (${Me.PctMana} < ${MinMana}) {
		/reply I'm low on mana. Try again in a few minutes.
		/return
	}
	| I use this to pull the twitch spells out of the ini and creates an array |
	/call INItoArray ${Character_Ini},Necromancer,Twitch# TwitchSpells
	/if (${Verbosity}) /docommand ${ChatToggle} Twitching ${Spawn[${InFrom}].CleanName} ${TwitchNum} times.
	/reply Twitching ${InFrom} ${TwitchNum} times.
	/call TrueTarget ${Spawn[pc ${InFrom}].ID}
	/declare i int local
	/for i 1 to ${TwitchSpells.Size}
		/if (!${Me.Gem[${TwitchSpells[${i}].Arg[2,/]}].Name.Equal[${TwitchSpells[${i}].Arg[1,/]}]}) /memspell ${TwitchSpells[${i}].Arg[2,/]} "${TwitchSpells[${i}].Arg[1,/]}"
		/delay 10s !${Window[SpellBookWnd].Open}
	/next i
    /for i 1 to ${TwitchNum}
        :tryagain
		/if (${Me.SpellReady[${TwitchSpells[1].Arg[1,/]}]} || ${Me.SpellReady[${TwitchSpells[2].Arg[1,/]}]} || ${Me.SpellReady[${TwitchSpells[3].Arg[1,/]}]} || ${Me.SpellReady[${TwitchSpells[4].Arg[1,/]}]} || ${Me.SpellReady[${TwitchSpells[5].Arg[1,/]}]}) {
            /if (${Me.SpellReady[${TwitchSpells[1].Arg[1,/]}]}) {
				/docommand ${ChatToggle} Twitching ${Target.CleanName} Number : ${i}.
				/call e3_Cast ${Spawn[pc ${InFrom}].ID} "${TwitchSpells[1].Arg[1,/]}" ${TwitchSpells[1].Arg[2,/]} ${Spell[${TwitchSpells[1].Arg[1,/]}].RecastTime}s
            } else /if (${Me.SpellReady[${TwitchSpells[2].Arg[1,/]}]}) {
				/docommand ${ChatToggle} Twitching ${Target.CleanName} Number : ${i}.
				/call e3_Cast ${Spawn[pc ${InFrom}].ID} "${TwitchSpells[2].Arg[1,/]}" ${TwitchSpells[1].Arg[2,/]} ${Spell[${TwitchSpells[2].Arg[1,/]}].RecastTime}s
            } else /if (${Me.SpellReady[${TwitchSpells[3].Arg[1,/]}]}) {
				/docommand ${ChatToggle} Twitching ${Target.CleanName} Number : ${i}.
				/call e3_Cast ${Spawn[pc ${InFrom}].ID} "${TwitchSpells[3].Arg[1,/]}" ${TwitchSpells[1].Arg[2,/]} ${Spell[${TwitchSpells[3].Arg[1,/]}].RecastTime}s
            }
        } else {
			/goto :tryagain
		}
	/next i
}
/return
#event ManaDump "<#*#> Mana dump #1#"
sub event_ManaDump(string line, string DumpClasses)
/if (${Me.Class.ShortName.Equal[NEC]}) {
	/if (${Defined[ManaBattery]}) /varset ManaBattery TRUE
	/if (${Select[${DumpClasses},Healers,Priests,Healer,Priest]}) {
		/docommand ${ChatToggle} Mana dumping CLR,DRU,SHM
		/varset TwitchClasses CLR,DRU,SHM
	} else /if (${Select[${DumpClasses},Casters,Caster]}) {
		/docommand ${ChatToggle} Mana dumping ENC,WIZ,MAG
		/varset TwitchClasses ENC,WIZ,MAG
	} else /if (!${Select[${DumpClasses},Casters,Caster,Healers,Priests,Healer,Priest]}) {
		/docommand ${ChatToggle} Mana dumping ${DumpClasses}
		/varset TwitchClasses ${DumpClasses}
	}
}
/return
#event End_ManaDump "<#*#> End mana dump"
sub event_End_ManaDump
/if (${ManaBattery}) {
	/docommand ${ChatToggle} Ending mana dump on ${TwitchClasses}
	/if (${Defined[ManaBattery]}) /varset ManaBattery FALSE
	/if (${Defined[TwitchClasses]}) /varset TwitchClasses UNDEFINED
}
/return
sub TwitchCasters
/declare CheckClient int local 1
/declare twitchTarget string local NOTARGET
:ClientCount
/if (${NetBots.Client.Arg[${CheckClient}, ].Length}) {
	/if (${Select[${Spawn[${NetBots.Client.Arg[${CheckClient}, ]}].Class.ShortName},${TwitchClasses}]} && ${NetBots[${NetBots.Client.Arg[${CheckClient}, ]}].Zone} == ${Zone.ID}  && ${Spawn[${NetBots.Client.Arg[${CheckClient}, ]}].Type.NotEqual[Corpse]}) {
		/if (${NetBots[${NetBots.Client.Arg[${CheckClient}, ]}].PctMana} < ${NetBots[${twitchTarget}].PctMana} || ${Select[${twitchTarget},NOTARGET]}) /varset twitchTarget ${NetBots[${NetBots.Client.Arg[${CheckClient}, ]}].Name}
	}
	/varcalc CheckClient ${CheckClient}+1
	/goto :ClientCount
}
/if (!${Select[${twitchTarget},NOTARGET]} && ${NetBots[${twitchTarget}].PctMana} < 95 && ${NetBots[${twitchTarget}].PctMana} > 0) {
	/if (${NetBots[${healTarget}].PctMana} <= ${twitch_Pct} && ${Me.PctMana} > 5) {
		/if (${Me.SpellReady[Sedulous Subversion]} || ${Me.SpellReady[Covetous Subversion]}) {
            /if (${Me.SpellReady[Sedulous Subversion]}) {
				/call e3_Cast ${Spawn[${twitchTarget}].ID} "Sedulous Subversion" Gem6
            } else /if (${Me.SpellReady[Covetous Subversion]}) {
				/call e3_Cast ${Spawn[${twitchTarget}].ID} "Covetous Subversion" Gem7
            }
        }
	} else /if (${Me.PctMana} > 5) {
		/if (${Defined[ManaBattery]}) /varset ManaBattery FALSE
		/if (${Defined[TwitchClasses]}) /varset TwitchClasses UNDEFINED
	}
}
/return