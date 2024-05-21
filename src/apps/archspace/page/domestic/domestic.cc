#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageDomestic::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	long long int
		TempInt;

	static CString
		ProjectName;
	ProjectName.clear();

	static CString
		ControlModelStatus;
	ControlModelStatus.clear();

	CPlanetList *
		PlanetList = aPlayer->get_planet_list();
	CPlanet *
		HomePlanet = (CPlanet *)PlanetList->get(0);
	if (!HomePlanet)
	{
		message_page("You have no planet. Ask ARCHSPACE Development Team.");
		return true;
	}

	ITEM("HOME_PLANET_ICON", HomePlanet->get_planet_image_url());
	ITEM("STRING_HOME_PLANET", GETTEXT("Home Planet"));
	ITEM("HOME_PLANET",
			(char *)format("<A HREF=\"planet_detail.as?PLANET_ID=%d\">%s</a>",
							HomePlanet->get_id(), HomePlanet->get_name()));

	ITEM("STRING_NUMBER_OF_PLANET_S_", GETTEXT("Number of Planet(s)"));
	ITEM("NUMBER_OF_PLANET_S_", PlanetList->length());

	ITEM("STRING_CONCENTRATION_MODE", GETTEXT("Concentration Mode"));
	ITEM("CONCENTRATION_MODE_NAME", aPlayer->get_mode_name());

	ITEM("STRING_POPULATION", GETTEXT("Population"));
	ITEM("DETAIL_POPULATION",
			(char *)format(GETTEXT("%1$s k"), dec2unit(aPlayer->calc_population())));

	ITEM("STRING_PRODUCTION_POINT", GETTEXT("Production Point"));
	ITEM("PP",
			(char *)format(GETTEXT("%1$s PP"), dec2unit(aPlayer->get_production())));

	ITEM("STRING_CONTROL_MODEL", GETTEXT("Control Model"));

	CControlModel *
		ControlModel = aPlayer->get_control_model();

	ITEM("STRING_ENVIRONMENT", GETTEXT("Environment"));
	ITEM("ENVIRONMENT_CM", ControlModel->get_value(CControlModel::CM_ENVIRONMENT));

	ITEM("STRING_GROWTH", GETTEXT("Growth"));
	ITEM("GROWTH_CM", ControlModel->get_value(CControlModel::CM_GROWTH));

	ITEM("STRING_RESEARCH", GETTEXT("Research"));
	ITEM("RESEARCH_CM", ControlModel->get_value(CControlModel::CM_RESEARCH));

	ITEM("STRING_PRODUCTION", GETTEXT("Production"));
	ITEM("PRODUCTION_CM", ControlModel->get_value(CControlModel::CM_PRODUCTION));

	ITEM("STRING_MILITARY", GETTEXT("Military"));
	ITEM("MILITARY_CM", ControlModel->get_value(CControlModel::CM_MILITARY));

	ITEM("STRING_SPY", GETTEXT("Spy"));
	ITEM("SPY_CM", ControlModel->get_value(CControlModel::CM_SPY));

	ITEM("STRING_COMMERCE", GETTEXT("Commerce"));
	ITEM("COMMERCE_CM", ControlModel->get_value(CControlModel::CM_COMMERCE));

	ITEM("STRING_EFFICIENCY", GETTEXT("Efficiency"));
	ITEM("EFFICIENCY_CM", ControlModel->get_value(CControlModel::CM_EFFICIENCY));

	ITEM("STRING_GENIUS", GETTEXT("Genius"));
	ITEM("GENIUS_CM", ControlModel->get_value(CControlModel::CM_GENIUS));

	ITEM("STRING_DIPLOMACY", GETTEXT("Diplomacy"));
	ITEM("DIPLOMACY_CM", ControlModel->get_value(CControlModel::CM_DIPLOMACY));

	ITEM("STRING_FACILITY_COST", GETTEXT("Facility Cost"));
	ITEM("FACILITY_COST_CM", ControlModel->get_value(CControlModel::CM_FACILITY_COST));

	ITEM("STRING_BUILDINGS", GETTEXT("Buildings"));

	int
		Factory = 0,
		ResearchLab = 0,
		MilitaryBase = 0;

	for (int i=0 ; i<PlanetList->length() ; i++)
	{
		CPlanet *
			Planet = (CPlanet *)PlanetList->get(i);
		CResource
			Building = Planet->get_building();

		Factory += Building.get(BUILDING_FACTORY);
		ResearchLab += Building.get(BUILDING_RESEARCH_LAB);
		MilitaryBase += Building.get(BUILDING_MILITARY_BASE);
	}

	ITEM("STRING_FACTORY", GETTEXT("Factory"));
	ITEM("FACTORY", dec2unit(Factory));

	ITEM("STRING_RESEARCH_LAB", GETTEXT("Research Lab."));
	ITEM("RESEARCH_LAB", dec2unit(ResearchLab));

	ITEM("STRING_MILITARY_BASE", GETTEXT("Military Base"));
	ITEM("MILITARY_BASE", dec2unit(MilitaryBase));

	ITEM("STRING_BALANCE_SHEET", GETTEXT("Balance Sheet"));

	CResource
		BuildingUpkeep,
		IncomeByPlanet;
	int
		ProductionPointByCommerce = 0,
		ProductionPointByRace = 0,
		ProductionPointByPayingFleetUpkeep = 0,
		ResearchPointByEffect = 0,
		MilitaryPointByFortress = 0,
		MilitaryPointByEffect = 0;
	int
		LosingProductionPointByEffect = 0,
		LosingProductionPointBySecurity = 0,
		LosingProductionPointByBuildingUpkeep = 0,
		LosingProductionPointByShipProduction = 0,
		LosingProductionPointByPayingFleetUpkeep = 0,
		LosingProductionPointByRepairing = 0,
		LosingProductionPointByCouncilTax = 0,
		LosingResearchPointByEffect = 0,
		LosingMilitaryPointByEffect = 0,
		LosingMilitaryPointByPayingFleetUpkeep = 0,
		LosingMilitaryPointByRepairing = 0;

	for (int i=0 ; i<PlanetList->length() ; i++)
	{
		CPlanet *
			Planet = (CPlanet *)PlanetList->get(i);
		if (Planet->is_paralyzed() == true) continue;

		IncomeByPlanet.change(RESOURCE_PRODUCTION, Planet->get_pp_per_turn());
		IncomeByPlanet.change(RESOURCE_MILITARY, Planet->get_mp_per_turn());
		IncomeByPlanet.change(RESOURCE_RESEARCH, Planet->get_rp_per_turn());

		int
			CommerceIncome = Planet->calc_commerce();
		if (MAX_PLAYER_PP - ProductionPointByCommerce < CommerceIncome)
		{
			ProductionPointByCommerce = MAX_PLAYER_PP;
		}
		else
		{
			ProductionPointByCommerce += CommerceIncome;
		}

		BuildingUpkeep.change(BUILDING_FACTORY, Planet->get_upkeep_per_turn(BUILDING_FACTORY));
		BuildingUpkeep.change(BUILDING_MILITARY_BASE, Planet->get_upkeep_per_turn(BUILDING_MILITARY_BASE));
		BuildingUpkeep.change(BUILDING_RESEARCH_LAB, Planet->get_upkeep_per_turn(BUILDING_RESEARCH_LAB));
	}

	int
		NewProductionPoint;
	TempInt = (long long int)IncomeByPlanet.get(RESOURCE_PRODUCTION) + (long long int)ProductionPointByCommerce;
	if (TempInt > (long long int)MAX_PLAYER_PP)
	{
		NewProductionPoint = MAX_PLAYER_PP;
	}
	else
	{
		NewProductionPoint = (int)TempInt;
	}
	int
		NewMilitaryPoint =
				IncomeByPlanet.get(RESOURCE_MILITARY),
		NewResearchPoint =
				IncomeByPlanet.get(RESOURCE_RESEARCH);

	if (aPlayer->has_effect(CPlayerEffect::PA_PRODUCE_MP_PER_TURN) == true)
	{
		int
			NewMilitaryPointByEffect = aPlayer->calc_PA(
					NewMilitaryPoint, CPlayerEffect::PA_PRODUCE_MP_PER_TURN);
		//MilitaryPointByEffect += NewMilitaryPointByEffect - NewMilitaryPoint;

		NewMilitaryPoint = NewMilitaryPointByEffect;
	}

	if (aPlayer->has_effect(CPlayerEffect::PA_PRODUCE_RP_PER_TURN) == true)
	{
		int
			NewResearchPointByEffect = aPlayer->calc_PA(
				NewResearchPoint, CPlayerEffect::PA_PRODUCE_RP_PER_TURN);
		//ResearchPointByEffect += NewResearchPointByEffect - NewResearchPoint;

		NewResearchPoint = NewResearchPointByEffect;
	}

#define SPACE_MINING_MODULE		5525
	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	CKnownTechList *
		KnownTechList = aPlayer->get_tech_list();
	for (int i=0 ; i<FleetList->length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)FleetList->get(i);

		for (int i=0 ; i<DEVICE_MAX_NUMBER ; i++)
		{
			if (Fleet->get_device(i) == SPACE_MINING_MODULE)
			{
				int
					NumberOfTech = KnownTechList->count_by_category(CTech::TYPE_MATTER_ENERGY);
				ProductionPointByRace +=
						(int)(Fleet->get_size() * Fleet->get_size() * (100 + NumberOfTech) * Fleet->get_current_ship() / 50);

				break;
			}
		}
	}
	NewProductionPoint += ProductionPointByRace;
#undef SPACE_MINING_MODULE

	if (aPlayer->has_effect(CPlayerEffect::PA_CONSUME_PP_PER_TURN) == true)
	{
		int
			NewProductionPointByEffect = aPlayer->calc_minus_PA(
					NewProductionPoint, CPlayerEffect::PA_CONSUME_PP_PER_TURN);
		LosingProductionPointByEffect += NewProductionPoint - NewProductionPointByEffect;

		NewProductionPoint = NewProductionPointByEffect;
	}

	if (aPlayer->has_effect(CPlayerEffect::PA_IMPERIAL_RETRIBUTION) == true)
	{
		LosingProductionPointByEffect += aPlayer->get_production()/2;
		LosingResearchPointByEffect += aPlayer->get_research()/2;
		LosingMilitaryPointByEffect += NewMilitaryPoint/2;

		NewMilitaryPoint -= NewMilitaryPoint/2;
	}

	int
		CurrentProductionPoint = NewProductionPoint;
	int
		ReservedPP = aPlayer->get_production();

	LosingProductionPointBySecurity += CSpy::get_security_upkeep(
			aPlayer->get_security_level(), CurrentProductionPoint);
	CurrentProductionPoint -= LosingProductionPointBySecurity;

	LosingProductionPointByBuildingUpkeep += (BuildingUpkeep.get(BUILDING_FACTORY) + BuildingUpkeep.get(BUILDING_MILITARY_BASE) + BuildingUpkeep.get(BUILDING_RESEARCH_LAB))/10;
	CurrentProductionPoint -= LosingProductionPointByBuildingUpkeep;

	int
		ShipProduction =
				aPlayer->get_new_resource()->get(RESOURCE_PRODUCTION)*(30+aPlayer->get_control_model()->get_military()*5)/100;
	if (ShipProduction < 0) ShipProduction = 0;

	LosingProductionPointByShipProduction += ShipProduction;
	CurrentProductionPoint -= LosingProductionPointByShipProduction;

	aPlayer->calc_all_ships_upkeep(&ReservedPP, CurrentProductionPoint, &NewMilitaryPoint, &ProductionPointByPayingFleetUpkeep, &LosingProductionPointByPayingFleetUpkeep, &LosingMilitaryPointByPayingFleetUpkeep);

	aPlayer->simulate_repair_damaged_ship(&ReservedPP, &NewMilitaryPoint, &LosingProductionPointByRepairing, &LosingMilitaryPointByRepairing);

	if (CurrentProductionPoint > 100)
	{
		LosingProductionPointByCouncilTax = CurrentProductionPoint * 5 / 100;
		CurrentProductionPoint -= LosingProductionPointByCouncilTax;
	}

	int
		TotalProductionPointPerTurn,
		TotalMilitaryPointPerTurn,
		TotalResearchPointPerTurn,
		TotalPPUpkeep,
		TotalMPUpkeep;

	TempInt = (long long int)IncomeByPlanet.get(RESOURCE_PRODUCTION) + (long long int)ProductionPointByCommerce + (long long int)ProductionPointByRace + (long long int)ProductionPointByPayingFleetUpkeep;
	if (TempInt > (long long int)MAX_PLAYER_PP)
	{
		TotalProductionPointPerTurn = MAX_PLAYER_PP;
	}
	else
	{
		TotalProductionPointPerTurn = (int)TempInt;
	}

	//TempInt = (long long int)IncomeByPlanet.get(RESOURCE_MILITARY) + (long long int)NewMilitaryPoint;
	TempInt = NewMilitaryPoint;
	if (TempInt > (long long int)MAX_PLAYER_MP)
	{
		TotalMilitaryPointPerTurn = MAX_PLAYER_MP;
	}
	else
	{
		TotalMilitaryPointPerTurn = (int)TempInt;
	}

	TempInt = (long long int)IncomeByPlanet.get(RESOURCE_RESEARCH) + (long long int)ResearchPointByEffect;
	if (TempInt > (long long int)MAX_PLAYER_PP)
	{
		TotalResearchPointPerTurn = MAX_PLAYER_PP;
	}
	else
	{
		TotalResearchPointPerTurn = (int)TempInt;
	}

	TempInt = (long long int)LosingProductionPointByBuildingUpkeep + (long long int)LosingProductionPointByPayingFleetUpkeep;
	if (TempInt > (long long int)MAX_PLAYER_PP)
	{
		TotalPPUpkeep = MAX_PLAYER_PP;
	}
	else
	{
		TotalPPUpkeep = (int)TempInt;
	}

	TotalMPUpkeep = LosingMilitaryPointByPayingFleetUpkeep;

	int
		TotalPPSpending,
		TotalMPSpending;

	TempInt = (long long int)LosingProductionPointByEffect + (long long int)LosingProductionPointBySecurity + (long long int)LosingProductionPointByShipProduction + (long long int)LosingProductionPointByRepairing + (long long int)LosingProductionPointByCouncilTax;
	if (TempInt > (long long int)MAX_PLAYER_PP)
	{
		TotalPPSpending = MAX_PLAYER_PP;
	}
	else
	{
		TotalPPSpending = (int)TempInt;
	}

	TempInt = (long long int)LosingMilitaryPointByEffect + (long long int)LosingMilitaryPointByRepairing;
	if (TempInt > (long long int)MAX_PLAYER_PP)
	{
		TotalMPSpending = MAX_PLAYER_PP;
	}
	else
	{
		TotalMPSpending = (int)TempInt;
	}

	ITEM("STRING_PRODUCTION", GETTEXT("Production"));
	ITEM("PRODUCTION",
			(char *)format("%s PP", dec2unit(TotalProductionPointPerTurn)));

	ITEM("STRING_UPKEEP", GETTEXT("Upkeep"));
	ITEM("UPKEEP",
			(char *)format(GETTEXT("%1$s PP"), dec2unit(-TotalPPUpkeep)));

	ITEM("STRING_SPENDING", GETTEXT("Spending"));

	ITEM("SPENDING",
			(char *)format(GETTEXT("%1$s PP"), dec2unit(-TotalPPSpending)));

	ITEM("STRING_NET_INCOME", GETTEXT("Net Income"));
	int
		NetIncome = TotalProductionPointPerTurn - TotalPPUpkeep - TotalPPSpending;
	ITEM("NET_INCOME",
			(char *)format(GETTEXT("%1$s PP"), dec2unit(NetIncome)));

	ITEM("STRING_PRODUCTION_PER_TURN", GETTEXT("Production per Turn"));
	ITEM("STRING_MILITARY_POINT", GETTEXT("Military Point"));
	ITEM("STRING_COMMERCE_PP", GETTEXT("Commerce"));
	ITEM("STRING_RESEARCH_POINT", GETTEXT("Research Point"));
	ITEM("STRING_TOTAL", GETTEXT("Total"));

	ITEM("PRODUCTION_POINT",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(TotalProductionPointPerTurn - ProductionPointByCommerce)));
	ITEM("MILITARY_POINT",
			(char *)format(GETTEXT("%1$s MP"),
							dec2unit(TotalMilitaryPointPerTurn)));
	ITEM("COMMERCE_PP",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(ProductionPointByCommerce)));
	ITEM("RESEARCH_POINT",
			(char *)format(GETTEXT("%1$s RP"),
							dec2unit(TotalResearchPointPerTurn)));

	ITEM("STRING_UPKEEP", GETTEXT("Upkeep"));

	ITEM("PRODUCTION_POINT_UPKEEP",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(TotalPPUpkeep)));

	ITEM("MILITARY_POINT_UPKEEP",
			(char *)format(GETTEXT("%1$s MP"),
							dec2unit(TotalMPUpkeep)));

	ITEM("FACTORY_UPKEEP",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(BuildingUpkeep.get(BUILDING_FACTORY)/10)));
	ITEM("MILITARY_BASE_UPKEEP",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(BuildingUpkeep.get(BUILDING_MILITARY_BASE)/10)));
	ITEM("RESEARCH_LAB_UPKEEP",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(BuildingUpkeep.get(BUILDING_RESEARCH_LAB)/10)));

	ITEM("STRING_FLEET_SHIP_UPKEEP_PP_", GETTEXT("Fleet/Ship Upkeep"));
	ITEM("FLEET_SHIP_UPKEEP_PP_",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(LosingProductionPointByPayingFleetUpkeep)));

	ITEM("STRING_FLEET_SHIP_UPKEEP_MP_", GETTEXT("Fleet/Ship Upkeep"));
	ITEM("FLEET_SHIP_UPKEEP_MP_",
			(char *)format(GETTEXT("%1$s MP"),
							dec2unit(LosingMilitaryPointByPayingFleetUpkeep)));

	ITEM("STRING_SPENDING", GETTEXT("Spending"));

	ITEM("PRODUCTION_POINT_SPENDING",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(TotalPPSpending)));

	ITEM("MILITARY_POINT_SPENDING",
			(char *)format(GETTEXT("%1$s MP"),
							dec2unit(TotalMPSpending)));

	ITEM("STRING_SHIP_POOL", GETTEXT("Ship Pool"));
	ITEM("SHIP_POOL",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(LosingProductionPointByShipProduction)));

	ITEM("STRING_REPAIRING_PP_", GETTEXT("Repairing"));
	ITEM("REPAIRING_PP_",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(LosingProductionPointByRepairing)));

	ITEM("STRING_REPAIRING_MP_", GETTEXT("Repairing"));
	ITEM("REPAIRING_MP_",
			(char *)format(GETTEXT("%1$s MP"),
							dec2unit(LosingMilitaryPointByRepairing)));

	ITEM("STRING_SECURITY",
			GETTEXT("Security"));
	ITEM("SECURITY",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(LosingProductionPointBySecurity)));

	ITEM("STRING_COUNCIL_TAX", GETTEXT("Council Tax"));
	ITEM("COUNCIL_TAX",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(LosingProductionPointByCouncilTax)));

	ITEM("STRING_ACHIEVED_PROJECT_S_", GETTEXT("Achieved Project(s)"));

	CPurchasedProjectList *
		ProjectList = aPlayer->get_purchased_project_list();
	if (!ProjectList->length())
	{
		ITEM("ACHIEVED_PROJECT",
				GETTEXT("You don't have any projects yet."));
	}
	else
	{
		bool
			Comma = false;
		for (int i=0 ; i<ProjectList->length() ; i++)
		{
			CPurchasedProject *
				Project = (CPurchasedProject *)ProjectList->get(i);

			if (Comma) ProjectName += ", ";
			ProjectName += Project->get_name();

			Comma = true;
		}
		ITEM("ACHIEVED_PROJECT", (char *)ProjectName);
	}

	ITEM("STRING_ACHIEVED_TECH_S_", GETTEXT("Achieved Tech(s)"));
	ITEM("STRING_SOCIAL_SCIENCE", GETTEXT("Social Science"));
	ITEM("STRING_MATTER_ENERGY_SCIENCE", GETTEXT("Matter-Energy Science"));
	ITEM("STRING_INFO_SCIENCE", GETTEXT("Info Science"));
	ITEM("STRING_LIFE_SCIENCE", GETTEXT("Life Science"));

	CKnownTechList *
		TechList = aPlayer->get_tech_list();

	ITEM("SOCIAL_SCIENCE", TechList->count_by_category(CTech::TYPE_SOCIAL));
	ITEM("MATTER_ENERGY_SCIENCE",
			TechList->count_by_category(CTech::TYPE_MATTER_ENERGY));
	ITEM("INFO_SCIENCE", TechList->count_by_category(CTech::TYPE_INFORMATION));
	ITEM("LIFE_SCIENCE", TechList->count_by_category(CTech::TYPE_LIFE));
	ITEM("STRING_CURRENTLY_RESEARCHING", GETTEXT("Currently Researching"));

	int
		TargetTechID = aPlayer->get_target_tech();
	if (!TargetTechID)
	{
		ITEM("TARGET_TECH", GETTEXT("Free Research"));
	} else
	{
		CTech *
			TargetTech = TECH_TABLE->get_by_id(TargetTechID);

		if (TargetTech)
		{
			ITEM("TARGET_TECH", TargetTech->get_name());
		} else
		{
			message_page("Wrong target tech.");
			return true;
		}
	}

	ITEM("CONCENTRATION_MODE_EXPLAIN",
			GETTEXT("You can set your concentration mode."));
	ITEM("PLANET_MANAGEMENT_EXPLAIN",
			GETTEXT("You can see the status of your planets and manage them."));
	ITEM("RESEARCH_EXPLAIN",
			GETTEXT("You can see your tech status, set the researching goal, or invest"
					" in research."));
	ITEM("PROJECT_EXPLAIN", GETTEXT("You can buy projects."));
	ITEM("WHITEBOOK_EXPLAIN", GETTEXT("A detailed report of your domain."));

//	system_log("end page handler %s", get_name());

	return output("domestic/domestic.html");
}
