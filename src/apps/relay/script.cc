#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "script.h"

bool
CRaceScript::get(CRaceList &aRaceList)
{
	system_log("start script no:%d", mRoot.length());
	int 
		i=0;
	TSomething
		Race;
	
	while((Race = get_array(i++)))
	{
		CRace
			*NewRace;

		NewRace = new CRace();

		TSomething 
			Number = get_section("Number", Race);
		TSomething 
			Description = get_section("Description", Race);

		if (Number)
		{
			system_log("Race %s:%s", get_name(Race), get_data(Number));
			NewRace->initialize(get_name(Race), atoi(get_data(Number)),
					(Description) ? get_data(Description):"<None>");

			aRaceList.push(NewRace);
		}
	}

	system_log("end script");
	return true;
}
