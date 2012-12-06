|----------------------------------------------------------------------------|
|- e3_Classes_Warrior.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains warrior specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Warrior's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the warrior class dynamic.	-|
|--------------------------------------------------------------------|
Sub WAR_Main
/if (${Debug}) /echo |- WAR_Main ==>

|-----------------------------------------------------------------------------------------------------------------------------------#1 check_CombatBuffs.
	/if (!${idle} && ${CombatBuffs.Size}) /call CombatBuffs
		/if (${ActionTaken}) /return
		
|------------------------------------------------------------------------------------------------------------------------------------#2 Assist Functions.
	/if (${Assisting}) /call WAR_Assist

/if (${Debug}) /echo <== WAR_Main -|
/return



|--------------------------------------------------------------------|
|- Warrior's Class specific Assist Loop.							-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the warrior class dynamic.	-|
|--------------------------------------------------------------------|
Sub WAR_Assist
/if (${Debug}) /echo |- WAR_Assist ==>

|-----------------------------------------------------------------------#1 Check Asssit status.
	/call check_AssistStatus

|--------------------------------------------------------------------------------#2 QuickBurns.
	/if (${use_QBurn}) /call QuickBurns
	
|---------------------------------------------------------------------------------#3 FullBurns.	
	/if (${use_FBurn}) /call FullBurns

|--------------------------------------------------------------------------#4 Combat Abilities.	
	/if (${MyAbilities.Size}) /call CombatAbilities
	
/if (${Debug}) /echo <== WAR_Assist -|	
/return



|------------------------------------------------------------------------|
|- WAR_Setup															-|
|------------------------------------------------------------------------|
|- Imports variables from the [Warrior] section of the Character_Ini.	-|
|------------------------------------------------------------------------|
Sub WAR_Setup
/if (${Debug}) /echo |- WAR_Setup ==>

	| /if (!${Ini[${Character_Ini},${Me.Class}].Length}) /call Make${Me.Class.ShortName}_Ini

	|---------------------------------------------------------------------------------Check MyAbilities for an Bellow type disc.
	/if (${MyAbilities.Size}) {
		/declare i int local
		
		/for i 1 to ${MyAbilities.Size}

		|-----------------------------------------------------------------------------------------Check for the best available 'Bellow' to use.
			/if (${MyAbilities[${i}].Arg[1,/].Equal[Ancient: Chaos Cry]}) {
				/declare BellowName string outer Ancient: Chaos Cry
				/declare MinBellowEndurance int outer ${Math.Calc[300*3]}
				/if (!${Defined[do_Taunt]}) /declare do_Taunt bool outer TRUE
			} else /if (${MyAbilities[${i}].Arg[1,/].Equal[Bellow of the Mastruq]}) {
				/declare BellowName string outer Bellow of the Mastruq
				/declare MinBellowEndurance int outer ${Math.Calc[275*3]}
				/if (!${Defined[do_Taunt]}) /declare do_Taunt bool outer TRUE
			} else /if (${MyAbilities[${i}].Arg[1,/].Equal[Incite]}) {
				/declare BellowName string outer Incite
				/declare MinBellowEndurance int outer ${Math.Calc[250*3]}
				/if (!${Defined[do_Taunt]}) /declare do_Taunt bool outer TRUE
			} else /if (${MyAbilities[${i}].Arg[1,/].Equal[Berate]}) {
				/declare BellowName string outer Berate
				/declare MinBellowEndurance int outer ${Math.Calc[200*3]}
				/if (!${Defined[do_Taunt]}) /declare do_Taunt bool outer TRUE
			} else /if (${MyAbilities[${i}].Arg[1,/].Equal[Bellow]}) {
				/declare BellowName string outer Bellow
				/declare MinBellowEndurance int outer ${Math.Calc[175*3]}
				/if (!${Defined[do_Taunt]}) /declare do_Taunt bool outer TRUE
			} else /if (${MyAbilities[${i}].Arg[1,/].Equal[Provoke]}) {
				/declare BellowName string outer Provoke
				/declare MinBellowEndurance int outer ${Math.Calc[85*3]}
				/if (!${Defined[do_Taunt]}) /declare do_Taunt bool outer TRUE
			}
			
		/next i
	}
	
/if (${Debug}) /echo <== WAR_Setup -|
/return


|-------------------------------------------------------------------------------------------------Auto_Bellow
	/call IniToVar "${Character_Ini},${Me.Class},Auto_Bellow(On/Off)" UNDEFINED bool local
	/if (${Macro.Return}) {
	
	|-----------------------------------------------------------------------------------------Check for the best available 'Bellow' to use.
		/if (${Me.CombatAbilityReady[Ancient: Chaos Cry]}) {
			/declare BellowName string outer Ancient: Chaos Cry
			/declare MinBellowEndurance int outer ${Math.Calc[300*3]}
		} else /if (${Me.CombatAbilityReady[Bellow of the Mastruq]}) {
			/declare BellowName string outer Bellow of the Mastruq
			/declare MinBellowEndurance int outer ${Math.Calc[275*3]}
		} else /if (${Me.CombatAbilityReady[Incite]}) {
			/declare BellowName string outer Incite
			/declare MinBellowEndurance int outer ${Math.Calc[250*3]}
		} else /if (${Me.CombatAbilityReady[Berate]}) {
			/declare BellowName string outer Berate
			/declare MinBellowEndurance int outer ${Math.Calc[200*3]}
		} else /if (${Me.CombatAbilityReady[Bellow]}) {
			/declare BellowName string outer Bellow
			/declare MinBellowEndurance int outer ${Math.Calc[175*3]}
		} else /if (${Me.CombatAbilityReady[Provoke]}) {
			/declare BellowName string outer Provoke
			/declare MinBellowEndurance int outer ${Math.Calc[85*3]}
		}
		
		/if (!${Defined[BellowName]}) {
			/echo ERROR: @[Auto_Bellow] - No bellow type disc dectected, disabling.
		} else {
			/declare do_Taunt bool outer TRUE
			/call BuildArray MyAbilities "${BellowName}/disc"
		}
	}


create do_Taunt toggles

|--------------------------------------------------------------------|
|- MakeWAR_Ini														-|
|--------------------------------------------------------------------|
|- Writes entries to the [Warrior] section of the Character_Ini.	-|
|--------------------------------------------------------------------|
Sub MakeWAR_Ini
/if (${Debug}) /echo |- WAR_Ini ==>


	
/if (${Debug}) /echo <== WAR_Ini -|
/return



|------------------------------------------------------------------------|
|- WAR_Background_Events												-|
|------------------------------------------------------------------------|
|- Events to add to the Background_Events, that are warrior specific.	-|
|------------------------------------------------------------------------|
Sub WAR_Background_Events

/return



|--------------------------
|- Attempt_Bellow
|-----------------------
SUB Attempt_Bellow
/if (${Debug}) /echo |- Attempt_Bellow ==>

	/if (!${Me.CombatAbilityReady[${BellowName}]}) {
		/if (${Debug}) /echo |- Attempt_Bellow -| [${BellowName}] is not ready.
	} else {
		/if (${Me.Endurance} < ${MinBellowEndurance}) {
			/if (${Debug}) /echo |- Attempt_Bellow -| I do not have enough endurance to use [${BellowName}].
		} else {
			/if (${bellowTimer} || ${disc_Timer_${BellowName.Replace[ ,]}}) {
				/if (${Debug}) /echo |- Attempt_Bellow -| I have not met my recast timer for [${BellowName}].
			} else {
				/call useDisc "${BellowName}"
				/call CreateTimer bellowTimer 600
			}
		}
	}
	
/if (${Debug}) /echo <== Attempt_Bellow -|
/RETURN