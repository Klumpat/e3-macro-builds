|--------------------------------------------------------------------------------|
|- e3_Macro_SmoothMoves.inc v2.6 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ													-|
|--------------------------------------------------------------------------------|
|- Records and saves routes to Routes_Ini. 										-|
|- Routes can be recalled later to run you from one place to another.			-|
|--------------------------------------------------------------------------------|
|- v2.6 changes,																-|
|- Spaces will no longer be replaced with '.'s when route names are saved.		-|
|- Bark functions now work properly with mob names that contain spaces.			-|
|- Minor rework of EVENT_run_Route.												-|
|- Recording functions have been changed to use a string, instead of an array.	-|
|- 
|--------------------------------------------------------------------------------|



|--------------------------------------------|
|- Commands:								-|
|- 	All commands are given in /echo, except -|
|- 'Run route' which can be given in /echo,	-|
|-  or /bc. 								-|
|--------------------------------------------|
|-	/echo Record Route						-|
|- 	/echo Begin recording					-|
|- 	/echo Start recording					-|
|--------------------------------------------|
|-	/echo Add Click door					-|
|-	/echo Add Use door						-|
|-	/echo Add Open door						-|
|--------------------------------------------|
|-	/echo Add Click zone					-|
|-	/echo Add Use zone						-|
|-	/echo Add Click stone					-|
|-	/echo Add Use stone						-|
|--------------------------------------------|
|- 	/echo Add Run through zone				-|
|-	/echo Add Run thru zone					-|
|--------------------------------------------|
|- 	/echo Add Bark 'msg'					-|
|-	/echo End Recording						-|
|-	/echo Stop recording					-|
|-	/echo Finish recording					-|
|--------------------------------------------|
|-	/echo Save route [NAME]					-|
|-	/echo Save route [NAME] -Overwrite		-|
|--------------------------------------------|
|-	/echo List routes						-|
|--------------------------------------------|
|-	/echo Run route NAME					-|
|--------------------------------------------|



SUB SmoothMoves_Setup
/if (!${Defined[SmoothMoves_Ini]}) /declare SmoothMoves_Ini string outer Routes.ini
/declare tryToZone_Timer_Setting string outer 2m
/declare SmoothMoves bool outer TRUE
/RETURN



|----------------------------------------|
|- Events to call while running routes.	-|
|----------------------------------------|
SUB Movement_Events
/doevents stop_Route
/call Background_Events
/RETURN



|--------------------------------------------|
|- Events to call while recording routes.	-|
|--------------------------------------------|
SUB Recording_Events
/doevents end_Recording
/doevents save_Recording
/doevents record_Route
/doevents add_click_Door
/doevents add_run_Thru_Zone
/doevents add_click_Zone
/doevents add_Bark
/RETURN



|----------------------------------------------------------------------------|
|- Records route by marking waypoints, at changes in heading.				-|
|----------------------------------------------------------------------------|								
|- For best results use a -move, stop, turn- method when recording routes.	-|
|----------------------------------------------------------------------------|
#event record_Route "[MQ2] Record Route"
#event record_Route "[MQ2] Begin recording"
#event record_Route "[MQ2] Start recording"
SUB event_record_Route
| /varset Debug TRUE
/if (${Debug}) /echo |- event_record_Route ==>

|------------------------------------------------------------------------------Output route recording instructions, and begin recording the route.
	/echo ** Use 'Click door' 'Click zone' and 'Run through zone' to add advanced functions to your route.  Use 'End route' or 'Save route' to stop recording. **
	/echo ** When recording routes it is best to change direction as infrequently as possible. To make the most efficient routes, run in straight lines, stop to face your new heading, then continue in a straight line. **
	/echo Recording movement...

|-----------------------------------------------------------Delete an existing tempRoute before starting a new one.
	/if (${Defined[tempRoute]}) /deletevar tempRoute
	/declare tempRoute string outer ${Me.Loc.Replace[ ,]}|

|----------------------------------------------------------------Declare the starting zone of this recording. (This is checked as the recording is saved.)
	/if (${Defined[tempRouteStartZone]}) /deletevar tempRouteStartZone
	/declare tempRouteStartZone int outer ${Zone.ID}	
	
|---------------------------------------------------------------------------------------Declare recording variables.
	/if (!${Defined[keep_Tracing]}) /declare keep_Tracing bool outer TRUE
	/declare old_Heading float local ${Me.Heading.Degrees}
	/declare old_Loc string local ${Me.Loc.Replace[ ,]}
	/declare popup_Timer timer local
	
	/if (${Defined[contRecording]}) /deletevar contRecording
	/declare contRecording bool outer TRUE

|-------------------------------------------------------------------------------------------------------------------------Start tracing the route.
	:trace_Route_Loop
	/if (${Debug}) /echo |- event_record_Route -| :trace_Route_Loop
	
|-----------------------------------------------------------------------------Popup recording alert every 5 sec.
	/if (!${popup_Timer}) {
		/popup ** Recording Route **
		/varset popup_Timer 5s
	}
		
	/call Triangulate_Distance ${old_Loc} ${Me.Loc.Replace[ ,]}
	
|------------------------------------------------------------------If my heading has changed, or I have moved more than 250 units, set a new heading.
	/if (${Me.Heading.Degrees}!=${old_Heading} || ${Int[${Macro.Return}]} > 250) {
		/varset old_Heading ${Me.Heading.Degrees}
		
	|------------------------------------------------------If my location has changed, check how far I've moved.
		/if (${Me.Y}!=${old_Loc.Arg[1,,]} || ${Me.X}!=${old_Loc.Arg[2,,]}) {
					
		|-----------------------------------------If I've moved more than 10 units, add a waypoint to the array.
			/if (${Int[${Macro.Return}]} > 10) {
				/varset old_Loc ${Me.Loc.Replace[ ,]}
				/varset tempRoute ${tempRoute}${Me.Loc.Replace[ ,]}|
				/if (${Debug}) /echo |- event_record_Route -| Adding waypoint [${Me.Loc}]. -- ${tempRoute}
			}
		}
	}

|----------------------------------------------------------------------------------Check recording events, and if I should continue recording.
	/call Recording_Events
	/if (${contRecording}) /goto :trace_Route_Loop

	/doevents flush

/if (${Debug}) /echo <== event_record_Route -|
| /varset Debug FALSE
/RETURN



|----------------------------------------------------------------|
|- Adds a door click to the route you're currently recording.	-|
|----------------------------------------------------------------|
#event add_click_Door "[MQ2] Add Open Door"
#event add_click_Door "[MQ2] Add Use Door"
#event add_click_Door "[MQ2] Add Click Door"
SUB event_add_click_Door
/if (${Debug}) /echo |- add_click_Door ==>

	/if (!${Defined[tempRoute]}) {
		/echo I'm not currently recording a route, dumbass.
	} else {
		/echo Adding door click to route...
		/varset tempRoute ${tempRoute}${Me.Loc.Replace[ ,]}_**clickDoor_${Me.Heading.Degrees}_${Me.Look}|
		/if (${Debug}) /echo |- event_add_click_Door -| Adding door click at [${Me.Loc}].  -- ${tempRoute}
	}
	
/if (${Debug}) /echo <== add_click_Door -|
/RETURN



|----------------------------------------------------------------|
|- Adds a zone click to the route you're currently recording.	-|
|----------------------------------------------------------------|
#event add_click_Zone "[MQ2] Add Click Stone"
#event add_click_Zone "[MQ2] Add Use Stone"
#event add_click_Zone "[MQ2] Add Click Zone"
#event add_click_Zone "[MQ2] Add Use Zone"
SUB event_add_click_Zone
/if (${Debug}) /echo |- event_add_click_Zone ==>

	/if (!${Defined[tempRoute]}) {
		/echo I'm not currently recording a route, dumbass.
	} else {
		/echo Adding zone click...
		/call event_end_Recording
		/echo ** To continue this route through a zone line, please save the route in this zone, then record a route in your destination zone with the same name. **
		/varset tempRoute ${tempRoute}${Me.Loc.Replace[ ,]}_**clickZone_${Me.Heading.Degrees}_${Me.Look}|
		/if (${Debug}) /echo |- event_add_click_Zone -| Adding zone click at [${Me.Loc}].  -- ${tempRoute}
	}
	
/if (${Debug}) /echo <== event_add_click_Zone -|
/RETURN



|--------------------------------------------------------------------|
|- Adds a run through zone to the route you're currently recording.	-|
|--------------------------------------------------------------------|
#event add_run_Thru_Zone "[MQ2] Add Run thru zone"
#event add_run_Thru_Zone "[MQ2] Add Run through zone"
SUB event_add_run_Thru_Zone
/if (${Debug}) /echo |- event_add_run_Thru_Zone ==>

	/if (!${Defined[tempRoute]}) {
		/echo I'm not currently recording a route, dumbass.
	} else {
		/echo Adding run through zone function to your route...
		/call event_end_Recording
		/echo ** To continue this route through a zone line, please save the route in this zone, then record a route in your destination zone with the same name. For example, to go from the Guild Lobby to the Plane of Time zone in Tranquility, you'd need a route named 'Plane of Time' in both PoKnowledge and PoTranquility. **
		/varset tempRoute ${tempRoute}${Me.Loc.Replace[ ,]}_**runThruZone_${Me.Heading.Degrees}_${Me.Look}|
		/if (${Debug}) /echo |- event_add_run_Thru_Zone -| Adding zone click at [${Me.Loc}].  -- ${tempRoute}
	}
	
/if (${Debug}) /echo <== event_add_run_Thru_Zone -|	
/RETURN



|----------------------------------------------------------------|
|- Adds an NPC bark to the route you're currently recording.	-|
|----------------------------------------------------------------|
|- @param line: '[MQ2] Add Bark target msg'						-|
|- @param msgTarget: The npc to bark at.						-|
|- @param barkIt: The msg to bark at the target.				-|
|----------------------------------------------------------------|
#event add_Bark "[MQ2] Add Bark #1# #2#"
#event add_Bark "[MQ2] Add Bark #1# #2# -zone"
SUB event_add_Bark(line, msgTarget, barkIt)
/if (${Debug}) /echo |- event_add_Bark ==>

	/if (!${Defined[tempRoute]}) {
		/echo I'm not currently recording a route, dumbass.
	} else {
		/echo Adding bark function to your route...
		/echo ** To continue this route through a zone line, please save the route in this zone, then record a route in your destination zone with the same name. For example, to go from the Guild Lobby to the Plane of Time zone in Tranquility, you'd need a route named 'Plane of Time' in both PoKnowledge and PoTranquility. **
		/if (${Int[${msgTarget}]} && ${Spawn[id ${msgTarget}].Type.Equal[npc]}) /varset msgTarget ${Spawn[id ${msgTarget}].Name}
		/if (${msgTarget.Find[_]}) /varset msgTarget ${Spawn[npc ${msgTarget}].Name.Replace[_,-]}
		/if (${barkIt.Find[ ]}) /varset barkIt ${barkIt.Replace[ ,^]}

	|------------------------------------------------------------------------------------------------------------------Specify zone after barking.
		/if (${line.Find[-zone]}) {
			/call BuildArray ${temp_Route_Name} ${Me.Loc.Replace[ ,]}_**barkAtMob_${msgTarget}_${barkIt}_zone
		} else {
			/call BuildArray ${temp_Route_Name} ${Me.Loc.Replace[ ,]}_**barkAtMob_${msgTarget}_${barkIt}
		}
		/if (${Debug}) /echo |- event_add_Bark -| Adding zone click at [${Me.Loc}].  -- ${tempRoute}
	}
	
/if (${Debug}) /echo <== event_add_Bark -|	
/RETURN	



|--------------------------------------------------------|
|- Stops recording, and calculates the routes length.	-|
|--------------------------------------------------------|
#event end_Recording "[MQ2] End recording"
#event end_Recording "[MQ2] Stop recording"
#event end_Recording "[MQ2] Finish recording"
SUB event_end_Recording(bool squelchEchos)
/if (${Debug}) /echo |- event_end_Recording ==>

	/if (!${Defined[tempRoute]}) {
		/echo I'm not currently recording a route, dumbass.
	} else {
	
	|----------------------------------------------------------------------------------------------------------------------------------Output to user.
		/varset tempRoute ${tempRoute}${Me.Loc.Replace[ ,]}|
		/if (!${squelchEchos}) /echo Finished recording.  To save this route, use the command '/echo Save Route [Name of Route]'.
		/if (${Defined[contRecording]}) /deletevar contRecording
	}

/if (${Debug}) /echo <== event_end_Recording -|
/RETURN



|------------------------------------------------|
|- Saves temporary routes to Routes_Ini.		-|
|------------------------------------------------|
|- @param line: '[MQ2] Save Route [Name]'		-|
|- @param routeName: Name of the route to save.	-|
|------------------------------------------------|
#event save_Recording "[MQ2] Save Route [#1#]"
#event save_Recording "[MQ2] Save Route [#1#] -Overwrite"
#event save_Recording "[MQ2] Save Route #1#"
#event save_Recording "[MQ2] Save Route #1# -Overwrite"
SUB event_save_Recording(line, routeName)
/if (${Debug}) /echo |- event_save_Recording ==>

	/if (!${Defined[tempRoute]}) {
		/echo I'm not currently recording a route, dumbass.
	} else {
		
	|-----------------------------------------------------------------------------------Setup SmoothMoves variables.
		/if (!${SmoothMoves}) /call SmoothMoves_Setup
		
	|-----------------------------------------------------------If there is already a route with this name.	
		/if (${Ini[${SmoothMoves_Ini},${Zone.ShortName},${routeName}].Length} && !${line.Find[-overwrite]}) {
			/echo [${routeName}] already exists, please choose a different name, or use '/echo Save Route [NAME] -Overwrite.
		} else {
			
			/call EVENT_end_Recording 1
			/echo Saving route as [${routeName}]...
			
		|------------------------------------------------------------------------------------------------------------Add the string to Routes_Ini.
			/ini ${SmoothMoves_Ini} ${Zone.ShortName} "${routeName}" ${tempRoute}
			/deletevar tempRoute
			/if (${Defined[contRecording]}) /deletevar contRecording
			/doevents flush
		}
	}

/if (${Debug}) /echo <== event_save_Recording -|
/RETURN STOP_RECORDING



|------------------------------------------------|
|- Echos available routes in your current zone.	-|
|------------------------------------------------|
|- Use, /routes or /bc List routes				-|
|------------------------------------------------|
#event list_Routes "[MQ2] List Routes"
SUB event_list_Routes
/if (${Debug}) /echo |- event_list_Routes ==>

|-------------------------------------------------------------------------Call setup if it hasn't been done already.	
	/if (!${SmoothMoves}) /call SmoothMoves_Setup

|---------------------------------------------------------------------------------------------------------If I don't have any routes in this zone.
	/if (!${Ini[${SmoothMoves_Ini},${Zone.ShortName}].Length}) {
		/echo You don't have any saved routes in ${Zone}.
		
|------------------------------------------------------------------------------------------------Else, if there are routes available in this zone.
	} else {
		/echo Routes available in ${Zone},
		/declare count int local 1
		
	|------------------------------------------------------------------Count available routes, and echo their names.
		:count_Routes
		/if (${Ini[${SmoothMoves_Ini},${Zone.ShortName}].Replace[||,].Arg[${count},|].Length}) {
			/echo [${count}] ${Ini[${SmoothMoves_Ini},${Zone.ShortName}].Replace[||,].Arg[${count},|]}
			/varcalc count ${count} + 1
			/goto :count_Routes
		}
		/echo You may select routes by name or number.
	}
	
/if (${Debug}) /echo <== event_list_Routes -|
/RETURN



|--------------------------------------------|
|- Stop running a route.					-|
|--------------------------------------------|
|- Use /echo Stop route, or /bc Stop route	-|
|--------------------------------------------|
#event stop_Route "[MQ2] Stop route"
#event stop_Route "[MQ2] Stop running"
#event stop_Route "[MQ2] End route"
#event stop_Route "<#*#> Stop route"
#event stop_Route "<#*#> Stop running"
#event stop_Route "<#*#> End route"
SUB event_stop_Route
/if (${Debug}) /echo |- event_stop_Route ==>

	/if (${Defined[route_From_Ini]}) {
		/echo Stopping...
		/declare stop_Route bool outer TRUE
	}
	
/if (${Debug}) /echo <== event_stop_Route -|
/RETURN



|----------------------------------------------------|
|- Runs indicated routes from Routes_Ini.			-|
|----------------------------------------------------|
|- @param line: '[MQ2] Run Route [Name]'			-|
|- @param route_Name: Name of the route to run.		-|
|- @param ChatSender: Name of the command sender.	-|
|----------------------------------------------------|
#event run_Route "[MQ2] Run Route #1#"
#event run_Route "<#2#> Run Route #1#"
SUB event_run_Route(line, route_Name, ChatSender)
/if (${Debug}) /echo |- run_Route ==>

|-----------------------------------------------------------------------------------------------Clean-up route name.
	/if (!${SmoothMoves}) /call SmoothMoves_Setup

|-----------------------------------------If the ChatSender is in zone, or, if the command was issued through /echo.
	/if (${SpawnCount[pc ${ChatSender}]} || !${Defined[ChatSender]}) {

	|--------------------------------------------Declare route variables.
		/declare i int local
		/declare startZone int local
		/declare closest_Point_wLOS int local
		/declare total_WPs int local
		/declare dist_Calc float local
		/declare route_From_Ini string outer
		/declare startpointFound bool local
		
	|-------------------------------------------If route was specified by number, find the route associated with it.
		/if (${Int[${route_Name}]}) {
			/if (${Ini[${SmoothMoves_Ini},${Zone.ShortName}].Replace[||,].Arg[${route_Name},|].Length}) {
				/varset route_Name ${Ini[${SmoothMoves_Ini},${Zone.ShortName}].Replace[||,].Arg[${route_Name},|]}
			} else {
			
			|-----------------------------------------------Else, if there was no route associated with this number.
				/if (!${no_Echo}) /echo [${route_Name}] not a known route, use '/echo List routes' to review available routes in ${Zone}.
			}
		}
		
	|----------------------------------------------------------------------------------------------------If the route was not found in Routes_Ini.
		:reload_FromZone
		/if (${Debug}) /echo |- event_record_Route -| :reload_FromZone
		
		/if (!${Ini[${SmoothMoves_Ini},${Zone.ShortName},${route_Name}].Length}) {
			/if (!${no_Echo}) /echo [${route_Name}] not a known route. Use '/echo List routes' to review available routes in ${Zone}.
		} else {
		
		|----------------------------------------------------------------------------------------------------If the route was found in the Routes_Ini.
			/echo Recalling route [${route_Name}]...
			/varset route_From_Ini ${Ini[${SmoothMoves_Ini},${Zone.ShortName},${route_Name}]}
			
		|--------------------------------------------------------------------------------------------Turn off stick.
			/if (${Stick.Active}) /squelch /stick off
			/if (${NetAdvPath.State}) /squelch /netfollow off
			
		|---------------------------------------------------------------------------------------Set route variables.
			/varset i 1
			/varset startZone ${Zone.ID}
			/varset closest_Point_wLOS -1
			/varset dist_Calc 99999
			/varset startpointFound FALSE
			
		|------------------------------------------------------------------------Loop through the route, to find the closest waypoint to start on.
			
			:check_next_Waypoint
			/if (${Debug}) /echo |- event_record_Route -| :check_next_Waypoint
			
			/if (${route_From_Ini.Arg[${i},|].Length}) {
				/if (${Debug}) /echo Checking WP:[${i} - ${route_From_Ini.Arg[${i},|]}] :: LoS:[${LineOfSight[${Me.Y},${Me.X}:${route_From_Ini.Arg[${i},|].Arg[1,_].Arg[1,,]},${route_From_Ini.Arg[${i},|].Arg[1,_].Arg[2,,]}]}] :: Special Function:[${Bool[${route_From_Ini.Arg[${i},|].Find[**]}]}]
	
			|-----------------------------------------------------Check line of sight, and distance of the waypoint.
				/if (${LineOfSight[${Me.Y},${Me.X}:${route_From_Ini.Arg[${i},|].Arg[1,_].Arg[1,,]},${route_From_Ini.Arg[${i},|].Arg[1,_].Arg[2,,]}]}) {
					
					/if (!${startpointFound}) /varset startpointFound TRUE
					
					/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${route_From_Ini.Arg[${i},|].Arg[1,_].Arg[1,,]},${route_From_Ini.Arg[${i},|].Arg[1,_].Arg[2,,]}
					
					/if (${Debug}) /echo Distance = ${Macro.Return}
				
				|------------------If this waypoint is closer than the closest known waypoint, set the new distance.
					/if (${Macro.Return}<${dist_Calc}) {
						/varset dist_Calc ${Macro.Return}
						/varset closest_Point_wLOS ${i}
					}
				}
				/varcalc i ${i} + 1
				/goto :check_next_Waypoint
			}
			
		|-------------------------------------------------------------------------------Varcalc total_WPs to (i - 1) to sync /for loop.
			/varcalc total_WPs ${i} - 1
			
		|--------------------------------------------------------------------------------------------Check for a starting point.
			/if (!${startpointFound}) {
				/docommand ${ChatToggle} I could not find a starting point for [${route_Name}].
			} else {
			
			|----------------------------------------------------------------------------------------For the closest waypoint to the end of the route.
				/for i ${closest_Point_wLOS} to ${total_WPs}
					
				|-----------------------------------------------------------------------------------If this element is a waypoint, go to it.
					/if (!${route_From_Ini.Arg[${i},|].Find[**]}) {
		
						/call next_WP ${route_From_Ini.Arg[${i},|].Arg[1,_].Arg[1,,]} ${route_From_Ini.Arg[${i},|].Arg[1,_].Arg[2,,]}
					
				|--------------------------------------------------------------------------------If this element is a specialized function, call the corresponding SUB.
					} else {
					|														Special functions														|
					
					|----------------------------------------------------------------------------------------Click Zone.
						/if (${route_From_Ini.Arg[${i},|].Find[clickZone]}) {
							/echo Attempting to click zone...
							/call click_Zone ${route_From_Ini.Arg[${i},|].Arg[1,_]} ${startZone}
							
					
					|----------------------------------------------------------------------------------Run Through Zone.
						} else /if (${route_From_Ini.Arg[${i},|].Find[runThruZone]}) {
							/echo Attempting to run through zone...
							/call Run_Thru_Zone ${route_From_Ini.Arg[${i},|].Arg[1,_]} ${route_From_Ini.Arg[${i},|].Arg[3,_]} ${route_From_Ini.Arg[${i},|].Arg[4,_]} ${startZone}
					
					
					|----------------------------------------------------------------------------------------Click Door.
						} else /if (${route_From_Ini.Arg[${i},|].Find[clickDoor]}) {
							/echo Attempting to click door...
							/call click_Door ${route_From_Ini.Arg[${i},|].Arg[1,_]} ${route_From_Ini.Arg[${i},|].Arg[3,_]} ${route_From_Ini.Arg[${i},|].Arg[4,_]}
				
				
					|---------------------------------------------------------------------------------------Bark at NPC.
						} else /if (${route_From_Ini.Arg[${i},|].Find[barkAtMob]}) {
							/echo Barking at [${Spawn[npc ${If[${route_From_Ini.Arg[${i},|].Arg[3,_].Find[-]},${route_From_Ini.Arg[${i},|].Arg[3,_].Replace[-,_]},${route_From_Ini.Arg[${i},|].Arg[3,_]}]}].CleanName}]...
							/call barkAtNPC "${route_From_Ini.Arg[${i},|]}" ${startZone}
						}
						
					|													End of special functions													|
					}
			
				/call Movement_Events

			|------------------------------------------------------------If a stop was called, end this fuction.
				/if (${stop_Route}) {
					/deletevar stop_Route
					/goto :command_Stop
				}	
				
			|------------------------------------------If I have zoned before the route was finished, check to see if I should continue the route.
				/if (${Zone.ID} != ${startZone}) {
					/if (!${Defined[no_Echo]}) /declare no_Echo bool local TRUE
					/goto :reload_FromZone
				}
				
			|---------------------------------------------------------------------------------------------------------Go to next element of the route.
				/next i
				
			|------------------------------------------------------------------------------------------------------------------------Announce to user.
				/if (${Verbosity}) /docommand ${ChatToggle} I have arrived at my destination.
			}
		}
	}
	:command_Stop
	/if (${Defined[route_From_Ini]}) /deletevar route_From_Ini
	
/if (${Debug}) /echo <== run_Route -|
/RETURN



|--------------------------------------------------------|
|- Runs to the next waypoint.							-|
|--------------------------------------------------------|
|- @param y_loc: Y intercept of your next destination.	-|
|- @param x_loc: X intercept of your next destination.	-|
|--------------------------------------------------------|
SUB next_WP(float y_loc, float x_loc)
/if (${Debug}) {
	/echo |- next_WP ==>
	/echo Moving to next location ==> ${y_loc},${x_loc}
}

|-------------------------------------------------------------------------------------------Engage MQ2MoveUtils to move to the specified location.
	/squelch /moveto Loc ${y_loc} ${x_loc} dist 7
	/delay 10 ${Me.Moving}
	
|-----------------------------------------------------------------------------------------------Wait to stop moving.
	:wait4Stop
	/if (${Debug}) /echo |- next_WP -| :wait4Stop
	
|------------------------------------------------------------------------------Check if movement events were called.
	/call Movement_Events
	
|------------------------------------------If a 'stop route' was called, stop moving, and return to event_run_Route.
	/if (${stop_Route}) {
		/moveto off
		/goto :skip_wait4Stop
	}	

|-------------Calculate distance to my destination.  If I'm closer than 5 units, start moving to the next waypoint.
	/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${y_loc},${x_loc}	
	/if (${Macro.Return}<15) {
		| /echo skipping to next wp
		/goto :skip_wait4Stop
	}
	
|------------------------------------------------------------------------------------------Look up, if I'm in water.
	/if (${Me.Underwater} && ${Me.Look} < 5) /look 5	

|---------------------------------------------------------------------------------------------------------------If I'm still moving, wait to stop.
	/if (${Me.Moving}) /goto :wait4Stop	
	
| ADD STUCK LOGIC HERE

	:skip_wait4Stop

/if (${Debug}) /echo <== next_WP -|
/RETURN



|----------------------------------------------------------------------------|
|- Clicks doors on a recorded route.										-|
|----------------------------------------------------------------------------|
|- @param locToStart: Where to stand when trying to click the door.			-|
|- @param snapHeading: The direction to face when trying to click the door.	-|
|- @param snapLook: The angle to look when trying to click the door.		-|
|----------------------------------------------------------------------------|
SUB click_Door(locToStart, float snapHeading, float snapLook)
/if (${Debug}) /echo |- click_Door ==>
	
	/declare retry_Position int local 0
	:retry_Position
	/if (${Debug}) /echo |- click_Door -| :retry_Position
	
|----------------------------------------------------------------------------------------------Engage MQ2MoveUtils to move to the door's location.
	/squelch /moveto loc ${locToStart.Arg[1,,]} ${locToStart.Arg[2,,]} dist 1
	/delay 2s ${Me.Moving}
	
|----------------------------------------------------------------------------------Wait until I have stopped moving.	
	:wait4Stop
	/if (${Debug}) /echo |- click_Door -| :wait4Stop
	/call Movement_Events 
	/if (${Me.Moving}) /goto :wait4Stop

|------------------------------------------------------------------Check distance from the door's location, if I'm too far, try to position again.
	/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${locToStart}
	/if (${Macro.Return}>10 && ${retry_Position}<6) {
		/varcalc retry_Position ${retry_Position} + 1
		/goto :retry_Position
	}
	
|----------------------------------------------------------------------------------------------------------Face the door, and attempt to click it.
	/face fast heading ${Math.Calc[${snapHeading}*-1]}
	/look ${snapLook}
	/delay 1s
	/keypress U
	/delay 2s
	
/if (${Debug}) /echo <== click_Door -|
/RETURN



|--------------------------------------------------------------------|
|- Clicks zones on a recorded route.								-|
|--------------------------------------------------------------------|
|- @param locToStart: Where to stand when trying to click the door.	-|
|- @param startZone: Zone you are attempting to leave.				-|
|--------------------------------------------------------------------|
SUB click_Zone(locToStart, int startZone)
/if (${Debug}) /echo |- click_Zone ==>

|--------------------------------------------------------------------------------------------------------------------Declare zone click variables.
	/declare tryToZone_Timer timer local ${tryToZone_Timer_Setting}
	/declare keypress_Timer timer local
	/declare i int local
	
|--------------------------------Switch to first person camera.
	/keypress First_Person_Camera
	:retry
	/if (${Debug}) /echo |- click_Zone -| :retry_Position
	
|---------------------------------------------------------------------------------------------------------------------If I'm in the starting zone.
	/if (${Zone.ID}==${startZone}) {
	
	|----------------------Check distance from object I want to click, if I'm too far away, move closer.
		/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${locToStart}
		/if (${Macro.Return} > 8) {
			/squelch /moveto loc ${locToStart.Arg[1,,]} ${locToStart.Arg[2,,]} dist 8
			/delay 1s ${Me.Moving}
			/delay 5s !${Me.Moving}
		}
		
	|---------------------------------------------------------------------------------------------Face door and attempt to click it for 5 seconds.
		/varset keypress_Timer 5s
		/squelch /doortarget
		/squelch /face fast door
		/squelch /look -45
		:keypress
		/if (${Debug}) /echo |- click_Zone -| :keypress
		
	|---------------------------Shifts look angle up and down as I click.
		/look ${If[${Me.Look} < 45,${Math.Calc[${Me.Look} + 5]},-45]}
		/keypress U
		/delay 2
		/if (${keypress_Timer} && ${Zone.ID} && ${Zone.ID} == ${startZone}) /goto :keypress
		
	|-------------------------------------------------------------------------------------If I'm still in the startZone, move back, and try again.
		/if (${Zone.ID} && ${Zone.ID} == ${startZone} && ${tryToZone_Timer}) {
			/squelch /target clear
			/keypress back hold
			/delay 5
			/keypress forward
			/delay ${Math.Rand[50]}
			/goto :retry
			
	|-----------------------------------------------------------------------------------------------Else, if I'm zoning, wait until I've finished.
		} else /if (!${Zone.ID} || ${Zone.ID} != ${startZone}) {
			/delay 2m ${SpawnCount[pc ${Me.CleanName}]} && ${Me.PctHPs}
		}
		/squelch /stick off
	}

/if (${Debug}) /echo <== click_Zone -|
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
	/if (${Debug}) /echo |- run_Thru_Zone -| :retry_Position
	
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



|--------------------------------------------------------------------|
|- Barks at an NPC on a recorded route.								-|
|--------------------------------------------------------------------|
|- @param barkInfo: A string containing the bark target, and msg.	-|
|--------------------------------------------------------------------|
SUB barkAtNPC(barkInfo, int startZone)
/if (${Debug}) /echo |- barkAtNPC ==>
	
	/declare retryTimer timer local 300
	/declare barkLoc string local ${barkInfo.Arg[1,_]}
	/declare barkTarget string local ${barkInfo.Arg[3,_].Replace[-,_]}
	/declare barkMsg string local ${barkInfo.Arg[4,_].Replace[^, ]}
	/if (${barkInfo.Arg[5,_].Equal[zone]}) /declare zoneAfterBark bool local TRUE
	
	:retry
	/if (${Debug}) /echo |- barkAtNPC -| BarkLoc = ${barkLoc} - barkTarget = ${barkTarget} - barkMsg = ${barkMsg} - zoneAfterBark = ${zoneAfterBark} - startZone=${startZone}
	
	/call next_WP ${barkInfo.Arg[1,_].Arg[1,,]} ${barkInfo.Arg[1,_].Arg[2,,]}
	
	/if (!${SpawnCount[npc ${barkTarget}]}) {
		/echo I don't see [${barkTarget}] in zone, ending route.
	} else {
		/call event_bark PLACEHOLDER "${barkTarget}" "${barkMsg}" ${Me.CleanName}
		/delay 30
	}
	
	/if (${zoneAfterBark} && ${Zone.ID} == ${startZone}) {
		/if (${retryTimer}) {
			/goto :retry
		} else {
			/echo I failed to zone after barking at [${Spawn[npc ${barkTarget}].CleanName}].  Ending route.
			/declare stop_Route bool outer TRUE
		}
	}
	
/if (${Debug}) /echo <== barkAtNPC -|
/RETURN