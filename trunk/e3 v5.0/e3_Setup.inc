
|--------------------------------------------------------------------|
|- e3_Macro_Setup.inc ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ										-|
|--------------------------------------------------------------------|
|- Contains setup functions for e3.mac								-|
|--------------------------------------------------------------------|

| ----------------------------------------------------------------------------------------------------------------------e3 Core Includes. ==>
#include e3_Aliases.inc
#include e3_Assists.inc
#include e3_Background.inc
#include e3_Basics.inc
#include e3_BuffCheck.inc
#include e3_Casting.inc
#include e3_Functions.inc
#include e3_Heals.inc
#include e3_Help.inc
#include e3_Inis.inc
#include e3_Pets.inc

#include e3_Classes_Bard.inc
#include e3_Classes_Beastlord.inc
#include e3_Classes_Berserker.inc
#include e3_Classes_Cleric.inc
#include e3_Classes_Druid.inc
#include e3_Classes_Enchanter.inc
#include e3_Classes_Magician.inc
#include e3_Classes_Monk.inc
#include e3_Classes_Necromancer.inc
#include e3_Classes_Paladin.inc
#include e3_Classes_Ranger.inc
#include e3_Classes_Rogue.inc
#include e3_Classes_ShadowKnight.inc
#include e3_Classes_Shaman.inc
#include e3_Classes_Warrior.inc
#include e3_Classes_Wizard.inc
| ----------------------------------------------------------------------------------------------------------------------e3 Core Includes. ==>

#include e3_BegForBuffs.inc
#include VetAAs.inc

SUB e3_Initialize(modeSelect)

/call CheckPlugins

/echo Initializing...

| ------------------------------------------------------------------------Create a file to define file structure in the Macros folder.
	/if (!${Ini[e3 File Paths.ini].Length}) {
		/echo Welcome to e3, preforming first time setup...
		/call make_FilePathsIni
	}
| ---------------------------------------------------------------------------------------------------------Create core settings files.
	/if (!${Ini[e3 File Paths.ini,File Paths,Macro Settings File].Length}) {
		/echo ERROR: Could not find designated file path for [Macro Settings File], please review [e3 File Paths.ini] and restart.
		/endmacro
	} else {
		/declare Macro_Ini string outer ${Ini[e3 File Paths.ini,File Paths,Macro Settings File]}
	}

| -----------------------------------------------------------------------------------------------------------------Generate Macro_Ini.
	/if (!${Ini[${Macro_Ini}].Length}) {
		/call EVENT_updateAliases
		/call make_MacroSettingsIni
	}
| -----------------------------------------------------------------------------------------------------Import Character_Ini file path.
	/if (!${Ini[e3 File Paths.ini,File Paths,Bot Settings Files].Length}) {
		/echo ERROR: Could not find designated file path for [Bot Settings Files], please review [e3 File Paths.ini] and restart.
		/endmacro
	} else {
		/declare Character_Ini string outer ${Ini[e3 File Paths.ini,File Paths,Bot Settings Files]}
	}
	
| -----------------------------------------------------------------------------------------------------Import Macro Data file path.
	/if (!${Ini[e3 File Paths.ini,File Paths,Macro Data File].Length}) {
		/echo ERROR: Could not find designated file path for [Macro Data File], please review [e3 File Paths.ini] and restart.
		/endmacro
	} else {
		/declare MacroData_Ini string outer ${Ini[e3 File Paths.ini,File Paths,Macro Data File]}
	}

| -------------------------------------------------------------------------------------------------------------Generate Character_Ini.
	/if (!${Ini[${Character_Ini}].Length}) /call make_CharacterSettingsIni
	
| ---------------------------------------------------------------------------------------------------------------Import core settings.
	/if (${Ini[${Macro_Ini},General,Debug Mode (On/Off)].Length}) /call INItoVar "${Macro_Ini},General,Debug Mode (On/Off)" Debug bool outer

	/declare ChatToggle string outer /bc
	/if (${Ini[${Macro_Ini},General,Macro Output Channel].Length}) /call IniToVar "${Macro_Ini},General,Macro Output Channel" ChatToggle string outer
	
	/if (${Ini[${Macro_Ini},General,Verbosity (On/Off)].Length}) /call IniToVar "${Macro_Ini},General,Verbosity (On/Off)" Verbosity bool outer
	
| ---------------------------------------------------------------------------------------------------------------Setup core functions.
	
	/echo Setting up includes...
	
	/call assist_Setup
	/call background_Setup
	/call basics_Setup
	/call buffCheck_Setup
	/call casting_Setup
	/call heal_Setup
	/call pet_Setup
	/call ${Me.Class.ShortName}_Setup
	/call setup_BegForBuffs
	
	/if (${Ini[${MacroData_Ini},PendingExchanges-${MacroQuest.Server},${Me.CleanName}].Length} && ${Ini[${MacroData_Ini},PendingExchanges-${MacroQuest.Server},${Me.CleanName}].NotEqual[NONE]} && !${itemExchange_Timer}) {
		/echo Completing a pending exchange, [${Ini[${MacroData_Ini},PendingExchanges-${MacroQuest.Server},${Me.CleanName}].Arg[1,/]}] to [${Ini[${MacroData_Ini},PendingExchanges-${MacroQuest.Server},${Me.CleanName}].Arg[2,/]}]...
		/call completePendingExchange
	}
	
	/echo ** If you like my macro, give me money!  Visit 'http://code.google.com/p/e3-macro-builds/' where you can find the donations link, and issues page to post bugs or suggestions.
	/if (${NetBots.Client.Find[Tackleberry]}) /echo Though you may find this message a bit 'extreme' so is bitching about a few lines of text which can easily be ignored.
	/echo e3 loaded.
	
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


|**
#EVENT e3Toggle "[MQ2] Toggle #1# #2#"
SUB EVENT_e3Toggle(line, varToSet, varSetting)
/if (${Debug}) /echo |- EVENT_e3Toggle ==>

	

/if (${Debug}) /echo <== EVENT_e3Toggle -|
/RETURN
**|


|----------------------------------------------------|
|- Checks for and enables required macro plugins.	-|
|----------------------------------------------------|
SUB CheckPlugins
/echo Checking for required plugins...

| REQUIRED PLUGINS ==>

| -----------------------------------------------------------------------------------------------------------------------------------Check MQ2EQBC.
	/if (!${Plugin[MQ2EQBC].Name.Length}) {
		/echo Plugin MQ2EQBC is not loaded, attempting to resolve...
		/plugin MQ2EQBC
		/delay 10s !${Select[${EQBC},NULL]}
		/if (${Select[${EQBC},NULL]}) {
			/echo ***WARNING*** Could not load MQ2EQBC, macro functionality may be limited.
			/echo Starting in Solo Mode...
		}
	}

|-------------------------------------------------------------------------------------------Auto-Connect to EQBCS.
	/if (${Bool[${EQBC}]}) {
		/if (!${EQBC.Connected}) {
			/bccmd connect
			/delay 100 ${EQBC.Connected}
			/delay 5
			/if (!${EQBC.Connected}) /echo ***WARNING*** Could not connect to EQBCS, please open EQBCS and try again.  Macro functionality may be limited...
		}


	| --------------------------------------------------------------------------------------------------------------------------------Check MQ2NetBots.
		/if (!${Plugin[MQ2NetBots].Name.Length}) {
			/echo Plugin MQ2NetBots is not loaded, attempting to resolve...
			/plugin MQ2NetBots
			/delay 3s ${NetBots}
			/if (!${NetBots}) {
				/echo ***WARNING*** Could not load MQ2NetBots, macro functionality may be limited.
			}
		}
		
	|-------------------------------------------------------------------------------------------Auto-Connect to NetBots.
		/if (${NetBots}) {
			/squelch /netbots on grab=on send=on
		}
		
	| -----------------------------------------------------------------------------------------------------------------------------Check MQ2NetAdvPath.
		/if (!${Plugin[MQ2NetAdvPath].Name.Length}) {
			/echo Plugin MQ2NetAdvPath is not loaded, attempting to resolve...
			/plugin MQ2NetAdvPath
			/delay 3s ${NetAdvPath}
			/if (!${NetAdvPath}) {
				/echo ***WARNING*** Could not load MQ2NetAdvPath, please ensure you're using a copy of MQ2 which was compiled with the MQ2NetAdvPath plugin and try again.
				/echo Attempting to load MQ2MoveUtils
			}
		}
	}
	
| ------------------------------------------------------------------------------------------------------------------------------Check MQ2MoveUtils.
	/if (!${Plugin[MQ2MoveUtils].Name.Length}) {
		/echo Plugin MQ2MoveUtils is not loaded, attempting to resolve...
		/plugin MQ2MoveUtils
		/delay 3s !${Select[${Stick.Status},NULL]}
		/if (${Select[${Stick.Status},NULL]}) {
			/echo ***WARNING*** Could not load MQ2MoveUtils, macro functionality may be limited.
			/if (!${NetAdvPath}) {
				/echo Follow and Assist stick DISABLED.
			} else {
				/echo Assist and NPC stick DISABLED.
				/echo Follow restricted to NetBots.
			}
		}
	}	
		
| -------------------------------------------------------------------------------------------------------------------------------Check MQ2Exchange.
	/if (!${Plugin[MQ2Exchange].Name.Length}) {
		/echo Plugin MQ2Exchange is not loaded, attempting to resolve...
		/plugin MQ2Exchange
		/delay 3s ${Plugin[MQ2Exchange].Name.Length}
		/if (!${Plugin[MQ2Exchange].Name.Length}) {
			/echo ***WARNING*** Could not load MQ2Exchange, macro functionality may be limited.
			/echo Item swapping is DISABLED.
		}
	}

| ----------------------------------------------------------------------------------------------------------------------------------Check MQ2Twist.
	/if (${Select[${Me.Class.ShortName},BRD]}) {
		/if (!${Plugin[MQ2Twist].Name.Length}) {
			/echo Plugin Unable to detect MQ2Twist, attempting to resolve...
			/plugin MQ2Twist
			/delay 3s !${Select[${Twist},NULL]}
			/if (${Select[${Twist},NULL]}) {
				/echo ***WARNING*** Could not load MQ2Twist, macro functionality may be limited.
				/echo Bard melodies DISABLED.
			}
		}	
	}

| <== REQUIRED PLUGINS

| ----------------------------------------------------------------------------------------------------------------------------Check MQ2ItemDisplay.
	/if (!${Plugin[mq2itemdisplay].Name.Length}) /plugin MQ2ItemDisplay

| ------------------------------------------------------------------------------------------------------------------------------------Check MQ2HUD.

	/if (!${Plugin[mq2hud].Name.Length}) /plugin MQ2HUD

/RETURN