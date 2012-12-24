|----------------------------------------------------------------------------------------------------|
|- e3_Macro_BegForBuffs.inc	v1.0 ( originally designed to work with e3.mac )						-|
|- Written by Killians of PEQ																		-|
|----------------------------------------------------------------------------------------------------|
|- Allows chat senders to request buffs by their full name, or alias listed in the BuffAliases_Ini.	-|
|----------------------------------------------------------------------------------------------------|



|---------------------------------------Specify chat channels to monitor.
#chat group
#chat tell



|------------------------------------------------------------------------|
|- Request spells by name or alias.										-|
|------------------------------------------------------------------------|
|- @param ChatChannel: The channel a chat command was given through.	-|
|- @param ChatSender: The PC who sent the chat message.					-|
|- @param ChatText: The message that was sent.							-|
|------------------------------------------------------------------------|
SUB event_Chat(ChatChannel, ChatSender, ChatText)
/if (${Debug}) /echo |- Event_Chat ==>

|-----------------------------------------------------------------------------------------------------------------------Clean-up ChatSenders name.
	/if (${ChatChannel.Equal[TELL]} && ${ChatSender.Left[1].Compare[ ]}<0 || ${ChatChannel.Equal[GROUP]} && ${ChatSender.Left[1].Compare[ ]}<0) /varset ChatSender ${ChatSender.Right[-2].Left[-1]}
	
|---------------------Check to see the incoming text is not an integer, this would cause a non-numeric error.
	/if (!${Int[${ChatText}]}) {
	
	|-----------------------------------------------------------------------------------------------------------------If a BuffList was requested.
	|** This script is broken and can potentially crash EQBC....something to do with too many characters being passed.  Disabling for the time being.
		/if (${ChatText.Equal[Bufflist]} || ${ChatText.Equal[ListBuffs]}) {
			
		|-------------------------------------------------------------------------Create a list of my spell aliases.
			/declare MyAliases string local ${Ini[${SpellAliases_Ini},${Me.Class}].Replace[||,].Arg[1,|].Replace[., ]}
			/declare count int local 2
			/declare MessageLength int local
			:count_SpellAliases
			/if (${Ini[${SpellAliases_Ini},${Me.Class}].Replace[||,].Arg[${count},|].Length}) {
				
				/varset MyAliases ${MyAliases},${Ini[${SpellAliases_Ini},${Me.Class}].Replace[||,].Arg[${count},|].Replace[., ]}
				/varcalc count ${count} + 1
				/goto :count_SpellAliases
				
				
				/if (${ChatChannel.Equal[TELL]}) {
					/reply You may request, ${MyAliases}.  Alternatively, you may request any ${Me.Class} spell by name.
				} else {
					/g You may request, ${MyAliases}.  Alternatively, you may request any ${Me.Class} spell by name.
				}
				
				
			}
		}
	**|
	|-----------------------------------------If the ChatText is not found in the spellbook, check SpellAliases_Ini.
		:Retry_Alias
		/if (${Debug}) /echo |- Event_Chat -| :Retry_Alias
		
		/if (!${Me.Book[${ChatText}]}) {
			/if (${Ini[${SpellAliases_Ini},${Me.Class},${ChatText}].Length}) {
				
			|--------------------------------------------------------------------Disable follow before porting.	
				/if (${ChatText.Find[gport]}) {
					/if (${Stick.Active}) /squelch /stick off
					/if (${NetAdvPath.State}) /squelch /netfollow off
					/delay 30 !${Stick.Active} && !${NetAdvPath.State}
				}
				
				/varset ChatText ${Ini[${SpellAliases_Ini},${Me.Class},${ChatText}]}
				/goto :Retry_Alias
			}
			
		} else {
			
			/if (${Select[${ChatText},${restrictedBuffs.Replace[/,,]}]}) {
				
				/if (${ChatChannel.Equal[TELL]}) {
					/reply [${ChatText}] is a restricted spell.
				} else {
					/g [${ChatText}] is a restricted spell.
				}
				
			} else {
		|------------------------------------------------------------------------------If the ChatText was found in my spellbook, or SpellAliases_Ini.
		
			|------------------------------------------------------------------Make sure ChatSender is in the same zone.
				/if (!${SpawnCount[pc ${ChatSender}]}) {
					
					/if (${ChatChannel.Equal[TELL]}) /reply We're not in the same zone.
					
				} else {
				
				|--------------------------------------------------------------------------------------------Check mana.
					/if (${Spell[${ChatText}].Mana} > ${Me.CurrentMana}) {
						/if (${ChatChannel.Equal[TELL]}) {
							/reply I don't have enough mana to cast ${ChatText}, Try again later.
						} else {
							/g I don't have enough mana to cast ${ChatText}, Try again later.
						}
					} else {
					
					|---------------------------------------------------------------------------------------Check range.
						/if (${Spawn[pc ${ChatSender}].Distance} > ${Spell[${ChatText}].MyRange} && ${Spell[${ChatText}].MyRange}) {
							
							/if (${ChatChannel.Equal[TELL]}) {
								/reply You're out of range.
							} else {
								/g You're out of range.
							}
						
						} else {
						
						|---------------------------------------------------------------------------------Cast the requested spell, on the ChatSender.
							/if (!${Defined[DefaultGem]}) /declare DefaultGem int local 8
							/if (${ChatChannel.Equal[TELL]}) {
								/reply  ${Spell[${ChatText}]} inc...
							} else {
								/g  ${Spell[${ChatText}]} inc...
							}
							
							/call e3_Cast ${Spawn[pc ${ChatSender}].ID} "${Spell[${ChatText}]}" Gem${DefaultGem} ${Math.Calc[${Spell[${ChatText}].RecastTime}+30]}s
							/if (${Target.ID}) /squelch /target clear
						}
					}
				}
			}
		}
	}
	
/if (${Debug}) /echo <== Event_Chat -|
/RETURN



|--------------------------------|
|- Setup BegForBuff variables.	-|
|--------------------------------|
SUB setup_BegForBuffs
/if (${Debug}) /echo |- setup_BegForBuffs ==>

	/if (!${Ini[${SpellAliases_Ini}].Length}) /call MakeSpellAliases_Ini
	/if (${Ini[${Macro_Ini},Buffs,Restricted Buffs].Length}) /call IniToVar "${Macro_Ini},Buffs,Restricted Buffs" restrictedBuffs string outer
	/call IniToVar "${Macro_Ini},Buffs,Allow Buff Requests (On/Off/PWOnly)" allow_BuffRequests ${If[${Ini[${Macro_Ini},Buffs,Allow Buff Requests (On/Off/PWOnly)].Equal[pwonly]},string,bool]} outer

/if (${Debug}) /echo <== setup_BegForBuffs -|
/RETURN