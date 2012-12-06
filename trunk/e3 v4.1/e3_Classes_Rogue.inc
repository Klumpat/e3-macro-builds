|----------------------------------------------------------------------------|
|- e3_Classes_Rogue.inc v2.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ												-|
|----------------------------------------------------------------------------|
|- Contains rogue specific functions related to,							-|
|-		Setup and Ini entry information										-|
|-		MainLoop, and AssistLoop hooks										-|
|-		Over-all functionality / flow										-|
|----------------------------------------------------------------------------|



|--------------------------------------------------------------------|
|- Rogue's Class specific mainLoop hook.							-|
|--------------------------------------------------------------------|
|- Prioritizes macro functions based on the rogue class dynamic.	-|
|--------------------------------------------------------------------|
Sub ROG_Main
/if (${Debug}) /echo |- ROG_Main ==>

|-----------------------------------------------------------------------------------------------------------------------------------#1 check_CombatBuffs.
	/if (!${idle} && ${CombatBuffs.Size}) /call CombatBuffs
		/if (${ActionTaken}) /return
		
|------------------------------------------------------------------------------------------------------------------------------------#2 Assist Functions.
	/if (${Assisting}) /call ROG_Assist
		/if (${ActionTaken}) /return
		
|-------------------------------------------------------------------------------------------------------------------------------------------#3 Auto Hide.
	/if (${AutoHide} && !${Me.Combat} && !${Assisting} && !${Me.Invis} && !${Me.Moving}) /call AutoHide

|-----------------------------------------------------------------------------------------------------------------------------------------#4 Auto Forage.
	/if (${autoForage}) /call AutoForage
	
/if (${Debug}) /echo <== ROG_Main -|
/return



|--------------------------------------------------------------------|
|- Rogue's Class specific Assist Loop.								-|
|--------------------------------------------------------------------|
|- Prioritizes Assist functions based on the rogue class dynamic.	-|
|--------------------------------------------------------------------|
Sub ROG_Assist
/if (${Debug}) /echo |- ROG_Assist ==>

|-----------------------------------------------------------------------#1 Check Asssit status.
	/call check_AssistStatus

|--------------------------------------------------------------------------------#2 QuickBurns.
	/if (${use_QBurn}) /call QuickBurns
	
|---------------------------------------------------------------------------------#3 FullBurns.	
	/if (${use_FBurn}) /call FullBurns

|--------------------------------------------------------------------------#4 Combat Abilities.	
	/if (${MyAbilities.Size}) /call CombatAbilities

/if (${Debug}) /echo <== ROG_Assist -|	
/return



|--------------------------------------------------------------------|
|- ROG_Setup														-|
|--------------------------------------------------------------------|
|- Imports variables from the [Rogue] section of the Character_Ini.	-|
|--------------------------------------------------------------------|
Sub ROG_Setup
/if (${Debug}) /echo |- ROG_Setup ==>

	/if (!${Ini[${Character_Ini},${Me.Class}].Length}) /call Make${Me.Class.ShortName}_Ini

	/call INItoVar "${Character_Ini},Rogue,Auto-Hide (On/Off)" AutoHide bool outer
	
|---------------------------------------------------------------------------------Check MyAbilities for an Assassin's Strike type disc.
	/if (${MyAbilities.Size}) {
		/declare i int local
		
		/for i 1 to ${MyAbilities.Size}
		|-----------------------------------------------------------------------------------------Check for the best available 'Assassin's Strike' to use.
			/if (${MyAbilities[${i}].Arg[1,/].Equal[Ancient: Chaos Strike]}) {
				/declare StrikeName string outer Ancient: Chaos Strike
				/declare MinStrikeEndurance int outer 575
			} else /if (${MyAbilities[${i}].Arg[1,/].Equal[Kyv Strike]}) {
				/declare StrikeName string outer Kyv Strike
				/declare MinStrikeEndurance int outer 550
			} else /if (${MyAbilities[${i}].Arg[1,/].Equal[Assassin's Strike]}) {
				/declare StrikeName string outer Assassin's Strike
				/declare MinStrikeEndurance int outer 515
			} else /if (${MyAbilities[${i}].Arg[1,/].Equal[Thief's Vengeance]}) {
				/declare StrikeName string outer Thief's Vengeance
				/declare MinStrikeEndurance int outer 425
			}	
		/next i
	}

/if (${Debug}) /echo <== ROG_Setup -|	
/return



|----------------------------------------------------------------|
|- MakeROG_Ini													-|
|----------------------------------------------------------------|
|- Writes entries to the [Rogue] section of the Character_Ini.	-|
|----------------------------------------------------------------|
Sub MakeROG_Ini
/if (${Debug}) /echo |- ROG_Ini ==>

	/call WriteToIni "${Character_Ini},${Me.Class},Auto-Hide (On/Off)" "Do you want this character to automatically sneak / hide whenever you're not in combat?" 0
	/call WriteToIni "${Character_Ini},${Me.Class},Auto-Forage (On/Off)" "*Do you want this character to automatically forage?" 0
	
/if (${Debug}) /echo <== ROG_Ini -|
/return



|------------------------------------------------------------------------|
|- ROG_Background_Events												-|
|------------------------------------------------------------------------|
|- Events to add to the Background_Events, that are rogue specific.	-|
|------------------------------------------------------------------------|
Sub ROG_Background_Events

/return



|--------------------------------------------------------------------------------|
|- Attempt_Backstab																-|
|--------------------------------------------------------------------------------|
|- Attempts to arm 'Assassin's Strike' type disciplines before using backstab.	-|
|--------------------------------------------------------------------------------|
Sub Attempt_Backstab
/if (${Debug}) /echo |- Attempt_Backstab ==>

	/if (${Me.Inventory[Mainhand].Type.NotEqual[Piercing]}) {
		/if (${Debug}) /echo |- Attempt_Backstab -| I need a piercing weapon in my main hand to backstab...
	} else {
	
	|-------------------------------------------------------------------------If I have an 'Assassin's Strike' type disc, and it is ready to be used.
		/if (${Defined[StrikeName]} && ${Me.Endurance} > ${MinStrikeEndurance} && !${disc_Timer_${StrikeName.Replace[ ,]}} && ${Me.AbilityReady[Hide]}) {
	
		|----------------------------------------------Stop moving.
			/if (${Me.Moving}) {
				/squelch /stick pause
				/delay 2s !${Me.Moving}
			}
			
		|------------------------------------------Turn off combat.
			/if (${Me.Combat}) {
				/attack off
				/delay 2s !${Me.Combat}
			}
			
		|-----------------------------------------------------Hide.
			/doability Hide
			/delay 3s ${Me.Invis}
			
		|-------------------------Use 'Assassin's Strike' type disc.
			/if (${Me.Invis}) {
				/call useDisc "${StrikeName}"
				/attack on
				/squelch /stick unpause
			}
		}
	|-------------------------------------------------------------------------------------------------------------------If backstab is ready, use it.
		/if (${Me.AbilityReady[Backstab]}) /doability Backstab
	}

/if (${Debug}) /echo <== Attempt_Backstab -|
/return



|------------------------------------------------------------|
|- AutoHide
|------------------------------------------------------------|
|- Automatically engages sneak and hide when not in combat.	-|
|------------------------------------------------------------|
sub AutoHide
/if (${Debug}) /echo |- AutoHide ==>

|----------------------------------------------------------------------Engage sneak.
	/if (!${Me.Sneaking} && ${Me.AbilityReady[Sneak]}) {
		/doability Sneak
		/delay 2s ${Me.Sneaking}
	}
	
|-----------------------------------------------------------------------Engage hide.
	/if (${Me.Sneaking} && ${Me.AbilityReady[Hide]}) {
		/doability Hide
		/delay 2s ${Me.Invis}
	}

/if (${Debug}) /echo <== AutoHide -|
/return