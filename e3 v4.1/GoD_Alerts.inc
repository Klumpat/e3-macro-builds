|----------------------------------------------------------------|
|- GoD_Alerts.inc ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ									-|
|----------------------------------------------------------------|
|- Contains various alerts for GoD events.						-|
|----------------------------------------------------------------|

#event Define_UquaKey "#*#The #1# must unlock the door to the next room.#*#"
SUB event_Define_UquaKey(string line, string KeyToUse)
	/gu >>^<< The ${KeyToUse} unlocks the door >>^<<
	/popup >>^<< The ${KeyToUse} unlocks the door >>^<<
/return

#event setClass_Warrior "#*#Brute force and brawn#*#"
#event setClass_Shaman "#*#Cringe at the appearance of talismans#*#"
#event setClass_Beastlord "#*#Deep gashes of feral savagery#*#"
#event setClass_Necromancer "#*#Doom of death#*#"
#event setClass_Cleric "#*#Dread of celestial spirit#*#"
#event setClass_Shadowknight "#*#Dreads the strike of dead#*#"
#event setClass_Monk "#*#Focused tranquility#*#"
#event setClass_Bard "#*#Foreboding melody#*#"
#event setClass_Paladin "#*#Holy blade#*#"
#event setClass_Rogue "#*#Ignores anything behind it#*#"
#event setClass_Enchanter "#*#Mind and body vulnerable#*#"
#event setClass_Wizard "#*#Falters when struck with the power of the elements#*#"
#event setClass_Berserker "#*#Shies from heavy blades#*#"
#event setClass_Magician "#*#Summoned elements#*#"
#event setClass_Druid "#*#The creature seems weak in the face of the power of nature#*#"
#event setClass_Ranger "#*#True shots and fast blades#*#"

SUB event_setClass_Warrior
	/popup Spawn must be killed by a >>^<< WARRIOR >>^<<
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< WARRIOR >>^<<
/return

SUB event_setClass_Shaman
	/popup Spawn must be killed by a >>^<< SHAMAN >>^<< 
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< SHAMAN >>^<< 
/return

SUB event_setClass_Beastlord
	/popup Spawn must be killed by a >>^<< BEASTLORD >>^<< 
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< BEASTLORD >>^<< 
/return

SUB event_setClass_Necromancer
	/popup Spawn must be killed by a >>^<< NECROMANCER >>^<< 
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< NECROMANCER >>^<< 
/return

SUB event_setClass_Cleric
	/popup Spawn must be killed by a >>^<< CLERIC >>^<< 
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< CLERIC >>^<< 
/return

SUB event_setClass_Shadowknight
	/popup Spawn must be killed by a >>^<< SHADOWKNIGHT >>^<< 
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< SHADOWKNIGHT >>^<< 
/return

SUB event_setClass_Monk
	/popup Spawn must be killed by a >>^<< MONK >>^<< 
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< MONK >>^<< 
/return

SUB event_setClass_Bard
	/popup Spawn must be killed by a >>^<< BARD >>^<< 
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< BARD >>^<< 
/return

SUB event_setClass_Paladin
	/popup Spawn must be killed by a >>^<< PALADIN >>^<< 
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< PALADIN >>^<< 
/return

SUB event_setClass_Rogue
	/popup Spawn must be killed by a >>^<< ROGUE >>^<< 
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< ROGUE >>^<< 
/return

SUB event_setClass_Enchanter
	/popup Spawn must be killed by a >>^<< ENCHANTER >>^<< 
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< ENCHANTER >>^<< 
/return

SUB event_setClass_Wizard
	/popup Spawn must be killed by a >>^<< WIZARD >>^<< 
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< WIZARD >>^<< 
/return

SUB event_setClass_Berserker
	/popup Spawn must be killed by a >>^<< BERSERKER >>^<< 
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< BERSERKER >>^<< 
/return

SUB event_setClass_Magician
	/popup Spawn must be killed by a >>^<< MAGICIAN >>^<< 
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< MAGICIAN >>^<< 
/return

SUB event_setClass_Druid
	/popup Spawn must be killed by a >>^<< DRUID >>^<< 
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< DRUID >>^<< 
/return

SUB event_setClass_Ranger
	/popup Spawn must be killed by a >>^<< RANGER >>^<< 
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< RANGER >>^<< 
/return

#event startClass_Priests "#*#The creature cannot stand up to the power of healers#*#"
#event startClass_Hybrids "#*#The creature appears weak to the combined effort of might and magic#*#"
#event startClass_Casters "#*#The creature will perish under the strength of intelligent magic#*#"
#event startClass_Melees "#*#The creature appears weak to the combined effort of strength and cunning#*#"

SUB event_startClass_Priests
	/if (!${Defined[murauderKill]}) {
		/declare murauderKill int outer 3
	} else /if (${Defined[murauderKill]}) {
		/varset murauderKill 3
	}
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< PRIEST >>^<< 
/return

SUB event_startClass_Hybrids
	/if (!${Defined[murauderKill]}) {
		/declare murauderKill int outer 2
	} else /if (${Defined[murauderKill]}) {
		/varset murauderKill 2
	}
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< HYBRID >>^<< 
/return

SUB event_startClass_Casters
	/if (!${Defined[murauderKill]}) {
		/declare murauderKill int outer 1
	} else /if (${Defined[murauderKill]}) {
		/varset murauderKill 1
	}
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< CASTER >>^<< 
/return

SUB event_startClass_Melees
	/if (!${Defined[murauderKill]}) {
		/declare murauderKill int outer 4
	} else /if (${Defined[murauderKill]}) {
		/varset murauderKill 4
	}
	/docommand ${ChatToggle} Spawn must be killed by a >>^<< MELEE >>^<< 
/return

#event MobSet "#*#shudders as if somehow changing.#*#"
SUB event_MobSet
	/if (${murauderKill} < 4) {
		/if (${Defined[murauderKill]}) /varcalc murauderKill ${murauderKill} + 1
	} else /if (${murauderKill} == 4) {
		/if (${Defined[murauderKill]}) /varset murauderKill 1
	}
	/if (${murauderKill} == 1) {
		/docommand ${ChatToggle} Vrex Xalkaks Marauder must be killed by a  >>^<< CASTER >>^<< 
	} else /if (${murauderKill} == 2) {
		/docommand ${ChatToggle} Vrex Xalkaks Marauder must be killed by a >>^<< HYBRID >>^<< 
	} else /if (${murauderKill} == 3) {
		/docommand ${ChatToggle} Vrex Xalkaks Marauder must be killed by a >>^<< PRIEST >>^<< 
	} else /if (${murauderKill} == 4) {
		/docommand ${ChatToggle} Vrex Xalkaks Marauder must be killed by a >>^<< MELEE >>^<< 
	}
/return