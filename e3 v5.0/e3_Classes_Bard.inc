|------------------------------------------------------------------------|
|- e3_Classes_Bard.inc v3.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains Bard specific functions related to,							-|
|-		Setup and Ini entry information									-|
|-		MainLoop, and AssistLoop hooks									-|
|-		Over-all functionality / flow									-|
|------------------------------------------------------------------------|



|----------------------------------------------------------------|
|- Bard's Class specific mainLoop hook.							-|
|----------------------------------------------------------------|
|- Prioritizes macro functions based on the bard class dynamic.	-|
|----------------------------------------------------------------|
Sub BRD_Main
/if (${Debug}) /echo |- BRD_Main ==>

	| Check melody
	/if (${playingMelody} && !${Twist.Twisting}) /call restart_Melody
	
	| check_CombatBuffs.
	/if (!${medBreak} && !${idle} && ${CombatBuffs.Size}) /call CombatBuffs
	/if (!${ActionTaken}) {
	
		| Assist Functions.
		/if (${Assisting}) /call ${Me.Class.ShortName}_Assist
	}

/if (${Debug}) /echo <== BRD_Main -|
/return



|--------------------------------------------------------------------|
|- Bard's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the bard class dynamic.	-|
|--------------------------------------------------------------------|
Sub BRD_Assist
/if (${Debug}) /echo |- BRD_Assist ==>

	/call e3_AssistFunctions
	
/if (${Debug}) /echo <== BRD_Assist -|
/return



#event saveMelody "<#1#> SaveMelody #2# #3#"
#event saveMelody "<#1#> Save Melody #2# #3#"
#event saveMelody "[MQ2] SaveMelody #2# #3#"
#event saveMelody "[MQ2] Save Melody #2# #3#"
SUB EVENT_saveMelody(line, ChatSender, melodyName, songsToSave)
/if (${Debug}) /echo |- EVENT_saveMelody ==>

	| If no ChatSender is defined, make one.	
	/if (!${Defined[ChatSender]}) /declare ChatSender string local ${Me.CleanName}

	| If I'm the ChatSender.
	/if (${ChatSender.Equal[${Me.CleanName}]}) {

		/declare i int local

		| Check if the melody already exists, and should not be overwritten.
		/if (${Ini[${Character_Ini},${melodyName} Melody].Length}) {	
			/if (${Debug}) /echo |- EVENT_saveMelody -| Overwriting [${melodyName}] melody...
			/for i 1 to 5
				/if (${Ini[${Character_Ini},${melodyName}_Melody,Song#${i}].Length}) /call WriteToIni "${Character_Ini},${melodyName} Melody,Song#${i}" *PLACEHOLDER 1
			/next i
		}
		
		/echo Saving [${melodyName}] melody...
		
		/varset i 1
	
		:check_nextSong

		| If the gem number is not recognized
		/if (!${Int[${songsToSave.Arg[${i}, ]}]} || ${Int[${songsToSave.Arg[${i}, ]}]} < 1 || ${Int[${songsToSave.Arg[${i}, ]}]} > 9) {
			/if (${songsToSave.Arg[${i}, ].NotEqual[-Overwrite]}) {
				/echo [${songsToSave.Arg[${i}, ]}] is not a valid gem number.  Please use the following syntax when saving melodies, "Save Melody [NAME] [# # # # #] (-Overwrite)".  Ex.  /echo Save Melody Raid 1 2 3 4 5 -Overwrite
			}
		} else {
			/echo Song#${i}=${Me.Gem[${songsToSave.Arg[${i}, ]}]}/gem|${songsToSave.Arg[${i}, ]}
			/call WriteToIni "${Character_Ini},${melodyName} Melody,Song#${i}" "${Me.Gem[${songsToSave.Arg[${i}, ]}]}/gem|${songsToSave.Arg[${i}, ]}" ${If[${Ini[${Character_Ini},${melodyName} Melody,Song#${i}].Length},1,0]}
		}
		/varcalc i ${i} + 1
		/if (${songsToSave.Arg[${i}, ].Length}) /goto :check_nextSong
	}

/if (${Debug}) /echo <== EVENT_saveMelody -|
/return



#EVENT startMelody "<#*#> Melody #1#"
#EVENT startMelody "[MQ2] Melody #1#"
#EVENT startMelody "#*# tells you, 'Melody #1#'"
#EVENT startMelody "#*# tells the group, 'Melody #1#'"
SUB EVENT_startMelody(line, melodyName)
/if (${Debug}) /echo |- EVENT_startMelody ==>

	/if (${Me.Class.ShortName.Equal[BRD]}) {
	
	|--------------------------------------------------------------------------------If I don't have the melodyName listed in my ini.
		/if (!${Ini[${Character_Ini},${melodyName} Melody,Song#1].Length}) {
		
		|----------------------------------------------------------------------------If Stop, end, or pause were used, stop the melody.
			/if (${Select[${melodyName},Stop,End,Pause]}) {
				/varset playingMelody FALSE
				/varset songSet
			
				/docommand ${ChatToggle} Ending melody.
				/twist end
			} else {
				/docommand ${ChatToggle} [${melodyName}] is not a known melody.
			}
			
		} else {
			
			/declare songList string local
			/declare retryTimer timer local 100
			/declare spellGem int local
			/declare spellName string local
			/declare i int local 1
			
			:check_nextSong
			/if (${Ini[${Character_Ini},${melodyName} Melody,Song#${i}].Arg[1,/].NotEqual[PLACEHOLDER]}) {
			
				/varset spellName ${Ini[${Character_Ini},${melodyName} Melody,Song#${i}].Arg[1,/]}
			
				/if (!${Me.Book[${spellName}]}) {	
					/echo I don't have [${spellName}] in my spellbook, playing the melody without it.
				} else {
					
					| If the song is not memmed.
					/if (!${Me.Gem[${spellName}]}) {
						| /Gem| Check.
						/call argueString gem "${Ini[${Character_Ini},${melodyName} Melody,Song#${i}]}"
						/varset spellGem ${Macro.Return}
						
						/if (${Bool[${Me.Gem[${spellGem}]}]}) {
							:waitToForget
							| Forget Spell
							/if (${Debug}) /echo |- cast_Spell -| wait to forget loop
							/notify CastSpellWnd CSPW_Spell${Math.Calc[${spellGem} - 1].Int} rightmouseup
							/delay 2
							/goto :waitToForget
						}
						
						:retry
						/if (${Debug}) /echo |- EVENT_startMelody -| :memloop
						/memspell ${spellGem} "${spellName}"
						/delay 90 ${Me.Gem[${spellName}]}
						/if (!${Me.Gem[${spellName}]} && ${retryTimer}) {
							/goto :retry
						} else /if (${Window[SpellBookWnd].Open}) {
							/stand
						}
					}
					
					| Update songList with the corresponding gem#
					/varset songList ${songList} ${Me.Gem[${spellName}]}
				}
			}
			
			| Check for additional Songs.
			/varcalc i ${i} + 1
			/if (${Ini[${Character_Ini},${melodyName} Melody,Song#${i}].Length}) /goto :check_nextSong
		
			| Twist the newly built songList.
			/varset playingMelody TRUE
			/varset songSet ${songList}
			
			/docommand ${ChatToggle} Playing [${melodyName}] melody...
			/twist ${songList}
		}
	}
	
/if (${Debug}) /echo <== EVENT_startMelody -|
/return



SUB restart_Melody
/if (${Debug}) /echo |- check_Melody ==>

	/twist ${Twist.List}

/if (${Debug}) /echo <== check_Melody -|
/RETURN



Sub BRD_Setup
/if (${Debug}) /echo |- BRD_Setup ==>
	
	/declare returnTwist bool outer 0
	
	/if (!${Defined[playingMelody]}) /declare playingMelody bool global
	/if (!${Defined[songSet]}) /declare songSet string global
	
/if (${Debug}) /echo <== BRD_Setup -|
/return



Sub BRD_Background_Events
/if (${Debug}) /echo |- BRD_Background_Events ==>
	
	/doevents saveMelody
	/doevents startMelody

/if (${Debug}) /echo <== BRD_Background_Events -|
/return



SUB BRD_CharacterSettings
/if (${Debug}) /echo |- BRD_CharacterSettings ==>

/if (${Debug}) /echo <== BRD_CharacterSettings -|
/RETURN



Sub BRD_Aliases
/if (${Debug}) /echo |- BRD_Aliases ==>

	/squelch /alias /savemelody delete
	/alias /SaveMelody /echo Save Melody
	
	/squelch /alias /playmelody delete
	/alias /PlayMelody /bc  Melody

/if (${Debug}) /echo <== BRD_Aliases -|
/return