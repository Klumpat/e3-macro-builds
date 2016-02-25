# Getting Started #

So, you've decided you want to start using Killians's macro. Maybe you've used MQ2 before, maybe you haven't. Either way, this guide will tell you how to get MQ2 and Killians's macro set up for yor characters, assumig you know nothing about MQ2 at all.

First things first. You've already downloaded **_KilliansMQ2.zip_** and the current version of e3 from Killians's Google code page at http://code.google.com/p/e3-macro-builds/downloads/list. Any .inc file you see, normally formatted as `e3_Classes_ClassName.inc`, for example, is an updated version of a macro file. Download all of those as well.

# Putting Things in Their Place #
A note about the MQ2 folder structure, for clarity's sake:
> Inside the KilliansMQ2 folder you'll see a bunch of files and two folders. The files are the plugins (.dll's) that MacroQuest2.exe uses when it runs. The plugins all do specific 	things; MQ2Melee handles melee positioning, using skills, AAs, and whatnot, that are related to combat. MQ2EQBC (MQ2EQBotChat)is a plugin that makes it possible for 	your characters to communicate with each other through a lightweight server 	program you run on your computer, instead of through a server hosted somewhere else. And so on.

> There are a few .ini files here as well. You'll see .ini files for your characters (PEQTGC\_CharName.ini) after you load MQ2. Those will be set up for you by the macro. Of note are Macroquest.ini and MQ2HUD.ini. Macroquest.ini is where you can configure custom aliases, / commands that you create using the /alias command in-game (/alias 	/commandname /command, Ex: /alias /iwannadance /dance). MQ2HUD allows you to 	load custom HUDs into the game window. Use this to display more information about whatever MQ2 has access to. HUDs can be very simple, HUDs can be very complex. If you're curious, then google some MQ2 huds and rework them to your liking. They're a bit too complex for the scope of this readme and, in this case as in most, doin' is learnin'.

> The two subfolders here are self-explanatory. "UIFiles" has UI files. "Macros" is where you will put all of your macros and their required files, UNLESS the requires file is a plugin. Badaboom, badabing.

> Open KilliansMQ2.zip and put the folder where you want it to stay, renaming it if you're picky like that. Open the e3.zip and place all of its contents into the “Macros” folder in your MQ2 folder. Any include or other macro file you've downloaded, place inside the Macros folder as well, overwriting the older version.

# Loading Up #

Go to the main MQ2 folder and start MacroQues2.exe AND eqbcs.exe. Load EQ however you normally do. When you've loaded to character selection you'll see a new window that likely blocks part of your character and another smaller one in the top left corner simply titled "MQ". You're in business. Now load into the world.
Once you're in you'll see that the changelog window has thankfully disappeared, but the MQ window hasn't. Don't close that. You have a new friend to live with. Put him somewhere you can see, but somewhere that won't annoy you.
Now to get the macro running you have to type "/mac e3" on each character you'll use it on. The macro will put the appropriate settings into your character's PEQTC\_CharacterName.ini files, so you can keep on ignoring those.
The most important step now is setting up character .ini files. e3 sets up your macro folder in the following way: the macro itself and all of its little buddies  are in the "Macros" folder. Don't mess with these unless you like breaking things or know what you're doing. The "Bot\_Inis" subfolder holds your configuration files for your characters, "Macro\_Inis" subfolder holds the configuration files for the macro itself. "Read Mes" has readmes.

A Micro Macro Overview
When e3 is fully set up, a group (or groups) engage a mob by one of two ways. 1) A character runs up to the mob and hits autoattack. The mob's hp gets lowered to a certain percent, which then causes the attacking character to issue “/bc Assist on ${Target.ID}”. 2) A character targets a mob and issues the same command.
In both cases all characters running e3 that are in zone and in assist range will assist on the target mob, as definied in ther character .inis. For melee's with [Assist\_Stick](Assist_Stick.md) settings that means running and hitting. For casters, not so much. It is important to note that the character issuing the command to assist does not follow the stick commands in the character's .ini, but it will do everything else in there. This character becomes the “master” for that fight. So, if you want to play your cleric as your main, you can call assists by the above command (Mmm, hotkey). If you want to play your warrior or any other melee as your main, you can just run in and punch mobs in the nose. If you want to switch characters each fight so no one feels neglected, you're a hippy.
The macro lets you set how far your characters are from the mob they're attacking, what spells they cast, if they debuff on assist or by command and with what spells, what buffs they cast, what buffs they'll always keep up on themselves, and some more stuff, too. But first you have to tell the macro what all those things are.

Setting Up Your Characters
To have the macro actually do things, you'll have to set up configuration files for all of your characters. To do this, open Bot\_inis in the Macros folder. Open whatever .ini for whichever character you want to set up first. Each setting that you'll write to this file will have its own line, as this partial heal section demonstrates thusly:

BAD:
> [Heals](Heals.md)
> MainTank=TankdudeMainTank\_Heal(Spell/Gem/Pct)=Complete Heal/gem3/60
GOOD:
> [Heals](Heals.md)
> MainTank=Tankdude
> MainTank\_Heal(Spell/Gem/Pct)=Complete Heal/gem3/60

And so on. The formating for most of the .inis will follow the same pattern of "Spell,AA,Ability,Disc/Gem #,AA, Ability, Disc/Percent HP or Mana" (% of target or bot; varies by what the line sets up, obviously). An ability line will usually look like this:

> [LifeSupport](LifeSupport.md)
> LifeSupport#1=Feign Death/ability/30

The [LifeSupport](LifeSupport.md) section of your .inis are things characters will do to not die, hence the name. Each section's name explains what the section does.
Setting up class sections is usually simpler. In the class section of a cleric's .ini, it only calls for a number on the lines:

> [Cleric](Cleric.md)
> CelestialRegeneration\_Pct=55
> DivineArbitration\_Pct=32

So for those, only put the percents, as above, that you want your cleric to use those abilities at. In other instances, such as the Nukes section, you'll have to add a bit more:

> [Nukes](Nukes.md)
> Nuke#1=Ancient: Strike of Chaos/gem1/0m/6s

This line is saying a few things. Reading left from right we have the spell name ("Ancient: Strike of Chaos"), what gem it's memmed in ("gem1"), to what level of mana the wizard should cast it ("0m"), meaing until he's out of mana, and the delay between casts ("6s"), which means six seconds.
In the [Buffs](Buffs.md) section you'll notice three categories of buffs, SelfBuff,  GroupBuff, BuffOther, and CombatBuff. SelfBuffs are just that, self buffs. The default setting for this section is spells, so you do not need to add a gem number or identify it as a spell. Items get the /item tag after their name.
GroupBuff is what the character will cast when the /bc Buff me, or its variants are issued. BuffOther is what buffs the character will keep up on other characters. CombatBuff is what the character will cast on other bots during combat.

Assisting and Stick
Now let's look at the [Assist\_Stick](Assist_Stick.md) section. If the macro sees this section and it's filled out, the character will move into combat when assist is called by another character. We have two main settings:

> [Assist\_Stick](Assist_Stick.md)
> StickPoint=behind
> StickDistance=10

Stick point is where on the mob the character will stay while everyone is attacking.  You can set this to behind, front, pin, behindonce, !front. These are explained at http://www.macroquest2.com/wiki/index.php/MQ2MoveUtils. Distance is just the distance your guys are away from their target. You can set this at MaxMelee or a number. 5 is very close, 10 is pretty close, and so it goes.
That covers most sections you'll have to fill out, so you should have a clear idea of how to set up these files for your characters. If you're confused about syntax, reference the Sample Inis. If something doesn't work, check to make sure the line is formated correctly.
The sections explain what they do pretty well. Follow the pattern and you'll be fine.

Fine Tuning Important General Macro Settings
Our last stop is the e3\_Settings.ini in the Macro\_Inis folder. This file has a bunch of stuff you can mostly likely ignore. But of special importance are two settings you may want to change, under [Settings](Settings.md):

> Default\_SpellSet=${Me.CleanName}Group
> Default\_SpellGem=9

The first controls what spell set your characters will load when the die or when the /bc load spells command is issued.  The line ${Me.CleanName}Group is MQ2speak for "My toon's name"Group (Ex: ClericdudeGroup). On each character you should save your spell bars how you want them under a common format since each will then load whatever spell bar you save after. Default\_SpellGem sets what gem the character will use for buff requests or to cast spells that he might have in his character .ini but not memmed. Set this to whatever spell gem you need to have memmed the least, so your cleric isn't unmemming his main heal to cast virtue in the middle of a fight.

Getting Tha Monies
If you wish to have a character loot everything you kill, you'll also set that variable here.
> [NinjaLoot](NinjaLoot.md)
> Use\_NinjaLoot.inc=On
> NinjaLoot\_Looters=Lootwhore
> NinjaLoot\_Alert=3
> Seek\_Radius=35

The first line turns looting on or off. The second names the character or characters that loot. Adding more than one is done thusly:

> NinjaLoot\_Looters=Lootwhore,Lootsalot

The last setting changes what chat channel the character announces no drop loot or items he can't loot. The default, 3, is group.

Spell Aliases
Back in the Macro\_Inis folder, you'll also want to look at SpellAliases.ini. Each subsection is for a classs, each entry lists what text will cast what spell. For example, under [Enchanter](Enchanter.md):

> VoQ=Voice Of Quellious
> could be changed to
> DanceDanceYouChickenLover=Voice of Quellious

Then, whenever an enchanter you're running e3 on has someone tell them to dance while calling them a chicken lover, without any punctuation as above, your toon would cast Voice of Quellious on this stranger who has an odd way of discussing another person's interests. You could also just leave it as VoQ. That probably makes more sense.

And that does it! Once you've done all of that for each character you wish to use the macro on, you should be able to go out and slayeth thy prey. There are some more nuances to the macro, most of which have to do with the many, many commands that come with it...
But that's why there's Slash Commands.txt in the Readme folder. Read it. Love it. Respect it.
Have fun!