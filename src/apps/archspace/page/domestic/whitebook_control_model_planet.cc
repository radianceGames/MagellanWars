#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageWhitebookControlModelPlanet::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	QUERY("PLANET_ID", PlanetIDString);
	int
		PlanetID = as_atoi(PlanetIDString);
	CPlanet *
		Planet = PLANET_TABLE->get_by_id(PlanetID);

	if (!Planet)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("There is no planet wih ID %1$s"),
								dec2unit(PlanetID)));
		return output("domestic/whitebook_error.html");
	}

	if (Planet->get_owner()->get_game_id() != aPlayer->get_game_id())
	{
		ITEM("ERROR_MESSAGE",
						(char *)format(GETTEXT("You do not own this planet."),
										dec2unit(PlanetID)));
		return output("domestic/whitebook_error.html");
	}

	ITEM("CONTROL_MODEL_TITLE",
			(char *)format(GETTEXT("Control Model on Planet %1$s"), Planet->get_name()));

	CControlModel *
		PlayerCM = aPlayer->get_control_model();
	CControlModel *
		PlanetCM = Planet->get_control_model();

	int
		Diff;

	ITEM("STRING_ENVIRONMENT", GETTEXT("Environment"));

	Diff = PlanetCM->get_environment() - PlayerCM->get_environment();
	if (Diff != 0)
	{
		ITEM("ENVIRONMENT_CM",
				(char *)format("%d (%s)",
								PlanetCM->get_environment(), integer_with_sign(Diff)));
	} else
	{
		ITEM("ENVIRONMENT_CM", PlanetCM->get_environment());
	}

	ITEM("STRING_GROWTH", GETTEXT("Growth"));

	Diff = PlanetCM->get_growth() - PlayerCM->get_growth();
	if (Diff != 0)
	{
		ITEM("GROWTH_CM",
				(char *)format("%d (%s)",
								PlanetCM->get_growth(), integer_with_sign(Diff)));
	} else
	{
		ITEM("GROWTH_CM", PlanetCM->get_growth());
	}


	ITEM("STRING_PRODUCTION", GETTEXT("Production"));

	Diff = PlanetCM->get_production() - PlayerCM->get_production();
	if (Diff != 0)
	{
		ITEM("PRODUCTION_CM",
				(char *)format("%d (%s)",
								PlanetCM->get_production(), integer_with_sign(Diff)));
	} else
	{
		ITEM("PRODUCTION_CM", PlanetCM->get_production());
	}

	ITEM("STRING_MILITARY", GETTEXT("Military"));

	Diff = PlanetCM->get_military() - PlayerCM->get_military();
	if (Diff != 0)
	{
		ITEM("MILITARY_CM",
				(char *)format("%d (%s)",
								PlanetCM->get_military(), integer_with_sign(Diff)));
	} else
	{
		ITEM("MILITARY_CM", PlanetCM->get_military());
	}

	ITEM("STRING_COMMERCE", GETTEXT("Commerce"));

	Diff = PlanetCM->get_commerce() - PlayerCM->get_commerce();
	if (Diff != 0)
	{
		ITEM("COMMERCE_CM",
				(char *)format("%d (%s)",
								PlanetCM->get_commerce(), integer_with_sign(Diff)));
	} else
	{
		ITEM("COMMERCE_CM", PlanetCM->get_commerce());
	}

	ITEM("STRING_DIPLOMACY", GETTEXT("Diplomacy"));

	Diff = PlanetCM->get_diplomacy() - PlayerCM->get_diplomacy();
	if (Diff != 0)
	{
		ITEM("DIPLOMACY_CM",
				(char *)format("%d (%s)",
								PlanetCM->get_diplomacy(), integer_with_sign(Diff)));
	} else
	{
		ITEM("DIPLOMACY_CM", PlanetCM->get_diplomacy());
	}

	ITEM("STRING_EFFICIENCY", GETTEXT("Efficiency"));

	Diff = PlanetCM->get_efficiency() - PlayerCM->get_efficiency();
	if (Diff != 0)
	{
		ITEM("EFFICIENCY_CM",
				(char *)format("%d (%s)",
								PlanetCM->get_efficiency(), integer_with_sign(Diff)));
	} else
	{
		ITEM("EFFICIENCY_CM", PlanetCM->get_efficiency());
	}

	ITEM("STRING_GENIUS", GETTEXT("Genius"));

	Diff = PlanetCM->get_genius() - PlayerCM->get_genius();
	if (Diff != 0)
	{
		ITEM("GENIUS_CM",
				(char *)format("%d (%s)",
								PlanetCM->get_genius(), integer_with_sign(Diff)));
	} else
	{
		ITEM("GENIUS_CM", PlanetCM->get_genius());
	}

	ITEM("STRING_FACILITY_COST", GETTEXT("Facility Cost"));

	Diff = PlanetCM->get_facility_cost() - PlayerCM->get_facility_cost();
	if (Diff != 0)
	{
		ITEM("FACILITY_COST_CM",
				(char *)format("%d (%s)",
								PlanetCM->get_facility_cost(), integer_with_sign(Diff)));
	} else
	{
		ITEM("FACILITY_COST_CM", PlanetCM->get_facility_cost());
	}

	ITEM("STRING_SPY", GETTEXT("Spy"));

	Diff = PlanetCM->get_spy() - PlayerCM->get_spy();
	if (Diff != 0)
	{
		ITEM("SPY_CM",
				(char *)format("%d (%s)",
								PlanetCM->get_spy(), integer_with_sign(Diff)));
	} else
	{
		ITEM("SPY_CM", PlanetCM->get_spy());
	}

	ITEM("STRING_RESEARCH", GETTEXT("Research"));

	Diff = PlanetCM->get_research() - PlayerCM->get_research();
	if (Diff != 0)
	{
		ITEM("RESEARCH_CM",
				(char *)format("%d (%s)",
								PlanetCM->get_research(), integer_with_sign(Diff)));
	} else
	{
		ITEM("RESEARCH_CM", PlanetCM->get_research());
	}

//	system_log("end page handler %s", get_name());

	return output("domestic/whitebook_control_model_planet.html");
}
