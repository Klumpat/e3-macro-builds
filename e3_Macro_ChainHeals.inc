|-----------------------
|- EVENT_build_HealChain
|---------------------------
|- 
|---------------------------
#EVENT build_HealChain "<#1#> Build heal chain #2#"
#EVENT build_HealChain "<#1#> Build heal chain in #2#"
#EVENT build_HealChain "#1# shout, 'Build heal chain #2#'"
#EVENT build_HealChain "#1# shout, 'Build heal chain in #2#'"
#EVENT build_HealChain "#1# shouts, 'Build heal chain #2#'"
#EVENT build_HealChain "#1# shouts, 'Build heal chain in #2#'"
#EVENT build_HealChain "#1# tell your raid, 'Build heal chain #2#'"
#EVENT build_HealChain "#1# tell your raid, 'Build heal chain in #2#'"
#EVENT build_HealChain "#1# tells the raid, 'Build heal chain #2#'"
#EVENT build_HealChain "#1# tells the raid, 'Build heal chain in #2#'"
#EVENT build_HealChain "#1# tell your party, 'Build heal chain #2#'"
#EVENT build_HealChain "#1# tell your party, 'Build heal chain in #2#'"
#EVENT build_HealChain "#1# tells the group, 'Build heal chain #2#'"
#EVENT build_HealChain "#1# tells the group, 'Build heal chain in #2#'"
#EVENT build_HealChain "#1# tells the guild, 'Build heal chain in #2#'"
#EVENT build_HealChain "#1# tells the guild, 'Build heal chain #2#'"
#EVENT build_HealChain "#1# say to your guild, 'Build heal chain in #2#'"
#EVENT build_HealChain "#1# say to your guild, 'Build heal chain #2#'"
SUB EVENT_build_HealChain(line, ChatSender, HealChain_ChatChannel)
/varset Debug TRUE
/if (${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}
/if (${ChatSender.Equal[You]}) /varset ChatSender ${Me.CleanName}
/if (${Debug}) {
	/echo |- EVENT_build_HealChain ==>
	/echo |- EVENT_build_HealChain -| ChatSender: [${ChatSender}] || HealChain_ChatChannel: [${HealChain_ChatChannel}]
}

|--------------------------------------------------------------------------------------------------------------Check Join_HealChain.
	/if (${join_HealChain} || ${ChatSender.Equal[${Me.CleanName}]}) {
	
	|------------------------------------------------------------------------------------------------------Check for PC in zone.
		/if (!${SpawnCount[pc ${ChatSender}]}) {
			/if (${Debug}) /echo ERROR: |- EVENT_build_HealChain -| [${ChatSender}] and I are not in the same zone.
		} else {
			
		|----------------------------------------------------------------------------------------------------Check for an existing chain.
			/if (${Defined[Chain_Channel]}) {
				/leave ${Chain_Channel}
				/deletevar Chain_Alpha
				
				/deletevar Chain_Channel
				/deletevar Chain_ChannelAutoJoin
				/deletevar Chain_ChannelNumber
				
				/deletevar Chain_Count
				/deletevar Chain_MemberList
				/deletevar Chain_Timing
				
				/deletevar Chain_TankList
				
				/deletevar Chain_Status
				/deletevar Chain_StartLoc
			}
			
		|------------------------------------------------------------------------------------------------Declare chain variables.
			
			/declare Chain_Alpha string outer ${ChatSender}
			
			/declare Chain_Channel string outer ${HealChain_ChatChannel}
			/declare Chain_ChannelAutoJoin bool outer TRUE
			/declare Chain_ChannelNumber int outer
			
			/declare Chain_Count int outer
			/declare Chain_MemberList string outer
			/declare Chain_Timing string outer
			/declare Chain_Timer timer outer
			
			/declare Chain_TankList string outer
			
			/declare Chain_Status string outer BUILDING
			/declare Chain_StartLoc string outer
			/declare Chain_Target string outer
			/declare Chain_LastHealer string outer
			
		|---------------------------------------------------------------------Join the chain channel
			/call join_ChainChannel
			
		|---------------------------------------------------------------------------------------If I failed to join channel, and have run out of time, announce to user.
			/if (!${Chain_ChannelNumber}) {
				/docommand ${ChatToggle} Failed to join [${Chain_Channel}].
			} else {
			
			|---------------------------------------------------------------------------------------If I joined the Chain_Channel, 
				/if (${join_HealChain}) {
					/docommand /${Chain_ChannelNumber} Add Me
				}
			}
		}
	}
	
/if (${Debug}) /echo <== EVENT_build_HealChain -|
/varset Debug FALSE
/RETURN



|----------------------------------------------------------------------------|
|- join_ChainChannel														-|
|----------------------------------------------------------------------------|
|- Joins the heal chain channel, and calls EVENT Set_Chain_ChannelNumber.	-|
|----------------------------------------------------------------------------|
SUB join_ChainChannel
/if (${Debug}) /echo |- join_ChainChannel ==>

|--------------------------------------------------------------------------------Declare local variables.
	/declare channelTimer timer local 150
	/declare miscTimer timer local 30

	/docommand ${ChatToggle} Joining heal chain [${Chain_Channel}].
	
|-------------------------------------------------------------------------------Attempt to join the Chain_Channel.
	:joinChannel_Loop
	/join ${Chain_Channel}

|-------------------------------------------------------Wait to join the Chain_Channel, then set the Chain_ChannelNumber.	
	:waitLoop
	/doevents Set_Chain_ChannelNumber
	
|----------------------------------------------------------------------------If the chain channel has not been set.
	/if (!${Chain_ChannelNumber}) {
		
	|-----------------------------------------------------------------If there is time left on the miscTimer, wait to join the channel.	
		/if (${miscTimer}) {
			/goto :waitLoop
			
	|-------------------------------------------------------------------------If I failed to join channel, and still have time to retry, try again.		
		} else /if (${channelTimer}) {
			/goto :joinChannel_Loop
		}
	}
		
/if (${Debug}) /echo <== join_ChainChannel -|
/RETURN



|----------------------------------------------------------------------------------------|
|- EVENT Set_Chain_ChannelNumber														-|
|----------------------------------------------------------------------------------------|
|- Looks for the Chain_Channel in the channel list, and sets the Chain_ChannelNumber.	-|
|----------------------------------------------------------------------------------------|
|- @param line: Line that triggered the event.											-|
|- @param myChannels: The list of channels you have currently joined.					-|
|----------------------------------------------------------------------------------------|
#EVENT Set_Chain_ChannelNumber "Channels: #1#"
SUB EVENT_Set_Chain_ChannelNumber(line, myChannels)
/if (${Debug}) /echo |- EVENT_SetHealChainChannelNumber ==>

|------------------------------------------------------------Check AutoJoin
	/if (${Chain_ChannelAutoJoin}) {
		
		/declare i int local 1
	
		:checkChannel_Loop
		
	|------------------------------------------------------------------------------If myChannels.Arg[i] is not the Chain_Channel
		/if (${myChannels.Arg[${i},,].Arg[2,=].Arg[1,(].NotEqual[${Chain_Channel}]}) {
			
		|------------------------------------------------------------------Check if there is another channel to compare
			/varcalc i ${i} + 1
			/if (${myChannels.Arg[${i},,].Arg[2,=].Length}) {
				/goto :checkChannel_Loop
				
		|---------------------------------------------------------------If there are no more channels to compare, announce to user.
			} else {
				/docommand ${ChatToggle} Failed to find Chain_ChannelNumber!
			}	
			
	|-----------------------------------------------------------------------------------------If myChannels.Arg[i] is the Chain_Channel
		} else {
		
		|------------------------------------------------------------------------------Set Chain_Channel, and Chain_ChannelNumber
			/varset Chain_Channel ${myChannels.Arg[${i},,].Arg[2,=].Arg[1,(]}
			/varset Chain_ChannelNumber ${i}
			/if (${Debug}) /echo |- EVENT_SetHealChainChannelNumber -| Chain_Channel: [${Chain_ChannelNumber}]
		}
	}

/if (${Debug}) /echo <== EVENT_SetHealChainChannelNumber -|
/RETURN



|----------------------------------------------------------------------------------------|
|- EVENT Set_Chain_ChannelNumber														-|
|----------------------------------------------------------------------------------------|
|- Looks for the Chain_Channel in the channel list, and sets the Chain_ChannelNumber.	-|
|----------------------------------------------------------------------------------------|
|- @param line: Line that triggered the event.											-|
|- @param ChatSender: The command sender.												-|
|----------------------------------------------------------------------------------------|
#EVENT add_ChainMember "#1# tells |${Chain_Channel}:${Chain_ChannelNumber}|, 'Add Me'"
#EVENT add_ChainMember "#1# tell |${Chain_Channel}:${Chain_ChannelNumber}|, 'Add Me'"
SUB EVENT_add_ChainMember(line, ChatSender)
/varset Debug TRUE
|
/if (${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}
/if (${ChatSender.Equal[you]}) /varset ChatSender ${Me.CleanName}
/if (${Debug}) /echo |- EVENT_add_ChainMember ==>

|------------------------------------------------------------------------If the ChatSender is not in zone.
	/if (!${SpawnCount[pc ${ChatSender}]}) {
		/docommand /${Chain_ChannelNumber} [${ChatSender}] is not in ${Zone}.
	} else {
	
	|-----------------------------------------------------------------------------------------------If Chain_Status is BUILDING, and the Chain_Alpha is me.
		/if (${Chain_Status.Equal[BUILDING]}) {
			
		|-------------------------------------------------------------------------------------------If the ChatSender is already part of the HealChain
			/if (${Select[${ChatSender},${Chain_MemberList}]}) {
				/if (${Debug}) /echo |- EVENT_add_ChainMember -| [${ChatSender}] is already part of the heal chain.
			} else {
			
			|--------------------------------------------------------------------------------------Set Chain_MemberList, Chain_Count, and announce new counts to the Chain_Channel.
				/varcalc Chain_Count ${Chain_Count} + 1
				/if (${Chain_Alpha.Equal[${Me.CleanName}]}) {
					/varset Chain_MemberList ${Chain_MemberList}${ChatSender},
					/docommand /${Chain_ChannelNumber} Added, ${ChatSender} ChainCount: [${Chain_Count}] - MinTiming [${If[${Math.Calc[10/${Chain_Count}]}>${Int[${Math.Calc[10/${Chain_Count}]}]},${Math.Calc[${Int[${Math.Calc[10/${Chain_Count}]}]}+1].Int},${Math.Calc[10/${Chain_Count}].Int}]}s].
				}
			}
		}
	}

/if (${Debug}) /echo <== EVENT_add_ChainMember -|
|
/varset Debug FALSE
/RETURN



#EVENT start_HealChain "#*# |${Chain_Channel}:${Chain_ChannelNumber}|, 'Start Chain on #1# #2#'"
#EVENT start_HealChain "#*# |${Chain_Channel}:${Chain_ChannelNumber}|, 'Start heal Chain on #1# #2#'"
SUB EVENT_start_HealChain(line, TankList, TimingSetting)
/varset Debug TRUE
|
/if (${Debug}) /echo |- EVENT_start_HealChain ==>
	/if (${Chain_Status.Equal[BUILDING]}) {
		/varset Chain_Status SET_CHAINLIST
		/varset Chain_TankList ${TankList}
		/declare i int local
		
		:removeCharacters_Loop
		/if (!${Int[${TimingSetting.Right[1]}]}) {
			/if (${TimingSetting.Length}) {
				/varset TimingSetting ${TimingSetting.Replace[${TimingSetting.Right[1]},]}
				/goto :removeCharacters_Loop
			} else {
				/docommand /${Chain_ChannelNumber} TimingSetting was invalid, please try again.
			}
		} else {
			/varset Chain_Timing ${TimingSetting}s
		}
		
		/if (${Chain_Alpha.Equal[${Me.CleanName}]}) /docommand /${Chain_ChannelNumber} Set HealChain [${Chain_MemberList}]
		
		/declare miscTimer timer local 50
		
		:wait_ChainSet
		/doevents set_HealChain
		/if (${Chain_Status.NotEqual[ENGAGED]} && ${miscTimer}) /goto :wait_ChainSet
		
		/if (${Select[${Me.CleanName},${Chain_MemberList}]} && ${Chain_Status.NotEqual[ENGAGED]}) {
			/docommand /${Chain_ChannelNumber} Failed to engage Heal Chain!
		} else {
			/if (${Chain_Status.Equal[ENGAGED]}) /call HealChain
		}
	}	
	
/if (${Debug}) /echo <== EVENT_start_HealChain -|
|
/varset Debug FALSE
/RETURN



#EVENT set_HealChain "#*# |${Chain_Channel}:${Chain_ChannelNumber}|, 'Set HealChain [#1#]'"
SUB EVENT_set_HealChain(line, ChainList)
/if (${Debug}) /echo |- EVENT_set_HealChain ==>

	/if (${join_HealChain} && ${Chain_Status.Equal[SET_CHAINLIST]}) {
		/varset Chain_MemberList ${ChainList}
		/varset Chain_Status ENGAGED
	}

/if (${Debug}) /echo <== EVENT_set_HealChain -|
/RETURN

			/declare Chain_Alpha string outer ${ChatSender}
			
			/declare Chain_Channel string outer ${HealChain_ChatChannel}
			/declare Chain_ChannelAutoJoin bool outer TRUE
			/declare Chain_ChannelNumber int outer
			
			/declare Chain_Count int outer
			/declare Chain_MemberList string outer
			/declare Chain_Timing string outer
			/declare Chain_Timer timer outer
			
			/declare Chain_TankList string outer
			
			/declare Chain_Status string outer BUILDING
			/declare Chain_StartLoc string outer
			/declare Chain_Target string outer
			/declare Chain_NextHealer string outer

SUB HealChain
/varset Debug TRUE
/if (${Debug}) /echo |- HealChain ==>
	
	/varset Chain_StartLoc ${Me.Loc.Replace[ ,]}
	/varset Chain_LastHealer ${Chain_MemberList.Arg[${Chain_Count},,]}
	
	:HealChain_Loop

|-----------------------------------------------------------------------------------Check for a valid tank.
	/call set_ChainTarget
	
	/if (${Chain_Target.Equal[NOT_FOUND]}) {
		/docommand /${Chain_ChannelNumber} No tanks were found!  Ending heal chain.
	} else {
		
	|------------------------------------------------------------------------------Find Next Healer.
		/call set_NextHealer
	}
	
|-----------------------------------------------------------------------------------Check HealChain_Events.
	/call HealChain_Events
	|	|
	/delay 2
	|	|
	/if (${Chain_Status.NotEqual[END_CHAIN]}) /goto :HealChain_Loop
	
/if (${Debug}) /echo <== HealChain -|
/varset Debug FALSE
/RETURN



SUB set_ChainTarget
/if (${Debug}) /echo |- set_ChainTarget ==>

	/declare i int local 1

	:check_Chain_TankList

|-----------------------------------------------------------------------------------------If the spawn is not in zone.	
	/if (!${SpawnCount[pc ${Chain_TankList.Arg[${i},,]}]}) {
		
	|--------------------------------------------------------------------------------------Check for additional tanks.
		/if (${Debug}) /echo |- set_ChainTarget -| [${Chain_TankList.Arg[${i},,]}] is not in ${Zone}. 
		/varcalc i ${i} + 1
		/if (${Chain_TankList.Arg[${i},,].Length}) {
			/goto :check_Chain_TankList
			
	|-----------------------------------------------------------------------If no tanks are found, end the heal chain.
		} else {
			/varset Chain_Target NOT_FOUND
			/varset Chain_Status END_CHAIN
		}
		
|---------------------------------------------------------------------------------If the spawn is in zone, check range.
	} else {
	
	|-------------------------------------------------------------------------Check tank range from the Chain_StartLoc.
		/call Triangulate_Distance ${Spawn[pc ${Chain_TankList.Arg[${i},,]}].Loc.Replace[ ,]} ${Chain_StartLoc}
		
	|----------------------------------------------------------------------If the tank is out of range of the heal chain.
		/if (${Macro.Return} > 100) {
			/if (${Debug}) /echo |- set_ChainTarget -| [${Chain_TankList.Arg[${i},,]}] out of range. 
			
		|--------------------------------------------------------------------------------------Check for additional tanks.
			/varcalc i ${i} + 1
			/if (${Chain_TankList.Arg[${i},,].Length}) {
				/goto :check_Chain_TankList
				
		|-----------------------------------------------------------------------If no tanks are found, end the heal chain.
			} else {
				/varset Chain_Target NOT_FOUND
				/varset Chain_Status END_CHAIN
			}
		} else {
			
		|---------------------------------------------------------If the spawn is in zone, and in range, set Chain_Target.
			/varset Chain_Target ${Spawn[pc ${Chain_TankList.Arg[${i},,]}].CleanName}
			/if (${Debug}) /echo |- set_ChainTarget -| Chain_Target= [${Chain_Target}]
		}
	}
	
/if (${Debug}) /echo <== set_ChainTarget -|
/RETURN

SUB set_NextHealer
/if (${Debug}) /echo |- set_Healer ==>
	
	/declare i int local 1
	
	:find_LastHealer
	
	/if (${Chain_MemberList.Arg[${i},,].NotEqual[${Chain_LastHealer}]}) {
		/varcalc i ${i} + 1
		/if (${Chain_MemberList.Arg[${i},,].Length}) {
			/goto :find_LastHealer
		} else {
			|	|
			/docommand /${Chain_ChannelNumber} Unable to find [${Chain_LastHealer}] on the Chain_MemberList.  Restarting chain...
			|	|
		}
	} else {
	
		/if (${Debug}) /echo |- set_Healer -| LastHealer: [${Chain_MemberList.Arg[${i},,]}]
		
	}
	
/if (${Debug}) /echo <== set_Healer -|
/RETURN


/if (!${Chain_MemberList.Arg[${Math.Calc[${i} + 1]},,].Length}) {
			/if (${Debug}) /echo resetting i
			/varset i 1
		}

|																																	|





	/declare i int local 1
	
	:find_LastHealer
	/if (${Chain_MemberList.Arg[${i},,].Equal[${Chain_LastHealer}]}) {
		
		/varcalc i ${i} + 1
		
	|-----------------------------------------------------------------------------------------If the spawn is not in zone.	
		:find_NextHealer
		/if (!${SpawnCount[pc ${Chain_MemberList.Arg[${i},,]}]}) {
			
		|------------------------------------------------------------------------------------Check for additional healers.
			/if (${Debug}) /echo |- set_ChainTarget -| [${Chain_TankList.Arg[${i},,]}] is not in ${Zone}. 
			/varcalc i ${i} + 1
			/if (${Chain_MemberList.Arg[${i},,].Length}) {
				/goto :find_NextHealer
				
		|-----------------------------------------------------------------------If no tanks are found, end the heal chain.
			} else {
				/varset Chain_Target NOT_FOUND
			}
			
	|---------------------------------------------------------------------------------If the spawn is in zone, check range.
		}
	}




|																																	|
	/declare i int local 1

	:check_Chain_TankList

|-----------------------------------------------------------------------------------------If the spawn is not in zone.	
	/if (!${SpawnCount[pc ${Chain_MemberList.Arg[${i},,]}]}) {
		
	|--------------------------------------------------------------------------------------Check for additional tanks.
		/if (${Debug}) /echo |- set_ChainTarget -| [${Chain_TankList.Arg[${i},,]}] is not in ${Zone}. 
		/varcalc i ${i} + 1
		/if (${Chain_TankList.Arg[${i},,].Length}) {
			/goto :check_Chain_TankList
			
	|-----------------------------------------------------------------------If no tanks are found, end the heal chain.
		} else {
			/varset Chain_Target NOT_FOUND
		}
		
|---------------------------------------------------------------------------------If the spawn is in zone, check range.
	} else {
	
	|-------------------------------------------------------------------------Check tank range from the Chain_StartLoc.
		/call Triangulate_Distance ${Spawn[pc ${Chain_TankList.Arg[${i},,]}].Loc.Replace[ ,]} ${Chain_StartLoc}
		
	|----------------------------------------------------------------------If the tank is out of range of the heal chain.
		/if (${Macro.Return} > 100) {
			/if (${Debug}) /echo |- set_ChainTarget -| [${Chain_TankList.Arg[${i},,]}] out of range. 
			
		|--------------------------------------------------------------------------------------Check for additional tanks.
			/varcalc i ${i} + 1
			/if (${Chain_TankList.Arg[${i},,].Length}) {
				/goto :check_Chain_TankList
				
		|-----------------------------------------------------------------------If no tanks are found, end the heal chain.
			} else {
				/varset Chain_Target NOT_FOUND
			}
		} else {
			
		|---------------------------------------------------------If the spawn is in zone, and in range, set Chain_Target.
			/varset Chain_Target ${Spawn[pc ${Chain_TankList.Arg[${i},,]}].CleanName}
			/if (${Debug}) /echo |- set_ChainTarget -| Chain_Target= [${Chain_Target}]
		}
	}

Sub DO_CH(TankOne, TankTwo, NextCleric, Delay, CHMsg)
  /delay 5 !${Me.Moving}
  /if (${Spawn[pc ${TankOne}].ID}) {
    /target pc ${TankOne}
  } else {
    /varset TankOne ${TankTwo}
    /target pc ${TankOne}
  }
  /doevents RSMyTurn
  /if (${EndCCH}) /return
  /doevents RSMyTurn
  /if (!${EndCCH}) {
    /if (!${MyTurn}) /call MedTime
    /if (${MyTurn}) {
      /varset MyTurn FALSE
      /delay ${Delay}s
      /if (${Target.Distance}<=${Spell[${HealSpell}].MyRange}) {
        /if (${DoDivArb} && ${Target.PctHPs}<30) /call DoDivArb
        /if (!${MyTurn}) /${CCHChannel} ${Time.Hour}:${Time.Minute}:${Time.Second}  --- ${CHMsg} ${TankOne} <<< Next ${NextCleric} in ${Delay}
        /call Cast "${HealSpell}" ${HealSpellGem} 1s
      } else /if (${Target.Distance}<=${Spell[${FastHeal}].MyRange}) {
        /if (!${MyTurn}) /${CCHChannel} Tank was out of Range for that cycle!! Casting ${FastHeal}
        /if (!${MyTurn}) /${CCHChannel} ${Time.Hour}:${Time.Minute}:${Time.Second}  --- ${CHMsg} ${TankOne} <<< Next ${NextCleric} in ${Delay}
        /delay 55
        /call Cast "${FastHeal}" ${FastHealGem} 1s
      }
    }
  }
/return



SUB HealChain_Events
/if (${Debug}) /echo |- HealChain_Events ==>

	/doevents set_LastHealer
	/doevents end_HealChain
	
/if (${Debug}) /echo <== HealChain_Events -|
/RETURN



#EVENT end_HealChain "#*# |${Chain_Channel}:${Chain_ChannelNumber}|, 'End chain'"
#EVENT end_HealChain "#*# |${Chain_Channel}:${Chain_ChannelNumber}|, 'End Heal Chain'"
| #EVENT end_HealChain "#*# |${Chain_Channel}:${Chain_ChannelNumber}|, 'No tanks were found! Ending heal chain.'"
SUB EVENT_end_HealChain
/if (${Debug}) /echo |- EVENT_end_HealChain ==>

	/if (${Chain_Status.Equal[ENGAGED]}) {
		/varset Chain_Status END_CHAIN
	}
	
/if (${Debug}) /echo <== EVENT_end_HealChain -|
/RETURN



#EVENT set_LastHealer "#1# |${Chain_Channel}:${Chain_ChannelNumber}|, 'Healing #*#'"
SUB EVENT_set_LastHealer(line, ChatSender)
/if (${Debug}) /echo |- EVENT_set_LastHealer ==>

	/if (${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}
	/if (${ChatSender.Equal[you]}) /varset ChatSender ${Me.CleanName}

	/if (${Chain_Status.Equal[ENGAGED]}) {
		/varset Chain_LastHealer ${ChatSender}
		/varset Chain_Timer ${Chain_Timing}
	}
		
/if (${Debug}) /echo <== EVENT_set_LastHealer -|
/RETURN