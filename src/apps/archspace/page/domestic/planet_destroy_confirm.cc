#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../planet.h"

bool
CPagePlanetDestroyConfirm::handler(CPlayer *aPlayer)
{
	CPlanetList *
		PlanetList = aPlayer->get_planet_list();
	
	QUERY("PLANET_ID", PlanetIDString);
	QUERY("REMOVE", RemoveIDString);

	int PlanetID = as_atoi(PlanetIDString);
	int RemoveID = as_atoi(RemoveIDString);
	
	CPlanet *Planet = aPlayer->get_planet(PlanetID);
	CHECK(!Planet,
			(char *)format(GETTEXT("You don't have a planet with ID %1$s."),dec2unit(PlanetID)));
	
	CPlanet *
		HomePlanet = (CPlanet *)PlanetList->get(0);
	
	if ( HomePlanet->get_id() != PlanetID ) {

		if(RemoveID==1)
		{
			ITEM("CONFIRM_MESSAGE", GETTEXT("Planet will be removed in 24 hours"));
		
			PlanetList->add_destroy_planet(PlanetID);
			return output("domestic/planet_destroy_result.html");
		}
		else
		{
			ITEM("PLANET_ID", GETTEXT(PlanetIDString));
			ITEM("CONFIRM_MESSAGE",
				(char *)format(GETTEXT("If you destroy this planet you will not be able to recover it. Are you sure you want to destroy planet %1$s?"), dec2unit(PlanetID)));
			ITEM("REMOVE", GETTEXT("1"));
				return output("domestic/planet_destroy_confirm.html");
		}
	} 
	else
	{
		ITEM("CONFIRM_MESSAGE", GETTEXT("Your home planet cannot be destroyed"));
		return output("domestic/planet_destroy_result.html");
	}
	
}
