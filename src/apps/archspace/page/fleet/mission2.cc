#include <libintl.h>
#include "../../pages.h"

#define UPGRADE_COMMANDER_CHANGE	1
#define UPGRADE_SHIP_UPGRADE		2
#define UPGRADE_REMOVE_ADD_SHIPS	3

bool
CPageMission2::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	static CString
		Message;
	Message.clear();

	CCommandSet
		FleetSet;
	FleetSet.clear();

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	for (int i=0 ; i<FleetList->length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)FleetList->get(i);
		if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

		CString
			QueryVar;
		QueryVar.clear();
		QueryVar.format("FLEET%d", i);

		QUERY(QueryVar, FleetIDString);

		if (FleetIDString)
		{
			if (!strcasecmp(FleetIDString, "ON"))
			{
				FleetSet += i;
			}
		}
	}

	if(FleetSet.is_empty())
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't select any fleets."));
		return output("fleet/mission_error.html");
	}

	QUERY("MISSION", MissionIDString);
	int
		MissionID;
	if(!MissionIDString) MissionID = 0;
	else
		MissionID = as_atoi(MissionIDString);

	ITEM("FLEET_SET",
			(char *)format("<INPUT TYPE=hidden NAME=FLEET_SET VALUE=%s>\n",
							FleetSet.get_string()));

	switch (MissionID)
	{
		case CMission::MISSION_TRAIN :
		{
			int
				UpkeepMP = 0;
			for (int i=0 ; i<FleetList->length() ; i++)
			{
				CFleet *
					Fleet = (CFleet *)FleetList->get(i);
				if (!FleetSet.has(i)) continue;

				UpkeepMP += Fleet->calc_upkeep() * 2;
			}

			/*if (UpkeepMP <= aPlayer->get_last_turn_military()) UpkeepPP = 0;
			else
			{
				if ((UpkeepMP-aPlayer->get_last_turn_military()) > MAX_PLAYER_PP/20)
				{
					UpkeepPP = MAX_PLAYER_PP;
				}
				else
				{
					UpkeepPP = 20 * (UpkeepMP-aPlayer->get_last_turn_military());
				}
				UpkeepMP = aPlayer->get_last_turn_military();
			}

			if (UpkeepPP > aPlayer->get_production())
			{
				ITEM("ERROR_MESSAGE",
						(char *)format(
								GETTEXT("Training selected fleet(s) will cost %1$s MP and %2$s PP.<BR>"
										"You don't have enough PP. Please try again later."),
										dec2unit(UpkeepMP),
										dec2unit(UpkeepPP)));

				return output("fleet/mission_error.html");
			}*/

			ITEM("STRING_TRAIN", GETTEXT("Train"));

			Message = GETTEXT("Fleets on a mission will not participate"
								" in the defense plan,<BR>"
								"and once they enter a mission,"
								" you cannot change status"
								" until the mission finishes.");
			Message += "<BR><BR>\n";

			/*int
				TrainInHour = (CMission::mTrainMissionTime / 60) / 60,
				TrainInMin = (CMission::mTrainMissionTime / 60) % 60;

			CString
				UpkeepMPString,
				UpkeepPPString,
				TrainInHourString,
				TrainInMinString;
			UpkeepMPString = dec2unit(UpkeepMP);
			UpkeepPPString = dec2unit(UpkeepPP);
			TrainInHourString = dec2unit(TrainInHour);
			TrainInMinString = dec2unit(TrainInMin);*/

			Message.format(GETTEXT("Training selected fleet(s) will cost %1$d additional MP per turn. You will pay 20 PP per MP you lack if you have insufficient MP."),
							UpkeepMP);
			Message += "<BR><BR>\n";


			/*int
				FleetExp = 10+aPlayer->get_control_model()->get_military()*5;

			if (FleetExp < CMission::mFleetExpTrain)
							FleetExp = CMission::mFleetExpTrain;*/

			Message.format(GETTEXT("Training fleets will gain %1$d experiance per turn and the admiral will gain %2$d experience per turn. These fleets may be recalled at any time."),
							CMission::mFleetExpTrain, CMission::mAdmiralExpTrain);

			Message += "<BR>\n";
			Message += GETTEXT("Are you sure you want to train the fleet?");

			ITEM("CONFIRM_MESSAGE", (char *)Message);

			return output("fleet/mission_train_confirm.html");
		}

		case CMission::MISSION_STATION_ON_PLANET :
		{
			ITEM("STRING_DISPATCH_TO_PLANET", GETTEXT("Dispatch to Planet"));

			Message = GETTEXT("Fleets on a mission will not participate"
								" in the defense plan,<BR>"
								"and once they enter a mission,"
								" you cannot change status"
								" until the mission finishes.");
			Message += "<BR>\n";
			Message += GETTEXT("Stationed fleet(s) will detect"
								" and interpret enemy invasion,<BR>"
								"and deter special operations"
								" and privateers on the planet.");
			Message += "<BR><BR>\n";
			Message += GETTEXT("Are you sure you want to dispatch selected fleet(s)"
								" to the following planet?");
			ITEM("CONFIRM_MESSAGE", (char *)Message);

			ITEM("STRING_PLANET_SELECT", GETTEXT("Planet Select"));
			ITEM("SELECT_PLANET", aPlayer->get_planet_list()->html_select());

			return output("fleet/mission_station_confirm.html");
		}

		case CMission::MISSION_PATROL :
		{
			ITEM("STRING_PATROL", GETTEXT("Patrol"));

			Message = GETTEXT("Fleets on a mission will not participate"
								" in the defense plan,<BR>"
								"and once they enter a mission,"
								" you cannot change status"
								" until the mission finishes.");
			Message += "<BR>\n";

			int
				PatrolInHour = CMission::mPatrolMissionTime / 60 / 60;
			Message.format(
					GETTEXT("Patrolling fleet(s) will have"
							" the same effect as stationed fleets,<BR>"
							"but the duration is only %1$d hours."),
							PatrolInHour);
			Message += "<BR><BR>\n";

			Message += GETTEXT("Are you sure you want to assign the selected fleet(s) to"
								" patrol the following planet?");
			ITEM("CONFIRM_MESSAGE", (char *)Message);

			ITEM("STRING_PLANET_SELECT", GETTEXT("Planet Select"));
			ITEM("SELECT_PLANET", aPlayer->get_planet_list()->html_select());

			return output("fleet/mission_patrol_confirm.html");
		}

		case CMission::MISSION_DISPATCH_TO_ALLY :
		{
			CPlayerRelationList *
				RelationList = aPlayer->get_relation_list();

			if (!RelationList->is_there_relation(CRelation::RELATION_ALLY))
			{
				ITEM("ERROR_MESSAGE",
						GETTEXT("You have no allies to send fleet(s)."));

				return output("fleet/mission_error.html");
			}

			ITEM("STRING_DISPATCH_TO_ALLY", GETTEXT("Dispatch to Ally"));

			Message = GETTEXT("Fleets on a mission will not participate"
								" in the defense plan,<BR>"
								"and once they enter a mission,"
								" you cannot change status"
								" until the mission finishes.");
			Message += "<BR><BR>\n";
			Message += GETTEXT("Dispatched fleet(s) will support"
								" the main fleet in defensive battle.");

			Message += "<BR>\n";
   
//            int VirtualPower = 0;

            for (int i=0 ; i<FleetList->length() ; i++)
			{
				CFleet *
					Fleet = (CFleet *)FleetList->get(i);
				if (!FleetSet.has(i)) continue;

//				VirtualPower += Fleet->get_power();
			}

			int
				DispathToAllyInHour =
						CMission::mDispatchToAllyMissionTime / 60 / 60;
			Message.format(GETTEXT("The fleet(s) will return"
									" when %1$d hour(s) have passed,"
									" or the alliance is broken,"
									" or the fleet(s) can't fight"),
							DispathToAllyInHour);
/*			Message.format(GETTEXT(" any more, and will contribute %s"
                                    " to the target's virtual power."),
                            dec2unit(VirtualPower));*/
			Message += "<BR>\n";
			Message += GETTEXT("Are you sure you want to send these fleets to the following player?");
			Message += "<BR>\n";

			ITEM("CONFIRM_MESSAGE", (char *)Message);

			ITEM("STRING_PLAYER_SELECT", GETTEXT("Player Select"));

			ITEM("SELECT_PLAYER",
					RelationList->select_player_by_relation_html(aPlayer, CRelation::RELATION_ALLY));

			return output("fleet/mission_dispatch_confirm.html");
		}

		default :
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("You selected an unknown mission."));

			return output("fleet/mission_error.html");
		}
	}
}

#undef UPGRADE_COMMANDER_CHANGE
#undef UPGRADE_SHIP_UPGRADE
#undef UPGRADE_REMOVE_ADD_SHIPS

