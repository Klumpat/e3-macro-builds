#EVENT Combine "[MQ2] Combine"
#EVENT Combine "<#*#> Combine"
#EVENT Combine "[MQ2] Auto Combine"
#EVENT Combine "<#*#> Auto Combine"
SUB EVENT_Combine
/if (${Debug}) /echo |- EVENT_Combine ==>

	/if (${Window[TradeSkillWnd].Open}) {
	
	|------------------------------------------------------------------------------------------Check that a recipe has been selected.
		/if (!${Window[TradeSkillWnd].Child[COMBW_CombineButton].Enabled}) {
			/echo ERROR: You must have a recipe selected to AutoCombine.
		} else {
		
	|-----------------------------------------------------------------------------------------Try to combine the selected recipe.
		
		|--------------------------------------------------Declare variables.
			
			/declare CombiningTradeSkills bool outer TRUE
			/declare combineReturn string outer
			/declare skills int outer 0
			/declare wins int outer 0
			/declare fails int outer 0
			/declare total int outer 0
			/declare rate float outer 0
			/declare miscTimer timer local
			
		|---------------------------------------------Flush old events.
			/doevents flush
			
			/docommand ${ChatToggle} AutoCombine: Combining tradeskill ingredients...
			
			:combineLoop
				
				/varset combineReturn X
				
				:waiting_Cursor
				/if (${Cursor.ID}) {
					/if (${Select[${Cursor},${badItems}]} || ${Select[${Cursor.ID},${badItems}]}) {
						/echo Destroying [${Cursor}].
						/destroy
					}
					/call ClearCursor
					/delay 10 ${Cursor.ID}
					/goto :waiting_Cursor
				}
				
				:buttonLoop
				/call CombineEvents
				/call Background_Events
				/if (${Cursor.ID}) /goto :waiting_Cursor
				/if (!${Window[TradeSkillWnd].Child[COMBW_CombineButton].Enabled}) /goto :buttonLoop
				
				/nomodkey /notify TradeskillWnd CombineButton leftmouseup

			|---------------------------------------------------Wait for an item, or event return
				/varset miscTimer 100
	
				:waiting_Item
				/if (${Debug}) /echo |- EVENT_Combine -| Waiting for combine loop.
				
				/call CombineEvents
				/call Background_Events
				
				/if (!${Cursor.ID} && ${combineReturn.NotEqual[COMBINE_FAILED]} && ${combineReturn.NotEqual[COMBINE_FINISHED]}) {
					
				|-----------------------------------------------If I'm out of time, alert the user that the combine script has failed.
					/if (!${miscTimer}) {
						/docommand ${ChatToggle}  ERROR: |- AutoCombine -| Combine has failed, due to an unknown error.  Ending the macro for safety.
						/beep
						/delay 2
						/beep
						/endmacro
					} else {
					
					|---------------------------------------Keep waiting for an outcome.
						/goto :waiting_Item
					}
				}
			
			/if (${Me.FreeInventory} && ${combineReturn.NotEqual[COMBINE_FINISHED]}) {
				/goto :combineLoop
			} else {
				/if (${total}) {
				
				|-------------------------------------------------------------------Skillups.
					/if (${skills}) {
						/varcalc rate 100*${skills}/${total}
						/echo (${skills}) skillups out of (${total}) combines (${rate}%).
					}	
				|--------------------------------------------------------------------------Successful combines.
				    /varcalc rate 100*${wins}/${total}
					/echo (${wins}) of (${total}) combines succeeded (${rate}%).
				    
				|---------------------------------------------------------------------------Failed combines.
					/if (${fails}) {
						/varcalc rate 100*${fails}/${total}
					    /echo (${fails}) of (${total}) combines failed (${rate}%).
				    }
			    }
			   
				/deletevar CombiningTradeSkills
				/deletevar combineReturn
				/deletevar skills
				/deletevar wins
				/deletevar fails
				/deletevar total
				/deletevar rate
				
				/doevents flush
				
				/docommand ${ChatToggle} AutoCombine: I have finished combining tradeskill ingredients.
			   
			}
		}
	}

/if (${Debug}) /echo <== EVENT_Combine -|
/RETURN



SUB CombineEvents
/doevents skillup
/doevents success
/doevents failure
/doevents done
/RETURN



#event done "Sorry, but you don't have everything you need for this recipe in your general inventory."
Sub Event_done
/if (${Debug}) /echo |- Event_done ==>

	/if (${CombiningTradeSkills}) /varset combineReturn COMBINE_FINISHED

/if (${Debug}) /echo <== Event_done -|
/return



#event success "You have fashioned the items together to create something new: #1#."
Sub Event_success(line, newItem)
/if (${Debug}) /echo |- Event_success ==>

	/if (${CombiningTradeSkills}) {
		/varcalc wins ${wins} + 1
		/varcalc total ${total} + 1
		/varset combineReturn COMBINE_SUCCESS
		/delay 10 ${Cursor.ID}
		/if (${Verbosity}) /echo AutoCombine: Attempt [${total}] successful, created [${newItem}].
	}

/if (${Debug}) /echo <== Event_success -|
/return



#event failure "You lacked the skills to fashion the items together."
Sub Event_failure
/if (${Debug}) /echo |- Event_failure ==>

	/if (${CombiningTradeSkills}) {
		/varcalc fails ${fails} + 1
		/varcalc total ${total} + 1
		/varset combineReturn COMBINE_FAILED
		/if (${Verbosity}) /echo AutoCombine: Attempt [${total}] failed.
	}

/if (${Debug}) /echo <== Event_failure -|
/return



#event skillup "You have become better at#*#"
Sub Event_skillup
/if (${Debug}) /echo |- Event_skillup ==>

	/if (${CombiningTradeSkills}) {
	   /varcalc skills ${skills}+1
	   /varset combineReturn COMBINE_SKILLUP
	   /echo AutoCombine: Skillup!
	}

/if (${Debug}) /echo <== Event_skillup -|
/return 