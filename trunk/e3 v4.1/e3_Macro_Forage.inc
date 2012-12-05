|------------------------------------------------------------------------|
|- e3_Macro_Forage.inc v1.0	( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Auto loots corpses of mobs you have killed.							-|
|------------------------------------------------------------------------|



SUB AutoForage
/if (${Debug}) /echo |- AutoForage ==>

	/if (!${Cursor.ID} && ${Me.FreeInventory}) {
		/if (${Me.AbilityReady[Forage]} && !${Me.Combat} && !${Me.Mount.ID}) {
			/if (${Me.State.NotEqual[STAND]}) {
				/declare flagSit bool local
				/stand
			}
			/doability forage
			/delay 10 ${Cursor.ID}
			/if (${Cursor.ID}) /call handleForagedItem
		}	
	}

/if (${Debug}) /echo <== AutoForage -|
/RETURN



SUB handleForagedItem
/if (${Debug}) /echo |- handleForagedItem ==>
	
	/delay 2
	/declare iniEntryVariables string local
	/varset iniEntryVariables ${If[${Math.Calc[${Cursor.Value}/1000].Int},~${Math.Calc[${Cursor.Value}/1000].Int}p,${If[${Math.Calc[${Cursor.Value}/100].Int},~${Math.Calc[${Cursor.Value}/100].Int}g,${If[${Math.Calc[${Cursor.Value}/10].Int},~${Math.Calc[${Cursor.Value}/10].Int}s,${If[${Math.Calc[${Cursor.Value}/1].Int},~${Math.Calc[${Cursor.Value}/1].Int}c,]}]}]}]}${If[${Cursor.Stackable},(${Cursor.StackSize}),]}${If[${Cursor.NoDrop},(ND),]}${If[${Cursor.Container},(C),]}
	/if (!${Ini[${Forage_Ini},${Cursor.Name.Left[1]},${Cursor}${iniEntryVariables}].Length}) {
		/call WriteToIni "${Forage_Ini},${Cursor.Name.Left[1]},${Cursor}${iniEntryVariables}" ${If[${Cursor.Container},Container,Keep${If[${Cursor.Stackable},/${Cursor.StackSize},]}]}
		/echo AutoForage: Adding [${Cursor} (${Ini[${Forage_Ini},${Cursor.Name.Left[1]},${Cursor}${iniEntryVariables}]})]
	}
	
	:checkitem
	/if (${Debug}) /echo |- handleForagedItem -| :checkitem
	
	/if (${Ini[${Forage_Ini},${Cursor.Name.Left[1]},${Cursor}${iniEntryVariables}].Equal[Destroy]}) {
		/echo AutoForage: Destroying [${Cursor}]
		:Overstock
		/destroy
	} else {
		/if (${Ini[${Forage_Ini},${Cursor.Name.Left[1]},${Cursor}${iniEntryVariables}].Arg[2,/].Length} && ${Math.Calc[${FindItemCount[${Cursor}]} + 1].Int} > ${Ini[${Forage_Ini},${Cursor.Name.Left[1]},${Cursor}${iniEntryVariables}].Arg[2,/]}) {
			/echo AutoForage: Fully stocked on [${Cursor}], destroying.
			/goto :Overstock
		} else {
			/call clearCursor
		}
	}
	
/if (${Debug}) /echo <== handleForagedItem -|
/RETURN



|----------------------------|
|- Setup forage variables.	-|
|----------------------------|
SUB setup_AutoForage
/if (${Debug}) /echo |- setup_Forage ==>
	
	/call IniToVar "${Character_Ini},${Me.Class},Auto-Forage (On/Off)" autoForage bool outer
	/if (${autoForage} && !${Ini[${Forage_Ini}].Length}) /call Build_Alphabetized_Ini ${Forage_Ini}
	
/if (${Debug}) /echo <== setup_Forage -|	
/RETURN