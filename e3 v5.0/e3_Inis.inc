|--------------------------------------------------------------------|
|- e3_Inis.inc v1.0	( originally designed to work with e3.mac )		-|
|- Written by Killians of PEQ										-|
|--------------------------------------------------------------------|
|- A universal places to /call ini updates from various includes.	-|
|--------------------------------------------------------------------|



|----------------------------------------------------------------------------------|
|- Generates a file where the user can designate folders for Macro, and Bot inis. -|
|----------------------------------------------------------------------------------|
SUB make_FilePathsIni
/if (${Debug}) /echo |- make_FilePathsIni ==>

	/echo Generating [e3 File Paths.ini], which contains settings related to e3's folder structure, or, names of it's subsequent settings files.

	/noparse /ini "e3 File Paths.ini" "File Paths" "Bot Settings Files" "Bot Inis\${MacroQuest.Server}-${Me.CleanName}"
	/noparse /ini "e3 File Paths.ini" "File Paths" "Macro Settings File" "Macro Inis\e3 Settings.ini"
	/noparse /ini "e3 File Paths.ini" "File Paths" "Macro Data File" "e3 Data.ini"

/if (${Debug}) /echo <== make_FilePathsIni -|
/RETURN



|--------------------------------------------------------------------------------|
|- Generates a general Macro Settings file, to be used by multiple includes.	-|
|--------------------------------------------------------------------------------|
SUB make_MacroSettingsIni
/if (${Debug}) /echo |- make_CoreSettingsIni ==>

	/echo Generating [${Macro_Ini}], which contains universal settings for e3's preformance and functionality.

	/call WriteToIni "${Macro_Ini},General,Debug Mode (On/Off)" Off
	/call WriteToIni "${Macro_Ini},General,Macro Output Channel" /bc
	/call WriteToIni "${Macro_Ini},General,Verbosity (On/Off)" Off
	
	| ==>
	/call background_MacroSettings
	/call basics_MacroSettings
	/call assist_MacroSettings
	/call buffCheck_MacroSettings
	/call casting_MacroSettings
	/call heal_MacroSettings
	/call pet_MacroSettings
	| <==

	| Validate the file was created.
	/if (!${Ini[${Macro_Ini}].Length}) {
		/echo ERROR: Could not create [${Macro_Ini}].  Please check your designated file paths, and make sure you have the proper folder structure, then restart.
		/endmacro
	}

/if (${Debug}) /echo <== make_CoreSettingsIni -|
/RETURN



|--------------------------------------------------------------------------------|
|- Generates a general Macro Settings file, to be used by multiple includes.	-|
|--------------------------------------------------------------------------------|
SUB make_CharacterSettingsIni
/if (${Debug}) /echo |- make_CharacterSettingsIni ==>

	/echo Generating [${Character_Ini}], which contains settings that control this bot's behavior.
	
	| ==>
	/call background_CharacterSettings
	/call basics_CharacterSettings
	/call buffCheck_CharacterSettings
	/call casting_CharacterSettings
	/call pet_CharacterSettings
	/call ${Me.Class.ShortName}_CharacterSettings
	/call heal_CharacterSettings
	/call assist_CharacterSettings
	| <==

	| Validate the file was created.
	/if (!${Ini[${Character_Ini}].Length}) {
		/echo ERROR: Could not create [${Character_Ini}].  Please check your designated file paths, and make sure you have the proper folder structure, then restart.
		/endmacro
	}

/if (${Debug}) /echo <== make_CharacterSettingsIni -|
/RETURN



|----------------------------------------------------------------------------|
|- Build an ini file, of a given name, that contains alphabetized sections.	-|
|----------------------------------------------------------------------------|
|- @param Ini_File: Name of the ini file you want to create.				-|
|----------------------------------------------------------------------------|
SUB Build_Alphabetized_Ini(Ini_File)
/if (${Debug}) /echo |- Build_Alphabetized_Ini ==>

	/ini "${Ini_File}" A "A is for"
	/ini "${Ini_File}" B "B is for"
	/ini "${Ini_File}" C "C is for"
	/ini "${Ini_File}" D "D is for"
	/ini "${Ini_File}" E "E is for"
	/ini "${Ini_File}" F "F is for"
	/ini "${Ini_File}" G "G is for"
	/ini "${Ini_File}" H "H is for"
	/ini "${Ini_File}" I "I is for"
	/ini "${Ini_File}" J "J is for"
	/ini "${Ini_File}" K "K is for"
	/ini "${Ini_File}" L "L is for"
	/ini "${Ini_File}" M "M is for"
	/ini "${Ini_File}" N "N is for"
	/ini "${Ini_File}" O "O is for"
	/ini "${Ini_File}" P "P is for"
	/ini "${Ini_File}" Q "Q is for"
	/ini "${Ini_File}" R "R is for"
	/ini "${Ini_File}" S "S is for"
	/ini "${Ini_File}" T "T is for"
	/ini "${Ini_File}" U "U is for"
	/ini "${Ini_File}" V "V is for"
	/ini "${Ini_File}" W "W is for"
	/ini "${Ini_File}" X "X is for"
	/ini "${Ini_File}" Y "Y is for"
	/ini "${Ini_File}" Z "Z is for"
  
/if (${Debug}) /echo |- Build_Alphabetized_Ini ==>
/RETURN