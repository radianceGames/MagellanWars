#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPagePlayerSeeDomestic::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());
    // Default player access check
    CPortalUser *thisAdmin = PORTAL_TABLE->get_by_portal_id(aPlayer->get_portal_id());
    CString error_msg;
    error_msg.format("%s '%s' %s.", 
                     GETTEXT("Access Level "), 
                     user_level_to_string(thisAdmin->get_user_level()), 
                     GETTEXT("does not grant access"));
    if (!ADMIN_TOOL->has_access(aPlayer))
    {
       ITEM("ERROR_MESSAGE", (char *)error_msg);
       return output("admin/admin_error.html"); 
    }  

    QUERY("PORTAL_ID",PortalIDString);
    int PortalID = 0;
    if (PortalIDString)
    {
       PortalID = as_atoi(PortalIDString);
    }

    ITEM("PORTAL_ID", PortalID);
	CPlayer *
		Player = PLAYER_TABLE->get_by_portal_id(PortalID);
	if (!Player)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("This account doesn't have any characters."));
		return output("admin/admin_error.html");

	}
    ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)format("Viewed player %s domestic sheet (Portal ID: %d)", Player->get_nick(), PortalID));
	static CString
		ProjectName;
	ProjectName.clear();

	static CString
		ControlModelStatus;
	ControlModelStatus.clear();

	CPlanetList *
		PlanetList = Player->get_planet_list();
	CPlanet *
		HomePlanet = (CPlanet *)PlanetList->get(0);
	if (HomePlanet == NULL)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("This player doesn't have any planets."));
		return output("admin/admin_error.html");
	}
  
    // TODO: reinstate planet_detail.as for admins
	ITEM("STRING_HOME_PLANET", GETTEXT("Home Planet"));
	ITEM("HOME_PLANET",
			(char *)format("<!--<A HREF=\"../planet_detail.as?PLANET_ID=%d\">-->%s<!--</a>-->",
							HomePlanet->get_id(), HomePlanet->get_name()));

	ITEM("STRING_NUMBER_OF_PLANET_S_", GETTEXT("Number of Planet(s)"));
	ITEM("NUMBER_OF_PLANET_S_", PlanetList->length());

	ITEM("STRING_CONCENTRATION_MODE", GETTEXT("Concentration Mode"));
	ITEM("CONCENTRATION_MODE_NAME", Player->get_mode_name());

	ITEM("STRING_POPULATION", GETTEXT("Population"));
	ITEM("DETAIL_POPULATION",
			(char *)format(GETTEXT("%1$s k"), dec2unit(Player->calc_population())));

	ITEM("STRING_PRODUCTION_POINT", GETTEXT("Production Point"));
	ITEM("PP",
			(char *)format(GETTEXT("%1$s PP"), dec2unit(Player->get_production())));

	ITEM("STRING_CONTROL_MODEL", GETTEXT("Control Model"));

	CControlModel *
		ControlModel = Player->get_control_model();

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

	int
		FactoryUpkeep = 0,
		MilitaryBaseUpkeep = 0,
		ResearchLabUpkeep = 0;
	int
		ProductionPointByPlanet = 0,
		ProductionPointByCommerce = 0,
		ProductionPointByRace = 0,
		ProductionPointByPayingFleetUpkeep = 0,
		ResearchPointByPlanet = 0,
		ResearchPointByEffect = 0,
		MilitaryPointByPlanet = 0,
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
		if (Player->has_effect(CPlayerEffect::PA_PARALYZE_PLANET, Planet->get_id()))
		{
			continue;
		}

		ProductionPointByPlanet += Planet->get_pp_per_turn();
		ResearchPointByPlanet += Planet->get_rp_per_turn();
		MilitaryPointByPlanet += Planet->get_mp_per_turn();
		ProductionPointByCommerce += Planet->calc_commerce();

		FactoryUpkeep += (Planet->get_upkeep()).get(RESOURCE_PRODUCTION);
		MilitaryBaseUpkeep += (Planet->get_upkeep()).get(RESOURCE_MILITARY);
		ResearchLabUpkeep += (Planet->get_upkeep()).get(RESOURCE_RESEARCH);
	}

	int
		NewProductionPoint =
				ProductionPointByPlanet + ProductionPointByCommerce,
		NewMilitaryPoint =
				MilitaryPointByPlanet,
		NewResearchPoint =
				ResearchPointByPlanet;

	for (int i=0 ; i<FORTRESS_LIST->length() ; i++)
	{
		CFortress *
			Fortress = (CFortress *)FORTRESS_LIST->get(i);
		if (Fortress->get_owner_id() == Player->get_game_id())
		{
			MilitaryPointByFortress += 10000;
		}
	}
	NewMilitaryPoint += MilitaryPointByFortress;

	if (Player->has_effect(CPlayerEffect::PA_PRODUCE_MP_PER_TURN) == true)
	{
		int
			NewMilitaryPointByEffect = Player->calc_PA(
					NewMilitaryPoint, CPlayerEffect::PA_PRODUCE_MP_PER_TURN);
		MilitaryPointByEffect += NewMilitaryPointByEffect - NewMilitaryPoint;

		NewMilitaryPoint = NewMilitaryPointByEffect;
	}

	if (Player->has_effect(CPlayerEffect::PA_PRODUCE_RP_PER_TURN) == true)
	{
		int
			NewResearchPointByEffect = Player->calc_PA(
				NewResearchPoint, CPlayerEffect::PA_PRODUCE_RP_PER_TURN);
		ResearchPointByEffect += NewResearchPointByEffect - NewResearchPoint;

		NewResearchPoint = NewResearchPointByEffect;
	}

#define SPACE_MINING_MODULE		5525
	CFleetList *
		FleetList = Player->get_fleet_list();
	CKnownTechList *
		KnownTechList = Player->get_tech_list();
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
						Fleet->get_size() * (100 + NumberOfTech) * Fleet->get_current_ship();

				break;
			}
		}
	}
	NewProductionPoint += ProductionPointByRace;
#undef SPACE_MINING_MODULE

	if (Player->has_effect(CPlayerEffect::PA_CONSUME_PP_PER_TURN) == true)
	{
		int
			NewProductionPointByEffect = Player->calc_minus_PA(
					NewProductionPoint, CPlayerEffect::PA_CONSUME_PP_PER_TURN);
		LosingProductionPointByEffect += NewProductionPoint - NewProductionPointByEffect;

		NewProductionPoint = NewProductionPointByEffect;
	}

	if (Player->has_effect(CPlayerEffect::PA_IMPERIAL_RETRIBUTION) == true)
	{
		LosingProductionPointByEffect += Player->get_production()/2;
		LosingResearchPointByEffect += Player->get_research()/2;
		LosingMilitaryPointByEffect += NewMilitaryPoint/2;

		NewMilitaryPoint -= NewMilitaryPoint/2;
	}

	int
		CurrentProductionPoint = NewProductionPoint;
	int
		ReservedPP = Player->get_production();

	LosingProductionPointBySecurity += CSpy::get_security_upkeep(
			Player->get_security_level(), CurrentProductionPoint);
	CurrentProductionPoint -= LosingProductionPointBySecurity;

	LosingProductionPointByBuildingUpkeep += FactoryUpkeep + MilitaryBaseUpkeep + ResearchLabUpkeep;
	CurrentProductionPoint -= LosingProductionPointByBuildingUpkeep;

	int
		ShipProduction =
				Player->get_new_resource()->get(RESOURCE_PRODUCTION)*(30+Player->get_control_model()->get_military()*5)/100;
	if (ShipProduction < 0) ShipProduction = 0;

	LosingProductionPointByShipProduction += ShipProduction;
	CurrentProductionPoint -= LosingProductionPointByShipProduction;

	Player->calc_all_ships_upkeep(&ReservedPP, CurrentProductionPoint, &NewMilitaryPoint, &ProductionPointByPayingFleetUpkeep, &LosingProductionPointByPayingFleetUpkeep, &LosingMilitaryPointByPayingFleetUpkeep);

	Player->simulate_repair_damaged_ship(&ReservedPP, &NewMilitaryPoint, &LosingProductionPointByRepairing, &LosingMilitaryPointByRepairing);

	if (CurrentProductionPoint > 100)
	{
		LosingProductionPointByCouncilTax = CurrentProductionPoint * 5 / 100;
		CurrentProductionPoint -= LosingProductionPointByCouncilTax;
	}

	int
		TotalProductionPointPerTurn =
				ProductionPointByPlanet + ProductionPointByCommerce + ProductionPointByRace + ProductionPointByPayingFleetUpkeep,
		TotalMilitaryPointPerTurn =
				MilitaryPointByPlanet + MilitaryPointByFortress + MilitaryPointByEffect,
		TotalResearchPointPerTurn =
				ResearchPointByPlanet + ResearchPointByEffect;
	int
		TotalPPUpkeep = LosingProductionPointByBuildingUpkeep + LosingProductionPointByPayingFleetUpkeep,
		TotalMPUpkeep = LosingMilitaryPointByPayingFleetUpkeep;

	int
		TotalPPSpending = LosingProductionPointByEffect + LosingProductionPointBySecurity + LosingProductionPointByShipProduction + LosingProductionPointByRepairing + LosingProductionPointByCouncilTax,
		TotalMPSpending = LosingMilitaryPointByEffect + LosingMilitaryPointByRepairing;

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
	ITEM("STRING_COMMERCE_PP", GETTEXT("Commerce PP"));
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
							dec2unit(FactoryUpkeep)));
	ITEM("MILITARY_BASE_UPKEEP",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(MilitaryBaseUpkeep)));
	ITEM("RESEARCH_LAB_UPKEEP",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(ResearchLabUpkeep)));

	ITEM("STRING_FLEET_SHIP_UPKEEP_PP_",
			(char *)format(GETTEXT("%1$s (PP)"), GETTEXT("Fleet/Ship Upkeep")));
	ITEM("FLEET_SHIP_UPKEEP_PP_",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(LosingProductionPointByPayingFleetUpkeep)));

	ITEM("STRING_FLEET_SHIP_UPKEEP_MP_",
			(char *)format(GETTEXT("%1$s (MP)"), GETTEXT("Fleet/Ship Upkeep")));
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

	ITEM("STRING_REPAIRING_PP_",
			(char *)format(GETTEXT("%1$s (PP)"), GETTEXT("Repairing")));
	ITEM("REPAIRING_PP_",
			(char *)format(GETTEXT("%1$s PP"),
							dec2unit(LosingProductionPointByRepairing)));

	ITEM("STRING_REPAIRING_MP_",
			(char *)format(GETTEXT("%1$s (MP)"), GETTEXT("Repairing")));
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
		ProjectList = Player->get_purchased_project_list();
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
		TechList = Player->get_tech_list();

	ITEM("SOCIAL_SCIENCE", TechList->count_by_category(CTech::TYPE_SOCIAL));
	ITEM("MATTER_ENERGY_SCIENCE",
			TechList->count_by_category(CTech::TYPE_MATTER_ENERGY));
	ITEM("INFO_SCIENCE", TechList->count_by_category(CTech::TYPE_INFORMATION));
	ITEM("LIFE_SCIENCE", TechList->count_by_category(CTech::TYPE_LIFE));

	ITEM("STRING_CURRENTLY_RESEARCHING", GETTEXT("Currently Researching"));

	int
		TargetTechID = Player->get_target_tech();
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
			ITEM("TARGET_TECH", "Wrong target tech.");
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

     // Generic Strings
	ITEM("ADMIN_LINK_STRING", GETTEXT("[Admin Main]"));
 	ITEM("INSPECT_LINK_STRING", GETTEXT("[Inspection]"));
 	ITEM("BLACK_MARKET_LINK_STRING", GETTEXT("[Black Market Control]"));
 	ITEM("CONFIG_LINK_STRING", GETTEXT("[Modify Config]"));
   	ITEM("RESTRICTION_LINK_STRING", GETTEXT("[Restriction Control]"));

    //	system_log("end page handler %s", get_name());
	return output("admin/player_see_domestic.html");
}

