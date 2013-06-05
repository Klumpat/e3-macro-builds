#EVENT macroHelp "<#*#> Help #1#,#2#"
SUB EVENT_macroHelp(line, category, topic)
/if (${Debug}) /echo |- EVENT_macroHelp ==>

	/if (!${Ini[e3 Help.ini,${category},${topic}].Length}) {
		/echo Could not find [${category},${topic}].  Use "/e3help list" for a list of help topics.
	} else {
		/echo ${Ini[e3 Help.ini,${category},${topic}]}
	}

/if (${Debug}) /echo <== EVENT_macroHelp -|
/RETURN

#EVENT listTopics "<#*#> Help List"
SUB EVENT_listTopics

/RETURN

/if (!${category.Length}) {
		/echo Please include the category, and topic, you need help with.
		/echo
		/echo Help topics:
		/echo
		
		:nextCategory
		/if (${Ini[e3 Help.ini].Arg[${i},|].Length}) {
			/echo ${Ini[e3 Help.ini].Arg[${i},|]}
			/varcalc i ${i} + 1
			/goto :nextCategory
		}
		
	} else /if (${category.Length} && !${topic.Length}) {
		/echo Explain category / topic selection HERE ***
		:nextTopic
		/if (${Ini[e3 Help.ini,${category}].Arg[${i},|].Length}) {
			/echo ${Ini[e3 Help.ini,${category}].Arg[${i},|]}
			/varcalc i ${i} + 1
			/goto :nextTopic
		}

	} else /if (${category.Length} && ${topic.Length}) {

	}

