|------------------------------------------------------------------------|
|- e3_Macro_Background.inc	( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains background functions that update e3 status and variables,	-|
|- along with many minimally invasive macro functions.					-|
|------------------------------------------------------------------------| 

| finish auto destroy stuff
| Create toggles for various functions to be listed in either e3_Settings, or e3_Data.  These should be added to the set / toggle function list

|----------------------------------------------------------------------------|
|- Updates zone related variables, and safeZone via 'SUB check_ZoneType'.	-|
|----------------------------------------------------------------------------|
SUB check_Zone
/if (${Debug}) /echo |- check_Zone ==>

|--------------------------------------------------------------------------------------------------------------------------Update macro timers
	/varset idleTimer ${idleInterval}m
	/varset rebuffTimer 0
	
|---------------------------------------------------------------------------------------------------------------------Turn off assist / attack
	/if (${Assisting} || ${Me.Combat}) /call AssistOff
	
|--------------------------------------------------------------------------------------------------------------------------------Load Spellset
	/if (${Me.Class.CanCast} && ${Defined[Default_SpellSet]}) /call check_Gems

	/varset currentZone ${Zone.ID}
	
/if (${Debug}) /echo <== check_Zone -|
/RETURN



|--------------------------------------------------------------------------------------------------------------------|
|- Attempts to load the spellset specified in your Macro_Ini, if you're missing more than 3 gems on your spellbar.  -|
|--------------------------------------------------------------------------------------------------------------------|
SUB check_Gems
/if (${Debug}) /echo |- check_Gems ==>

|-----------------------------------------------------------------------------------------------------------------------Count Empty Spell Gems
	/declare i int local
	/declare spellCount int local
	/for i 1 to 9
		/if (${Select[${Me.Gem[${i}]},NULL]}) /varcalc spellCount ${spellCount}+1
	/next i
	
|---------------------------------------------------------------------------------------------------------------------If more than 3 are empty
	/if (${spellCount} > 3) {
		/declare spellsetMem_Timer timer local 300
		/docommand ${ChatToggle} Loading spells...
		/memspellset ${Default_SpellSet}
		:Memloop
		/if (${Debug}) /echo |- check_Gems -| :Memloop
		
		/call Background_Events
		/if (${Debug}) /echo spellsetMem_Timer = [${spellsetMem_Timer}]
		/if (${Window[SpellBookWnd].Open} && ${spellsetMem_Timer}) /goto :Memloop
		/if (${Me.Sitting}) /stand	
	}
	
/if (${Debug}) /echo <== check_Gems -|
/RETURN



|--------------------------------------------------------------------------------------------|
|- Reports bots who have fallen behind, and attempts to re-aquire follow when it breaks.	-|
|--------------------------------------------------------------------------------------------|
SUB check_Follow
/if (${Debug}) /echo |- check_Follow ==>

	/if (!${spamTimer}) {
	
		/if (${FollowTarget.Equal[${Me.CleanName}]}) {
			/declare i int local 1
			:NextFollower
			
				/if (!${SpamTimer_${Followers.Arg[${i},,]}}) {
					/if (${SpawnCount[pc ${Followers.Arg[${i},,]}]} && !${Spawn[pc ${Followers.Arg[${i},,]} radius ${MaxResponseDist}].ID}) {
						/popup ${Followers.Arg[${i},,]} has fallen behind.
						/call CreateTimer SpamTimer_${Followers.Arg[${i},,]} 70
						/varset spamTimer 10
					}
				}

			/varcalc i ${i} + 1
			/if (${Followers.Arg[${i},,].Length}) /goto :NextFollower

		} else {
			/if (${SpawnCount[pc ${FollowTarget}]} && ${Spawn[pc ${FollowTarget}].Distance} < ${LeashLength} && ${Spawn[pc ${FollowTarget}].LineOfSight}) {
				/if (!${NetAdvPath.Status} && !${Stick.Active}) {
					/call AquireFollow
				}
				/if (${Me.FeetWet}) /squelch /face fast ${FollowTarget}
			}
		}
	}

/if (${Debug}) /echo <== check_Follow -|
/RETURN



|--------------------------------------------------------------------------------------------------------------------|
|- Checks to see if your character is idle.																			-|
|--------------------------------------------------------------------------------------------------------------------|
|- Updates idleTimer to the idleInterval set in your Macro_Ini, whenever you've recently been active, or in combat.	-|
|- Sets idle=TRUE if you're in a safeZone, or when you have been inactive for longer than the idleInterval.			-|
|--------------------------------------------------------------------------------------------------------------------|
SUB check_Idle
/if (${Debug}) /echo |- check_Idle ==>
	
	| Check to see if I have been active recently.  If so, set Idle false, and update the idleTimer	
	/if (${activeTimer} || ${combatTimer}) {
		/varset idleTimer ${idleInterval}m
		/if (${idle}) /varset idle FALSE
	} else {
	
	|---------------------------------------------------------------------If I haven't been active, and the idleTimer is spent, set idle TRUE.
		/if (!${idle} && !${idleTimer})  {
			/varset idle TRUE
		}
	}
	
/if (${Debug}) {
	/echo |- check_Idle -| ( idleTimer = [${idleTimer}] : idle = [${idle}])
	/echo <== check_Idle -|
}
/RETURN



|--------------------------------------------------------------------------------------------|
|- Checks to see if your character is active.												-|	
|--------------------------------------------------------------------------------------------|			
|- Pauses the macro while you're moving, or have a corpse open.								-|
|- Pauses the macro while you have items on your cursor, and clears them after 30 seconds.	-|
|- Pauses the macro when you manually cast spells.											-|
|- Automatically closes your spellbook, if its been open for more than 30 seconds.			-|
|- Announces in /bc when you're feigning death.												-|
|- Pauses the macro when you're trading / banking.											-|
|- Automatically turns off combat when attacking PCs or when you have no target.			-|
|--------------------------------------------------------------------------------------------|
SUB check_Active
/if (${Debug}) /echo |- check_Active ==>

	/if (!${SpawnCount[pc ${Me}]}) /call check_Zoning
	/if (${Me.Moving} || ${Corpse.Open}) /call check_Movement
	/call check_Cursor
	/if (${Me.Combat} && !${combatTimer}) /call check_AutoAttack
	/if (${Me.Class.ShortName.NotEqual[BRD]} && ${Me.Casting.ID}) /call check_Casting
	/if (${Window[BigBankWnd].Open} || ${Window[MerchantWnd].Open} || ${Window[TradeWnd].Open} || ${Window[GuildBankWnd].Open} || ${Window[MerchantWnd].Open}) /call check_MerchantWnd
	/call check_Spellbook
	/call check_Feigning
	
/if (${Debug}) /echo <== check_Active -|
/RETURN



SUB check_Zoning
/if (${Debug}) /echo |- check_Zoning ==>

	/varset activeTimer 15
	/varset rebuffTimer 0

/if (${Debug}) /echo <== check_Zoning -|
/RETURN
SUB check_Movement
/if (${Debug}) /echo |- check_Movement ==>

	/varset activeTimer 5
	/varset rebuffTimer ${Math.Calc[${rebuffDelay} * 10].Int}

/if (${Debug}) /echo <== check_Movement -|
/RETURN
SUB check_AutoAttack
/if (${Debug}) /echo |- check_AutoAttack ==>

	/if (!${Target.ID} || ${Spawn[${Target.ID}].Type.Equal[PC]} && !${Toggle-PvP}) /attack off

/if (${Debug}) /echo <== check_AutoAttack -|
/RETURN	
SUB check_Casting
/if (${Debug}) /echo |- check_Casting ==>

	/call e3_CastingChecks
	/varset activeTimer ${Math.Calc[${activeDelay} * 10].Int}
	/varset rebuffTimer ${Math.Calc[${rebuffDelay} * 10].Int}

/if (${Debug}) /echo <== check_Casting -|
/RETURN	
SUB check_MerchantWnd
/if (${Debug}) /echo |- check_MerchantWnd ==>

	/varset activeTimer 150
	/varset rebuffTimer ${Math.Calc[${rebuffDelay} * 10].Int}
	/doevents flush

/if (${Debug}) /echo <== check_MerchantWnd -|
/RETURN
SUB check_Cursor
/if (${Debug}) /echo |- check_Cursor ==>

	/if (${Cursor.ID}) {
	
	|------------------------------------------------------------------------------Destroy badItems.
		/if (${Defined[badItems]}) {
			/if (${Select[${Cursor},${badItems}]} || ${Select[${Cursor.ID},${badItems}]}) {
				/echo Destroying ${Cursor}...
				/destroy
			}
		}

	|--------------Check to see if I'm waiting to drop this item, or if I've picked up something new
		/if (${Old_cursorID} != ${Cursor.ID}) {
			/varset cursorTimer ${Math.Calc[${inventoryTimer} * 10].Int}
			/varset Old_cursorID ${Cursor.ID}
		} else {
			/if (!${cursorTimer}) {
				/if (${do_Funnies}) /em pops a toot!
				/call clearCursor
			}
		}
		
		/varset rebuffTimer ${rebuffDelay}
		
	} else {
		/if (${Old_cursorID}) {
			/varset cursorTimer 0
			/varset Old_cursorID 0
		}
	}

/if (${Debug}) /echo <== check_Cursor -|
/RETURN
SUB check_Spellbook
/if (${Debug}) /echo |- check_Spellbook ==>

	/if (${Window[SpellbookWnd].Open}) {
		/if (!${spellbook_Timer}) {
		
			| if the timer isn't defined, make one.
			/if (!${Defined[spellbook_Timer]}) {
				/declare spellbook_Timer timer outer ${spellbookTimer}s
			} else {
				/if (${Window[SpellbookWnd].Open}) {
					/if (${do_Funnies}) /em bites ${If[${Me.Gender.Equal[male]},his,her]} toenails.
					/stand
				}
			}
		}
		
		/varset activeTimer ${Math.Calc[${activeDelay} * 10].Int}
		/varset rebuffTimer ${Math.Calc[${rebuffDelay} * 10].Int}
		
	} else {
		/if (${Defined[spellbook_Timer]}) /deletevar spellbook_Timer
	}

/if (${Debug}) /echo <== check_Spellbook -|
/RETURN
SUB check_Feigning
/if (${Debug}) /echo |- check_Feigning ==>

	/if (${Me.Feigning}) {
		/if (!${Select[${Me.Class.ShortName},NEC,SHD,MNK]}) {
			/stand
			/if (${Assisting} && ${StickOn}) /attack on
		} else {
		
			/if (!${Defined[feign_Timer]}) {
				/docommand ${ChatToggle} I AM FEIGNED.
				/if (${do_Funnies}) /em has a severe ass itch...
				/declare feign_Timer timer outer 15s
			}
		}
		
		/varset activeTimer ${Math.Calc[${activeDelay} * 10].Int}
		/varset rebuffTimer ${Math.Calc[${rebuffDelay} * 10].Int}
		
	} else {
		/if (${Defined[feign_Timer]}) /deletevar feign_Timer
	}

/if (${Debug}) /echo <== check_Feigning -|
/RETURN



|------------------------------------------------------------------------------------------------|
|- Updates the combatTimer whenever you, or someone in your party, is Assisting, or in combat. 	-|
|------------------------------------------------------------------------------------------------|
|- Sets combatTimer to the combatDelay whenever combat is detected.								-|
|------------------------------------------------------------------------------------------------|
SUB check_Combat

	| Check NetBots to see if anyone is in combat.
	/declare i int local
	/for i 1 to ${NetBots.Counts}
	
	/if (${NetBots[${NetBots.Client.Arg[${i}, ]}].Attacking} || ${Me.Combat} || ${Assisting}) {
		/if (${Debug}) /echo |- check_Combat -| ${NetBots[${NetBots.Client.Arg[${i}, ]}]} is attacking, setting combatTimer
		
	| Update macro timers.
		/varset combatTimer ${Math.Calc[${combatDelay} * 10].Int}
	}
	/if (!${combatTimer}) /next i

/RETURN



|------------------------
|- Completes item casting exchanges, after a sufficient time has passed.
|------------------------
SUB completePendingExchange
/if (${Debug}) /echo |- completePendingExchanges ==>
	
	/declare swapbackComplete bool local FALSE
	/declare swapRetryTimer timer local 50
	
	:SwapBackLoop
	/if (${Debug}) /echo |- e3_Setup -| :SwapBackLoop
	
	/call SwapItem "${Ini[${MacroData_Ini},PendingExchanges-${MacroQuest.Server},${Me.CleanName}].Arg[1,/]}" ${Ini[${MacroData_Ini},PendingExchanges-${MacroQuest.Server},${Me.CleanName}].Arg[2,/]}
	
	/if (!${InvSlot[${Ini[${MacroData_Ini},PendingExchanges-${MacroQuest.Server},${Me.CleanName}].Arg[2,/]}].Item.Name.Equal[${Ini[${MacroData_Ini},PendingExchanges-${MacroQuest.Server},${Me.CleanName}].Arg[1,/]}]}) {
		/if (${retryTimer}) {
			/goto :SwapBackLoop
		} else {
			/echo I have failed to swap [${Ini[${MacroData_Ini},PendingExchanges-${MacroQuest.Server},${Me.CleanName}].Arg[1,/]}] back.
		}
	} else {
		/call WriteToIni "${MacroData_Ini},PendingExchanges-${MacroQuest.Server},${Me.CleanName}" NONE 1
		/varset swapbackComplete TRUE
	}

/if (${Debug}) /echo <== completePendingExchanges -|
/RETURN ${swapbackComplete}



#EVENT add_AutoDestroyItem "[MQ2] Add Auto Destroy Item #1#"
SUB EVENT_add_AutoDestroyItem(line, WhatToAdd)
/if (${Debug}) /echo |- add_AutoDestroyItem ==>

	/if (!${Cursor.ID}) {
		/echo You must be holding the item you wish to add to the auto destroy list.
	} else {
		/if (${WhatToAdd.NotEqual[${Cursor.Name}]}) {
			/echo ERROR: The item on your cursor does not match the auto destroy request.  Please try again.
		} else {
			/if (${Select[${WhatToAdd},${Ini[${Macro_Ini},Background,Auto-Destroy List]}]}) {
				/echo [${WhatToAdd}] is already on the Auto-Destroy List.
			} else {
				/echo Adding [${WhatToAdd}] to the Auto-Destroy list.
				/if (!${Ini[${Macro_Ini},Background,Auto-Destroy List].Length}) {
					/ini "${Macro_Ini}" "Background" "Auto-Destroy List" "${WhatToWrite}"
				} else {
					/ini "${Macro_Ini}" "Background" "Auto-Destroy List" "${Ini[${Macro_Ini},Background,Auto-Destroy List]},${WhatToWrite}"
				}
			}
		}
	}

/if (${Debug}) /echo <== add_AutoDestroyItem -|
/RETURN



#EVENT remove_AutoDestroyItem "[MQ2] Remove Auto Destroy Item #1#"
SUB EVENT_remove_AutoDestroyItem(line, WhatToAdd)
/if (${Debug}) /echo |- remove_AutoDestroyItem ==>

	/if (!${Select[${WhatToAdd},${Ini[${Macro_Ini},Background,Auto-Destroy List]}]}) {
		/echo [${WhatToAdd}] is not on the Auto-Destroy List.
	} else {
		/echo this function is not yet finished
	}

/if (${Debug}) /echo <== remove_AutoDestroyItem -|
/RETURN



Sub background_Setup
/if (${Debug}) /echo |- background_Setup ==>

	| Create outer variables.
	/if (!${Defined[idle]}) /declare idle bool global
	/if (!${Defined[idleTimer]}) /declare idleTimer timer global
	
	/declare ActionTaken bool outer FALSE
	/declare spamTimer timer outer
	
	/declare activeTimer timer outer
	/declare activeDelay int outer 5
	
	/declare combatTimer timer outer
	/declare combatDelay int outer 100
	
	/declare cursorTimer timer outer
	/declare Old_cursorID int outer 0
	
	/declare rebuffTimer timer outer
	/declare rebuffDelay int outer 100
	
	/declare currentZone int outer 0

	/declare do_Funnies bool outer ${If[${Select[Killians,${NetBots.Client.Replace[ ,,]}]},FALSE,TRUE]}
	
	| Import Ini Variables.
	
	|-----------------------------------------------------------------------------------Clear Cursor Timer
	/if (${Ini[${Macro_Ini},Background,Auto-Inventory Timer (Sec)].Length}) {
		/call IniToVar "${Macro_Ini},Background,Auto-Inventory Timer (Sec)" inventoryTimer string outer
	} else {
		/echo Warning: Could not find a setting for [Auto-Inventory Timer] in [${Macro_Ini}].  [Auto-Inventory Timer] has been set to 30 seconds.
		/declare inventoryTimer int outer 30
	}
	
	|---------------------------------------------------------------------------------Close Spellbook Timer
	/if (${Ini[${Macro_Ini},Background,Close Spellbook Timer (Sec)].Length}) {
		/call IniToVar "${Macro_Ini},Background,Close Spellbook Timer (Sec)" spellbookTimer string outer
	} else {
		/echo Warning: Could not find a setting for [Close Spellbook Timer] in [${Macro_Ini}].  [Close Spellbook Timer] has been set to 1 minute.
		/declare spellbookTimer int outer 60
	}
	
	|---------------------------------------------------------------------------------Idle Timer
	/if (${Ini[${Macro_Ini},Background,Idle Time Out (Min)].Length}) {
		/call IniToVar "${Macro_Ini},Background,Idle Time Out (Min)" idleInterval int outer
	} else {
		/echo Warning: Could not find a setting for [Idle Time Out] in [${Macro_Ini}].  [Idel Time Out] has been set to 10 minutes.
		/declare idleInterval int outer 10
	}
	
	/declare reagentCheckInterval int outer 60
	/if (${Ini[${Macro_Ini},Background,Reagent Check Interval (Min)].Length}) /varset reagentCheckInterval ${Ini[${Macro_Ini},Background,Reagent Check Interval (Min)]}
	
	/if (${Ini[${Macro_Ini},Background,Auto-Destroy List].Length}) /call IniToVar "${Macro_Ini},Background,Auto-Destroy List" badItems string outer

/if (${Debug}) /echo <== background_Setup -|
/return



SUB background_MacroSettings
/if (${Debug}) /echo |- background_MacroSettings ==>

	/call WriteToIni "${Macro_Ini},Background,Auto-Destroy List"
	/call WriteToIni "${Macro_Ini},Background,Idle Time Out (Min)" 10
	/call WriteToIni "${Macro_Ini},Background,Auto-Inventory Timer (Sec)" 30
	/call WriteToIni "${Macro_Ini},Background,Close Spellbook Timer (Sec)" 30

/if (${Debug}) /echo <== background_MacroSettings -|
/RETURN



SUB background_CharacterSettings
/if (${Debug}) /echo |- background_CharacterSettings ==>

	|	|

/if (${Debug}) /echo <== background_CharacterSettings -|
/RETURN



Sub background_Aliases
/if (${Debug}) /echo |- background_Aliases ==>

	/squelch /alias /AddAutoDestroy delete
	/squelch /alias /RemoveAutoDestroy delete
	
	/alias /AddAutoDestroy /echo Add Auto-Destro item ${Cursor.Name}
	/alias /RemoveAutoDestroy /echo Remove Auto-Destro item ${Cursor.Name}

/if (${Debug}) /echo <== background_Aliases -|
/return