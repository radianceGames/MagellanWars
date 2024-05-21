#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageRaidPlanet::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	QUERY("TARGET_PLAYER_ID", TargetPlayerIDString);
	int
		TargetPlayerID = as_atoi(TargetPlayerIDString);

	CPlayer *
		TargetPlayer = PLAYER_TABLE->get_by_game_id(TargetPlayerID);

	CHECK(!TargetPlayer,
			(char *)format(GETTEXT("The player with ID %1$s doesn't exist."),
							dec2unit(TargetPlayerID)));

	CHECK(TargetPlayer->get_game_id() == EMPIRE_GAME_ID,
			GETTEXT("You can't attack the Empire in this menu."));

	CHECK(TargetPlayer->is_dead(),
	                format(GETTEXT("%1$s is dead."), TargetPlayer->get_nick()));

	CHECK(TargetPlayer == aPlayer, GETTEXT("You can't raid yourself."));

	CCouncil *
		TargetPlayerCouncil = TargetPlayer->get_council();

	CRelation *
		Relation;

        CString
                RelationDescription;

        Relation = aPlayer->get_relation(TargetPlayer);

        if (aPlayer->get_council() == TargetPlayer->get_council() || (Relation != NULL && Relation->get_relation() == CRelation::RELATION_HOSTILE))
        {
                if (Relation)
                        RelationDescription = Relation->get_relation_description();
                else
                        RelationDescription = CRelation::get_relation_description(
                                                                CRelation::RELATION_NONE);
        }
        else
        {
                Relation = TargetPlayerCouncil->get_relation(aPlayer->get_council());
                if (Relation)
                {
                        RelationDescription.format("%s %s",
                                                                                GETTEXT("Council"),
                                                                                Relation->get_relation_description());
                }
                else
                {
                        RelationDescription = CRelation::get_relation_description(
                                                                                                CRelation::RELATION_NONE);
                }
        }
        if(!Relation)
        {
                ITEM("ERROR_MESSAGE",
                                        (char *)format(GETTEXT("You have no relation with %1$s."),TargetPlayer->get_nick()));

                return output("war/war_error.html");
        }

        if(Relation->get_relation() != CRelation::RELATION_WAR && Relation->get_relation() != CRelation::RELATION_TOTAL_WAR && Relation->get_relation() != CRelation::RELATION_HOSTILE && Relation->get_relation() != CRelation::RELATION_BOUNTY)
        {
                ITEM("ERROR_MESSAGE",
                                (char *)format(GETTEXT("You have no relation with %1$s."),TargetPlayer->get_nick()));

                return output("war/war_error.html");

        }
        if((CGame::get_game_time() - Relation->get_time()) < 600)
		{
			ITEM("ERROR_MESSAGE",
								(char *)format(GETTEXT("You have not had hostile relations with the target player long enough to attack. Please wait another %1$s seconds"),
							dec2unit((Relation->get_time() + 600) - CGame::get_game_time()) ));
			return output("war/war_error.html");
		}

    CString Attack;
    Attack = aPlayer->check_attackableHi(TargetPlayer);
    CHECK(Attack.length(), Attack);

	QUERY("RAID_FLEET_ID", RaidFleetIDString);
	int RaidFleetID = as_atoi(RaidFleetIDString);
	CFleetList * FleetList = aPlayer->get_fleet_list();
	CFleet *RaidFleet = (CFleet *)FleetList->get_by_id(RaidFleetID);
	CHECK(!RaidFleet,
			GETTEXT("You did not select any fleet.<BR>"
					"Please try again."));

	CHECK(RaidFleet->get_status() != CFleet::FLEET_STAND_BY,
			format(GETTEXT("%1$s fleet is not on stand-by."),
					RaidFleet->get_nick()));
	RaidFleet->init_mission(CMission::MISSION_SORTIE, 0);

	ITEM("ATTACKING_PLAYER_MESSAGE",
			format(GETTEXT("Attacking - %1$s"), TargetPlayer->get_nick()));

	ITEM("SELECT_PLANET_MESSAGE",
			GETTEXT("Select the planet to attack."
				" If you do not select the target planet in 2 turns,<BR>"
				" your fleets will automatically return home without "
				"an attack."));

	CPlanetList *
		PlanetList = TargetPlayer->get_planet_list();
	CPlanet *InitPlanet = (CPlanet *)PlanetList->get(PlanetList->length()/2);
	CResource &InitPlanetBuilding = InitPlanet->get_building();

	ITEM("PLANET_WITH_ORDER", InitPlanet->get_order_name());

	if (InitPlanet->get_order() == 0)
	{
		ITEM("PLANET_IMAGE",
				(char *)format("%s/image/as_game/domestic/planet_home_icon.gif",
								(char *)CGame::mImageServerURL));
	}
	else
	{
		ITEM("PLANET_IMAGE",
				(char *)format("%s/image/as_game/domestic/planet_icon.gif",
								(char *)CGame::mImageServerURL));
	}


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

	ITEM("BORDER_PLANET_SELECT", PlanetList->raid_border_planet_select_html());

	static CString
		JSPlanetInfo;
	JSPlanetInfo.clear();

	for (int i = PlanetList->length()/2 ; i<PlanetList->length() ; i++)
	{
		CPlanet *
			Planet = (CPlanet *)PlanetList->get(i);
		CResource &
			Building = Planet->get_building();

		if (i == PlanetList->length()/2)
		{
			JSPlanetInfo.format("if (planet == \"%d\")\n", Planet->get_id());
		} else
		{
			JSPlanetInfo.format("  else if (planet == \"%d\")\n", Planet->get_id());
		}
		JSPlanetInfo += "{\n";
		JSPlanetInfo.format("    document.raidPlanet.home.value = \"%s\";\n",
							Planet->get_name());
		JSPlanetInfo.format("    document.raidPlanet.population.value = \"%s\";\n",
							Planet->get_population_with_unit());
		JSPlanetInfo.format("    document.raidPlanet.size.value = \"%s\";\n",
							Planet->get_size_description());
		JSPlanetInfo.format("    document.raidPlanet.attribute.value = \"%s\";\n",
							Planet->get_attribute_description());
		JSPlanetInfo.format("    document.raidPlanet.resource.value = \"%s\";\n",
							Planet->get_resource_description());
		JSPlanetInfo.format("    document.raidPlanet.building.value = "
							"\"%s %d     %s %d     %s %d\";\n",
							GETTEXT("Factory"), Building.get(BUILDING_FACTORY),
							GETTEXT("Laboratory"), Building.get(BUILDING_RESEARCH_LAB),
							GETTEXT("Military Base"), Building.get(BUILDING_MILITARY_BASE));
		JSPlanetInfo += "}\n";
	}

	ITEM("JS_PLANET_INFO", (char *)JSPlanetInfo);

	ITEM("TARGET_PLAYER_ID", TargetPlayerID);
	ITEM("RAID_FLEET_ID", RaidFleetID);

//	system_log( "end page handler %s", get_name() );

	return output("war/raid_planet.html");
}

