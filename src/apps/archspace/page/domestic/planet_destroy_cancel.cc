#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../planet.h"

bool
CPagePlanetDestroyCancel::handler(CPlayer *aPlayer)
{
	CPlanetList *
		PlanetList = aPlayer->get_planet_list();
	
	QUERY("PLANET_ID", PlanetIDString);

	int PlanetID = as_atoi(PlanetIDString);
	
	CPlanet *Planet = aPlayer->get_planet(PlanetID);
	CHECK(!Planet,
			(char *)format(GETTEXT("You don't have a planet with ID %1$s."),dec2unit(PlanetID)));
	
	ITEM("CONFIRM_MESSAGE", GETTEXT("The Planet's destruction was canceled"));
		
	PlanetList->cancel_destroy_planet(PlanetID);
	return output("domestic/planet_destroy_cancel.html");

	
}
