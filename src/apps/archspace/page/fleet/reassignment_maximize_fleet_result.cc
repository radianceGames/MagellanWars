#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool CPageReassignmentMaximizeFleetResult::handler(CPlayer *aPlayer) {
//	system_log( "start page handler %s", get_name() );

    /*
    CString Message;
    Message.clear();

    CCommandSet FleetSet;
    FleetSet.clear();          */
    
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
    
    ITEM("STRING_MAXIMIZE_FLEET", GETTEXT("Maximize Fleet(s)"));

    /*
    QUERY("FLEET_SET", FleetSetString);
    if (!FleetSetString)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't select any fleets."));
		return output("fleet/reassignment_error.html");
	}

	FleetSet.set_string(FleetSetString);
	if (FleetSet.is_empty())
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't select any fleets."));
		return output("fleet/reassignment_error.html");
	}         */
	
    //CFleetList *FleetList = aPlayer->get_fleet_list();
    
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
