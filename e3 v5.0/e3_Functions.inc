|--------------------------------------------------------------------------------|
|- e3_Macro_AdvFunctions.inc v1.0 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ													-|
|--------------------------------------------------------------------------------|
|- Contains functional scripts used in the setup, and maintenance of e3.mac.	-|
|--------------------------------------------------------------------------------|



|--------------------------------------------------------------------------------------------------------|
|- Validates Ini entries from supplied Ini keys, and creates variables of the specified type and scope.	-|
|--------------------------------------------------------------------------------------------------------|
|- Returns validated Ini entries if scope is local, or creates variables of specified type and scope.	-|
|- Use * as the first character of any ini entry to invalidate that entry 								-|
|- Use the bool Debug to report back debug information 													-|
|--------------------------------------------------------------------------------------------------------|
|- @param Ini_Key: The ini file, section, and entry to create a variable from.							-|
|- @param VarToMake: The name of the variable you want to declare.										-|
|- @param VarType: The type of variable to be declared.													-|
|- @param VarScope: The scope of the variable to be declared.											-|
|--------------------------------------------------------------------------------------------------------|
|- Syntax: /call IniToVar IniKey VariableToMake VariableType VariableScope								-|
|-	Ex:																									-|
|--------------------------------------------------------------------------------------------------------|
|-		To create an outer variable,  																	-|
|-			/call IniToVar Bot_Killians.ini,AssistStick,StickDistance int outer							-|
|--------------------------------------------------------------------------------------------------------|
|-		To pass a local variable, 																		-|
|-			/call IniToVar Bot_Killians.Ini,Basics,Use_Potion/Pct UNDEFINED string local					-|
|-			/declare PotionName ${Macro.Return.Arg[1,/]}													-|
|-			/declare PotionPct ${Macro.Return.Arg[2,/]}													-|
|--------------------------------------------------------------------------------------------------------|
SUB IniToVar(Ini_Key, VarToMake, VarType, VarScope)
/if (${Debug}) {
	/echo |- IniToVar ==>
	/echo |- IniToVar -| Checking for Ini_Key: [${Ini_Key}]
}

|-----------------------------------------------------------------------------------------------------------------Check for the specified ini key.
	/if (!${Ini[${Ini_Key}].Length}) {
		/if (${Debug}) /echo |- IniToVar -| ERROR: Ini_Key [${Ini_Key}] was not found.
	} else {
	
	|---------------------------------------------If the entry has been invalidated.
		/if (${Ini[${Ini_Key}].Left[1].Equal[*]}) {
			/if (${Debug}) /echo |- IniToVar -| ERROR: Ini_Key [${Ini_Key}] has been invalidated.
		} else {
	|------------------------------------------------------------------------------------------------If the ini key was found, validate the entry.
		
		|--------------------------------------------------------------------------------------------Validate bool.
			/if (${VarType.Equal[bool]}) {
				/if (!${Select[${Ini[${Ini_Key}]},TRUE,ON,YES,1]} && !${Select[${Ini[${Ini_Key}]},FALSE,OFF,NO,0]}) {
					/echo |- IniToVar -|  ERROR: Ini_Key [${Ini_Key}] must be, TRUE or FALSE, YES or NO, ON or OFF, 1 or 0.  Please enter a valid setting, and reload.
				} else {
					/if (${Defined[${VarToMake}]}) /deletevar ${VarToMake}
					/declare ${VarToMake} bool ${VarScope} ${If[${Select[${Ini[${Ini_Key}]},TRUE,ON,YES,1]},TRUE,FALSE]}
					/if (${Debug}) /echo |- IniToVar -| Defined: ${VarToMake} [${${VarToMake}}]
				}
				
		|---------------------------------------------------------------------------------------------Validate int.
			} else /if (${VarType.Equal[Int]}) {
				/if (!${Int[${Ini[${Ini_Key}]}]}) {
					/echo |- IniToVar -| ERROR: Ini_Key [${Ini_Key}] must be an integer.  Please enter a valid whole number, and reload.
				} else {
					/if (${Defined[${VarToMake}]}) /deletevar ${VarToMake}
					/declare ${VarToMake} int ${VarScope} ${Ini[${Ini_Key}]}
					/if (${Debug}) /echo |- IniToVar -| Defined: ${VarToMake} [${${VarToMake}}]
				}
		
		|-------------------------------------------------------------------------------------------Validate float.
			} else /if (${VarType.Equal[Float]}) {
				/if (!${Float[${Ini[${Ini_Key}]}]}) {
					/echo |- IniToVar -| ERROR: Ini_Key [${Ini_Key}] must be a real number.  Please enter a valid number, and reload.
				} else {
					/if (${Defined[${VarToMake}]}) /deletevar ${VarToMake}
					/declare ${VarToMake} float ${VarScope} ${Ini[${Ini_Key}]}
					/if (${Debug}) /echo |- IniToVar -| Defined: ${VarToMake} [${${VarToMake}}]
				}
				
		|-----------------------------------------------------------------------------------Create string or timer.
			} else {
				/if (${Defined[${VarToMake}]}) /deletevar ${VarToMake}
				/declare ${VarToMake} ${VarType} ${VarScope} ${Ini[${Ini_Key}]}
				/if (${Debug}) /echo |- IniToVar -| Defined: ${VarToMake} [${${VarToMake}}]
			}
			
			/if (${Defined[Validation_Count]}) {
				| /if (${Debug}) /echo |- IniToVar -| Validation_Count: [${Validation_Count} + ${Ini[${Ini_Key}].Length} = ${Math.Calc[${Validation_Count} + ${Ini[${Ini_Key}].Length}].Int}]
				/varcalc Validation_Count ${Validation_Count} + ${Ini[${Ini_Key}].Length}
			}
		}
	}
	
/if (${Debug}) /echo <== IniToVar -|
/RETURN ${${VarToMake}}



|--------------------------------------------------------------------------------------------|
|- Validates Ini entries from supplied Ini keys, and creates arrays of the specified scope.	-|
|--------------------------------------------------------------------------------------------|
|- Use * as the first character of any ini entry to invalidate that entry .					-|
|- Use the bool Debug to report back debug information.										-|
|--------------------------------------------------------------------------------------------|
|- @param Ini_Key: The ini file, section, and entry to create an array from.				-|
|- @param ArrayName: The name of the array you want to declare.								-|
|- @param ArrayScope: The scope of the array to be declared.								-|
|--------------------------------------------------------------------------------------------|
|- Syntax: /call IniToArray IniKey ArrayName VariableScope									-|
|-	Ex:																						-|
|--------------------------------------------------------------------------------------------|
|-		To create an outer array,  															-|
|-			/call IniToVar "Bot_Killians.ini,Buffs,SelfBuffs" MySelfBuffs outer				-|
|--------------------------------------------------------------------------------------------|
|-		To pass a local array, 																-|
|-			/call IniToVar Bot_Killians.ini,Buffs,SelfBuffs MySelfBuffs local				-|
|--------------------------------------------------------------------------------------------|
SUB IniToArray(Ini_Key, ArrayName, ArrayScope)
/if (${Debug}) {
	/echo |- IniToArray ==>
	/echo |- IniToArray -| Checking for Ini_Key: [${Ini_Key}]
}

|-----------------------------------------------------------------------------------------------------------------Check for the specified ini key.
	/if (!${Ini[${Ini_Key}1].Length}) {
		/if (${Debug}) /echo |- IniToVar -| ERROR: Ini_Key [${Ini_Key}] was not found.
	} else {
	
	|---------------------------------------------If the Ini_Key has been invalidated.
		/if (${Ini[${Ini_Key}1].Left[1].Equal[*]}) {
			/if (${Debug}) /echo |- IniToVar -| ERROR: Ini_Key [${Ini_Key}] has been invalidated.
		} else {
		
	|----------------------------------------------------------------------------------------------------If the Ini_Key is valid, create an array.
			
		|--------------------------------Count ini entries.
			/declare i int local 1
			/declare count int local
			:CountingLoop
			/if (${Ini[${Ini_Key}${i}].Length}) {
				/varset count ${i}
				/varcalc i ${i} + 1
				/goto :CountingLoop
			}
			
		|----------------------------------------------------------------------------------------Declare the array.
			/if (${Defined[${ArrayName}]}) /deletevar ${ArrayName}
			/declare ${ArrayName}[${count}] string ${If[${Defined[ArrayScope]},${ArrayScope},outer]}
			
		|-------------------------------------------------------------------------Import ini entries into the array.
			/for i 1 to ${${ArrayName}.Size}
				/varset ${ArrayName}[${i}] ${Ini[${Ini_Key}${i}]}
				/if (${Defined[Validation_Count]}) {
					| /if (${Debug}) /echo |- IniToVar -| Validation_Count: [${Validation_Count} + ${${ArrayName}[${i}].Length} = ${Math.Calc[${Validation_Count} + ${${ArrayName}[${i}].Length}].Int}]
					/varcalc Validation_Count ${Validation_Count} + ${${ArrayName}[${i}].Length}
				}
			/next i
		}
	}
	
/if (${Debug}) {

|---------------------------------------------------------------------------------------------------------------If debugging, echo array elements.
	/if (${${ArrayName}.Size}) {
		/echo |- Ini --> Array -| Created: ${ArrayName}[${${ArrayName}.Size}]
		/for i 1 to ${${ArrayName}.Size}
			/echo |- Ini --> Array -| ${ArrayName}[${i}] - ${${ArrayName}[${i}]}
		/next i
	} else {
		/echo |- Ini --> Array -| ERROR: Failed to create [${ArrayName}]
	}
	/echo <== IniToArray -|
}
/RETURN ${ArrayName}



|----------------------------------------------------------------------------|
|- Creates entries, and writes settings to specified inis.					-|
|----------------------------------------------------------------------------|
|- Will not over-write existing entries, unless indicated.					-|
|----------------------------------------------------------------------------|
|- @param Ini_Key: The ini file, section, and entry to write to.			-|
|- @param WhatToWrite: Data to write to the given Ini_Key.					-|
|- @param OverWrite: Allow overwrite of existing settings?					-|
|----------------------------------------------------------------------------|
|- Syntax: /call WriteToIni "Ini_Key" "WhatToWrite" OverWrite(TRUE/FALSE)	-|
|-	Ex:																		-|
|----------------------------------------------------------------------------|
|-		To create an ini entry,												-|
|-			/call WriteToIni "Loot.ini,B,Bone Chips"							-|
|----------------------------------------------------------------------------|
|-		To create an ini entry with a setting,								-|
|-			/call WriteToIni "Loot.ini,B,Bone Chips" "Keep"					-|
|----------------------------------------------------------------------------|
|-		To over-write an existing ini setting,								-|
|-			/call WriteToIni "Loot.ini,B,Bone Chips" "Destroy" TRUE			-|
|----------------------------------------------------------------------------|
SUB WriteToIni(Ini_Key, WhatToWrite, bool OverWrite)
/if (${Debug}) {
	/echo |- WriteToIni ==>
	/echo |- WriteToIni -| Ini_Key = [${Ini_Key}] - Data to write = [${WhatToWrite}] - OverWrite = [${OverWrite}]	
}

|--------------------------------------------------------------------------------------------------------If the Ini_Key does not exist, create it.	
	/if (!${Ini[${Ini_Key}].Length}) {
	
	|---------------------------------------------------------------------If there is something to write, write it.
		:OverWrite
		/if (${Defined[WhatToWrite]}) {
			/ini "${Ini_Key.Arg[1,,]}" "${Ini_Key.Arg[2,,]}" "${Ini_Key.Arg[3,,]}" "${WhatToWrite}"
		
	|----------------------------------------------------------------------------------Else, create the entry only.
		} else {
			/ini "${Ini_Key.Arg[1,,]}" "${Ini_Key.Arg[2,,]}" "${Ini_Key.Arg[3,,]}"
		}
		
|-----------------------------------------------------------Else, if the Ini_Key does exist, and OverWrite is true, write to the existing Ini_Key.
	} else /if (${OverWrite}) {
		/goto :OverWrite
	}

/if (${Debug}) /echo <== WriteToIni -|
/RETURN



|--------------------------------------------------------|
|- Creates new arrays, or stacks onto existing arrays.	-|
|--------------------------------------------------------|
|- Syntax: /call BuildArray ArrayName Data				-|
|-  Ex: /call BuildArray DebuffTargets ${Target.ID}		-|
|--------------------------------------------------------|
|- @param ArrayName: Name of the array to build.		-|
|- @param Data: Data you wish to add to the array.		-|
|--------------------------------------------------------|
SUB BuildArray(ArrayName, Data)
/if (${Debug}) /echo |- BuildArray ==>

|---------------------------------------------------------------------------------------------------------If the array is not defined, declare it.
	/if (!${Defined[${ArrayName}]}) {
		/declare ${ArrayName}[1] string outer
		/varset ${ArrayName}[1] ${Data}
		
|-----------------------------------------------------------------------------------------------------------Else, if the array is already defined.
	} else {
	
	|--------------------------------------------------------------------------Create a copy of the existing array.
		/declare ${ArrayName}Copy[${${ArrayName}.Size}] string local
		/declare i int local
		/for i 1 to ${${ArrayName}.Size}
			/varset ${ArrayName}Copy[${i}] ${${ArrayName}[${i}]}
		/next i
		
	|------------------------------------------------------------------------------------Delete the existing array.
		/deletevar ${ArrayName}
		
	|----------------------------------------------------------------------------------------Build a resized array.
		/declare ${ArrayName}[${Math.Calc[${${ArrayName}Copy.Size}+1].Int}] string outer
		/for i 1 to ${${ArrayName}.Size}
			/varset ${ArrayName}[${i}] ${${ArrayName}Copy[${i}]}
		/next i
	
	|-------------------------------------------------------------------------------Insert new data into the array.
		/varset ${ArrayName}[${${ArrayName}.Size}] ${Data}
	}

/if (${Debug}) /echo <== BuildArray -|
/RETURN ${ArrayName}



|----------------------------------------------------|
|- Removes elements from an existing array.			-|
|----------------------------------------------------|
|- @param ArrayName: Name of the array to search.	-|
|- @param Data: Data you wish to remove.			-|
|----------------------------------------------------|
SUB RemoveArrayElement(ArrayName, Data)
/if (${Debug}) {
	/echo |- RemoveArrayElement ==>
	/echo |- RemoveArrayElement -| Removing [${Data}] from [${ArrayName}]
}

|-------------------------------------------------------------------------------------------------------------Create a copy of the existing array.
	/declare ${ArrayName}Copy[${${ArrayName}.Size}] string local
	/declare i int local
	/for i 1 to ${${ArrayName}.Size}
		/varset ${ArrayName}Copy[${i}] ${${ArrayName}[${i}]}
	/next i
	
|----------------------------------------------------------------------------------------Delete the existing array.
	/deletevar ${ArrayName}
	
|---------------------------------------------------------------------------------------------------Count array elements that do not match 'Data'.
	/declare x int local
	/for i 1 to ${${ArrayName}Copy.Size}	
		/if (${${ArrayName}Copy[${i}].NotEqual[${Data}]}) {
			/varcalc x ${x} + 1
		}
	/next i
	
|-------------------------------------------------------------------------------If there are elements that do not match 'Data', rebuild the array.
	/if (${x}) {
	|--------------------------------------------------------------------------------------Declare a resized array.
		/if (${Debug}) /echo |- RemoveArrayElement -| Rebuilding [${ArrayName}]
		/declare ${ArrayName}[${x}] string outer
		
	|------------------------------------------------------Copy the old array, skipping elements that match 'Data'.
		/varset x 1
		/for i 1 to ${${ArrayName}Copy.Size}
			/if (${${ArrayName}Copy[${i}].NotEqual[${Data}]}) {
				/if (${Debug}) /echo |- RemoveArrayElement -| Rebuilding [${ArrayName}[${x}]] [${${ArrayName}Copy[${i}]}]
				/varset ${ArrayName}[${x}] ${${ArrayName}Copy[${i}]}
				/varcalc x ${x} + 1
			}
		/next i
	}
	
/if (${Debug}) /echo <== RemoveArrayElement -|
/RETURN



|------------------------------------------------------------|
|- Reliably targets specified Target.IDs.					-|
|------------------------------------------------------------|
|- @param TargetID: The ID of the spawn you wish to target.	-|
|------------------------------------------------------------|
SUB TrueTarget(TargetID)
/if (${Debug}) /echo |- TrueTarget${If[${TargetID}, [${TargetID}],]} ==>

|------------------------------------------------------------------------------------------------------------------Check if the target is in zone.
	/if (${SpawnCount[id ${TargetID}]}) {
		
		/declare retryTimer timer local 30
		:Target_Loop
		/if (${Debug}) /echo |- TrueTarget -| Attempting to aquire Target [${TargetID} -- ${Spawn[id ${TargetID}].CleanName}]
		/squelch /target id ${TargetID}
		/delay 2 ${Target.ID} == ${TargetID}
		
		/if (${Target.ID} != ${TargetID}) {
			/if (${retryTimer}) {
				/goto :Target_Loop
			} else {
				/echo ERROR: Failed to target [${Spawn[id ${TargetID}].CleanName} - ${TargetID}]
			}
		}
	}

/if (${Debug}) /echo <== TrueTarget -|
/RETURN



|------------------------------------------------------------------------------------|
|- Creates timers, at the specified duration, and adds their names to timerArray.	-|
|------------------------------------------------------------------------------------|
|- @param timerName: Name of the timer you want to declare.							-|
|- @param timerSetting: Setting of the timer you're declaring.						-|
|------------------------------------------------------------------------------------|
SUB createTimer(timerName, timerSetting)
/if (${Debug}) /echo |- createTimer ==>

|-------------------------------------------------------------------------------------------------Delete old timer.
	/if (${Defined[${timerName}]}) /deletevar ${timerName}

|-------------------------------------------------------------------------------------------------------------------------------Declare new timer.
	/declare ${timerName} timer global ${timerSetting}
	
|---------------------------------------------------------------------------Add the timer's name to the timerArray.
	/call BuildArray timerArray ${timerName}

/if (${Debug}) {
	/echo created ${timerName} [${${timerName}}]
	/echo <== createTimer -|
}
/RETURN



|----------------------------------------------------|
|- Deletes spent timers made by 'SUB createTimer'.	-|
|----------------------------------------------------|
SUB check_macroTimers
/if (${Debug}) /echo |- check_macroTimers ==>

	/declare i int local
	/declare SpentTimerCounter int local

|-----------------------------------------------------------------------------------------Check each entry of the timer array, count spent timers.	
	/for i 1 to ${timerArray.Size}
		/if (!${${timerArray[${i}]}}) {
			/varcalc SpentTimerCounter ${SpentTimerCounter} + 1
			/if (${Defined[${timerArray[${i}]}]}) {
				/if (${Debug}) /echo Deleting timer: ${timerArray[${i}]}
				/deletevar ${timerArray[${i}]}
			}
		}
	/next i
	
|-----------------------------------------------------------------------------If all the timers listed on timerArray are spent, delete timerArray.
	/if (${SpentTimerCounter}==${timerArray.Size}) {
		/if (${Debug}) /echo All timers have been cleared.
		/deletevar timerArray
	}
	
/if (${Debug}) /echo <== check_macroTimers -|
/RETURN



|----------------------------------------------------------------------------------------|
|- Auto inventories items from your cursor. 											-|
|----------------------------------------------------------------------------------------|
|- Alerts the user, and ends the macro, if there is no free inventory space available.	-|
|----------------------------------------------------------------------------------------|
SUB ClearCursor
/if (${Debug}) /echo |- ClearCursor ==>

	/declare i int local 1
	/declare dumpSlot string local
	
|----------------------------------------------------If I have an item on my cursor, check what type of item it is.
	:auto_inv

|------------------------------------------------------------------------------------------------------------------If the item is not a container.
	/if (!${Cursor.Container}) {
	
	|-------------------------------------------------------If I have free inventory space, autoinventory the item.
		/if (${Me.FreeInventory}) {
			/autoinventory
			/delay 1
			
	|----------------------------------Else, if I'm out of free inventory space, alert the user, and end the macro.
		} else {
			/docommand ${ChatToggle} I have run out of inventory space and I have an item on my cursor!!!
			/for i 1 to 5
				/beep
			/next i
			/endmacro
		}
		
|----------------------------------------------------------------------------------------------------------------------If the item is a container.
	} else {
	
	|-----------------------------------------------------------------------------------Check for a free pack slot.
		/for i 1 to 8
		/if (${InvSlot[pack${i}].Item.Container}) {
			/next i
			
		|--------------------------------------------If I have no free pack slots, alert the user, and end the macro.
			/if (${InvSlot[pack8].Item.Container}) {
				/docommand ${ChatToggle} I have run out of inventory space and I have a bag on my cursor!!!
				/for i 1 to 7
					/beep
				/next i
				/endmacro
			}
			
	|----------------------------------------------------------Else, if I have a free pack slot, drop the backpack.
		} else /if (!${InvSlot[pack${i}].Item.Container}) {
			/nomodkey /itemnotify pack${i} leftmouseup
		}
	}
	
|-----------------------------------------------------------------------------------If I still have something on my cursor, try again to clear it.
	/if (${Cursor.ID}) /goto :auto_inv

/if (${Debug}) /echo <== ClearCursor -|
/RETURN



|----------------------------------------------------------------------------|
|- Triangulates distance between to points using the Pythagoras theorem.	-|
|----------------------------------------------------------------------------|
|- @param loc#1: Location 'A'												-|
|- @param loc#2: Location 'B'												-|
|----------------------------------------------------------------------------|
SUB Triangulate_Distance(string loc#1, string loc#2)
/RETURN ${Math.Sqrt[${Math.Calc[${loc#2.Arg[2,,]}-${loc#1.Arg[2,,]}]}*${Math.Calc[${loc#2.Arg[2,,]}-${loc#1.Arg[2,,]}]}+${Math.Calc[${loc#2.Arg[1,,]}-${loc#1.Arg[1,,]}]}*${Math.Calc[${loc#2.Arg[1,,]}-${loc#1.Arg[1,,]}]}]}



|----------------------------------------------------------------------------------------------------|
|- MoveTo																							-|
|----------------------------------------------------------------------------------------------------|
|- Attempts to utilize MQ2MoveUtils /moveto function, to move to a given destination.				-|
|----------------------------------------------------------------------------------------------------|
|- @param1 destination: Destinations can be provided as an ID, or loc, using the following syntax:	-|
|-																									-|
|- Move to a Spawn ID use, -SpawnID|#																-|
|-		Ex.  /call MoveTo -SpawnID|${Target.ID}														-|
|-																									-|
|- Move to a Location use, -Loc|Y,X																	-|
|-		Ex.  /call MoveTo -Loc|${Target.Y},${Target.X}												-|
|----------------------------------------------------------------------------------------------------|
SUB MoveTo(destinationInfo, int distCheck)
/varset Debug 1
/if (${Debug}) /echo |- MoveTo ==>

	/declare X_Loc float local
	/declare Y_Loc float local
	/if (!${Defined[distCheck]}) /declare distCheck int local 10

|---------------------------------------------------------Set MoveTo variables per destinationType.
	/if (${destinationInfo.Find[-SpawnID]}) {
		
		/call TrueTarget ${destinationInfo.Arg[2,|]}
		/varset Y_Loc ${Target.Y}
		/varset X_Loc ${Target.X}
		
	} else /if (${destinationInfo.Find[-Loc]}) {

		/varset Y_Loc ${destinationInfo.Arg[2,|].Arg[1,,]}
		/varset X_Loc ${destinationInfo.Arg[2,|].Arg[2,,]}

	} else {
		/echo [${destinationInfo}] does not specify a valid destinationType.  Use -SpawnID|#, or, -Loc|Y,X to note your intended destinationType.
	}

	|-------------------------------------------------------------------------Check if target is too far.
	/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${Y_Loc},${X_Loc}
	/if (${Int[${Macro.Return}]} > ${MaxResponseDist}) {
		/echo [${Spawn[id ${destination}].CleanName}] is too far away.
	} else {

	|---------------------------------------------------------------------Check if target is already close enough
		/if (${Int[${Macro.Return}]} < 10) {
			/if (${Debug}) /echo I am already at the destination.
		} else {

		| Declare a timer based on how far you have to move (1s for every 50 units + 3).
			/declare retryTimer timer local ${Math.Calc[${Macro.Return} / 50 + 3].Int}s
			
		| Engage MQ2MoveUtils.
			:retry_MoveToLoc
			/squelch /moveto loc ${Y_Loc} ${X_Loc} dist ${If[${Defined[distCheck]},${distCheck},10]}
			/delay 10 ${Me.Moving} || ${MoveTo.Moving}
			
			:movingTo_Loop
			
			| Debug Echo
			/if (${Debug} && !${spamTimer_MTLoop}) {
				/echo |- MoveToSpawn -| :MoveTo_Loop
				/call createTimer spamTimer_MTLoop 50
			}

			/call Background_Events
			/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${Y_Loc},${X_Loc}
			
			/if (${MoveTo.Moving}) {
				/if (${Macro.Return} > ${distCheck} && ${retryTimer}) {
					/goto :movingTo_Loop
				} else {
					/squelch /stick off
					/if (${Debug} && !${spamTimer_MTFAIL}) {
						/echo I have failed to move to the intended target.
						/call createTimer spamTimer_MTFAIL 30
					}
				}
			} else {

				/if (${Macro.Return} > ${Math.Calc[${distCheck} + ${Math.Calc[${distCheck} / 4].Int}].Int}) {
					/if (${retryTimer}) {
						/goto :retry_MoveToLoc
					} else {
						/if (${Debug} && !${spamTimer_MTFAIL}) {
							/echo I have failed to move to the intended target.
							/call createTimer spamTimer_MTFAIL 30
						}
					}
				} else {
					/squelch /stick off
					/if (${Debug}) /echo I have arrived at my intended destination.
				}
			}
		}
	}

/if (${Debug}) /echo <== MoveTo -|
/varset Debug 0
/RETURN



SUB argueString(WhatToFind, givenData)
/if (${Debug}) /echo |- argueString ==>

	/declare i int local 1
	/declare foundData string local NULL
	
	:nextArg
	
	/if (${givenData.Find[/]}) {
		/if (${givenData.Arg[${i},/].Find[${WhatToFind}]}) {
		
			/varset foundData ${givenData.Arg[${i},/].Arg[2,|]}
			/if (${Debug}) /echo |- argueString -| Found [${WhatToFind}] - [${foundData}]
		
		} else {
			/varcalc i ${i} + 1
			/if (${givenData.Arg[${i},/].Length}) /goto :nextArg
		}
	} else {
		/if (${givenData.Arg[${i}, ].Find[${WhatToFind}]}) {
		
			/varset foundData ${givenData.Arg[${i}, ].Arg[2,|]}
			/if (${Debug}) /echo |- get_SpellInfo -| Found [${WhatToFind}] - [${foundData}]
		
		} else {
			/varcalc i ${i} + 1
			/if (${givenData.Arg[${i}, ].Length}) /goto :nextArg
		}
	}

/if (${Debug}) /echo <== argueString -|
/RETURN ${foundData}



SUB check_selectedBots(selectedBots,ChatSender)
/if (${Debug}) /echo |- check_selectedBots ==>

	/declare botSelected bool local FALSE
	/if (${selectedBots.Equal[all]} || ${Select[${Me.CleanName},${selectedBots}]} || ${Select[${Me.Class},${selectedBots}]} || ${Select[${Me.Class.ShortName},${selectedBots}]} || ${selectedBots.Equal[Me]} && ${ChatSender.Equal[${Me.CleanName}]} || ${selectedBots.Equal[melee]} && ${Select[${Me.Class.ShortName},${MeleeClasses}]} || ${selectedBots.Equal[casters]} && ${Select[${Me.Class.ShortName},${CasterClasses}]} || ${selectedBots.Equal[ranged]} && ${Select[${Me.Class.ShortName},${RangedClasses}]} || ${selectedBots.Equal[Healers]} && ${Select[${Me.Class.ShortName},${PriestClasses}]} || ${selectedBots.Equal[Priests]} && ${Select[${Me.Class.ShortName},${PriestClasses}]}) /varset botSelected TRUE

/if (${Debug}) /echo <== check_selectedBots -|
/RETURN ${botSelected}



SUB verifyEvent(ChatSender, eventLine)
/if (${Debug}) /echo |- verifyEvent ==>

	/declare userValidated bool local FALSE
	/declare includeBot bool local TRUE
	/declare inZone bool local FALSE
	/declare inRange bool local FALSE

	| -Reset ChatSender.
	/if (${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}
	/if (${ChatSender.Equal[You]}) /varset ChatSender ${Me.CleanName}
	/if (${ChatSender.Equal[MQ2]}) /varset ChatSender ${Me.CleanName}
	
	| -Validate ChatSender.
	/if (${NetBots[${ChatSender}].ID}) /varset userValidated TRUE
	
	| -Check inZone
	/if (${SpawnCount[pc ${ChatSender}]}) /varset inZone TRUE
	
	| -Check inRange
	/if (${SpawnCount[pc ${ChatSender} radius ${MaxResponseDist}]}) /varset inRange TRUE

	| -Check for -only|
	/if (${eventLine.Find[only|]}) {
		/varset includeBot FALSE
		/call argueString -only| "${eventLine}"
		/call check_selectedBots "${Macro.Return}" "${ChatSender}"
		/if (${Macro.Return}) /varset includeBot TRUE
	}
		
	| -Check for -exclude|
	/if (${eventLine.Find[exclude|]}) {
		/call argueString -exclude| "${eventLine}"
		/call check_selectedBots "${Macro.Return}" "${ChatSender}"
		/if (${Macro.Return}) /varset includeBot FALSE
	}
	
	| -Check for -include|
	/if (${eventLine.Find[include|]}) {
		/call argueString -include| "${eventLine}"
		/call check_selectedBots "${Macro.Return}" "${ChatSender}"
		/if (${Macro.Return}) /varset includeBot TRUE
	}
	
/if (${Debug}) {
	/echo |- verifyEvent -| ChatSender=${ChatSender} || includeBot=${includeBot} || inZone=${inZone} || inRange=${inRange}
	/echo <== verifyEvent -|
}
/RETURN ${ChatSender}_${userValidated}_${includeBot}_${inZone}_${inRange}



|** e3 event skeleton

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
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
	
	
		| EVENT CODE HERE
	
	
	}
	
/if (${Debug}) /echo <== EVENT_Follow -|
/RETURN

**|



|** deprecated SCRIPTS (Keep for reference)---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


|------------------------------------------------------------------------------------------------|
|- MoveToSpawn																					-|
|------------------------------------------------------------------------------------------------|
|- Tries for up to 10 seconds to move to a given spawn ID, using MQ2MoveUtils /moveto function.	-|
|------------------------------------------------------------------------------------------------|
|- Bots must have line of sight on their MoveTo target.											-|
|------------------------------------------------------------------------------------------------|
SUB MoveToSpawn(MoveToID)
/if (${Debug}) /echo |- MoveToSpawn ==>

|-----------------------------------------------------------------------------Check LoS on the MoveToID.
	/if (!${Spawn[id ${MoveToID}].LineOfSight}) {
		/echo I cannot see ${Spawn[id ${MoveToID}].CleanName}
	} else {
		
	|----------------------------------------------------------------------Declare a 10 second timer.
		/declare retryTimer timer local 100
	
	|-------------------------------------------------------------------------Target the MoveToID
		/call TrueTarget ${MoveToID}
		
		/if (${Target.ID} == ${MoveToID}) {
			
			:MoveTo_Loop
			/if (${Debug}) /echo |- MoveToSpawn -| :MoveTo_Loop
		
		|--------------------------------------------------------------------------Engage MQ2MoveUtils.
			/moveto ID ${Target.ID} dist 7
			
		|------------------------------------------------------------------------Wait to start moving.
			/delay 10 ${Me.Moving}
			
		|----------------------------------------------------------------------Wait to stop moving.
			/delay 50 !${Me.Moving}
			
		|---------------------------------------------------------------Check target range.
			/if (${Target.Distance} > 20) {
			
			|------------------------------------------------------If there is time to try again.
				/if (${retryTimer}) {
					/goto :MoveTo_Loop
				
			|-------------------------------------------------------Alert user, and return.
				} else {
					/echo ERROR: @MoveToSpawn - Failed to move to ${Spawn[id ${MoveToID}].CleanName}
				}
					
			}
		}
	}

/if (${Debug}) /echo <== MoveToSpawn -|
/RETURN



|--------------------------------------------------------|
|- Runs to a given location.							-|
|--------------------------------------------------------|
|- @param y_loc: Y intercept of your next destination.	-|
|- @param x_loc: X intercept of your next destination.	-|
|--------------------------------------------------------|
SUB MoveToLoc(float y_loc, float x_loc)
/if (${Debug}) {
	/echo |- MoveToLoc ==>
	/echo Moving to location [${y_loc},${x_loc}]
}
	/if (!${LineOfSight[${Me.Y},${Me.X}:${y_loc},${x_loc}]}) {
		/docommand ${ChatToggle} I do not have line of sight on my destination.
	} else {
		
		/declare retryTimer timer local 150
		/declare startingLoc string local ${Me.Loc.Replace[ ,]}
		
	|-------------------------------------------------------------------------------------------Engage MQ2MoveUtils to move to the specified location.
		/squelch /moveto Loc ${y_loc} ${x_loc} dist 2
		/delay 10 ${Me.Moving}
		
	|-----------------------------------------------------------------------------------------------Wait to stop moving.
		:wait4Stop
		/if (${Debug}) /echo |- MoveToLoc -| :wait4Stop
		
	|------------------------------------------------------------------------------Check if movement events were called.
		/call Background_Events
		
	|---------------------------------------------------------------------------------------------------------------If I'm still moving, wait to stop.
		/if (${Me.Moving}) {
			/if (${retryTimer}) {
				/goto :wait4Stop
			} else {
				/squelch /stick off
			}
		}

	|------------------------------------------------------------------------------------------------------------------Calulate distance to desired location.
		/call Triangulate_Distance ${Me.Loc.Replace[ ,]} ${y_loc},${x_loc}
		/if (${Macro.Return} > 15) /docommand ${ChatToggle} I did not arrive at the desired location.
	}
	
/if (${Debug}) /echo <== MoveToLoc -|
/RETURN




---------------------------------------------------------------------------------------------------------------------------------------------->
- This method of validation was abandoned due to excessive overhead on startup, and being a downright pain in my ass...

SUB rewrite_ValidationKey(int newValue, int CurrentArg)
/if (${Debug}) /echo |- rewrite_ValidationKey ==>

	/if (!${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Length}) {
		/if (${Debug}) /echo |- rewrite_ValidationKey -| Creating key: [${newValue}].
		/call WriteToIni "${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}" "${newValue}" 1	
	} else {
	
		/declare i int local
		/declare old_Key string local ${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}]}
		/declare new_Key string local
		
		/for i 1 to ${CurrentArg}
		
			/if (${i} < ${CurrentArg}) {
				/varset new_Key ${new_Key}${Int[${old_Key.Arg[${i},-]}]}-
			} else {
				/varset new_Key ${new_Key}${newValue}-
			}
		
		/next i
		
		/varcalc i ${CurrentArg} + 1
		
		:check_NextArg
		/if (${Ini[${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}].Arg[${i},-].Length}) {
			/varset new_Key ${new_Key}${Int[${old_Key.Arg[${i},-]}]}- 
			/varcalc i ${i} + 1
			/goto :check_NextArg
		}
		/if (${Debug}) /echo |- rewrite_ValidationKey -| Rewriting key: [${new_Key}] 
		/call WriteToIni "${MacroData_Ini},ValidationKey-${MacroQuest.Server},${Me.CleanName}" "${new_Key}" 1
	}

/if (${Debug}) /echo <== rewrite_ValidationKey -|
/RETURN

---------------------------------------------------------------------------------------------------------------------------------------------->