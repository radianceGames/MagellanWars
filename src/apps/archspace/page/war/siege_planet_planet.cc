#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include <cstdio>

bool
CPageSiegePlanetPlanet::handler(CPlayer *aPlayer)
{
	system_log( "start page handler %s", get_name() );

	if (aPlayer->has_siege_blockade_restriction() == true)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("Due to Imperial regulations, you may not siege for another %1$d seconds."), aPlayer->get_siege_blockade_restriction_time()));

		return output("war/war_error.html");
	}
	CPreference *aPreference = aPlayer->get_preference();
	static CString
		DeployInfo;
	DeployInfo.clear();

	int	TargetPlayerID;
	if (aPreference != NULL && aPreference->has(CPreference::PR_JAVA))
	{
		QUERY("TID", TIDString);
		CHECK(TIDString == NULL,
		GETTEXT("You didn't enter a target player's ID."));
		TargetPlayerID = as_atoi(TIDString);
	}
	else
	{
		QUERY("TARGET_PLAYER_ID", TargetPlayerIDString);
		CHECK(TargetPlayerIDString == NULL,
		GETTEXT("You didn't enter a target player's ID."));
		TargetPlayerID = as_atoi(TargetPlayerIDString);
	}

	CPlayer *
		TargetPlayer = PLAYER_TABLE->get_by_game_id(TargetPlayerID);
	CHECK(!TargetPlayer,
			GETTEXT("The targetted player doesn't exist."));

	CHECK(TargetPlayer->get_game_id() == EMPIRE_GAME_ID,
			GETTEXT("You can't attack the Empire in this menu."));

	CHECK(TargetPlayer->get_game_id() == aPlayer->get_game_id(),
			GETTEXT("You can't attack yourself."));

	CPlanetList *
		PlanetList = TargetPlayer->get_planet_list();

	CHECK((TargetPlayer->is_dead() || PlanetList->length()==0),
			(char *)format(GETTEXT("%1$s is dead."),
							TargetPlayer->get_nick()));

	if (TargetPlayer->has_siege_blockade_protection() == true)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("Recently that player has had a planetary siege battle in his/her domain. You decided not to move your armada there until you get a clear information."));

		return output("war/war_error.html");
	}

	CString Attack;
	Attack = aPlayer->check_attackable(TargetPlayer);
	CHECK(Attack.length(), Attack);

	CCouncil *
		TargetPlayerCouncil = TargetPlayer->get_council();

	CRelation* Relation = aPlayer->get_relation(TargetPlayer);

	if (Relation != NULL && Relation->get_relation() == CRelation::RELATION_BOUNTY)
	{
	}
	else if (aPlayer->get_council() == TargetPlayerCouncil)
	{
		CHECK(!Relation,
			(char*)format(GETTEXT("You have no relation with %1$s."),
								TargetPlayer->get_nick()));

			CHECK((Relation->get_relation() != CRelation::RELATION_WAR && Relation->get_relation() != CRelation::RELATION_TOTAL_WAR),
			(char*)format(GETTEXT("You are not at war with %1$s."),
						TargetPlayer->get_nick()));
		if (TargetPlayer->is_protected())
		{
			ITEM("ERROR_MESSAGE",
			(char *)format(GETTEXT("The player %1$s has lost a large amount of their planets recently and Imperial Law prevents you from attacking them."),
			TargetPlayer->get_nick()));

			return output("war/war_error.html");
		}
	}
	else
	{
		Relation = TargetPlayerCouncil->get_relation(aPlayer->get_council());

		CHECK(!Relation,
			(char *)format(GETTEXT("Your council has no relationship with %1$s's council."),
							TargetPlayer->get_nick()));

		CHECK(Relation->get_relation() != CRelation::RELATION_WAR &&
				Relation->get_relation() != CRelation::RELATION_TOTAL_WAR,
			(char *)format(GETTEXT("Your council is not at war or total war with %1$s's council."),
								TargetPlayer->get_nick()));
		if (Relation->get_relation() == CRelation::RELATION_WAR)
		{
			if (TargetPlayer->is_protected())
			{
				ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("The player %1$s has lost a large amount of their planets recently and Imperial Law prevents you from attacking them."),
				TargetPlayer->get_nick()));
				return output("war/war_error.html");
			}
			else if((CGame::get_game_time() - Relation->get_time()) < 600)
			{
				ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("You have not been at war with the player %1$s long enough to attack. Please wait another %2$s seconds"),
				TargetPlayer->get_nick(), dec2unit((Relation->get_time() + 600) - CGame::get_game_time()) ));
				return output("war/war_error.html");
			}
		}
	}

	QUERY("FLEET_NUMBER", FleetNumberString)
	int
		FleetNumber = as_atoi(FleetNumberString);
	CHECK(!FleetNumber,
			GETTEXT("Wrong fleet number received."
					" Please ask Archspace Development Team."));

	CFleetList
		SortieFleetList;

	int	CapitalID;
	if (aPreference != NULL && aPreference->has(CPreference::PR_JAVA))
    {
        QUERY("capFleet_ID", CapitalIDString)
        CapitalID = as_atoi(CapitalIDString);
    }
	else
	{
        QUERY("capFleet_id", CapitalIDString)
        CapitalID = as_atoi(CapitalIDString);
    }

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	CFleet *
		Fleet = (CFleet *)FleetList->get_by_id(CapitalID);

	CHECK(!Fleet, GETTEXT("You didn't select a capital fleet."));
	CHECK(Fleet->get_status() != CFleet::FLEET_STAND_BY,
			format(GETTEXT("%1$s fleet is not on stand-by."),
					Fleet->get_nick()));

	QUERY("capFleet_O", CommandString);
	if (CommandString == NULL)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Wrong command string received."
						" Please ask Archspace Development Team."));

		return output("war/war_error.html");
	}

	int
		Command = CDefenseFleet::get_fleet_command_from_string((char *)CommandString);
	if (Command < 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Wrong command string received."
						" Please ask Archspace Development Team."));

		return output("war/war_error.html");
	}

	SortieFleetList.add_fleet(Fleet);

	DeployInfo.format("<INPUT TYPE=hidden NAME=\"FLEET1_O\" VALUE=\"%d\">\n",
						Command);
	DeployInfo += "<INPUT TYPE=hidden NAME=\"FLEET1_X\" VALUE=\"309\">\n";
	DeployInfo += "<INPUT TYPE=hidden NAME=\"FLEET1_Y\" VALUE=\"326\">\n";
	DeployInfo.format("<INPUT TYPE=hidden NAME=\"FLEET1_ID\" VALUE=\"%d\">\n",
						 CapitalID);

	CDefensePlan
		OffensePlan;

	for (int i=2 ; i<=FleetNumber ; i++)
	{
		char Query[100];

		if (aPreference != NULL && aPreference->has(CPreference::PR_JAVA))
		    sprintf(Query, "Fleet%d_ID", i);
		else
            sprintf(Query, "fleet%d_id", i);


		QUERY(Query, FleetIDString);

		int
			FleetID = as_atoi(FleetIDString);
		CFleet *
			Fleet = (CFleet *)FleetList->get_by_id(FleetID);

		if (Fleet == NULL)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("Wrong fleet ID received."
							" Please ask Archspace Development Team."));

			SortieFleetList.remove_without_free_all();

			return output("war/war_error.html");
		}
		if (Fleet->get_status() != CFleet::FLEET_STAND_BY)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("%1$s fleet is not on stand-by."),
									Fleet->get_nick()));

			SortieFleetList.remove_without_free_all();

			return output("war/war_error.html");
		}
		int
			LocationX;
		int
			LocationY;


		if (aPreference != NULL && aPreference->has(CPreference::PR_JAVA))
		{
		  QUERY((char *)format("Fleet%d_X", i), LocationXString)
		  QUERY((char *)format("Fleet%d_Y", i), LocationYString)
		  LocationY = as_atoi(LocationYString);
		  LocationX = as_atoi(LocationXString);
		}
        else
        {
          QUERY((char *)format("fleet%d_X", i), LocationXString)
          QUERY((char *)format("fleet%d_Y", i), LocationYString)
		  LocationY = as_atoi(LocationYString);
		  LocationX = as_atoi(LocationXString);
        }
		if (LocationX < 9 || LocationX > 609 ||
				LocationY < 226 || LocationY > 426) continue;

		SortieFleetList.add_fleet(Fleet);

		sprintf(Query, "fleet%d_O", i);
		QUERY(Query, CommandString);

		Command = CDefenseFleet::get_fleet_command_from_string((char *)CommandString);

		if (Command < 0)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("Wrong command string received."
							" Please ask Archspace Development Team."));

			SortieFleetList.remove_without_free_all();

			return output("war/war_error.html");
		}
		DeployInfo.format(
				"<INPUT TYPE=hidden NAME=\"FLEET%d_O\" VALUE=\"%d\">\n",
					i, Command);
		DeployInfo.format(
				"<INPUT TYPE=hidden NAME=\"FLEET%d_X\" VALUE=\"%d\">\n",
					i, LocationX);
		DeployInfo.format(
				"<INPUT TYPE=hidden NAME=\"FLEET%d_Y\" VALUE=\"%d\">\n",
					i, LocationY);
		DeployInfo.format(
				"<INPUT TYPE=hidden NAME=\"FLEET%d_ID\" VALUE=\"%d\">\n",
							i, Fleet->get_id());

		CDefenseFleet *
			OffenseFleet = new CDefenseFleet();
		OffenseFleet->set_owner(aPlayer->get_game_id());
		OffenseFleet->set_fleet_id(FleetID);
		OffenseFleet->set_command(Command);
		OffenseFleet->set_x(3000-(LocationY - 226)*10);
		OffenseFleet->set_y(8000-(LocationX - 9)*10);

		OffensePlan.add_defense_fleet(OffenseFleet);
	}

	if (OffensePlan.is_there_stacked_fleet() == true)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You stacked 1 or more fleets on another fleet(s)."));

		SortieFleetList.remove_without_free_all();

		return output("war/war_error.html");
	}
	else
	{
		for (int i=SortieFleetList.length()-1 ; i>=0 ; i--)
		{
			CFleet *
				Fleet = (CFleet *)SortieFleetList.get(i);
			Fleet->init_mission(CMission::MISSION_SORTIE, 0);
		}

		SortieFleetList.remove_without_free_all();
	}

	ITEM("ATTACKING_PLAYER_MESSAGE",
			(char *)format(GETTEXT("Attacking - %1$s"), TargetPlayer->get_nick()));

	ITEM("SELECT_PLANET_MESSAGE",
			GETTEXT("Select the planet to attack."
				" If you do not select the target planet in 2 turns,<BR>"
				" your fleets will automatically return home without an attack."));

	CPlanet *
		InitPlanet = (CPlanet *)PlanetList->get(PlanetList->length()/2);
	CResource &
		InitPlanetBuilding = InitPlanet->get_building();

	ITEM("PLANET_WITH_ORDER", InitPlanet->get_order_name());

	CString
		PlanetImage;
	PlanetImage.clear();
	PlanetImage.format("%s/image/as_game/", (char *)CGame::mImageServerURL);

	if (InitPlanet->get_order() == 0)
		PlanetImage += "domestic/planet_home_icon.gif";
	else
		PlanetImage += "domestic/planet_icon.gif";

	ITEM("PLANET_IMAGE", (char *)PlanetImage);

	ITEM("PLANET_NAME", InitPlanet->get_name());

	ITEM("STRING_POPULATION", GETTEXT("Population"));
	ITEM("POPULATION", InitPlanet->get_population_with_unit());

	ITEM("STRING_SIZE", GETTEXT("Size"));
	ITEM("SIZE", InitPlanet->get_size_description());

	ITEM("STRING_ATTRIBUTE", GETTEXT("Attribute"));
	ITEM("ATTRIBUTE", InitPlanet->get_attribute_description());

	ITEM("STRING_RESOURCE", GETTEXT("Resource"));
	ITEM("RESOURCE", InitPlanet->get_resource_description());

	ITEM("STRING_BUILDINGS", GETTEXT("Buildings"));
	ITEM("BUILDING",
			(char *)format("%s %d     %s %d     %s %d",
							GETTEXT("Factory"),
							InitPlanetBuilding.get(BUILDING_FACTORY),
							GETTEXT("Laboratory"),
							InitPlanetBuilding.get(BUILDING_RESEARCH_LAB),
							GETTEXT("Military Base"),
							InitPlanetBuilding.get(BUILDING_MILITARY_BASE)));

	ITEM("BORDER_PLANET_SELECT", PlanetList->siege_planet_border_planet_select_html());

	CString JSPlanetInfo;

	for (int i = PlanetList->length()/2 ; i<PlanetList->length() ; i++)
	{
		CPlanet *Planet = (CPlanet *)PlanetList->get(i);
		CResource &Building = Planet->get_building();

		if (i == PlanetList->length()/2)
			JSPlanetInfo.format("if (planet == \"%d\")\n", Planet->get_id());
		else
			JSPlanetInfo.format("  else if (planet == \"%d\")\n", Planet->get_id());
		JSPlanetInfo += "{\n";
		JSPlanetInfo.format("    document.siegePlanet.home.value = \"%s\";\n",
							Planet->get_name());
		JSPlanetInfo.format("    document.siegePlanet.population.value = \"%s\";\n",
							Planet->get_population_with_unit());
		JSPlanetInfo.format("    document.siegePlanet.size.value = \"%s\";\n",
							Planet->get_size_description());
		JSPlanetInfo.format("    document.siegePlanet.attribute.value = \"%s\";\n",
							Planet->get_attribute_description());
		JSPlanetInfo.format("    document.siegePlanet.resource.value = \"%s\";\n",
							Planet->get_resource_description());
		JSPlanetInfo.format("    document.siegePlanet.building.value = "
							"\"%s %d     %s %d     %s %d\";\n",
							GETTEXT("Factory"), Building.get(BUILDING_FACTORY),
							GETTEXT("Laboratory"), Building.get(BUILDING_RESEARCH_LAB),
							GETTEXT("Military Base"), Building.get(BUILDING_MILITARY_BASE));
		JSPlanetInfo += "}\n";
	}

	ITEM("JS_PLANET_INFO", (char *)JSPlanetInfo);

	ITEM("FLEET_NUMBER", FleetNumber);
	ITEM("DEPLOY_INFO", (char *)DeployInfo);

	ITEM("TARGET_PLAYER_ID", TargetPlayerID);

	system_log( "end page handler %s", get_name() );

	return output("war/siege_planet_planet.html");
}

