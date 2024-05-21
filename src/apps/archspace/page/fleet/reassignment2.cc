#include <libintl.h>
#include "../../pages.h"
#include "../../game.h"
#include "../../archspace.h"

#define REASSIGNMENT_COMMANDER_CHANGE 1
#define REASSIGNMENT_SHIP_REASSIGNMENT 2
#define REASSIGNMENT_REMOVE_ADD_SHIPS 3
#define REASSIGNMENT_REFILL_FLEET 4
#define REASSIGNMENT_MAXIMIZE_FLEET 5

bool
CPageReassignment2::handler(CPlayer *aPlayer)
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
		return output("fleet/reassignment_error.html");
	}

	QUERY("REASSIGNMENT", ReassignmentIDString);
	int
		ReassignmentID;
	if(!ReassignmentIDString) ReassignmentID = 0;
	else
	{
		ReassignmentID = as_atoi(ReassignmentIDString);
	}

	ITEM("FLEET_SET",
			(char *)format("<INPUT TYPE=hidden NAME=FLEET_SET VALUE=%s>\n",
							FleetSet.get_string()));
    /*
	CAdmiral *
		Admiral = aPlayer->get_admiral_list()->get_by_id(Fleet->get_admiral_id());
	if(!Admiral)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(
						GETTEXT("Fleet %1$s's admiral does not exist."),
						Fleet->get_name()));
		return output("fleet/reassignment_error.html");
	}  */
	
	CFleet *Fleet;

	switch(ReassignmentID)
	{
		case REASSIGNMENT_COMMANDER_CHANGE :
		{
            static CString
            Message;
            Message.clear();

            CCommandSet FleetSet;
            FleetSet.clear();

            CFleetList *FleetList = aPlayer->get_fleet_list();
            for (int i=0 ; i<FleetList->length() ; i++) {
                CFleet *Fleet = (CFleet *)FleetList->get(i);
                if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

                CString QueryVar;
                QueryVar.clear();
                QueryVar.format("FLEET%d", i);

                QUERY(QueryVar, FleetIDString);

                if (FleetIDString) {
                    if (!strcasecmp(FleetIDString, "ON")) {
                        FleetSet += i;
                    }
                }
            }

            if(FleetSet.is_empty()) {
                ITEM("ERROR_MESSAGE", GETTEXT("You didn't select any fleets."));
                return output("fleet/reassignment_error.html");
            }

            // Use the last selected fleet if multiple are selected
            int j = 0;
            for (int i=0 ; i<FleetList->length() ; i++) {
                Fleet = (CFleet *)FleetList->get(i);

                if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;
                if (FleetSet.has(i) == true) {
                    j = i;
                    break;
                }
            }
            
            Fleet = (CFleet *)FleetList->get(j);
			
            CAdmiral *Admiral = aPlayer->get_admiral_list()->get_by_id(Fleet->get_admiral_id());
            if(!Admiral)
            {
            ITEM("ERROR_MESSAGE", (char *)format(GETTEXT("Fleet %1$s's admiral does not exist."),
						Fleet->get_name()));
            return output("fleet/reassignment_error.html");
            }

			ITEM("STRING_COMMANDER_CHANGE", GETTEXT("Commander Change"));

			ITEM("STRING_FLEET", GETTEXT("Fleet"));
			ITEM("STRING_CLASS", GETTEXT("Class"));
			ITEM("STRING_TOTAL_NUMBER_OF_FLEET_SHIP_S_",
					GETTEXT("Total Number of Fleet Ship(s)"));
			ITEM("STRING_CURRENT_FLEET_COMMANDER",
					GETTEXT("Current Fleet Commander"));
			ITEM("STRING_MAX__COMMAND", GETTEXT("Max. Command"));

			ITEM("FLEET", Fleet->get_name());
			ITEM("CLASS", Fleet->CShipDesign::get_name());
			ITEM("TOTAL_NUMBER_OF_FLEET_SHIP", Fleet->get_current_ship());
			ITEM("CURRENT_FLEET_COMMANDER", Admiral->get_name());
			ITEM("MAX_COMMAND", Admiral->get_fleet_commanding());

			ITEM("STRING_FLEET_COMMANDER_S__IN_THE_POOL",
					GETTEXT("Fleet Commander(s) in the Pool"));

			if (!aPlayer->get_admiral_pool()->length())
			{
				ITEM("FLEET_COMMANDER_LIST",
						GETTEXT("There are no free fleet commanders."));
				return output("fleet/reassignment_commander_error.html");
			}

			Message = "<TABLE WIDTH=550 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=#2A2A2A>\n";
			Message += "<TR BGCOLOR=#171717>\n";

			Message += "<TH CLASS=tabletxt WIDTH=51><FONT COLOR=666666>";
			Message += GETTEXT("ID");
			Message += "</FONT></TH>\n";

			Message += "<TH WIDTH=24 CLASS=tabletxt><FONT COLOR=666666>";
			Message += "</FONT></TH>\n";

			Message += "<TH WIDTH=113 CLASS=tabletxt><FONT COLOR=666666>";
			Message += GETTEXT("Name");
			Message += "</FONT></TH>\n";

			Message += "<TH WIDTH=52 CLASS=tabletxt><FONT COLOR=666666>";
			Message += GETTEXT("Level");
			Message += "</FONT></TH>\n";

			Message += "<TH WIDTH=59 CLASS=tabletxt><FONT COLOR=666666>";
			Message += GETTEXT("Exp.");
			Message += "</FONT></TH>\n";

			Message += "<TH WIDTH=100 CLASS=tabletxt><FONT COLOR=666666>";
			Message += GETTEXT("Max. Command");
			Message += "</FONT></TH>\n";

			Message += "<TH WIDTH=105 CLASS=tabletxt><FONT COLOR=666666>";
			Message += GETTEXT("Efficiency");
			Message += "</FONT></TH>\n";

			Message += "</TR>\n";

			for( int i = 0; i < aPlayer->get_admiral_pool()->length(); i++ )
			{
				CAdmiral
					*FreeAdmiral = (CAdmiral*)aPlayer->get_admiral_pool()->get(i);
				Message += "<TR> \n";

				Message.format("<TD CLASS=tabletxt WIDTH=51 ALIGN=CENTER>%d</TD>\n",
								FreeAdmiral->get_id());

				Message.format("<TD WIDTH=24 CLASS=tabletxt ALIGN=CENTER>\n");
				Message.format("<INPUT TYPE=RADIO NAME=FREE_ADMIRAL VALUE=%d>\n",
								FreeAdmiral->get_id());
				Message += "</TD>\n";

				Message += "<TD WIDTH=113 CLASS=tabletxt ALIGN=LEFT>";
				Message.format("<A HREF=\"/archspace/fleet/fleet_commander_information.as?ADMIRAL_ID=%d\">%s</A>",
								FreeAdmiral->get_id(), FreeAdmiral->get_name());
				Message += "</TD>\n";

				Message.format("<TD WIDTH=52 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
								FreeAdmiral->get_level());

				Message.format("<TD WIDTH=59 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
								FreeAdmiral->get_exp());

				Message.format("<TD WIDTH=100 CLASS=tabletxt ALIGN=CENTER>%d</TD>\n",
								FreeAdmiral->get_fleet_commanding());

				Message.format("<TD WIDTH=105 CLASS=tabletxt ALIGN=CENTER>%d%%</TD>\n",
								FreeAdmiral->get_real_efficiency());

				Message += "</TR> \n";
			}

			Message += "</TABLE>\n";
			ITEM("FLEET_COMMANDER_LIST", (char *)Message);

			return output("fleet/reassignment_commander_change.html");
		}

		case REASSIGNMENT_SHIP_REASSIGNMENT :
		{
			static CString
            Message;
            Message.clear();

            CCommandSet FleetSet;
            FleetSet.clear();

            CFleetList *FleetList = aPlayer->get_fleet_list();
            for (int i=0 ; i<FleetList->length() ; i++) {
                CFleet *Fleet = (CFleet *)FleetList->get(i);
                if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

                CString QueryVar;
                QueryVar.clear();
                QueryVar.format("FLEET%d", i);

                QUERY(QueryVar, FleetIDString);

                if (FleetIDString) {
                    if (!strcasecmp(FleetIDString, "ON")) {
                        FleetSet += i;
                    }
                }
            }

            if(FleetSet.is_empty()) {
                ITEM("ERROR_MESSAGE", GETTEXT("You didn't select any fleets."));
                return output("fleet/reassignment_error.html");
            }

            // Use the last selected fleet if multiple are selected
            int j = 0;
            for (int i=0 ; i<FleetList->length() ; i++) {
                Fleet = (CFleet *)FleetList->get(i);

                if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;
                if (FleetSet.has(i) == true) {
                    j = i;
                    break;
                }
            }
            
            Fleet = (CFleet *)FleetList->get(j);
			
			CAdmiral *Admiral = aPlayer->get_admiral_list()->get_by_id(Fleet->get_admiral_id());
            if(!Admiral)
            {
            ITEM("ERROR_MESSAGE", (char *)format(GETTEXT("Fleet %1$s's admiral does not exist."),
						Fleet->get_name()));
            return output("fleet/reassignment_error.html");
            }

            ITEM("STRING_SHIP_REASSIGNMENT", GETTEXT("Ship Reassignment"));

			ITEM("STRING_FLEET", GETTEXT("Fleet"));
			ITEM("STRING_EXP_", GETTEXT("Exp."));
			ITEM("STRING_CLASS", GETTEXT("Class"));
			ITEM("STRING_TOTAL_NUMBER_OF_FLEET_SHIP_S_",
					GETTEXT("Total Number of Fleet Ship(s)"));
			ITEM("STRING_FLEET_COMMANDER", GETTEXT("Fleet Commander"));
			ITEM("STRING_MAX__COMMAND", GETTEXT("Max. Command"));

			ITEM("FLEET", Fleet->get_name());
			ITEM("EXP_", Fleet->get_exp());
			ITEM("CLASS", Fleet->CShipDesign::get_name());
			ITEM("TOTAL_NUMBER_OF_FLEET_SHIP", Fleet->get_current_ship());
			ITEM("FLEET_COMMANDER_NAME", Admiral->get_name());
			ITEM("MAX_COMMAND", Admiral->get_fleet_commanding());

			ITEM("STRING_SHIPS_IN_THE_POOL", GETTEXT("Ship(s) in the Pool"));
			CDock *
				Pool = (CDock *)aPlayer->get_dock();
			/* start telecard 2001/01/22
			ITEM("POOL_LIST", Pool->radio_ship_reassignment_html());
			*/
			ITEM("POOL_LIST", Pool->radio_ship_reassignment_html( Fleet->get_ship_class() ));
			// end telecard
			if( Pool->length() == 0 )
			{
				ITEM("STRING_NUMBER_OF_SHIPS", " ");
				ITEM("MAX_COMMAND", " ");
				ITEM("REASSIGNMENT_FLEET", " ");
				ITEM("NEW_SIZE", " ");
				ITEM("SUBMIT", " ");
			}
			else if( Pool->length() == 1 )
			{
				CDockedShip *
					ship = (CDockedShip *)Pool->get(0);
				if( ship->get_design_id() == Fleet->get_ship_class() )
				{
					ITEM("STRING_NUMBER_OF_SHIPS", " ");
					ITEM("MAX_COMMAND", " ");
					ITEM("REASSIGNMENT_FLEET", " ");
					ITEM("NEW_SIZE", " ");
					ITEM("SUBMIT", " ");
				}
				else
				{
					ITEM("STRING_NUMBER_OF_SHIPS", GETTEXT("Number of Ship(s) to Form the Fleet"));
					ITEM("MAX_COMMAND",
							(char *)format(GETTEXT("(Max : %1$s)"),
											dec2unit(Admiral->get_fleet_commanding())));
					ITEM("REASSIGNMENT_FLEET", (char *)format("<INPUT TYPE=hidden NAME=FLEET VALUE=%d>\n", Fleet->get_id()));
					ITEM("NEW_SIZE", "<INPUT TYPE=\"text\" NAME=\"FLEET_NEW_SIZE\" SIZE=\"10\">");
					ITEM("SUBMIT",
							(char *)format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_reassignment.gif\" BORDER=0></TD>", (char *)CGame::mImageServerURL));
				}
			}
			else
			{
				ITEM("STRING_NUMBER_OF_SHIPS", GETTEXT("Number of Ship(s) to Form the Fleet"));
				ITEM("MAX_COMMAND",
						(char *)format(GETTEXT("(Max : %1$s)"),
										dec2unit(Admiral->get_fleet_commanding())));
				ITEM("REASSIGNMENT_FLEET", (char *)format("<INPUT TYPE=hidden NAME=FLEET VALUE=%d>\n", Fleet->get_id()));
				ITEM("NEW_SIZE", "<INPUT TYPE=\"text\" NAME=\"FLEET_NEW_SIZE\" SIZE=\"10\">");
				ITEM("SUBMIT",
						(char *)format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_reassignment.gif\" BORDER=0></TD>", (char *)CGame::mImageServerURL));
			}
			return output("fleet/reassignment_ship_reassignment.html");
		}

		case REASSIGNMENT_REMOVE_ADD_SHIPS :
		{
			static CString
            Message;
            Message.clear();

            CCommandSet FleetSet;
            FleetSet.clear();

            CFleetList *FleetList = aPlayer->get_fleet_list();
            for (int i=0 ; i<FleetList->length() ; i++) {
                CFleet *Fleet = (CFleet *)FleetList->get(i);
                if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

                CString QueryVar;
                QueryVar.clear();
                QueryVar.format("FLEET%d", i);

                QUERY(QueryVar, FleetIDString);

                if (FleetIDString) {
                    if (!strcasecmp(FleetIDString, "ON")) {
                        FleetSet += i;
                    }
                }
            }

            if(FleetSet.is_empty()) {
                ITEM("ERROR_MESSAGE", GETTEXT("You didn't select any fleets."));
                return output("fleet/reassignment_error.html");
            }

            // Use the last selected fleet if multiple are selected
            int j = 0;
            for (int i=0 ; i<FleetList->length() ; i++) {
                Fleet = (CFleet *)FleetList->get(i);

                if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;
                if (FleetSet.has(i) == true) {
                    j = i;
                    break;
                }
            }
            
            Fleet = (CFleet *)FleetList->get(j);
			
			CAdmiral *Admiral = aPlayer->get_admiral_list()->get_by_id(Fleet->get_admiral_id());
            if(!Admiral)
            {
            ITEM("ERROR_MESSAGE", (char *)format(GETTEXT("Fleet %1$s's admiral does not exist."),
						Fleet->get_name()));
            return output("fleet/reassignment_error.html");
            }

            ITEM("STRING_REMOVE_ADD_SHIPS",
					GETTEXT("Remove/Add Ship(s)"));

			ITEM("STRING_FLEET", GETTEXT("Fleet"));
			ITEM("STRING_EXP_", GETTEXT("Exp."));
			ITEM("STRING_FLEET_COMMANDER", GETTEXT("Fleet Commander"));
			ITEM("STRING_MAX____OF_SHIPS",
					GETTEXT("Max. # of Ship(s)"));

			ITEM("FLEET", Fleet->get_name());
			ITEM("EXP_", Fleet->get_exp());
			ITEM("FLEET_COMMANDER_NAME", Admiral->get_name());
			ITEM("MAX___OF_SHIPS", Admiral->get_fleet_commanding());

			ITEM("CLASS_NAME", Fleet->CShipDesign::get_name());
			ITEM("CLASS_INFO", Fleet->print_html(aPlayer));

			ITEM("STRING_CURRENT_NUMBER_OF_SHIPS_IN_THE_FLEET",
					GETTEXT("Current Number of Ship(s) in the Fleet"));
			ITEM("STRING_NUMBER_OF_SHIPS_AVAILABLE_IN_THE_POOL",
					GETTEXT("Number of Ship(s) Available in the Pool"));
			ITEM("STRING_NUMBER_OF_SHIPS_TO_FORM_THE_FLEET",
					GETTEXT("Number of Ship(s) to Form the Fleet"));

			ITEM("CURRENT_NUMBER_OF_SHIPS_IN_THE_FLEET",
					Fleet->get_current_ship());
			CDock *
				Pool = aPlayer->get_dock();
			ITEM("NUMBER_OF_SHIPS_AVAILABLE_IN_THE_POOL",
					Pool->count_ship(Fleet->get_design_id()));

			ITEM("MAX",
					(char *)format(GETTEXT("(Max : %1$s)"),
									dec2unit(Admiral->get_fleet_commanding())));

			ITEM("REASSIGNMENT_FLEET",
					(char *)format("<INPUT TYPE=hidden NAME=FLEET VALUE=%d>\n",
									Fleet->get_id()));

			return output("fleet/reassignment_removeadd_ship.html");
		}

        // Mass Refill
		case REASSIGNMENT_REFILL_FLEET :
		{
            static CString
            Message;
            Message.clear();

            CCommandSet FleetSet;
            FleetSet.clear();

            CFleetList *FleetList = aPlayer->get_fleet_list();
            for (int i=0 ; i<FleetList->length() ; i++) {
                CFleet *Fleet = (CFleet *)FleetList->get(i);
                if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

                CString QueryVar;
                QueryVar.clear();
                QueryVar.format("FLEET%d", i);

                QUERY(QueryVar, FleetIDString);

                if (FleetIDString) {
                    if (!strcasecmp(FleetIDString, "ON")) {
                        FleetSet += i;
                    }
                }
            }

            if(FleetSet.is_empty()) {
                ITEM("ERROR_MESSAGE", GETTEXT("You didn't select any fleets."));
                return output("fleet/reassignment_error.html");
            }

            ITEM("STRING_REFILL_FLEET", GETTEXT("Refill Fleet(s)"));

            CDock *Pool = aPlayer->get_dock();

            CString Result;
            Result.clear();
            CString ClassInfoURL;

            bool changedFleet = false;

            for (int i=0 ; i<FleetList->length() ; i++) {
                CFleet *Fleet = (CFleet *)FleetList->get(i);

                if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;
                if (FleetSet.has(i) == false) continue;

                int ShipNeed = Fleet->get_max_ship() - Fleet->get_current_ship();
                if(ShipNeed == 0) continue;

                int current_ship =  Fleet->get_current_ship();
                int available_ship = Pool->count_ship(Fleet->get_design_id());

                if (available_ship == 0) continue;

                if (ShipNeed > available_ship) {
                    ShipNeed = available_ship;
                }

                changedFleet = true;

                //Fleet->set_max_ship(current_ship + ShipNeed);
                Fleet->set_current_ship(current_ship + ShipNeed);
                aPlayer->get_dock()->change_ship((CShipDesign *)Fleet, -ShipNeed);

                ClassInfoURL.clear();
                ClassInfoURL.format("<A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\">%s</A>",
						    Fleet->get_design_id(),
						    Fleet->CShipDesign::get_name());

                aPlayer->get_fleet_list()->refresh_power();	//telecard 2001/06/14
                aPlayer->refresh_power();

                Message.clear();
                Message.format(GETTEXT("You have successfully changed %1$s as a fleet of %2$s %3$s.<BR>"),
						        Fleet->get_name(),
						        dec2unit(current_ship + ShipNeed),
						        (char *)ClassInfoURL);

                Result += Message;

                Fleet->type(QUERY_UPDATE);
                STORE_CENTER->store(*Fleet);

            }

            if (changedFleet) {
                ITEM("RESULT_MESSAGE", (char *)Result);
            } else {
                ITEM("RESULT_MESSAGE", GETTEXT("No fleets were modified by this process."));
            }

            return output("fleet/reassignment_refill_fleet_result.html");
		}

        // Mass Maximize
		case REASSIGNMENT_MAXIMIZE_FLEET :
		{
            static CString
            Message;
            Message.clear();

            CCommandSet FleetSet;
            FleetSet.clear();

            CFleetList *FleetList = aPlayer->get_fleet_list();
            for (int i=0 ; i<FleetList->length() ; i++) {
                CFleet *Fleet = (CFleet *)FleetList->get(i);
                if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

                CString QueryVar;
                QueryVar.clear();
                QueryVar.format("FLEET%d", i);

                QUERY(QueryVar, FleetIDString);

                if (FleetIDString) {
                    if (!strcasecmp(FleetIDString, "ON")) {
                        FleetSet += i;
                    }
                }
            }

            if(FleetSet.is_empty()) {
                ITEM("ERROR_MESSAGE", GETTEXT("You didn't select any fleets."));
                return output("fleet/reassignment_error.html");
            }

            ITEM("STRING_MAXIMIZE_FLEET", GETTEXT("Maximize Fleet(s)"));

            CDock *Pool = aPlayer->get_dock();

            CString Result;
            Result.clear();
            CString ClassInfoURL;

            bool changedFleet = false;

            for (int i=0 ; i<FleetList->length() ; i++) {
                CFleet *Fleet = (CFleet *)FleetList->get(i);

                if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;
                if (FleetSet.has(i) == false) continue;

                CAdmiral *Admiral = aPlayer->get_admiral_list()->get_by_id(Fleet->get_admiral_id());

                int ShipNeed = Admiral->get_fleet_commanding() - Fleet->get_current_ship();
                if(ShipNeed == 0) continue;

                int current_ship =  Fleet->get_current_ship();
                int available_ship = Pool->count_ship(Fleet->get_design_id());

                if (available_ship == 0) continue;

                if (ShipNeed > available_ship) {
                    ShipNeed = available_ship;
                }

                changedFleet = true;

                Fleet->set_max_ship(current_ship + ShipNeed);
                Fleet->set_current_ship(current_ship + ShipNeed);
                aPlayer->get_dock()->change_ship((CShipDesign *)Fleet, -ShipNeed);

                ClassInfoURL.clear();
                ClassInfoURL.format("<A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\">%s</A>",
						    Fleet->get_design_id(),
						    Fleet->CShipDesign::get_name());

                aPlayer->get_fleet_list()->refresh_power();	//telecard 2001/06/14
                aPlayer->refresh_power();

                Message.clear();
                Message.format(GETTEXT("You have successfully changed %1$s as a fleet of %2$s %3$s.<BR>"),
						        Fleet->get_name(),
						        dec2unit(current_ship + ShipNeed),
						        (char *)ClassInfoURL);

                Result += Message;

                Fleet->type(QUERY_UPDATE);
                STORE_CENTER->store(*Fleet);

            }

            if (changedFleet) {
                ITEM("RESULT_MESSAGE", (char *)Result);
            } else {
                ITEM("RESULT_MESSAGE", GETTEXT("No fleets were modified by this process."));
            }

            return output("fleet/reassignment_maximize_fleet_result.html");
		}
	}
	
	return output("fleet/reassignment_error.html");
}

#undef REASSIGNMENT_COMMANDER_CHANGE
#undef REASSIGNMENT_SHIP_REASSIGNMENT
#undef REASSIGNMENT_REMOVE_ADD_SHIPS
#undef REASSIGNMENT_REFILL_FLEET
#undef REASSIGNMENT_MAXIMIZE_FLEET

