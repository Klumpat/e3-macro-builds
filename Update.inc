|----------------------------------------------------------------------------|
|- e3_Macro_Update.inc 2.0( originally designed to work with e3.mac )		-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|----------------------------------------------------------------------------|
|- Contains information and functions to update an older version of e3.mac.	-|
|----------------------------------------------------------------------------|



|------------------------------------------------------------------------------------------------------------|
|- Compares version listed in the Macro_Ini with the version that is being run, and updates accordingly.	-|
|------------------------------------------------------------------------------------------------------------|
|- @param version: e3_Version listed in e3.mac																-|
|------------------------------------------------------------------------------------------------------------|
SUB Update_e3(version)
/call IniToVar ${Macro_Ini},Settings,e3_Version UNDEFINED string local
/declare old_macroEdition int local ${Int[${Macro.Return.Arg[1,.]}]}
/declare old_fileRevisions int local ${Int[${Macro.Return.Arg[2,.]}]}
/declare old_bugFixes int local ${Int[${Macro.Return.Arg[3,.]}]}
/declare old_newFeatures int local ${Int[${Macro.Return.Arg[4,.]}]}

/echo Detected new version, updating from, v${Macro.Return} to v${version}...

/call MakeMacro_Ini
/call MakeCharacter_Ini
/call Update_Aliases
|----------------------------------------------------------------------------------------------------------------------------------------------------|
|-----------------------------------------------------------------------------------------------------------------Add new update information here.	-|
|----------------------------------------------------------------------------------------------------------------------------------------------------|

	|------------------------------------------------------------------------ Updates for revision 4.1	-|
		/if (${old_macroEdition} < 4 || ${old_fileRevisions} < 1) {
			
			/squelch /alias /clickit delete
			/squelch /alias /runthruzone delete
|**			
	e3 v4.0 beta with all current revisions up to this point.

Things that have changed,

Included changes from previously posted .inc files, including Loot, Cleric, Shaman, and AutoSell functions.

Added,

e3_Macro_SmoothMoves.inc -- A route running script.  Open this file, and look at the top, for instructions on how to use it.

e3_Macro_Forage.inc -- Fixed AutoForage functions.

e3_Macro_AutoSell.inc -- This is a companion file for e3_Macro_Loot.inc.  You can add the parameter /Sell, to the end of any of your loot.ini entries.  Use /bc AutoSell, to sell items to the nearest merchant.

e3_Macro_Combine.inc -- An auto tradeskill combine macro.  You must have the tradeskill window open, with a recipe selected.  Use, /bc Combine to combine a selected recipe until you run out of ingredients.


Still to come, 

CH Chain

Ranged attack options for rangers / zerkers etc
**|
		}
	|----------------------------------------------------------------------------------------------------|

|----------------------------------------------------------------------------------------------------------------------------------------------------|
|----------------------------------------------------------------------------------------------------------------------------------------------------|
|----------------------------------------------------------------------------------------------------------------------------------------------------|

/call WriteToIni ${Macro_Ini},Settings,e3_Version ${version} 1
/declare e3_Updated bool outer TRUE

/echo
/echo e3 has been updated! You may need to edit the [${Character_Ini}] file, and restart MQ2 before all functions are available.  Scroll up to to see what's been changed since your last update.
/RETURN