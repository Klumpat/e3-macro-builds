|------------------------------------------------------------------------|
|- e3_Macro_Background.inc	( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains background functions that update e3 status and variables,	-|
|- along with many minimally invasive macro functions.					-|
|------------------------------------------------------------------------| 



|--------------------------------------------------------------------|
|- Background.inc's mainLoop hook.									-|
|--------------------------------------------------------------------|
|- Calls background functions, as an extention of the main loop.	-|
|--------------------------------------------------------------------|
SUB Background_Services
/if (${Debug}) /echo |- Background_Services ==>
|-------------------------------------------------------------------------------------------------------------------------------------Status check.
	/call check_Active
	/if (${currentZone} != ${Zone.ID}) /call check_Zone
	/call check_Idle
	/if (${autoTribute}) /call check_autoTribute
	
|-----------------------------------------------------------------------------Check Follow
	/if (${Following} && !${Assisting}) /call check_Follow
	
|-------------------------------------------------------------------------------------------------------------------------------------Assist hooks.
	/if (${auto_AssistOn} && !${Assisting}) /call autoAssist
	
|---------------------------------------------------------------------------------------------------------------------------------------------Misc.
	/if (!${Cursor.ID}) {
		/if (!${Me.Casting.ID} || ${Me.Class.ShortName.Equal[BRD]}) {
			/if (${lifeSupport.Size}) /call check_lifeSupport
			/if (${Defined[ConstantIllusion]} && !${Select[${Me.Race},${illusionRace}]}) /call check_Illusion
		}
	}
	/call Background_Events
	
/if (${Debug}) /echo <== Background_Services -|
/RETURN



|------------------------------------------------------------|
|- Background.inc's Tertiary mainloop hook.					-|
|------------------------------------------------------------|											
|- Calls clean-up and maintenance functions.				-|	
|------------------------------------------------------------|
SUB Tertiary_Services
/if (${Debug}) /echo |- Tertiary_Services ==>

	/if (${timerArray.Size}) /call check_macroTimers
	/if (${track_Reageants.Size}) /call check_Reageants
	/if (${medBreak}) /call check_MedBreak
	/doevents

/if (${Debug}) /echo <== Tertiary_Services -|
/RETURN



|--------------------------------------------------------------------------------------------------------------------|
|- Background events and run-times that are checked even while casting or otherwise considered active.		   		-|
|--------------------------------------------------------------------------------------------------------------------|
|- This function is checked constantly, included events and functions should have minimal character interaction.	-|
|--------------------------------------------------------------------------------------------------------------------|
SUB Background_Events
/if (${Debug}) /echo |- Background_Events ==>

|-----------------------------------------------------------------------------------------e3_Macro_Assists.inc events.
	/doevents Enrage
	/doevents EnrageOff
	/doevents Assist
	/doevents BackOff
	/doevents RainOn
	/doevents RainOff
	/doevents DebuffsOn
	/doevents DebuffsOff
	/doevents DoTsOn
	/doevents DoTsOff
	/doevents Taunt_On
	/doevents Taunt_Off
	/doevents SwarmPets
	/doevents QuickBurn
	/doevents FullBurn

	/if (${Assisting}) {
		/call check_AssistTarget
	}
	
|--------------------------------------------------------------------------------------------------Background
	/doevents SetSafeZone
	
|-------------------------------------------------------------------------------------------------e3_Macro_Basics.inc events.
	/doevents disconnect
	/doevents Follow
	/doevents Stop
	/doevents MoveTo
	/doevents MoveHere
	/doevents clickIt	
	/doevents setGroup
	/doevents GroupUp
	/doevents Invite
	/doevents Disband
	/doevents Consent
	/doevents addConcent
	/doevents flushConcent
	/doevents pingCorpses
	/doevents gate
	/doevents Evacuate
	/doevents FindItems
	/doevents CheckBuffs
	/doevents RemoveBuff
	/doevents CountBots
	/doevents drop_Invis
	/doevents ClickYes
	/if (${Me.Class.ShortName.NotEqual[BRD]} && !${Me.Casting.ID} && !${Cursor.ID} || ${Me.Class.ShortName.Equal[BRD]} && !${Cursor.ID}) /doevents Swap_Items
	/doevents RaidAccept
	/doevents RaidInvite
	/doevents Request_RaidInvite
	/doevents Force_RaidDisband
	/doevents GroupPortCommand
	/doevents toggle-PvP
	/doevents End_MedBreak
	
|-----------------------------------------------------------------------------------------------------------e3_Macro_ChainHeals.inc
	/doevents build_HealChain
	/doevents add_ChainMember
	/doevents start_HealChain
	
|--------------------------------------------------------------------------------------------------------Loot.
	/doevents checkLootSetting
	/doevents updateLootSetting
	
|------------------------------------------------------------------------------------------------------------------Pets.
	/doevents suspendMinion
	
|----------------------------------------------------------------------------------------------------------Smooth Moves.
	/doevents record_Route
	/doevents end_Recording
	/doevents save_Recording
	/doevents list_Routes
	
|--------------------------------------------------------------------------------------------------------------Wait4Rez.
	/doevents LootNow
	/if (!${Waiting4Rez}) {
		/doevents WaitNow
		/doevents Dead
	}
	
|---------------------------------------------------------------------------------------------------------Misc scripts.
	/if (${userTimers.Size}) /call check_userTimers

|--------------------------------------------------------------------------------------Class Specific Background Events.
	/if (${Bool[${Me.Class}]}) /call ${Me.Class.ShortName}_Background_Events
	
|-----------------------------------------------------------------------------------------------------GoD Alerts.
	/if (${Zone.ShortName.Equal[uqua]}) /doevents Define_UquaKey
	/if (${Zone.ShortName.Equal[ikkinz]}) {
		/doevents setClass_Warrior
		/doevents setClass_Shaman
		/doevents setClass_Beastlord
		/doevents setClass_Necromancer
		/doevents setClass_Cleric
		/doevents setClass_Shadowknight
		/doevents setClass_Monk
		/doevents setClass_Bard
		/doevents setClass_Paladin
		/doevents setClass_Rogue
		/doevents setClass_Enchanter
		/doevents setClass_Wizard
		/doevents setClass_Berserker
		/doevents setClass_Magician
		/doevents setClass_Druid
		/doevents setClass_Ranger
		/doevents MobSet
		/doevents startClass_Priests
		/doevents startClass_Hybrids
		/doevents startClass_Casters
		/doevents startClass_Melees
	}
	
/if (${Debug}) /echo <== Background_Events -|
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

|-----------------------------------------------------------------------------------------------------------------------------------Movement Check
	/if (${Me.Moving} || ${Corpse.Open}) {
		/if (${Debug}) /echo |- check_Active -| Moving / Corpse Check
		/varset activeTimer ${activeDelay}
		/varset rebuffTimer ${rebuffDelay}
	}
	
|-------------------------------------------------------------------------------------------------------------------------------------Zoning Check
	/if (!${SpawnCount[pc ${Me}]}) {
		/if (${Debug}) /echo |- check_Active -| Zoning check
		/varset activeTimer 15
	}
	
|-------------------------------------------------------------------------------------------------------------------------------------Cursor Check
	/if (${Cursor.ID}) {
		/if (${Debug}) /echo |- check_Active -| Cursor Check ${If[${Defined[cursorItem_Timer]}, -- ${cursorItem_Timer},]}
		
	|------------------------------------------------------------------------------Destroy badItems.
		/if (${Select[${Cursor},${badItems}]} || ${Select[${Cursor.ID},${badItems}]}) {
			/echo Destroying ${Cursor}...
			/destroy
		} else {
		
		|--------------Check to see if I'm waiting to drop this item, or if I've picked up something new
			/if (${Old_cursorID} != ${Cursor.ID}) {
			
			|-------------------------------------------------------------------Clear old variables.
				/if (${Defined[cursorTimer]}) {
					/deletevar cursorTimer
					/deletevar Old_cursorID
				}
				
			|------------------------------------------------------------------Create new variables.
				/declare cursorTimer timer outer ${inventoryTimer}
				/declare Old_cursorID int outer ${Cursor.ID}
			} else {
				/if (!${cursorTimer}) /call clearCursor
			}
		}
	}
	/if (!${Cursor.ID} && ${Defined[cursorTimer]}) {
		/deletevar cursorTimer
		/deletevar Old_cursorID
	}
	
|------------------------------------------------------------------------------------------------------------------------------------------Casting
	/if (${Me.Class.ShortName.NotEqual[BRD]} && ${Me.Casting.ID}) {
		/if (${Debug}) /echo |- check_Active -| Casting Check
		/call e3_CastingChecks

	|---------------------------------------------------------------------------Update macro timers.
		/varset activeTimer ${activeDelay}
	}
	
|----------------------------------------------------------------------------------------------------------------------------------------SpellBook
	/if (${Window[SpellbookWnd].Open} || ${Defined[spellbook_Timer]}) {
		/if (${Debug}) /echo |- check_Active -| SpellBook Check ${If[${Defined[spellbook_Timer]}, -- ${spellbook_Timer},]}
		
	|----------------------------------------------------------------------------------if timer = 0.
		/if (!${spellbook_Timer}) {
		
		|--------------------------------------------------if the timer isn't defined, make one.  Wait 10 seconds before closing the spellbook
			/if (!${Defined[spellbook_Timer]}) {
				/declare spellbook_Timer timer outer ${spellbookTimer}
			} else {
				/if (${Window[SpellbookWnd].Open}) /stand
			}
		}
		
	|-------------------------------------------------------------------------------------------------------------------Remove the spent timer
		/if (!${Window[SpellbookWnd].Open} && ${Defined[spellbook_Timer]}) /deletevar spellbook_Timer
		
	|----------------------------------------------------------------------------------------------------------------------Update macro timers
		/varset activeTimer ${activeDelay}
	}
	
|------------------------------------------------------------------------------------------------------------------------------------------Feigned
	/if (${Me.Feigning}) {
		/if (${Debug}) /echo |- check_Active -| Feigning Check ${If[${Defined[feign_Timer]}, -- ${feign_Timer},]}
		
	|-------------------------------------------------------------------------if I'm not a class who can feign, stand up, then check Assisting
		/if (!${Select[${Me.Class.ShortName},NEC,SHD,MNK]}) {
			/stand
			/if (${Assisting} && ${StickOn}) /attack on
		} else {
		
		|-------------------------if the timer isn't defined, make one.  Output that you're feigned, and wait 15 seconds before doing it again
			/if (!${Defined[feign_Timer]}) {
				/docommand ${ChatToggle} I AM FEIGNED.
				/declare feign_Timer timer outer 15s
			}
		}
		
	|-------------------------------------------------------------------------------------------------------------------Remove the spent timer
		/if (${Defined[feign_Timer]} && !${Me.Feigning} || ${Defined[feign_Timer]} && !${feign_Timer}) /deletevar feign_Timer
	
	|-------------Update macro timers
		/varset activeTimer ${activeDelay}
	}

|---------------------------------------------------------------------------------------------------------------------------------------Bank Check
	/if (${Window[BigBankWnd].Open} || ${Window[MerchantWnd].Open} || ${Window[TradeWnd].Open} || ${Window[GuildBankWnd].Open} || ${Window[MerchantWnd].Open}) {
		/if (${Debug}) /echo |- check_Active -| Bank Check
		
	|----------------------------------------------------------------------------------------------------------------------Update macro timers
		/varset activeTimer ${Math.Calc[${activeDelay}*10]}
		/varset rebuffTimer ${rebuffDelay}
		/doevents flush
	}
	
|---------------------------------------------------------------------------------------------------------------------------------------Attack Off
	/if (${Me.Combat} && !${combatTimer}) {
		/if (!${Target.ID} || ${Spawn[${Target.ID}].Type.Equal[PC]} && !${Toggle-PvP}) {
			/attack off
		}
	}

/if (${Debug}) /echo <== check_Active -|
/RETURN



|--------------------------------------------------------------------------------------------------------------------|
|- Checks to see if your character is idle.																			-|
|--------------------------------------------------------------------------------------------------------------------|
|- Updates idleTimer to the idleInterval set in your Macro_Ini, whenever you've recently been active, or in combat.	-|
|- Sets idle=TRUE if you're in a safeZone, or when you have been inactive for longer than the idleInterval.			-|
|--------------------------------------------------------------------------------------------------------------------|
SUB check_Idle
/if (${Debug}) /echo |- check_Idle ==>
	
	|----------------------------------------If I'm not in a safe zone, check to see if I have been active recently.  If so, update the idleTimer.	
		/if (${activeTimer} || ${combatTimer}) {
			/varset idleTimer ${idleInterval}
			/if (${idle}) /varset idle FALSE
		} else {
		
		|---------------------------------------------------------------------If I haven't been active, and the idleTimer is spent, set idle TRUE.
			/if (!${idle} && !${idleTimer})  /varset idle TRUE
		}
	
/if (${Debug}) {
	/echo |- check_Idle -| ( idleTimer = [${idleTimer}] : idle = [${idle}])
	/echo <== check_Idle -|
}
/RETURN



|------------------------------------------------------------------------------------------------|
|- Updates the combatTimer whenever you, or someone in your party, is Assisting, or in combat. 	-|
|------------------------------------------------------------------------------------------------|
|- Sets combatTimer to the combatDelay whenever combat is detected.								-|
|------------------------------------------------------------------------------------------------|
SUB check_Combat

|----------------------------------------------------------------------------------------------------------------------Check NetBots to see if anyone is in combat.
	/declare i int local
	/for i 1 to ${NetBots.Counts}
	
	/if (${NetBots[${NetBots.Client.Arg[${i}, ]}].Attacking} || ${Me.Combat} || ${Assisting}) {
		/if (${Debug}) /echo |- check_Combat -| ${NetBots[${NetBots.Client.Arg[${i}, ]}]} is attacking, setting combatTimer
		
	|------------------------------------------------------------------------------------------------------------------------------------------Update macro timers.
		/varset combatTimer ${combatDelay}
	}
	/if (!${combatTimer}) /next i

/RETURN



|----------------------------------------------------------------------------|
|- Updates zone related variables, and safeZone via 'SUB check_ZoneType'.	-|
|----------------------------------------------------------------------------|
SUB check_Zone
/if (${Debug}) /echo |- check_Zone ==>

|--------------------------------------------------------------------------------------------------------------------------Update macro timers
	/varset idleTimer ${idleInterval}
	/varset rebuffTimer 0
	
|---------------------------------------------------------------------------------------------------------------------Turn off assist / attack
	/if (${Assisting} || ${AdvAssisting} || ${Me.Combat}) /call AssistOff
	
|--------------------------------------------------------------------------------------------------------------------------------Load Spellset
	/if (${Me.Class.CanCast} && ${Defined[Default_SpellSet]}) /call check_Gems
	
|--------------------------------------------------------------------------------------------------------------------------Clear Badspell list
	/if (${Defined[bad_Spells]}) /varset bad_Spells PLACEHOLDER
	
|-------------------------------------------------------------------------------------------------------------------------------------Set Zone 
	/call check_ZoneType
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



|----------------------------------------------------------------------------------------|
|- Reads safeZones_Ini for zone settings and updates safeZones variable. 				-|
|----------------------------------------------------------------------------------------|
|- When new zones are entered, they are added to the safeZones_Ini, and set to 'safe'.	-|
|----------------------------------------------------------------------------------------|
SUB check_ZoneType
/if (${Debug}) {
	/echo |- check_ZoneType ==>
	/echo ${safeZones_Ini},${Zone.ShortName.Left[1]},${Zone.ShortName} == [${Ini[${safeZones_Ini},${Zone.ShortName.Left[1]},${Zone.ShortName}].Equal[safe]}]
}

|--------------------------------------------------------If a safeZones_Ini entry for this zone does not exist, create one, and default to 'safe'.
	/if (!${Ini[${safeZones_Ini},${Zone.ShortName.Left[1]},${Zone.ShortName}].Length}) {
		/echo ${Zone} has been added to your ${safeZones_Ini} file, and set by default as a safe zone.  Use '/bc Set Raid Zone' or '/bc Set Safe Zone' to update settings for this zone.
		/ini ${safeZones_Ini} ${Zone.ShortName.Left[1]} ${Zone.ShortName} Safe
		/varset safeZone TRUE
		
|------------------------------------------------------------------------------Else, if there is an entry for this zone, set safeZone accordingly.
	} else /if (${Ini[${safeZones_Ini},${Zone.ShortName.Left[1]},${Zone.ShortName}].Length}) {
		/if (${Ini[${safeZones_Ini},${Zone.ShortName.Left[1]},${Zone.ShortName}].Equal[safe]}) {
			/varset safeZone TRUE
		} else /if (${Ini[${safeZones_Ini},${Zone.ShortName.Left[1]},${Zone.ShortName}].Equal[raid]}) {
			/varset safeZone FALSE
		}
	}

/if (${Debug}) /echo <== check_ZoneType -|
/RETURN



|----------------------------------------------------------------------------------------------------|
|- Updates safeZones_Ini entries to 'safe' or 'raid' via /bc Set Raid Zone, or /bc Set Safe Zone.	-|
|----------------------------------------------------------------------------------------------------|
|- @param line: <'Name'> Set 'Setting' Zone.														-|
|- @param zoneSetting: Holds 'Raid' or 'Safe' zone settings.										-|
|- @param charSender: The PC setting the zone type.													-|
|----------------------------------------------------------------------------------------------------|
#event Set_zoneType "<#2#> <Set #1# Zone>"
#event Set_zoneType "<#2#> Set #1# Zone"
#event Set_zoneType "<#2#> <Set Zone> #1#"
#event Set_zoneType "<#2#> Set Zone #1#"
#event Set_zoneType "[MQ2] <Set #1# Zone>"
#event Set_zoneType "[MQ2] Set #1# Zone"
#event Set_zoneType "[MQ2] <Set Zone> #1#"
#event Set_zoneType "[MQ2] Set Zone #1#"
SUB event_Set_zoneType(line, zoneSetting, chatSender)
/if (${SpawnCount[pc ${chatSender}]} || !${Defined[chatSender]}) {
	/if (${zoneSetting.Equal[safe]}) {
		/echo ${Zone} has been listed as a safe zone.
		/ini ${safeZones_Ini} ${Zone.ShortName.Left[1]} ${Zone.ShortName} Safe
	} else /if (${zoneSetting.Equal[raid]}) {
		/echo ${Zone} has been listed as a raid zone.
		/ini ${safeZones_Ini} ${Zone.ShortName.Left[1]} ${Zone.ShortName} Raid
	}
	/call check_Zone
}
/RETURN



|----------------------------------------------------------------------------------------------------|
|- Updates safeZones_Ini entries to 'safe' or 'raid' via /bc Set Raid Zone, or /bc Set Safe Zone.	-|
|----------------------------------------------------------------------------------------------------|
SUB check_autoTribute
/if (${Debug}) {
	/echo |- check_autoTribute ==>
	/echo |- check_autoTribute -| Idle = ${idle} -- SafeZone = ${safeZone}
}

|------------------------------------------------------------------------If idle is TRUE, and my remaining TributeTimer is < 10, turn off Tribute.	
	/if (${safeZone}) {
		/if	(${Me.TributeActive} && ${Me.TributeTimer} <= 10) {
			/echo |- AutoTribute -| I'm in a safe zone, turning tribute off.
			/call TributeOff
		}
	} else /if (${idle}) {
		/if	(${Me.TributeActive} && ${Me.TributeTimer} <= 10) {
			/echo |- AutoTribute -| I'm idle, turning tribute off.
			/call TributeOff
		}
	} else {
		/if (!${Me.TributeActive} && ${Me.CurrentFavor} > 500) {
			/echo |- AutoTribute -| I've entered a raid zone, turning tribute on.
			/call TributeOn 
		}
	}

/if (${Debug}) /echo <== check_autoTribute -|
/RETURN



|--------------------|
|- Turn Tribute off	-|
|--------------------|
SUB TributeOff
/if (${Debug}) /echo |- TributeOff ==>

	/declare LoopCounter int local |----Counting variable.
|-------------------------------------------------------------------------------Open the tribute window.
	/if (!${Window[TributeBenefitWnd].Open}) /keypress "alt u" release
	
|-------------------------------------------------------------------------------Click deactivate button.
	:deactivate_Loop
	/if (${Debug}) /echo |- TributeOff -| :deactivate_Loop
	
		/varset LoopCounter ${Math.Calc[${LoopCounter}+1]}
		/notify TributeBenefitWnd DowngradeButton leftmouseup
		/delay 2 !${Me.TributeActive}
		/if (${Me.TributeActive} && ${LoopCounter} < 15) /goto :deactivate_Loop
	
|------------------------------------------------------------------------------Close the tribute window.
	
	/varset LoopCounter 0	
	:closeWindow_Loop
	/if (${Debug}) /echo |- TributeOff -| :closeWindow_Loop
	
		/varset LoopCounter ${Math.Calc[${LoopCounter}+1]}
		/keypress "alt u" release
		/delay 2 !${Window[TributeBenefitWnd].Open}
		/if (${Window[TributeBenefitWnd].Open} && ${LoopCounter} < 15) /goto :closeWindow_Loop
		
/if (${Debug}) /echo <== TributeOff -|
/RETURN



|--------------------|
|- Turn Tribute on	-|
|--------------------|
SUB TributeOn
/if (${Debug}) /echo |- TributeOn ==>

	/declare LoopCounter int local |----Counting variable.
|-------------------------------------------------------------------------------Open the tribute window.
	/if (!${Window[TributeBenefitWnd].Open}) /keypress "alt u" release
	
|-------------------------------------------------------------------------------Click deactivate button.
	:activate_Loop
	/if (${Debug}) /echo |- TributeOn -| :activate_Loop
	
		/varset LoopCounter ${Math.Calc[${LoopCounter}+1]}
		/notify TributeBenefitWnd DowngradeButton leftmouseup
		/delay 2 ${Me.TributeActive}
		/if (!${Me.TributeActive} && ${LoopCounter} < 15) /goto :activate_Loop
	
|------------------------------------------------------------------------------Close the tribute window.
	
	/varset LoopCounter 0	
	:closeWindow_Loop
	/if (${Debug}) /echo |- TributeOn -| :closeWindow_Loop
	
		/varset LoopCounter ${Math.Calc[${LoopCounter}+1]}
		/keypress "alt u" release
		/delay 2 !${Window[TributeBenefitWnd].Open}
		/if (${Window[TributeBenefitWnd].Open} && ${LoopCounter} < 15) /goto :closeWindow_Loop
		
/if (${Debug}) /echo <== TributeOn -|
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
			/if (${Debug}) /echo |- check_Follow -| :NextFollower
			
				/if (!${SpamTimer_${Followers.Arg[${i},,]}}) {
					/if (${SpawnCount[pc ${Followers.Arg[${i},,]}]} && !${Spawn[pc ${Followers.Arg[${i},,]} radius ${FollowBreakDistance}].ID}) {
						/popup ${Followers.Arg[${i},,]} has fallen behind.
						/call CreateTimer SpamTimer_${Followers.Arg[${i},,]} 70
						/varset spamTimer 10
					}
				}
			/varcalc i ${i} + 1
			/if (${Followers.Arg[${i},,].Length}) /goto :NextFollower
		} else {
			/if (${SpawnCount[pc ${FollowTarget}]} && !${NetAdvPath.Status} && !${Stick.Active} && ${Spawn[pc ${FollowTarget}].Distance} < ${FollowBreakDistance} && ${Spawn[pc ${FollowTarget}].LineOfSight}) {
				/call AquireFollow
			}
		}
	}

/if (${Debug}) /echo <== check_Follow -|
/RETURN



|------------------------------------------------------------|
|- Maintain a ConstantIllusion, specified in the Macro_Ini	-|
|------------------------------------------------------------|
|- Illusions must be instant cast.							-|
|------------------------------------------------------------|
SUB check_Illusion
/if (${Me.FreeBuffSlots} > ${MaxBuffs} && ${FindItem[${ConstantIllusion}].ID} && ${Me.Buff[${FindItem[${ConstantIllusion}].Spell}].Duration}<20 && ${Spell[${FindItem[${ConstantIllusion}].Spell}].Stacks}) /call e3_Cast 0 "${ConstantIllusion}" item 0s
/RETURN



|--------------------------------------------|
|- Manually recast ConstantIllusion items.	-|
|--------------------------------------------|
#event RecastIllusions "<#*#> Recast Illusions"
SUB event_RecastIllusions 
/if (${Me.Buff[${FindItem[${ConstantIllusion}].Spell}].ID}) {
	/nomodkey /notify BuffWindow Buff${Math.Calc[${Me.Buff[${FindItem[${ConstantIllusion}].Spell}].ID}-1].Int} leftmouseup
} else /if (${Me.Buff[Illusion: Dark Elf].ID}) {
	/nomodkey /notify BuffWindow Buff${Math.Calc[${Me.Buff[Illusion: Dark Elf].ID}-1].Int} leftmouseup
}
/RETURN



|--------------------------------------------------------------------------------------------------------------------|
|- Uses Items, Spells, Abilities, AAs, and Disciplines listed in the [LifeSupport] section of the Character_Ini.	-|
|--------------------------------------------------------------------------------------------------------------------|
SUB check_lifeSupport
/if (${Debug}) /echo |- check_lifeSupport ==>

	/declare i int local
	/for i 1 to ${lifeSupport.Size}
|----------------------------------------------------------------------------------------------------------------If my HPs are > 0 and < ability %
		/if (${Me.PctHPs} && ${Me.PctHPs} < ${lifeSupport[${i}].Arg[3,/]}) {
		
|----------------------------------------------------------------------------------------------------------------------------------------Use Spell
			/if (${lifeSupport[${i}].Arg[2,/].Left[3].Equal[gem]}) {
				/call e3_Cast ${Me.ID} "${lifeSupport[${i}].Arg[1,/]}" ${lifeSupport[${i}].Arg[2,/]}
				
|-----------------------------------------------------------------------------------------------------------------------------------------Use Item
			} else /if (${lifeSupport[${i}].Arg[2,/].Equal[item]}) {
				/if (!${item_Timer_${FindItem[${lifeSupport[${i}].Arg[1,/]}].ID}}) /call e3_Cast ${Me.ID} "${lifeSupport[${i}].Arg[1,/]}" item
				
|----------------------------------------------------------------------------------------------------------------------------Use Alternate Ability
			} else /if (${lifeSupport[${i}].Arg[2,/].Equal[alt]} || ${lifeSupport[${i}].Arg[2,/].Equal[aa]} || ${lifeSupport[${i}].Arg[2,/].Equal[altability]}) {
				/if (!${alt_Timer_${Me.AltAbility[${lifeSupport[${i}].Arg[1,/]}].ID}}) /call e3_Cast ${Me.ID} "${lifeSupport[${i}].Arg[1,/]}" alt
				
|-----------------------------------------------------------------------------------------------------------------------------------Use Discipline
			} else /if (${lifeSupport[${i}].Arg[2,/].Equal[disc]} || ${lifeSupport[${i}].Arg[2,/].Equal[discipline]}) {
				/if (!${disc_Timer_${lifeSupport[${i}].Arg[1,/].Replace[ ,]}}) /call e3_Cast ${Me.ID} "${lifeSupport[${i}].Arg[1,/]}" disc
				
|--------------------------------------------------------------------------------------------------------------------------------------Use Ability
			} else /if (${lifeSupport[${i}].Arg[2,/].Equal[ability]} || ${lifeSupport[${i}].Arg[2,/].Equal[skill]}) {
				/if (${Me.AbilityReady[${lifeSupport[${i}].Arg[1,/]}]}) /doability "${lifeSupport[${i}].Arg[1,/]}"
			}		
		}
	/next i
	
/if (${Debug}) /echo <== check_lifeSupport -|
/RETURN



|--------------------------------------------------------------------------------------------------------------------------------|
|- Automatically calls assist when bots in melee combat, and the target has fallen below the EngagePct, listed in the Macro_Ini	-|
|--------------------------------------------------------------------------------------------------------------------------------|
SUB autoAssist
/if (${Debug}) /echo |- autoAssist ==>

	/if (${Target.ID} && ${Me.Combat} && ${Target.ID} != ${PauseTarget} && ${Target.PctHPs} < ${EngagePct} && ${Select[${Spawn[id ${Target.ID}].Type},${AcceptableTargetTypes}]}) {
		/docommand ${ChatToggle} Assist me
		/delay 1
	}
	
/if (${Debug}) /echo <== autoAssist -|
/RETURN



|----------------------------------------------------------------------------------------|
|- Creates a timer of the specified name and setting to be tracked by check_userTimers.	-|
|----------------------------------------------------------------------------------------|
|- @param line: <Name> Make timer 'Name' 'Setting'										-|
|- @param timerName: Name of the timer you want to declare.								-|
|- @param timerSetting: Setting of the timer you want ot declare.						-|
|----------------------------------------------------------------------------------------|
#event make_Timer "<#*#> Make timer #1# #2#"
#event make_Timer "[#*#] Make timer #1# #2#"
SUB event_make_Timer(ling, timerName, timerSetting)
/declare i int local
/if (${Defined[userTimers]}) {
	/for i 1 to ${userTimers.Size}
		/if (${userTimers[${i}].Equal[${timerName}]}) {
			/echo I already have a timer named [${timerName}], choose a new name, and try again.
			/goto :skip
		}
	/next i
}
/echo Creating [${timerName}] and setting it for [${timerSetting}]
/call BuildArray userTimers ${timerName} stack
/call CreateTimer ${timerName} ${timerSetting}
:skip
/RETURN

SUB check_userTimers

/declare i int local
/declare spentTimers int local
/for i 1 to ${userTimers.Size}

|- If the timer being checked is less than 6 seconds, and the spamTimer associated with this userTimer is 0 or NULL
/if (${${userTimers[${i}]}}<60 && ${${userTimers[${i}]}} && !${spamTimer<5_${userTimers[${i}]}}) {
	|- Popup timer name - remaining seconds
	/popup ${userTimers[${i}]} [${Math.Calc[${${userTimers[${i}]}}/10].Int}sec]
	|- Create a 7/10 of a second spamTimer
	/call CreateTimer spamTimer<5_${userTimers[${i}]} 7
	/beep 1
	
	
|- If the timer being checked is less than 30 seconds, and the spamTimer associated with this userTimer is 0 or NULL
} else /if (${${userTimers[${i}]}}<305 && ${${userTimers[${i}]}} && !${spamTimer<30_${userTimers[${i}]}}) {
	/popup ${userTimers[${i}]} [${Math.Calc[${${userTimers[${i}]}}/10].Int}]
	/call CreateTimer spamTimer<30_${userTimers[${i}]} 98
	/beep 1
	

|- If the timer being checked is less than 60 seconds, and the spamTimer associated with this userTimer is 0 or NULL
} else /if (${${userTimers[${i}]}}<605 && ${${userTimers[${i}]}} && !${spamTimer_${userTimers[${i}]}}) {
	/popup ${userTimers[${i}]} [${Math.Calc[${${userTimers[${i}]}}/10].Int}]
	/call CreateTimer spamTimer_${userTimers[${i}]} 598
	/beep 1
}
/if (!${${userTimers[${i}]}}) /varcalc spentTimers ${spentTimers} + 1
/next i
/if (${spentTimers}==${userTimers.Size}) /deletevar userTimers
/RETURN

SUB check_Reageants
/declare i int local
/for i 1 to ${track_Reageants.Size}
/if (${FindItemCount[${track_Reageants[${i}].Arg[1,/]}]} < ${track_Reageants[${i}].Arg[2,/]}) {
	/if (${FindItemCount[${track_Reageants[${i}].Arg[1,/]}]}) {
		/if (!${Defined[${track_Reageants[${i}].Arg[1,/].Replace[ ,]}_Counter]}) {
			/declare ${track_Reageants[${i}].Arg[1,/].Replace[ ,]}_Counter int outer ${FindItemCount[${track_Reageants[${i}].Arg[1,/]}]}
			/docommand ${ChatToggle} I have ${FindItemCount[${track_Reageants[${i}].Arg[1,/]}]} ${track_Reageants[${i}].Arg[1,/]}${If[${track_Reageants[${i}].Arg[1,/].Right[1].NotEqual[s]},s,]} left.
		} else /if (${Defined[${track_Reageants[${i}].Arg[1,/].Replace[ ,]}_Counter]}) {
			/if (${FindItemCount[${track_Reageants[${i}].Arg[1,/]}]} != ${${track_Reageants[${i}].Arg[1,/].Replace[ ,]}_Counter}) {
				/docommand ${ChatToggle} I have ${FindItemCount[${track_Reageants[${i}].Arg[1,/]}]} ${track_Reageants[${i}].Arg[1,/]}${If[${track_Reageants[${i}].Arg[1,/].Right[1].NotEqual[s]},s,]} left.
				/varset ${track_Reageants[${i}].Arg[1,/].Replace[ ,]}_Counter ${FindItemCount[${track_Reageants[${i}].Arg[1,/]}]}
			}
		}
	} else /if (!${FindItemCount[${track_Reageants[${i}].Arg[1,/]}]} && !${out_Of_${track_Reageants[${i}].Arg[1,/].Replace[ ,]}}) {
		/docommand ${ChatToggle} I'm completely out of ${track_Reageants[${i}].Arg[1,/]}${If[${track_Reageants[${i}].Arg[1,/].Right[1].NotEqual[s]},s,]}!
		/declare out_Of_${track_Reageants[${i}].Arg[1,/].Replace[ ,]} bool outer TRUE
	}
}
/next i
/RETURN



|--------------------------------|
|- setup_Background_Services	-|
|--------------------------------|
SUB setup_Background_Services
/if (${Debug}) /echo |- setup_Background_Services ==>

|-------------------------------------------------------------------------------------------------------------------------------------check_Active
	
	/declare activeTimer timer outer
	/declare combatTimer timer outer
	/declare rebuffTimer timer outer
	/declare activeDelay int outer 5
	/declare combatDelay int outer 90
	/declare rebuffDelay int outer 90
	/declare last_Loc string outer
	/declare checkFollowTimer timer outer

	/if (!${Defined[idle]}) /declare idle bool global
	/if (!${Defined[idleTimer]}) /declare idleTimer timer global
	
	/declare currentZone int outer 0
	/declare safeZone bool outer
	
|----------------------------------------------------------------------------------------------------------------Import Macro_Ini settings
	/call IniToVar ${Macro_Ini},Background,AutoDestroy badItems string outer
	/call IniToVar ${Macro_Ini},Background,AutoInventoryTimer inventoryTimer string outer
	/call IniToVar ${Macro_Ini},Background,CloseSpellbookTimer spellbookTimer string outer
	/call IniToVar ${Macro_Ini},Background,IdleTimeOut idleInterval string outer

	/call INItoVar ${Macro_Ini},AutoTribute,AutoTribute(On/Off) autoTribute bool outer
	
	/call IniToArray ${Character_Ini},LifeSupport,LifeSupport# lifeSupport
	/call IniToArray ${Character_Ini},ReageantCheck,Reageant# check_Reageants
	
	
	/if (!${Ini[${safeZones_Ini}].Length}) /call Build_Alphabetized_Ini ${safeZones_Ini}

/if (${Debug}) /echo <== setup_Background_Services -|
/RETURN