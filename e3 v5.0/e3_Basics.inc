|------------------------------------------------------------------------|
|- e3_Macro_Basics.inc v4.0	( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Contains basic and miscellaneous EVENTs and functions of e3.mac.		-|
|------------------------------------------------------------------------|

| finish medbreak
| check gate and evacuate
| fix findit command

|--------------------------------------------------------------------------------------------------------------------|
|- Engages, and sets variables related to Follow.																	-|
|--------------------------------------------------------------------------------------------------------------------|
|- Only bots in the same zone will engage follow.																	-|
|- Bots will wait to engage follow until, they can see the FollowTarget, and it is within the MaxResponseDist.		-|
|- If bots are assisting, they will back off and follow until re-engage.											-|
|--------------------------------------------------------------------------------------------------------------------|
#EVENT Follow "[#1#] Follow"
#EVENT Follow "<#1#> Follow"
#EVENT Follow "#1# tells you, 'Follow'"
#EVENT Follow "#1# tells the group, 'Follow'"
#EVENT Follow "#1# you tell your party, 'Follow'"

#EVENT Follow "[#1#] Follow #2#"
#EVENT Follow "<#1#> Follow #2#"
#EVENT Follow "#1# tells you, 'Follow #2#'"
#EVENT Follow "#1# tells the group, 'Follow #2#'"
#EVENT Follow "#1# you tell your party, 'Follow #2#'"
SUB EVENT_Follow(line, ChatSender, eventParams)
/if (${Debug}) /echo |- EVENT_Follow ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]}) {
	
		/declare SpawnToFollow string local ${If[${eventParams.Arg[1, ].Find[-]} || ${eventParams.Arg[1, ].Equal[me]} || ${eventParams.Arg[1, ].Equal[on]},${ChatSender},${eventParams.Arg[1, ]}]}

		| -Turn Following on and set the FollowTarget
		/varset Following TRUE
		/varset FollowTarget ${Spawn[pc ${SpawnToFollow}].CleanName}
		
		| -If I am the follow target
		| =>
		/if (${FollowTarget.Equal[${Me}]}) {

			| -Turn off follow plugins.
			/if (${Stick.Active}) /squelch /stick off
			/if (${NetAdvPath.State}) /squelch /netfollow off

			| -create a list of bots following you.
			/varset Followers 
			/declare i int local
			/for i 1 to ${NetBots.Counts}
				/if (${NetBots.Client.Arg[${i}, ].NotEqual[${Me.CleanName}]} && ${SpawnCount[pc ${NetBots.Client.Arg[${i}, ]}]} && ${Spawn[pc ${NetBots.Client.Arg[${i}, ]}].Distance}<${MaxResponseDist}) /varset Followers ${Followers}${NetBots.Client.Arg[${i}, ]},
			/next i
		| <=
		} else {
		
		| -If I am not the follow target
		| =>
		
			| -Check Range
			/if (${Spawn[${FollowTarget}].Distance} > ${MaxResponseDist}) {
				/docommand ${ChatToggle} [${FollowTarget}] is too far away.
			} else {
			
				| -Check LoS.
				/if (!${Spawn[${FollowTarget}].LineOfSight}) {
					/docommand ${ChatToggle} I cannot see [${FollowTarget}].
				} else {
				
					| -Stop assisting so we don't try to stick to the AssistTarget.	
					/if (${Assisting} || ${Me.Combat}) /call Event_BackOff
					
					| -Start Following.
					/if (${Verbosity}) /docommand ${ChatToggle} Following ${FollowTarget}.
					/call AquireFollow
				}
			}
		| <=
		}
	}
	
/if (${Debug}) /echo <== EVENT_Follow -|
/RETURN



|----------------------------------------------------------------------------------------------------|
|- Engages follow plugins on specified follow targets.												-|
|----------------------------------------------------------------------------------------------------|
|- Uses NetAdvPath if the follow target is one of your bots, or MoveUtils, if its someone elses.	-|		
|----------------------------------------------------------------------------------------------------|
SUB AquireFollow
/if (${Debug}) /echo |- AquireFollow ==>

	|------------------------------------------------------------------------------------Check for FollowTarget in zone.
	/if (${SpawnCount[${FollowTarget}]}) {
		
		|--------------------------------------------------------------------------------Check range to the FollowTarget
		/if (${SpawnCount[${FollowTarget} radius ${MaxResponseDist}]}) {

			|----------------------------------------------------------------------------Check that I'm not the follow target.
			/if (${FollowTarget.NotEqual[${Me.CleanName}]}) {
		
				|------------------------------------------------------------------------Turn off any previous follow plugins.
				/if (${Stick.Active}) /squelch /stick off
				/if (${NetAdvPath.State}==1) /squelch /netfollow off
				
				/declare followTargetID int local
				
				|--------------------------------------------------------------------Check Target ID
				/if (${SpawnCount[pc ${FollowTarget}]}) {
					/varset followTargetID ${Spawn[pc ${FollowTarget}].ID}
				} else /if (${SpawnCount[npc ${FollowTarget}]}) {
					/varset followTargetID ${Spawn[npc ${FollowTarget}].ID}
				} else /if (${SpawnCount[pet ${FollowTarget}]}) {
					/varset followTargetID ${Spawn[pet ${FollowTarget}].ID}
				}

				| If the FollowTarget is a NetBot, and you're using NetAdvPath
				/if (${NetBots[${FollowTarget}].ID} && ${NetAdvPath}) {
				
				| If the FollowTarget is a NetBot, engage MQ2NetAdvPath.
					/if (${Spawn[id ${followTargetID}].LineOfSight}) {
						/call TrueTarget ${followTargetID}
						/squelch /netfollow on
					} else /if (${NetAdvPath.State}==2) {
						/squelch /netfollow unpause
					}
				} else {
					
					|---------------------------------------------------If MQ2NetAdvPath was not detected, or the FollowTarget was not a NetBot, engage MQ2MoveUtils.
				
					|---------------------------------------------------Check LoS
					/if (${Spawn[id ${followTargetID}].LineOfSight}) {
						/call TrueTarget ${followTargetID}
						/squelch /stick hold 20 uw
					}
				}

				|-------------------------------------------------------Clear my target
				/if (${Target.ID} == ${followTargetID}) /squelch /target clear
			}
		}
	}

/if (${Debug}) /echo <== AquireFollow -|
|** 
	ADD A LOOP TO CALCULATE DISTANCE TO YOUR TARGET. ONCE DISTANCE STOPS DECREASING REISSUE FOLLOW COMMAND, TO SNAP TO TARGET
	
	/declare HeadingToSpawn string local ${Spawn[pc ${FollowTarget}].HeadingTo}
	:movingLoop
	/if (${Me.Moving}) {
		/if () {
		
		}
	}
**|
/RETURN



|----------------------------------------------------------------------------|
|- Disengages follow on bots who are in the same zone as the ChatSender.	-|
|----------------------------------------------------------------------------|
|- Use, /stop, or, 'stop' in /tell, /goup, or /bc.							-|
|----------------------------------------------------------------------------|
#EVENT Stop	"[#1#] Stop"
#EVENT Stop	"<#1#> Stop"
#EVENT Stop "#1# tells you, 'Stop'"
#EVENT Stop "#1# tells the group, 'Stop'"
#EVENT Stop "#1# you tell your party, 'Stop'"

#EVENT Stop	"<#1#> Stop #2#"
#EVENT Stop "#1# tells you, 'Stop #2#'"
#EVENT Stop "#1# tells the group, 'Stop #2#'"
#EVENT Stop "#1# you tell your party, 'Stop #2#'"
SUB EVENT_Stop(line, ChatSender, eventParams)
/if (${Debug}) /echo |- EVENT_Stop ==>

	| -Check if I'm following.
	/if (${Following}) {
		
		| -Verify the event
		/call verifyEvent "${ChatSender}" "${line}"
		/varset ChatSender ${Macro.Return.Arg[1,_]}
		|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|
		/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]}) {
	
			| -Wait to get close to the followTarget.
			/if (!${Spawn[${FollowTarget}].Moving} && ${Spawn[${FollowTarget}].Distance} > ${LeashLength}) /delay 100 ${Spawn[${FollowTarget}].Distance} < ${LeashLength} || ${Spawn[${FollowTarget}].Moving}
		
			| -Stop following
			/if (${Stick.Active}) /squelch /stick off
			/if (${NetAdvPath.State}==1) /squelch /netfollow off
			/varset FollowTarget NULL
			/varset Following FALSE
		}
	}

/if (${Debug}) /echo <== EVENT_Stop -|
/RETURN STOP



#EVENT MoveHere "<#1#> Move to #2#"
#EVENT MoveHere "#1# tells you, 'Move to #2#'"
#EVENT MoveHere "#1# tells the group, 'Move to #2#'"
#EVENT MoveHere "#1# you tell your party, 'Move to #2#'"
SUB EVENT_MoveHere(line, ChatSender, eventParams)
/if (${Debug}) /echo |- EVENT_MoveHere ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
		
		| -Check if the ChatSender is me.
		/if (${ChatSender.Equal[${Me}]}) {
			/if (${Debug}) /echo |- EVENT_MoveHere -| I am the ChatSender, skipping.
		} else {
		
			| -Set WhereTo.
			/declare WhereTo string local ${If[${eventParams.Arg[1, ].Find[-]} || ${eventParams.Arg[1, ].Equal[me]} || ${eventParams.Arg[1, ].Equal[here]},${ChatSender},${eventParams.Arg[1, ]}]}
			
			| -Change WhereTo to a spawnID
			/if (!${Int[${WhereTo}]}) {
				/if (${SpawnCount[pc ${WhereTo}]}) {
					/varset WhereTo ${Spawn[pc ${WhereTo}].ID}
				} else /if (${SpawnCount[pet ${WhereTo}]}) {
					/varset WhereTo ${Spawn[pet ${WhereTo}].ID}
				} else /if (${SpawnCount[npc ${WhereTo}]}) {
					/varset WhereTo ${Spawn[npc ${WhereTo}].ID}
				}
			}
			
			| -Stop assisting so I don't try to stick to the AssistTarget.	
			/if (${Assisting} || ${Me.Combat}) /call Event_BackOff
			
			| -Wait to stop casting
			/if (${Me.Casting.ID}) /delay 300 !${Me.Casting.ID}
			
			/declare retryTimer timer local 150
			
			:retry_MoveTo
			/call MoveTo -SpawnID|${WhereTo}
			
			| -Check if I have moved to the Target.
			/if (${Spawn[id ${WhereTo}].Distance} > 10) {
				/if  (${retryTimer}) {
					/goto :retry_MoveTo
				} else {
					/echo |- EVENT_MoveHere -| Failed to move to the specified target.
				}
			} else {
				/if (${Debug}) /echo |- EVENT_MoveHere -| I have arrived at the specified target.
			}
			
			| -If I'm following, reaquire follow.
			/if (${Following}) {
				/if (${WhereTo.Equal[${Spawn[id ${FollowTarget}].CleanName}]}) {
					/call AquireFollow
				} else {
					/call EVENT_Stop "<${Me.CleanName}> Stop" ${Me.CleanName}
				}
			}
		}
	}

/if (${Debug}) /echo <== EVENT_MoveHere -|
/RETURN



|--------------------------------------------------------------------------------------------------------|
|- Commands bots to bark a message at a target.															-|
|- Targets can be designated by name, or ID, using -ID| or -Name|
|--------------------------------------------------------------------------------------------------------|
|- @param line: '<Name> Bark at MobName BarkMsg'														-|
|- @param msgTarget: The NPC you wish to bark at.														-|
|- @param barkIt: What to bark at the NPC.																-|
|- @param ChatSender: The command sender that requested bark.											-|
|- @param selectedBots: Bots specified to bark at npc.													-|
|--------------------------------------------------------------------------------------------------------|
|- Targets must be within the MaxResponseDist (default 250units).										-|
|- Use, '/bark MESSAGE', or, the command 'Bark at NAME MESSAGE' can be given in /bc, /g, or /tell.		-|
|--------------------------------------------------------------------------------------------------------|
#EVENT bark "<#1#> Bark at #2# #3#"
#EVENT bark "#1# tells you, 'Bark at #2# #3#'"
#EVENT bark "#1# tells the group, 'Bark at #2# #3#'"
#EVENT bark "#1# you tell your party, 'Bark at #2# #3#'"
SUB EVENT_bark(line, ChatSender, msgTarget, barkMsg)
/if (${Debug}) /echo |- EVENT_Bark ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |---------Included Bot----------|    |-------------In Zone-----------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
		
		| -If msgTarget is not an ID, find TargetID.
		/if (!${Int[${msgTarget}]}) {
			/if (${SpawnCount[npc ${msgTarget}]}) {
				/varset msgTarget ${Spawn[npc ${msgTarget}].ID}
			} else /if (${SpawnCount[pc ${msgTarget}]}) {
				/varset msgTarget ${Spawn[npc ${msgTarget}].ID}
			} else /if (${SpawnCount[pet ${msgTarget}]}) {
				/varset msgTarget ${Spawn[npc ${msgTarget}].ID}
			} else {
				/echo Could not find a SpawnID for [${msgTarget}].
				/varset msgTarget NULL
			}
		}
		
		/declare barkTarget string outer ${Spawn[id ${msgTarget}].CleanName}
		/declare barkReturned bool outer FALSE
		/declare retryTimer timer local 150
		/declare startingLoc string local
		
		/if (${Stick.Active}) /squelch /stick off
		/if (${NetAdvPath.State}==1) /squelch /netfollow off
		/if (${Me.Casting.ID}) /delay 300 !${Me.Casting.ID}
		
		:retry_Bark
		/if (${Debug} && !${spamTimer_retryBark}) {
			/echo |- EVENT_bark -| :retry_Bark loop
			/call createTimer spamTimer_retryBark 20
		}
		
		| -Target the msgTarget
		/call TrueTarget ${msgTarget}
		/if (${Target.Distance} > 10) {
			/call MoveTo -Loc|${Target.Y},${Target.X}
			/if (${retryTimer}) {
				/goto :retry_Bark
			} else {
				/echo |- EVENT_Bark -| Failed to move to the intended target.
			}
		} else {
		
			| -Wait a moment, then bark the message.
			/delay ${Math.Rand[30]}
			
			/varset startingLoc ${Me.Loc.Replace[ ,]}
			
			| -Reset barkMsg
			/if (${barkMsg.Find[-]} && ${barkMsg.Find[|]}) {
				/varset barkMsg ${barkMsg.Arg[1,-]}
				/varset barkMsg ${barkMsg.Left[${Math.Calc[${barkMsg.Length} - 1].Int}]}
			}
			
			/say ${barkMsg}
			
			| -Wait up to 3 seconds for something to happen.
			/delay 30 ${Zone.ID} != ${currentZone} || ${Me.Loc.Replace[ ,].NotEqual[${startingLoc}]} || ${Window[ConfirmationDialogBox].Open} || ${Window[LargeDialogWindow].Open}

			| -Click Yes to a confirmation box.
			/if (${Window[ConfirmationDialogBox].Open}) {
				/notify ConfirmationDialogBox Yes_Button leftmouseup
				/delay 10
			} else /if (${Window[LargeDialogWindow].Open}) {
				/notify LargeDialogWindow LDW_YesButton leftmouseup
				/delay 10
			}
			
			| -Check to see if the bark was successful.
			/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${startingLoc}
			
			/doevents barkReturn

			/if (${Zone.ID} == ${currentZone} && ${Macro.Return} < 50 && !${barkReturned}) {
				/if (${retryTimer}) {
					/goto :retry_Bark
				} else {
					/docommand ${ChatToggle} Nothing seems to have happened...
				}
			}
		}
		/deletevar barkTarget
		/deletevar barkReturned
	}

/if (${Debug}) /echo <== EVENT_Bark -|
/RETURN



|------------------------------------------------------------|
|- Attempts to detected NPC responses to PC hails in /say.	-|
|------------------------------------------------------------|
|- @param line: '|BarkTarget| says, 'Something''			-|
|- @param returnBark: What the NPC replied.					-|
|------------------------------------------------------------|
#EVENT barkReturn "|${barkTarget}| #*#"
SUB EVENT_barkReturn(line, returnBark)
/if (${Debug}) /echo |- EVENT_barkReturn ==>
	
	/varset barkReturned TRUE
	
/if (${Debug}) /echo <== EVENT_barkReturn -|
/RETURN



|----------------------------------------------------------------------------|
|- Moves to a given location, and clicks the nearest object.				-|
|----------------------------------------------------------------------------|
|- @param ChatSender: The command sender.									-|
|- @param locToStart: Where to stand when attempting to click the object.	-|
|----------------------------------------------------------------------------|
#EVENT clickIt "[#1#] Click object at #2#"
#EVENT clickIt "<#1#> Click object at #2#"
#EVENT clickIt "#1# tells you, 'Click object at #2#'"
#EVENT clickIt "#1# tells the group, 'Click object at #2#'"
#EVENT clickIt "#1# you tell your party, 'Click object at #2#'"
SUB EVENT_clickIt(line, ChatSender, locToStart)
/if (${Debug}) /echo |- EVENT_clickIt ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {

		| -Check LoS.
		/if (!${LineOfSight[${Me.Loc.Replace[ ,]}:${locToStart}]}) {
			/docommand ${ChatToggle} I cannot see the object to click.
		} else {
			
			| -Check range to object.
			/call Triangulate_Distance ${locToStart} ${Me.Loc.Replace[ ,]}
			/if (${Macro.Return} > ${Math.Calc[${MaxResponseDist}*3].Int}) {
				/docommand ${ChatToggle} That object is too far away to click.
			} else {
					
				/declare retryTimer timer local 300
				/declare miscTimer timer local
				/declare startZone int local ${Zone.ID}
				/if (${locToStart.Find[ ]}) /varset locToStart ${locToStart.Arg[1, ]}
				
				/if (${Stick.Active}) /squelch /stick off
				/if (${NetAdvPath.State}==1) /squelch /netfollow off
				/if (${Me.Casting.ID}) /delay 300 !${Me.Casting.ID}
		
				:retryClick
				
				| -Check if I need to move closer.
				/call Triangulate_Distance ${locToStart} ${Me.Loc.Replace[ ,]}
				/if (${Macro.Return} > 10) {
					/if (${retryTimer}) {
						/call MoveTo -Loc|${locToStart}
						/goto :retryClick
					} else {
						/docommand ${ChatToggle} I failed to move to the inteded object
					}
				} else {
				
					| -Switch to first person camera.
					/keypress First_Person_Camera
				
					| -Face object and attempt to click it for up to 5 seconds.
					/varset miscTimer 50
					/squelch /doortarget
					/squelch /face fast door
					/squelch /look -75

					:keypress
					
					| -Shifts look angle up and down while attempting to click.
					/look ${If[${Me.Look} < 75,${Math.Calc[${Me.Look} + 5]},-75]}
					/if (${Target.ID}) /squelch /target clear
					/keypress U
					/delay 2
					
					| -Check if I have zoned, or moved.
					/call Triangulate_Distance ${locToStart} ${Me.Loc.Replace[ ,]}
					/if (${Zone.ID} == ${currentZone} && ${Macro.Return} < 50) {

						| -Check keypressTimer.
						/if (${miscTimer}) {
							/goto :keypress
						} else {
						
							| -Check the retryTimer.
							/if (${retryTimer}) {
								/if (${Debug}) /echo |- EVENT_clickIt -| Click attempt failed, retrying.
								/keypress back hold
								/delay 5
								/keypress forward
								/delay ${Math.Rand[30]}
								
								/if (${retryTimer}) /goto :retryClick
						
							| -If the retryTimer has expired, and I have not clicked the object.
							} else {
								/docommand ${ChatToggle} I have failed to click the object.
							}
						}
					}
				}
			}
		}
	}

/if (${Debug}) /echo <== EVENT_clickIt -|
/RETURN



|----------------------------------------------------------------------------|
|- Points faces bots to your current heading, and runs through a zone line.	-|
|----------------------------------------------------------------------------|
|- @param ChatSender: The command sender.									-|
|- @param locToStart: Where to stand when trying to click the door.			-|
|- @param snapHeading: The direction to face when trying to click the door.	-|
|----------------------------------------------------------------------------|
#EVENT runThruZone "[#1#] Run through zone at #2#"
#EVENT runThruZone "<#1#> Run through zone at #2#"
#EVENT runThruZone "#1# tells you, 'Run through zone at #2#'"
#EVENT runThruZone "#1# tells the group, 'Run through zone at #2#'"
#EVENT runThruZone "#1# you tell your party, 'Run through zone at #2#'"
SUB EVENT_runThruZone(line, ChatSender, eventParams)
/if (${Debug}) /echo |- EVENT_runThruZone ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {

		/declare locToStart string local ${eventParams.Arg[1, ].Arg[1,,]},${eventParams.Arg[1, ].Arg[2,,]}
		/declare startHeading float local ${eventParams.Arg[1, ].Arg[3,,]}
		/declare retryTimer timer local 300
		/declare miscTimer timer local
		/declare startZone int local ${Zone.ID}
	
		| -Check that you can see the location to start from.
		/if (!${LineOfSight[${Me.Loc.Replace[ ,]}:${locToStart}]}) {
			/docommand ${ChatToggle} I cannot see the starting location.
		} else {
		
			| -Check range to object.
			/call Triangulate_Distance ${locToStart} ${Me.Loc.Replace[ ,]}
			/if (${Macro.Return} > ${Math.Calc[${MaxResponseDist}*3].Int}) {
				/docommand ${ChatToggle} I am to far away from that zone line.
			} else {
			
				/if (${Stick.Active}) /squelch /stick off
				/if (${NetAdvPath.State}==1) /squelch /netfollow off
				/if (${Me.Casting.ID}) /delay 300 !${Me.Casting.ID}
			
				:retryRTZ
			
				| -Check if I need to move closer.
				/call Triangulate_Distance ${locToStart} ${Me.Loc.Replace[ ,]}
				/if (${Macro.Return} > 10) {
					/if (${retryTimer}) {
						/call MoveTo -Loc|${locToStart}
						/goto :retryRTZ
					} else {
						/docommand ${ChatToggle} I failed to move to the inteded zone line.
					}
				} else {
				
					| -Face the zone and run foward for 15 seconds.
					/face fast heading ${Math.Calc[${snapHeading}*-1]}
					/look ${snapLook}
					/delay 5
					/keypress forward hold
					
					| -Wait to zone.
					/delay 100 !${Zone.ID} || ${Zone.ID} != ${startZone}
					/if (!${Zone.ID}) /delay 600 ${Zone.ID}
	
					| -Check if I zoned.
					/if (${Zone.ID} == ${startZone}) {
						/if (${retryTimer}) {
							/goto :retryRTZ
						} else {
							/docommand ${ChatToggle} I failed to run through the zoneline in [${Zone}].
							/call MoveTo -Loc|${locToStart}
						}
					}
				}
			}
		}
	}

/if (${Debug}) /echo <== EVENT_runThruZone -|
/RETURN



|------------------------------------------------|
|- All bots click yes to confirmation boxes.	-|
|------------------------------------------------|
|- Use, /clickyes, or, /bc Click yes			-|
|------------------------------------------------|
#EVENT ClickYes	"[#1#] Click Yes"
#EVENT ClickYes	"<#1#> Click Yes"
#EVENT ClickYes	"#1# tells you, 'Click Yes'"
#EVENT ClickYes	"#1# you tell your party, 'Click Yes'"
#EVENT ClickYes	"#1# tells the group, 'Click Yes'"
#EVENT ClickYes	"<#1#> Click Yes |-|#*#"
#EVENT ClickYes	"#1# tells you, 'Click Yes |-|#*#'"
#EVENT ClickYes	"#1# you tell your party, 'Click Yes |-|#*#'"
#EVENT ClickYes	"#1# tells the group, 'Click Yes |-|#*#'"
SUB EVENT_ClickYes(line, ChatSender)
/if (${Debug}) /echo |- EVENT_ClickYes ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {

		| -Click yes.
		/if (${Window[ConfirmationDialogBox].Open}) {
			/notify ConfirmationDialogBox Yes_Button leftmouseup
		} else /if (${Window[LargeDialogWindow].Open}) {
			/notify LargeDialogWindow LDW_YesButton leftmouseup
		}
	}

/if (${Debug}) /echo <== EVENT_ClickYes -|
/RETURN



|----------------------------------------|
|- Drops invis on connected NetBots.	-|
|----------------------------------------|
#EVENT dropInvis "[#1#] Drop Invis"
#EVENT dropInvis "[#1#] Drop Invisibility"
#EVENT dropInvis "<#1#> Drop Invis"
#EVENT dropInvis "<#1#> Drop Invisibility"
#EVENT dropInvis "#1# tells you, 'Drop Invis'"
#EVENT dropInvis "#1# tells you, 'Drop Invisibility'"
#EVENT dropInvis "#1# tells the group, 'Drop Invis'"
#EVENT dropInvis "#1# tells the group, 'Drop Invisibility'"
#EVENT dropInvis "<#1#> Drop Invis |-|#*#"
#EVENT dropInvis "<#1#> Drop Invisibility |-|#*#"
#EVENT dropInvis "#1# tells you, 'Drop Invis |-|#*#'"
#EVENT dropInvis "#1# tells you, 'Drop Invisibility |-|#*#'"
#EVENT dropInvis "#1# tells the group, 'Drop Invis |-|#*#'"
#EVENT dropInvis "#1# tells the group, 'Drop Invisibility |-|#*#'"
SUB EVENT_dropInvis(line, ChatSender)
/if (${Debug}) /echo |- EVENT_dropInvis ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
		/if (${Me.Invis}) {
		
			/if (${Target.ID}) /declare oldTarget int local ${Target.ID}
			
			/call TrueTarget ${Me.ID}
			/attack on
			/delay 30 ${Me.Combat}
			/attack off 
			
			/if (${Defined[oldTarget]}) /call TrueTarget ${oldTarget}
		}
	}

/if (${Debug}) /echo <== EVENT_dropInvis -|
/RETURN



|----------------------------------------------------|
|- Uses /exit on all bots, except the ChatSender.	-|
|----------------------------------------------------|
|- Use, /bc Exit, or, -[Exit]- in any channel.		-|
|----------------------------------------------------|
#EVENT Exit "<#1#> Exit"
#EVENT Exit "#1# tell you, 'Exit'"
#EVENT Exit "#1# tells the group, 'Exit'"
#EVENT Exit "<#1#> Exit |-|#*#"
#EVENT Exit "#1# tell you, 'Exit |-|#*#'"
#EVENT Exit "#1# tells the group, 'Exit |-|#*#'"
SUB EVENT_Exit(line, ChatSender)
/if (${Debug}) /echo |- EVENT_Exit ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {

		/docommand ${ChatToggle} Bye!
		/if (${Spawn[${ChatSender}].ID}!=${Me.ID}) /exit
	}

/if (${Debug}) /echo <== EVENT_Exit -|
/RETURN



|--------------------------------------------------------|
|- Attempt to gate on all bots.							-|
|--------------------------------------------------------|
|- Use, /bc gate, /tell 'Name' [Gate], or, /g [Gate].	-|
|--------------------------------------------------------|
#EVENT gate "[#1#] Gate"
#EVENT gate	"<#1#> Gate"
#EVENT gate "#1# tells you, 'Gate'"
#EVENT gate "#1# you tell your party, 'Gate'"
#EVENT gate "#1# tells the group, 'Gate'"
#EVENT gate	"<#1#> Gate |-|#*#"
#EVENT gate "#1# tells you, 'Gate |-|#*#'"
#EVENT gate "#1# you tell your party, 'Gate |-|#*#'"
#EVENT gate "#1# tells the group, 'Gate |-|#*#'"
SUB EVENT_gate(line, ChatSender)
/if (${Debug}) /echo |- EVENT_Gate ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {

		| -Attempt to gate.
		/if (!${Me.Book[gate]}) {
			/if (${FindItem[Philter of Major Translocation].ID}) {
				/docommand ${ChatToggle} Preparing to gate...
				/call e3_Cast ${Me.ID} "Philter of Major Translocation" item
			} else /if (${FindItem[Vial of Swirling Smoke].ID}) {
				/docommand ${ChatToggle} Preparing to gate...
				/call e3_Cast ${Me.ID} "Vial of Swirling Smoke" item
			} else {
				/docommand ${ChatToggle} I am not able to gate at this time.
			}
		} else {
			/docommand ${ChatToggle} Preparing to gate...
			/call e3_Cast ${Me.ID} "Gate" gem${DefaultGem} ${Math.Calc[${Spell[Gate].RecastTime}+3]}s
		}
	}

/if (${Debug}) /echo <== EVENT_Gate -|
/RETURN



|--------------------------------------------------------|
|- Request evac from bots set to Evac_OnCommand.		-|
|--------------------------------------------------------|
|- Use, /bc Evac, /tell 'Name' [Evac], or, /g [Evac].	-|
|--------------------------------------------------------|
#EVENT Evacuate "[#1#] Evac"
#EVENT Evacuate	"<#1#> Evac"
#EVENT Evacuate "#1# tells you, 'Evac'"
#EVENT Evacuate "#1# you tell your party, 'Evac'"
#EVENT Evacuate "#1# tells the group, 'Evac'"
#EVENT Evacuate "[#1#] Evacuate"
#EVENT Evacuate	"<#1#> Evacuate"
#EVENT Evacuate "#1# tells you, 'Evacuate'"
#EVENT Evacuate "#1# you tell your party, 'Evacuate'"
#EVENT Evacuate "#1# tells the group, 'Evacuate'"

#EVENT Evacuate	"<#1#> Evac |-|#*#"
#EVENT Evacuate "#1# tells you, 'Evac |-|#*#'"
#EVENT Evacuate "#1# you tell your party, 'Evac |-|#*#'"
#EVENT Evacuate "#1# tells the group, 'Evac |-|#*#'"
#EVENT Evacuate	"<#1#> Evacuate |-|#*#"
#EVENT Evacuate "#1# tells you, 'Evacuate |-|#*#'"
#EVENT Evacuate "#1# you tell your party, 'Evacuate |-|#*#'"
#EVENT Evacuate "#1# tells the group, 'Evacuate |-|#*#'"
SUB EVENT_Evacuate(line, ChatSender)
/if (${Select[${Me.Class.ShortName},DRU,WIZ]}) {
/if (${Debug}) /echo |- EVENT_Evacuate ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {

		| -Attempt to evac.
		/if (${Me.AltAbilityReady[Exodus]}) {
			/docommand ${ChatToggle} Evacuating!
			/call e3_Cast ${Me.ID} "Exodus" alt
		} else /if (${Me.Book[${Evac_Spell.Arg[1,/]}]}) {
			/docommand ${ChatToggle} Evacuating!
			/call e3_Cast ${Me.ID} "${Evac_Spell.Arg[1,/]}" gem${DefaultGem} ${Math.Calc[${Spell[${Evac_Spell}].RecastTime}+10]}s
		} else {
			/docommand ${ChatToggle} I am not able to Evac at this time.
		}
	}

/if (${Debug}) /echo <== EVENT_Evacuate -|
}
/RETURN



|------------------------------------------------------------|
|- Attempts to load the spellset listed in e3_Settings.ini.	-|
|------------------------------------------------------------|
#EVENT LoadSpells "[#1#] Load Spells"
#EVENT LoadSpells "<#1#> Load Spells"
#EVENT LoadSpells "#1# tells you, 'Load Spells'"
#EVENT LoadSpells "#1# you tell your party, 'Load Spells'"
#EVENT LoadSpells "#1# tells the group, 'Load Spells'"
#EVENT LoadSpells "<#1#> Load Spells |-|#*#"
#EVENT LoadSpells "#1# tells you, 'Load Spells |-|#*#'"
#EVENT LoadSpells "#1# you tell your party, 'Load Spells |-|#*#'"
#EVENT LoadSpells "#1# tells the group, 'Load Spells |-|#*#'"
SUB EVENT_LoadSpells(line, ChatSender)
/if (${Debug}) /echo |- EVENT_LoadSpells ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {

		| -Check if I can cast, and have a spellset.	
		/if (${Me.Class.CanCast} && ${Defined[Default_SpellSet]}) {
	
			/declare spellsetMem_Timer timer local 300
			/docommand ${ChatToggle} Loading spells...
			/memspellset ${Default_SpellSet}
			:Memloop
			/if (${Debug}) /echo |- EVENT_LoadSpells -| :Memloop
			
			/call Background_Events
			/if (${Debug}) /echo spellsetMem_Timer = [${spellsetMem_Timer}]
			/if (${Window[SpellBookWnd].Open} && ${spellsetMem_Timer}) /goto :Memloop
			/if (${Me.Sitting}) /stand	
		}
	}

/if (${Debug}) /echo <== EVENT_LoadSpells -|
/RETURN


	
|----------------------------------------------------|
|- Attempts to remove specified buffs on NetBots.	-|
|----------------------------------------------------|
|- Use, /bc Remove [Name], or /bc Remove 'Name'.	-|
|----------------------------------------------------|
#EVENT RemoveBuff "[#1#] Remove buff #2#"
#EVENT RemoveBuff "<#1#> Remove buff #2#"
#EVENT RemoveBuff "#1# tells you, 'Remove buff #2#'"
#EVENT RemoveBuff "#1# tells the group, 'Remove buff #2#'"
#EVENT RemoveBuff "#1# you tell your party, 'Remove buff #2#'"
SUB EVENT_RemoveBuff(line, ChatSender, eventParams)
/if (${Debug}) /echo |- EVENT_RemoveBuff ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {

		/declare badBuff string local ${eventParams.Arg[1,-]}
		/declare i int local

		/for i 0 to 29
			
			/if (${Bool[${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip}]} && ${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip.Find[${badBuff}]}) {
				/varset badBuff ${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip.Arg[1,(]}
				/varset badBuff ${badBuff.Left[${Math.Calc[${badBuff.Length}-1]}]}
				/if (${Debug}) /echo |- EVENT_RemoveBuff -| Removing [${badBuff}]
				/nomodkey /notify BuffWindow Buff${Math.Calc[${Me.Buff[${badBuff}].ID}-1].Int} leftmouseup
			}
			
		/next i
	}

/if (${Debug}) /echo <== EVENT_RemoveBuff -|
/RETURN



|------------------------------------------------------------|
|- Checks for specified buffs and debuffs on NetBots.		-|
|------------------------------------------------------------|
|- Use, /bc Check Buff [Name], or /bc Check buff 'Name'.	-|
|------------------------------------------------------------|
#EVENT CheckBuffs "[#1#] Check buff #2#"
#EVENT CheckBuffs "<#1#> Check buff #2#"
#EVENT CheckBuffs "#1# tells you, 'Check buff #2#'"
#EVENT CheckBuffs "#1# you tell your party, 'Check buff #2#'"
#EVENT CheckBuffs "#1# tells the group, 'Check buff #2#'"
SUB EVENT_CheckBuffs(line, ChatSender, eventParams)
/if (${Debug}) /echo |- EVENT_CheckBuffs ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {
	
		/declare badBuff string local ${eventParams.Arg[1,-]}
		/declare i int local

		/for i 0 to 29
			
			/if (${Bool[${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip}]} && ${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip.Find[${badBuff}]}) {
				/varset badBuff ${Window[BuffWindow].Child[BW_Buff${i}_Button].Tooltip.Arg[1,(]}
				/varset badBuff ${badBuff.Left[${Math.Calc[${badBuff.Length}-1]}]}
				/if (${Debug}) /echo |- EVENT_RemoveBuff -| Removing [${badBuff}]
				/docommand ${ChatToggle} I have [${Math.Calc[${Me.Buff[${badBuff}].Duration} * 6 / 60].Int} minutes] left on [${badBuff}].
			}
			
		/next i
	}

/if (${Debug}) /echo <== EVENT_CheckBuffs -|
/RETURN



|--------------------------------------------------------|
|- Searches inventory, and banks, for indicated items.	-|
|--------------------------------------------------------|
|- Use, /bc Find 'Name', or, /echo Find 'Name'.			-|
|--------------------------------------------------------|
#EVENT FindItems "[#1#] Find #2#"
#EVENT FindItems "<#1#> Find #2#"
#EVENT FindItems "<#1#> Find #2# |-|#*#"

#EVENT FindItems "[#1#] Find Item #2#"
#EVENT FindItems "<#1#> Find Item #2#"
#EVENT FindItems "<#1#> Find Item #2# |-|#*#"
SUB EVENT_FindItems(line, ChatSender, eventParams)
/if (${Debug}) /echo |- EVENT_FindItems ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {
	
		| -Declare counting variables.
		/declare i int local
		/declare e int local
		/declare itemName string local
		/declare ItemToFind string local ${eventParams.Arg[1,-]}

		/if (${FindItemCount[${ItemToFind}]} || ${FindItemBankCount[${ItemToFind}]} || ${Cursor.Name.Equal[${ItemToFind}]}) {
			
			/docommand ${ChatToggle} >>> Found (${Math.Calc[${FindItemCount[${ItemToFind}]} + ${FindItemBankCount[${ItemToFind}]}].Int}) examples of [${ItemToFind}] in the following slots ... <<<
			
		
			| -Check cursor.
			/if (${Cursor.Name.Equal[${ItemToFind}]})  /docommand ${ChatToggle} >>> Found [${Cursor.Name}] -> [Cursor]. <<<
			
			| -Check Inventory.
			/if (${FindItemCount[${ItemToFind}]}) {
			
				| -Search Equippped Items.
				/for i 1 to 22
					/varset itemName ${InvSlot[${i}].Item}
					/if (${itemName.Find[${ItemToFind}]}) /docommand ${ChatToggle} [${InvSlot[${i}].Item}] -> (${InvSlot[${i}].Name})
				/next i
				
				| -Search packslots.
				/for i 1 to 8	
					/varset itemName ${Me.Inventory[pack${i}]}
					/if (${itemName.Find[${ItemToFind}]}) /docommand ${ChatToggle} [${Me.Inventory[pack${i}]}] -> (InvSlot${i})
					
					/if (${Me.Inventory[pack${i}].Container}) {
						/for e 1 to ${Me.Inventory[pack${i}].Container}
							/varset itemName ${Me.Inventory[pack${i}].Item[${e}]}
							/if (${itemName.Find[${ItemToFind}]}) /docommand ${ChatToggle} [${Me.Inventory[pack${i}].Item[${e}]}] -> (${Me.Inventory[pack${i}]} -> InvSlot${i} -> PackSlot${e})
						/next e
					}	
				/next i
			}
			
			/if (${FindItemBankCount[${ItemToFind}]}) {
			
				| -Search packslots.
				/for i 1 to 26	
					/varset itemName ${Me.Bank[${i}]}
					/if (${itemName.Find[${ItemToFind}]}) /docommand ${ChatToggle} [${Me.Bank[${i}]}] -> (BankSlot${i})
					
					/if (${Me.Bank[${i}].Container}) {
						/for e 1 to ${Me.Bank[${i}].Container}
							/varset itemName ${Me.Bank[${i}].Item[${e}]}
							/if (${itemName.Find[${ItemToFind}]}) /docommand ${ChatToggle} [${Me.Bank[${i}].Item[${e}]}] -> (${Me.Bank[${i}]}] -> BankSlot${i} -> PackSlot${e})
						/next e
					}
				/next i
			}
		}
	}

/if (${Debug}) /echo <== EVENT_FindItems -|
/RETURN



|------------------------------------------------------------|
|- Checks for bots who are out of range, or out of zone.	-|
|------------------------------------------------------------|
|- Use, /count, or, /bc Count bots							-|
|------------------------------------------------------------|
#EVENT CountBots "[#*#] Count"
#EVENT CountBots "<#*#> Count"
#EVENT CountBots "[#*#] Count Bots"
#EVENT CountBots "<#*#> Count Bots"
#EVENT CountBots "[#*#] RollCall"
#EVENT CountBots "<#*#> RollCall"
#EVENT CountBots "[#*#] Roll Call"
#EVENT CountBots "<#*#> Roll Call"
#EVENT CountBots "[#*#] HeadCount"
#EVENT CountBots "<#*#> HeadCount"
#EVENT CountBots "[#*#] Head Count"
#EVENT CountBots "<#*#> Head Count"
SUB EVENT_CountBots
/if (${Debug}) /echo |- EVENT_CountBots ==>

	/if (!${NetBots.Counts}) {
		/echo You must be connected to MQ2EQBC, and, MQ2NetBots, to use this function.
	} else {
		/declare i int local
		
		/declare InRangeCount int local
		
		/declare OutOfRange string local
		/declare OutOfRangeCount int local
		
		/declare OutOfZone string local
		/declare OutOfZoneCount int local
		
		/for i 1 to ${NetBots.Counts}
		
			/if (!${SpawnCount[pc ${NetBots.Client[${i}]}]}) {
				/varset OutOfZone ${OutOfZone}${NetBots.Client[${i}]},
				/varcalc OutOfZoneCount ${OutOfZoneCount} + 1
			} else /if (${SpawnCount[pc ${NetBots.Client[${i}]}]} && !${SpawnCount[pc ${NetBots.Client[${i}]} radius ${MaxResponseDist}]}) {
				/varset OutOfRange ${OutOfRange}${NetBots.Client[${i}]},
				/varcalc OutOfRangeCount ${OutOfRangeCount} + 1
			} else {
				/varcalc InRangeCount ${InRangeCount} + 1
			}
		
		/next i
	}
	
	/if (${NetBots.Counts} < 2) {
		/echo ------------------------------------------
		/echo No other NetBots are connected.
		/echo ------------------------------------------
	} else {
		/echo ------------------------------------------
		/echo [${NetBots.Counts}] NetBots connected.
		/echo ------------------------------------------
	}
	
	/if (${InRangeCount} > 1) {
		/echo [${InRangeCount}] NetBots are present.
		/echo ------------------------------------------
	}
	
	/if (${OutOfRange.Length}) {
		/varset i 1
		/echo [${OutOfRangeCount}] NetBots out of range:
		:next_OutOfRange
		/echo ----- ${OutOfRange.Arg[${i},,]}, ${Int[${Spawn[pc ${OutOfRange.Arg[${i},,]}].Distance}]} units away.
		/varcalc i ${i} + 1
		/if (${OutOfRange.Arg[${i},,].Length}) /goto :next_OutOfRange
		/echo ------------------------------------------
	}
	
	/if (${OutOfZone.Length}) {
		/varset i 1

		/echo [${OutOfZoneCount}] NetBots out of zone:
		:next_OutOfZone
		/echo ----- ${OutOfZone.Arg[${i},,]} is in, ${Zone[${NetBots[${OutOfZone.Arg[${i},,]}].Zone}]}.
		/varcalc i ${i} + 1
		/if (${OutOfZone.Arg[${i},,].Length}) /goto :next_OutOfZone
		/echo ------------------------------------------
	}
	
/if (${Debug}) /echo <== EVENT_CountBots -|
/RETURN



|----------------------------------------------------|
|- Preforms item swaps, listed in Character_Inis.	-|
|----------------------------------------------------|
#EVENT Swap_Items "<#*#> Swap #1#"
#EVENT Swap_Items "[MQ2] Swap #1#"
SUB EVENT_Swap_Items(line, IniEntry)
/if (${Debug}) /echo |- Event Swap_Items ==>

	| If I don't have a matching Ini entry.
	/if (!${Ini[${Character_Ini},Swap Items,${IniEntry}].Length}) {
		/echo |- Swap_Items -| I have no listing for [${IniEntry}].
	} else {
	
		/if (${Cursor.ID}) /call ClearCursor
		/if (${Bool[${Me.Casting}]} && ${Me.Class.ShortName.NotEqual[BRD]}) /delay 300 !${Bool[${Me.Casting}]}
	
		/declare i int local 1
		
		/declare itemToSwap string local
		/declare slotToSwap string local
		
		:SwapLoop
		/if (${Debug}) /echo |- EVENT_Swap_Items -| Swaploop.
		
		/varset itemToSwap ${Ini[${Character_Ini},Swap Items,${IniEntry}].Arg[${i},,].Arg[1,/]}
		/varset slotToSwap ${Ini[${Character_Ini},Swap Items,${IniEntry}].Arg[${i},,].Arg[2,/]}
		
		| If I don't have the item in my inventory.
		/if (!${FindItemCount[${itemToSwap}]}) {
			/echo |- Event Swap_Items -| #*# Error #*# [${itemToSwap}] was not found.
			/varcalc i + 1
			/if (${Ini[${Character_Ini},Swap Items,${IniEntry}].Arg[${i},,].Length}) /goto :SwapLoop
		} else {
		
			| If I'm swapping in a 2 hander, while dual wielding.
			/if (${slotToSwap.Equal[Mainhand]} && ${FindItem[${itemToSwap}].Type.Left[2].Equal[2H]} && ${InvSlot[Offhand].ID}) /unequip Offhand
		
			| Swap in item.
			/echo Equipping [${itemToSwap}]
			/call SwapItem "${itemToSwap}" ${slotToSwap}
			
			/varcalc i ${i} + 1
			/if (${Ini[${Character_Ini},Swap Items,${IniEntry}].Arg[${i},,].Length}) /goto :SwapLoop
		}
	}

/if (${Debug}) /echo <== Swap_Items -|
/RETURN



#EVENT useItem "[MQ2] Use Item #1#"
SUB EVENT_useItem(line, itemToUse)
/if (${Debug}) /echo |- EVENT_useItem ==>

	:TryAgain
	| if I cant find the item
	/if (!${FindItemCount[=${itemToUse}]}) {
	
		| check ini for an alias
		/if (${Ini[${Character_Ini},Use Items,${itemToUse}].Length}) {
			/varset itemToUse ${Ini[${Character_Ini},Use Items,${itemToUse}]}
			/goto :TryAgain
		} else {
			/echo Could not find [${itemToUse}].
		}
	} else {
		
		| check item timers
		/if (${FindItem[=${itemToUse}].Timer} || ${itemTimer_${FindItem[=${itemToUse}].ID}}) {
			/echo [${itemToUse}] is not ready! [${If[${FindItem[=${itemToUse}].Timer},${FindItem[=${itemToUse}].Timer} ticks,${Math.Calc[${itemTimer_${FindItem[=${itemToUse}].ID}}/10].Int} sec]}]
		} else {

			| cast the item
			/if (${Me.Casting.ID}) /call Interrupt
			/if (${Cursor.ID}) /call clearCursor
			/if (${Target.ID}) /declare oldTarget int local ${Target.ID}

			/call e3_Cast ${Me.ID} "${itemToUse}" item
			/if (${oldTarget}) /call TrueTarget ${oldTarget}
		}
	}

/if (${Debug}) /echo <== EVENT_useItem -|
/RETURN



|----------------------------------------------------|
|- Ends the macro before a bot finishes camping.	-|
|----------------------------------------------------|
#EVENT EndMacro "It will take about 5 more seconds to prepare your camp."
SUB EVENT_EndMacro
	/echo User is camping, ending macro.
	/endmacro
/RETURN



|----------------------------------------|
|- Consents a PC to drag your corpse.	-|
|----------------------------------------|
#EVENT Consent "[#1#] Consent #2#"
#EVENT Consent "<#1#> Consent #2#"
#EVENT Consent "#1# tells you, 'Consent #2#'"
#EVENT Consent "#1# tells the group, 'Consent #2#'"
#EVENT Consent "#1# you tell your party, 'Consent #2#'"
SUB Event_Consent(line, ChatSender, ConsentTarget)
/if (${Debug}) /echo |- EVENT_Consent ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]}) {
	
		| -Reset ConsentTarget
		/if (${ConsentTarget.Find[-]}) {
			/varset ConsentTarget ${ConsentTarget.Arg[1,-]}
			/varset ConsentTarget ${ConsentTarget.Left[${Math.Calc[${ConsentTarget.Length} - 1].Int}]}
		}
		
		/if (${ConsentTarget.NotEqual[${Me.CleanName}]}) {
			/if (${Verbosity}) /docommand ${ChatToggle} Consenting ${ConsentTarget}
			/consent ${ConsentTarget}
		}
	}

/if (${Debug}) /echo <== EVENT_Consent -|
/RETURN



|----------------------------------------|
|- Automatically accepts group invites.	-|
|----------------------------------------|
#EVENT Invite "#1# invites you to join a group."
SUB EVENT_Invite(line, ChatSender)
/if (${Debug}) /echo |- EVENT_Invite ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]}) {
	
		| -Wait to finish casting
		/if (${Me.Casting.ID}) /delay 300 !${Me.Casting.ID}
	
		| -If the groupleader was valid, disband a current group, and join the new one.
		/if (${Group} || ${Raid.Members}) /call EVENT_Disband
		
		/declare retryTimer timer local 150
		
		:retry_Invite
		/if (${Debug}) /echo |- EVENT_Invite -| retry_Invite loop.
		
		/call Background_Events
		/invite
		/delay 5 ${Group}
		
		/if (!${Group}) {
			/if (${retryTimer}) {
				/goto :retry_Invite
			} else {
				/echo Failed to join the group.
			}
		}
	}	

/if (${Debug}) /echo <== EVENT_Invite -|
/RETURN



|----------------------------------------|
|- Disband bots from group and raid.	-|
|----------------------------------------|
|- Use, /bc disband						-|
|----------------------------------------|
#EVENT Disband "#1# tells you, 'Incoming group invite...'"
#EVENT Disband "<#1#> Disband all"
SUB EVENT_Disband(line, ChatSender)
/if (${Debug}) /echo |- EVENT_Disband ==>

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]}) {
	
		| -Wait to stop casting.
		/if (${Me.Casting.ID}) /delay 300 !${Me.Casting.ID}
	
		| -Disband from a raid.
		/if (${Raid.Members}) {
			/call TrueTarget ${Me.ID}
			/raiddisband
			/delay 10 !${Raid.Members}
		}
		
		| -Disband from a group.
		/if (${Group}) {
			/call TrueTarget ${Me.ID}
			/disband
			/delay 10 !${Group}
		}
		
		/squelch /target clear
	}

/if (${Debug}) /echo <== EVENT_Disband -|
/RETURN



|--------------------------------------------------------------------------------------------|
|- Groupleaders save current group formations to the Groups.ini file, to be recalled later.	-|
|--------------------------------------------------------------------------------------------|
|- Use, /savegroup 'Name', /bc Save Group 'Name', or /bc SetGroup 'Name'					-|
|--------------------------------------------------------------------------------------------|
#EVENT saveGroup "<#1#> Save Group #2#"
#EVENT saveGroup "<#1#> SaveGroup #2#"
SUB EVENT_saveGroup(line, ChatSender, groupName)
/if (${Debug}) /echo |- EVENT_saveGroup ==>

	| -Check that I'm part of the group.
	/if (!${Group} && ${ChatSender.Equal[${Me}]}) {
		/echo setGroup Error: You need to make a group first, dumbass.
	} else {

		| -Check that I'm the group leader.
		/if (${Select[${Group.Leader},${Me}]}) {
		
			| -Check for an -OverWrite command.
			/if (${groupName.Find[ -OverWrite]}) {
				/declare overWrite bool local TRUE
				/varset groupName ${groupName.Left[${Math.Calc[${groupName.Length} - 11].Int}]}
			}
			
			| -Check for an existing Ini entry.
			/if (${Ini[${Group_Ini},${MacroQuest.Server}_${Me}_${groupName}].Length}) {
			
				| -Check for an -OverWrite command
				/if (!${overWrite}) {
					/echo I already have a save group named, [${groupName}].  Choose a new name, or add '-OverWrite' to overwrite the exising group.  Ex, '/SaveGroup Main Group -Overwrite'
				} else {
					/goto :OverWrite
				}
			} else {
			
				:OverWrite
				
				/echo Saving current group. Use /group ${groupName} to recall this group.

				/declare i int local
				
				/for i 1 to 5
				
					| -Save group members 1-5.
					/if (${Bool[${Group.Member[${i}]}]}) {
						/call WriteToIni "${Group_Ini},${MacroQuest.Server}_${Me}_${groupName},GroupMember#${i}" ${Group.Member[${i}]} 1
					
					| -If the group has less that 5 members, set placeholders
					} else {
						/call WriteToIni "${Group_Ini},${MacroQuest.Server}_${Me}_${groupName},GroupMember#${i}" PLACEHOLDER 1
					}
				/next i
			}
		}
	}

/if (${Debug}) /echo <== EVENT_saveGroup -|
/RETURN



|---------------------------------------------------------|
|- Lists all saved group setups, on your current server. -|
|---------------------------------------------------------|
#EVENT listGroups "<#1#> List Groups"
#EVENT listGroups "<#1#> ListGroups"
#EVENT listGroups "[MQ2] List Groups"
#EVENT listGroups "[MQ2] ListGroups"
SUB EVENT_listGroups
/if (${Debug}) /echo |- listGroups ==>

	| -Check Group_Ini for groups on this server.
	/if (!${Ini[${Group_Ini}].Find[${MacroQuest.Server}]}) {
		/echo You don't have any group setups saved on, [${MacroQuest.Server}].
	} else {
	
		/echo The following group setups are saved on [${MacroQuest.Server}].
		
		/declare i int local 1
		/declare e int local
		
		:next_groupListing

		/if (${Ini[${Group_Ini}].Arg[${i},|].Arg[1,_].Find[${MacroQuest.Server}]}) {
			/echo
			/echo --- Group: [${Ini[${Group_Ini}].Arg[${i},|].Arg[3,_]}] ---
			/echo Leader: ${Ini[${Group_Ini}].Arg[${i},|].Arg[2,_]}
			
			| -Reset 'e' the group member counting variable.
			/varset e 1
			
			:next_memberListing
			
			| -Check that the entry is not a PLACEHOLDER.
			/if (${Ini[${Group_Ini},${Ini[${Group_Ini}].Arg[${i},|]},${Ini[${Group_Ini},${Ini[${Group_Ini}].Arg[${i},|]}].Arg[${e},|]}].NotEqual[PLACEHOLDER]}) {
				/echo Group member#${e}: ${Ini[${Group_Ini},${Ini[${Group_Ini}].Arg[${i},|]},${Ini[${Group_Ini},${Ini[${Group_Ini}].Arg[${i},|]}].Arg[${e},|]}]}
				/varcalc e ${e} + 1
				/if (${Ini[${Group_Ini},${Ini[${Group_Ini}].Arg[${i},|]},${Ini[${Group_Ini},${Ini[${Group_Ini}].Arg[${i},|]}].Arg[${e},|]}].Length}) /goto :next_memberListing
			}
		}	

		/varcalc i ${i} + 1
		/if (${Ini[${Group_Ini}].Arg[${i},|].Length}) /goto :next_groupListing
		/echo	
	}

/if (${Debug}) /echo <== listGroups -|
/RETURN



|------------------------------------------------|
|- Recalls groups saved in your Groups.ini.		-|
|------------------------------------------------|
|- Use, /group 'Name', or, /bc Group 'Name'.	-|
|------------------------------------------------|
#EVENT groupUp "<#*#> Group #1#"
SUB EVENT_groupUp(line, groupName)
/if (${Debug}) /echo |- EVENT_groupUp ==>

	| -Check for groupName in Group_Ini.
	/if (${Ini[${Group_Ini},${MacroQuest.Server}_${Me}_${groupName}].Length}) {
	
		/call EVENT_Disband "PLACEHOLDER" "${Me.CleanName}"
	
		/echo Recalling [${groupName}] configuration...
		
		| -Build a list of people to invite.
		/call IniToArray "${Group_Ini},${MacroQuest.Server}_${Me}_${groupName},GroupMember#" groupMembers
		
		/declare tellDelay int local 20
		/declare formingGroup bool outer TRUE
		/declare PC_Online bool outer FALSE
		/declare PC_Checked bool outer
		
		/declare tellTimer timer local
		/declare retryTimer timer local
		/declare i int local
		
		/for i 1 to 5
		
			/if (${groupMembers[${i}].NotEqual[PLACEHOLDER]}) {
				
				| -Alert PC of incoming group invite.
				/tell ${groupMembers[${i}]} Incoming group invite...
				
				/varset PC_Checked FALSE
				/varset tellTimer ${MacroQuest.Ping}
				
				| -Wait for tell to be sent, to check if PC is online
				:waitResponse
				/doevents checkPCOnline
				
				/if (!${PC_Checked}) {
					/if (${tellTimer}) {
						/goto :waitResponse
					} else {
						/echo I failed to receive a response from [${groupMembers[${i}]}].
					}
				} else {

					| -Set the retryTimer for 30 seconds
					/varset retryTimer 300
					
					| -Attempt to invite
					:retryInvite
					/invite ${groupMembers[${i}]}
					/delay 30 ${Group.Member[${groupMembers[${i}]}]}
				
					| -If the character has not joined the group
					/if (!${Group.Member[${groupMembers[${i}]}]}) {
						/if (${retryTimer}) {
							/goto :retryInvite
						} else {
							/echo [${groupMembers[${i}]}] failed to join the group.
						}
					}
				
					| -Make sure we have waited long enough, before sending the next tell.
					/if (${Math.Calc[${MacroQuest.Ping} - ${tellTimer}].Int} < ${tellDelay}) /delay ${Math.Calc[${tellDelay} - ${Math.Calc[${MacroQuest.Ping} - ${tellTimer}].Int}].Int}
				}
			}

		/next i
		
		/deletevar formingGroup
		/deletevar PC_Online
		/deletevar PC_Checked
	}

/if (${Debug}) /echo <== EVENT_groupUp -|
/RETURN



#EVENT checkPCOnline "You told #1#, '#*#'"
SUB EVENT_checkPCOnline(line, offLine_PC)
/if (${Debug}) /echo |- EVENT_pcOffline ==>
	
	/if (${Defined[PC_Online]}) {
		
		/varset PC_Checked TRUE
		/varset PC_Online FALSE

		| -Check if the PC I sent a tell to was offline.
		/if (${line.Find[is not online at this time]}) {
			/echo [${offLine_PC}] is not online.
		} else {
			/varset PC_Online TRUE
		}
	}

/if (${Debug}) /echo <== EVENT_pcOffline -|
/RETURN



| #*#
|------------------------------------------------|
|- Engages MedBreak mode, on bots who can cast.	-|
|------------------------------------------------|
#EVENT medBreak_on "<#1#> Start MedBreak#*#"
#EVENT medBreak_on "<#1#> Med On#*#"
#EVENT medBreak_on "<#1#> MedBreak On#*#"
#EVENT medBreak_on "<#1#> Start Meditating#*#"
#EVENT medBreak_on "<#1#> Meditate on#*#"
SUB EVENT_medBreak_on(line, ChatSender)
/if (${Debug}) /echo |- EVENT_medBreak_on ==>

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
			
			/if (${Spawn[pc ${ChatSender}].Distance} <= ${MaxResponseDist}) {
				/docommand ${ChatToggle} Meditating...
			} else /if (${Spawn[pc ${ChatSender}].Distance} > ${MaxResponseDist}) {
				/docommand ${ChatToggle} I am too far away from you to meditate, right now.  I'll start meditating when you're in range.
			}
		}
	}
	
/if (${Debug}) /echo <== EVENT_medBreak_on -|
/RETURN



| #*#
|----------------------------------------------------|
|- disengages MedBreak mode, on bots who can cast.	-|
|----------------------------------------------------|
#EVENT medBreak_off "<#*#> End MedBreak"
#EVENT medBreak_off "<#*#> Stop MedBreak"
#EVENT medBreak_off "<#*#> End Med"
#EVENT medBreak_off "<#*#> Stop Med"
#EVENT medBreak_off "<#*#> Med off"
#EVENT medBreak_off "<#*#> MedBreak off"
#EVENT medBreak_off "<#*#> Meditate off"
#EVENT medBreak_off "<#*#> Stop meditating"
SUB EVENT_medBreak_off
/if (${Debug}) /echo |- EVENT_medBreak_off ==>

	/if (${medBreak}) {
		/if (${Verbosity}) /docommand ${ChatToggle} I am no longer meditating.
		/if (${Me.Sitting}) /stand
		/if (${Defined[medBreak]}) /deletevar medBreak
		/if (${Defined[medBreak_Caller]}) /deletevar medBreak_Caller
		/if (${Defined[medBreak_Hold]}) /deletevar medBreak_Hold
	}
	
/if (${Debug}) /echo <== EVENT_medBreak_off -|
/RETURN



| #*#
|--------------------------------------------------------------------------------|
|- MedBreak hook for Tertiary loop.												-|
|--------------------------------------------------------------------------------|
|- Automatically disengages and re-engages MedBreak when conditions are met.	-|
|- Ends MedBreak mode at 99% mana, unless, 'MedBreak hold command is given.		-|
|--------------------------------------------------------------------------------|
SUB check_MedBreak
/if (${Debug}) /echo |- check_MedBreak ==>

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

/if (${Debug}) /echo <== check_MedBreak -|
/RETURN



Sub basics_Setup
/if (${Debug}) /echo |- basics_Setup ==>

	/declare MeleeClasses string outer BRD,BER,BST,MNK,PAL,RNG,ROG,SHD,WAR
	/declare PriestClasses string outer CLR,DRU,SHM
	/declare CasterClasses string outer ENC,MAG,NEC,WIZ

	/if (!${Defined[Following]}) /declare Following bool global FALSE
	/if (!${Defined[FollowTarget]}) /declare FollowTarget string global NULL
	/if (!${Defined[Followers]}) /declare Followers string global NULL

	/if (!${Defined[consentTargets]}) /declare consentTargets string global ${Me.CleanName}
	
	/declare MaxResponseDist int outer 250
	/if (${Ini[${Macro_Ini},General,Max Response Distance].Length} && ${Int[${Ini[${Macro_Ini},General,Max Response Distance]}]}) /call IniToVar "${Macro_Ini},General,Max Response Distance" MaxResponseDist int outer

	/declare LeashLength int outer 125
	/if (${Ini[${Macro_Ini},General,Leash Length].Length} && ${Int[${Ini[${Macro_Ini},General,Leash Length]}]}) /call IniToVar "${Macro_Ini},General,Leash Length" LeashLength int outer
	
	| -Add Groups_Ini file path
	/if (!${Ini[e3 File Paths.ini,File Paths,Saved Groups].Length}) /call WriteToIni "e3 File Paths.ini,File Paths,Saved Groups" "Macro Inis\Groups.ini" 1
	
	| -Import Groups_Ini.
	/if (!${Ini[e3 File Paths.ini,File Paths,Saved Groups].Length}) {
		/echo ERROR: Could not find designated file path for [Saved Groups], please review [e3 File Paths.ini] and restart.
		/endmacro
	} else {
		/declare Group_Ini string outer ${Ini[e3 File Paths.ini,File Paths,Saved Groups]}
	}

/if (${Debug}) /echo <== basics_Setup -|
/return



Sub basics_Background_Events

/doevents add_AutoDestroyItem
/doevents remove_AutoDestroyItem
/doevents Set_zoneType
/doevents Stop
/doevents barkReturn
/doevents Exit
/doevents RemoveBuff
/doevents CheckBuffs
/doevents FindItems
/doevents CountBots
/doevents EndMacro
/doevents Consent
/doevents Invite
/doevents Disband
/doevents saveGroup
/doevents listGroups
/doevents groupUp
/doevents checkPCOnline

/return



SUB basics_MacroSettings
/if (${Debug}) /echo |- basics_MacroSettings ==>

	/call WriteToIni "${Macro_Ini},General,Max Response Distance" 500
	/call WriteToIni "${Macro_Ini},General,Leash Length" 100

/if (${Debug}) /echo <== basics_MacroSettings -|
/RETURN



SUB basics_CharacterSettings
/if (${Debug}) /echo |- basics_CharacterSettings ==>

	/call WriteToIni "${Character_Ini},Swap Items,;(Example)Two Hander" "Lodestar/mainhand"
	/call WriteToIni "${Character_Ini},Swap Items,;(Example)One Handers" "Heartwood Blade/mainhand,Chaotic Black Scimitar/offhand"
	/call WriteToIni "${Character_Ini},Use Items,;(Example)ShinkItem" "Bracelet of the Shadow Hive"

/if (${Debug}) /echo <== basics_CharacterSettings -|
/RETURN



Sub basics_Aliases
/if (${Debug}) /echo |- basics_Aliases ==>

	/squelch /alias /followme delete
	/squelch /alias /followit delete
	/alias /followon /bc Follow ${Me.CleanName}
	/alias /followme /bc Follow ${Me.CleanName}
	/alias /followit /bc Follow ${Target.CleanName}
	
	/squelch /alias /stop delete
	/alias /stop /bc Stop
	
	/squelch /alias /mtm delete
	/squelch /alias /mtt delete
	/squelch /alias /movetome delete
	/squelch /alias /movetotarget delete
	/alias /mtm /bc Move to me
	/alias /mtt /bc Move to ${Target.ID}
	/alias /movetome /bc Move to me
	/alias /movetotarget /bc Move to ${Target.ID}
	
	/squelch /alias /bark delete
	/alias /Bark /bc Bark at -id|${Target.ID}
	
	/squelch /alias /clickit delete
	/alias /ClickIt /bc Click object at ${Me.Loc.Replace[ ,]}
	
	/squelch /alias /runthruzone delete
	/squelch /alias /RunThroughZone delete
	/squelch /alias /RTZ delete
	/alias /RunThruZone /bc Run through zone at ${Me.Loc.Replace[ ,]} ${Me.Heading.Degrees}
	/alias /RunThroughZone /bc Run through zone at ${Me.Loc.Replace[ ,]} ${Me.Heading.Degrees}
	/alias /RTZ /bc Run through zone at ${Me.Loc.Replace[ ,]} ${Me.Heading.Degrees}
	
	/squelch /alias /ClkYes delete
	/squelch /alias /Yes delete
	/alias /ClkYes /bc Click yes
	/alias /Yes /bc Click yes
	
	/squelch /alias /DropInvis delete
	/alias /DropInvis /bc Drop Invisibility
	
	/squelch /alias /BotExit delete
	/alias /BotExit /bc Exit

	/squelch /alias /Gate delete
	/alias /Gate /bc Gate
	
	/squelch /alias /Evac delete
	/alias /Evac /bc Evac
	
	/squelch /alias /LoadSpells /delete
	/alias /LoadSpells /bc Load Spells
	
	/squelch /alias /RemoveBuff delete
	/alias /RemoveBuff /bc Remove Buff
	
	/squelch /alias /CheckBuff delete
	/alias /CheckBuff /bc Check Buff
	
	/squelch /alias /FindItem delete
	/squelch /alias /FindIt delete
	/alias /FindItem /bc Find Item
	/alias /FindIt /bc Find Item ${Cursor}
	
	/squelch /alias /CountBots delete
	/squelch /alias /Count delete
	/squelch /alias /BotCount delete
	/squelch /alias /RollCall delete
	/alias /CountBots /echo Head Count
	/alias /Count /echo Head Count
	/alias /BotCount /echo Head Count
	/alias /RollCall /echo Head Count
	
	/squelch /alias /Swap delete
	/alias /Swap /echo Swap
	
	/squelch /alias /useitem delete
	/alias /useitem /echo Use Item
	
	/squelch /alias /Consentme delete
	/alias /ConsentMe /bc Consent ${Me.CleanName}
	
	/squelch /alias /Group delete
	/squelch /alias /GroupUp delete
	/squelch /alias /SaveGroup delete
	/squelch /alias /SaveGroups delete
	/squelch /alias /SetGroup delete
	/squelch /alias /SetGroups delete
	/squelch /alias /listgroups delete
	/squelch /alias /DisbandAll delete
	/squelch /alias /DisbandBots delete
	
	/alias /Group /bc Group
	/alias /GroupUp /bc Group
	/alias /SaveGroup /bc Save Group
	/alias /SaveGroups /bc Save Group
	/alias /listgroups /bc List Groups
	/alias /DisbandAll /bc Disband all
	/alias /DisbandBots /bc Disband all

/if (${Debug}) /echo <== basics_Aliases -|
/return