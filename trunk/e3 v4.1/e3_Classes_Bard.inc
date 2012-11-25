|------------------------------------------------------------------------|
|- e3_Classes_Bard.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains bard specific functions related to,							-|
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

|-----------------------------------------------------------------------------------------------------------------------------------#1 check_CombatBuffs.
	/if (!${idle} && ${PriorityBuffs.Size}) /call checkPriorityBuffs
		/if (${ActionTaken}) /return

|------------------------------------------------------------------------------------------------------------------------------------#2 Assist Functions.
	/if (${Assisting}) /call BRD_Assist
		/if (${ActionTaken}) /return
		
|-----------------------------------------------------------------------------------------------------------------------------------------#3 Auto Forage.	
	/if (${autoForage}) /call AutoForage
		/if (${ActionTaken}) /return
	
/if (${Debug}) /echo <== BRD_Main -|
/return



|--------------------------------------------------------------------|
|- Bard's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the bard class dynamic.	-|
|--------------------------------------------------------------------|
Sub BRD_Assist
/if (${Debug}) /echo |- BRD_Assist ==>

|-----------------------------------------------------------------------#1 Check Asssit status.
	/call check_AssistStatus
	
|--------------------------------------------------------------------------------#2 QuickBurns.
	/if (${use_QBurn}) /call QuickBurns
	
|---------------------------------------------------------------------------------#3 FullBurns.	
	/if (${use_FBurn}) /call FullBurns
	
|--------------------------------------------------------------------------#4 Combat Abilities.
/if (${MyAbilities.Size}) /call CombatAbilities

|-------------------------------------------------------------------------------------#5 Nukes.		
	/if (${MyNukes.Size} && !${NukeTimer}) /call Nukes
	
/if (${Debug}) /echo <== BRD_Assist -|
/return



|------------------------------------------------------------------------|
|- BRD_Setup															-|
|------------------------------------------------------------------------|
|- Imports variables from the [Bard] section of the Character_Ini.	-|
|------------------------------------------------------------------------|
Sub BRD_Setup
/varset Debug 1
/if (${Debug}) /echo |- BRD_Setup ==>

	/if (!${Ini[${Character_Ini},${Me.Class}].Length}) /call Make${Me.Class.ShortName}_Ini
	/declare returnTwist bool outer 0
	
|--------------Convert melody syntax from v4.1 (Song Name,#) to v4.2 (Song Name/gem#)
	/declare i int local 1
	/declare e int local
	
	:check_IniEntries
	/if (${Ini[${Character_Ini}].Arg[${i},|].Length}) {
		/if (${Ini[${Character_Ini}].Arg[${i},|].Find[_Melody]}) {
			/varset e 1
			:checkSongs
			/if (${Ini[${Character_Ini},${Ini[${Character_Ini}].Arg[${i},|]},Song#${e}].Length}) {
				/if (${Ini[${Character_Ini},${Ini[${Character_Ini}].Arg[${i},|]},Song#${e}].Find[,]}) {
					/echo |- BRD_Setup -| Invalid melody syntax detected; attempting to auto-correct...
					/call WriteToIni "${Character_Ini},${Ini[${Character_Ini}].Arg[${i},|]},Song#${e}" "${Ini[${Character_Ini},${Ini[${Character_Ini}].Arg[${i},|]},Song#${e}].Arg[1,,]}/gem${Ini[${Character_Ini},${Ini[${Character_Ini}].Arg[${i},|]},Song#${e}].Arg[2,,]}" 1
				}
				/varcalc e ${e} + 1
				/goto :checkSongs
			}
		}
		/varcalc i ${i} + 1
		/goto :check_IniEntries
	}

/if (${Debug}) /echo <== BRD_Setup -|
/varset Debug 0
/return



|----------------------------------------------------------------|
|- MakeBRD_Ini													-|
|----------------------------------------------------------------|
|- Writes entries to the [Bard] section of the Character_Ini.	-|
|----------------------------------------------------------------|
Sub MakeBRD_Ini
/if (${Debug}) /echo |- BRD_Ini ==>

	/call WriteToIni ${Character_Ini},Bard,AutoForage(On/Off) "*Do you want this character to automatically forage?" 0

/if (${Debug}) /echo <== BRD_Ini -|
/return



|--------------------------------------------------------------------|
|- BRD_Background_Events											-|
|--------------------------------------------------------------------|
|- Events to add to the Background_Events, that are bard specific.	-|
|--------------------------------------------------------------------|
Sub BRD_Background_Events
/doevents save_Melody
/doevents Set_Melody
/return


#event saveMelody "<#1#> SaveMelody #2# #3#"
#event saveMelody "<#1#> Save Melody #2# #3#"
#event saveMelody "[MQ2] SaveMelody #2# #3#"
#event saveMelody "[MQ2] Save Melody #2# #3#"
SUB EVENT_saveMelody(line, ChatSender, melodyName, songsToSave)
| /varset Debug 1
/if (${Debug}) /echo |- EVENT_saveMelody ==>

|-----------------------------------------------------------------------------------------------If no ChatSender is defined, make one.	
	/if (!${Defined[ChatSender]}) /declare ChatSender string local ${Me.CleanName}

|---------------------------------------------------------------------------------------------------------If I'm the ChatSender.
	/if (${ChatSender.Equal[${Me.CleanName}]}) {
	
	|----------------------------------------------------------------------------------------------------Check if the melody already exists, and should not be overwritten.
		/if (${Ini[${Character_Ini},${melodyName}_Melody].Length} && !${line.Find[-Overwrite]}) {
			/echo You already have a melody saved as [${melodyName}].  Choose a new name, or, use the -overwrite command to overwrite the existing melody.  Ex.  Save Melody Raid 1 2 3 4 5 -Overwrite
		} else {
			
			/declare i int local
			
			/if (${line.Find[-overwrite]}) {
				/if (${Debug}) /echo |- EVENT_saveMelody -| Overwriting [${melodyName}] melody...
				/for i 1 to 5
					/if (${Ini[${Character_Ini},${melodyName}_Melody,Song#${i}].Length}) /call WriteToIni ${Character_Ini},${melodyName}_Melody,Song#${i} *PLACEHOLDER 1
				/next i
			}
			
			/echo Saving [${melodyName}] melody...
			
			/varset i 1
			
			:check_nextSong
			
		|----------------------------------------------------------------------------------------------------------If the gem number is not recognized
			/if (!${Int[${songsToSave.Arg[${i}, ]}]} || ${Int[${songsToSave.Arg[${i}, ]}]} < 1 || ${Int[${songsToSave.Arg[${i}, ]}]} > 9) {
				
				/if (${songsToSave.Arg[${i}, ].NotEqual[-Overwrite]}) {
					/echo [${songsToSave.Arg[${i}, ]}] is not a valid gem number.  Please use the following syntax when saving melodies, "Save Melody [NAME] [# # # # #] (-Overwrite)".  Ex.  /echo Save Melody Raid 1 2 3 4 5 -Overwrite
				}
			} else {
				
				/echo Song#${i}=${Me.Gem[${songsToSave.Arg[${i}, ]}]}/gem${songsToSave.Arg[${i}, ]}
				/call WriteToIni "${Character_Ini},${melodyName}_Melody,Song#${i}" "${Me.Gem[${songsToSave.Arg[${i}, ]}]}/gem${songsToSave.Arg[${i}, ]}" ${If[${Ini[${Character_Ini},${melodyName}_Melody,Song#${i}].Length},1,0]}
				
			}
			/varcalc i ${i} + 1
			/if (${songsToSave.Arg[${i}, ].Length}) /goto :check_nextSong
		}
	}

/if (${Debug}) /echo <== EVENT_saveMelody -|
| /varset Debug 0
/return



#EVENT startMelody "<#*#> Melody #1#"
#EVENT startMelody "[MQ2] Melody #1#"
#EVENT startMelody "#*# tells you, 'Melody #1#'"
#EVENT startMelody "#*# tells the group, 'Melody #1#'"
SUB EVENT_startMelody(line, melodyName)
| /varset Debug 1
/if (${Debug}) /echo |- EVENT_startMelody ==>

	/if (${Me.Class.ShortName.Equal[BRD]}) {
	
	|--------------------------------------------------------------------------------If I don't have the melodyName listed in my ini.
		/if (!${Ini[${Character_Ini},${melodyName}_Melody,Song#1].Length}) {
		
		|----------------------------------------------------------------------------If Stop, end, or pause were used, stop the melody.
			/if (${Select[${melodyName},Stop,End,Pause]}) { 
				/docommand ${ChatToggle} Ending melody.
				/twist end
			} else {
				/docommand ${ChatToggle} [${melodyName}] is not a known melody.
			}
			
		} else {
			
			/declare songList string local
			/declare i int local 1
			/declare retryTimer timer local 300
			
			:check_nextSong
			/if (${Ini[${Character_Ini},${melodyName}_Melody,Song#${i}].Arg[1,/].NotEqual[PLACEHOLDER]}) {
			
			|-------------------------------------------------------------------------------------------------------If the song is not memmed.
				/if (!${Me.Gem[${Ini[${Character_Ini},${melodyName}_Melody,Song#${i}].Arg[1,/]}]}) {
					:retry
					
					/if (${Debug}) /echo |- EVENT_startMelody -| :memloop
					/memspell ${Ini[${Character_Ini},${melodyName}_Melody,Song#${i}].Arg[2,/].Right[1]} "${Ini[${Character_Ini},${melodyName}_Melody,Song#${i}].Arg[1,/]}"
					/delay 90 ${Me.Gem[${Ini[${Character_Ini},${melodyName}_Melody,Song#${i}].Arg[1,/]}]}
					/if (!${Me.Gem[${Ini[${Character_Ini},${melodyName}_Melody,Song#${i}].Arg[1,/]}]} && ${retryTimer}) {
						/goto :retry
					} else /if (${Window[SpellBookWnd].Open}) {
						/stand
					}
				}
			
			|--------------------------------------------------------------------------------------------------Update songList with the corresponding gem#
				/varset songList ${songList} ${Me.Gem[${Ini[${Character_Ini},${melodyName}_Melody,Song#${i}].Arg[1,/]}]}
			}
			
		|-----------------------------------------------------------------------------------------------------Check for additional Songs.
			/varcalc i ${i} + 1
			/if (${Ini[${Character_Ini},${melodyName}_Melody,Song#${i}].Length}) /goto :check_nextSong
		
		|-------------------------------------------------------------------------------------------------------------Twist the newly built songList.
			/docommand ${ChatToggle} Playing [${melodyName}] melody...
			/twist ${songList}
			
		}
	}
	
/if (${Debug}) /echo <== EVENT_startMelody -|
| /varset Debug 0
/return
