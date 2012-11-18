|--------------------------------------------------------------------|
|- e3_Macro_Setup.inc ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ										-|
|--------------------------------------------------------------------|
|- Contains setup functions for e3.mac								-|
|--------------------------------------------------------------------|


|----------------------------------------------------------------------------------------------------------------|
|- Checks plugins, creates inis, imports variables, and calls setup functions associated with other includes.	-|
|----------------------------------------------------------------------------------------------------------------|
SUB e3_Setup
|---------------------------------------------------------------------------------------------------Declare Macro_Ini path\file and check plugins.
/declare Macro_Ini string outer e3\Macro_Inis\e3_Settings.ini
/call CheckPlugins

/echo Initializing...

|--------------------------------------------------------------------------------------------------------------------------Create macro variables.
/declare ActionTaken bool outer
/declare spamTimer timer outer

/if (!${Defined[Following]}) /declare Following bool global FALSE
/if (!${Defined[FollowTarget]}) /declare FollowTarget string global NULL
/if (!${Defined[Followers]}) /declare Followers string global NULL
/declare MeleeClasses string outer BRD,BER,BST,MNK,PAL,RNG,ROG,SHD,WAR
/declare PriestClasses string outer CLR,DRU,SHM
/declare CasterClasses string outer ENC,MAG,NEC,WIZ

|---------Check for Macro_Ini.  If it doesn't exist, run an initial setup to create a Macro_Ini and update aliases.
	/if (!${Ini[${Macro_Ini}].Length}) {
		/echo Welcome to e3, running first time setup...
		/call Update_Aliases
		/echo Building [${Macro_Ini}].  Edit this file to change universal macro settings.
		/call MakeMacro_Ini
		/declare initial_Setup bool outer TRUE
	}

|-------------------------------------------------------------------------------------------------------------------------Load Macro_Ini settings.
	/echo Importing Macro Ini settings...
	/call INItoVar ${Macro_Ini},Settings,DebugMode Debug bool outer
	/call INItoVar ${Macro_Ini},Settings,ValidateIniEntries validation_On bool outer
	/call IniToVar ${Macro_Ini},Settings,ChatChannel ChatToggle string outer
	/call IniToVar ${Macro_Ini},Settings,Verbosity Verbosity bool outer
	
	/call IniToVar ${Macro_Ini},Follow/MoveTo,FollowBreakDistance FollowBreakDistance int outer
	/call IniToVar ${Macro_Ini},Follow/MoveTo,MoveToBreakDistance MoveToBreakDistance int outer
	/call IniToVar ${Macro_Ini},Follow/MoveTo,LeashLength LeashLength int outer
	
	/call IniToVar ${Macro_Ini},FilePaths,Bot_Ini_Path Character_Ini string outer
	/call IniToVar ${Macro_Ini},FilePaths,discTimers_Ini_Path discTimer_Ini string outer	
	/call IniToVar ${Macro_Ini},FilePaths,Group_Ini_Path Group_Ini string outer
	/call IniToVar ${Macro_Ini},FilePaths,Forage_Ini_Path Forage_Ini string outer
	/call IniToVar ${Macro_Ini},FilePaths,itemTimers_Ini_Path itemTimers_Ini string outer	
	/call IniToVar ${Macro_Ini},FilePaths,Loot_Ini_Path Loot_Ini string outer
	/call IniToVar ${Macro_Ini},FilePaths,Routes_Ini_Path SmoothMoves_Ini string outer
	/call IniToVar ${Macro_Ini},FilePaths,SpellAliases_Ini_Path SpellAliases_Ini string outer
	/call IniToVar ${Macro_Ini},FilePaths,ZoneSettings_Ini_Path safeZones_Ini string outer
	
|---------------------------------------------------------------Check for Bot_Ini. If one doesn't exist, create it.
	/if (!${Ini[${Character_Ini}].Length}) {
		/echo Building [${Character_Ini}].  Review this file to adjust ${Me.CleanName}'s settings.
		/call MakeCharacter_Ini
	}

|--------------------------------------------------------------------------------------------------------------------------------Call .inc setups.
	/echo Importing Character Ini settings...

	/call setup_Casting
	
	/call setup_Background_Services
	
	/call setup_Assists

	/call ${Me.Class.ShortName}_Setup
	
	/call setup_BegForBuffs

	/call setup_Buffs

	/call setup_Heals

	/call setup_Pets
	
	/if (${Me.Ability[Forage]}) /call setup_AutoForage

	/call setup_Loot

	/call setup_Wait4Rez

|------------------------------------------------------------------------------------Used in corpse drag functions.
	/if (!${Defined[consentTargets]}) {
		/declare consentTargets string global ${Me.CleanName}
	} else /if (${Defined[consentTargets]}) {
		/varset consentTargets ${Me.CleanName}
	}

|-----------------------------------------------------------------------------------------------------------------Check for updates and DebugMode.
	/echo Checking for updates...
	/if (!${Select[${e3_Version},${Ini[${Macro_Ini},Settings,e3_Version]}]}) /call Update_e3 ${e3_Version}
	/if (!${e3_Updated}) {
		/echo e3 loaded.
		/call startMsg
	}
	
	/if (${Debug}) {
		/echo
		/echo
		/echo *** Debug_Mode ON ***
		/echo The macro has paused so you can review setup information, type /mqpause to continue.
		/echo
		/echo
		
	|------------------------------------------------------------------If macro is in DebugMode, pause after setup.
		/mqpause
	}
/RETURN



SUB startMsg
/if (${Debug}) /echo |- startMsg ==>

	/declare MsgNum int local ${Math.Rand[8]}

	/if (${MsgNum}==1) {
		/echo ** Unless you have recently made use of the donation link at http://code.google.com/p/e3-macro-builds/, I do not provide technical support for this macro. **
	} else /if (${MsgNum}==2) {
		/echo ** If you like my macro and want to show your appreciation, go to, http://code.google.com/p/e3-macro-builds/, to donate! **
	} else /if (${MsgNum}==3) {
		/echo ** One donation is worth a thousand thanks.  If you like my macro, go to, http://code.google.com/p/e3-macro-builds/, and donate! **
	} else /if (${MsgNum}==4) {
		/echo ** Unless you're a hot chick with firm breasts, or have recently made use of the donation link on the webpage, post all questions, issues, or suggestions on the issues page of http://code.google.com/p/e3-macro-builds/. **
	} else /if (${MsgNum}==5) {
		/echo ** What's that?! Oh, it's the donations link at, http://code.google.com/p/e3-macro-builds/.  Maybe you should check it out... **
	} else /if (${MsgNum}==6) {
		/echo ** Have you used the website's donation button today? **
	} else /if (${MsgNum}==7) {
		/echo ** If you like my macro, give me money!  Go to, http://code.google.com/p/e3-macro-builds/, and donate.
	} else /if (${MsgNum}==8) {
		/echo ** Unless you have donated, do not bug me for tech support. **
	}
	
/if (${Debug}) /echo <== startMsg -|
/RETURN



|----------------------------------------------------|
|- Checks for and enables required macro plugins.	-|
|----------------------------------------------------|
SUB CheckPlugins
/echo Checking Plugins...

|-----------------------------------------------------------------------------------------------------------------------------------Check MQ2EQBC.
/if (!${Plugin[MQ2EQBC].Name.Length}) {
	/echo Plugin MQ2EQBC is not loaded, attempting to resolve...
	/plugin MQ2EQBC
	/delay 10s !${Select[${EQBC},NULL]}
	/if (${Select[${EQBC},NULL]}) {
		/echo ***WARNING*** Could not connect to EQBC.  Make sure you have the EQBCS application running and restart.
		/endmacro
	}
}

	|-------------------------------------------------------------------------------------------Auto-Connect to EQBCS.
	/if (!${EQBC.Connected}) {
		/bccmd connect
		/delay 10s ${EQBC.Connected}
		/if (!${EQBC.Connected}) {
			/echo ***WARNING*** Could not connect to EQBCS, please open EQBCS and try again.
			/endmacro
		}
	}

|--------------------------------------------------------------------------------------------------------------------------------Check MQ2NetBots.
/if (!${Plugin[MQ2NetBots].Name.Length}) {
	/echo Plugin MQ2NetBots is not loaded, attempting to resolve...
	/plugin MQ2NetBots
	/delay 3s ${NetBots}
	/if (!${NetBots}) {
		/echo ***WARNING*** Could not load MQ2NetBots, please ensure you're using a copy of MQ2 which was compiled with the MQ2NetBots plugin and try again.
		/endmacro
	}
}
	|-------------------------------------------------------------------------------------------Auto-Connect to NetBots.
	/if (${NetBots}) {
		/squelch /netbots on grab=on send=on
	}
	
|-------------------------------------------------------------------------------------------------------------------------------Check MQ2Exchange.
/if (!${Plugin[MQ2Exchange].Name.Length}) {
	/echo Plugin MQ2Exchange is not loaded, attempting to resolve...
	/plugin MQ2Exchange
	/delay 3s ${Plugin[MQ2Exchange].Name.Length}
	/if (!${Plugin[MQ2Exchange].Name.Length}) {
		/echo ***WARNING*** Could not load MQ2Exchange, please ensure you're using a copy of MQ2 which was compiled with the MQ2Cast plugin and try again.
		/endmacro
	}
}

|------------------------------------------------------------------------------------------------------------------------------Check MQ2MoveUtils.
/if (!${Plugin[MQ2MoveUtils].Name.Length}) {
	/echo Plugin MQ2MoveUtils is not loaded, attempting to resolve...
	/plugin MQ2MoveUtils
	/delay 3s !${Select[${Stick.Status},NULL]}
	/if (${Select[${Stick.Status},NULL]}) {
		/echo ***WARNING*** Could not load MQ2MoveUtils, please ensure you're using a copy of MQ2 which was compiled with the MQ2NetAdvPath plugin and try again.
		/endmacro
	}
}

|-----------------------------------------------------------------------------------------------------------------------------Check MQ2NetAdvPath.
/if (!${Plugin[MQ2NetAdvPath].Name.Length}) {
	/echo Plugin MQ2NetAdvPath is not loaded, attempting to resolve...
	/plugin MQ2NetAdvPath
	/delay 3s ${NetAdvPath}
	/if (!${NetAdvPath}) {
		/echo ***WARNING*** Could not load MQ2NetAdvPath, please ensure you're using a copy of MQ2 which was compiled with the MQ2NetAdvPath plugin and try again.
		/echo Defaulting to MQ2MoveUtils
	}
}

|----------------------------------------------------------------------------------------------------------------------------------Check MQ2Twist.
/if (${Select[${Me.Class.ShortName},BRD]}) {
	/if (!${Plugin[MQ2Twist].Name.Length}) {
		/echo Plugin Unable to detect MQ2Twist, attempting to resolve...
		/plugin MQ2Twist
		/delay 3s !${Select[${Twist},NULL]}
		/if (${Select[${Twist},NULL]}) {
			/echo ***WARNING*** Could not load MQ2Twist, the macro may not be able to properly maintain melodies.
		}
	}	
}

|----------------------------------------------------------------------------------------------------------------------------Check MQ2ItemDisplay.
/if (!${Plugin[mq2itemdisplay].Name.Length}) {
	/echo Plugin Unable to detect MQ2ItemDisplay, attempting to resolve...
		/plugin MQ2ItemDisplay
		/delay 3s ${Plugin[mq2itemdisplay].Name.Length}
	/if (!${Plugin[mq2itemdisplay].Name.Length}) {
		/echo Could not load MQ2ItemDisplay, item inspection will not contain additional information.
	}
}

|------------------------------------------------------------------------------------------------------------------------------------Check MQ2HUD.
/if (!${Plugin[mq2hud].Name.Length}) {
	/echo Plugin Unable to detect MQ2HUD, attempting to resolve...
		/plugin MQ2HUD
		/delay 2s ${Plugin[mq2hud].Name.Length}
	/if (!${Plugin[mq2hud].Name.Length}) {
		/echo Could not load MQ2HUD.
	}
}

|-----------------------------------------------------------------------------------------------------------------------------------Check MQ2Cast.
/if (!${Plugin[MQ2Cast].Name.Length}) {
	/echo Plugin MQ2Cast is not loaded, attempting to resolve...
	/plugin MQ2Cast
	/delay 3s ${Cast.Active}
	/if (!${Cast.Active}) {
		/echo ***WARNING*** Could not load MQ2Cast, please ensure you're using a copy of MQ2 which was compiled with the MQ2Cast plugin and try again.
		/endmacro
	}
}
/RETURN



|------------------------------------------------------------|
|- Deletes and Recreates aliases associciated with e3.mac.	-|
|------------------------------------------------------------|
SUB Update_Aliases
/if (${Debug}) /echo |- Update_Aliases ==>
	
	/echo Updating macro aliases...
|------------------------------------------------------------------------------------------------------------------------------Create new aliases.

|-----------------------------------------------------------------------------------------------Follow aliases.
	/squelch /alias /followon delete
	/squelch /alias /followme delete
	/squelch /alias /followit delete
	/squelch /alias /stop delete
	
	/alias /followon /bc Follow ${Me.CleanName}
	/alias /followme /bc Follow ${Me.CleanName}
	/alias /followit /bc Follow ${Target.CleanName}
	/alias /stop /bc Stop
	
|-------------------------------------------------------------------------------------------Move to me aliases.
	/squelch /alias /Allmtm delete
	/squelch /alias /Priestmtm delete
	/squelch /alias /Priestsmtm delete
	/squelch /alias /Castermtm delete
	/squelch /alias /castersmtm delete
	/squelch /alias /Meleemtm delete
	/squelch /alias /Meleesmtm delete
	/squelch /alias /Rangedmtm delete
	/squelch /alias /Rangemtm delete
	
	/alias /Allmtm /bc move to me
	/alias /Priestmtm /bc Priests move to me
	/alias /Priestsmtm /bc Priests move to me
	/alias /Castermtm /bc Casters move me
	/alias /castersmtm /bc Casters move me
	/alias /Meleemtm /bc Melee move me
	/alias /Meleesmtm /bc Melee move me
	/alias /Rangedmtm /bc Ranged move me
	/alias /Rangemtm /bc Ranged move me

|---------------------------------------------------------------------------------------Move to target aliases.
	/squelch /alias /Allthere delete
	/squelch /alias /Athere delete
	/squelch /alias /PriestThere delete
	/squelch /alias /PriestsThere delete
	/squelch /alias /CasterThere delete
	/squelch /alias /CastersThere delete
	/squelch /alias /MeleeThere delete
	/squelch /alias /MeleesThere delete
	/squelch /alias /RangeThere delete
	/squelch /alias /RangedThere delete
	/squelch /alias /mtm delete
	
	/alias /Allthere /bc All move to ${Target.ID}
	/alias /Athere /bc All move to ${Target.ID}
	/alias /PriestThere /bc Priests to ${Target.ID}
	/alias /PriestsThere /bc Priests move to ${Target.ID}
	/alias /CasterThere /bc Casters move to ${Target.ID}
	/alias /CastersThere /bc Casters move to ${Target.ID}
	/alias /MeleeThere /bc Melee move to ${Target.ID}
	/alias /MeleesThere /bc Melee move to ${Target.ID}
	/alias /RangeThere /bc Ranged move to ${Target.ID}
	/alias /RangedThere /bc Ranged move to ${Target.ID}
	/alias /mtm /bc Move to me

|----------------------------------------------------------------------------------------------Group functions.
	/squelch /alias /group delete
	/squelch /alias /groupup delete
	/squelch /alias /setGroup delete
	/squelch /alias /SaveGroup delete
	/squelch /alias /disbandall delete
	/squelch /alias /disbandbots delete
	
	/alias /group /bc Group
	/alias /groupup /bc Group
	/alias /setGroup /bc SetGroup
	/alias /SaveGroup /bc Save Group
	/alias /disbandall /bc Disband all
	/alias /disbandbots /bc Disband all

|------------------------------------------------------------------------------------------------------Vet AAs.
	/squelch /alias /infusions delete
	/squelch /alias /infusion delete
	/squelch /alias /intensify delete
	/squelch /alias /intensity delete
	/squelch /alias /lessons delete
	/squelch /alias /lesson delete
	/squelch /alias /mgb delete
	
	/alias /infusions /bc Infusions
	/alias /infusion /bc Infusions
	/alias /intensify /bc Intensify
	/alias /intensity /bc Intensify
	/alias /lessons /bc Lessons
	/alias /lesson /bc Lessons
	/alias /mgb /bc MGB

|---------------------------------------------------------------------------------------------Medbreak aliases.
	/squelch /alias /medOn delete
	/squelch /alias /medOff delete
	
	/alias /medOn /bc Start MedBreak
	/alias /medOff /bc End MedBreak

|------------------------------------------------------------------------------------------------Debug aliases.
	/squelch /alias /debugON delete
	/squelch /alias /debugOFF delete

	/alias /debugON /varset Debug TRUE
	/alias /debugOFF /varset Debug FALSE

|-----------------------------------------------------------------------------------------------Zoning aliases.
	/squelch /alias /evac delete
	/squelch /alias /gate delete
	
	/alias /evac /bc Evac
	/alias /gate /bc Gate
	
|-------------------------------------------------------------------------------------------------Pets aliases.
	/squelch /alias /suspendpets delete
	/squelch /alias /suspets delete
	/squelch /alias /returnpets delete
	/squelch /alias /retpets delete
	/squelch /alias /shrinkpets delete
	
	/alias /suspendpets /bc Suspend Pets
	/alias /suspets /bc Suspend Pets
	/alias /returnpets /bc Return Pets
	/alias /retpets /bc Return Pets
	/alias /shrinkpets /bc Shrink Pets
	
|------------------------------------------------------------------------------------------------Buff aliases.
	/squelch /alias /buff delete
	/squelch /alias /buffme delete
	/squelch /alias /buffs delete
	/squelch /alias /bufft delete
	/squelch /alias /buffit delete
	/squelch /alias /buffall delete
	/squelch /alias /buffgroups delete
	/squelch /alias /buffpet delete
	
	/alias /buff /bc Buff me
	/alias /buffme /bc Buff me
	/alias /buffs /bc Buff me
	/alias /bufft /bc Buffs on ${Target.ID}
	/alias /buffit /bc Buffs on ${Target.ID}
	/alias /buffall /bc Buff All
	/alias /buffgroups /bc Buff All
	/alias /buffpet /bc Buffs on ${Spawn[pet ${Spawn[id ${Target.ID}].Pet}].ID}
	
|------------------------------------------------------------------------------------------------Heals aliases.
	/squelch /alias /updategroup delete
	/squelch /alias /updategrp delete
	/squelch /alias /rfshheals delete
	/squelch /alias /refreshheals delete
	/squelch /alias /resetheals delete
	/squelch /alias /rstheals delete
	
	/alias /updategroup /bc Update Group
	/alias /updategrp /bc Update Group
	/alias /rfshheals /bc Refresh Heals
	/alias /refreshheals /bc Refresh Heals
	/alias /resetheals /bc Refresh Heals
	/alias /rstheals /bc Refresh Heals
	
|---------------------------------------------------------------------------------------Class specific aliases.
	/squelch /alias /rezit delete
	
	/alias /rezit /bc RezOn ${Target.CleanName.Arg[1,']}
	
|----------------------------------------------------------------------------------------------Assists aliases.
	/squelch /alias /tauntOff delete
	/squelch /alias /tauntOn delete
	/squelch /alias /assiston delete
	/squelch /alias /assistme delete
	/squelch /alias /backoff delete
	/squelch /alias /dotsoff delete
	/squelch /alias /debuffsoff delete
	/squelch /alias /dotson delete
	/squelch /alias /rainoff delete
	/squelch /alias /rainon delete
	/squelch /alias /debuff delete
	/squelch /alias /debuffson delete
	/squelch /alias /SwarmPets delete
	/squelch /alias /quickburn delete
	/squelch /alias /qburn delete
	/squelch /alias /Fullburn delete
	/squelch /alias /fburn delete
	
	/alias /tauntOff /varset do_Taunt FALSE
	/alias /tauntOn /varset do_Taunt TRUE
	/alias /assiston /bc Assist me
	/alias /assistme /bc Assist me
	/alias /backoff /bc Back off
	/alias /dotsoff /bc DoTs off
	/alias /debuffsoff /bc Debuffs off
	/alias /dotson /bc DoTs on ${Target.ID}
	/alias /rainoff /bc Rain off
	/alias /rainon /bc Rain on
	/alias /debuff /bc Debuffs on ${Target.ID}
	/alias /debuffson /bc Debuffs on ${Target.ID}
	/alias /SwarmPets /bc Swarm Pets on ${Target.ID}
	/alias /quickburn /bc Quick Burn
	/alias /qburn /bc Quick Burn
	/alias /Fullburn /bc Full Burn
	/alias /fburn /bc Full Burn
		
|------------------------------------------------------------------------------------------SmoothMoves aliases.
	| this alias was added in error in version 4.1
	/squelch /alias /add delete

	/squelch /alias /recroute delete
	/squelch /alias /recordroute delete
	/squelch /alias /traceroute delete
	/squelch /alias /stoprec delete
	/squelch /alias /stoprecord delete
	/squelch /alias /stoprecording delete
	/squelch /alias /endrec delete
	/squelch /alias /endrecord delete
	/squelch /alias /endrecording delete
	/squelch /alias /listroutes delete
	/squelch /alias /listroute delete
	/squelch /alias /lstrts delete
	/squelch /alias /addopendoor delete
	/squelch /alias /addclickdoor delete
	/squelch /alias /addusedoor delete
	/squelch /alias /addopenzone delete
	/squelch /alias /addclickzone delete
	/squelch /alias /addusezone delete
	/squelch /alias /addclickstone delete
	/squelch /alias /addusestone delete
	/squelch /alias /addrunthruzone delete
	/squelch /alias /addrtz delete
	/squelch /alias /Stoproute delete
	/squelch /alias /allStoproute delete
	/squelch /alias /run delete
	/squelch /alias /runroute delete
	/squelch /alias /routes delete
	
	/alias /recroute /echo Record Route
	/alias /recordroute /echo Record Route
	/alias /traceroute /echo Record Route
	/alias /stoprec /echo End Recording
	/alias /stoprecord /echo End Recording
	/alias /stoprecording /echo End Recording
	/alias /endrec /echo End Recording
	/alias /endrecord /echo End Recording
	/alias /endrecording /echo End Recording
	/alias /listroutes /echo List Routes
	/alias /listroute /echo List Routes
	/alias /lstrts /echo List Routes
	/alias /addopendoor /echo Add Click Door
	/alias /addclickdoor /echo Add Click Door
	/alias /addusedoor /echo Add Click Door
	/alias /addopenzone /echo Add Click Zone
	/alias /addclickzone /echo Add Click Zone
	/alias /addusezone /echo Add Click Zone
	/alias /addclickstone /echo Add Click Zone
	/alias /addusestone /echo Add Click Zone
	/alias /addrunthruzone /echo Add Run through zone
	/alias /addrtz /echo Add Run through zone
	/alias /Stoproute /bc Stop Route
	/alias /allStoproute /bc Stop Route
	/alias /run /bc Run Route
	/alias /runroute /bc Run Route
	/alias /routes /echo List Routes
	
|-------------------------------------------------------------------------------------------------Misc aliases.
	/squelch /alias /consentme delete
	/squelch /alias /pingcorpses delete
	/squelch /alias /clkyes delete
	/squelch /alias /yes delete
	/squelch /alias /swap delete
	/squelch /alias /countbots delete
	/squelch /alias /count delete
	/squelch /alias /botCount delete
	/squelch /alias /finditem delete
	/squelch /alias /checkbuff delete
	/squelch /alias /removebuff delete
	/squelch /alias /botexit delete
	/squelch /alias /addtimer delete
	/squelch /alias /bark delete
	/squelch /alias /pvpon delete
	/squelch /alias /pvpoff delete
	/squelch /alias /findit delete
	/squelch /alias /dropinvis delete
	/squelch /alias /raidzone delete
	/squelch /alias /safezone delete
	/squelch /alias /raidz delete
	/squelch /alias /safez delete
	/squelch /alias /portto delete
	/squelch /alias /clickit delete
	
	/alias /consentme /bc Consent ${Me.CleanName}
	/alias /pingcorpses /bc Ping Corpses
	/alias /clkyes /bc Click yes
	/alias /yes /bc Click yes
	/alias /swap /echo Swap
	/alias /countbots /echo count bots
	/alias /rollcall /echo Roll Call
	/alias /count /echo count bots
	/alias /botCount /echo count bots
	/alias /finditem /bc Find Item
	/alias /checkbuff /bc Check Buff
	/alias /removebuff /bc Remove Buff
	/alias /botexit /bc Exit
	/alias /portto /bc Port To
	/alias /addtimer /echo Make Timer
	/alias /bark /bc Bark at ${Target.ID}
	/alias /pvpon /bc Toggle PvP [ON]
	/alias /pvpoff /bc Toggle PvP [OFF]
	/alias /findit /bc Find Item ${Cursor}
	/alias /dropinvis /bc Drop Invis
	/alias /raidzone /bc Set Raid Zone
	/alias /safezone /bc Set Safe Zone
	/alias /raidz /bc Set Raid Zone
	/alias /safez /bc Set Safe Zone
	/alias /clickit /bc Click object at ${Me.Loc.Replace[ ,]}
	
/if (${Debug}) /echo <== Update_Aliases -|
/RETURN



|--------------------------------------------------------------------------------------------------------------------|
|- Creates a Macro_Ini file in the path specified in ${Macro_Ini}, and populates it with universal macro settings.	-|
|--------------------------------------------------------------------------------------------------------------------|
|- @param Version: The e3_Version to write to e3_Settings.ini														-|
|--------------------------------------------------------------------------------------------------------------------|
SUB MakeMacro_Ini(Version)
/if (${Debug}) /echo |- MakeMacro_Ini ==>

|-----------------------------------------------------------------------------------------------------------------------------------Macro entries.
	/call WriteToIni ${Macro_Ini},Settings,e3_Version 4.1 1
	/call WriteToIni ${Macro_Ini},Settings,ChatChannel /bc
	/call WriteToIni ${Macro_Ini},Settings,DebugMode Off
	/call WriteToIni ${Macro_Ini},Settings,ValidateIniEntries On
	/call WriteToIni ${Macro_Ini},Settings,Verbosity Off
	
|---------------------------------------------------------------------------------------------------------------------e3_Macro_Assist.inc entries.
	/call WriteToIni ${Macro_Ini},Assists,AssistBreakDistance 250
	/call WriteToIni ${Macro_Ini},Assists,Allowed_TargetTypes NPC,Pet
	/call WriteToIni ${Macro_Ini},Assists,AutoAssist(On/Off) On
	/call WriteToIni ${Macro_Ini},Assists,EngagePct 98
	/call WriteToIni ${Macro_Ini},Assists,AE_Radius 200
	
|-----------------------------------------------------------------------------------------------------------------------e3_Macro_Loot.inc entries.
	/call WriteToIni ${Macro_Ini},AutoLoot,AutoLoot(On/Off) On
	/call WriteToIni ${Macro_Ini},AutoLoot,Looters Bitz,Shagga,Draaxx
	/call WriteToIni ${Macro_Ini},AutoLoot,AutoLoot_Alert 3
	/call WriteToIni ${Macro_Ini},AutoLoot,Seek_Radius 35
	
|-------------------------------------------------------------------------------------------------------------------------------------AutoTribute.
	/call WriteToIni ${Macro_Ini},AutoTribute,AutoTribute(On/Off) On
	
|-----------------------------------------------------------------------------------------------------------------e3_Macro_Background.inc entries.
	/call WriteToIni ${Macro_Ini},Background,AutoDestroy "*List names, or IDs, of items you want to automatically destroy when they are on your cursor.  Use , to add additional items."
	/call WriteToIni ${Macro_Ini},Background,IdleTimeOut 5m
	/call WriteToIni ${Macro_Ini},Background,AutoInventoryTimer 30s
	/call WriteToIni ${Macro_Ini},Background,CloseSpellBookTimer 30s

|------------------------------------------------------------------------------------------------------------------e3_Macro_BuffCheck.inc entries.
	/call WriteToIni ${Macro_Ini},Buffs,Allow_BuffRequests On
	/call WriteToIni ${Macro_Ini},Buffs,Restricted_BuffRequests On

|--------------------------------------------------------------------------------------------------------------------e3_Macro_Casting.inc entries.
	/call WriteToIni ${Macro_Ini},Casting,PauseStickToCast Off
	/call WriteToIni ${Macro_Ini},Casting,Default_SpellSet "*Name of your saved spellsets"
	/call WriteToIni ${Macro_Ini},Casting,Default_SpellGem 8
	
|-----------------------------------------------------------------------------------------------------------------------------------Corpse Summoning.
	/call WriteToIni ${Macro_Ini},CorpseSummoning,SoulStone "Pristine Soulstone"
	/call WriteToIni ${Macro_Ini},CorpseSummoning,SoulStoneCost 170
|---------------------------------------------------------------------------------------------------------------------------Follow/MoveTo entries.
	/call WriteToIni ${Macro_Ini},Follow/MoveTo,FollowBreakDistance 250
	/call WriteToIni ${Macro_Ini},Follow/MoveTo,MoveToBreakDistance 750
	/call WriteToIni ${Macro_Ini},Follow/MoveTo,LeashLength 125

|-------------------------------------------------------------------------------------------------------------------e3_Macro_Wait4Rez.inc entries.
	/call WriteToIni ${Macro_Ini},Wait4Rez,Wait4Rez(On/Off) On
	/call WriteToIni ${Macro_Ini},Wait4Rez,Wait4Rez_Alert 1
	/call WriteToIni ${Macro_Ini},Wait4Rez,BagDelay 20
	/call WriteToIni ${Macro_Ini},Wait4Rez,ItemDelay 3

|----------------------------------------------------------------------------------------------------------------------------------Ini file paths.
	/noparse /ini e3\Macro_Inis\e3_Settings FilePaths Bot_Ini_Path e3\Bot_Inis\Bot_${Me.CleanName}.ini
	/noparse /ini e3\Macro_Inis\e3_Settings FilePaths discTimers_Ini_Path e3\Macro_Inis\discTimers.ini
	/noparse /ini e3\Macro_Inis\e3_Settings FilePaths Group_Ini_Path e3\Macro_Inis\Groups.ini
	/noparse /ini e3\Macro_Inis\e3_Settings FilePaths Forage_Ini_Path e3\Macro_Inis\Forage.ini
	/noparse /ini e3\Macro_Inis\e3_Settings FilePaths itemTimers_Ini_Path e3\Macro_Inis\itemTimers.ini
	/noparse /ini e3\Macro_Inis\e3_Settings FilePaths Loot_Ini_Path e3\Macro_Inis\Loot.ini
	/noparse /ini e3\Macro_Inis\e3_Settings FilePaths Routes_Ini_Path e3\Macro_Inis\Routes.ini
	/noparse /ini e3\Macro_Inis\e3_Settings FilePaths SpellAliases_Ini_Path e3\Macro_Inis\SpellAliases.ini
	/noparse /ini e3\Macro_Inis\e3_Settings FilePaths ZoneSettings_Ini_Path e3\Macro_Inis\ZoneSettings.ini
	
/if (${Debug}) /echo <== MakeMacro_Ini -|
/RETURN



|------------------------------------------------------------------------------------------------------------|
|- Creates a Macro_Ini file in the path specified in ${Macro_Ini}, and populates it with macro settings.	-|
|------------------------------------------------------------------------------------------------------------|
SUB MakeCharacter_Ini
/if (${Debug}) /echo |- MakeCharacter_Ini ==>

|-----------------------------------------------------------------------------------------------------------------------------LifeSupport entries.
	/call WriteToIni ${Character_Ini},LifeSupport,LifeSupport#1 "*List spells, items, AAs, or discs you want this character to use in emergencies. Order entries from highest % HPs to lowest.  Specify ability Name / Type / PctHPs to use, ex. LifeSupport#1=Feign Death/gem5/40, LifeSupport#2=Distillate of Divine Healing X/item/30, LifeSupport#3=Death Peace/alt/20, LifeSupport#4=Weaponshield Discipline/disc/15."

|------------------------------------------------------------------------------------------------------------------e3_Macro_BuffCheck.inc entries.
/if (!${Ini[${Character_Ini},Buffs].Length}) {
	/call WriteToIni ${Character_Ini},Buffs,SelfBuff#1 "*List any items, spells, or alternate abilities you want this character to maintain on themselves.  If using a clicky, add '/item' after the item name, '/alt', for an alternate ability.  Ex. Buff#1=Mask of Deception/item, Buff#2=Flaming Arrows/alt"
	/call WriteToIni ${Character_Ini},Buffs,GroupBuff#1 "*List any buffs you want this character to cast, when given the 'Buff me' command"
	/call WriteToIni ${Character_Ini},Buffs,BuffOther#1 "*List any buffs to maintain on other NetBots, at a low priority ( BuffOthers are cast when you stop to rebuff yourself ). Add '/group' to spells that can only be cast on group members.  Ex. BuffOther#1=Yasa/Call of the Rathe"
	/call WriteToIni ${Character_Ini},Buffs,CombatBuff#1 "*List any buffs to maintain on other NetBots, at a high priority ( These are buffs maintained during combat like, Avatar, Jaguar, Divine Intervention etc.). Add '/group' to spells that can only be cast on group members. Ex. CombatBuff#1=Killians/Spirit of the Jaguar/gem4"
	/call WriteToIni ${Character_Ini},Buffs,InstantIllusion "*Enter an instant click illusion item to constantly maintain"
}

|--------------------------------------------------------------------------------------------------------------------------Class specific entries.
	/call Make${Me.Class.ShortName}_Ini

|-------------------------------------------------------------------------------------------------If I'm a healer, add e3_Macro_Heals.inc entries.
/if (${Me.Class.HealerType}) {
	/if (!${Ini[${Character_Ini},Heals].Length}) {
		/call WriteToIni ${Character_Ini},Heals,MainTank "*List a main tank. Use , to add additional tanks.  Ex MainTank=Killians,Jalal,Bitz"
		/call WriteToIni ${Character_Ini},Heals,MainTank_Heal(Spell/Gem/Pct) "*Heal spell to use on the main tank/gem to mem it to/pct to heal the main tank at"
		/call WriteToIni ${Character_Ini},Heals,MainTank_QuickHeal(Spell/Gem/Pct) "*Quick heal spell to use on the main tank/gem to mem it to/pct to heal the main tank at"
		/call WriteToIni ${Character_Ini},Heals,Patch_Heal(Spell/Gem/Pct) "*Heal spell to use on everyone except the main tank/gem to mem it to/pct to cast at"
		/call WriteToIni ${Character_Ini},Heals,PriorityTarget#1 "*List targets take priority over other characters when being considered for heals. ( You dont need to add tanks here )"
		/call WriteToIni ${Character_Ini},Heals,HoT_Mode(MTOnly/All/Off) "*Heal Over Time Mode, 'MTOnly' will only cast HoTs on your main tank, 'All' will cast HoT on everyone"
		/call WriteToIni ${Character_Ini},Heals,Heal_Over_Time(Spell/Gem/Pct) "*Heal over time spell to cast/gem to mem it to/pct to cast at"
		/call WriteToIni ${Character_Ini},Heals,Pet_Heal(Spell/Gem/Pct) "*Heal spell to cast on pets/gem to mem it to/pct to cast at"
		/if (${Me.Class.ShortName.Equal[CLR]} || ${Me.Class.ShortName.Equal[DRU]}) /call WriteToIni ${Character_Ini},Heals,Join_CH-Chain(On/Off) "*Set this 'On' to have this character join in Complete Heal chains."
	}
}

|-----------------------------------------------------------------------------------------------If I'm a pet class, add e3_Macro_Pets.inc entries.
/if (${Select[${Me.Class.ShortName},BST,ENC,MAG,NEC,SHD,SHM]}) {
	/if (!${Ini[${Character_Ini},Pets].Length}) {
		/call WriteToIni ${Character_Ini},Pets,Pet(Spell/Reageant) "*Pet spell/reageant if there is one"
		/call WriteToIni ${Character_Ini},Pets,Pet_AutoShrink(On/Off) "*Do you want to automatically shrink pets?"
		/call WriteToIni ${Character_Ini},Pets,Pet_Heal(Spell/Gem/Pct) "*Would you like to use a pet specific heal like Planar Renewal or Touch of Death?"
		/call WriteToIni ${Character_Ini},Pets,Pet_Mend(Pct) "*If you have the 'Mend Companion' alternate ability, what percent would you like to use it at?"
		/call WriteToIni ${Character_Ini},Pets,PetBuff#1 "*List any spells you want this character to maintain on their pets"
	}
}

|--------------------------------------------------------------------------------------------------------------------e3_Macro_Assists.inc entries.

|-------------------------------------------------------------------If I'm a melee class, add Assist_Stick entries.
/if (${Select[${Me.Class.ShortName},BRD,BER,BST,MNK,PAL,RNG,ROG,SHD,WAR]}) {
	/if (!${Ini[${Character_Ini},Assists].Length}) {
		/call WriteToIni ${Character_Ini},Assists,AssistType(Melee/Ranged) Melee
		/call WriteToIni ${Character_Ini},Assists,Melee_StickPoint behind
		/call WriteToIni ${Character_Ini},Assists,Melee_StickDistance MaxMelee
	}
}	

|----------------------------------------------------------------------If I'm a melee class, add Abilities entries.
/if (${Select[${Me.Class.ShortName},BER,BST,MNK,PAL,RNG,ROG,SHD,WAR]}) {
	/if (!${Ini[${Character_Ini},Abilities].Length}) {
		/call WriteToIni ${Character_Ini},Abilities,Ability#1 "*List any melee abilities you want this character to use, add /ability, /alt, or /disc respectively. Ex. Ability#1=Feral Swipe/alt"
	}
}

|-------------------------------------------------------------------------If I'm a casting class, add Nuke entries.
/if (${Select[${Me.Class.ShortName},BST,DRU,ENC,MAG,PAL,NEC,RNG,SHD,SHM,WIZ]}) {
	/if (!${Ini[${Character_Ini},Nukes].Length}) {
		/call WriteToIni ${Character_Ini},Nukes,Ex#1 "*List any nukes you want this character to cast during assists, specify gem# or item and a minimum mana or item timer. - Nuke#1=Name/Gem#/MinMana-Timer/Drop Aggro Delay"
		/call WriteToIni ${Character_Ini},Nukes,Ex#2 "*Ex Spell Entry: Nuke#1=Frozen Wind/gem7/35m/7s"
		/call WriteToIni ${Character_Ini},Nukes,Ex#3 "*Ex Item Entry: Aged Shissar Elemntalist Staff/item/210s/3s"
		/call WriteToIni ${Character_Ini},Nukes,Nuke#1
	}
}

|--------------------------------------------------------------------------If I'm a casting class, add DoT entries.
/if (${Select[${Me.Class.ShortName},BST,DRU,ENC,NEC,RNG,SHD,SHM]}) {
	/if (!${Ini[${Character_Ini},DoTs].Length}) {
		/call WriteToIni ${Character_Ini},DoTs,Ex#1 "*List any DoTs you want this character to cast, the gem to mem it to, and the minimum pct you need to cast the spell"
		/call WriteToIni ${Character_Ini},DoTs,Ex#2 "*DoT_OnAssist will be cast on each target, while DoT_OnCommand will only be cast on command."
		/call WriteToIni ${Character_Ini},DoTs,Ex#3 "*Ex Spell Entry:  DoT_OnAssist#1=Blood of Thule/gem7/35m"
		/call WriteToIni ${Character_Ini},DoTs,Ex#4 "*Ex Item Entry:  DoT_OnCommand#1=Aged Shissar Deathspeaker Staff/item/120s"
		/call WriteToIni ${Character_Ini},DoTs,DoT_OnAssist#1
		/call WriteToIni ${Character_Ini},DoTs,DoT_OnCommand#1
	}
}

|-----------------------------------------------------------------------If I'm a casting class, add Debuff entries.
/if (${Select[${Me.Class.ShortName},BST,DRU,ENC,MAG,NEC,SHM]}) {
	/if (!${Ini[${Character_Ini},Debuffs].Length}) {
		/call WriteToIni ${Character_Ini},Debuffs,Ex#1 "*List any Debuffs you want this character to cast, the gem to mem it to, the minimum pct mana to have to cast the spell"
		/call WriteToIni ${Character_Ini},Debuffs,Ex#2 "*Debuff_OnAssist will be cast on each target, while Debuff_OnCommand will only be cast on command."
		/call WriteToIni ${Character_Ini},Debuffs,Ex#3 "*Ex Spell Entry:  Debuff_OnAssist#1=Turgur's Insects/gem7/35m"
		/call WriteToIni ${Character_Ini},Debuffs,Ex#4 "*Ex Item Entry:  Debuff_OnCommand#1=Time's Antithesis/item/5s"
		/call WriteToIni ${Character_Ini},Debuffs,Debuff_OnAssist#1
		/call WriteToIni ${Character_Ini},Debuffs,Debuff_OnCommand#1
	}
}

|-------------------------------------------------------------------------If I'm a casting class, add Rain entries.
/if (${Select[${Me.Class.ShortName},DRU,MAG,SHM,WIZ]}) {
	/if (!${Ini[${Character_Ini},Rain].Length}) {
		/call WriteToIni ${Character_Ini},Rain,Ex#1 "*List any Rain spells you want this character to cast, the gem to mem it to, and the minimum pct mana to have to cast the spell,"
		/call WriteToIni ${Character_Ini},Rain,Ex#2 "*Ex. Rain#1=Name/MinMana/Gem# - Rain#1=Tears of Marr/gem7/25m"
		/call WriteToIni ${Character_Ini},Rain,Rain#1
	}
}

|-----------------------------------------------------------------------------------------------------Burn entries.
/if (!${Ini[${Character_Ini},Burn].Length}) {
	/call WriteToIni ${Character_Ini},Burn,Ex#1 "*List any Spells/Items/AAs/Discs you want this character to use during quick and full burns."
	/call WriteToIni ${Character_Ini},Burn,Ex#2 "*Ex1 Spell: Nuke#1=Frozen Wind/gem7"
	/call WriteToIni ${Character_Ini},Burn,Ex#3 "*Ex2 AA: Guardian of the Forest/alt"
	/call WriteToIni ${Character_Ini},Burn,Ex#4 "*Ex Item: Aged Shissar Elemntalist Staff/item/210s"
	/call WriteToIni ${Character_Ini},Burn,Ex#5 "*Ex Disc: Trueshot Discipline/disc"
	/call WriteToIni ${Character_Ini},Burn,QuickBurn#1
	/call WriteToIni ${Character_Ini},Burn,FullBurn#1
}

|-------------------------------------------------------------------------------------------------------------------------------Swap item entries.
/call WriteToIni ${Character_Ini},SwapItems,Ex#1 "*Create an entry of any name, List ItemName/SlotToEquip."
/call WriteToIni ${Character_Ini},SwapItems,Ex#2 "*Ex. CrappySword=Rusty Longsword/mainhand"
/call WriteToIni ${Character_Ini},SwapItems,Ex#2 "*Use a , to add additional items to swap on the same command"
/call WriteToIni ${Character_Ini},SwapItems,Ex#3 "*Ex. TwoCrappySwords=Rusty Longsword/Mainhand,Rusty Short Sword/Offhand"
/call WriteToIni ${Character_Ini},SwapItems,Ex#4 "*Use /swap EntryName in game to swap the items.  Ex /swap CrappySword, or /swap TwoCrappySwords"

|--------------------------------------------------------------------------------------------------------------------------Check Reageant entries.
/call WriteToIni ${Character_Ini},ReageantCheck,Ex#1 "*List reageants you want to keep track of, along with the minimum to have in stock."
/call WriteToIni ${Character_Ini},ReageantCheck,Reageant#1 "*Ex. Small Portal Fragments/50"

/if (${Debug}) /echo <== MakeCharacter_Ini -|
/RETURN



|----------------------------------------------------------------------------|
|- Build an ini file, of a given name, that contains alphabetized sections	-|
|----------------------------------------------------------------------------|
|- @param Ini_File: Name of the ini file you want to create.				-|
|----------------------------------------------------------------------------|
SUB Build_Alphabetized_Ini(Ini_File)
/if (${Debug}) /echo |- Build_Alphabetized_Ini ==>
	
	/echo Building [${Ini_File}] file...
	/ini ${Ini_File} A A Apes
	/ini ${Ini_File} B B Brush
	/ini ${Ini_File} C C Cranberries
	/ini ${Ini_File} D D Down
	/ini ${Ini_File} E E Eskimo
	/ini ${Ini_File} F F Foreheads.
	/ini ${Ini_File} G G Goats
	/ini ${Ini_File} H H Hunt
	/ini ${Ini_File} I I "Indians (Feather not dot)."
	/ini ${Ini_File} J J Jackrabbits
	/ini ${Ini_File} K K Kill
	/ini ${Ini_File} L L Lazy
	/ini ${Ini_File} M M Mothers.
	/ini ${Ini_File} N N "Nile Crocodiles"
	/ini ${Ini_File} O O Open
	/ini ${Ini_File} P P People
	/ini ${Ini_File} Q Q Quite
	/ini ${Ini_File} R R Readily.
	/ini ${Ini_File} S S Salamanders
	/ini ${Ini_File} T T Toot
	/ini ${Ini_File} U U Under
	/ini ${Ini_File} V V Volcanoes.
	/ini ${Ini_File} W W Warthogs
	/ini ${Ini_File} X X X-Ray
	/ini ${Ini_File} Y Y Young
	/ini ${Ini_File} Z Z Zebras.
  
/if (${Debug}) /echo |- Build_Alphabetized_Ini ==>
/RETURN



|------------------------|
|- Build discTimer_Ini.	-|
|------------------------|
SUB Make_discTimers_Ini
/echo Building [${discTimer_Ini}] file...

|- Bard entries -|

/call WriteToIni "${discTimer_Ini},Bard,Puretone Discipline" 4050s
/call WriteToIni "${discTimer_Ini},Bard,Deftdance Discipline" 3780s
/call WriteToIni "${discTimer_Ini},Bard,Fearless Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Bard,Resistant Discipline" 1800s

|- Beastlord entries -|

/call WriteToIni "${discTimer_Ini},Beastlord,Protective Spirit Discipline" 234s
/call WriteToIni "${discTimer_Ini},Beastlord,Bestial Fury Discipline" 1530s
/call WriteToIni "${discTimer_Ini},Beastlord,Fearless Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Beastlord,Resistant Discipline" 1800s

|- Berserker entries -|

/call WriteToIni "${discTimer_Ini},Berserker,Resistant Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Berserker,Fearless Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Berserker,Healing Will Discipline" 300s
/call WriteToIni "${discTimer_Ini},Berserker,Focused Will Discipline" 300s
/call WriteToIni "${discTimer_Ini},Berserker,Leg Strike" 12s
/call WriteToIni "${discTimer_Ini},Berserker,Leg Cut" 12s
/call WriteToIni "${discTimer_Ini},Berserker,Leg Slice" 12s
/call WriteToIni "${discTimer_Ini},Berserker,Head Strike" 12s
/call WriteToIni "${discTimer_Ini},Berserker,Head Pummel" 12s
/call WriteToIni "${discTimer_Ini},Berserker,Head Crush" 12s
/call WriteToIni "${discTimer_Ini},Berserker,Divertive Strike" 12s
/call WriteToIni "${discTimer_Ini},Berserker,Distracting Strike" 12s
/call WriteToIni "${discTimer_Ini},Berserker,Confusing Strike" 12s
/call WriteToIni "${discTimer_Ini},Berserker,Corroded Axe" 6s
/call WriteToIni "${discTimer_Ini},Berserker,Blunt Axe" 6s
/call WriteToIni "${discTimer_Ini},Berserker,Steel Axe" 6s
/call WriteToIni "${discTimer_Ini},Berserker,Bearded Axe" 6s
/call WriteToIni "${discTimer_Ini},Berserker,Mithril Axe" 6s
/call WriteToIni "${discTimer_Ini},Berserker,Balanced War Axe" 6s
/call WriteToIni "${discTimer_Ini},Berserker,Bonesplicer Axe" 6s
/call WriteToIni "${discTimer_Ini},Berserker,Fleshtear Axe" 6s
/call WriteToIni "${discTimer_Ini},Berserker,Cold Steel Cleaving Axe" 6s
/call WriteToIni "${discTimer_Ini},Berserker,Mithril Bloodaxe" 6s
/call WriteToIni "${discTimer_Ini},Berserker,Rage Axe" 6s
/call WriteToIni "${discTimer_Ini},Berserker,Bloodseeker's Axe" 6s
/call WriteToIni "${discTimer_Ini},Berserker,Battlerage Axe" 6s
/call WriteToIni "${discTimer_Ini},Berserker,Deathfury Axe" 6s
/call WriteToIni "${discTimer_Ini},Berserker,Battle Cry" 1800s
/call WriteToIni "${discTimer_Ini},Berserker,War Cry" 1800s
/call WriteToIni "${discTimer_Ini},Berserker,Battle Cry of Dravel" 1800s
/call WriteToIni "${discTimer_Ini},Berserker,War Cry of Dravel" 1800s
/call WriteToIni "${discTimer_Ini},Berserker,Battle Cry of the Mastruq" 1800s
/call WriteToIni "${discTimer_Ini},Berserker,Ancient: Cry of Chaos" 1800s
/call WriteToIni "${discTimer_Ini},Berserker,Burning Rage Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Berserker,Focused Fury Discipline" 1500s
/call WriteToIni "${discTimer_Ini},Berserker,Cleaving Rage Discipline" 1500s
/call WriteToIni "${discTimer_Ini},Berserker,Battle Focus Discipline" 2400s
/call WriteToIni "${discTimer_Ini},Berserker,Inspired Anger Discipline" 1500s
/call WriteToIni "${discTimer_Ini},Berserker,Reckless Discipline" 2400s
/call WriteToIni "${discTimer_Ini},Berserker,Blind Rage Discipline" 1500s
/call WriteToIni "${discTimer_Ini},Berserker,Indomitable Discipline" 1320s
/call WriteToIni "${discTimer_Ini},Berserker,Cleaving Anger Discipline" 1320s
/call WriteToIni "${discTimer_Ini},Berserker,Sprint Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Berserker,Tainted Axe of Hatred" 6s
/call WriteToIni "${discTimer_Ini},Berserker,Throw Stone" 10s
/call WriteToIni "${discTimer_Ini},Berserker,Rage Volley" 12s

|- Monk entries -|

/call WriteToIni "${discTimer_Ini},Monk,Earthwalk Discipline" 234s
/call WriteToIni "${discTimer_Ini},Monk,Ancient: Phantom Chaos" 20s
/call WriteToIni "${discTimer_Ini},Monk,Throw Stone" 10s
/call WriteToIni "${discTimer_Ini},Monk,Phantom Zephyr" 20s
/call WriteToIni "${discTimer_Ini},Monk,Phantom Wind" 20s
/call WriteToIni "${discTimer_Ini},Monk,Stonestance Discipline" 234s
/call WriteToIni "${discTimer_Ini},Monk,Whirlwind Discipline" 2400s
/call WriteToIni "${discTimer_Ini},Monk,Voiddance Discipline" 2400s
/call WriteToIni "${discTimer_Ini},Monk,Phantom Echo" 20s
/call WriteToIni "${discTimer_Ini},Monk,Phantom Call" 20s
/call WriteToIni "${discTimer_Ini},Monk,Phantom Shadow" 20s
/call WriteToIni "${discTimer_Ini},Monk,Planeswalk Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Monk,Innerflame Discipline" 1320s
/call WriteToIni "${discTimer_Ini},Monk,Hundred Fists Discipline" 1320s
/call WriteToIni "${discTimer_Ini},Monk,Speed Focus Discipline" 1320s
/call WriteToIni "${discTimer_Ini},Monk,Fearless Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Monk,Focused Will Discipline" 300s
/call WriteToIni "${discTimer_Ini},Monk,Thunderkick Discipline" 600s
/call WriteToIni "${discTimer_Ini},Monk,Silentfist Discipline" 600s
/call WriteToIni "${discTimer_Ini},Monk,Ashenhand Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Monk,Healing Will Discipline" 300s
/call WriteToIni "${discTimer_Ini},Monk,Resistant Discipline" 1800s

|- Paladin entries -|

/call WriteToIni "${discTimer_Ini},Paladin,Holyforge Discipline" 4050s
/call WriteToIni "${discTimer_Ini},Paladin,Sanctification Discipline" 4050s
/call WriteToIni "${discTimer_Ini},Paladin,Fearless Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Paladin,Resistant Discipline" 1800s

|- Ranger entries -|

/call WriteToIni "${discTimer_Ini},Ranger,Weapon Shield Discipline" 4050s
/call WriteToIni "${discTimer_Ini},Ranger,Trueshot Discipline" 4050s
/call WriteToIni "${discTimer_Ini},Ranger,Fearless Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Ranger,Resistant Discipline" 1800s

|- Rogue entries -|

/call WriteToIni "${discTimer_Ini},Rogue,Weapon Affinity Discipline"
/call WriteToIni "${discTimer_Ini},Rogue,Fearless Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Rogue,Resistant Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Rogue,Healing Will Discipline" 300s
/call WriteToIni "${discTimer_Ini},Rogue,Focused Will Discipline" 300s
/call WriteToIni "${discTimer_Ini},Rogue,Sneak Attack" 30s
/call WriteToIni "${discTimer_Ini},Rogue,Thief's Vengeance" 30s
/call WriteToIni "${discTimer_Ini},Rogue,Assassin's Strike" 30s
/call WriteToIni "${discTimer_Ini},Rogue,Kyv Strike" 30s
/call WriteToIni "${discTimer_Ini},Rogue,Ancient: Chaos Strike" 30s
/call WriteToIni "${discTimer_Ini},Rogue,Throw Stone" 10s
/call WriteToIni "${discTimer_Ini},Rogue,Deadly Precision Discipline" 300s
/call WriteToIni "${discTimer_Ini},Rogue,Kinesthetics Discipline" 300s
/call WriteToIni "${discTimer_Ini},Rogue,Deadeye Discipline" 1200s
/call WriteToIni "${discTimer_Ini},Rogue,Blinding Speed Discipline" 1200s
/call WriteToIni "${discTimer_Ini},Rogue,Nimble Discipline" 1260s
/call WriteToIni "${discTimer_Ini},Rogue,Duelist Discipline" 1320s
/call WriteToIni "${discTimer_Ini},Rogue,Twisted Chance Discipline" 1320s
/call WriteToIni "${discTimer_Ini},Rogue,Counterattack Discipline" 2400s

|- Shadow Knight entries -|

/call WriteToIni "${discTimer_Ini},Shadow Knight,Leechcurse Discipline" 4050s
/call WriteToIni "${discTimer_Ini},Shadow Knight,Unholy Aura Discipline" 2160s
/call WriteToIni "${discTimer_Ini},Shadow Knight,Fearless Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Shadow Knight,Resistant Discipline" 1800s

|- Warrior entries -|

/call WriteToIni "${discTimer_Ini},Warrior,Ancient: Chaos Cry" 30s
/call WriteToIni "${discTimer_Ini},Warrior,Bellow of the Mastruq" 30s
/call WriteToIni "${discTimer_Ini},Warrior,Stonewall Discipline" 630s
/call WriteToIni "${discTimer_Ini},Warrior,Incite" 30s
/call WriteToIni "${discTimer_Ini},Warrior,Healing Will Discipline" 300s
/call WriteToIni "${discTimer_Ini},Warrior,Spirit of Rage Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Warrior,Aggressive Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Warrior,Fortitude Discipline" 2400s
/call WriteToIni "${discTimer_Ini},Warrior,Fellstrike Discipline" 1500s
/call WriteToIni "${discTimer_Ini},Warrior,Precision Discipline" 1500s
/call WriteToIni "${discTimer_Ini},Warrior,Berate" 30s
/call WriteToIni "${discTimer_Ini},Warrior,Furious Discipline" 2400s
/call WriteToIni "${discTimer_Ini},Warrior,Defensive Discipline" 630s
/call WriteToIni "${discTimer_Ini},Warrior,Mighty Strike Discipline" 1500s
/call WriteToIni "${discTimer_Ini},Warrior,Charge Discipline" 1500s
/call WriteToIni "${discTimer_Ini},Warrior,Bellow" 30s
/call WriteToIni "${discTimer_Ini},Warrior,Evasive Discipline" 468s
/call WriteToIni "${discTimer_Ini},Warrior,Fearless Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Warrior,Resistant Discipline" 1800s
/call WriteToIni "${discTimer_Ini},Warrior,Provoke" 30s
/call WriteToIni "${discTimer_Ini},Warrior,Focused Will Discipline" 300s
/call WriteToIni "${discTimer_Ini},Warrior,Throw Stone" 10s
/RETURN



|----------------------------|
|- Build SpellAliases_Ini.	-|
|----------------------------|
SUB MakeSpellAliases_Ini

/echo Building [${SpellAliases_Ini}] file...
|-------------------------------------------------------------------------------------Beastlord Aliases.
	/call WriteToIni "${SpellAliases_Ini},Beastlord,SD" "Spiritual Dominion"
	/call WriteToIni "${SpellAliases_Ini},Beastlord,SV" "Spiritual Vigor"

|-------------------------------------------------------------------------------------Cleric Aliases.
	/call WriteToIni "${SpellAliases_Ini},Cleric,Kazad" "Kazad's Mark"
	/call WriteToIni "${SpellAliases_Ini},Cleric,Virtue" "Virtue"
	/call WriteToIni "${SpellAliases_Ini},Cleric,Virt" "Virtue"
	/call WriteToIni "${SpellAliases_Ini},Cleric,HoV" "Hand of Virtue"	
	/call WriteToIni "${SpellAliases_Ini},Cleric,BoR" "Blessing of Reverence"	
	/call WriteToIni "${SpellAliases_Ini},Cleric,CHeal" "Complete Heal"	
	/call WriteToIni "${SpellAliases_Ini},Cleric,HoT" "Holy Elixir"
	/call WriteToIni "${SpellAliases_Ini},Cleric,Heal" "Holy Light"
	/call WriteToIni "${SpellAliases_Ini},Cleric,QHeal" "Supernal Remedy"
	/call WriteToIni "${SpellAliases_Ini},Cleric,GroupHeal" "Word of Replenishment"
	/call WriteToIni "${SpellAliases_Ini},Cleric,GroupHoT" "Ethereal Elixir"	
	
|-------------------------------------------------------------------------------------Druid Aliases.
	/call WriteToIni "${SpellAliases_Ini},Druid,FoE" "Flight of Eagles"
	/call WriteToIni "${SpellAliases_Ini},Druid,DS" "Legacy of Bracken"
	/call WriteToIni "${SpellAliases_Ini},Druid,BoT9" "Blessing of the Nine"	
	/call WriteToIni "${SpellAliases_Ini},Druid,Heal" "Sylvan Infusion"
	/call WriteToIni "${SpellAliases_Ini},Druid,Regen" "Blessing of Replenishment"
	/call WriteToIni "${SpellAliases_Ini},Druid,CoS" "Circle of Seasons"
	/call WriteToIni "${SpellAliases_Ini},Druid,Str" "Nature's Might"
	/call WriteToIni "${SpellAliases_Ini},Druid,GroupWolf" "Share Form of the Great Wolf"
	/call WriteToIni "${SpellAliases_Ini},Druid,GroupInvis" "Foliage Shield"
	/call WriteToIni "${SpellAliases_Ini},Druid,Evac" "Succor"
	
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortNK" "Circle of Karana"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortLavastorm" "Circle of Lavastorm"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortSRo" "Circle of Ro"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortMisty" "Circle of Misty"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortFeerrott" "Circle of Feerrott"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortCommons" "Circle of Commons"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortEK" "Succor: East"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortSureFall" "Circle of Surefall Glade"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortButcher" "Circle of Butcher"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortSteamfont" "Circle of Steamfont"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortSkyfire" "Wind of the North"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortEJ" "Wind of the South"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortDawnshroud" "Circle of Dawnshroud"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortDSP" "Circle of Dawnshroud"	
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortTwilight" "Circle of Twilight"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortGrimling" "Circle of Grimling"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortNexus" "Circle of the Nexus"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortStonebrunt" "Circle of Stonebrunt"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortTox" "Circle of Toxxulia"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortBarindu" "Circle of Barindu"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortNatimbi" "Circle of Natimbi"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortPoK" "Circle of Knowledge"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortKnowledge" "Circle of Knowledge"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortCS" "Circle of Cobalt Scar"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortWL" "Circle of Wakening Lands"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortGD" "Circle of Great Divide"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortCombines" "Circle of the Combines"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortDreadlands" "Circle of the Combines"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortDL" "Circle of the Combines"

|-------------------------------------------------------------------------------------Enchanter Aliases.	
	/call WriteToIni "${SpellAliases_Ini},Enchanter,Haste" "Vallon's Quickening"
	/call WriteToIni "${SpellAliases_Ini},Enchanter,VQ" "Vallon's Quickening"
	/call WriteToIni "${SpellAliases_Ini},Enchanter,VoQ" "Voice Of Quellious"
	/call WriteToIni "${SpellAliases_Ini},Enchanter,C5" "Voice Of Quellious"
	/call WriteToIni "${SpellAliases_Ini},Enchanter,KEI" "Koadic's Endless Intellect"
	/call WriteToIni "${SpellAliases_Ini},Enchanter,C4" "Koadic's Endless Intellect"
	/call WriteToIni "${SpellAliases_Ini},Enchanter,GoD" "Guard of Druzzil"
	/call WriteToIni "${SpellAliases_Ini},Enchanter,CHA" "Overwhelming Splendor"

	
|-------------------------------------------------------------------------------------Magician Aliases.
	/call WriteToIni "${SpellAliases_Ini},Magician,DS" "Maelstrom of Ro"
	/call WriteToIni "${SpellAliases_Ini},Magician,CoH" "Call of the Hero"

|-------------------------------------------------------------------------------------Necromancer Aliases.	
	/call WriteToIni "${SpellAliases_Ini},Necromancer,DMF" "Dead Men Floating"
	/call WriteToIni "${SpellAliases_Ini},Necromancer,IVU" "Invisibility versus Undead"
	/call WriteToIni "${SpellAliases_Ini},Necromancer,SummonCorpse" "Conjure Corpse"
	/call WriteToIni "${SpellAliases_Ini},Necromancer,Sac" "Sacrifice"
	/call WriteToIni "${SpellAliases_Ini},Necromancer,Bind" "Bind Affinity"

|-------------------------------------------------------------------------------------Paladin Aliases.	
	/call WriteToIni "${SpellAliases_Ini},Paladin,BSS" "Brell's Stalwart Shield"

|-------------------------------------------------------------------------------------Ranger Aliases.	
	/call WriteToIni "${SpellAliases_Ini},Ranger,SoTP" "Spirit of the Predator"
	/call WriteToIni "${SpellAliases_Ini},Ranger,Pred" "Spirit of the Predator"
	/call WriteToIni "${SpellAliases_Ini},Ranger,SoT" "Strength of Tunare"
	/call WriteToIni "${SpellAliases_Ini},Ranger,Tunare" "Strength of Tunare"
	/call WriteToIni "${SpellAliases_Ini},Ranger,Rathe" "Call of the Rathe"
	/call WriteToIni "${SpellAliases_Ini},Ranger,CoR" "Call of the Rathe"
	/call WriteToIni "${SpellAliases_Ini},Ranger,SoE" "Spirit of Eagle"	
	/call WriteToIni "${SpellAliases_Ini},Ranger,SoW" "Spirit of Wolf"
	/call WriteToIni "${SpellAliases_Ini},Ranger,DS" "Shield of Spikes"
	/call WriteToIni "${SpellAliases_Ini},Ranger,CoS" "Circle of Summer"	
	/call WriteToIni "${SpellAliases_Ini},Ranger,CoW" "Circle of Winter"	
	
|-------------------------------------------------------------------------------------Shadow Knight Aliases.
	/call WriteToIni "${SpellAliases_Ini},Shadow Knight,Voice" "Voice of Thule"
	/call WriteToIni "${SpellAliases_Ini},Shadow Knight,VoT" "Voice of Thule"
	/call WriteToIni "${SpellAliases_Ini},Shadow Knight,Thule" "Voice of Thule"
	
|------------------------------------------------------------------------------------Shaman Aliases.	
	/call WriteToIni "${SpellAliases_Ini},Shaman,HoT" "Breath of Trushar"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Heal" "Daluda's Mending"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Fo7" "Focus of the Seventh"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Focus" "Focus of Soul"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Haste" "Talisman of Celerity"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Billy" "Spirit of Bih`li"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Bih`li" "Spirit of Bih`li"
	/call WriteToIni "${SpellAliases_Ini},Shaman,SoB" "Spirit of Bih`li"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Regen" "Blessing of Replenishment"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Agi" "Talisman of the Wrulan"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Dex" "Talisman of the Raptor"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Sta" "Talisman of the Boar"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Str" "Talisman of the Diaku"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Tribunal" "Talisman of the Tribunal"
	/call WriteToIni "${SpellAliases_Ini},Shaman,ToT" "Talisman of the Tribunal"	
	/call WriteToIni "${SpellAliases_Ini},Shaman,Jag" "Spirit of the Jaguar"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Shrink" "Shrink"
	/call WriteToIni "${SpellAliases_Ini},Shaman,GroupShrink" "Tiny Terror"
	/call WriteToIni "${SpellAliases_Ini},Shaman,GShrink" "Tiny Terror"
	/call WriteToIni "${SpellAliases_Ini},Shaman,RGC" "Remove Greater Curse"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Cha" "Unfailing Reverence"

|-------------------------------------------------------------------------------------Wizard Aliases.	
	/call WriteToIni "${SpellAliases_Ini},Wizard,Decess" "Decession"
	/call WriteToIni "${SpellAliases_Ini},Wizard,Evac" "Decession"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TL" "Teleport"
	
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortWK" "West Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortNRo" "Ro Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortCommons" "Common Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortCazic" "Cazic Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortNek" "Nek Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortNektulos" "Nek Portal"	
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortNK" "North Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortGfay" "Fay Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortEJ" "Markar's Relocation"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortEmerald" "Markar's Relocation"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportSkyfire" "Tishan's Relocation"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportDL" "Combine Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportDreadlands" "Combine Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportCombines" "Combine Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportDawnshroud" "Dawnshroud Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportDSP" "Dawnshroud Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportTwilight" "Twilight Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportGrimling" "Grimling Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportNexus" "Nexus Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GroupTL" "Translocate: Group"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportTx" "Tox Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportBarindu" "Barindu Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportNatimbi" "Natimbi Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportPoK" "Knowledge Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportKnowledge" "Knowledge Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortSky" "Alter Plane: Sky"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortHate" "Alter Plane: Hate"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortCS" "Cobalt Scar Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortWL" "Wakening Lands Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortGD" "Great Divide Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortIceclade" "Iceclad Portal"
	
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLDawnshroud" "Translocate: Dawnshroud"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLCazic" "Translocate: Cazic"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLNRo" "Translocate: Ro"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLWC" "Translocate: West"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLNek" "Translocate: Nek"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLCommon" "Translocate: Common"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLFay" "Translocate: Fay"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLCombine" "Translocate: Combine"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLTwilight" "Translocate: Twilight"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLGrimling" "Translocate: Grimling"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLNexus" "Translocate: Nexus"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLGroup" "Translocate"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLTox" "Translocate: Tox"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLNatimbi" "Translocate: Natimbi"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLBarindu" "Translocate: Barindu"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLPoK" "Translocate: Knowledge"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLCS" "Translocate: Cobalt Scar"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLWL" "Translocate: Wakening Lands"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLGD" "Translocate: Great Divide"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLIceclad" "Translocate: Iceclad"
/RETURN