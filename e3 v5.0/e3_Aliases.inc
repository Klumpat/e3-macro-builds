|--------------------------------------------------------------------|
|- e3_Aliases.inc v1.0	( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ										-|
|--------------------------------------------------------------------|
|- A universal place to /call alias updates from various includes.	-|
|--------------------------------------------------------------------|



|------------------------------------------------------------|
|- Deletes and Recreates aliases associciated with e3.mac.	-|
|------------------------------------------------------------|
#EVENT updateAliases "[MQ2] Update macro aliases"
SUB EVENT_updateAliases
/if (${Debug}) /echo |- EVENT_updateAliases ==>

	/echo Updating macro aliases...
	
	/squelch /alias /updateAliases delete
	/alias /updateAliases /echo Update macro aliases
	/call assist_Aliases
	/call background_Aliases
	/call basics_Aliases
	/call buffCheck_Aliases
	/call casting_Aliases
	/call heal_Aliases
	/call pet_Aliases
	/call BRD_Aliases
	/call BST_Aliases
	/call BER_Aliases
	/call CLR_Aliases
	/call DRU_Aliases
	/call ENC_Aliases
	/call MAG_Aliases
	/call MNK_Aliases
	/call NEC_Aliases
	/call PAL_Aliases
	/call RNG_Aliases
	/call ROG_Aliases
	/call SHD_Aliases
	/call SHM_Aliases
	/call WAR_Aliases
	/call WIZ_Aliases
	
	/call VetAAs_Aliases

/if (${Debug}) /echo <== EVENT_updateAliases -|
/RETURN