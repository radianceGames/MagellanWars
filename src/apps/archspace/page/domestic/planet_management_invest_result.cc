#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include <cstdlib>

bool
CPagePlanetManagementInvestResult::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	static CString
		Message;
	Message.clear();

	ITEM("STRING_INVESTMENT", GETTEXT("Investment"));

	QUERY("PLANET_ID", PlanetIDString);
	int
		PlanetID = as_atoi(PlanetIDString);
	CPlanet *
		Planet = aPlayer->get_planet(PlanetID);
	CHECK(!Planet,
			(char *)format(GETTEXT("You don't have a planet with ID %1$s."),
							dec2unit(PlanetID)));

	QUERY("INVEST", InvestString);
	CHECK(!InvestString,
			GETTEXT("You didn't enter proper value for investment."));

	int
		Invest = as_atoi(InvestString);
	if (Invest < 1)
	{
		ITEM("ERROR_MESSAGE", 
			GETTEXT("You didn't enter proper value for investment."));
		ITEM("PLANET_ID", Planet->get_id());

		return output("domestic/planet_management_invest_error.html");
	}

	CHECK(Invest > aPlayer->get_production(),
					GETTEXT("You don't have enough PP to invest."));

	CControlModel *
		ControlModel = aPlayer->get_control_model();

	int
		OldGrowth = ControlModel->get_growth();
	int
		OldFacility = ControlModel->get_facility_cost();

	Planet->change_investment(Invest);
	aPlayer->change_reserved_production(-Invest);
	refresh_product_point_item();
	Planet->build_control_model(aPlayer);

	Planet->type(QUERY_UPDATE);
	STORE_CENTER->store(*Planet);

	int
		NewGrowth = ControlModel->get_growth();
	int
		NewFacility = ControlModel->get_facility_cost();

	Message.format(GETTEXT("You have successfully invested %1$s PP in %2$s."),
					dec2unit(Invest), Planet->get_name());
	Message += "<BR>\n";

	if (NewGrowth - OldGrowth)
	{
		Message += "<BR>\n";
		Message.format("%s %s",
						GETTEXT("Growth CM"),
						integer_with_sign(NewGrowth - OldGrowth));
	}
	if (NewFacility - OldFacility)
	{
		Message += "<BR>\n";
		Message.format("%s %s",
						GETTEXT("Facility CM"),
						integer_with_sign(NewFacility - OldFacility));
		Message += "<BR>\n";
		Message += GETTEXT("Now you can build more buildings in a turn.");
	}

	Message += "<BR>\n";
	Message += GETTEXT("The Terraforming of this planet has been accelerated.");

	ITEM("RESULT_MESSAGE", (char *)Message);

	ITEM("PLANET_ID", Planet->get_id());

//	system_log("end page handler %s", get_name());

	return output("domestic/planet_management_invest_result.html");
}

