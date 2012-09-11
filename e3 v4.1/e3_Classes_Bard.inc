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

|** Unused at this time. MQ2Twist handles restarting of broken melodies.
/if (${MelodyStatus} && !${Me.Song[${SongName2}].ID} && !${Me.Song[${SongName3}].ID} && !${Me.Song[${SongName4}].ID}) /varset MelodyBroke TRUE
/if (${Assisting}) /call Assist_Functions
/if (${MelodyStatus} && !${Me.Casting.ID} ) {
	/delay 5s ${Me.Casting.ID}
	/if (!${Me.Casting.ID}) {
		/echo /twist ${If[${Song1},${Song1},]} ${If[${Song2},${Song2},]} ${If[${Song3},${Song3},]} ${If[${Song4},${Song4},]} ${If[${Song5},${Song5},]}
		/twist ${If[${Song1},${Song1},]} ${If[${Song2},${Song2},]} ${If[${Song3},${Song3},]} ${If[${Song4},${Song4},]} ${If[${Song5},${Song5},]}
	}
}
**|

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
/if (${Debug}) /echo |- BRD_Setup ==>

	/if (!${Ini[${Character_Ini},${Me.Class}].Length}) /call Make${Me.Class.ShortName}_Ini
	
	/if (!${Defined[MelodyStatus]}) /declare MelodyStatus	bool 	global	FALSE
	/if (!${Defined[Song1]})	/declare	Song1	int global
	/if (!${Defined[Song2]})	/declare	Song2 	int	global
	/if (!${Defined[Song3]}) /declare	Song3 	int global
	/if (!${Defined[Song4]}) /declare	Song4 	int global
	/if (!${Defined[Song5]}) /declare	Song5 	int global
	/declare MelodyBroke		bool 	outer 	FALSE

/if (${Debug}) /echo <== BRD_Setup -|
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



|-------------------------------------------------------------------------------------------------------------------------------------Bard Scripts
#event save_Melody "<#*#> SaveMelody #1# #2#"
#event save_Melody "[MQ2] SaveMelody #1# #2#"
#event save_Melody "<#*#> SaveMelody #1# #2# #3#"
#event save_Melody "[MQ2] SaveMelody #1# #2# #3#"
#event save_Melody "<#*#> SaveMelody #1# #2# #3# #4#"
#event save_Melody "[MQ2] SaveMelody #1# #2# #3# #4#"
#event save_Melody "<#*#> SaveMelody #1# #2# #3# #4# #5#"
#event save_Melody "[MQ2] SaveMelody #1# #2# #3# #4# #5#"
#event save_Melody "<#*#> SaveMelody #1# #2# #3# #4# #5# #6#"
#event save_Melody "[MQ2] SaveMelody #1# #2# #3# #4# #5# #6#"
#event save_Melody "<#*#> Save Melody #1# #2#"
#event save_Melody "[MQ2] Save Melody #1# #2#"
#event save_Melody "<#*#> Save Melody #1# #2# #3#"
#event save_Melody "[MQ2] Save Melody #1# #2# #3#"
#event save_Melody "<#*#> Save Melody #1# #2# #3# #4#"
#event save_Melody "[MQ2] Save Melody #1# #2# #3# #4#"
#event save_Melody "<#*#> Save Melody #1# #2# #3# #4# #5#"
#event save_Melody "[MQ2] Save Melody #1# #2# #3# #4# #5#"
#event save_Melody "<#*#> Save Melody #1# #2# #3# #4# #5# #6#"
#event save_Melody "[MQ2] Save Melody #1# #2# #3# #4# #5# #6#"

sub event_save_Melody(string line, string NameToSave, int S1, int S2, int S3, int S4, int S5)
/if (${Me.Class.ShortName.Equal[BRD]}) {
	/if (${Ini[${Character_Ini},${NameToSave}_Melody].Length}) {
		/echo setMelody Error: ${NameToSave} is already in use.  Please choose another name and try again.
	} else /if (!${Ini[${Character_Ini},${NameToSave}_Melody].Length} || ${line.Right[9].Equal[overwrite]}) {
		/declare i int local
		/declare x int local
		/if (${line.Right[9].Equal[overwrite]}) {
			/echo Clearing ${NameToSave} melody...
			/for i 1 to 5
				/ini ${Character_Ini} ${NameToSave}_Melody "Song#${i}" NULL
			/next i
		}
		/echo Saving ${NameToSave} melody...
		/for i 1 to 5
		/if (${Defined[S${i}]} && !${Select[${Me.Gem[${S${i}}]},NULL]} && ${S${i}} > 0 && ${S${i}} < 10) {
			/echo Song#${i}= ${Me.Gem[${S${i}}]},gem${S${i}}
			/ini ${Character_Ini} ${NameToSave}_Melody "Song#${i}" "${Me.Gem[${S${i}}]},gem${S${i}}"
		/next i
		} else /if (${Defined[S${i}]} && ${S${i}} <= 0 || ${Defined[S${i}]} && ${S${i}} >= 10) {
			/echo setMelody Error: Gem${S${i}} is out of bounds
			/next i
		}
	}
}
/doevents flush
/return

#event Set_Melody "<#*#> Melody #1#"
#event Set_Melody "#*# tells you, 'Melody #1#'"
#event Set_Melody "#*# tells the group, 'Melody #1#'"

sub Event_Set_Melody(string line, string MelodySet)
/if (${Me.Class.ShortName.Equal[BRD]}) {
	/declare i int local
	/declare x int local
	/if (${Ini[${Character_Ini},${MelodySet}_Melody].Length}) {
		/for i 1 to 5
			/varset Song${i} 0
			/if (${Ini[${Character_Ini},${MelodySet}_Melody,Song#${i}].Length}) {
				/if (${Me.Gem[${Ini[${Character_Ini},${MelodySet}_Melody,Song#${i}].Arg[1,,]}]}) {
					/varset Song${i} ${Ini[${Character_Ini},${MelodySet}_Melody,Song#${i}].Arg[2,,]}
				} else {
					/varset x 1
					:retry
					/memspell ${Ini[${Character_Ini},${MelodySet}_Melody,Song#${i}].Arg[2,,].Right[1]} "${Ini[${Character_Ini},${MelodySet}_Melody,Song#${i}].Arg[1,,]}"
					/delay 5s ${Me.Gem[${Ini[${Character_Ini},${MelodySet}_Melody,Song#${i}].Arg[1,,]}]}
					/if (${Window[SpellBookWnd].Open}) {
						/stand
						/if (${x} < 5) {
							/varcalc x ${x} + 1
							/goto :retry
						}
					}
					/varset Song${i} ${Ini[${Character_Ini},${MelodySet}_Melody,Song#${i}].Arg[2,,].Right[1]}
				}
			}
		/next i
		/varset MelodyStatus TRUE
		/twist ${If[${Song1},${Song1},]} ${If[${Song2},${Song2},]} ${If[${Song3},${Song3},]} ${If[${Song4},${Song4},]} ${If[${Song5},${Song5},]}
		/docommand ${ChatToggle} Playing ${MelodySet} melody...
	} else /if (!${Ini[${Character_Ini},${MelodySet}_Melody].Length} && ${MelodySet.Equal[Stop]} || !${Ini[${Character_Ini},${MelodySet}_Melody].Length} && ${MelodySet.Equal[End]}) {
		/docommand ${ChatToggle} Ending melody.
		/twist end
		/varset MelodyStatus FALSE
		/for i 1 to 5
			/varset Song${i} 0
		/next i
	} else {
		/docommand ${ChatToggle} ${MelodySet} is not a known melody.
	}
}
/return