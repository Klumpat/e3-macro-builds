|------------------------------------------------------------------------|
|- e3_Macro_Basics.inc v2.1	( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains basic and miscellaneous events and functions of e3.mac.		-|
|------------------------------------------------------------------------|



|----------------------------------------|
|- Manual command for EQBC Reconnect.	-|																
|----------------------------------------|
|- Use, [EQBC RECONNECT] in any channel -|
|----------------------------------------|
#event disconnect "#*#[EQBCRECONNECT]#*#"
#event disconnect "#*#**EQBC RECONNECT**#*#"
#event disconnect "#*#did not recieve expectd ping from server, pinging#*#"
SUB event_disconnect
/echo EQBC DISCONNECT!!!  Attempting to reconnect...
/bccmd 	quit
/delay 10s !${EQBC.Connected}
/bccmd connect
/delay 10s ${EQBC.Connected}
/if (!${EQBC.Connected}) /echo ***WARNING*** Could not connect to EQBCS, please open EQBCS and restart the macro.
/RETURN



|--------------------------------------------------------------------------------------------------------|
|- Commands all bots within range, including yourself, to bark a given message, at a specified NPC.		-|
|--------------------------------------------------------------------------------------------------------|
|- @param line: '<Name> Bark at MobName BarkMsg'														-|
|- @param msgTarget: The NPC you wish to bark at.														-|
|- @param barkIt: What to bark at the NPC.																-|
|- @param ChatSender: The command sender that requested bark.											-|
|- @param SpecifiedBots: Bots specified to bark at npc.													-|
|--------------------------------------------------------------------------------------------------------|
|- Targets must be within the FollowBreakDistance (default 250units).									-|
|- Use, '/bark MESSAGE', or, the command 'Bark at NAME MESSAGE' can be given in /bc, /g, or /tell.		-|
|--------------------------------------------------------------------------------------------------------|
#event bark "[#3#] Bark at #1# #2#"
#event bark "<#3#> Bark at #1# #2#"
#event bark "#3# tells the group, 'Bark at #1# #2#'"
#event bark "#3# tells you, 'Bark at #1# #2#'"
#event bark "[#3#] #4# Bark at #1# #2#"
#event bark "<#3#> #4# Bark at #1# #2#"
#event bark "#3# tells the group, '#4# Bark at #1# #2#'"
#event bark "#3# tells you, '#4# Bark at #1# #2#'"
SUB event_bark(line, msgTarget,  barkIt, ChatSender, SpecifiedBots)
/if (${Debug}) {
	/echo |- event_Bark ==>
	/echo |- event_Bark -| msgTarget=${msgTarget} || barkMsg=${barkIt} || ChatSender=${ChatSender} || SpecifiedBots=${SpecifiedBots}
}

|---------------------------------------------------------------------------------------------------------Reset ChatSender.
	/if (${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}

|--------------------------------------------------------Check SpecifiedBots
	/if (${Select[${Me.CleanName},${SpecifiedBots}]} || ${Select[${Me.Class.ShortName},${SpecifiedBots}]} || ${Select[${Me.Class},${SpecifiedBots}]} || !${Defined[SpecifiedBots]} || ${SpecifiedBots.Equal[all]} || ${SpecifiedBots.Equal[melee]} && ${Select[${Me.Class.ShortName},${MeleeClasses}]} || ${SpecifiedBots.Equal[casters]} && ${Select[${Me.Class.ShortName},${CasterClasses}]} || ${SpecifiedBots.Equal[ranged]} && ${Select[${Me.Class.ShortName},${RangedClasses}]} || ${SpecifiedBots.Equal[Healers]} && ${Select[${Me.Class.ShortName},${PriestClasses}]} || ${SpecifiedBots.Equal[Priests]} && ${Select[${Me.Class.ShortName},${PriestClasses}]}) {

	|------------------------------------------------------------------------Look for the spawn to bark at.
		/if (!${Int[${msgTarget}]}) /varset msgTarget ${Spawn[npc ${msgTarget}].ID}
		
		/if (!${SpawnCount[id ${msgTarget}]}) {
			/docommand ${ChatToggle} I don't see [${msgTarget}] in ${Zone}.
		} else {
		
		|---------------------------------------------------------------------------------------Check range to the spawn.
			/if (!${SpawnCount[pc ${ChatSender} radius ${FollowBreakDistance}]}) {
				/docommand ${ChatToggle} [${msgTarget}] is too far away to bark at.
			} else {
				
				/call TrueTarget ${msgTarget}
				
				/if (${Target.Distance} > 7) /call MoveToSpawn ${msgTarget}
				
			|-----------------------------------------Wait a moment, then bark the message.
				/delay ${Math.Rand[50]}
				/hail
				/say ${barkIt}	
			}	
		}
	}
	
/if (${Debug}) /echo <== event_Bark -|
/RETURN



#EVENT clickIt "<#1#> Click object at #2#"
|--------------------------------------------------------------------|
|- Moves to a given location, and clicks the nearest object.		-|
|--------------------------------------------------------------------|
|- @param locToStart: Where to stand when trying to click the door.	-|
|--------------------------------------------------------------------|
SUB EVENT_clickIt(line, ChatSender, locToStart)
| /varset Debug 1
/if (${Debug}) /echo |- EVENT_clickIt ==>

	/if (${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}
	/if (${ChatSender.Equal[You]}) /varset ChatSender ${Me.CleanName}
	
|-----------------------------------------------------------------------------------------------------------------------------------------------------Check that you are in the same zone as the chatsender.
	/if (!${SpawnCount[pc ${ChatSender}]}) {
		/docommand ${ChatToggle} ${ChatSender} and I are not in the same zone.
	} else {
	
	|-----------------------------------------------------------------------------------------------------------------------------------------------------Check that you can see the location to click from.
		/if (!${LineOfSight[${Me.Loc.Replace[ ,]}:${locToStart}]}) {
			/docommand ${ChatToggle} I cannot see ${ChatSender}.
		} else {
			
		|-------------------------------------------------------------------------------------------------------------------------------------------------------------Check distance to the destination loc.
			/call Triangulate_Distance ${locToStart} ${Me.Loc.Replace[ ,]}
			/if (${Debug}) /echo |- EVENT_clickIt -| Distance to click location= [${Macro.Return}]
			
		|----------------------------------------------------------------------------------------------------------If the location is greater than your MoveToBreakDistance (default 750), announce to user.
			/if (${Macro.Return} > ${MoveToBreakDistance}) {
				/docommand ${ChatToggle} I am too far away to click that object.
			} else {
			
			|----------------------------------------------------------------------------------------------------------------------If you're the chatsender, allow bots a few moments to initiate the event.
				/if (${Me.CleanName.Equal[${ChatSender}]}) /delay 50
			
			|-----------------------------------------------------------------------------------------------------------------------------------------------------------------Switch to first person camera.
				/keypress First_Person_Camera
			
			|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------Declare variables.
				/declare retryTimer timer local 1200
				/declare miscTimer timer local
				
				/declare startZone int local ${Zone.ID}
				/declare startLoc string local
				
				:retry
				/if (${Debug}) /echo |- EVENT_clickIt -| :retry
				
			|-----------------------------------------------------------------------------------------------------------------------------------------------------------------Move to the starting location.
				/if (${Macro.Return} > 8) /call MoveToLoc ${locToStart.Arg[1,,]} ${locToStart.Arg[2,,]}
				
			|---------------------------------------------------------------------------------------------------------------------------------------------Face object and attempt to click it for 5 seconds.
				/varset miscTimer 50
				/varset startLoc ${Me.Loc.Replace[ ,]}
				/squelch /doortarget
				/squelch /face fast door
				/squelch /look -75
				
				:keypress
				/if (${Debug}) /echo |- EVENT_clickIt -| :keypress
				
			|------------------------------------------------------------------------------------------------------------------------------------------------------Shifts look angle up and down as I click.
				/look ${If[${Me.Look} < 75,${Math.Calc[${Me.Look} + 5]},-75]}
				/keypress U
				/delay 2
				
			|--------------------------------------------------------------------------------------------------------------------Calculate distance from my starting location, to see if I have been warped.
				/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${locToStart}
		
			|------------------------------------------------------------------------------------------------------------If I am still in the same zone, and have not moved more than 5 units, check timers.
				/if (${Zone.ID} == ${startZone} && ${Int[${Macro.Return}]} < 50) {
					/if (${miscTimer}) {
						/goto :keypress
					} else {
						/if (${retryTimer}) {
							/if (${Debug}) /echo |- EVENT_clickIt -| Click attempt failed, retrying.
							/squelch /target clear
							/keypress back hold
							/delay 5
							/keypress forward
							/delay ${Math.Rand[30]}
							/goto :retry
						} else {
							/docommand ${ChatToggle} I have failed to click the object.
						}
					}
				}
			}
		}
	}
	
/if (${Debug}) /echo  <== EVENT_clickIt -|
| /varset Debug 0
/RETURN



|**
|--------------------------------------------------------------------|
|- Adds a run through zone to the route you're currently recording.	-|
|--------------------------------------------------------------------|
#event add_run_Thru_Zone "[MQ2] Add Run thru zone"
#event add_run_Thru_Zone "[MQ2] Add Run through zone"
SUB event_add_run_Thru_Zone
/if (${Debug}) /echo |- add_run_Thru_Zone ==>

	/if (${keep_Tracing}) {
		/echo Adding run through zone function to your route...
		/call event_end_Recording
		/echo ** To continue this route through a zone line, please save the route in this zone, then record a route in your destination zone with the same name. For example, to go from the Guild Lobby to the Plane of Time zone in Tranquility, you'd need a route named 'Plane of Time' in both PoKnowledge and PoTranquility. **
		/call BuildArray ${temp_Route_Name} ${Me.Loc.Replace[ ,]}_**runThruZone_${Me.Heading.Degrees}_${Me.Look}
	} else {
		/echo I'm not currently recording a route, dumbass.
	}
	
/if (${Debug}) /echo <== add_run_Thru_Zone -|	
/RETURN



|----------------------------------------------------------------------------|
|- Runs through a zone line on a recorded route.							-|
|----------------------------------------------------------------------------|
|- @param locToStart: Where to stand when trying to click the door.			-|
|- @param snapHeading: The direction to face when trying to click the door.	-|
|- @param snapLook: The angle to look when trying to click the door.		-|
|- @param startZone: Zone you are attempting to leave.						-|
|----------------------------------------------------------------------------|
SUB run_Thru_Zone(locToStart, float snapHeading, float snapLook, int startZone)
/if (${Debug}) /echo |- run_Thru_Zone ==>

	/declare tryToZone_Timer timer local ${tryToZone_Timer_Setting}

|-------------------------------------------------------------------------------------Switch to first person camera.
	/keypress First_Person_Camera
	:retry
	/if (${Zone.ID}==${startZone}) {
	
	|-------------------------------------------------------------------------------------------------Move to the zone line I want to run through.
		/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${locToStart}
		/if (${Macro.Return} > 8) {
			/squelch /moveto loc ${locToStart.Arg[1,,]} ${locToStart.Arg[2,,]} dist 8
			/delay 1s ${Me.Moving}
			/delay 5s !${Me.Moving}
		}
		
	|-------------------------------------------------------------------------------------------------Face the zone and run foward for 15 seconds.
		/face fast heading ${Math.Calc[${snapHeading}*-1]}
		/look ${snapLook}
		/delay 5
		/keypress forward hold
		/delay 15s
		/keypress back
		
	|------------------------------------------------------------------------------------------------If I haven't zoned, move back, and try again.
		/if (${Zone.ID} && ${Zone.ID} == ${startZone} && ${tryToZone_Timer}) {
			/squelch /target clear
			/keypress back hold
			/delay 5
			/keypress forward
			/delay ${Math.Rand[50]}
			/goto :retry
		}
		/squelch /stick off	
	}
	
/if (${Debug}) /echo <== run_Thru_Zone -|
/RETURN

**|



|--------------------------------------------------------------------------------------------------------------------|
|- Engages, and sets variables related to Follow.																	-|
|--------------------------------------------------------------------------------------------------------------------|
|- Only bots in the same zone will engage follow.																	-|
|- Bots will wait to engage follow until, they can see the FollowTarget, and it is within the FollowBreakDistance.	-|
|- If bots are assisting, they will back off and follow until re-engage.											-|
|--------------------------------------------------------------------------------------------------------------------|
#event Follow "<#1#> Follow"
#event Follow "<#1#> Follow #3#"
#event Follow "<#1#> #2# Follow #3#"
#event Follow "#1# tells the group, 'Follow'"
#event Follow "#1# tells the group, 'Follow #3#'"
#event Follow "#1# tells the group, '#2# Follow #3#'"
#event Follow "#1# tells you, 'Follow'"
#event Follow "#1# tells you, 'Follow #3#'"
#event Follow "#1# tells you, '#2# Follow #3#'"
SUB event_Follow(Line, ChatSender, SpecifiedBots, SpawnToFollow)
/if (${Debug}) /echo |- event_Follow ==>

|--------------------------------------------------------------------------------------------Clean up, event variables.
	
	/if (${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}
	/if (${SpawnToFollow.Equal[me]} || ${SpawnToFollow.Equal[on]}) /varset SpawnToFollow ${ChatSender}
	/if (!${Defined[SpawnToFollow]}) /declare SpawnToFollow string local ${ChatSender}
	
|----------------------------------------------------------------------------------------------------------------------------Set follow variables.
	
|-------------------------------------------------------------------------Check to see if the SpawnToFollow is in zone.
	/if (!${SpawnCount[pc ${SpawnToFollow}]}) {
		/if (${Verbosity}) /docommand ${ChatToggle} I can't find ${SpawnToFollow}, we're not in the same zone.
	} else {
	
	|------------------------------------------------------Stop assisting so we don't try to stick to the AssistTarget.	
		/if (${Assisting} || ${Me.Combat}) /call Event_BackOff
		
	|--------------------------------------------------------Check SpecifiedBots
		/if (${Select[${Me.CleanName},${SpecifiedBots}]} || ${Select[${Me.Class.ShortName},${SpecifiedBots}]} || ${Select[${Me.Class},${SpecifiedBots}]} || !${Defined[SpecifiedBots]} || ${SpecifiedBots.Equal[all]} || ${SpecifiedBots.Equal[melee]} && ${Select[${Me.Class.ShortName},${MeleeClasses}]} || ${SpecifiedBots.Equal[casters]} && ${Select[${Me.Class.ShortName},${CasterClasses}]} || ${SpecifiedBots.Equal[ranged]} && ${Select[${Me.Class.ShortName},${RangedClasses}]} || ${SpecifiedBots.Equal[Healers]} && ${Select[${Me.Class.ShortName},${PriestClasses}]} || ${SpecifiedBots.Equal[Priests]} && ${Select[${Me.Class.ShortName},${PriestClasses}]}) {
			
		|--------------------------------------------------------------------------------------------------------------If I am not the chatsender.
			/if (${ChatSender.NotEqual[${Me.CleanName}]}) {
				
			|-------------------------------------------------------------------Turn Following on and set the FollowTarget.
				/varset Following TRUE
				/varset FollowTarget ${Spawn[pc ${SpawnToFollow}].CleanName}
			
			|----------------------------------------------------If FollowTarget is outside of the FollowBreakDistance.
				/if (${Spawn[${FollowTarget}].Distance} > ${FollowBreakDistance} || !${Spawn[${FollowTarget}].LineOfSight}) {
					/if (${Verbosity}) /docommand ${ChatToggle} ${FollowTarget} I can't find you, ${FollowTarget}.  I'll start following when I do.
				} else {
				
				|--------------------------------------------------------------------------------------Start Following.
					/if (${Verbosity}) /docommand ${ChatToggle} Following ${FollowTarget}
					/call AquireFollow
				}	
			} else {
			
		|------------------------------------------------------------------------------------------------------------------If I am the chatsender.	
			
			|-------------------------------------------------------------------Turn Following on and set the FollowTarget.
				/varset Following TRUE
				/varset FollowTarget ${Me.CleanName}
				/if (${Stick.Active}) /squelch /stick off
				/if (${NetAdvPath.State}) /squelch /netfollow off

				/varset Followers
				/declare i int local
				/for i 1 to ${NetBots.Counts}
					/if (${NetBots.Client.Arg[${i}, ].NotEqual[${Me.CleanName}]} && ${SpawnCount[pc ${NetBots.Client.Arg[${i}, ]}]} && ${Spawn[pc ${NetBots.Client.Arg[${i}, ]}].Distance}<=${FollowBreakDistance}) /varset Followers ${Followers}${NetBots.Client.Arg[${i}, ]},
				/next i
				/if (${Debug}) /echo |- event_Follow -| Followers= [${Followers}]
			}
		}
	}

/if (${Debug}) /echo <== event_Follow -|
/RETURN



|----------------------------------------------------------------------------------------------------|
|- Engages follow plugins on specified follow targets.												-|
|----------------------------------------------------------------------------------------------------|
|- Uses NetAdvPath if the follow target is one of your bots, or MoveUtils, if its someone elses.	-|		
|----------------------------------------------------------------------------------------------------|
SUB AquireFollow
/if (${Debug}) /echo |- AquireFollow ==>

|-------------------Validate FollowTarget, make sure they are in zone, within range, and you have line of sight on them
	/if (${Spawn[pc ${FollowTarget}].ID} && ${FollowTarget.NotEqual[${Me.CleanName}]} && ${Spawn[pc ${FollowTarget}].Distance}<${FollowBreakDistance}) {

	|-----------------------------------------------------------------------------Turn off any previous follow plugins.
		/if (${Stick.Active}) /squelch /stick off
		/if (${NetAdvPath.State}==1) /squelch /netfollow off
		
	|-------------------------------------------------------------------------------------------------------------If the FollowTarget is a NetBot.
		/if (${NetBots[${FollowTarget}].ID}) {

		|-----------------------------------------------If I have line of sight on the FollowTarget, engage NetAdvPath.
			/if (${Spawn[pc ${FollowTarget}].LineOfSight}) {

			|------------------------------------------------------------------------------------------Target the FollowTarget.
				/call TrueTarget ${Spawn[pc ${FollowTarget}].ID}
				/squelch /netfollow on
				/delay 5 ${Me.Moving}
		|--------------If I don't have line of sight, check to see if NetAdvPath is paused, if so re-engage NetAdvPath.
			} else /if (${NetAdvPath.State}==2) {
				/squelch /netfollow unpause
				/delay 5 ${Me.Moving}
			}
|** ADD A LOOP TO CALCULATE DISTANCE TO YOUR TARGET. ONCE DISTANCE STOPS DECREASING REISSUE FOLLOW COMMAND, TO SNAP TO TARGET
			/declare HeadingToSpawn string local ${Spawn[pc ${FollowTarget}].HeadingTo}
			:movingLoop
			/if (${Me.Moving}) {
				/if () {
				
				}
			}
			
			**|
		} else {
		
	|---------------------------------------------------If MQ2NetAdvPath was not detected, or the FollowTarget was not a NetBot, engage MoveUtils.
			/call TrueTarget ${Spawn[pc ${FollowTarget}].ID}
			/echo ${If[!${NetAdvPath},"MQ2NetAdvPath was not detected, defaulting to MQ2MoveUtils.","${FollowTarget} is not a NetBot,  defaulting to MQ2MoveUtils."]}
			/squelch /stick hold 20 uw
		}
		/if (${Target.ID}) /squelch /target clear
	}
	
/if (${Debug}) /echo <== AquireFollow -|
/RETURN



|----------------------------------------------------------------------------|
|- Disengages follow on bots who are in the same zone as the ChatSender.	-|
|----------------------------------------------------------------------------|
|- Use, /stop, or, 'stop' in /tell, /goup, or /bc.							-|
|----------------------------------------------------------------------------|
#event Stop	"<#1#> Stop"
#event Stop	"<#1#> #2# Stop"
#event Stop "#1# tells you, 'Stop'"
#event Stop "#1# tells you, '#2# Stop'"
#event Stop "#1# tells the group, 'Stop'"
#event Stop "#1# tells the group, '#2# Stop'"
SUB event_Stop(Line, ChatSender, SpecifiedBots)
/if (${Debug}) /echo |- event_Stop ==>

	/if (${Following}) {
	
	|--------------------------------------------------------------------------------------------Clean up, event variables.
		/if (${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}
		
	|-------------------------------------------------------------------------Check to see if the ChatSender is in zone.	
		/if (${SpawnCount[pc ${ChatSender}]}) {
		
		|--------------------------------------------------------Check SpecifiedBots
			/if (${Select[${Me.CleanName},${SpecifiedBots}]} || ${Select[${Me.Class.ShortName},${SpecifiedBots}]} || ${Select[${Me.Class},${SpecifiedBots}]} || !${Defined[SpecifiedBots]} || ${SpecifiedBots.Equal[all]} || ${SpecifiedBots.Equal[melee]} && ${Select[${Me.Class.ShortName},${MeleeClasses}]} || ${SpecifiedBots.Equal[casters]} && ${Select[${Me.Class.ShortName},${CasterClasses}]} || ${SpecifiedBots.Equal[ranged]} && ${Select[${Me.Class.ShortName},${RangedClasses}]} || ${SpecifiedBots.Equal[Healers]} && ${Select[${Me.Class.ShortName},${PriestClasses}]} || ${SpecifiedBots.Equal[Priests]} && ${Select[${Me.Class.ShortName},${PriestClasses}]}) {
				/if (${Stick.Active}) /squelch /stick off
				/if (${NetAdvPath.State}==1) /squelch /netfollow off
				/varset FollowTarget NULL
				/varset Following FALSE
				/if (${Verbosity} && ${ChatSender.Equal[${Me.CleanName}]}) /docommand ${ChatToggle} I am no longer following.
			}
		}
	}
	
/if (${Debug}) /echo <== event_Stop -|
/RETURN



#event MoveHere "<#1#> Move to #2#"
#event MoveHere "<#1#> #3# Move to #2#"
#event MoveHere "#1# tells you, 'Move to #2#'"
#event MoveHere "#1# tells you, '#3# Move to #2#'"
#event MoveHere "#1# tells the group, 'Move to #2#'"
#event MoveHere "#1# tells the group, '#3# Move to #2#'"
SUB event_MoveHere(line, ChatSender, WhereTo, SpecifiedBots)
/if (${Debug}) /echo |- event_MoveHere ==>

	|-----------------------------------Validate ChatSender, and SpecifiedBots.
	/if (${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}
	/if (${WhereTo.Equal[me]} || ${WhereTo.Equal[here]}) /varset WhereTo ${Spawn[pc ${ChatSender}].CleanName}
	/if (${SpawnCount[pc ${ChatSender}]} && ${SpawnCount[pc ${WhereTo} radius ${MoveToBreakDistance}]}) {
		|--------------------------------------------------------Check SpecifiedBots
		/if (${Select[${Me.CleanName},${SpecifiedBots}]} || ${Select[${Me.Class.ShortName},${SpecifiedBots}]} || ${Select[${Me.Class},${SpecifiedBots}]} || !${Defined[SpecifiedBots]} || ${SpecifiedBots.Equal[all]} || ${SpecifiedBots.Equal[melee]} && ${Select[${Me.Class.ShortName},${MeleeClasses}]} || ${SpecifiedBots.Equal[casters]} && ${Select[${Me.Class.ShortName},${CasterClasses}]} || ${SpecifiedBots.Equal[ranged]} && ${Select[${Me.Class.ShortName},${RangedClasses}]} || ${SpecifiedBots.Equal[Healers]} && ${Select[${Me.Class.ShortName},${PriestClasses}]} || ${SpecifiedBots.Equal[Priests]} && ${Select[${Me.Class.ShortName},${PriestClasses}]}) {
			/if (${ChatSender.NotEqual[${Me}]}) {
			|------------------------------------------------------Stop assisting so we don't try to stick to the AssistTarget.	
				/if (${Assisting} || ${Me.Combat}) /call Event_BackOff
			
			|------------------------------------------------------------------Move to the WhereTo target.
				/moveto id ${Spawn[pc ${WhereTo}].ID} uw dist 10
				/delay 5 !${Me.Moving}
				/declare Moving_Timer timer local 100
				:moveto_Loop
				/if (${Debug}) /echo |- event_MoveHere -| :moveto_Loop
				
				/call Background_Events
				
				/if (!${SpawnCount[pc ${WhereTo} radius 15]}) {
					/if (${Me.Moving} && ${Moving_Timer}) {
						/goto :moveto_Loop
					} else {
						/echo I failed to move to [${WhereTo}]
					}
				}
				
				/if (${Following}) {
					/if (${WhereTo.Equal[${FollowTarget}]}) {
						/call AquireFollow
					} else {
						/call event_Stop NULL ${ChatSender}
					}
				}
			}
		}
	}
	
/if (${Debug}) /echo <== event_MoveHere -|
/RETURN



|--------------------------------------------------------------------------------------------|
|- Groupleaders save current group formations to the Groups.ini file, to be recalled later.	-|
|--------------------------------------------------------------------------------------------|
|- Use, /savegroup 'Name', /bc Save Group 'Name', or /bc SetGroup 'Name'					-|
|--------------------------------------------------------------------------------------------|
#event setGroup "<#*#> Save Group #1#"
#event setGroup "<#*#> SetGroup #1#"
SUB event_setGroup(line, groupName)
/if (${Select[${Group.Leader},${Me}]} && ${Group}) {
	/if (!${Ini[${Group_Ini},${groupName}_${Me}].Length} || ${line.Right[9].Equal[Overwrite]}) {
		/echo Saving current group. Use /group ${groupName} to recall this group.
		/declare i int local
		/for i 1 to ${Group}
			/ini ${Group_Ini} "${groupName}_${Me}" GroupMember${i} ${Group.Member[${i}]}
		/next i
	} else /if (${Ini[${Group_Ini},${groupName}_${Me}].Length}) {
		/echo setGroup Error: ${groupName} is already in use.  Please choose another name and try again.
	}
} else /if (!${Group}) {
	/echo setGroup Error: You need to make a group first, dumbass.
}
/RETURN



|------------------------------------------------|
|- Recalls groups saved in your Groups.ini.		-|
|------------------------------------------------|
|- Use, /group 'Name', or, /bc Group 'Name'.	-|
|------------------------------------------------|
#event GroupUp "<#*#> Group #1#"
SUB event_GroupUp(line, groupName)
/if (${Group} || ${Raid.Members}) {
	/call TrueTarget ${Me.ID}
	/raiddisband
	/delay 5
	/disband
	/delay 5s !${Group} && ${Raid.Members} == 0
}
/if (${Ini[${Group_Ini},${groupName}_${Me}].Length}) {
	/if (${Me.Casting.ID}) /delay 30s ${Me.Casting.ID}	
	/echo Recalling [${groupName}] group members...
	
	|- Build a list of people to invite
	/call IniToArray ${Group_Ini},${groupName}_${Me},GroupMember groupMembers
	/declare i int local
	/declare groupSize int local 0
	|- Invite each person, and make sure they accept
	/for i 1 to ${groupMembers.Size}
		/echo Inviting ${groupMembers[${i}]}
		/invite ${groupMembers[${i}]}
		/delay 1s
	/next i
}
/RETURN



|----------------------------------------|
|- Automatically accepts group invites.	-|
|----------------------------------------|
#event Invite "#*# invites you to join a group."
SUB event_Invite
/if (!${Group} && ${Raid.Members}) {
	/call TrueTarget ${Me.ID}
	/raiddisband
	/delay 5s !${Group} && ${Raid.Members} == 0
}
	/declare i int local 0
	:retry_Invite
	/call Background_Events
	/varcalc i ${i} + 1
	/invite
	/delay 5 ${Group}
	/if (${Debug}) /echo |- event_Invite -| retry_Invite loop.
	/if (!${Group} && ${i}<50) /goto :retry_Invite
/RETURN



|----------------------------------------|
|- Disband bots from group and raid.	-|
|----------------------------------------|
|- Use, /bc disband						-|
|----------------------------------------|
#event Disband "<#*#> Disband all"
SUB event_Disband
/call TrueTarget ${Me.ID}
/if (${Group} || ${Raid.Members}) {
	/target pc ${Me}
	/delay 1s ${Target.ID} == ${Me.ID}
	/raiddisband
	/delay 5
	/disband
	/delay 5s !${Group} && ${Raid.Members} == 0
}
/squelch /target clear
/RETURN

| Corpse drag |
#event Consent "#*# tells you, 'Consent #1#'"
#event Consent "<#*#> Consent #1#"
SUB Event_Consent(line, ConsentTarget)
/if (${Verbosity}) /docommand ${ChatToggle} Consenting ${ConsentTarget}
/consent ${ConsentTarget}
/RETURN
#event addConsent "You have been given permission to drag #1#'s corpse in all zones."
SUB event_addConsent(line, consentTarget)
/if (!${Select[${consentTarget},${consentTargets}]}) {
	/varset consentTargets ${consentTargets},${consentTarget}
}
/RETURN
#event flushConsent "<#*#> Flush consent"
SUB event_flushConsent
/echo Flushing my current consented targets.
	/if (${Defined[consentTargets]}) /varset consentTargets ${Me.CleanName}
/RETURN
#event pingCorpses "<#1#> Ping corpses"
SUB event_pingCorpses(ChatSender)
/if (${Defined[consentTargets]}) {
	/declare i int local
	/for i 1 to ${SpawnCount['s radius 100]}
		/if (${Select[${Spawn[${NearestSpawn[${i},'s radius 100]}].CleanName.Arg[1,']},${consentTargets}]}) {
			/call TrueTarget ${Spawn[${NearestSpawn[${i},'s radius 100]}].ID}
			/corpse
		}
	/next i
}
/squelch /target clear
/RETURN



|--------------------------------------------------------|
|- Attempt to gate on all bots.							-|
|--------------------------------------------------------|
|- Use, /bc gate, /tell 'Name' [Gate], or, /g [Gate].	-|
|--------------------------------------------------------|
#event gate	"<#1#> [Gate]"
#event gate "#1# tells you, '[Gate]'"
#event gate "#1# tells the group, '[Gate]'"
#event gate	"<#1#> #2# [Gate]"
#event gate "#1# tells you, '#2# [Gate]'"
#event gate "#1# tells the group, '#2# [Gate]'"
SUB event_gate(Line, ChatSender, SpecifiedBots)
/if (${Debug}) /echo |- event_Gate ==>

|--------------------------------------------------------Check SpecifiedBots
	/if (${Select[${Me.CleanName},${SpecifiedBots}]} || ${Select[${Me.Class.ShortName},${SpecifiedBots}]} || ${Select[${Me.Class},${SpecifiedBots}]} || !${Defined[SpecifiedBots]} || ${SpecifiedBots.Equal[all]} || ${SpecifiedBots.Equal[melee]} && ${Select[${Me.Class.ShortName},${MeleeClasses}]} || ${SpecifiedBots.Equal[casters]} && ${Select[${Me.Class.ShortName},${CasterClasses}]} || ${SpecifiedBots.Equal[ranged]} && ${Select[${Me.Class.ShortName},${RangedClasses}]} || ${SpecifiedBots.Equal[Healers]} && ${Select[${Me.Class.ShortName},${PriestClasses}]} || ${SpecifiedBots.Equal[Priests]} && ${Select[${Me.Class.ShortName},${PriestClasses}]}) {

		/if (!${Select[${Me.Book[Gate]},NULL]}) {
			/docommand ${ChatToggle} Preparing to gate...
			/call e3_Cast ${Me.ID} "Gate" gem${DefaultGem} ${Math.Calc[${Spell[Gate].RecastTime}+3]}s
		} else  /if (${Select[${Me.Book[Gate]},NULL]}) {
			/if (${FindItem[Philter of Major Translocation].ID}) {
				/docommand ${ChatToggle} Preparing to gate...
				/call e3_Cast ${Me.ID} "Philter of Major Translocation" item
			} else /if (${FindItem[Vial of Swirling Smoke].ID}) {
				/docommand ${ChatToggle} Preparing to gate...
				/call e3_Cast ${Me.ID} "Vial of Swirling Smoke" item
			} else {
				/docommand ${ChatToggle} I am not able to gate at this time.
			}
		}
	}
	
/if (${Debug}) /echo <== event_Gate -|
/RETURN



|--------------------------------------------------------|
|- Request evac from bots set to Evac_OnCommand.		-|
|--------------------------------------------------------|
|- Use, /bc Evac, /tell 'Name' [Evac], or, /g [Evac].	-|
|--------------------------------------------------------|
#event Evacuate "<#*#> Evac"
#event Evacuate "#*# tells you, '[Evac]'"
#event Evacuate "#*# tells the group, '[Evac]'"
SUB event_Evacuate
/if (${Debug}) /echo |- event_Evacuate ==>

	/if (${castEvac}) {
		/if (${Me.Casting.ID}) /interrupt
		/docommand ${ChatToggle} Evacuating.
		/if (${Me.AltAbilityReady[Exodus]}) {
			/call e3_Cast ${Me.ID} "Exodus" alt
		} else /if (${Me.Book[${Evac_Spell.Arg[1,/]}]}) {
			/call e3_Cast ${Me.ID} "${Evac_Spell.Arg[1,/]}" ${If[${Evac_Spell.Arg[2,/].Length} && ${Evac_Spell.Arg[2,/].Find[gem]},${Evac_Spell.Arg[2,/]},Gem${DefaultGem}]} ${Math.Calc[${Spell[${Evac_Spell}].RecastTime}+3]}s
		} else {
			/docommand ${ChatToggle} I am not able to Evac at this time.
		}
	}

/if (${Debug}) /echo <== event_Evacuate -|
/RETURN



|--------------------------------------------------------|
|- Searches inventory, and banks, for indicated items.	-|
|--------------------------------------------------------|
|- Use, /bc Find 'Name', or, /echo Find 'Name'.			-|
|--------------------------------------------------------|
#event FindItems "<#*#> Find Item [#1#]"
#event FindItems "<#*#> Find [#1#]"
#event FindItems "<#*#> Find Item #1#"
#event FindItems "<#*#> Find #1#"
#event FindItems "[#*#] Find Item [#1#]"
#event FindItems "[#*#] Find [#1#]"
#event FindItems "[#*#] Find Item #1#"
#event FindItems "[#*#] Find #1#"
SUB event_FindItems(line, ItemToFind)
/if (${Debug}) /echo |- EVENT_FindItems ==>
	
|--------------------------------------------------------------------------------------------------------------------------------------------------------Declare counting variables.
	/declare i int local
	/declare e int local
	/declare itemName string local
	
|---------------------------------------------------------------------------------------------------------------------------Check Inventory.
	/if (${FindItemCount[${ItemToFind}]}) {
	
	|------------------------------------------------------------------Search packslots.
		/for i 1 to 8
			
			/varset itemName ${Me.Inventory[pack${i}]}
			/if (${itemName.Find[${ItemToFind}]}) /docommand ${ChatToggle} Found [${Me.Inventory[pack${i}]}${If[${Me.Inventory[pack${i}].StackSize} > 1,(${Me.Inventory[pack${i}].StackCount}),]}] in backpack slot [${i}].
			
			/if (${Me.Inventory[pack${i}].Container}) {
				/for e 1 to ${Me.Inventory[pack${i}].Container}
					/varset itemName ${Me.Inventory[pack${i}].Item[${e}]}
					/if (${itemName.Find[${ItemToFind}]}) /docommand ${ChatToggle} Found [${Me.Inventory[pack${i}].Item[${e}]}${If[${Me.Inventory[pack${i}].Item[${e}].StackSize} > 1,(${Me.Inventory[pack${i}].Item[${e}].StackCount}),]}] in [${Me.Inventory[pack${i}]}(${i})] bag slot [${e}].
				/next e
			}
			
		/next i
	}
	
	/if (${FindItemBankCount[${ItemToFind}]}) {
	
	|------------------------------------------------------------------Search packslots.
		/for i 1 to 26
			
			/varset itemName ${Me.Bank[${i}]}
			/if (${itemName.Find[${ItemToFind}]}) /docommand ${ChatToggle} Found [${Me.Bank[${i}]}${If[${Me.Bank[${i}].StackSize} > 1,(${Me.Bank[${i}].StackCount}),]}] in bank slot [${i}].
			
			/if (${Me.Bank[${i}].Container}) {
				/for e 1 to ${Me.Bank[${i}].Container}
					/varset itemName ${Me.Bank[${i}].Item[${e}]}
					/if (${itemName.Find[${ItemToFind}]}) /docommand ${ChatToggle} Found [${Me.Bank[${i}].Item[${e}]}${If[${Me.Bank[${i}].Item[${e}].StackSize} > 1,(${Me.Bank[${i}].Item[${e}].StackCount}),]}] in my bank. [${Me.Bank[${i}]}(${i})], bag slot [${e}].
				/next e
			}
			
		/next i
	}

/if (${Debug}) /echo <== EVENT_FindItems -|
/RETURN



|------------------------------------------------------------|
|- Checks for specified buffs and debuffs on NetBots.		-|
|------------------------------------------------------------|
|- Use, /bc Check Buff [Name], or /bc Check buff 'Name'.	-|
|------------------------------------------------------------|
#event CheckBuffs "<#*#> Check buff [#1#]"
#event CheckBuffs "<#*#> Check [#1#]"
#event CheckBuffs "<#*#> Check buff #1#"
#event CheckBuffs "<#*#> Check #1#"
#event CheckBuffs "[#*#] Check buff [#1#]"
#event CheckBuffs "[#*#] Check [#1#]"
#event CheckBuffs "[#*#] Check buff #1#"
#event CheckBuffs "[#*#] Check #1#"
SUB event_CheckBuffs(line, BuffToCheck)
/if (${Debug}) /echo |- EVENT_CheckBuffs ==>

	/if (${Me.Buff[${BuffToCheck}].ID}) /docommand ${ChatToggle} I currently have [${Math.Calc[${Me.Buff[${BuffToCheck}].Duration} * 6 / 60].Int} minutes] left on ${BuffToCheck}

/if (${Debug}) /echo <== EVENT_CheckBuffs -|
/RETURN



|----------------------------------------------------|
|- Attempts to remove specified buffs on NetBots.	-|
|----------------------------------------------------|
|- Use, /bc Remove [Name], or /bc Remove 'Name'.	-|
|----------------------------------------------------|
#event RemoveBuff "<#*#> Remove buff [#1#]"
#event RemoveBuff "<#*#> Remove buff #1#"
#event RemoveBuff "[#*#] Remove buff [#1#]"
#event RemoveBuff "[#*#] Remove buff #1#"
#event RemoveBuff "<#*#> Remove [#1#]"
#event RemoveBuff "<#*#> Remove #1#"
#event RemoveBuff "[#*#] Remove [#1#]"
#event RemoveBuff "[#*#] Remove #1#"
SUB event_RemoveBuff(line, badBuff)
| /varset Debug TRUE
/if (${Debug}) /echo |- EVENT_RemoveBuff ==>

	/declare i int local

	/for i 0 to 29
		
		/if (${Bool[${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip}]} && ${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip.Find[${badBuff}]}) {
			/varset badBuff ${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip.Arg[1,(]}
			/varset badBuff ${badBuff.Left[${Math.Calc[${badBuff.Length}-1]}]}
			/if (${Debug}) /echo |- EVENT_RemoveBuff -| Removing [${badBuff}]
			/nomodkey /notify BuffWindow Buff${Math.Calc[${Me.Buff[${badBuff}].ID}-1].Int} leftmouseup
		}
		
	/next i

/if (${Debug}) /echo <== EVENT_RemoveBuff -|
| /varset Debug FALSE
/RETURN



|------------------------------------------------------------|
|- Checks for bots who are out of range, or out of zone.	-|
|------------------------------------------------------------|
|- Use, /count, or, /bc Count bots							-|
|------------------------------------------------------------|
#event CountBots "[#*#] Count"
#event CountBots "<#*#> Count"
#event CountBots "[#*#] Count Bots"
#event CountBots "<#*#> Count Bots"
#event CountBots "[#*#] RollCall"
#event CountBots "<#*#> RollCall"
#event CountBots "[#*#] Roll Call"
#event CountBots "<#*#> Roll Call"
SUB event_CountBots
/if (${Debug}) /echo |- event_CountBots ==>

	/if (!${NetBots.Counts}) {
		/echo You must be connected to MQ2EQBC, and, MQ2NetBots, to use this function.
	} else {
		/declare i int local
		/declare OutOfZone string local
		/declare OutOfRange string local 
		/for i 1 to ${NetBots.Counts}
		/if (!${SpawnCount[pc ${NetBots.Client[${i}]}]}) {
			/varset OutOfZone ${OutOfZone}${NetBots.Client[${i}]},
		} else /if (${SpawnCount[pc ${NetBots.Client[${i}]}]} && !${SpawnCount[pc ${NetBots.Client[${i}]} radius ${FollowBreakDistance}]}) {
			/varset OutOfRange ${OutOfRange}${NetBots.Client[${i}]},
		}
		/next i
	}
	
	/if (${OutOfZone.Length}) {
		/varset i 1
		/echo Clients Out Of Zone:
		/echo ---------------------------------
		:next_OutOfZone
		/echo [${i}] ${OutOfZone.Arg[${i},,]} is in, ${Zone[${NetBots[${OutOfZone.Arg[${i},,]}].Zone}]}.
		/varcalc i ${i} + 1
		/if (${OutOfZone.Arg[${i},,].Length}) /goto :next_OutOfZone
		/echo ---------------------------------
	}
	
	/if (${OutOfRange.Length}) {
		/varset i 1
		/echo Clients Out Of Range:
		/echo ---------------------------------
		:next_OutOfRange
		/echo [${i}] ${OutOfRange.Arg[${i},,]}, ${Int[${Spawn[pc ${OutOfRange.Arg[${i},,]}].Distance}]} units away.
		/varcalc i ${i} + 1
		/if (${OutOfRange.Arg[${i},,].Length}) /goto :next_OutOfRange
		/echo ---------------------------------
	}
	
	/if (!${OutOfRange.Length} && !${OutOfZone.Length}) /echo ${NetBots.Counts} bots are present.
	
/if (${Debug}) /echo <== event_CountBots -|
/RETURN



|------------------------------------------------|
|- All bots click yes to confirmation boxes.	-|
|------------------------------------------------|
|- Use, /clickyes, or, /bc Click yes			-|
|------------------------------------------------|
#event ClickYes	"<#1#> Click Yes#"
SUB event_ClickYes
/if (${Debug}) /echo |- event_ClickYes ==>

	/notify ConfirmationDialogBox Yes_Button leftmouseup

/if (${Debug}) /echo <== event_ClickYes -|
/RETURN



|----------------------------------------------------|
|- Uses /exit on all bots, except the ChatSender.	-|
|----------------------------------------------------|
|- Use, /bc Exit, or, -[Exit]- in any channel.		-|
|----------------------------------------------------|
#event Exit "#*# tell you, '-[Exit]-'"
#event Exit "<#1#> Exit"
SUB event_Exit(line, ChatSender)
/docommand ${ChatToggle} Peace fucker!
/if (${Spawn[${ChatSender}].ID}!=${Me.ID}) /exit
/RETURN



|------------------------------------------------------------|
|- Attempts to load the spellset listed in e3_Settings.ini.	-|
|------------------------------------------------------------|
#event LoadSpells "<#*#> Load Spells"
SUB event_LoadSpells
/call check_Gems	
/RETURN



|----------------------------------------|
|- Drops invis on connected NetBots.	-|
|----------------------------------------|
#event drop_Invis "<#*#> Drop Invis"
SUB event_drop_Invis
/attack on
/delay 3s ${Me.Combat}
/attack off
/squelch /target clear
/RETURN



|----------------------------------------------------|
|- Preforms item swaps, listed in Character_Inis.	-|
#event Swap_Items "<#*#> Swap #1#"
#event Swap_Items "[MQ2] Swap #1#"
SUB event_Swap_Items(line, IniEntry)
/if (${Debug}) /echo |- Event Swap_Items ==>
/if (${Ini[${Character_Ini},SwapItems,${IniEntry}].Length}) {
	/declare ItemToSwap string local ${Ini[${Character_Ini},SwapItems,${IniEntry}]}
	/if (${Debug}) /echo |- Swap_Items -| ItemsToSwap: [${ItemToSwap}]
	/declare i int local 1
	/declare CurrentArgument string local
	:SwapLoop
	/if (${Debug}) /echo |- Event Swap_Items -| :SwapLoop
	/varset CurrentArgument ${ItemToSwap.Arg[${i},,]}
	/if (!${Select[${FindItem[${CurrentArgument.Arg[1,/]}].ID},NULL]}) {
		/if (${CurrentArgument.Arg[2,/].Equal[Mainhand]} && ${FindItem[${CurrentArgument.Arg[1,/]}].Type.Left[2].Equal[2H]} && !${Select[${InvSlot[Offhand].Item},NULL]}) {
			/echo Unequipping [${InvSlot[Offhand].Item}]
			/unequip Offhand
		}
		/if (!${Select[${InvSlot[${CurrentArgument.Arg[2,/]}].Item},NULL]}) {
			/echo Swapping [${InvSlot[${CurrentArgument.Arg[2,/]}].Item}] for [${CurrentArgument.Arg[1,/]}]
		} else /if (${Select[${InvSlot[${CurrentArgument.Arg[2,/]}].Item},NULL]}) {
			/echo Equipping [${CurrentArgument.Arg[1,/]}]
		}
		/if (!${Select[${FindItem[${CurrentArgument.Arg[1,/]}].ID},NULL]}) {
			/if (${Cursor.ID}) /call ClearCursor
			/exchange "${CurrentArgument.Arg[1,/]}" ${CurrentArgument.Arg[2,/]}
			/delay 1s ${InvSlot[${slotName}].Item.Name.Equal[${itemName}]}
			/if (${Cursor.ID}) /call ClearCursor
		}
	} else /if (${Select[${FindItem[${CurrentArgument.Arg[1,/]}].ID},NULL]}) {
		/echo |- Event Swap_Items -| Error: Item: [${CurrentArgument.Arg[1,/]}] was not found. 
	}
	/varcalc i ${i} + 1
	/if (${ItemToSwap.Arg[${i},,].Length}) {
		/if (${Debug}) /echo |- event_Swap_Items -| Swaploop.
		/goto :SwapLoop
	}
} else /if (!${Ini[${Character_Ini},SwapItems,${IniEntry}].Length}) {
	/echo |- Event Swap_Items -| Error: Ini entry: [${IniEntry}] was not found. 
}
/if (${Debug}) /echo |- Swap_Items -| <==
/RETURN



|----------------------------------------|
|- Automatically accepts raid invites.	-|
|----------------------------------------|
#event RaidAccept "#*#invites you to join a raid.#*#"
SUB event_RaidAccept
	/raidaccept
/RETURN



|------------------------------------------------------------|
|- Invites ChatSender to raid.								-|
|------------------------------------------------------------|
|- Use, /tell 'RaidLeader' Raid invite, or, /gu Raid Invite	-|
|------------------------------------------------------------|
#event RaidInvite "#1# tells you, 'raid invite'"
#event RaidInvite "#1# tells the guild, 'raid invite'"
SUB event_RaidInvite(line, InviteTarget)
/if (${InviteTarget.Left[1].Compare[ ]}<0) /varset InviteTarget ${InviteTarget.Right[-2].Left[-1]}
/if (${SpawnCount[${InviteTarget}]}) {
	/if (${Raid.Leader.Name.Equal[${Me.CleanName}]} || ${Me.Name.Equal[Killians]}) {
		/raidinvite ${InviteTarget}
	}
}
/RETURN



|--------------------------------------------|
|- Forces a raid request in guild.			-|
|--------------------------------------------|
|- Use, /tell 'Name' Request raid invite	-|
|--------------------------------------------|
#event Request_RaidInvite "#*# tells you, 'Request raid invite'"
SUB event_Request_RaidInvite
/gu Raid invite
/RETURN



|----------------------------------------------------------------------------------------|
|- Forces all bots to disband from their raid.											-|
|----------------------------------------------------------------------------------------|
|- Use, /bc RaidDisband, /bc disband from raid, or /tell 'Name' [Force raid disband].	-|
|----------------------------------------------------------------------------------------|
#event Force_RaidDisband "<#*#> RaidDisband"
#event Force_RaidDisband "<#*#> Disband from raid"
#event Force_RaidDisband "#*# tells you, '[Force Raid Disband]'"
SUB event_Force_RaidDisband
/bcaa //raiddisband
/RETURN



|----------------------------------------------------|
|- Ends the macro before a bot finishes camping.	-|
|----------------------------------------------------|
#event EndMacro "It will take about 5 more seconds to prepare your camp."
SUB event_EndMacro
/echo User is camping, ending macro.
/endmacro
/RETURN



|----------------------------------------|
|- Arms MGB on all bots who are able.	-|
|----------------------------------------|
|- Use, /bc MGB, or /tell 'Name' [MGB]	-|
|----------------------------------------|
#event MGB "<#1#> MGB"
#event MGB "#1# tells you, '[MGB]'"
#event MGB "<#1#> #2# MGB"
#event MGB "#1# tells you, '#2# [MGB]'"
SUB event_MGB(Line, ChatSender, SpecifiedBots)
/if (${Debug}) /echo |- EVENT_MGB ==>

|-----------------------------------Validate ChatSender, and SpecifiedBots.
	/if (${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}
	/if (${SpawnCount[pc ${ChatSender}]} && ${SpawnCount[pc ${ChatSender} radius ${FollowBreakDistance}]}) {
	|--------------------------------------------------------Check SpecifiedBots
		/if (${Select[${Me.CleanName},${SpecifiedBots}]} || ${Select[${Me.Class.ShortName},${SpecifiedBots}]} || ${Select[${Me.Class},${SpecifiedBots}]} || !${Defined[SpecifiedBots]} || ${SpecifiedBots.Equal[all]} || ${SpecifiedBots.Equal[melee]} && ${Select[${Me.Class.ShortName},${MeleeClasses}]} || ${SpecifiedBots.Equal[casters]} && ${Select[${Me.Class.ShortName},${CasterClasses}]} || ${SpecifiedBots.Equal[ranged]} && ${Select[${Me.Class.ShortName},${RangedClasses}]} || ${SpecifiedBots.Equal[Healers]} && ${Select[${Me.Class.ShortName},${PriestClasses}]} || ${SpecifiedBots.Equal[Priests]} && ${Select[${Me.Class.ShortName},${PriestClasses}]}) {

			/if (!${Me.AltAbilityReady[35]}) {
				/docommand ${ChatToggle} Mass Group Buff is not ready.
			} else {
				
				/if (${Select[${Me.Class},Bard]} && ${MelodyStatus} || ${Twist.Twisting}) {
					/squelch /twist stop
					/declare overrideTwist bool outer TRUE
				}
				
				/call e3_Cast ${Me.ID} 35 alt
				
				/if (${overrideTwist}) /timed 50 /squelch /twist start
			}
		}
	}
	
/if (${Debug}) /echo <== EVENT_GateToLobby -|
/RETURN



|------------------------------------------------------------|
|- Uses 'Lessons of the Devoted', on all bots who are able.	-|
|------------------------------------------------------------|
#event Lessons "<#*#> Lessons"
#event Lessons "#*# tells the group, 'Lessons'"
#event Lessons "#*# tells you, 'Lessons'"
#event Lessons "#*# you tell your party, 'Lessons'"
SUB event_Lessons
/if (${Debug}) /echo |- EVENT_Lessons ==>

	/if (!${Me.AltAbilityReady[Lesson of the Devoted]}) {
		/docommand ${ChatToggle} Lessons of the Devoted is not ready.
	} else {
		
		/if (${Select[${Me.Class},Bard]} && ${MelodyStatus} || ${Twist.Twisting}) {
			/squelch /twist stop
			/declare overrideTwist bool outer TRUE
		}
		
		/call e3_Cast ${Me.ID} "Lesson of the Devoted" alt
		
		/if (${overrideTwist}) /timed 50 /squelch /twist start
	}

/if (${Debug}) /echo <== EVENT_Lessons -|
/RETURN



|----------------------------------------------------------------|
|- Uses 'Infusion of the Faithful', on all bots who are able.	-|
|----------------------------------------------------------------|
#event Infusion "<#*#> Infusions"
#event Infusion "#*# tells the group, 'Infusions'"
#event Infusion "#*# tells you, 'Infusions'"
#event Infusion "#*# you tell your party, 'Infusions'"
SUB event_Infusion
/if (${Debug}) /echo |- EVENT_Infusion ==>

	/if (!${Me.AltAbilityReady[Infusion of the Faithful]}) {
		/docommand ${ChatToggle} Infusion of the Faithful is not ready.
	} else {
		
		/if (${Select[${Me.Class},Bard]} && ${MelodyStatus} || ${Twist.Twisting}) {
			/squelch /twist stop
			/declare overrideTwist bool outer TRUE
		}
		
		/call e3_Cast ${Me.ID} "Infusion of the Faithful" alt
		
		/if (${overrideTwist}) /timed 50 /squelch /twist start
	}

/if (${Debug}) /echo <== EVENT_Infusion -|
/RETURN



|--------------------------------------------------------|
|- Uses 'Staunch Recovery', on all bots who are able.	-|
|--------------------------------------------------------|
#event Staunch "<#*#> Recover"
#event Staunch "#*# tells the group, 'Recover'"
#event Staunch "#*# tells you, 'Recover'"
#event Staunch "#*# you tell your party, 'Recover'"
SUB event_Staunch(line, SpecifiedBots)
/if (${Debug}) /echo |- EVENT_Staunch ==>

|--------------------------------------------------------Check SpecifiedBots
	/if (${Select[${Me.CleanName},${SpecifiedBots}]} || ${Select[${Me.Class.ShortName},${SpecifiedBots}]} || ${Select[${Me.Class},${SpecifiedBots}]} || !${Defined[SpecifiedBots]} || ${SpecifiedBots.Equal[all]} || ${SpecifiedBots.Equal[melee]} && ${Select[${Me.Class.ShortName},${MeleeClasses}]} || ${SpecifiedBots.Equal[casters]} && ${Select[${Me.Class.ShortName},${CasterClasses}]} || ${SpecifiedBots.Equal[ranged]} && ${Select[${Me.Class.ShortName},${RangedClasses}]} || ${SpecifiedBots.Equal[Healers]} && ${Select[${Me.Class.ShortName},${PriestClasses}]} || ${SpecifiedBots.Equal[Priests]} && ${Select[${Me.Class.ShortName},${PriestClasses}]}) {
	
		/if (!${Me.AltAbilityReady[Staunch Recovery]}) {
			/docommand ${ChatToggle} Staunch Recovery is not ready.
		} else {
			
			/if (${Select[${Me.Class},Bard]} && ${MelodyStatus} || ${Twist.Twisting}) {
				/squelch /twist stop
				/declare overrideTwist bool outer TRUE
			}
			
			/call e3_Cast ${Me.ID} "Staunch Recovery" alt
			
			/if (${overrideTwist}) /timed 50 /squelch /twist start
		}
	}
	
/if (${Debug}) /echo <== EVENT_Staunch -|
/RETURN



|--------------------------------------------------------|
|- Uses 'Steadfast Servant', on all bots who are able.	-|
|--------------------------------------------------------|
#event Servant "<#*#> Servants"
#event Servant  "#*# tells the group, 'Servants'"
#event Servant  "#*# tells you, 'Servants'"
#event Servant  "#*# you tell your party, 'Servants'""
SUB event_Servant
/if (${Debug}) /echo |- EVENT_Servant ==>

	/if (!${Me.AltAbilityReady[Steadfast Servant]}) {
		/docommand ${ChatToggle} Steadfast Servant is not ready.
	} else {
		
		/if (${Select[${Me.Class},Bard]} && ${MelodyStatus} || ${Twist.Twisting}) {
			/squelch /twist stop
			/declare overrideTwist bool outer TRUE
		}
		
		/call e3_Cast ${Me.ID} "Steadfast Servant" alt
		
		/if (${overrideTwist}) /timed 50 /squelch /twist start
	}

/if (${Debug}) /echo <== EVENT_Servant -|
/RETURN



|----------------------------------------------------------------|
|- Uses 'Intensity of the Resolute', on all bots who are able.	-|
|----------------------------------------------------------------|
#event Intensity "<#*#> Intensify"
#event Intensity "<#*#> Use Intensity of the Resolute"
#event Intensity  "#*# tells the group, 'Intensify'"
#event Intensity  "#*# tells you, 'Intensify'"
#event Intensity  "#*# you tell your party, 'Intensify'""
SUB event_Intensity
/if (${Debug}) /echo |- EVENT_Intensity ==>

	/if (!${Me.AltAbilityReady[Intensity of the Resolute]}) {
		/docommand ${ChatToggle} Intensity of the Resolute is not ready.
	} else {
		
		/if (${Select[${Me.Class},Bard]} && ${MelodyStatus} || ${Twist.Twisting}) {
			/squelch /twist stop
			/declare overrideTwist bool outer TRUE
		}
		
		/call e3_Cast ${Me.ID} "Intensity of the Resolute" alt
		
		/if (${overrideTwist}) /timed 50 /squelch /twist start
	}

/if (${Debug}) /echo <== EVENT_Intensity -|
/RETURN



|--------------------------------------------------------|
|- Uses 'Expedient Recovery', on all bots who are able.	-|
|--------------------------------------------------------|
#event RecoverCorpses "<#*#> Recover Corpses"
#event RecoverCorpses "<#*#> Use expedient recovery"
#event RecoverCorpses  "#*# tells the group, 'Recover Corpses'"
#event RecoverCorpses  "#*# tells you, 'Recover Corpses'"
#event RecoverCorpses  "#*# you tell your party, 'Recover Corpses'""
SUB event_RecoverCorpses
/if (${Debug}) /echo |- EVENT_RecoverCorpses ==>

	/if (!${Me.AltAbilityReady[Expedient Reovery]}) {
		/docommand ${ChatToggle} Expedient is not ready.
	} else {
		
		/if (${Select[${Me.Class},Bard]} && ${MelodyStatus} || ${Twist.Twisting}) {
			/squelch /twist stop
			/declare overrideTwist bool outer TRUE
		}
		
		/call e3_Cast ${Me.ID} "Expedient Reovery" alt
		
		/if (${overrideTwist}) /timed 50 /squelch /twist start
	}
	
/if (${Debug}) /echo <== EVENT_RecoverCorpses -|
/RETURN



|--------------------------------------------------------|
|- Uses 'Throne of Heroes', on all bots who are able.	-|
|--------------------------------------------------------|
#event GateToLobby "<#*#> Gate to lobby"
#event GateToLobby "<#*#> Use Throne of Heroes"
#event GateToLobby  "#*# tells the group, 'Gate to lobby'"
#event GateToLobby  "#*# tells you, 'Gate to lobby'"
#event GateToLobby  "#*# you tell your party, 'Gate to lobby'""

SUB event_GateToLobby
/if (${Debug}) /echo |- EVENT_GateToLobby ==>

	/if (!${Me.AltAbilityReady[Throne of Heros]}) {
		/docommand ${ChatToggle} Throne of Heroes is not ready.
	} else {
		
		/if (${Select[${Me.Class},Bard]} && ${MelodyStatus} || ${Twist.Twisting}) {
			/squelch /twist stop
			/declare overrideTwist bool outer TRUE
		}
		
		/call e3_Cast ${Me.ID} "Throne of Heros" alt
		
		/if (${overrideTwist}) /timed 200 /squelch /twist start
	}

/if (${Debug}) /echo <== EVENT_GateToLobby -|
/RETURN



|------------------------------------------------------------|
|- Group leaders relay port commands from /bc to /group.	-|
|------------------------------------------------------------|
|- Use, /bc port to 'destination'							-|
|-		ex.  /bc Port to pok								-|
|------------------------------------------------------------|
#event GroupPortCommand "<#*#> Port to #1#"
SUB event_GroupPortCommand(line, Destination)
/if (${Following}) {
	/squelch /netfollow off
	/squelch /stick off
}
/if (${Select[${Group.Leader},${Me}]}) /g gport${Destination}
/RETURN



|------------------------------------------------|
|- Engages MedBreak mode, on bots who can cast.	-|
|------------------------------------------------|
#event medBreak_on "<#1#> Start MedBreak#*#"
#event medBreak_on "<#1#> Med On#*#"
#event medBreak_on "<#1#> MedBreak On#*#"
#event medBreak_on "<#1#> Start Meditating#*#"
#event medBreak_on "<#1#> Meditate on#*#"
SUB event_medBreak_on(line, ChatSender)
/if (${Me.Class.CanCast} && ${Me.Class.ShortName.NotEqual[BRD]}) {
	/if (${SpawnCount[pc ${ChatSender}]}) {
		|----------------------------------------------------------------------------------------------------------------------Clear old variables
		/if (${Defined[medBreak]}) /deletevar medBreak
		/if (${Defined[medBreak_Caller]}) /deletevar medBreak_Caller
		/if (${Defined[medBreak_Hold]}) /deletevar medBreak_Hold
		|---------------------------------------------------------------------------------------------------------------------Create new variables
		/declare medBreak bool outer TRUE
		/declare medBreak_Caller string outer ${ChatSender}
		/if (${line.Right[4].Equal[Hold]}) /declare medBreak_Hold bool outer TRUE
		
		/if (${Spawn[pc ${ChatSender}].Distance} <= ${FollowBreakDistance}) {
			/docommand ${ChatToggle} Meditating...
		} else /if (${Spawn[pc ${ChatSender}].Distance} > ${FollowBreakDistance}) {
			/docommand ${ChatToggle} I am too far away from you to meditate, right now.  I'll start meditating when you're in range.
		}
	}
}
/RETURN



|----------------------------------------------------|
|- disengages MedBreak mode, on bots who can cast.	-|
|----------------------------------------------------|
#event medBreak_off "<#*#> End MedBreak"
#event medBreak_off "<#*#> Stop MedBreak"
#event medBreak_off "<#*#> End Med"
#event medBreak_off "<#*#> Stop Med"
#event medBreak_off "<#*#> Med off"
#event medBreak_off "<#*#> MedBreak off"
#event medBreak_off "<#*#> Meditate off"
#event medBreak_off "<#*#> Stop meditating"
SUB event_medBreak_off
/if (${medBreak}) {
	/if (${Verbosity}) /docommand ${ChatToggle} I am no longer meditating.
	/if (${Me.Sitting}) /stand
	/if (${Defined[medBreak]}) /deletevar medBreak
	/if (${Defined[medBreak_Caller]}) /deletevar medBreak_Caller
	/if (${Defined[medBreak_Hold]}) /deletevar medBreak_Hold
}
/RETURN



|--------------------------------------------------------------------------------|
|- MedBreak hook for Tertiary loop.												-|
|--------------------------------------------------------------------------------|
|- Automatically disengages and re-engages MedBreak when conditions are met.	-|
|- Ends MedBreak mode at 99% mana, unless, 'MedBreak hold command is given.		-|
|--------------------------------------------------------------------------------|
SUB check_MedBreak
|--------------------------------------------------------------------------------------------------------------------check Follow / Leash Distance
	/if (${Following} && ${SpawnCount[pc ${FollowTarget}]} && ${Spawn[pc ${FollowTarget}].Distance} > ${LeashLength}  && !${Me.Moving}) /call check_Follow

	/if (!${Me.Moving}) {
		/if (${Stick.Active}) /squelch /stick off
		/if (${NetAdvPath.State}) /squelch /netfollow off
	}

|-----------------------------------------------------------------------------------------------------------------------------check Bots in combat
/declare i int local
/for i 1 to ${NetBots.Counts}
/if (${NetBots[${NetBots.Client[${i}]}].Attacking} || ${Me.Combat} || ${Assisting} || ${AdvAssisting}) {
	/varset combatTimer 15s
	/docommand ${ChatToggle} [${NetBots.Client[${i}]}] is in combat, ending med break.
	/if (${Me.Sitting}) /stand
	/if (${Defined[medBreak]}) /deletevar medBreak
	/if (${Defined[medBreak_Caller]}) /deletevar medBreak_Caller
	/if (${Defined[medBreak_Hold]}) /deletevar medBreak_Hold
	/RETURN
}
/next i
|------------------------------------------------------------------------------------------------------------------------do ***Meditating*** popup
/if (!${spamTimer}) {
	/popup ***Meditating***
	/varset spamTimer 3s
}
|------------------------------------------------------------------------------------------------------------------sit down, if we are not sitting
/if (!${Me.Sitting} && !${Me.Casting.ID}) /sit
|---------------------------------------------------------------------------------------------------------------end MedBreak when we are full mana
/if (${Me.PctMana} >= 99 && !${medBreak_Hold}) {
	/docommand ${ChatToggle} I am at full mana, ending MedBreak.
	/if (${Me.Sitting}) /stand
	/if (${Defined[medBreak]}) /deletevar medBreak
	/if (${Defined[medBreak_Caller]}) /deletevar medBreak_Caller
	/if (${Defined[medBreak_Hold]}) /deletevar medBreak_Hold
}
/RETURN


