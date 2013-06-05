|----------------------------------------------------------------------------------------------------|
|- e3_Macro_BegForBuffs.inc	v2.0 ( originally designed to work with e3.mac )						-|
|- Written by Killians of PEQ																		-|
|----------------------------------------------------------------------------------------------------|
|- Allows chat senders to request buffs by their full name, or alias listed in the BuffAliases_Ini.	-|
|----------------------------------------------------------------------------------------------------|

| Make an event to change buff request toggle
| Adding restricted spells
| Adding spell aliases
| updating spell aliases
| add a -group function, to cast on all group members

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

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (!${Bool[${Macro.Return.Arg[2,_]}]}) {
		
		/call IniToVar "${Macro_Ini},Buff Requests,Allow Buff Requests (On/Off)" allow_BuffRequests bool local
		
		| If the user is not valid, check allow_BuffRequests
		/if (${Macro.Return}) {
			
			| If allow_BuffRequests=TRUE, check restricked PCs
			/if (${Select[${ChatSender},${Ini[${Macro_Ini},Buff Requests,Restricted PCs]}]}) {			
				/tell ${ChatSender} I ain't buffin you, sucka!
			} else {
				/goto :allowAccess
			}
			
		| If allow_BuffRequests=FALSE
		} else {
			/tell ${ChatSender} Sorry, I'm not currently buffing.
		}
	} else {
		
		:allowAccess

		| If the ChatText is not found in the spellbook, check SpellAliases_Ini.
		:Retry_Alias
		/if (${Debug}) /echo |- Event_Chat -| :Retry_Alias

		/if (!${Me.Book[${ChatText}]}) {

			/if (${Ini[${SpellAliases_Ini},${Me.Class},${ChatText}].Length}) {
				
				| Disable follow before porting.	
				/if (${ChatText.Find[gport]}) {
					/if (${Stick.Active}) /squelch /stick off
					/if (${NetAdvPath.State}) /squelch /netfollow off
					/delay 30 !${Stick.Active} && !${NetAdvPath.State}
				}

				/varset ChatText ${Ini[${SpellAliases_Ini},${Me.Class},${ChatText}]}
				/goto :Retry_Alias
			}
		} else {
		
			| Check restricted spells list
			/if (${Select[${ChatText},${Ini[${Macro_Ini},Buff Requests,Restricted Spells]}]}) {
				/if (${ChatChannel.Equal[TELL]}) {
					/reply [${ChatText}] is a restricted spell.
				} else {
					/g [${ChatText}] is a restricted spell.
				}
			} else {
		
				| Check the request was a beneficial spell
				/if (${Spell[${ChatText}].SpellType.Left[10].NotEqual[Beneficial]}) {
					/if (${ChatChannel.Equal[TELL]}) {
						/reply You can only request beneficial spells.
					} else {
						/g You can only request beneficial spells.
					}
				} else {
		
					| Check range.
					/if (${Bool[${Spell[${ChatText}].MyRange}]} && ${Spawn[pc ${ChatSender}].Distance} > ${Spell[${ChatText}].MyRange}) {
						/if (${ChatChannel.Equal[TELL]}) {
							/reply You're out of range.
						} else {
							/g You're out of range.
						}
					} else {
					
						| Check mana.
						/if (${Spell[${ChatText}].Mana} > ${Me.CurrentMana}) {
							/if (${ChatChannel.Equal[TELL]}) {
								/reply I don't have enough mana to cast ${ChatText}, Try again later.
							} else {
								/g I don't have enough mana to cast ${ChatText}, Try again later.
							}
						} else {
		
							| Cast the requested spell, on the ChatSender.
							/if (!${Defined[DefaultGem]}) /declare DefaultGem int local 8
							/if (${Target.ID}) /declare oldTarget int local ${Target.ID}
							/if (${ChatChannel.Equal[TELL]}) {
								/reply  ${Spell[${ChatText}]} inc...
							} else {
								/g  ${Spell[${ChatText}]} inc...
							}
							
							/call e3_Cast ${Spawn[pc ${ChatSender}].ID} "${Spell[${ChatText}]}" Gem${DefaultGem} ${Math.Calc[${Spell[${ChatText}].RecastTime}+30]}s
							/if (!${oldTarget}) {
								/squelch /target clear
							} else {
								/call TrueTarget ${oldTarget}
							}
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

	| ---------------------------------------------------------------------------------------------------------------Add SpellAlias File Path.
	/if (!${Ini[e3 File Paths.ini,File Paths,Spell Aliases File].Length}) /call WriteToIni "e3 File Paths.ini,File Paths,Spell Aliases File" "Macro Inis\Spell Aliases.ini" 1
	
	| ----------------------------------------------------------------------------------------------------------------Import SpellAliases_Ini.
	/if (!${Ini[e3 File Paths.ini,File Paths,Spell Aliases File].Length}) {
		/echo ERROR: Could not find designated file path for [Spell Aliases], please review [e3 File Paths.ini] and restart.
		/endmacro
	} else {
		/declare SpellAliases_Ini string outer ${Ini[e3 File Paths.ini,File Paths,Spell Aliases File]}
	}

	/if (!${Ini[${SpellAliases_Ini}].Length}) /call MakeSpellAliases_Ini

/if (${Debug}) /echo <== setup_BegForBuffs -|
/RETURN



SUB add_BegForBuffs_MacroSettings
/RETURN



|----------------------------|
|- Build SpellAliases_Ini.	-|
|----------------------------|
| Needs to be updated
SUB MakeSpellAliases_Ini

	/echo Generating [${SpellAliases_Ini}], which contains universal settings for e3's preformance and functionality.

	| Beastlord Aliases.
	/call WriteToIni "${SpellAliases_Ini},Beastlord,SD" "Spiritual Dominion"
	/call WriteToIni "${SpellAliases_Ini},Beastlord,SoW" "Spirit of Wolf"
	/call WriteToIni "${SpellAliases_Ini},Beastlord,SV" "Spiritual Vigor"
	/call WriteToIni "${SpellAliases_Ini},Beastlord,Shrew" "Pack Shrew"

	| Cleric Aliases.
	/call WriteToIni "${SpellAliases_Ini},Cleric,Kazad" "Kazad's Mark"
	/call WriteToIni "${SpellAliases_Ini},Cleric,Virtue" "Virtue"
	/call WriteToIni "${SpellAliases_Ini},Cleric,Virt" "Virtue"
	/call WriteToIni "${SpellAliases_Ini},Cleric,HoV" "Hand of Virtue"	
	/call WriteToIni "${SpellAliases_Ini},Cleric,BoR" "Blessing of Reverence"	
	/call WriteToIni "${SpellAliases_Ini},Cleric,CHeal" "Complete Heal"	
	/call WriteToIni "${SpellAliases_Ini},Cleric,HoT" "Holy Elixir"
	/call WriteToIni "${SpellAliases_Ini},Cleric,Heal" "Holy Light"
	/call WriteToIni "${SpellAliases_Ini},Cleric,QHeal" "Supernal Remedy"
	/call WriteToIni "${SpellAliases_Ini},Cleric,GroupHeal" "Word of Replenishment"
	/call WriteToIni "${SpellAliases_Ini},Cleric,GroupHoT" "Ethereal Elixir"
	/call WriteToIni "${SpellAliases_Ini},Cleric,RGC" "Remove Greater Curse"	
	
	| Druid Aliases.
	/call WriteToIni "${SpellAliases_Ini},Druid,FoE" "Flight of Eagles"
	/call WriteToIni "${SpellAliases_Ini},Druid,DS" "Legacy of Bracken"
	/call WriteToIni "${SpellAliases_Ini},Druid,BoT9" "Blessing of the Nine"	
	/call WriteToIni "${SpellAliases_Ini},Druid,Heal" "Sylvan Infusion"
	/call WriteToIni "${SpellAliases_Ini},Druid,Regen" "Blessing of Replenishment"
	/call WriteToIni "${SpellAliases_Ini},Druid,CoS" "Circle of Seasons"
	/call WriteToIni "${SpellAliases_Ini},Druid,Str" "Nature's Might"
	/call WriteToIni "${SpellAliases_Ini},Druid,GroupWolf" "Share Form of the Great Wolf"
	/call WriteToIni "${SpellAliases_Ini},Druid,GroupInvis" "Foliage Shield"
	/call WriteToIni "${SpellAliases_Ini},Druid,Evac" "Succor"
	/call WriteToIni "${SpellAliases_Ini},Druid,SoE" "Spirit of Eagle"
	/call WriteToIni "${SpellAliases_Ini},Druid,PoS" "Protection of Seasons"
	/call WriteToIni "${SpellAliases_Ini},Druid,RGC" "Remove Greater Curse"
	/call WriteToIni "${SpellAliases_Ini},Druid,SoW" "Spirit of Wolf"
	/call WriteToIni "${SpellAliases_Ini},Druid,Shrew" "Pack Shrew"
	
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortNK" "Circle of Karana"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortLavastorm" "Circle of Lavastorm"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortSRo" "Circle of Ro"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortMisty" "Circle of Misty"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortFeerrott" "Circle of Feerrott"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortCommons" "Circle of Commons"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortEK" "Succor: East"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortSureFall" "Circle of Surefall Glade"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortButcher" "Circle of Butcher"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortSteamfont" "Circle of Steamfont"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortSkyfire" "Wind of the North"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortEJ" "Wind of the South"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortDawnshroud" "Circle of Dawnshroud"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortDSP" "Circle of Dawnshroud"	
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortTwilight" "Circle of Twilight"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortGrimling" "Circle of Grimling"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortNexus" "Circle of the Nexus"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortStonebrunt" "Circle of Stonebrunt"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortTox" "Circle of Toxxulia"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortBarindu" "Circle of Barindu"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortNatimbi" "Circle of Natimbi"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortPoK" "Circle of Knowledge"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortKnowledge" "Circle of Knowledge"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortCS" "Circle of Cobalt Scar"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortWL" "Circle of Wakening Lands"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortGD" "Circle of Great Divide"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortCombines" "Circle of the Combines"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortDreadlands" "Circle of the Combines"
	/call WriteToIni "${SpellAliases_Ini},Druid,GPortDL" "Circle of the Combines"

	| Enchanter Aliases.	
	/call WriteToIni "${SpellAliases_Ini},Enchanter,Haste" "Vallon's Quickening"
	/call WriteToIni "${SpellAliases_Ini},Enchanter,VQ" "Vallon's Quickening"
	/call WriteToIni "${SpellAliases_Ini},Enchanter,VoQ" "Voice Of Quellious"
	/call WriteToIni "${SpellAliases_Ini},Enchanter,C5" "Voice Of Quellious"
	/call WriteToIni "${SpellAliases_Ini},Enchanter,KEI" "Koadic's Endless Intellect"
	/call WriteToIni "${SpellAliases_Ini},Enchanter,C4" "Koadic's Endless Intellect"
	/call WriteToIni "${SpellAliases_Ini},Enchanter,GoD" "Guard of Druzzil"
	/call WriteToIni "${SpellAliases_Ini},Enchanter,CHA" "Overwhelming Splendor"
	/call WriteToIni "${SpellAliases_Ini},Enchanter,SoV" "Speed of Vallon"
	
	| Magician Aliases.
	/call WriteToIni "${SpellAliases_Ini},Magician,DS" "Maelstrom of Ro"
	/call WriteToIni "${SpellAliases_Ini},Magician,CoH" "Call of the Hero"

	| Necromancer Aliases.	
	/call WriteToIni "${SpellAliases_Ini},Necromancer,GDMF" "Dead Men Floating"
	/call WriteToIni "${SpellAliases_Ini},Necromancer,GroupDMF" "Dead Men Floating"
	/call WriteToIni "${SpellAliases_Ini},Necromancer,DMF" "Dead Man Floating"
	/call WriteToIni "${SpellAliases_Ini},Necromancer,IVU" "Invisibility versus Undead"
	/call WriteToIni "${SpellAliases_Ini},Necromancer,SummonCorpse" "Conjure Corpse"
	/call WriteToIni "${SpellAliases_Ini},Necromancer,Sac" "Sacrifice"
	/call WriteToIni "${SpellAliases_Ini},Necromancer,Bind" "Bind Affinity"

	| Paladin Aliases.	
	/call WriteToIni "${SpellAliases_Ini},Paladin,BSS" "Brell's Stalwart Shield"
	/call WriteToIni "${SpellAliases_Ini},Paladin,RGC" "Remove Greater Curse"

	| Ranger Aliases.	
	/call WriteToIni "${SpellAliases_Ini},Ranger,SoTP" "Spirit of the Predator"
	/call WriteToIni "${SpellAliases_Ini},Ranger,Pred" "Spirit of the Predator"
	/call WriteToIni "${SpellAliases_Ini},Ranger,SoT" "Strength of Tunare"
	/call WriteToIni "${SpellAliases_Ini},Ranger,Tunare" "Strength of Tunare"
	/call WriteToIni "${SpellAliases_Ini},Ranger,Rathe" "Call of the Rathe"
	/call WriteToIni "${SpellAliases_Ini},Ranger,CoR" "Call of the Rathe"
	/call WriteToIni "${SpellAliases_Ini},Ranger,SoE" "Spirit of Eagle"
	/call WriteToIni "${SpellAliases_Ini},Ranger,SoW" "Spirit of Wolf"
	/call WriteToIni "${SpellAliases_Ini},Ranger,DS" "Shield of Spikes"
	/call WriteToIni "${SpellAliases_Ini},Ranger,CoS" "Circle of Summer"	
	/call WriteToIni "${SpellAliases_Ini},Ranger,CoW" "Circle of Winter"
	/call WriteToIni "${SpellAliases_Ini},Ranger,Shrew" "Pack Shrew"	
	
	| Shadow Knight Aliases.
	/call WriteToIni "${SpellAliases_Ini},Shadow Knight,Voice" "Voice of Thule"
	/call WriteToIni "${SpellAliases_Ini},Shadow Knight,VoT" "Voice of Thule"
	/call WriteToIni "${SpellAliases_Ini},Shadow Knight,Thule" "Voice of Thule"
	
	| Shaman Aliases.	
	/call WriteToIni "${SpellAliases_Ini},Shaman,HoT" "Breath of Trushar"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Heal" "Daluda's Mending"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Fo7" "Focus of the Seventh"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Focus" "Focus of Soul"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Haste" "Talisman of Celerity"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Billy" "Spirit of Bih`li"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Bih`li" "Spirit of Bih`li"
	/call WriteToIni "${SpellAliases_Ini},Shaman,SoB" "Spirit of Bih`li"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Regen" "Blessing of Replenishment"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Agi" "Talisman of the Wrulan"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Dex" "Talisman of the Raptor"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Sta" "Talisman of the Boar"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Str" "Talisman of the Diaku"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Tribunal" "Talisman of the Tribunal"
	/call WriteToIni "${SpellAliases_Ini},Shaman,ToT" "Talisman of the Tribunal"	
	/call WriteToIni "${SpellAliases_Ini},Shaman,Jag" "Spirit of the Jaguar"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Shrink" "Shrink"
	/call WriteToIni "${SpellAliases_Ini},Shaman,GroupShrink" "Tiny Terror"
	/call WriteToIni "${SpellAliases_Ini},Shaman,GShrink" "Tiny Terror"
	/call WriteToIni "${SpellAliases_Ini},Shaman,RGC" "Remove Greater Curse"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Cha" "Unfailing Reverence"
	/call WriteToIni "${SpellAliases_Ini},Shaman,SoW" "Spirit of Wolf"
	/call WriteToIni "${SpellAliases_Ini},Shaman,Shrew" "Pack Shrew"

	| Wizard Aliases.	
	/call WriteToIni "${SpellAliases_Ini},Wizard,Decess" "Decession"
	/call WriteToIni "${SpellAliases_Ini},Wizard,Evac" "Decession"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TL" "Teleport"
	
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortWK" "West Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortNRo" "Ro Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortCommons" "Common Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortCazic" "Cazic Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortNek" "Nek Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortNektulos" "Nek Portal"	
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortNK" "North Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortGfay" "Fay Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortEJ" "Markar's Relocation"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortEmerald" "Markar's Relocation"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportSkyfire" "Tishan's Relocation"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportDL" "Combine Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportDreadlands" "Combine Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportCombines" "Combine Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportDawnshroud" "Dawnshroud Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportDSP" "Dawnshroud Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportTwilight" "Twilight Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportGrimling" "Grimling Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportNexus" "Nexus Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GroupTL" "Translocate: Group"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportTx" "Tox Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportBarindu" "Barindu Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportNatimbi" "Natimbi Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportPoK" "Knowledge Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GportKnowledge" "Knowledge Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortSky" "Alter Plane: Sky"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortHate" "Alter Plane: Hate"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortCS" "Cobalt Scar Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortWL" "Wakening Lands Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortGD" "Great Divide Portal"
	/call WriteToIni "${SpellAliases_Ini},Wizard,GPortIceclade" "Iceclad Portal"
	
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLDawnshroud" "Translocate: Dawnshroud"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLCazic" "Translocate: Cazic"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLNRo" "Translocate: Ro"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLWC" "Translocate: West"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLNek" "Translocate: Nek"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLCommon" "Translocate: Common"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLFay" "Translocate: Fay"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLCombine" "Translocate: Combine"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLTwilight" "Translocate: Twilight"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLGrimling" "Translocate: Grimling"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLNexus" "Translocate: Nexus"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLGroup" "Translocate"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLTox" "Translocate: Tox"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLNatimbi" "Translocate: Natimbi"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLBarindu" "Translocate: Barindu"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLPoK" "Translocate: Knowledge"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLCS" "Translocate: Cobalt Scar"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLWL" "Translocate: Wakening Lands"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLGD" "Translocate: Great Divide"
	/call WriteToIni "${SpellAliases_Ini},Wizard,TLIceclad" "Translocate: Iceclad"
/RETURN


#EVENT makeSpellList "[MQ2] Generate Character SpellList"
#EVENT makeSpellList "<#*#> Generate Character SpellList"
SUB EVENT_makeSpellList

	/echo Creating a spell list for [${Me.CleanName}]...

	/call Build_Alphabetized_Ini "Macro Inis\SpellList_${Me.CleanName}.ini"
	
	/declare i int local
	/declare spellName string local
	
	/for i 1 to 720
		
		/if (${Bool[${Me.Book[${i}]}]}) {
			/varset spellName ${Me.Book[${i}]}
			/ini "Macro Inis\SpellList_${Me.CleanName}.ini" ${spellName.Left[1]} "${spellName}"
		}
		
	/next i
	
	/echo Finished generating [${Me.CleanName}_SpellList.ini].

/RETURN



|** This script is broken and can potentially crash EQBC....something to do with too many characters being passed.  Disabling for the time being.

	|---------------------------------------------------------------------------------------------------------If a BuffList was requested.
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